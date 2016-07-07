#pragma once
#include <stdexcept>
#include <memory>
namespace slim
{
    class Object;
    class Symbol;
    typedef std::shared_ptr<Object> ObjectPtr;
    typedef std::shared_ptr<Symbol> SymPtr;

    class Error : public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };

    /**Base type for all errors thrown by the script interpreter.*/
    class ScriptError : public Error
    {
    public:
        using Error::Error;
        virtual ~ScriptError() {}
    };
    /**Errors from the lexer or parser.*/
    class SyntaxError : public ScriptError
    {
    public:
        using ScriptError::ScriptError;
    };
    class TypeError : public ScriptError
    {
    public:
        TypeError(const std::string &msg) : ScriptError(msg) {}
        TypeError(const Object *type, const std::string &type_name);
    };
    class IndexError : public ScriptError
    {
    public:
        using ScriptError::ScriptError;
    };
    class KeyError : public ScriptError
    {
    public:
        explicit KeyError(ObjectPtr key);
    };
    class UnorderableTypeError : public TypeError
    {
    public:
        UnorderableTypeError(const Object *lhs, const char *op, const Object *rhs);
    };
    class UnsupportedOperandTypeError : public TypeError
    {
    public:
        UnsupportedOperandTypeError(const Object *lhs, const char *op, const Object *rhs);
        UnsupportedOperandTypeError(const char *op, const Object *rhs);
    };
    class NoSuchMethod : public ScriptError
    {
    public:
        NoSuchMethod(const Object *obj, const std::string &method_name);
        NoSuchMethod(const Object *obj, const Symbol *method_name);
        explicit NoSuchMethod(const Symbol *method_name);
    };
    class DuplicateMethod : public ScriptError
    {
    public:
        DuplicateMethod(const Object *obj, const std::string &method_name);
        explicit DuplicateMethod(const std::string &method_name);
    };
    class InvalidArgument : public ScriptError
    {
    public:
        InvalidArgument() : ScriptError("InvalidArgument") {}
        InvalidArgument(const std::string &method_name);
        InvalidArgument(const Object *obj, const std::string &method_name);
        InvalidArgument(const Object *obj, const std::string &method_name, const std::string &msg);
    };

    class InvalidArgumentCount : public ScriptError
    {
    public:
        InvalidArgumentCount(size_t given, size_t min, size_t max);
    protected:
        static std::string make_message(size_t given, size_t min, size_t max);
    };
}
