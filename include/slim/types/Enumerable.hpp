#pragma once
#include "Object.hpp"
#include "Function.hpp"
#include <functional>
namespace slim
{
    class Array;
    class Hash;

    /**Enumerable mixin module.*/
    class Enumerable
    {
    public:
        virtual ObjectPtr each(const FunctionArgs &args) = 0;

        ObjectPtr each2(
            const FunctionArgs &args,
            std::function<ObjectPtr(const FunctionArgs &args)> func);

        //all?
        //any?
        //chunk
        //collect
        //collect_concat
        //count
        //cycle
        //detect
        //drop
        //drop_while
        //each_cons
        //each_entry
        //each_with_index
        //each_with_object
        //entries
        //find
        //find_all
        //find_index
        //first
        //flat_map
        //grep
        //group_by
        //include?
        //inject
        //lazy
        //map
        //max
        //max_by
        //member?
        //min
        //min_by
        //minmax
        //minmax_by
        //none?
        //one?
        //partition
        //reduce
        //reject
        //reverse_each
        //select
        //slice_after
        //slice_before
        //slice_when
        //sort
        //sort_by
        //take
        //take_while
        Ptr<Array> to_a(const FunctionArgs &args);
        Ptr<Hash> to_h(const FunctionArgs &args);
        //zip

    protected:
        template<class Implementor>
        static std::vector<Method> get_methods()
        {
            return
            {
                { method<Implementor>(&Enumerable::each), "each" },
                { method<Implementor>(&Enumerable::to_a), "to_a" },
                { method<Implementor>(&Enumerable::to_h), "to_h" }
            };
        }

        template<class Implementor, class Ret, class...Args>
        static auto method(Ret(Enumerable::*m)(Args...)) -> Ret(Implementor::*)(Args...)
        {
            return m;
        }
    };
}
