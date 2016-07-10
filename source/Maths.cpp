#include "Maths.hpp"
#include "types/Array.hpp"
#include "types/Number.hpp"
#include "Error.hpp"
#include <cmath>
namespace slim
{
    std::shared_ptr<Number> acos(const Number *n)
    {
        return make_value(std::acos(n->get_value()));
    }
    std::shared_ptr<Number> asin(const Number *n)
    {
        return make_value(std::asin(n->get_value()));
    }
    std::shared_ptr<Number> atan(const Number *n)
    {
        return make_value(std::atan(n->get_value()));
    }
    std::shared_ptr<Number> atan2(const Number *y, const Number *x)
    {
        return make_value(std::atan2(y->get_value(), x->get_value()));
    }

    std::shared_ptr<Number> acosh(const Number *n)
    {
        return make_value(std::acosh(n->get_value()));
    }
    std::shared_ptr<Number> asinh(const Number *n)
    {
        return make_value(std::asinh(n->get_value()));
    }
    std::shared_ptr<Number> atanh(const Number *n)
    {
        return make_value(std::atanh(n->get_value()));
    }

    std::shared_ptr<Number> cos(const Number *n)
    {
        return make_value(std::cos(n->get_value()));
    }
    std::shared_ptr<Number> sin(const Number *n)
    {
        return make_value(std::sin(n->get_value()));
    }
    std::shared_ptr<Number> tan(const Number *n)
    {
        return make_value(std::tan(n->get_value()));
    }

    std::shared_ptr<Number> cosh(const Number *n)
    {
        return make_value(std::cosh(n->get_value()));
    }
    std::shared_ptr<Number> sinh(const Number *n)
    {
        return make_value(std::sinh(n->get_value()));
    }
    std::shared_ptr<Number> tanh(const Number *n)
    {
        return make_value(std::tanh(n->get_value()));
    }

    std::shared_ptr<Number> cbrt(const Number *n)
    {
        return make_value(std::cbrt(n->get_value()));
    }
    std::shared_ptr<Number> sqrt(const Number *n)
    {
        return make_value(std::sqrt(n->get_value()));
    }
    std::shared_ptr<Number> exp(const Number *n)
    {
        return make_value(std::exp(n->get_value()));
    }
    std::shared_ptr<Number> log(const Number *n)
    {
        return make_value(std::log(n->get_value()));
    }
    std::shared_ptr<Number> log2(const Number *n)
    {
        return make_value(std::log2(n->get_value()));
    }
    std::shared_ptr<Number> log10(const Number *n)
    {
        return make_value(std::log10(n->get_value()));
    }

    std::shared_ptr<Number> erf(const Number *n)
    {
        return make_value(std::erf(n->get_value()));
    }
    std::shared_ptr<Number> erfc(const Number *n)
    {
        return make_value(std::erfc(n->get_value()));
    }
    //std::shared_ptr<Object> frexp(const Number *n)
    //{
    //    
    //}
    std::shared_ptr<Number> gamma(const Number *n)
    {
        return make_value(std::tgamma(n->get_value()));
    }
    std::shared_ptr<Number> hypot(const Number *x, const Number *y)
    {
        auto x2 = x->get_value();
        auto y2 = y->get_value();
        return make_value(std::sqrt(x2*x2 + y2*y2));
    }
    std::shared_ptr<Number> ldexp(const Number *x, const Number *exp)
    {
        return make_value(std::ldexp(x->get_value(), (int)exp->get_value()));
    }
    std::shared_ptr<Array> lgamma(const Number *n)
    {
        double a = std::lgamma(n->get_value());
        double b = std::tgamma(n->get_value());
        return make_array({make_value(a), make_value(b < 0 ? -1.0 : 1.0)});
    }
}
