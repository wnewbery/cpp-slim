#pragma once
#include "types/Object.hpp"
#include "types/Nil.hpp"
#include "types/Symbol.hpp"
#include <string>
#include <unordered_map>

namespace slim { namespace expr
{
    class Scope
    {
    public:
        typedef std::unordered_map<SymPtr, ObjectPtr, ObjHash, ObjEquals> Map;

        Scope() : parent(nullptr), map() {}
        Scope(Scope &parent) : parent(&parent), map() {}

        void set(const SymPtr &name, ObjectPtr val)
        {
            map[name] = val;
        }
        void set(const std::string &name, ObjectPtr val)
        {
            set(symbol(name), val);
        }
        ObjectPtr get(const SymPtr &name)
        {
            auto it = map.find(name);
            if (it !=  map.end()) return it->second;
            else if (parent) return parent->get(name);
            else return NIL_VALUE;
        }
        ObjectPtr get(const std::string &str)
        {
            return symbol(str);
        }
    private:
        Scope *parent;
        Map map;
    };
}}

