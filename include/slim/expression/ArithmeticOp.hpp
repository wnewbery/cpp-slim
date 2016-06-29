#pragma once
#include "Ast.hpp"
namespace slim
{
    namespace expr
    {
        class Negative : public UnaryOp
        {
        public:
            virtual const char *symbol()const override { return "-"; }
            virtual ObjectPtr eval(Scope &scope)const override;
        };
        class Mul : public BinaryOp
        {
        public:
            virtual const char *symbol()const override { return "*"; }
            virtual ObjectPtr eval(Scope &scope)const override;
        };
        class Div : public BinaryOp
        {
        public:
            virtual const char *symbol()const override { return "/"; }
            virtual ObjectPtr eval(Scope &scope)const override;
        };
        class Mod : public BinaryOp
        {
        public:
            virtual const char *symbol()const override { return "%"; }
            virtual ObjectPtr eval(Scope &scope)const override;
        };
        class Add : public BinaryOp
        {
        public:
            virtual const char *symbol()const override { return "+"; }
            virtual ObjectPtr eval(Scope &scope)const override;
        };
        class Sub : public BinaryOp
        {
        public:
            virtual const char *symbol()const override { return "-"; }
            virtual ObjectPtr eval(Scope &scope)const override;
        };
    }
}
