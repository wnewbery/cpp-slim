#pragma once
#include "types/Object.hpp"
#include <memory>
#include <string>
namespace slim
{
    namespace expr
    {
        class Scope;
        class ExpressionNode;
        typedef std::unique_ptr<ExpressionNode> ExpressionNodePtr;
        /**Represents an executable expression as an abstract syntax tree (AST).
         * 
         * Expressions always result in a single return value when evaluated.
         */
        class ExpressionNode
        {
        public:
            virtual ~ExpressionNode() {}
            /**Creates a string representation of this expression.*/
            virtual std::string to_string()const = 0;
            /**Evaulate the expression using the specified variable scope, returning the resulting
             * value.
             * 
             * The interpreter may throw any ScriptError derived exception, and any native
             * functions called by the expression may throw any C++ exception.
             */
            virtual ObjectPtr eval(Scope &scope)const = 0;
        };
        /**Abstract base for any left-side unary operator such as '!false'.*/
        class UnaryOp : public ExpressionNode
        {
        public:
            UnaryOp(ExpressionNodePtr &&arg) : arg(std::move(arg)) {}
            virtual std::string to_string()const override
            {
                return std::string("(") + symbol() + arg->to_string() + ")";
            }
            virtual const char *symbol()const = 0;
            virtual ObjectPtr eval(Scope &scope)const = 0;
        protected:
            ExpressionNodePtr arg;
        };
        /**Abstract base for any binary operators.*/
        class BinaryOp : public ExpressionNode
        {
        public:
            BinaryOp(ExpressionNodePtr &&lhs, ExpressionNodePtr &&rhs)
                : lhs(std::move(lhs)), rhs(std::move(rhs))
            {}
            virtual std::string to_string()const override
            {
                return "(" + lhs->to_string() + " " + symbol() + " " + rhs->to_string() + ")";
            }
            virtual const char *symbol()const = 0;
            virtual ObjectPtr eval(Scope &scope)const = 0;
        protected:
            ExpressionNodePtr lhs, rhs;
        };
    }
}
