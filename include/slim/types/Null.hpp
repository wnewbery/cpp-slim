#pragma once
#include "Object.hpp"
namespace slim
{
    class Null;
    extern const std::shared_ptr<Null> NULL_VALUE;
    class Null : public Object
    {
    public:
        static const std::string TYPE_NAME;

        template<class T>
        static std::shared_ptr<T> create()
        {
            return NULL_VALUE;
        }

        explicit Null() {}

        virtual const std::string& type_name()const override { return TYPE_NAME; }
        virtual std::string to_string()const override { return "null"; }
        virtual std::string inspect()const override  { return to_string(); }
        virtual bool is_true()const override { return false; }
        virtual size_t hash()const { return 0; }
        std::shared_ptr<Number> to_f();
        std::shared_ptr<Number> to_i();
    protected:
        virtual const MethodTable &method_table()const;
    };
}
