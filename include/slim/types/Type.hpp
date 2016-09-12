#pragma once
#include "Object.hpp"
#include "Symbol.hpp"
namespace slim
{
    /**Class/Module type object helper.*/
    class Type : public Object
    {
    public:
        ObjectPtr get_constant(SymPtr name)
        {
            auto it = constants.find(name);
            if (it != constants.end()) return it->second;
            else return Object::get_constant(name);
        }
    protected:
        ObjectMap constants;
    };
}
