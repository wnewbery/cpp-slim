#pragma once
#include <memory>
#include <stdexcept>
#include <string>
namespace slim
{
    class Object;
    class Symbol;
    typedef std::shared_ptr<Object> ObjectPtr;
    typedef std::shared_ptr<Symbol> SymPtr;

    /**These need to return "through" other C++ functions and is not a control flow C++ has
     * directly (or C, MRI appears to use thread locals and longjmp for these features).
     *
     * The other option would be along the lines of passing a context to every method, and then
     * checking it after every return to see if things like "break" or "return" were invoked by
     * a block within it.
     */
    class ControlFlowException : public std::exception
    {
    };
    //TODO: "break" script keyword is not fully implemented yet
    /**Used to implement the Ruby "break" statement without passing a context param
     * through every AST node.
     */
    class BreakException : public ControlFlowException
    {
    public:
        ObjectPtr value;

        BreakException();
        BreakException(ObjectPtr value);
    };
    /**Used for "break" and "return" like constructs within pure C++ enumeration code.
     * Such as in Enumerable.
     * This is useful because such C++ methods dont set up the same context as blocks do
     * in regards to what "break" and "return" will exit.
     */
    class SpecialFlowException : public ControlFlowException
    {
    public:
        ObjectPtr value;

        SpecialFlowException();
        SpecialFlowException(ObjectPtr value);
    };

    /**Base type for all exceptions.*/
    class Error : public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };

    /**Base type for all errors thrown at runtime by the script interpreter.*/
    class ScriptError : public Error
    {
    public:
        using Error::Error;
        virtual ~ScriptError() {}
    };
    /**Errors from the source lexer or parser.
     * As well as an error message, the class includes the source position where the error occurred.
     */
    class SyntaxError : public Error
    {
    public:
        SyntaxError(const std::string &file_name, int line, int offset, const std::string &message);

        const std::string &file_name()const { return _file_name; }
        const std::string &message()const { return _message; }
        int line()const { return _line; }
        int offset()const { return _offset; }
    private:
        std::string _file_name, _message;
        int _line, _offset;
    };
    /**SyntaxError from template code, rather than embedded scripts.*/
    class TemplateSyntaxError : public SyntaxError
    {
    public:
        using SyntaxError::SyntaxError;
    };

    /**An error used when an instance has an invalid type for some operation.
     * Mostly used in the same places Ruby uses its TypeError.
     */
    class TypeError : public ScriptError
    {
    public:
        TypeError(const std::string &msg) : ScriptError(msg) {}
        TypeError(const Object *type, const std::string &type_name);
    };
    /**Error used when accessing a container index that does not exist.*/
    class IndexError : public ScriptError
    {
    public:
        using ScriptError::ScriptError;
    };
    /**Error used when accessing a map key that does not exist.*/
    class KeyError : public IndexError
    {
    public:
        explicit KeyError(ObjectPtr key);
    };
    /**Error used when using an ordered comparision operator with types that are not ordered.
     * Such as: "string" < true.
     */
    class UnorderableTypeError : public TypeError
    {
    public:
        UnorderableTypeError(const Object *lhs, const char *op, const Object *rhs);
    };

    /**Attempted to call a method or get a constant that does not exist.*/
    class NameError : public ScriptError
    {
    public:
        using ScriptError::ScriptError;
    };
    /**Attempted to call a method on an object, but the object has no method by that name.*/
    class NoMethodError : public NameError
    {
    public:
        NoMethodError(const Object *obj, const std::string &method_name);
        NoMethodError(const Object *obj, const Symbol *method_name);
        explicit NoMethodError(const Symbol *method_name);
    };
    /**Attempted to access a constant in an object, but the object has no constant by that name.*/
    class NoConstantError : public NameError
    {
    public:
        NoConstantError(const Object *self, SymPtr name);
    };

    /**Used when the arguments used to call a method is wrong.
     * Note that exceptions such as TypeError may also be thrown depending on the methods
     * implementation.
     * 
     * The occurance of such exceptions is a programming error caused by violating a methods
     * contract that should be fixed and their existance is as a developer aid. Programs should
     * not depend on these exceptions for input validation or flow control, unless such validation
     * is explicitly documented by the method.
     */
    class ArgumentError : public ScriptError
    {
    public:
        ArgumentError() : ScriptError("InvalidArgument") {}
        ArgumentError(const std::string &msg) : ScriptError("ArgumentError: " + msg) {}
        ArgumentError(const Object *obj, const std::string &method_name);
        ArgumentError(const Object *obj, const std::string &method_name, const std::string &msg);
    };

    //TODO: Remove, use ArgumentError. Can have utility functions handle the message formatting of
    //the various ArgumentError cases.
    /**Used in most cases where a method is passed a number of arguments that it does not support.*/
    class ArgumentCountError : public ScriptError
    {
    public:
        ArgumentCountError(size_t given, size_t min, size_t max);
    protected:
        static std::string make_message(size_t given, size_t min, size_t max);
    };
}
