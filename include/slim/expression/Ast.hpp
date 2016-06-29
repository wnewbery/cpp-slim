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
        class ExpressionNode
        {
        public:
            virtual ~ExpressionNode() {}
            virtual std::string to_string()const = 0;
            virtual ObjectPtr eval(Scope &scope)const = 0;
        };

        class UnaryOp : public ExpressionNode
        {
        public:
            virtual std::string to_string()const override
            {
                return std::string("(") + symbol() + " " + arg->to_string() + ")";
            }
            virtual const char *symbol()const = 0;
            virtual ObjectPtr eval(Scope &scope)const = 0;
        protected:
            ExpressionNodePtr arg;
        };
        class BinaryOp : public ExpressionNode
        {
        public:
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
