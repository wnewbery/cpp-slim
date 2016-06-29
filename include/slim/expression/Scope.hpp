#pragma once
#include "types/Object.hpp"
#include "types/Null.hpp"
#include <string>
#include <unordered_map>

namespace slim { namespace expr
{
    class Scope
    {
    public:
        typedef std::unordered_map<std::string, ObjectPtr> Map;

        void set(const std::string &name, ObjectPtr val)
        {
            map[name] = val;
        }
        ObjectPtr get(const std::string &name)
        {
            auto it = map.find(name);
            if (it !=  map.end()) return it->second;
            else return NULL_VALUE;
        }
    private:
        Map map;
    };
}}

