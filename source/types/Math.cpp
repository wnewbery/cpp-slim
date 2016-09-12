#include "types/Math.hpp"
#include "types/Array.hpp"
#include "types/Number.hpp"
#include "Error.hpp"
#include "Function.hpp"
#include <cmath>
namespace slim
{
    const std::string Math::TYPE_NAME = "Math";

    Math::Math()
    {
        constants[symbol("E")]  = make_value(2.718281828459045);
        constants[symbol("PI")] = make_value(3.141592653589793);
    }

    const MethodTable &Math::method_table()const
    {
        static const MethodTable table(Type::method_table(),
        {
            { &Math::acos, "acos" },{ &Math::asin, "asin" },{ &Math::atan, "atan" },{ &Math::atan2, "atan2" },
            { &Math::acosh, "acosh" },{ &Math::asinh, "asinh" },{ &Math::atanh, "atanh" },
            { &Math::cos, "cos" },{ &Math::sin, "sin" },{ &Math::tan, "tan" },{ &Math::atan2, "atan2" },
            { &Math::cosh, "cosh" },{ &Math::sinh, "sinh" },{ &Math::tanh, "tanh" },
            { &Math::cbrt, "cbrt" },{ &Math::sqrt, "sqrt" },{ &Math::exp, "exp" },
            { &Math::log, "log" },{ &Math::log2, "log2" },{ &Math::log10, "log10" },
            { &Math::erf, "erf" },{ &Math::erfc, "erfc" },
            { &Math::gamma, "gamma" },{ &Math::hypot, "hypot" },{ &Math::ldexp, "ldexp" },{ &Math::lgamma, "lgamma" }
        });
        return table;
    }

    NumberPtr Math::acos(Number *n)
    {
        return make_value(std::acos(n->get_value()));
    }
    NumberPtr Math::asin(Number *n)
    {
        return make_value(std::asin(n->get_value()));
    }
    NumberPtr Math::atan(Number *n)
    {
        return make_value(std::atan(n->get_value()));
    }
    NumberPtr Math::atan2(Number *y, Number *x)
    {
        return make_value(std::atan2(y->get_value(), x->get_value()));
    }

    NumberPtr Math::acosh(Number *n)
    {
        return make_value(std::acosh(n->get_value()));
    }
    NumberPtr Math::asinh(Number *n)
    {
        return make_value(std::asinh(n->get_value()));
    }
    NumberPtr Math::atanh(Number *n)
    {
        return make_value(std::atanh(n->get_value()));
    }

    NumberPtr Math::cos(Number *n)
    {
        return make_value(std::cos(n->get_value()));
    }
    NumberPtr Math::sin(Number *n)
    {
        return make_value(std::sin(n->get_value()));
    }
    NumberPtr Math::tan(Number *n)
    {
        return make_value(std::tan(n->get_value()));
    }

    NumberPtr Math::cosh(Number *n)
    {
        return make_value(std::cosh(n->get_value()));
    }
    NumberPtr Math::sinh(Number *n)
    {
        return make_value(std::sinh(n->get_value()));
    }
    NumberPtr Math::tanh(Number *n)
    {
        return make_value(std::tanh(n->get_value()));
    }

    NumberPtr Math::cbrt(Number *n)
    {
        return make_value(std::cbrt(n->get_value()));
    }
    NumberPtr Math::sqrt(Number *n)
    {
        return make_value(std::sqrt(n->get_value()));
    }
    NumberPtr Math::exp(Number *n)
    {
        return make_value(std::exp(n->get_value()));
    }
    NumberPtr Math::log(Number *n)
    {
        return make_value(std::log(n->get_value()));
    }
    NumberPtr Math::log2(Number *n)
    {
        return make_value(std::log2(n->get_value()));
    }
    NumberPtr Math::log10(Number *n)
    {
        return make_value(std::log10(n->get_value()));
    }

    NumberPtr Math::erf(Number *n)
    {
        return make_value(std::erf(n->get_value()));
    }
    NumberPtr Math::erfc(Number *n)
    {
        return make_value(std::erfc(n->get_value()));
    }
    //std::shared_ptr<Object> Math::frexp(Number *n)
    //{
    //    
    //}
    NumberPtr Math::gamma(Number *n)
    {
        return make_value(std::tgamma(n->get_value()));
    }
    NumberPtr Math::hypot(Number *x, Number *y)
    {
        auto x2 = x->get_value();
        auto y2 = y->get_value();
        return make_value(std::sqrt(x2*x2 + y2*y2));
    }
    NumberPtr Math::ldexp(Number *x, Number *exp)
    {
        return make_value(std::ldexp(x->get_value(), (int)exp->get_value()));
    }
    std::shared_ptr<Array> Math::lgamma(Number *n)
    {
        double a = std::lgamma(n->get_value());
        double b = std::tgamma(n->get_value());
        return make_array({ make_value(a), make_value(b < 0 ? -1.0 : 1.0) });
    }
}
