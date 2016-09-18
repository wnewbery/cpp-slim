#pragma once
#include "Type.hpp"
namespace slim
{
    class Array;
    class Boolean;
    class Number;
    typedef std::shared_ptr<Number> NumberPtr;
    /**The Math module. Implements the static math methods and constants.*/
    class Math : public Type
    {
    public:
        static const std::string TYPE_NAME;
        Math();

        virtual const std::string& type_name()const override { return TYPE_NAME; }

        NumberPtr acos(Number *n);
        NumberPtr asin(Number *n);
        NumberPtr atan(Number *n);
        NumberPtr atan2(Number *y, Number *x);

        NumberPtr acosh(Number *n);
        NumberPtr asinh(Number *n);
        NumberPtr atanh(Number *n);

        NumberPtr cos(Number *n);
        NumberPtr sin(Number *n);
        NumberPtr tan(Number *n);

        NumberPtr cosh(Number *n);
        NumberPtr sinh(Number *n);
        NumberPtr tanh(Number *n);

        NumberPtr cbrt(Number *n);
        NumberPtr sqrt(Number *n);
        NumberPtr exp(Number *n);
        NumberPtr log(Number *n);
        NumberPtr log2(Number *n);
        NumberPtr log10(Number *n);

        NumberPtr erf(Number *n);
        NumberPtr erfc(Number *n);
        //frexp
        NumberPtr gamma(Number *n);
        NumberPtr hypot(Number *x, Number *y);
        NumberPtr ldexp(Number *x, Number *exp);
        std::shared_ptr<Array> lgamma(Number *n);
    protected:
        virtual const MethodTable &method_table()const override;
    };
}
