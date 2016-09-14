#pragma once
#include "../types/Object.hpp"
#include "../types/Nil.hpp"
#include "../types/Symbol.hpp"
#include "../Function.hpp"
#include <string>
#include <unordered_map>

namespace slim { namespace expr
{
    /**Attributes on the in-scope "self" variable.*/
    class ScopeAttributes
    {
    public:
        typedef std::unordered_map<SymPtr, ObjectPtr, ObjHash, ObjEquals> Map;

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
            return it != map.end() ? it->second : NIL_VALUE;
        }
    private:
        Map map;
    };
    /**Variable scope. */
    class Scope
    {
    public:
        typedef std::unordered_map<SymPtr, ObjectPtr, ObjHash, ObjEquals> Map;

        explicit Scope(ScopeAttributes &attrs)
            : attrs(attrs), parent(nullptr), map()
        {
            map[symbol("self")] = NIL_VALUE;
        }
        explicit Scope(Scope &parent)
            : attrs(parent.attrs), parent(&parent), map() {}

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
        /**Get the "self" variable.
         * In most situations, "self" should be set to a non-nil value.
         */
        ObjectPtr self()
        {
            static auto sym = symbol("self");
            auto it = map.find(sym);
            if (it != map.end()) return it->second;
            else
            {
                assert(parent);
                return parent->self();
            }
        }
        ObjectPtr get_attr(const SymPtr &name)
        {
            return attrs.get(name);
        }

        Map::iterator begin() { return map.begin(); }
        Map::iterator end() { return map.end(); }
    private:
        ScopeAttributes &attrs;
        Scope *parent;
        Map map;
    };
    /**Local variable symbol names in scope. Used to tell at parse time if a symbol refers
     * to a local variable or a global/self method.
     * 
     * Unlike the Scope object this is used at parse time, and so no values are known.
     * 
     * Note that the variable "self" is always defined (as variable 0).
     */
    class LocalVarNames
    {
    public:
        std::vector<std::string> names;
        LocalVarNames()
        {
            names.push_back("self");
        }

        void add(const std::string &name)
        {
            if (!is_var(name)) names.push_back(name);
        }
        bool is_var(const std::string &name)
        {
            for (auto &x : names) if (x == name) return true;
            return false;
        }
    };
}}

