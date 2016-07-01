#include "Error.hpp"
#include "types/Object.hpp"
namespace slim
{
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
}
