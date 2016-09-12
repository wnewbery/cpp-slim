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
namespace slim
{
    class Object;
    typedef std::shared_ptr<Object> ObjectPtr;
    typedef std::function<ObjectPtr(Object *self, const FunctionArgs &args)> Method2;

    class Method
    {
    public:
        Method2 f;
        SymPtr name;

        Method(Method2 f, const std::string &name_str) : f(f), name(symbol(name_str)) {}

        template<class T, class U>
        Method(U(T::*f2)(const FunctionArgs &args)const, const std::string &name_str)
            : f(), name(symbol(name_str))
        {
            f = [f2](Object *self, const FunctionArgs &args) -> ObjectPtr
            {
                assert(dynamic_cast<T*>(self) == static_cast<T*>(self));
                return do_call(self, f2, args);
            };
        }
        template<class T, class U>
        Method(U(T::*f2)()const, const std::string &name_str)
            : f(), name(symbol(name_str))
        {
            auto name = this->name;
            f = [f2, name](Object *self, const FunctionArgs &args) -> ObjectPtr
            {
                assert(dynamic_cast<T*>(self) == static_cast<T*>(self));
                if (!args.empty()) throw InvalidArgument(self, name->str());
                return do_call(self, f2);
            };
        }
        //varargs
        template<class T, class U>
        Method(U(T::*f2)(const FunctionArgs &args), const std::string &name_str)
            : f(), name(symbol(name_str))
        {
            f = [f2](Object *self, const FunctionArgs &args) -> ObjectPtr
            {
                assert(dynamic_cast<T*>(self) == static_cast<T*>(self));
                return do_call(self, f2, args);
            };
        }
        //no args
        template<class T, class U>
        Method(U(T::*f2)(), const std::string &name_str)
            : f(), name(symbol(name_str))
        {
            auto name = this->name;
            f = [f2, name](Object *self, const FunctionArgs &args) -> ObjectPtr
            {
                assert(dynamic_cast<T*>(self) == static_cast<T*>(self));
                if (!args.empty()) throw InvalidArgument(self, name->str());
                return do_call(self, f2);
            };
        }
        //1 arg
        template<class T, class U, class ARG1>
        Method(U(T::*f2)(ARG1 arg1), const std::string &name_str)
            : f(), name(symbol(name_str))
        {
            auto name = this->name;
            f = [f2, name](Object *self, const FunctionArgs &args) -> ObjectPtr
            {
                assert(dynamic_cast<T*>(self) == static_cast<T*>(self));
                if (args.size() != 1) throw InvalidArgument(self, name->str());
                auto arg1 = dynamic_cast<ARG1>(args[0].get());
                if (!arg1) throw InvalidArgument(name->str());
                return do_call(self, f2, arg1);
            };
        }
        //2 arg
        template<class T, class U, class ARG1, class ARG2>
        Method(U(T::*f2)(ARG1 arg1, ARG2 arg2), const std::string &name_str)
            : f(), name(symbol(name_str))
        {
            auto name = this->name;
            f = [f2, name](Object *self, const FunctionArgs &args) -> ObjectPtr
            {
                assert(dynamic_cast<T*>(self) == static_cast<T*>(self));
                if (args.size() != 2) throw InvalidArgument(self, name->str());
                auto arg1 = dynamic_cast<ARG1>(args[0].get());
                auto arg2 = dynamic_cast<ARG2>(args[1].get());
                if (!arg1 || !arg2) throw InvalidArgument(name->str());
                return do_call(self, f2, arg1, arg2);
            };
        }

        ObjectPtr operator()(Object *self, const FunctionArgs &args)const
        {
            return f(self, args);
        }

    private:
        /**Helper for do_call to determine the self (C++ this) type and void returns.*/
        template<class T> struct FunctionTraits;
        template<class RetType, class SelfType, class... Args>
        struct FunctionTraits<RetType(SelfType::*)(Args...)>
        {
            typedef RetType result_type;
            typedef SelfType self_type;
            typedef std::is_void<result_type> is_void_result;
        };

        /**Call the member function pointer with the correct self type, and convert a void return
         * to "nil".
         */
        template<class Func, class... Args>
        static ObjectPtr do_call(Object *self, Func func, Args &&... args)
        {
            typedef FunctionTraits<Func> Traits;
            auto self2 = static_cast<Traits::self_type*>(self);
            return do_call2(self2, func, Traits::is_void_result(), std::forward<Args>(args)...);
        }


        template<class SelfType, class Func, class... Args>
        static ObjectPtr do_call2(SelfType *self, Func func, std::true_type, Args &&... args)
        {
            (self->*func)(std::forward<Args>(args)...);
            return NIL_VALUE;
        }
        template<class SelfType, class Func, class... Args>
        static ObjectPtr do_call2(SelfType *self, Func func, std::false_type, Args &&... args)
        {
            return (self->*func)(std::forward<Args>(args)...);
        }
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
