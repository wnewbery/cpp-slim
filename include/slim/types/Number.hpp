#pragma once
#include "Object.hpp"
namespace slim
{
    class Boolean;
    /**Number script type.
     * Currently this is the only numeric type, taking the place of Ruby Numeric class and all its
     * subtypes. The number is stored as a double.
     */
    class Number : public Object
    {
    public:
        explicit Number(double v) : v(v) {}

        static const std::string &name()
        {
            static const std::string TYPE_NAME = "Number";
            return TYPE_NAME;
        }
        virtual const std::string& type_name()const override { return name(); }
        virtual std::string to_string()const override;
        virtual std::string inspect()const { return to_string(); }
        virtual bool eq(const Object *rhs)const override
        {
            return v == ((const Number*)rhs)->v;
        }
        virtual size_t hash()const { return detail::hash(v); }
        virtual int cmp(const Object *rhs)const override
        {
            double v2 = ((const Number*)rhs)->v;
            if (v < v2) return -1;
            if (v > v2) return 1;
            else return 0;
        }
        double get_value()const { return v; }

        //operators
        virtual ObjectPtr mul(Object *rhs)override;
        virtual ObjectPtr div(Object *rhs)override;
        virtual ObjectPtr mod(Object *rhs)override;
        virtual ObjectPtr pow(Object *rhs)override;
        virtual ObjectPtr add(Object *rhs)override;
        virtual ObjectPtr sub(Object *rhs)override;
        virtual ObjectPtr negate()override;
        virtual ObjectPtr bit_lshift(Object *rhs)override;
        virtual ObjectPtr bit_rshift(Object *rhs)override;
        virtual ObjectPtr bit_and(Object *rhs)override;
        virtual ObjectPtr bit_or(Object *rhs)override;
        virtual ObjectPtr bit_xor(Object *rhs)override;
        virtual ObjectPtr bit_not()override;

        std::shared_ptr<Number> to_f();
        std::shared_ptr<Number> to_i();
        std::shared_ptr<Number> next_float();
        std::shared_ptr<Number> prev_float();

        std::shared_ptr<Number> ceil();
        std::shared_ptr<Number> floor();
        std::shared_ptr<Number> round(const FunctionArgs &args);

        std::shared_ptr<Number> abs();
        //Ptr<Number> abs2();
        //angle, arg
        //coerce
        //conj, conjugate
        //denominator
        //div (integer division)
        //divmod
        //eql?
        //fdiv
        Ptr<Boolean> finite_q();
        //i
        //imag, imaginary
        ObjectPtr infinite_q();
        //integer?
        //modulo
        Ptr<Boolean> nan_q();
        //nonzero?
        //numerator
        //phase
        //polar
        //phase
        //quo
        //rationalize
        //real
        //real?
        //rect ,rectangular
        //remainder
        //step
        //to_c
        //to_r
        Ptr<Boolean> zero_q();
    protected:
        virtual const MethodTable &method_table()const;
    private:
        double v;
    };

    inline std::shared_ptr<Number> make_value(double v)
    {
        return create_object<Number>(v);
    }
    inline std::shared_ptr<Number> make_value(int v)
    {
        return create_object<Number>(v);
    }
    inline std::shared_ptr<Number> make_value(unsigned v)
    {
        return create_object<Number>(v);
    }
    inline std::shared_ptr<Number> make_value(long long v)
    {
        return create_object<Number>((double)v);
    }
    inline std::shared_ptr<Number> make_value(unsigned long long v)
    {
        return create_object<Number>((double)v);
    }
}
