#pragma once
#include "Object.hpp"
#include "Function.hpp"
#include <functional>
namespace slim
{
    class Array;
    class Boolean;
    class Hash;
    class Proc;

    /**Enumerable mixin module.*/
    class Enumerable
    {
    public:
        virtual ObjectPtr each(const FunctionArgs &args) = 0;
        virtual ObjectPtr this_obj() = 0;

        ObjectPtr each2(
            const FunctionArgs &args,
            std::function<ObjectPtr(const FunctionArgs &args)> func);

        /**Each element as a single element. Multiple values, e.g. from Hash or each_with_index
         * will be wrapped in an array.
         */
        ObjectPtr each_single(
            const FunctionArgs &args,
            std::function<ObjectPtr(Object *arg)> func);
        ObjectPtr each_single(std::function<ObjectPtr(Object *arg)> func)
        {
            return each_single({}, func);
        }

        Ptr<Boolean> all_q(const FunctionArgs &args);
        Ptr<Boolean> any_q(const FunctionArgs &args);
        //chunk
        //collect_concat (flat_map)
        Ptr<Number> count(const FunctionArgs &args);
        //cycle
        //detect (find)
        Ptr<Array> drop(Number *n);
        ObjectPtr drop_while(const FunctionArgs &args);
        //each_cons
        //each_entry
        ObjectPtr each_with_index(const FunctionArgs &args);
        //each_with_object
        //entries
        ObjectPtr find(const FunctionArgs &args);
        //find_all = select
        ObjectPtr find_index(const FunctionArgs &args);
        //first
        //flat_map
        //grep
        //group_by
        //include?
        //inject
        //lazy
        ObjectPtr map(const FunctionArgs &args);
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
        //reverse_each
        ObjectPtr reject(const FunctionArgs &args);
        ObjectPtr select(const FunctionArgs &args);
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
                { method<Implementor>(&Enumerable::all_q), "all?" },
                { method<Implementor>(&Enumerable::any_q), "any?" },
                { method<Implementor>(&Enumerable::count), "count" },
                { method<Implementor>(&Enumerable::each), "each" },
                { method<Implementor>(&Enumerable::map), "collect" },
                { method<Implementor>(&Enumerable::drop), "drop" },
                { method<Implementor>(&Enumerable::drop_while), "drop_while" },
                { method<Implementor>(&Enumerable::each_with_index), "each_with_index" },
                { method<Implementor>(&Enumerable::find), "find" },
                { method<Implementor>(&Enumerable::find_index), "find_index" },
                { method<Implementor>(&Enumerable::map), "map" },
                { method<Implementor>(&Enumerable::reject), "reject" },
                { method<Implementor>(&Enumerable::select), "find_all" },
                { method<Implementor>(&Enumerable::select), "select" },
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
