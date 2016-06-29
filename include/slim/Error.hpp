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

    class ScriptError : public Error
    {
    public:
        using Error::Error;
        virtual ~ScriptError() {}
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
