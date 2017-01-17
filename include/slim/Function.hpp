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

        /**Converts a function return value to a Slim object.*/
        template<class T> Ptr<T> convert_return_type(Ptr<T> &&ptr)
        {
            return std::move(ptr);
        }
        template<class T> Ptr<T> convert_return_type(Ptr<T> &ptr)
        {
            return ptr;
        }
        template<class T> auto convert_return_type(T &&ret)
        {
            return make_value(std::move(ret));
        }
        template<class T> auto convert_return_type(T &ret)
        {
            return make_value(ret);
        }

        /**Implementation detail of call with fixed types.
         * Has a matching pack of argument types and indices for calling func.
         */
        template<class... Args, class SelfType, size_t ...indices>
        ObjectPtr call_unpacked(Object *self, void(SelfType::*func)(Args...), const FunctionArgs &args, Indices<indices...>)
        {
            (static_cast<SelfType*>(self)->*func)(slim::unpack_arg<Args>(args[indices])...);
            return NIL_VALUE;
        }
        template<class... Args, class RetType, class SelfType, size_t ...indices>
        ObjectPtr call_unpacked(Object *self, RetType(SelfType::*func)(Args...), const FunctionArgs &args, Indices<indices...>)
        {
            return convert_return_type(
                (static_cast<SelfType*>(self)->*func)
                (slim::unpack_arg<std::remove_const<std::remove_reference<Args>::type>::type>(args[indices])...));
        }
        /**Call a varargs member method.*/
        template<class SelfType>
        ObjectPtr call(Object *self, void(SelfType::*func)(const FunctionArgs &args), const FunctionArgs &args)
        {
            (static_cast<SelfType*>(self)->*func)(args);
            return NIL_VALUE;
        }
        template<class RetType, class SelfType>
        ObjectPtr call(Object *self, RetType(SelfType::*func)(const FunctionArgs &args), const FunctionArgs &args)
        {
            return convert_return_type((static_cast<SelfType*>(self)->*func)(args));
        }
        /**Call func by converting each element of args to the appropriate type.*/
        template<class RetType, class SelfType, class... Args>
        ObjectPtr call(Object *self, RetType(SelfType::*func)(Args...), const FunctionArgs &args)
        {
            if (args.size() != sizeof...(Args))
            {
                throw ArgumentCountError(args.size(), sizeof...(Args), sizeof...(Args));
            }
            return call_unpacked<Args...>(self, func, args, BuildIndices<sizeof...(Args)>{});
        }

        template<class... Args, class SelfType, size_t ...indices>
        ObjectPtr free_call_unpacked(Object *self, void(*func)(SelfType *self, Args...), const FunctionArgs &args, Indices<indices...>)
        {
            func(static_cast<SelfType*>(self), slim::unpack_arg<Args>(args[indices])...);
            return NIL_VALUE;
        }
        template<class... Args, class RetType, class SelfType, size_t ...indices>
        ObjectPtr free_call_unpacked(Object *self, RetType(*func)(SelfType *self, Args...), const FunctionArgs &args, Indices<indices...>)
        {
            return convert_return_type(func(static_cast<SelfType*>(self),
                    slim::unpack_arg<std::remove_const<std::remove_reference<Args>::type>::type>(args[indices])...));
        }
        /**Call a varargs member method.*/
        template<class SelfType>
        ObjectPtr free_call(Object *self, void(*func)(SelfType *self, const FunctionArgs &args), const FunctionArgs &args)
        {
            free_call(static_cast<SelfType*>(self), args);
            return NIL_VALUE;
        }
        template<class RetType, class SelfType>
        ObjectPtr free_call(Object *self, RetType(*func)(SelfType *self, const FunctionArgs &args), const FunctionArgs &args)
        {
            return convert_return_type(func(static_cast<SelfType*>(self), args));
        }
        /**Call func by converting each element of args to the appropriate type.*/
        template<class RetType, class SelfType, class... Args>
        ObjectPtr free_call(Object *self, RetType(*func)(SelfType *self, Args...), const FunctionArgs &args)
        {
            if (args.size() != sizeof...(Args))
            {
                throw ArgumentCountError(args.size(), sizeof...(Args), sizeof...(Args));
            }
            return free_call_unpacked<Args...>(self, func, args, BuildIndices<sizeof...(Args)>{});
        }

        template<class... Args, size_t ...indices>
        ObjectPtr call_static_unpacked(void(*func)(Args...), const FunctionArgs &args, Indices<indices...>)
        {
            func(slim::unpack_arg<Args>(args[indices])...);
            return NIL_VALUE;
        }
        template<class... Args, class RetType, size_t ...indices>
        ObjectPtr call_static_unpacked(RetType(*func)(Args...), const FunctionArgs &args, Indices<indices...>)
        {
            return convert_return_type(func(slim::unpack_arg<Args>(args[indices])...));
        }
        /**Call a static/global function.*/
        inline ObjectPtr call_static(Object*, void(*func)(const FunctionArgs &args), const FunctionArgs &args)
        {
            func(args);
            return NIL_VALUE;
        }
        template<class RetType>
        ObjectPtr call_static(Object*, RetType(*func)(const FunctionArgs &args), const FunctionArgs &args)
        {
            return convert_return_type(func(args));
        }
        template<class RetType, class... Args>
        ObjectPtr call_static(Object *, RetType(*func)(Args...), const FunctionArgs &args)
        {
            if (args.size() != sizeof...(Args))
            {
                throw ArgumentCountError(args.size(), sizeof...(Args), sizeof...(Args));
            }
            return call_static_unpacked<Args...>(func, args, BuildIndices<sizeof...(Args)>{});
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
        ObjectPtr wrapped_free_call(Object *self, RawMember func, const FunctionArgs &args)
        {
            static_assert(sizeof(Func) == sizeof(RawFunc), "Cast assumes RawFunc was the correct storage size.");
            return free_call(self, (Func)func.func, args);
        }
        template<class Func>
        ObjectPtr wrapped_static_call(Object *self, RawMember func, const FunctionArgs &args)
        {
            static_assert(sizeof(Func) == sizeof(RawFunc), "Cast assumes RawFunc was the correct storage size.");
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

        // Member
        template<class Ret, class Self, class...Args>
        Method(Ret(Self::*method)(Args...), const SymPtr &name)
            : raw((detail::RawMethod)method)
            , caller(&detail::wrapped_call<decltype(method)>)
            , _name(name)
        {}
        template<class Ret, class Self, class...Args>
        Method(Ret(Self::*method)(Args...)const, const SymPtr &name)
            : raw((detail::RawMethod)method)
            , caller(&detail::wrapped_call<Ret(Self::*)(Args...)>)
            , _name(name)
        {}

        // Static
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

        // Free
        //template<class Ret, class...Args>
        //Method(Ret(*func)(Args...), const SymPtr &name)
        //    : raw((detail::RawFunc)func)
        //    , caller(&detail::wrapped_static_call<decltype(func)>)
        //    , _name(name)
        //{}
        template<class Func>
        static Method free(Func func, const SymPtr &name)
        {
            auto raw = (detail::RawFunc)func;
            return Method(raw, detail::wrapped_free_call<Func>, name);
        }
        template<class Func>
        static Method free(Func func, const std::string &name)
        {
            return free(func, symbol(name));
        }

        template<class Self, class T>
        static Method getter(T Self::*prop, const SymPtr &name)
        {

            auto raw  = (detail::RawProp)prop;
            Caller caller = [](Object *self, detail::RawMember raw, const FunctionArgs &args) -> ObjectPtr
            {
                if (!args.empty()) throw ArgumentCountError(args.size(), 0, 0);
                auto prop = (T Self::*)raw.prop;
                auto &tmp = ((Self*)self)->*prop;
                return detail::convert_return_type(tmp);
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
