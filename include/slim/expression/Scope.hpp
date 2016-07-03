#pragma once
#include "types/Object.hpp"
#include "types/Nil.hpp"
#include <string>
#include <unordered_map>

namespace slim { namespace expr
{
    class Scope
    {
    public:
        typedef std::unordered_map<std::string, ObjectPtr> Map;

        Scope() : parent(nullptr), map() {}
        Scope(Scope &parent) : parent(&parent), map() {}

        void set(const std::string &name, ObjectPtr val)
        {
            map[name] = val;
        }
        ObjectPtr get(const std::string &name)
        {
            auto it = map.find(name);
            if (it !=  map.end()) return it->second;
            else if (parent) return parent->get(name);
            else return NIL_VALUE;
        }
    private:
        Scope *parent;
        Map map;
    };
}}

