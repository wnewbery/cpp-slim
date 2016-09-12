#include "Error.hpp"
#include "types/Object.hpp"
#include "types/Symbol.hpp"
#include <sstream>
#include <cassert>
namespace slim
{
    TypeError::TypeError(const Object *type, const std::string &type_name)
        : RuntimeError(type->type_name() + " can not be converted to " + type_name)
    {
    }
    UnorderableTypeError::UnorderableTypeError(const Object *lhs, const char *op, const Object *rhs)
        : TypeError("unorderable types: " + lhs->type_name() + " " + op + " " + rhs->type_name())
    {
    }
    
    NoSuchMethod::NoSuchMethod(const Object *obj, const std::string &method_name)
        : ScriptError(obj->type_name() + " has no method " + method_name)
    {
    }
    NoSuchMethod::NoSuchMethod(const Object * obj, const Symbol *method_name)
        : ScriptError(obj->type_name() + " has no method " + method_name->str())
    {
    }
    NoSuchMethod::NoSuchMethod(const Symbol *method_name)
        : ScriptError("No global function " + method_name->str())
    {
    }
    NoSuchConstant::NoSuchConstant(const Object *self, SymPtr name)
        : NameError(self->type_name() + " has no constant " + name->str())
    {
    }
    InvalidArgument::InvalidArgument(const std::string &method_name)
        : ScriptError("Invalid argument for " + method_name)
    {
    }
    InvalidArgument::InvalidArgument(const Object *obj, const std::string &method_name)
        : ScriptError("Invalid argument for " + obj->type_name() + "." + method_name)
    {
    }
    InvalidArgument::InvalidArgument(const Object *obj, const std::string &method_name, const std::string &msg)
        : ScriptError("InvalidArgument: " + obj->type_name() + "." + method_name + " " + msg)
    {
    }
    KeyError::KeyError(ObjectPtr key)
        : ScriptError("Key not found: " + key->to_string())
    {
    }

    InvalidArgumentCount::InvalidArgumentCount(size_t given, size_t min, size_t max)
        : ScriptError(make_message(given, min, max))
    {
    }
    std::string InvalidArgumentCount::make_message(size_t given, size_t min, size_t max)
    {
        assert(min <= max);
        std::stringstream ss;
        ss << "ArgumentError: wrong number of arguments (given " << given << ", expected ";
        if (min == max) ss << min;
        else ss << min << ".." << max;
        ss << ")";
        return ss.str();
    }
}
