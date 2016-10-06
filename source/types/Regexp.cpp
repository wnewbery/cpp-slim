#include "types/Regexp.hpp"
#include "types/Array.hpp"
#include "types/Boolean.hpp"
#include "types/String.hpp"
#include <sstream>

namespace slim
{
    std::string MatchData::to_string() const
    {
        return match[0];
    }
    bool MatchData::eq(const Object *rhs) const
    {
        auto rhs2 = coerce<MatchData>(rhs);
        return str == rhs2->str && regex->eq(rhs2->regex.get()) && match == rhs2->match;
    }
    size_t MatchData::hash() const
    {
        size_t h = 0;
        detail::hash_combine(h, str);
        detail::hash_combine(h, regex->hash());
        return h;
    }
    ObjectPtr MatchData::el_ref(const FunctionArgs &args)
    {
        if (args.size() == 1)
        {
            auto i = (int)coerce<Number>(args[0])->get_value();
            if (i < 0) i = i + (int)match.size();
            if (i < 0 || i >= (int)match.size()) return NIL_VALUE;
            else return sub_str(i);
        }
        else if (args.size() == 2)
        {
            auto start  = (int)coerce<Number>(args[0])->get_value();
            if (start < 0) start = start + (int)match.size();
            if (start < 0 || start >= (int)match.size()) return NIL_VALUE;

            auto length = (int)coerce<Number>(args[1])->get_value();
            if (length < 0) return NIL_VALUE;

            auto out = create_object<Array>();
            for (int i = start; i < start + length && i < (int)match.size(); ++i)
                out->push_back(sub_str(i));
            return out;
        }
        else throw ArgumentCountError(args.size(), 1, 2);
    }
    Ptr<Object> MatchData::begin(Number *n)
    {
        auto sub = get_sub(n);
        if (!sub.matched) return NIL_VALUE;
        else return make_value(sub.first - str.begin());
    }
    Ptr<Array> MatchData::captures()
    {
        auto out = create_object<Array>();
        for (size_t i = 1; i < match.size(); ++i)
        {
            auto &sub = match[i];
            out->push_back(sub_str((int)i));
        }
        return out;
    }
    Ptr<Object> MatchData::end(Number * n)
    {
        auto sub = get_sub(n);
        if (!sub.matched) return NIL_VALUE;
        else return make_value(sub.second - str.begin());
    }
    Ptr<Object> MatchData::offset(Number * n)
    {
        auto sub = get_sub(n);
        if (!sub.matched) return make_array({NIL_VALUE, NIL_VALUE});
        return make_array({
                make_value(sub.first - str.begin()),
                make_value(sub.second - str.begin())
            });
    }
    Ptr<String> MatchData::post_match()
    {
        return make_value(match.suffix());
    }
    Ptr<String> MatchData::pre_match()
    {
        return make_value(match.prefix());
    }
    Ptr<Number> MatchData::size()
    {
        return make_value(match.size());
    }
    Ptr<String> MatchData::string()
    {
        return make_value(str);
    }
    Ptr<Array> MatchData::to_a()
    {
        auto out = create_object<Array>();
        for (size_t i = 0; i < match.size(); ++i)
        {
            out->push_back(sub_str((int)i));
        }
        return out;
    }
    Ptr<Array> MatchData::values_at(const FunctionArgs &args)
    {
        auto out = create_object<Array>();
        for (auto &arg : args)
        {
            auto i = (int)coerce<Number>(arg)->get_value();
            if (i < 0) i = i + (int)match.size();
            if (i < 0 || i >= (int)match.size())
                out->push_back(NIL_VALUE);
            else out->push_back(sub_str(i));
        }
        return out;
    }
    const MethodTable & MatchData::method_table() const
    {
        static const auto table = MethodTable(Object::method_table(), {
            {&MatchData::begin, "begin"},
            {&MatchData::captures, "captures"},
            {&MatchData::end, "end"},
            {&MatchData::offset, "offset"},
            {&MatchData::post_match, "post_match"},
            {&MatchData::pre_match, "pre_match"},
            {&MatchData::regexp, "regexp"},
            {&MatchData::size, "size"},
            {&MatchData::string, "string"},
            {&MatchData::to_a, "to_a"},
            {&MatchData::values_at, "values_at"}
        });
        return table;
    }

    std::ssub_match MatchData::get_sub(Number *n)const
    {
        auto i = (int)n->get_value();
        if (i < 0 || i >= (int)match.size())
            throw IndexError("index " + std::to_string(i) + " out of matches");
        return match[i];
    }
    Ptr<Object> MatchData::sub_str(int n)const
    {
        auto &sub = match[n];
        if (sub.matched) return make_value(sub.str());
        else return NIL_VALUE;
    }

    Ptr<String> Regexp::escape(String *str)
    {
        static const char hex[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
        std::string ret;
        for (char c : str->get_value())
        {
            if (c < 128 && (c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && (c < '0' || c > '9'))
            {
                ret += '\\';
                ret += 'x';
                ret += hex[(((unsigned)c) >> 4)];
                ret += hex[(((unsigned)c) & 0x0F)];
            }
            else ret += c;
        }
        return make_value(ret);
    }
    Ptr<Regexp> Regexp::new_instance(const FunctionArgs &args)
    {
        if (args.size() == 1)
        {
            auto cp = dynamic_cast<Regexp*>(args[0].get());
            if (cp) return create_object<Regexp>(*cp);
        }
        std::string src;
        int opts = 0;
        unpack<1>(args, &src, &opts);
        return create_object<Regexp>(src, opts);
    }
    std::regex_constants::syntax_option_type Regexp::syntax_options(int opts)
    {
        auto flags = std::regex_constants::ECMAScript;
        if (opts & IGNORECASE) flags |= std::regex_constants::icase;
        if (opts & EXTENDED) throw ScriptError("Regexp::EXTENDED is not supported");
        if (opts & MULTILINE) throw ScriptError("Regex multiline mode not supported by std::regex");
        return flags;
    }
    Regexp::Regexp(const std::string &str, int opts)
        : src(str), opts(opts), regex(str, syntax_options(opts))
    {
    }
    std::string Regexp::to_string() const
    {
        std::stringstream ss;
        ss << "(?";
        //options
        ss << ':' << src << ')';
        return ss.str();
    }
    std::string Regexp::inspect() const
    {
        std::stringstream ss;
        ss << '/' << src << '/';
        //options
        return ss.str();
    }
    bool Regexp::eq(const Object *rhs) const
    {
        auto r2 = dynamic_cast<const Regexp*>(rhs);
        return r2 && src == r2->src && opts == r2->opts;
    }
    size_t Regexp::hash() const
    {
        size_t h = 0;
        detail::hash_combine(h, src);
        detail::hash_combine(h, opts);
        return h;
    }
    Ptr<MatchData> Regexp::do_match(const std::string &str, int pos)
    {
        if (pos < 0) pos = (int)str.size() + pos;
        if (pos < 0 || pos >(int)str.size()) return nullptr;

        Ptr<MatchData> results(new MatchData(
            std::static_pointer_cast<Regexp>(shared_from_this()),
            str));
        if (std::regex_search(
            results->str.cbegin() + pos, results->str.cend(),
            results->match, regex))
        {
            return results;
        }
        else return nullptr;
    }
    std::smatch Regexp::do_rmatch(const std::string &str, int pos)
    {
        if (pos < 0) pos = (int)str.size() + pos;
        if (pos < 0 || pos >(int)str.size()) return {};

        auto new_src = "^[\\s\\S]*(" + src + ")";
        std::regex regex(new_src, syntax_options(opts));
        std::smatch match;
        std::regex_search(str.cbegin(), str.cbegin() + pos, match, regex);
        return match;
    }
    Ptr<Object> Regexp::match(const FunctionArgs &args)
    {
        std::string str;
        int pos = 0;
        unpack<1>(args, &str, &pos);
        auto ret = do_match(str, pos);
        if (ret) return ret;
        else return NIL_VALUE;
    }
    Ptr<Boolean> Regexp::casefold_q()
    {
        return make_value((opts & IGNORECASE) != 0);
    }
    Ptr<Number> Regexp::options()
    {
        return make_value(opts);
    }
    Ptr<String> Regexp::source()
    {
        return make_value(src);
    }
    const MethodTable & Regexp::method_table() const
    {
        static const auto table = MethodTable(Object::method_table(), {
            {&Regexp::match, "match"},
            {&Regexp::casefold_q, "casefold?"},
            {&Regexp::options, "options"},
            {&Regexp::source, "source"}
        });
        return table;
    }

    RegexpType::RegexpType()
    {
        constants[symbol("IGNORECASE")] = make_value(Regexp::IGNORECASE);
        constants[symbol("EXTENDED")] = make_value(Regexp::EXTENDED);
        constants[symbol("MULTILINE")] = make_value(Regexp::MULTILINE);
    }
    const slim::MethodTable &RegexpType::method_table()const
    {
        static const slim::MethodTable table(slim::Object::method_table(),
        {
            {&Regexp::new_instance, "new"},
            {&Regexp::new_instance, "compile"},
            {&Regexp::escape, "escape"}
        });
        return table;
    }
}
