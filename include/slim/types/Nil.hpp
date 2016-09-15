#pragma once
#include "Object.hpp"
namespace slim
{
    class Nil;
    extern const std::shared_ptr<Nil> NIL_VALUE;
    class Nil : public Object
    {
    public:
        static const std::string TYPE_NAME;

        template<class T>
        static std::shared_ptr<T> create()
        {
            return NIL_VALUE;
        }

        explicit Nil() {}

        virtual const std::string& type_name()const override { return TYPE_NAME; }
        virtual std::string to_string()const override { return ""; }
        virtual std::string inspect()const override  { return "nil"; }
        virtual bool is_true()const override { return false; }
        virtual size_t hash()const { return 0; }
        std::shared_ptr<Number> to_f();
        std::shared_ptr<Number> to_i();
    protected:
        virtual const MethodTable &method_table()const;
    };
}
