#include "types/Array.hpp"
#include "types/Enumerator.hpp"
#include "types/Proc.hpp"
#include "Value.hpp"
#include "Function.hpp"
#include "Operators.hpp"
#include <algorithm>
#include <sstream>
#include <set>

namespace slim
{
    const std::string Array::TYPE_NAME = "Array";

    std::string Array::inspect() const
    {
        std::stringstream ss;
        ss << '[';
        bool first = true;
        for (auto &i : arr)
        {
            if (first) first = false;
            else ss << ", ";
            ss << i->inspect();
        }
        ss << ']';
        return ss.str();
    }
    size_t Array::hash()const
    {
        size_t h = 0;
        for (auto &i : arr) detail::hash_combine(h, *i);
        return h;
    }
    int Array::cmp(const Object * orhs) const
    {
        auto &rhs = ((const Array*)orhs)->get_value();
        size_t i, j;
        for (i = j = 0; i < arr.size() && j < rhs.size(); ++i, ++j)
        {
            int c = slim::cmp(arr[i].get(), rhs[j].get());
            if (c != 0) return c;
        }
        if (arr.size() < rhs.size()) return -1;
        else if (arr.size() > rhs.size()) return 1;
        else return 0;
    }
    ObjectPtr Array::add(Object *rhs)
    {
        auto rhs_arr = coerce<Array>(rhs);
        std::vector<ObjectPtr> out;
        out.reserve(arr.size() + rhs_arr->arr.size());
        out = arr;
        out.insert(out.end(), rhs_arr->arr.begin(), rhs_arr->arr.end());
        return make_value(std::move(out));
    }
    ObjectPtr Array::sub(Object *rhs)
    {
        auto rhs_arr = coerce<Array>(rhs);
        std::vector<ObjectPtr> out; 
        for (auto &i : arr)
        {
            if (!rhs_arr->include_q_imp(i.get()))
                out.push_back(i);
        }
        return make_value(std::move(out));
    }

    std::shared_ptr<Object> Array::assoc(const Object * a)
    {
        for (auto &i : arr)
        {
            auto arr2 = dynamic_cast<Array*>(i.get());
            if (arr2 && !arr2->arr.empty() && slim::eq(a, arr2->get_value().front().get()))
            {
                return arr2->shared_from_this();
            }
        }
        return NIL_VALUE;
    }
    std::shared_ptr<Object> Array::at(const Number * n)
    {
        int i = (int)n->get_value();
        if (i < 0) i = ((int)arr.size()) + i;
        if (i < 0 || i >= (int)arr.size()) return NIL_VALUE;
        else return arr[(size_t)i];
    }
    std::shared_ptr<Array> Array::compact()
    {
        std::vector<ObjectPtr> compacted;
        for (auto &i : arr)
        {
            if (i != NIL_VALUE) compacted.push_back(i);
        }
        return make_value(std::move(compacted));
    }
    std::shared_ptr<Number> Array::count(const FunctionArgs & args)
    {
        if (args.empty()) return size();
        else if (args.size() > 1) throw ArgumentError(this, "count");
        //TODO: if block
        
        int c = 0;
        for (auto &i : arr)
        {
            if (slim::eq(args[0].get(), i.get())) ++c;
        }
        return make_value((double)c);
    }
    std::shared_ptr<Object> Array::each(const FunctionArgs &args)
    {
        if (args.size() == 1)
        {
            auto proc = coerce<Proc>(args[0]);
            for (auto &i : arr)
            {
                proc->call({i});
            }
            return shared_from_this();
        }
        else if(args.size() == 0)
        {
            return make_enumerator(this, { &Array::each, "each" });
        }
        else throw ArgumentError(this, "each");
    }
    std::shared_ptr<Boolean> Array::empty_q()
    {
        return make_value(arr.empty());
    }
    std::shared_ptr<Object> Array::fetch(const FunctionArgs & args)
    {
        if (args.empty() || args.size() > 2) throw ArgumentError(this, "fetch");
        int i = (int)as_number(args[0]);
        if (i >= 0 && i < (int)arr.size()) return arr[(size_t)i];
        else if (args.size() == 2) return args[1];
        else throw IndexError("Index out of bounds");
    }
    std::shared_ptr<Object> Array::first(const FunctionArgs & args)
    {
        if (args.size() == 0)
        {
            return arr.empty() ? NIL_VALUE : arr.front();
        }
        else if (args.size() == 1)
        {
            auto count = (int)as_number(args[0].get());
            if (count < 0) throw ArgumentError(this, "first");
            std::vector<ObjectPtr> out;
            for (int i = 0; i < count && i < (int)arr.size(); ++i) out.push_back(arr[i]);
            return make_value(std::move(out));
        }
        else throw ArgumentError(this, "first");
    }
    std::shared_ptr<Array> Array::flatten(const FunctionArgs & args)
    {
        std::vector<ObjectPtr> out;
        if (args.size() == 0) flatten_imp(out, -1);
        else if (args.size() == 1)
        {
            auto level = (int)as_number(args[0].get());
            flatten_imp(out, level);
        }
        else throw ArgumentError(this, "flatten");
        return make_value(std::move(out));
    }
    void Array::flatten_imp(std::vector<ObjectPtr> &out, int level)
    {
        for (auto &i : arr)
        {
            auto arr2 = dynamic_cast<Array*>(i.get());
            if (arr2 && level != 0) arr2->flatten_imp(out, level - 1);
            else out.push_back(i);
        }
    }
    std::shared_ptr<Boolean> Array::frozen_q()
    {
        return TRUE_VALUE;
    }
    bool Array::include_q_imp(const Object *obj)
    {
        for (size_t i = 0; i < arr.size(); ++i)
        {
            if (slim::eq(obj, arr[i].get())) return true;
        }
        return false;
    }
    std::shared_ptr<Boolean> Array::include_q(const Object * obj)
    {
        return make_value(include_q_imp(obj));
    }
    std::shared_ptr<Object> Array::index(const Object *obj)
    {
        for (size_t i = 0; i < arr.size(); ++i)
        {
            if (slim::eq(obj, arr[i].get())) return make_value((double)i);
        }
        return NIL_VALUE;
    }
    std::shared_ptr<String> Array::join(const String * o_sep)
    {
        auto sep = o_sep->get_value();
        std::stringstream ss;
        if (!arr.empty()) ss << arr.front()->to_string();
        for (size_t i = 1; i < arr.size(); ++i) ss << sep << arr[i]->to_string();
        return make_value(ss.str());
    }
    std::shared_ptr<Object> Array::last(const FunctionArgs & args)
    {
        if (args.size() == 0)
        {
            return arr.empty() ? NIL_VALUE : arr.back();
        }
        else if (args.size() == 1)
        {
            auto count = (int)as_number(args[0].get());
            if (count < 0) throw ArgumentError(this, "last");
            std::vector<ObjectPtr> out;
            int start = (int)arr.size() - count;
            if (start < 0) start = 0;
            for (int i = start; i < (int)arr.size(); ++i) out.push_back(arr[i]);
            return make_value(std::move(out));
        }
        else throw ArgumentError(this, "last");
    }
    std::shared_ptr<Number> Array::size()
    {
        return make_value((double)arr.size());
    }
    std::shared_ptr<Object> Array::rassoc(const Object * a)
    {
        for (auto &i : arr)
        {
            auto arr2 = dynamic_cast<Array*>(i.get());
            if (arr2 && arr2->arr.size() >= 2 && slim::eq(a, arr2->get_value()[1].get()))
            {
                return arr2->shared_from_this();
            }
        }
        return NIL_VALUE;
    }
    std::shared_ptr<Array> Array::reverse()
    {
        std::vector<ObjectPtr> out{arr.rbegin(), arr.rend()};
        return make_value(std::move(out));
    }
    std::shared_ptr<Object> Array::rindex(const Object *obj)
    {
        for (int i = (int)arr.size() - 1;  i >= 0; --i)
        {
            if (slim::eq(obj, arr[i].get())) return make_value((double)i);
        }
        return NIL_VALUE;
    }
    std::shared_ptr<Array> Array::rotate(const FunctionArgs & args)
    {
        int start = 1;
        if (args.size() == 1) start = (int)as_number(args[0]);
        else if (args.size() > 1) throw ArgumentError(this, "rotate");
        
        if (arr.empty()) return std::static_pointer_cast<Array>(shared_from_this());

        std::vector<ObjectPtr> out;
        while (start < 0)
        {
            start = ((int)arr.size()) + start;
        }
        for (size_t i = 0; i < arr.size(); ++i)
        {
            out.push_back(arr[(i + start) % arr.size()]);
        }
        return make_value(std::move(out));
    }
    std::shared_ptr<Object> Array::slice(const FunctionArgs & args)
    {
        if (args.size() == 1)
        {
            int i = (int)as_number(args[0]);
            if (i < 0) i = ((int)arr.size() + i);
            if (i >= 0 && i < (int)arr.size()) return arr[(size_t)i];
            else return NIL_VALUE;
        }
        else if (args.size() == 2)
        {
            int start = (int)as_number(args[0]);
            int length = (int)as_number(args[1]);
            if (start < 0) start = ((int)arr.size()) + start;
            if (start >= 0 && start < (int)arr.size())
            {
                std::vector<ObjectPtr> out;
                for (int i = start; i < start + length && i < (int)arr.size(); ++i)
                {
                    out.push_back(arr[(size_t)i]);
                }
                return make_value(std::move(out));
            }
            else if (start == (int)arr.size()) return make_array({});
            else return NIL_VALUE;
        }
        else throw ArgumentError(this, "slice");
    }
    std::shared_ptr<Array> Array::sort()
    {
        std::vector<ObjectPtr> out = arr;
        std::sort(out.begin(), out.end(), ObjLess());
        return make_value(std::move(out));
    }
    std::shared_ptr<Array> Array::take(const Number * n)
    {
        std::vector<ObjectPtr> out;
        auto count = (int)n->get_value();
        for (int i = 0; i < count && i < (int)arr.size(); ++i) out.push_back(arr[i]);
        return make_value(std::move(out));
    }
    std::shared_ptr<Array> Array::uniq()
    {
        std::vector<ObjectPtr> out;
        for (auto &i : arr)
        {
            if (std::find_if(out.begin(), out.end(), [i](const ObjectPtr &a) {
                return slim::eq(a.get(), i.get());
            }) == out.end())
            {
                out.push_back(i);
            }
        }
        return make_value(std::move(out));
    }
    std::shared_ptr<Array> Array::values_at(const FunctionArgs & args)
    {
        std::vector<ObjectPtr> out;
        for (auto &arg : args)
        {
            auto i = (int)as_number(arg);
            if (i >= 0 && i < (int)arr.size()) out.push_back(arr[i]);
            else out.push_back(NIL_VALUE);
        }
        return make_value(std::move(out));
    }
    const MethodTable & Array::method_table() const
    {
        static const MethodTable table(Object::method_table(),
        {
            { &Array::assoc, "assoc" },
            { &Array::at, "at" },
            { &Array::compact, "compact" },
            { &Array::count, "count" },
            { &Array::each, "each" },
            { &Array::empty_q, "empty?" },
            { &Array::fetch, "fetch" },
            { &Array::first, "first" },
            { &Array::flatten, "flatten" },
            { &Array::frozen_q, "frozen?" },
            { &Array::include_q, "include?" },
            { &Array::index, "index" },
            { &Array::index, "find_index" },
            { &Array::join, "join" },
            { &Array::last, "last" },
            { &Array::size, "size" },
            { &Array::size, "length" },
            { &Array::rassoc, "rassoc" },
            { &Array::reverse, "reverse" },
            { &Array::rindex, "rindex" },
            { &Array::rotate, "rotate" },
            { &Array::slice, "slice" },
            { &Array::sort, "sort" },
            { &Array::take, "take" },
            { &Array::uniq, "uniq" },
            { &Array::values_at, "values_at" }
        });
        return table;
    }
}
