#pragma once
#include "Object.hpp"
namespace slim
{
    class Boolean;
    extern const std::shared_ptr<Boolean> TRUE_VALUE;
    extern const std::shared_ptr<Boolean> FALSE_VALUE;
    class Boolean : public Object
    {
    public:
        static const std::string TYPE_NAME;

        template<class T>
        static std::shared_ptr<T> create(bool b)
        {
            return b ? TRUE_VALUE : FALSE_VALUE;
        }

        explicit Boolean(bool b) : b(b) {}

        virtual const std::string& type_name()const override { return TYPE_NAME; }
        virtual std::string to_string()const override { return b ? "true" : "false"; }
        virtual bool is_true()const override { return b; }
        virtual bool eq(const Object *rhs)const override
        {
            return b == ((const Boolean*)rhs)->b;
        }
        virtual int cmp(const Object *rhs)const override
        {
            return (b ? 1 : 0) - (((const Boolean*)rhs)->b ? 1 : 0);
        }
    private:
        bool b;
    };

    inline std::shared_ptr<Boolean> make_value(bool b)
    {
        return create_object<Boolean>(b);
    }
}
