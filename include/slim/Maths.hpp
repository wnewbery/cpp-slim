#pragma once
#include "Function.hpp"
namespace slim
{
    class Number;
    class Array;
    std::shared_ptr<Number> acos(const Number *n);
    std::shared_ptr<Number> asin(const Number *n);
    std::shared_ptr<Number> atan(const Number *n);
    std::shared_ptr<Number> atan2(const Number *y, const Number *x);

    std::shared_ptr<Number> acosh(const Number *n);
    std::shared_ptr<Number> asinh(const Number *n);
    std::shared_ptr<Number> atanh(const Number *n);

    std::shared_ptr<Number> cos(const Number *n);
    std::shared_ptr<Number> sin(const Number *n);
    std::shared_ptr<Number> tan(const Number *n);

    std::shared_ptr<Number> cosh(const Number *n);
    std::shared_ptr<Number> sinh(const Number *n);
    std::shared_ptr<Number> tanh(const Number *n);

    std::shared_ptr<Number> cbrt(const Number *n);
    std::shared_ptr<Number> sqrt(const Number *n);
    std::shared_ptr<Number> exp(const Number *n);
    std::shared_ptr<Number> log(const Number *n);
    std::shared_ptr<Number> log2(const Number *n);
    std::shared_ptr<Number> log10(const Number *n);

    std::shared_ptr<Number> erf(const Number *n);
    std::shared_ptr<Number> erfc(const Number *n);
    //std::shared_ptr<Object> frexp(const Number *n);
    std::shared_ptr<Number> gamma(const Number *n);
    std::shared_ptr<Number> hypot(const Number *x, const Number *y);
    std::shared_ptr<Number> ldexp(const Number *x, const Number *exp);
    std::shared_ptr<Array> lgamma(const Number *n);
}
