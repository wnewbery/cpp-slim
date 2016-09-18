#pragma once
#include "Object.hpp"
#include "Symbol.hpp"
#include "Error.hpp"
namespace slim
{
    /**Class/Module type object helper.*/
    class Type : public Object
    {
    public:
        virtual std::string inspect()const override
        {
            return type_name();
        }

        virtual ObjectPtr get_constant(SymPtr name)override
        {
            auto it = constants.find(name);
            if (it != constants.end()) return it->second;
            else throw NoConstantError(this, name);
        }

    protected:
        ObjectMap constants;
    };
}
