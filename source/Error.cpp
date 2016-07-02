#include "Error.hpp"
#include "types/Object.hpp"
namespace slim
{
    TypeError::TypeError(const Object *type, const std::string &type_name)
        : ScriptError(type->type_name() + " can not be converted to " + type_name)
    {
    }
    UnorderableTypeError::UnorderableTypeError(const Object *lhs, const char *op, const Object *rhs)
        : TypeError("unorderable types: " + lhs->type_name() + " " + op + " " + rhs->type_name())
    {
    }
    UnsupportedOperandTypeError::UnsupportedOperandTypeError(const Object *lhs, const char *op, const Object *rhs)
        : TypeError("unsupported operand types: " + lhs->type_name() + " " + op + " " + rhs->type_name())
    {
    }
    UnsupportedOperandTypeError::UnsupportedOperandTypeError(const char *op, const Object *rhs)
        : TypeError("unsupported operand types: " + std::string(op) + " " + rhs->type_name())
    {
    }
    NoSuchMethod::NoSuchMethod(const Object * obj, const std::string & method_name)
        : ScriptError(obj->type_name() + " has no method " + method_name)
    {
    }
    NoSuchMethod::NoSuchMethod(const std::string & method_name)
        : ScriptError("No global function " + method_name)
    {
    }
    DuplicateMethod::DuplicateMethod(const Object * obj, const std::string & method_name)
        : ScriptError(obj->type_name() + " already has method " + method_name)
    {
    }
    DuplicateMethod::DuplicateMethod(const std::string & method_name)
        : ScriptError("global function " + method_name + " already exists")
    {
    }
    InvalidArgument::InvalidArgument(const std::string &name)
        : ScriptError("Invalid argument for " + name)
    {
    }
    InvalidArgument::InvalidArgument(const Object *obj, const std::string &name)
        : ScriptError("Invalid argument for " + obj->type_name() + "." + name)
    {
    }
    KeyError::KeyError(ObjectPtr key)
        : ScriptError("Key not found: " + key->to_string())
    {
    }
}
