#pragma once
#include "Object.hpp"
#include <functional>
namespace slim
{
     
    class Enumerator : public Object
    {
    public:
        static const std::string TYPE_NAME;
        Enumerator(ObjectPtr forward_self, Method forward)
            : forward_self(forward_self), forward(forward)
        {}

        virtual const std::string& type_name()const override { return TYPE_NAME; }

        std::shared_ptr<Object> each(const FunctionArgs &args);
    protected:
        virtual const MethodTable &method_table()const;
    private:
        ObjectPtr forward_self;
        Method forward;
    };

    inline std::shared_ptr<Enumerator> make_enumerator(Object *forward_self, Method forward)
    {
        return std::make_shared<Enumerator>(forward_self->shared_from_this(), forward);
    }
    inline std::shared_ptr<Enumerator> make_enumerator(ObjectPtr forward_self, Method forward)
    {
        return std::make_shared<Enumerator>(forward_self->shared_from_this(), forward);
    }
}
