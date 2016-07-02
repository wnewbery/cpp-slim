#pragma once
#include "Object.hpp"
namespace slim
{
    class Number : public Object
    {
    public:
        static const std::string TYPE_NAME;
        explicit Number(double v) : v(v) {}

        virtual const std::string& type_name()const override { return TYPE_NAME; }
        virtual std::string to_string()const override;
        virtual bool is_true()const override { return v ? true : false; }
        virtual bool eq(const Object *rhs)const override
        {
            return v == ((const Number*)rhs)->v;
        }
        virtual int cmp(const Object *rhs)const override
        {
            double v2 = ((const Number*)rhs)->v;
            if (v < v2) return -1;
            if (v > v2) return 1;
            else return 0;
        }
        double get_value()const { return v; }

        //operators
        virtual ObjectPtr mul(Object *rhs);
        virtual ObjectPtr div(Object *rhs);
        virtual ObjectPtr mod(Object *rhs);
        virtual ObjectPtr add(Object *rhs);
        virtual ObjectPtr sub(Object *rhs);
        virtual ObjectPtr negate();

        std::shared_ptr<Number> to_f();
        std::shared_ptr<Number> to_i();
        std::shared_ptr<Number> abs();
        std::shared_ptr<Number> next_float();
        std::shared_ptr<Number> prev_float();

        std::shared_ptr<Number> ceil();
        std::shared_ptr<Number> floor();
        std::shared_ptr<Number> round(const FunctionArgs &args);

        //Not provided
        //arg
        //coerce
        //denominator
        //numerator
        //divmod
        //fdiv
        //eql?
        //finite?
        //infinite?
        //hash
        //modulo
        //nan?
        //phase
        //quo
        //rationalize
        //to_r
        //zero?
    protected:
        virtual const MethodTable &method_table()const;
    private:
        double v;
    };

    inline std::shared_ptr<Number> make_value(double v)
    {
        return create_object<Number>(v);
    }
}
