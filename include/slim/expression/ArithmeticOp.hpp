#pragma once
#include "Ast.hpp"
namespace slim
{
    namespace expr
    {
        class Negative : public UnaryOp
        {
        public:
            using UnaryOp::UnaryOp;
            virtual const char *symbol()const override { return "-"; }
            virtual ObjectPtr eval(Scope &scope)const override;
        };
        class Mul : public BinaryOp
        {
        public:
            using BinaryOp::BinaryOp;
            virtual const char *symbol()const override { return "*"; }
            virtual ObjectPtr eval(Scope &scope)const override;
        };
        class Div : public BinaryOp
        {
        public:
            using BinaryOp::BinaryOp;
            virtual const char *symbol()const override { return "/"; }
            virtual ObjectPtr eval(Scope &scope)const override;
        };
        class Mod : public BinaryOp
        {
        public:
            using BinaryOp::BinaryOp;
            virtual const char *symbol()const override { return "%"; }
            virtual ObjectPtr eval(Scope &scope)const override;
        };
        class Pow : public BinaryOp
        {
        public:
            using BinaryOp::BinaryOp;
            virtual const char *symbol()const override { return "**"; }
            virtual ObjectPtr eval(Scope &scope)const override;
        };
        class Add : public BinaryOp
        {
        public:
            using BinaryOp::BinaryOp;
            virtual const char *symbol()const override { return "+"; }
            virtual ObjectPtr eval(Scope &scope)const override;
        };
        class Sub : public BinaryOp
        {
        public:
            using BinaryOp::BinaryOp;
            virtual const char *symbol()const override { return "-"; }
            virtual ObjectPtr eval(Scope &scope)const override;
        };


        class Lshift : public BinaryOp
        {
        public:
            using BinaryOp::BinaryOp;
            virtual const char *symbol()const override { return "<<"; }
            virtual ObjectPtr eval(Scope &scope)const override;
        };
        class Rshift : public BinaryOp
        {
        public:
            using BinaryOp::BinaryOp;
            virtual const char *symbol()const override { return ">>"; }
            virtual ObjectPtr eval(Scope &scope)const override;
        };
        class And : public BinaryOp
        {
        public:
            using BinaryOp::BinaryOp;
            virtual const char *symbol()const override { return "&"; }
            virtual ObjectPtr eval(Scope &scope)const override;
        };
        class Or : public BinaryOp
        {
        public:
            using BinaryOp::BinaryOp;
            virtual const char *symbol()const override { return "|"; }
            virtual ObjectPtr eval(Scope &scope)const override;
        };
        class Xor : public BinaryOp
        {
        public:
            using BinaryOp::BinaryOp;
            virtual const char *symbol()const override { return "^"; }
            virtual ObjectPtr eval(Scope &scope)const override;
        };
        class Not : public UnaryOp
        {
        public:
            using UnaryOp::UnaryOp;
            virtual const char *symbol()const override { return "~"; }
            virtual ObjectPtr eval(Scope &scope)const override;
        };
    }
}
