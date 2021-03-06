#pragma once
#include "Object.hpp"
namespace slim
{
    class Boolean;
    extern const std::shared_ptr<Boolean> TRUE_VALUE;
    extern const std::shared_ptr<Boolean> FALSE_VALUE;
    /**Script Boolean type.*/
    class Boolean : public Object
    {
    public:
        template<class T>
        static std::shared_ptr<T> create(bool b)
        {
            return b ? TRUE_VALUE : FALSE_VALUE;
        }

        explicit Boolean(bool b) : b(b) {}

        static const std::string &name()
        {
            static const std::string TYPE_NAME = "Boolean";
            return TYPE_NAME;
        }
        virtual const std::string& type_name()const override { return name(); }

        virtual std::string to_string()const override { return b ? "true" : "false"; }
        virtual std::string inspect()const override { return to_string(); }
        virtual bool is_true()const override { return b; }
        virtual bool eq(const Object *rhs)const override
        {
            return b == ((const Boolean*)rhs)->b;
        }
        virtual size_t hash()const { return b ? 1 : 0; }
        virtual int cmp(const Object *rhs)const override
        {
            return (b ? 1 : 0) - (((const Boolean*)rhs)->b ? 1 : 0);
        }
        std::shared_ptr<Number> to_f();
        std::shared_ptr<Number> to_i();
    protected:
        virtual const MethodTable &method_table()const;
    private:
        bool b;
    };

    inline std::shared_ptr<Boolean> make_value(bool b)
    {
        return create_object<Boolean>(b);
    }
}
