#pragma once
#include <functional>
#include <vector>
#include <unordered_map>
#include <memory>
#include <cassert>
#include "Error.hpp"
namespace slim
{
    class Object;
    typedef std::shared_ptr<Object> ObjectPtr;
    typedef std::vector<ObjectPtr> FunctionArgs;
    typedef std::function<ObjectPtr(const FunctionArgs &args)> Function2;
    typedef std::function<ObjectPtr(Object *self, const FunctionArgs &args)> Method2;
    class Function
    {
    public:
        Function2 f;
        std::string name;

        ObjectPtr operator()(const FunctionArgs &args)const
        {
            return f(args);
        }
    };
    class Method
    {
    public:
        Method2 f;
        std::string name;

        Method(Method2 f, const std::string &name) : f(f), name(name) {}

        template<class T, class U>
        Method(U(T::*f2)(const FunctionArgs &args), const std::string &name)
            : f(), name(name)
        {
            f = [f2](Object *self, const FunctionArgs &args) -> ObjectPtr
            {
                assert(dynamic_cast<T*>(self) == static_cast<T*>(self));
                return (static_cast<T*>(self)->*f2)(args);
            };
        }
        template<class T, class U>
        Method(U(T::*f2)(), const std::string &name)
            : f(), name(name)
        {
            f = [f2, name](Object *self, const FunctionArgs &args) -> ObjectPtr
            {
                assert(dynamic_cast<T*>(self) == static_cast<T*>(self));
                if (!args.empty()) throw InvalidArgument(self, name);
                return (static_cast<T*>(self)->*f2)();
            };
        }
        template<class T, class U>
        Method(U(T::*f2)(const FunctionArgs &args)const, const std::string &name)
            : f(), name(name)
        {
            f = [f2](Object *self, const FunctionArgs &args) -> ObjectPtr
            {
                assert(dynamic_cast<T*>(self) == static_cast<T*>(self));
                return (static_cast<T*>(self)->*f2)(args);
            };
        }
        template<class T, class U>
        Method(U(T::*f2)()const, const std::string &name)
            : f(), name(name)
        {
            f = [f2, name](Object *self, const FunctionArgs &args) -> ObjectPtr
            {
                assert(dynamic_cast<T*>(self) == static_cast<T*>(self));
                if (!args.empty()) throw InvalidArgument(self, name);
                return (static_cast<T*>(self)->*f2)();
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
        typedef std::unordered_map<std::string, T> Map;

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
        const T *find(const std::string &name)const
        {
            auto it = map.find(name);
            return it != map.end() ? &it->second : nullptr;
        }
        const T& get(const std::string &name)const
        {
            auto f = find(name);
            if (f) return *f;
            else throw NoSuchMethod(name);
        }
    private:
        Map map;
    };
    typedef BaseFunctionTable<Function> FunctionTable;
    class MethodTable : public BaseFunctionTable<Method>
    {
    public:
        using BaseFunctionTable<Method>::BaseFunctionTable;
        const Method& get(const Object *self, const std::string &name)const
        {
            auto f = find(name);
            if (f) return *f;
            else throw NoSuchMethod(self, name);
        }
    };
}
