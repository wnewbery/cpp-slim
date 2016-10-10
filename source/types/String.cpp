#include "types/String.hpp"
#include "types/Array.hpp"
#include "types/Enumerator.hpp"
#include "types/HtmlSafeString.hpp"
#include "types/Proc.hpp"
#include "types/Range.hpp"
#include "types/Regexp.hpp"
#include "Value.hpp"
#include "Function.hpp"
#include "FunctionHelpers.hpp"
#include "Operators.hpp"
#include "Unicode.hpp"
#include <algorithm>
#include <sstream>
#include <unordered_set>

namespace slim
{
    namespace
    {
        const std::string WHITESPACE = " \t\r\n";
        bool ends_with(const std::string &str, const std::string &with)
        {
            return str.size() >= with.size() && str.compare(str.size() - with.size(), with.size(), with) == 0;
        }
    }

    std::string String::inspect()const
    {
        std::string out = "\"";
        for (auto c : v)
        {
            switch (c)
            {
            case '\\': out += "\\\\"; break;
            case '\'': out += "\\\'"; break;
            case '\"': out += "\\\""; break;
            case '\r': out += "\\r"; break;
            case '\n': out += "\\n"; break;
            case '\t': out += "\\t"; break;
            default: out.push_back(c); break;
            }
        }
        out += "\"";
        return out;
    }

    std::shared_ptr<Number> String::to_f()
    {
        double d = 0;
        try { d = std::stod(v.c_str()); }
        catch (const std::exception &) {}
        return make_value(d);
    }
    std::shared_ptr<Number> String::to_i()
    {
        int i = 0;
        try { i = std::stoi(v.c_str()); }
        catch (const std::exception &) {}
        return make_value((double)i);
    }
    std::shared_ptr<Symbol> String::to_sym()
    {
        return symbol(v);
    }

    std::shared_ptr<String> String::html_safe()
    {
        return std::make_shared<HtmlSafeString>(v);
    }

    ObjectPtr String::el_ref(const FunctionArgs & args)
    {
        if (args.size() == 1)
        {
            if (auto index = std::dynamic_pointer_cast<Number>(args[0]))
            {
                return do_slice((int)index->get_value(), 1);
            }
            else if (auto regex = std::dynamic_pointer_cast<Regexp>(args[0]))
            {
                auto match = regex->do_match(v, 0);
                if (match) return match->to_string_obj();
                else return NIL_VALUE;
            }
            else if (auto match_str = std::dynamic_pointer_cast<String>(args[0]))
            {
                if (v.find(match_str->v) != std::string::npos) return match_str;
                else return NIL_VALUE;
            }
            else if (auto range = std::dynamic_pointer_cast<Range>(args[0]))
            {
                int start, length;
                if (range->get_beg_len(&start, &length, (int)v.size()))
                    return make_value(v.substr((size_t)start, (size_t)length));
                else return NIL_VALUE;
            }
            else throw ArgumentError(this, "slice");
        }
        else if (args.size() == 2)
        {
            if (auto regex = std::dynamic_pointer_cast<Regexp>(args[0]))
            {
                auto match = regex->do_match(v, 0);
                if (match) return match->el_ref({args[1]});
                else return NIL_VALUE;
            }
            else
            {
                auto start = (int)coerce<Number>(args[0])->get_value();
                auto length = (int)coerce<Number>(args[1])->get_value();
                return do_slice(start, length);
            }
        }
        else throw ArgumentError(this, "slice");
    }
    ObjectPtr String::do_slice(int start, int length)
    {
        if (length < 0) return NIL_VALUE;
        if (start < 0) start = ((int)v.size()) + start;
        if (start == (int)v.size()) return make_value("");
        if (start < 0 || start >(int)v.size()) return NIL_VALUE;
        return make_value(v.substr((size_t)start, (size_t)length));
    }

    //Encoding/unicode
    std::shared_ptr<Boolean> String::ascii_only_q()
    {
        for (auto c : v) if (c < 0 || c >= 128) return FALSE_VALUE;
        return TRUE_VALUE;
    }
    Ptr<Array> String::bytes()
    {
        std::vector<ObjectPtr> vec;
        vec.reserve(v.size());
        for (auto &c : v)
            vec.emplace_back(make_value((unsigned char)c));
        return make_value(std::move(vec));
    }
    Ptr<Object> String::byteslice(const FunctionArgs &args)
    {
        Range *range;
        if (args.size() == 1 && (range = dynamic_cast<Range*>(args[0].get())))
        {
            int start, length;
            if (range->get_beg_len(&start, &length, (int)v.size()))
                return make_value(v.substr((size_t)start, (size_t)length));
            else return NIL_VALUE;
        }
        else
        {
            int offset, len = 1;
            unpack<1>(args, &offset, &len);
            if (offset < 0) offset = (int)v.size() + offset;
            if (offset < 0 || offset >= (int)v.size() || len < 0) return NIL_VALUE;
            return make_value(v.substr((size_t)offset, (size_t)len));
        }
    }
    Ptr<Array> String::chars()
    {
        std::vector<ObjectPtr> vec;
        vec.reserve(v.size());
        uint32_t cp;
        unsigned cp_len;
        for (size_t p = 0; p < v.size(); p += cp_len)
        {
            utf8_decode(v.c_str() + p, &cp, &cp_len);
            vec.push_back(make_value(v.substr(p, cp_len)));
        }
        return make_value(std::move(vec));
    }
    Ptr<String> String::chop()
    {
        for (int p = (int)v.size() - 1; p > 0; --p)
        {
            if (utf8_is_leading(v[p]))
            {
                return make_value(v.substr(0, (size_t)p));
            }
        }
        return make_value(std::string());
    }
    Ptr<String> String::chr()
    {
        if (v.empty()) return make_value(std::string());
        else return make_value(v.substr(0, utf8_next_len(v[0])));
    }
    Ptr<Array> String::codepoints()
    {
        std::vector<ObjectPtr> vec;
        vec.reserve(v.size());
        uint32_t cp;
        unsigned cp_len;
        for (size_t p = 0; p < v.size(); p += cp_len)
        {
            utf8_decode(v.c_str() + p, &cp, &cp_len);
            vec.push_back(make_value(cp));
        }
        return make_value(std::move(vec));
    }
    Ptr<Object> String::getbyte(Number *index)
    {
        auto n = (int)index->get_value();
        if (n < 0) n = (int)v.size() + n;
        if (n < 0 || n >= (int)v.size()) return NIL_VALUE;
        else return make_value((unsigned char)v[n]);
    }
    Ptr<String> String::scrub(const FunctionArgs &args)
    {
        std::string replacement = "\xEF\xBF\xBD"; //REPLACEMENT CHARACTER
        unpack<0>(args, &replacement);
        std::string out;
        for (size_t i = 0; i < v.size();)
        {
            uint32_t cp;
            unsigned len;
            if (try_utf8_decode(v.c_str() + i, &cp, &len))
            {
                out.append(v.c_str() + i, len);
                i += len;
            }
            else
            {
                out += replacement;
                ++i;
            }
        }
        return make_value(out);
    }

    std::shared_ptr<String> String::capitalize()
    {
        std::string ret = v;
        if (!ret.empty() && ret[0] >= 'a' && ret[0] <= 'z') ret[0] = (char)(ret[0] - 'a' + 'A');
        return make_value(ret);
    }

    ObjectPtr String::casecmp(String * rhs)
    {
        auto i1 = v.begin(), i2 = rhs->v.begin();
        for (; i1 != v.end() && i2 != rhs->v.end(); ++i1, ++i2)
        {
            auto c1 = ::tolower(*i1), c2 = ::tolower(*i2);
            if (c1 < c2) return make_value(-1.0);
            if (c1 > c2) return make_value(+1.0);
        }
        if (v.size() < rhs->v.size()) return make_value(-1.0);
        if (v.size() > rhs->v.size()) return make_value(+1.0);
        return make_value(0.0);
    }

    std::shared_ptr<String> String::center(const FunctionArgs & args)
    {
        int width;
        std::string padstr = " ";
        unpack<1>(args, &width, &padstr);

        if (padstr.empty()) throw ArgumentError(this, "ljust");
        if (width <= (int)v.size()) return make_value(v);
        
        auto left = (width - v.size()) / 2;
        auto right = left + (width - v.size()) % 2;
        std::string new_str;
        for (size_t i = 0; i < left; ++i) new_str += padstr[i % padstr.size()];
        new_str += v;
        for (size_t i = 0; i < right; ++i) new_str += padstr[i % padstr.size()];
        return make_value(new_str);
    }

    std::shared_ptr<String> String::ljust(const FunctionArgs & args)
    {
        int width;
        std::string padstr = " ";
        unpack<1>(args, &width, &padstr);

        if (padstr.empty()) throw ArgumentError(this, "ljust");
        if (width <= (int)v.size()) return make_value(v);

        std::string ret = v;
        for (int i = 0; i < width - (int)v.size(); ++i) ret += padstr[i % padstr.size()];
        return make_value(ret);
    }
    std::shared_ptr<String> String::rjust(const FunctionArgs & args)
    {
        int width;
        std::string padstr = " ";
        unpack<1>(args, &width, &padstr);

        if (padstr.empty()) throw ArgumentError(this, "rjust");
        if (width <= (int)v.size()) return make_value(v);

        std::string ret;
        for (int i = 0; i < width - (int)v.size(); ++i) ret += padstr[i % padstr.size()];
        ret += v;
        return make_value(ret);
    }

    std::shared_ptr<String> String::chomp(const FunctionArgs & args)
    {
        if (args.empty())
        {
            if (v.size() >= 2 && v[v.size() - 2] == '\r' && v[v.size() - 1] == '\n')
            {
                return make_value(std::string(v.begin(), v.end() - 2));
            }
            else if (v.size() >= 1 && (v.back() == '\r' || v.back() == '\n'))
            {
                return make_value(std::string(v.begin(), v.end() - 1));
            }
            else return coerce<String>(shared_from_this());
        }
        else if (args.size() == 1)
        {
            auto &sep = coerce<String>(args[0])->v;
            if (sep.empty())
            {
                auto end = v.find_last_not_of("\r\n");
                if (end == std::string::npos) return make_value("");
                else return make_value(std::string(v.c_str(), end + 1));
            }
            else if (ends_with(v, sep))
            {
                return make_value(std::string(v.c_str(), v.size() - sep.size()));
            }
            else return coerce<String>(shared_from_this());
        }
        else throw ArgumentError(this, "chomp");
    }

    std::shared_ptr<String> String::downcase()
    {
        auto ret = v;
        std::transform(ret.begin(), ret.end(), ret.begin(), ::tolower);
        return make_value(ret);
    }

    Ptr<Object> String::each_byte(const FunctionArgs &args)
    {
        Proc *proc = nullptr;
        unpack<0>(args, &proc);
        if (proc)
        {
            try
            {
                for (auto &i : v)
                    proc->call({ make_value((unsigned char)i) });
                return shared_from_this();
            }
            catch (const BreakException &e)
            {
                return e.value;
            }
        }
        else return make_enumerator(this, { &String::each_byte, "each_byte" });
    }
    Ptr<Object> String::each_char(const FunctionArgs &args)
    {
        Proc *proc = nullptr;
        unpack<0>(args, &proc);
        if (proc)
        {
            try
            {
                uint32_t cp;
                unsigned cp_len;
                for (size_t p = 0; p < v.size(); p += cp_len)
                {
                    utf8_decode(v.c_str() + p, &cp, &cp_len);
                    proc->call({ make_value(v.substr(p, cp_len)) });
                }
                return shared_from_this();
            }
            catch (const BreakException &e)
            {
                return e.value;
            }
        }
        else return make_enumerator(this, { &String::each_char, "each_char" });
    }
    Ptr<Object> String::each_codepoint(const FunctionArgs &args)
    {
        Proc *proc = nullptr;
        unpack<0>(args, &proc);
        if (proc)
        {
            try
            {
                uint32_t cp;
                unsigned cp_len;
                for (size_t p = 0; p < v.size(); p += cp_len)
                {
                    utf8_decode(v.c_str() + p, &cp, &cp_len);
                    proc->call({ make_value(cp) });
                }
                return shared_from_this();
            }
            catch (const BreakException &e)
            {
                return e.value;
            }
        }
        else return make_enumerator(this, { &String::each_codepoint, "each_codepoint" });
    }
    ObjectPtr String::each_line(const FunctionArgs & args)
    {
        Proc *proc = nullptr;
        std::string sep = "\n";

        if (args.size() == 2) unpack(args, &sep, &proc);
        else if (args.size() == 1)
        {
            proc = dynamic_cast<Proc*>(args[0].get());
            if (!proc) sep = coerce<String>(args[0])->get_value();
        }
        else if (args.size() > 0) throw ArgumentCountError(args.size(), 0, 2);

        if (proc)
        {
            auto lines = split_lines(sep);
            try
            {
                for (auto &i : lines) proc->call({ make_value(i) });
                return shared_from_this();
            }
            catch(const BreakException &e)
            {
                return e.value;
            }
        }
        else
        {
            return make_enumerator(this, { &String::each_line, "each_line" }, args);
        }
    }

    std::shared_ptr<Boolean> String::empty_q()
    {
        return make_value(v.empty());
    }

    std::shared_ptr<Boolean> String::end_with_q(const FunctionArgs & args)
    {
        for (auto &suffix : args)
        {
            auto &str = coerce<String>(suffix)->v;
            if (ends_with(v, str)) return TRUE_VALUE;
        }
        return FALSE_VALUE;
    }

    std::shared_ptr<Number> String::hex()
    {
        size_t p = 0;
        //optional sign
        bool neg = false;
        if (!v.empty() && v[0] == '+') p = 1;
        if (!v.empty() && v[0] == '-') { p = 1; neg = true; }
        
        //optional 0x
        if (v.size() >= p + 2 && v[p + 0] == '0' && v[p + 1] == 'x') p += 2;

        //digits
        double n = 0;
        for (; p < v.size(); ++p)
        {
            auto c = v[p];
            if (c >= '0' && c <= '9') n = n * 16 + c - '0';
            else if (c >= 'a' && c <= 'f') n = n * 16 + c - 'a' + 10;
            else if (c >= 'A' && c <= 'F') n = n * 16 + c - 'A' + 10;
            else break;
        }

        if (neg) n = -n;
        return make_value(n);
    }

    std::shared_ptr<Boolean> String::include_q(const String * rhs)
    {
        return make_value(v.find(rhs->v) != std::string::npos);
    }

    ObjectPtr String::index(const FunctionArgs &args)
    {
        Object *pattern;
        int offset = 0;
        unpack<1>(args, &pattern, &offset);

        if (offset < 0) offset = ((int)v.size()) + offset;
        if (offset < 0 || offset > (int)v.size()) return NIL_VALUE;
        if (auto regex = dynamic_cast<Regexp*>(pattern))
        {
            auto match = regex->do_match(v, offset);
            if (match) return match->begin(make_value(0).get());
            else return NIL_VALUE;
        }
        else if (auto substring = dynamic_cast<String*>(pattern))
        {
            auto p = v.find(substring->get_value(), (size_t)offset);
            if (p != std::string::npos) return make_value((double)p);
            else return NIL_VALUE;
        }
        else throw ArgumentError("Expected String or Regexp");
    }

    std::shared_ptr<Array> String::lines(const FunctionArgs & args)
    {
        std::string sep = "\n";
        unpack<0>(args, &sep);

        auto out = split_lines(sep);
        std::vector<ObjectPtr> ret;
        for (auto &s : out) ret.push_back(make_value(s));
        return make_array(ret);
    }

    std::shared_ptr<Number> String::ord()
    {
        if (v.empty()) throw ArgumentError(this, "ord");
        else return make_value((double)v[0]);
    }

    std::shared_ptr<Array> String::partition(Object *obj)
    {
        return do_partition(false, obj);
    }

    std::shared_ptr<String> String::reverse()
    {
        std::string ret;
        for (auto i = v.rbegin(); i != v.rend(); ++i) ret += *i;
        return make_value(ret);
    }

    std::shared_ptr<Array> String::rpartition(Object *obj)
    {
        return do_partition(true, obj);
    }

    ObjectPtr String::rindex(const FunctionArgs &args)
    {
        Object *pattern;
        int offset = (int)v.size();
        unpack<1>(args, &pattern, &offset);

        if (offset < 0) offset = ((int)v.size()) + offset;
        if (offset < 0) return NIL_VALUE;
        if (auto regex = dynamic_cast<Regexp*>(pattern))
        {
            if (offset >= (int)v.size()) offset = (int)v.size() - 1;
            auto match = regex->do_rmatch(v, offset + 1);
            if (match.size()) return make_value(match[1].first - v.begin());
            else return NIL_VALUE;
        }
        else if (auto substring = dynamic_cast<String*>(pattern))
        {
            auto p = v.rfind(substring->get_value(), (size_t)offset);
            if (p != std::string::npos) return make_value((double)p);
            else return NIL_VALUE;
        }
        else throw ArgumentError("Expected String or Regexp");
    }

    std::shared_ptr<Number> String::size()
    {
        return make_value((double)v.size());
    }

    std::shared_ptr<Array> String::split(const FunctionArgs &args)
    {
        //TODO: Suppress trailing nulls
        //TODO: Negative limit trailing null suppression
        //TODO: Single space
        Ptr<Object> pattern = make_value(" ");
        int limit = 0;
        bool suppress_nulls;
        unpack<0>(args, &pattern, &limit);
        if (limit < 0)
        {
            limit = 0;
            suppress_nulls = false;
        }
        else suppress_nulls = true;

        std::vector<std::string> out;

        if (auto str_obj = dynamic_cast<String*>(pattern.get()))
        {
            auto &str = str_obj->get_value();
            if (str.empty())
            {
                for (size_t p = 0; p < v.size(); ++p)
                {
                    if (limit > 0 && limit == (int)out.size() + 1)
                    {
                        //hit limit, add the rest and finish
                        out.push_back(v.substr(p));
                        break;
                    }
                    else out.push_back(v.substr(p, 1));
                }
            }
            else if (str == " ")
            {
                static const auto WS = " \t\n\r";
                auto p = v.find_first_not_of(WS);
                while (p < v.size())
                {
                    if (limit > 0 && limit == (int)out.size() + 1)
                    {
                        //hit limit, add the rest and finish
                        out.push_back(v.substr(p));
                        break;
                    }
                    auto next = v.find_first_of(WS, p);
                    if (next != std::string::npos)
                    {
                        out.push_back(v.substr(p, next - p));
                        p = v.find_first_not_of(WS, next);
                    }
                    else //not found, finish
                    {
                        out.push_back(v.substr(p));
                        break;
                    }
                }
            }
            else
            {
                size_t p = 0;
                while (limit == 0 || limit > (int)out.size() + 1)
                {
                    auto next = v.find(str, p);
                    if (next == std::string::npos) break;
                    out.push_back(v.substr(p, next - p));
                    p = next + str.size();
                }
                out.push_back(v.substr(p));
            }
        }
        else if (auto regex_obj = dynamic_cast<Regexp*>(pattern.get()))
        {
            auto &regex = regex_obj->get();
            auto i = v.cbegin(), end = v.cend();
            while ((limit == 0 || limit > (int)out.size() + 1) && i != end)
            {
                std::smatch match;
                std::regex_search(i, end, match, regex);
                if (match.empty()) break;
                if (match[0].first == match[0].second)
                {
                    out.emplace_back(i, match[0].first + 1);
                    i = match[0].first + 1;
                }
                else
                {
                    out.emplace_back(i, match[0].first);
                    i = match[0].second;
                }
                //captures
                for (size_t j = 1; j < match.size(); ++j)
                    out.push_back(match.str(j));
            }
            out.emplace_back(i, end);
        }
        else throw ArgumentError("Expected String or Regexp");

        if (suppress_nulls)
        {
            while (out.size() && out.back().empty())
                out.pop_back();
        }

        auto arr = create_object<Array>();
        for (auto &&str : out)
            arr->push_back(make_value(std::move(str)));
        return arr;
    }

    std::shared_ptr<Boolean> String::start_with_q(const FunctionArgs & args)
    {
        for (auto &suffix : args)
        {
            auto &str = coerce<String>(suffix)->v;
            if (v.find(str) == 0) return TRUE_VALUE;
        }
        return FALSE_VALUE;
    }

    std::shared_ptr<String> String::strip()
    {
        auto start = v.find_first_not_of(WHITESPACE);
        if (start == std::string::npos) return make_value("");
        auto end = v.find_last_not_of(WHITESPACE);
        return make_value(v.substr(start, end - start + 1));
    }
    std::shared_ptr<String> String::lstrip()
    {
        auto p = v.find_first_not_of(WHITESPACE);
        if (p == std::string::npos) return make_value("");
        else return make_value(v.substr(p));
    }
    Ptr<Object> String::match(const FunctionArgs &args)
    {
        std::string str;
        Ptr<Regexp> regex;
        Ptr<Number> pos = nullptr;
        if (try_unpack<1>(args, &str, &pos))
            regex = create_object<Regexp>(str);
        else unpack<1>(args, &regex, &pos);
        
        if (pos) return regex->match({shared_from_this(), pos});
        else return regex->match({shared_from_this()});
    }
    std::shared_ptr<String> String::rstrip()
    {
        auto p = v.find_last_not_of(WHITESPACE);
        if (p == std::string::npos) return make_value("");
        else return make_value(v.substr(0, p + 1));
    }

    std::shared_ptr<String> String::upcase()
    {
        auto ret = v;
        std::transform(ret.begin(), ret.end(), ret.begin(), ::toupper);
        return make_value(ret);
    }

    namespace
    {
        typedef std::function<std::string(const std::string &, const std::smatch *)> ReplaceFunc;
        ReplaceFunc replace_func(Object *replace)
        {
            if (auto str_obj = dynamic_cast<String*>(replace))
            {
                auto &str = str_obj->get_value();
                struct Part
                {
                    int sub; //-1 for literal string
                    std::string str;
                };
                std::vector<Part> parts;
                size_t i = 0, last = 0;
                while (true)
                {
                    auto next = str.find('\\', i);
                    if (next >= str.size() - 1) break;
                    auto c = str[next + 1];
                    if (c < '0' || c > '9')
                    {
                        i = next + 2;
                        continue;
                    }

                    if (next > i) parts.push_back({-1, str.substr(i, next - i)});
                    parts.push_back({c - '0', {}});
                    last = i = next + 2;
                }
                if (last != std::string::npos)
                    parts.push_back({-1, str.substr(last)});

                return [parts](const std::string &str, const std::smatch *match) -> std::string
                {
                    std::string out;
                    for (auto &part : parts)
                    {
                        if (part.sub < 0) out += part.str;
                        else if (part.sub == 0) out += str;
                        else if (match && (size_t)part.sub < match->size())
                            out += match->str((size_t)part.sub);
                    }
                    return out;
                };
            }
            else if (auto hash = dynamic_cast<Hash*>(replace))
            {
                return [hash](const std::string &str, const std::smatch *) -> std::string
                {
                    return hash->get(make_value(str))->to_string();
                };
            }
            else if (auto proc = dynamic_cast<Proc*>(replace))
            {
                return [proc](const std::string &str, const std::smatch *) -> std::string
                {
                    return coerce<String>(proc->call({make_value(str)}))->get_value();
                };
            }
            else throw ArgumentError("Invalid replacement type");
        }
    }
    Ptr<String> String::gsub(const FunctionArgs &args)
    {
        return do_sub(args, true);
    }
    Ptr<String> String::substitute(const FunctionArgs &args)
    {
        return do_sub(args, false);
    }
    Ptr<String> String::do_sub(const FunctionArgs &args, bool global)
    {
        Object *pattern, *replace;
        unpack(args, &pattern, &replace);
        auto f = replace_func(replace);
        if (auto regex = dynamic_cast<Regexp*>(pattern))
        {
            auto i = v.cbegin(), end = v.cend();
            std::string out;
            do
            {
                std::smatch match;
                if (!std::regex_search(i, end, match, regex->get())) break;
                out.append(i, match[0].first);
                out += f(match.str(0), &match);
                i = match[0].second;
            }
            while (global);
            out.append(i, end);
            return make_value(out);
        }
        else if (auto str_obj = dynamic_cast<String*>(pattern))
        {
            auto &str = str_obj->get_value();
            size_t i = 0;
            std::string out;
            do
            {
                auto next = v.find(str, i);
                if (next == std::string::npos) break;
                out += v.substr(i, next - i);
                out += f(str, nullptr);
                i = next + str.size();
            }
            while (global);
            out += v.substr(i);
            return make_value(out);
        }
        else throw ArgumentError("Expected String or Regexp");
    }

    const MethodTable &String::method_table()const
    {
        static const MethodTable table(Object::method_table(),
        {
            { &String::to_f, "to_f" },
            { &String::to_f, "to_d" },
            { &String::to_i, "to_i" },
            { &String::html_safe, "html_safe" },

            { &String::ascii_only_q, "ascii_only?" },
            { &String::bytes, "bytes" },
            { &String::byteslice, "byteslice" },
            { &String::chars, "chars" },
            { &String::chop, "chop" },
            { &String::chr, "chr" },
            { &String::codepoints, "codepoints" },
            { &String::getbyte, "getbyte" },
            { &String::scrub, "scrub" },

            { &String::capitalize, "capitalize" },
            { &String::casecmp, "casecmp" },
            { &String::center, "center" },
            { &String::chomp, "chomp" },
            { &String::downcase, "downcase" },
            { &String::each_byte, "each_byte" },
            { &String::each_char, "each_char" },
            { &String::each_codepoint, "each_codepoint" },
            { &String::each_line, "each_line" },
            { &String::empty_q, "empty?" },
            { &String::end_with_q, "end_with?" },
            { &String::gsub, "gsub" },
            { &String::hex, "hex" },
            { &String::include_q, "include?" },
            { &String::index, "index" },
            { &String::lines, "lines" },
            { &String::ljust, "ljust" },
            { &String::lstrip, "lstrip" },
            { &String::match, "match" },
            { &String::ord, "ord" },
            { &String::partition, "partition" },
            { &String::reverse, "reverse" },
            { &String::rjust, "rjust" },
            { &String::rpartition, "rpartition" },
            { &String::rstrip, "rstrip" },
            { &String::rindex, "rindex" },
            { &String::size, "size" },{ &String::size, "length" },{ &String::size, "bytesize" },
            { &String::el_ref, "slice" },
            { &String::split, "split" },
            { &String::start_with_q, "start_with?" },
            { &String::strip, "strip" },
            { &String::substitute, "sub" },
            { &String::upcase, "upcase" }
        });
        return table;
    }

    std::shared_ptr<Array> String::do_partition(bool reverse, Object *sep)
    {
        if (auto str = dynamic_cast<String*>(sep))
        {
            auto p = reverse ? v.rfind(str->v) : v.find(str->v);
            if (p != std::string::npos)
            {
                auto before = v.substr(0, p);
                auto after = v.substr(p + str->v.size());
                return make_array({make_value(before), str->shared_from_this(), make_value(after)});
            }
        }
        else
        {
            auto regex = coerce<Regexp>(sep);
            if (reverse)
            {
                auto match = regex->do_rmatch(v);
                if (match.size() > 1)
                {
                    auto begin = match[1].first;
                    auto end = match[1].second;
                    return make_array({
                        make_value(std::string(v.cbegin(), begin)),
                        make_value(std::string(begin, end)),
                        make_value(std::string(end, v.cend()))
                    });
                }
            }
            else
            {
                auto match = regex->do_match(v, 0);
                if (match)
                    return make_array({match->pre_match(), match->to_string_obj(), match->post_match()});
            }
        }
        //no match
        if (!reverse) return make_array({ shared_from_this(), make_value(""), make_value("") });
        else return make_array({ make_value(""), make_value(""), shared_from_this() });
    }

    std::vector<std::string> String::split_lines() const
    {
        auto find = [this](size_t offset) -> size_t
        {
            while (true)
            {
                auto p = v.find("\n\n", offset);
                if (p == std::string::npos) return std::string::npos;
                auto p2 = v.find_first_not_of('\n', p + 2);
                if (p2 == std::string::npos) return std::string::npos;
                return p2;
            }
        };
        std::vector<std::string> out;
        size_t p = 0;
        while (p < v.size())
        {
            auto p2 = find(p);
            if (p2 == std::string::npos)
            {
                out.push_back(v.substr(p));
                break;
            }

            out.push_back(v.substr(p, p2 - p));
            p = p2;
        }
        return out;
    }

    std::vector<std::string> String::split_lines(const std::string &sep) const
    {
        if (sep.empty()) return split_lines();

        std::vector<std::string> out;
        size_t p = 0;
        while (p < v.size())
        {
            auto p_end = v.find(sep, p);
            if (p_end == std::string::npos)
            {
                out.push_back(v.substr(p));
                break;
            }

            out.push_back(v.substr(p, p_end - p + sep.size()));
            p = p_end + sep.size();
        }
        return out;
    }
}
