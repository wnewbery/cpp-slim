#include "Error.hpp"
#include "types/Object.hpp"
#include "types/Symbol.hpp"
#include <sstream>
#include <cassert>
namespace slim
{
    std::string syntax_error_str(const std::string &file_name, int line, int offset, const std::string &message)
    {
        std::stringstream ss;
        ss << file_name << "(" << line << ":" << offset << "): " << message;
        return ss.str();
    }
    SyntaxError::SyntaxError(const std::string &file_name, int line, int offset, const std::string &message)
        : Error(syntax_error_str(file_name, line, offset, message))
        , _file_name(file_name), _message(message), _line(line), _offset(offset)
    {
    }

    TypeError::TypeError(const Object *type, const std::string &type_name)
        : ScriptError(type->type_name() + " can not be converted to " + type_name)
    {
    }
    UnorderableTypeError::UnorderableTypeError(const Object *lhs, const char *op, const Object *rhs)
        : TypeError("unorderable types: " + lhs->type_name() + " " + op + " " + rhs->type_name())
    {
    }
    
    NoMethodError::NoMethodError(const Object *obj, const std::string &method_name)
        : NameError(obj->type_name() + " has no method " + method_name)
    {
    }
    NoMethodError::NoMethodError(const Object * obj, const Symbol *method_name)
        : NameError(obj->type_name() + " has no method " + method_name->str())
    {
    }
    NoMethodError::NoMethodError(const Symbol *method_name)
        : NameError("No global function " + method_name->str())
    {
    }
    NoConstantError::NoConstantError(const Object *self, SymPtr name)
        : NameError(self->type_name() + " has no constant " + name->str())
    {
    }
    ArgumentError::ArgumentError(const std::string &method_name)
        : ScriptError("Invalid argument for " + method_name)
    {
    }
    ArgumentError::ArgumentError(const Object *obj, const std::string &method_name)
        : ScriptError("Invalid argument for " + obj->type_name() + "." + method_name)
    {
    }
    ArgumentError::ArgumentError(const Object *obj, const std::string &method_name, const std::string &msg)
        : ScriptError("InvalidArgument: " + obj->type_name() + "." + method_name + " " + msg)
    {
    }
    KeyError::KeyError(ObjectPtr key)
        : IndexError("Key not found: " + key->to_string())
    {
    }

    ArgumentCountError::ArgumentCountError(size_t given, size_t min, size_t max)
        : ScriptError(make_message(given, min, max))
    {
    }
    std::string ArgumentCountError::make_message(size_t given, size_t min, size_t max)
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
