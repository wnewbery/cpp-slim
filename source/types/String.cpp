#include "types/String.hpp"
#include "types/Array.hpp"
#include "types/Enumerator.hpp"
#include "types/HtmlSafeString.hpp"
#include "types/Proc.hpp"
#include "Value.hpp"
#include "Function.hpp"
#include "FunctionHelpers.hpp"
#include "Operators.hpp"
#include <algorithm>
#include <sstream>
#include <set>

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
            //TODO: If range, if regex
            auto index = dynamic_cast<Number*>(args[0].get());
            if (index)
            {
                return do_slice((int)index->get_value(), 1);
            }
            auto match_str = std::dynamic_pointer_cast<String>(args[0]);
            if (match_str)
            {
                if (v.find(match_str->v) != std::string::npos) return match_str;
                else return NIL_VALUE;
            }
            throw ArgumentError(this, "slice");
        }
        else if (args.size() == 2)
        {
            //TODO: If regexp, capture
            auto start = (int)coerce<Number>(args[0])->get_value();
            auto length = (int)coerce<Number>(args[1])->get_value();
            return do_slice(start, length);
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

    std::shared_ptr<Boolean> String::ascii_only_q()
    {
        for (auto c : v) if (c < 0 || c >= 128) return FALSE_VALUE;
        return TRUE_VALUE;
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

    ObjectPtr String::each_line(const FunctionArgs & args)
    {
        Proc *proc;
        std::vector<std::string> lines;
        if (args.size() == 1)
        {
            proc = coerce<Proc>(args[0].get());
            lines = split_lines("\n");
        }
        else if (args.size() == 2)
        {
            std::string sep;
            unpack(args, &sep, &proc);
            lines = split_lines(sep);
        }
        else throw ArgumentError(this, "each_line");

        for (auto &i : lines) proc->call({ make_value(i) });

        return shared_from_this();
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

    ObjectPtr String::index(const FunctionArgs & args)
    {
        std::string pattern;
        int offset = 0;
        unpack<1>(args, &pattern, &offset);

        if (offset < 0) offset = ((int)v.size()) + offset;
        if (offset < 0 || offset >= (int)v.size()) return NIL_VALUE;
        auto p = v.find(pattern, (size_t)offset);
        if (p == std::string::npos) return NIL_VALUE;
        else return make_value((double)p);
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

    std::shared_ptr<Array> String::partition(String * sep)
    {
        return do_partition(false, sep);
    }

    std::shared_ptr<String> String::reverse()
    {
        std::string ret;
        for (auto i = v.rbegin(); i != v.rend(); ++i) ret += *i;
        return make_value(ret);
    }

    std::shared_ptr<Array> String::rpartition(String * sep)
    {
        return do_partition(true, sep);
    }

    ObjectPtr String::rindex(const FunctionArgs & args)
    {
        if (args.size() < 1 && args.size() > 2) throw ArgumentError(this, "index");
        auto pattern = coerce<String>(args[0]);
        int offset = (int)v.size();
        if (args.size() == 2)
        {
            offset = (int)coerce<Number>(args[1])->get_value();
            if (offset < 0) offset = ((int)v.size()) + offset;
        }
        if (offset < 0) return NIL_VALUE;
        if (offset >= (int)v.size()) offset = (int)v.size();
        auto p = v.rfind(pattern->v, (size_t)offset);
        if (p == std::string::npos) return NIL_VALUE;
        else return make_value((double)p);
    }

    std::shared_ptr<Number> String::size()
    {
        return make_value((double)v.size());
    }

    std::shared_ptr<Array> String::split(const FunctionArgs & args)
    {
        std::string pattern = " ";
        int limit = 0;
        unpack<0>(args, &pattern, &limit);

        if (limit == 1) return make_array({make_value(v)});
        if (pattern.empty())
        {
            std::vector<ObjectPtr> out;
            for (auto c : v) out.push_back(make_value(std::string(&c, 1)));
            return make_array(out);
        }

        std::vector<ObjectPtr> out;
        auto add = [&out, limit](size_t p, const std::string &str) -> void
        {
            if (limit != 0 || !str.empty() || p == 0) out.push_back(make_value(str));
        };
        size_t p = 0;
        while (true)
        {
            size_t p_end = pattern == " " ? v.find_first_of(WHITESPACE, p) : v.find(pattern, p);
            if (p_end == std::string::npos)
            {
                add(p, v.substr(p));
                break;
            }
            
            add(p, v.substr(p, p_end - p));
            p = p_end + pattern.size();

            if (limit > 0 && out.size() + 1 == (size_t)limit)
            {
                add(p, v.substr(p));
                break;
            }
        }

        return make_array(out);
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

    const MethodTable &String::method_table()const
    {
        static const MethodTable table(Object::method_table(),
        {
            { &String::to_f, "to_f" },
            { &String::to_f, "to_d" },
            { &String::to_i, "to_i" },
            { &String::html_safe, "html_safe" },
            { &String::ascii_only_q, "ascii_only?" },
            { &String::capitalize, "capitalize" },
            { &String::casecmp, "casecmp" },
            { &String::center, "center" },
            { &String::chomp, "chomp" },
            { &String::downcase, "downcase" },
            { &String::each_line, "each_line" },
            { &String::empty_q, "empty?" },
            { &String::end_with_q, "end_with?" },
            { &String::hex, "hex" },
            { &String::include_q, "include?" },
            { &String::index, "index" },
            { &String::lines, "lines" },
            { &String::ljust, "ljust" },
            { &String::lstrip, "lstrip" },
            { &String::ord, "ord" },
            { &String::partition, "partition" },
            { &String::reverse, "reverse" },
            { &String::rjust, "rjust" },
            { &String::rpartition, "rpartition" },
            { &String::rstrip, "rstrip" },
            { &String::rindex, "rindex" },
            { &String::size, "size" },
            { &String::size, "length" },
            { &String::el_ref, "slice" },
            { &String::split, "split" },
            { &String::start_with_q, "start_with?" },
            { &String::strip, "strip" },
            { &String::upcase, "upcase" }
        });
        return table;
    }

    std::shared_ptr<Array> String::do_partition(bool reverse, String * sep)
    {
        auto p = reverse ? v.rfind(sep->v) : v.find(sep->v);
        if (p == std::string::npos)
        {
            if (!reverse) return make_array({ shared_from_this(), make_value(""), make_value("") });
            else return make_array({ make_value(""), make_value(""), shared_from_this() });
        }
        else
        {
            auto before = v.substr(0, p);
            auto after = v.substr(p + sep->v.size());
            return make_array({ make_value(before), sep->shared_from_this(), make_value(after) });
        }
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
