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
        static const std::string &name()
        {
            static const std::string TYPE_NAME = "Enumerator";
            return TYPE_NAME;
        }
        virtual const std::string& type_name()const override { return name(); }

        virtual ObjectPtr this_obj()override { return shared_from_this(); }

        virtual ObjectPtr each(const FunctionArgs &args)=0;
        ObjectPtr with_index(const FunctionArgs &args);
    protected:
        const MethodTable &method_table()const override;
    };
    /**Script method enumerator.*/
    class MethodEnumerator : public Enumerator
    {
    public:
        MethodEnumerator(ObjectPtr forward_self, Method forward, const FunctionArgs &args = {})
            : forward_self(forward_self), forward(forward), args(args)
        {}
        virtual ObjectPtr each(const FunctionArgs &args)override;
    protected:

    private:
        ObjectPtr forward_self;
        Method forward;
        FunctionArgs args;
    };
    /**C++ std::function enumerator.*/
    class FunctionEnumerator : public Enumerator
    {
    public:
        typedef std::function<ObjectPtr(const FunctionArgs &args)> Func;
        FunctionEnumerator(Func func, const FunctionArgs &args = {})
            : func(func), args(args) {}
        virtual ObjectPtr each(const FunctionArgs &args)override;
    private:
        Func func;
        FunctionArgs args;
    };

    inline Ptr<Enumerator> make_enumerator(Object *forward_self, Method forward, const FunctionArgs &args = {})
    {
        return create_object<MethodEnumerator>(forward_self->shared_from_this(), forward, args);
    }
    inline Ptr<Enumerator> make_enumerator(ObjectPtr forward_self, Method forward, const FunctionArgs &args = {})
    {
        return create_object<MethodEnumerator>(forward_self, forward, args);
    }
    inline Ptr<Enumerator> make_enumerator(FunctionEnumerator::Func f)
    {
        return create_object<FunctionEnumerator>(f);
    }
}
