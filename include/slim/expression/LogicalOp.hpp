#pragma once
#include "Ast.hpp"
namespace slim
{
    namespace expr
    {
        class LogicalNot : public UnaryOp
        {
        public:
            using UnaryOp::UnaryOp;
            virtual const char *symbol()const override { return "!"; }
            virtual ObjectPtr eval(Scope &scope)const override;
        };
        class LogicalAnd : public BinaryOp
        {
        public:
            using BinaryOp::BinaryOp;
            virtual const char *symbol()const override { return "&&"; }
            virtual ObjectPtr eval(Scope &scope)const override;
        };
        class LogicalOr : public BinaryOp
        {
        public:
            using BinaryOp::BinaryOp;
            virtual const char *symbol()const override { return "||"; }
            virtual ObjectPtr eval(Scope &scope)const override;
        };
    }
}
