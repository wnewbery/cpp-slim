#pragma once
#include "Enumerable.hpp"
#include "Object.hpp"
#include "Function.hpp"
#include <functional>
namespace slim
{
    class Array;
    class Hash;
    /**Script Enumerator type.*/
    class Enumerator : public Enumerable, public Object
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


        virtual ObjectPtr each(const FunctionArgs &args)override;
    protected:
        const MethodTable &method_table()const override;
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
