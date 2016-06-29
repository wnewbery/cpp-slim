#pragma once
#include <stdexcept>
namespace slim
{
    class Object;

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
        using ScriptError::ScriptError;
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
}
