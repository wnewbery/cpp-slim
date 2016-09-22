#pragma once
#include "Object.hpp"
#include "Function.hpp"
#include <functional>
namespace slim
{
    /**Script Enumerator type.*/
    class Enumerator : public Object
    {
    public:
        Enumerator(ObjectPtr forward_self, Method forward, const FunctionArgs &args={})
            : forward_self(forward_self), forward(forward), args(args)
        {}

        static const std::string &name()
        {
            static const std::string TYPE_NAME = "Enumerator";
            return TYPE_NAME;
        }
        virtual const std::string& type_name()const override { return name(); }

        std::shared_ptr<Object> each(const FunctionArgs &args);
    protected:
        virtual const MethodTable &method_table()const;
    private:
        ObjectPtr forward_self;
        Method forward;
        FunctionArgs args;
    };

    inline std::shared_ptr<Enumerator> make_enumerator(Object *forward_self, Method forward, const FunctionArgs &args = {})
    {
        return std::make_shared<Enumerator>(forward_self->shared_from_this(), forward, args);
    }
    inline std::shared_ptr<Enumerator> make_enumerator(ObjectPtr forward_self, Method forward, const FunctionArgs &args = {})
    {
        return std::make_shared<Enumerator>(forward_self->shared_from_this(), forward, args);
    }
}
