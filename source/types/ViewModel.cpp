#include "types/ViewModel.hpp"
#include "types/Nil.hpp"
#include "types/Symbol.hpp"

namespace slim
{
    const std::string ViewModel::TYPE_NAME = "ViewModel";

    ViewModel::ViewModel() {}
    ViewModel::~ViewModel() {}


    ObjectPtr ViewModel::get_constant(SymPtr name)
    {
        auto it = constants.find(name);
        if (it != constants.end()) return it->second;
        else throw NoSuchConstant(this, name);
    }

    void ViewModel::add_constant(SymPtr name, ObjectPtr constant)
    {
        constants[name] = constant;
    }
    void ViewModel::add_constant(const std::string &name, ObjectPtr constant)
    {
        add_constant(symbol(name), constant);
    }


    ObjectPtr ViewModel::get_attr(SymPtr name)
    {
        auto it = attrs.find(name);
        return it != attrs.end() ? it->second : NIL_VALUE;
    }

    void ViewModel::set_attr(SymPtr name, ObjectPtr value)
    {
        attrs[name] = value;
    }
    void ViewModel::set_attr(const std::string &name, ObjectPtr value)
    {
        set_attr(symbol(name), value);
    }
}
