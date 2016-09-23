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
    namespace detail
    {
        /**Used to determine some aspects of a member function pointer.*/
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
        template<class Func, class... Args>
        static ObjectPtr do_call_void_ret(Func func, std::true_type, Args &&... args)
        {
            func(std::forward<Args>(args)...);
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
        template<class Func, class... Args>
        static ObjectPtr do_call_void_ret(Func func, std::false_type, Args &&... args)
        {
            return func(std::forward<Args>(args)...);
        }
 
        /**Call a function with the correct self (C++ this) type given already correct arguments.*/
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
        
        /**Call a static/global function with correct arguments, but unconverted return type.*/
        template<class RetType, class... Args>
        ObjectPtr do_static_call(RetType(*func)(Args...), Args &&... args)
        {
            return do_call_void_ret(func,
                std::is_void<RetType>::type(),
                std::forward<Args>(args)...);
        }

        /**Implementation detail of call with fixed types.
         * Has a matching pack of argument types and indices for calling func.
         */
        template<class... Args, class Func, size_t ...indices>
        ObjectPtr do_call_typed(Object *self, Func func, const FunctionArgs &args, Indices<indices...>)
        {
            return do_call(self, func, slim::unpack_arg<Args>(args[indices])...);
        }
        template<class... Args, class Func, size_t ...indices>
        ObjectPtr do_call_static_typed(Func func, const FunctionArgs &args, Indices<indices...>)
        {
            return do_static_call(func, slim::unpack_arg<Args>(args[indices])...);
        }

        /**Call a varargs member method.*/
        template<class RetType, class SelfType>
        ObjectPtr call(Object *self, RetType(SelfType::*func)(const FunctionArgs &args), const FunctionArgs &args)
        {
            return do_call(self, func, args);
        }
        /**Call func by converting each element of args to the appropriate type.*/
        template<class RetType, class SelfType, class... Args>
        ObjectPtr call(Object *self, RetType(SelfType::*func)(Args...), const FunctionArgs &args)
        {
            if (args.size() != sizeof...(Args))
            {
                throw ArgumentCountError(args.size(), sizeof...(Args), sizeof...(Args));
            }
            return do_call_typed<Args...>(self, func, args, BuildIndices<sizeof...(Args)>{});
        }
        /**Call a static/global varargs function.*/
        template<class RetType>
        ObjectPtr call(Object*, RetType(*func)(const FunctionArgs &args), const FunctionArgs &args)
        {
            return do_call(func, args);
        }
        /**Call a static/global typed args function.*/
        template<class RetType, class... Args>
        ObjectPtr call_static(Object *, RetType(*func)(Args...), const FunctionArgs &args)
        {
            if (args.size() != sizeof...(Args))
            {
                throw ArgumentCountError(args.size(), sizeof...(Args), sizeof...(Args));
            }
            return do_call_static_typed<Args...>(func, args, BuildIndices<sizeof...(Args)>{});
        }


        /**Storage for all member method pointers, but with an incorrect self type and parameter list.*/
        typedef ObjectPtr(Object::*RawMethod)(const FunctionArgs &args);
        /**Storage for all global/static method pointers, but an incorrect parameter list and return type.*/
        typedef ObjectPtr(*RawFunc)(const FunctionArgs &args);
        /**Storage for all member variable pointers, but with an incorrect self and result type.*/
        typedef int Object::*RawProp;

        union RawMember
        {
            RawFunc func;
            RawMethod method;
            RawProp prop;

            RawMember() {}
            RawMember(RawFunc func) : func(func) {}
            RawMember(RawMethod method) : method(method) {}
            RawMember(RawProp prop) : prop(prop) {}
        };
        
        /**Call a method with the correct member function pointer type.
         * The function signature does not include the template type, allowing a pointer to
         * instantiations to be stored in the non-template Method class.
         */
        template<class Func>
        ObjectPtr wrapped_call(Object *self, RawMember func, const FunctionArgs &args)
        {
            static_assert(sizeof(Func) == sizeof(RawMethod), "Cast assumes RawMethod was the correct storage size.");
            return call(self, (Func)func.method, args);
        }
        template<class Func>
        ObjectPtr wrapped_static_call(Object *self, RawMember func, const FunctionArgs &args)
        {
            static_assert(sizeof(Func) == sizeof(RawFunc), "Cast assumes RawMethod was the correct storage size.");
            return call_static(self, (Func)func.func, args);
        }
    }

    /**A member method of a script type.*/
    class Method
    {
    public:
        typedef ObjectPtr(*Caller)(Object *, detail::RawMember, const FunctionArgs &);

        Method() {}

        Method(detail::RawMember raw, Caller caller, const SymPtr &name)
            : raw(raw), caller(caller), _name(name)
        {}

        template<class Ret, class Self, class...Args>
        Method(Ret(Self::*method)(Args...), const SymPtr &name)
            : raw((detail::RawMethod)method)
            , caller(&detail::wrapped_call<decltype(method)>)
            , _name(name)
        {}
        template<class Ret, class...Args>
        Method(Ret(*func)(Args...), const SymPtr &name)
            : raw((detail::RawFunc)func)
            , caller(&detail::wrapped_static_call<decltype(func)>)
            , _name(name)
        {}
        template<class Func>
        Method(Func func, const std::string &name)
            : Method(func, symbol(name))
        {}

        template<class Self, class T>
        static Method getter(T Self::*prop, const SymPtr &name)
        {

            auto raw  = (detail::RawProp)prop;
            Caller caller = [](Object *self, detail::RawMember raw, const FunctionArgs &args) -> ObjectPtr
            {
                if (!args.empty()) throw ArgumentCountError(args.size(), 0, 0);
                auto prop = (T Self::*)raw.prop;
                return make_value(((Self*)self)->*prop);
            };

            return Method(raw, caller, name);
        }
        template<class Self, class T>
        static Method getter(T Self::*prop, const std::string &name)
        {
            return getter(prop, symbol(name));
        }

        /**Call the method.
         * @param self The this pointer for the method call. It is assumed that this is of the correct
         * type for the provided function pointer.
         * @param args The list of method arguments for the call, which may be any of the script
         * types and may not be correct for the call. The method checks the number of types and
         * throws an exception if they are wrong. Arguments may not be nullptr, use NIL_VALUE.
         *
         * If the underlying function has specific types rather than FunctionArgs, then args is 
         * converted in a manner similar to slim::unpack, calling try_unpack_arg for each one with
         * ADL.
         *
         * @return The object returned by the method call.
         */
        ObjectPtr operator()(Object *self, const FunctionArgs &args)const
        {
            return caller(self, raw, args);
        }

        /**The name of the method, as referenced by scripts.*/
        const SymPtr &name()const { return _name; }
    private:
        detail::RawMember raw;
        Caller caller;
        SymPtr _name;
    };

    /**The table of named methods for a type.*/
    class MethodTable
    {
    public:
        typedef std::unordered_map<SymPtr, Method, ObjHash, ObjEquals> Map;

        /**Constructs an empty method table.*/
        MethodTable() : map() {}

        /**Constructs a method table using the methods in the initializer_list.*/
        MethodTable(std::initializer_list<Method> methods)
        {
            for (auto &f : methods) add(f);
        }
        /**Duplicates an existing method table, then adds additional methods to it.
         * Useful when extending types.
         */
        MethodTable(const MethodTable &table, std::initializer_list<Method> methods)
            : map(table.map)
        {
            for (auto &f : methods) add(f);
        }

        /**Adds a method to the table. Overrwrites any existing method with that name.*/
        MethodTable& add(const Method &func)
        {
            map[func.name()] = func;
            return *this;
        }
        /**Adds a list of methods using add.*/
        template<class T>
        MethodTable& add_all(const T &container)
        {
            for (const Method &method : container) add(method);
            return *this;
        }
        MethodTable& add_all(std::initializer_list<Method> methods)
        {
            for (auto &f : methods) add(f);
            return *this;
        }
        /**Find a method. Returns nullptr if the method is not found.*/
        const Method *find(SymPtr name)const
        {
            auto it = map.find(name);
            return it != map.end() ? &it->second : nullptr;
        }
    private:
        Map map;
    };
}
