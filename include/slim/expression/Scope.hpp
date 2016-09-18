#pragma once
#include "../types/Object.hpp"
#include "../types/Nil.hpp"
#include "../types/Symbol.hpp"
#include "../types/ViewModel.hpp"
#include "../Function.hpp"
#include <string>
#include <unordered_map>

namespace slim { namespace expr
{
    /**Variable scope. */
    class Scope
    {
    public:
        typedef std::unordered_map<SymPtr, ObjectPtr, ObjHash, ObjEquals> Map;

        /**Constructs the root scope, with no variables except "self".*/
        explicit Scope(ViewModelPtr self)
            : parent(nullptr), _self(self), map()
        {
            map[symbol("self")] = _self;
        }
        /**Constructs an inner scope (such as the one created by a "{|params| expr}" block.
         * The new scope includes all of the variables from the parent, but new variables are not
         * added to the parent.
         */
        explicit Scope(Scope &parent)
            : parent(&parent), _self(parent._self), map() {}

        /**Sets the value of a variable.
         * Note that currently this is only used for block "|params|", and will never set a
         * variable on the parent, even if it was already defined.
         */
        void set(const SymPtr &name, ObjectPtr val)
        {
            map[name] = val;
        }
        void set(const std::string &name, ObjectPtr val)
        {
            set(symbol(name), val);
        }
        /**Gets a variable from this or any parent scope.*/
        ObjectPtr get(const SymPtr &name)
        {
            auto it = map.find(name);
            if (it !=  map.end()) return it->second;
            else if (parent) return parent->get(name);
            else
            {
                //Since the script parser uses the existance of variables to call a method if a
                //variable does not exist, this should really never happen.
                throw std::runtime_error("Attempted to access an undefined variable");
            }
        }

        /**Get the "self" variable. */
        ViewModelPtr self() { return _self; }

        /**Begin iterator for all variables in this scope (not including parents).
         * Used for tests and debugging.
         */
        Map::iterator begin() { return map.begin(); }
        /**End iterator. See begin.*/
        Map::iterator end() { return map.end(); }
    private:
        Scope *parent;
        ViewModelPtr _self;
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

