#pragma once
#include <functional>
#include <vector>
#include <unordered_map>
#include <memory>
#include <cassert>
#include "Error.hpp"
#include "types/Object.hpp"
#include "types/Nil.hpp"
#include "types/Symbol.hpp"
#include "FunctionHelpers.hpp"
namespace slim
{
    class Object;
    typedef std::shared_ptr<Object> ObjectPtr;
    typedef std::function<ObjectPtr(Object *self, const FunctionArgs &args)> Method2;

    typedef ObjectPtr(*RawFunction)(Object *self, const FunctionArgs &args);
    namespace detail
    {
        template<class T> struct MethodTraits;
        template<class RetType, class SelfType, class... Args>
        struct MethodTraits<RetType(SelfType::*)(Args...)>
        {
            typedef RetType result_type;
            typedef SelfType self_type;
            typedef std::is_void<result_type> is_void_result;
            static constexpr size_t arg_count = sizeof...(Args);
        };

        //Want to expand the functions arguments along with the dynamic FunctionArgs array while converting
        //each type.
        /**A varadic type that takes a pack of size_t indices as a parameter pack.*/
        template<size_t...> struct Indices {};
        /**Recursively build a parameter pack from 0 to N - 1.
         * Fully resolves to a type derived from Indices containing those values as template parameters.
         * 
         * e.g.
         * BuildIndices<3> : BuildIndices<2, 2>
         * BuildIndices<2, 2> : BuildIndices<1, 1, 2>
         * BuildIndices<1, 1, 2> : BuildIndices<0, 0, 1, 2>
         * BuildIndices<0, 0, 1, 2> : Indices<0, 1, 2>
         */
        template<size_t N, size_t... Is> struct BuildIndices
            : BuildIndices<N - 1, N - 1, Is...> {};
        /**BuildIndices terminal case which is a Indices<>.*/
        template<size_t... Is> struct BuildIndices<0, Is...> : Indices<Is...> {};


        /**Call and handle void returns. For std::true_type return NIL_VALUE.*/
        template<class SelfType, class Func, class... Args>
        static ObjectPtr do_call_void_ret(SelfType *self, Func func, std::true_type, Args &&... args)
        {
            (self->*func)(std::forward<Args>(args)...);
            return NIL_VALUE;
        }
        /**Call and handle void returns. For std::false_type return result of func implicitly
         * converted to ObjectPtr.
         */
        template<class SelfType, class Func, class... Args>
        static ObjectPtr do_call_void_ret(SelfType *self, Func func, std::false_type, Args &&... args)
        {
            return (self->*func)(std::forward<Args>(args)...);
        }
        /**Call a function with the correct self (C++ this) type with already correct arguments.*/
        template<class Func, class... Args>
        ObjectPtr do_call(Object *self, Func func, Args &&... args)
        {
            typedef MethodTraits<Func> Traits;
            auto self_typed = static_cast<typename Traits::self_type*>(self);
            assert(self_typed == dynamic_cast<typename Traits::self_type*>(self));
            return do_call_void_ret(self_typed, func,
              typename Traits::is_void_result(),
              std::forward<Args>(args)...);
        }
        /**Call a varargs function.*/
        template<class Func> ObjectPtr call_varargs(Object *self, Func func, const FunctionArgs &args)
        {
            return do_call(self, func, args);
        }
        /**Implementation detail of call_typed. Has a matching pack of argument types and indices for them.*/
        template<class... Args, class Func, size_t ...indices>
        ObjectPtr do_call_typed(Object *self, Func func, const FunctionArgs &args, Indices<indices...>)
        {
            return do_call(self, func, slim::unpack_arg<Args>(args[indices])...);
        }
        /**Call func by converting each element of args to the appropriate type.*/
        template<class RetType, class SelfType, class... Args>
        ObjectPtr call_typed(Object *self, RetType(SelfType::*func)(Args...), const FunctionArgs &args)
        {
            if (args.size() != sizeof...(Args))
            {
                throw InvalidArgumentCount(args.size(), sizeof...(Args), sizeof...(Args));
            }
            return do_call_typed<Args...>(self, func, args, BuildIndices<sizeof...(Args)>{});
        }

        template<class RetType, class SelfType>
        ObjectPtr call(Object *self, RetType(SelfType::*func)(const FunctionArgs &args), const FunctionArgs &args)
        {
            return call_varargs(self, func, args);
        }
        template<class RetType, class SelfType, class... Args>
        ObjectPtr call(Object *self, RetType(SelfType::*func)(Args...), const FunctionArgs &args)
        {
            return call_typed(self, func, args);
        }

        typedef ObjectPtr(Object::*RawMethod)(const FunctionArgs &args);
        
        template<class Func>
        ObjectPtr wrapped_call(Object *self, RawMethod func, const FunctionArgs &args)
        {
            return call(self, (Func)func, args);
        }
    }

    class Method
    {
    public:
        const SymPtr name;

        template<class Func>
        Method(Func func, const SymPtr &name)
            : name(name)
            , method((detail::RawMethod)func)
            , caller(&detail::wrapped_call<Func>)
        {}
        template<class Func>
        Method(Func func, const std::string &name)
            : Method(func, symbol(name))
        {}

        ObjectPtr operator()(Object *self, const FunctionArgs &args)const
        {
            return caller(self, method, args);
        }
    private:
        detail::RawMethod method;
        ObjectPtr(*caller)(Object *, detail::RawMethod, const FunctionArgs &);
    };

    class MethodTable
    {
    public:
        typedef std::unordered_map<SymPtr, Method, ObjHash, ObjEquals> Map;

        MethodTable() : map() {}
        MethodTable(std::initializer_list<Method> functions)
        {
            for (auto &f : functions) add(f);
        }
        MethodTable(const MethodTable &table, std::initializer_list<Method> functions)
            : map(table.map)
        {
            for (auto &f : functions) add(f);
        }

        void add(const Method &func)
        {
            map.emplace(func.name, func);
        }
        const Method *find(SymPtr name)const
        {
            auto it = map.find(name);
            return it != map.end() ? &it->second : nullptr;
        }
        const Method& get(SymPtr name)const
        {
            auto f = find(name);
            if (f) return *f;
            else throw NoSuchMethod(name.get());
        }
    private:
        Map map;
    };
}
