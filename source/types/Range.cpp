#include "types/Range.hpp"
#include "types/Array.hpp"
#include "types/Boolean.hpp"
#include "types/Enumerator.hpp"
#include "types/Hash.hpp"
#include "types/Number.hpp"
#include "types/Proc.hpp"
#include <deque>
#include <sstream>

namespace slim
{
    Range::Range(Ptr<Number> begin, Ptr<Number> end, Boolean *exclude_end)
        : Range(begin, end, exclude_end->is_true())
    {}
    Range::Range(Ptr<Number> begin, Ptr<Number> end, bool exclude_end)
        : _begin(begin->get_value()), _end(end->get_value())
        , exclude_end(exclude_end)
    {}
    Range::Range(Ptr<Object> begin, Ptr<Object> end, bool exclude_end)
        : Range(coerce<Number>(begin), coerce<Number>(end), exclude_end)
    {}


    std::string Range::to_string()const
    {
        std::stringstream ss;
        ss << _begin << (exclude_end ? "..." : "..") << _end;
        return ss.str();
    }
    bool Range::eq(const Object *rhs)const
    {
        auto x = coerce<Range>(rhs);
        return _begin == x->_begin && _end == x->_end && exclude_end == x->exclude_end;
    }
    size_t Range::hash()const
    {
        size_t h = 0;
        detail::hash_combine(h, _begin);
        detail::hash_combine(h, _end);
        detail::hash_combine(h, exclude_end);
        return h;
    }

    bool Range::get_beg_len(int *range_begin, int *range_len, int seq_len)
    {
        //The cast means a range such a "(0.0)...(1.5)" ends up only having "0" rather than "[0, 1]"
        //as with #each, #to_a, etc. This is intentional as is what Ruby 2.3 does.
        int b = (int)_begin;
        int e = (int)_end;

        if (b < 0)
            b += seq_len;
        if (e < 0)
            e += seq_len;

        if (!exclude_end) e += 1;
        if (e < b) e = b;

        *range_begin = b;
        *range_len = e - b;
        return 0 <= b && b <= seq_len && 0 <= *range_len;
    }

    Ptr<Object> Range::begin()
    {
        return make_value(_begin);
    }
    bool Range::cover_q(Object *obj)
    {
        auto num = coerce<Number>(obj)->get_value();
        if (exclude_end) return _begin <= num && num < _end;
        else return _begin <= num && num <= _end;
    }
    ObjectPtr Range::each(const FunctionArgs &args)
    {
        Proc*proc = nullptr;
        unpack<0>(args, &proc);
        if (proc)
        {
            if (exclude_end)
            {
                for (auto i = _begin; i < _end; i += 1)
                    proc->call({make_value(i)});
            }
            else
            {
                for (auto i = _begin; i <= _end; i += 1)
                    proc->call({make_value(i)});
            }
            return shared_from_this();
        }
        else return make_enumerator(this, {&Range::each, "each"});
    }
    Ptr<Object> Range::end()
    {
        return make_value(_end);
    }
    bool Range::exclude_end_q()
    {
        return exclude_end;
    }
    Ptr<Object> Range::first(const FunctionArgs &args)
    {
        Number *n_obj = nullptr;
        unpack<0>(args, &n_obj);
        if (n_obj)
        {
            auto n = (int)n_obj->get_value();
            if (n < 0) throw ArgumentError("negative size");
            std::vector<Ptr<Object>> arr;
            if (exclude_end)
            {
                for (auto i = _begin; i < _end && (int)arr.size() < n; i += 1)
                    arr.push_back(make_value(i));
            }
            else
            {
                for (auto i = _begin; i <= _end && (int)arr.size() < n; i += 1)
                    arr.push_back(make_value(i));
            }
            return make_value(arr);
        }
        else return make_value(_begin);
    }
    bool Range::include_q(Object *obj)
    {
        return cover_q(obj);
    }
    Ptr<Object> Range::last(const FunctionArgs &args)
    {
        Number *n_obj = nullptr;
        unpack<0>(args, &n_obj);
        if (n_obj)
        {
            auto n = (int)n_obj->get_value();
            if (n < 0) throw ArgumentError("negative size");
            std::deque<Ptr<Object>> arr;
            if (exclude_end)
            {
                for (auto i = _begin; i < _end; i += 1)
                {
                    if ((int)arr.size() >= n) arr.pop_front();
                    arr.push_back(make_value(i));
                }
            }
            else
            {
                for (auto i = _begin; i <= _end; i += 1)
                {
                    if ((int)arr.size() >= n) arr.pop_front();
                    arr.push_back(make_value(i));
                }
            }
            return make_array({arr.begin(), arr.end()});
        }
        else return make_value(_end);
    }
    Ptr<Object> Range::size()
    {
        if (exclude_end)
        {
            if (_end <= _begin) return make_value(0);
            double n = _end - _begin;
            int ni = (int)n;
            if (n == ni) return make_value(ni);
            else return make_value(ni + 1);
        }
        else
        {
            if (_end < _begin) return make_value(0);
            return make_value((int)(_end - _begin + 1));
        }
    }
    Ptr<Object> Range::step(const FunctionArgs &args)
    {
        Proc *proc = nullptr;
        if (args.size() == 1) proc = dynamic_cast<Proc*>(args[0].get());
        else if (args.size() == 2) proc = coerce<Proc>(args[1].get());
        else if (args.size() >= 3) throw ArgumentCountError(args.size(), 0, 2);

        if (proc)
        {
            double step = 1;
            if (args.size() == 2) step = coerce<Number>(args[0])->get_value();
            if (exclude_end)
            {
                for (auto i = _begin; i < _end; i += step)
                    proc->call({make_value(i)});
            }
            else
            {
                for (auto i = _begin; i <= _end; i += step)
                    proc->call({make_value(i)});
            }
            return shared_from_this();
        }
        else return make_enumerator(this, {&Range::step, "step"}, args);
    }


    const MethodTable &Range::method_table()const
    {
        static const MethodTable table = MethodTable(Object::method_table())
            .add_all(Enumerable::get_methods<Range>())
            .add_all({
            { &Range::begin, "begin" },
            { &Range::cover_q, "cover?" },
            { &Range::end, "end" },
            { &Range::exclude_end_q, "exclude_end?" },
            { &Range::first, "first" },
            { &Range::include_q, "include?" },
            { &Range::include_q, "member?" },
            { &Range::last, "last" },
            { &Range::size, "size" },
            { &Range::step, "step" }
        });
        return table;
    }
}
