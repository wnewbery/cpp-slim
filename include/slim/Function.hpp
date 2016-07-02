#pragma once
#include <functional>
#include <vector>
#include <unordered_map>
#include "Error.hpp"
namespace slim
{
    class Object;
    typedef std::shared_ptr<Object> ObjectPtr;
    typedef std::vector<ObjectPtr> FunctionArgs;
    typedef std::function<ObjectPtr(const FunctionArgs&)> Function2;
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

    class FunctionTable
    {
    public:
        typedef std::unordered_map<std::string, Function> Map;

        FunctionTable() : map() {}
        FunctionTable(std::initializer_list<Function> functions)
        {
            for (auto &f : functions) add(f);
        }
        FunctionTable(const FunctionTable &table, std::initializer_list<Function> functions)
            : map(table.map)
        {
            for (auto &f : functions) add(f);
        }

        void add(const Function &func)
        {
            bool added = map.emplace(func.name, func).second;
            if (!added) throw DuplicateMethod(func.name);
        }
        const Function *find(const std::string &name)const
        {
            auto it = map.find(name);
            return it != map.end() ? &it->second : nullptr;
        }
        const Function& get(const std::string &name)const
        {
            auto f = find(name);
            if (f) return *f;
            else throw NoSuchMethod(name);
        }
    private:
        Map map;
    };
}
