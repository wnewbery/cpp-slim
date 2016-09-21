#include "types/Number.hpp"
#include "Error.hpp"
#include "Function.hpp"
#include <sstream>
#include <cmath>

namespace slim
{
    std::string Number::to_string()const
    {
        std::stringstream ss;
        ss << v;
        return ss.str();
    }

    const MethodTable &Number::method_table()const
    {
        static const MethodTable table(Object::method_table(),
        {
            { &Number::to_f, "to_f" },
            { &Number::to_f, "to_d" },
            { &Number::to_i, "to_i" },
            { &Number::abs, "abs" },
            { &Number::next_float, "next_float" },
            { &Number::prev_float, "prev_float" },
            { &Number::ceil, "ceil" },
            { &Number::floor, "floor" },
            { &Number::round, "round" },

            //alias
            { &Number::to_i, "truncate" },
            { &Number::to_i, "to_int" },
            { &Number::abs, "magnitude" }
        });
        return table;
    }

    ObjectPtr Number::mul(Object *rhs)
    {
        return make_value(v * coerce<Number>(rhs)->get_value());
    }
    ObjectPtr Number::div(Object *rhs)
    {
        return make_value(v / coerce<Number>(rhs)->get_value());
    }
    ObjectPtr Number::mod(Object *rhs)
    {
        return make_value(std::fmod(v, coerce<Number>(rhs)->get_value()));
    }
    ObjectPtr Number::pow(Object *rhs)
    {
        return make_value(std::pow(v, coerce<Number>(rhs)->get_value()));
    }
    ObjectPtr Number::add(Object *rhs)
    {
        return make_value(v + coerce<Number>(rhs)->get_value());
    }
    ObjectPtr Number::sub(Object *rhs)
    {
        return make_value(v - coerce<Number>(rhs)->get_value());
    }
    ObjectPtr Number::negate()
    {
        return make_value(-v);
    }

    ObjectPtr Number::bit_lshift(Object * rhs)
    {
        return make_value((int)v << (int)coerce<Number>(rhs)->get_value());
    }
    ObjectPtr Number::bit_rshift(Object * rhs)
    {
        return make_value((int)v >> (int)coerce<Number>(rhs)->get_value());
    }
    ObjectPtr Number::bit_and(Object * rhs)
    {
        return make_value((int)v & (int)coerce<Number>(rhs)->get_value());
    }
    ObjectPtr Number::bit_or(Object * rhs)
    {
        return make_value((int)v | (int)coerce<Number>(rhs)->get_value());
    }
    ObjectPtr Number::bit_xor(Object * rhs)
    {
        return make_value((int)v ^ (int)coerce<Number>(rhs)->get_value());
    }
    ObjectPtr Number::bit_not()
    {
        return make_value(~(int)v);
    }

    std::shared_ptr<Number> Number::to_f()
    {
        return std::static_pointer_cast<Number>(shared_from_this());
    }
    std::shared_ptr<Number> Number::to_i()
    {
        return make_value(std::trunc(v));
    }

    std::shared_ptr<Number> Number::abs()
    {
        return make_value(std::abs(v));
    }

    std::shared_ptr<Number> Number::next_float()
    {
        auto v2 = std::nextafter(v, INFINITY);
        return make_value(v2);
    }

    std::shared_ptr<Number> Number::prev_float()
    {
        auto v2 = std::nextafter(v, -INFINITY);
        return make_value(v2);
    }

    std::shared_ptr<Number> Number::ceil()
    {
        return make_value(std::ceil(v));
    }

    std::shared_ptr<Number> Number::floor()
    {
        return make_value(std::floor(v));
    }


    inline double round_f(double v, double ndigits)
    {
        auto factor = std::pow(10.0, ndigits - std::ceil(std::log10(std::fabs(v))));
        return std::round(v * factor) / factor;
    }
    std::shared_ptr<Number> Number::round(const FunctionArgs & args)
    {
        double ndigits = 0;
        if (args.size() == 1) ndigits = as_number(args[0]);
        else if (args.size() > 1) throw ArgumentError(this, "round");

        if (v == 0) return std::static_pointer_cast<Number>(shared_from_this());
        if (ndigits == 0) return make_value(std::round(v));
        else if (ndigits > 0) return make_value(round_f(v, ndigits));
        else return make_value(std::round(round_f(v, -ndigits)));
    }


}

