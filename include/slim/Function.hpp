#pragma once
#include <functional>
#include <vector>
#include <unordered_map>
#include <memory>
#include <cassert>
#include "Error.hpp"
#include "types/Object.hpp"
#include "types/Symbol.hpp"
namespace slim
{
    class Object;
    typedef std::shared_ptr<Object> ObjectPtr;
    typedef std::function<ObjectPtr(const FunctionArgs &args)> Function2;
    typedef std::function<ObjectPtr(Object *self, const FunctionArgs &args)> Method2;
    class Function
    {
    public:
        Function2 f;
        SymPtr name;


        Function(Function2 f, const std::string &name) : f(f), name(symbol(name)) {}

        //1 arg
        template<class RET, class ARG1>
        Function(RET(*f2)(ARG1 arg1), const std::string &name_str) : f(), name(symbol(name_str))
        {
            auto name = this->name;
            f = [f2, name](const FunctionArgs &args) -> ObjectPtr
            {
                if (args.size() != 1) throw InvalidArgument(name->str());
                auto arg1 = dynamic_cast<ARG1>(args[0].get());
                if (!arg1) throw InvalidArgument(name->str());
                return f2(arg1);
            };
        }
        //2 arg
        template<class RET, class ARG1, class ARG2>
        Function(RET(*f2)(ARG1 arg1, ARG2 arg2), const std::string &name_str) : f(), name(symbol(name_str))
        {
            auto name = this->name;
            f = [f2, name](const FunctionArgs &args) -> ObjectPtr
            {
                if (args.size() != 2) throw InvalidArgument(name->str());
                auto arg1 = dynamic_cast<ARG1>(args[0].get());
                auto arg2 = dynamic_cast<ARG2>(args[1].get());
                if (!arg1 || !arg2) throw InvalidArgument(name->str());
                return f2(arg1, arg2);
            };
        }

        ObjectPtr operator()(const FunctionArgs &args)const
        {
            return f(args);
        }
    };
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
                return (static_cast<T*>(self)->*f2)(args);
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
                return (static_cast<T*>(self)->*f2)();
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
                return (static_cast<T*>(self)->*f2)(args);
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
                return (static_cast<T*>(self)->*f2)();
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
                return (static_cast<T*>(self)->*f2)(arg1);
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
                return (static_cast<T*>(self)->*f2)(arg1, arg2);
            };
        }

        ObjectPtr operator()(Object *self, const FunctionArgs &args)const
        {
            return f(self, args);
        }
    };

    template <class T> class BaseFunctionTable
    {
    public:
        typedef std::unordered_map<SymPtr, T, ObjHash, ObjEquals> Map;

        BaseFunctionTable() : map() {}
        BaseFunctionTable(std::initializer_list<T> functions)
        {
            for (auto &f : functions) add(f);
        }
        BaseFunctionTable(const BaseFunctionTable<T> &table, std::initializer_list<T> functions)
            : map(table.map)
        {
            for (auto &f : functions) add(f);
        }

        void add(const T &func)
        {
            map.emplace(func.name, func);
        }
        const T *find(SymPtr name)const
        {
            auto it = map.find(name);
            return it != map.end() ? &it->second : nullptr;
        }
        const T& get(SymPtr name)const
        {
            auto f = find(name);
            if (f) return *f;
            else throw NoSuchMethod(name.get());
        }
    private:
        Map map;
    };
    typedef BaseFunctionTable<Function> FunctionTable;
    class MethodTable : public BaseFunctionTable<Method>
    {
    public:
        using BaseFunctionTable<Method>::BaseFunctionTable;
        const Method& get(const Object *self, SymPtr name)const
        {
            auto f = find(name);
            if (f) return *f;
            else throw NoSuchMethod(self, name.get());
        }
    };
}
