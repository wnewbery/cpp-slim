#pragma once
#include "Ast.hpp"
namespace slim
{
    namespace expr
    {
        class Eq : public BinaryOp
        {
        public:
            using BinaryOp::BinaryOp;
            virtual const char *symbol()const override { return "=="; }
            virtual ObjectPtr eval(Scope &scope)const override;
        };
        class Ne : public BinaryOp
        {
        public:
            using BinaryOp::BinaryOp;
            virtual const char *symbol()const override { return "!="; }
            virtual ObjectPtr eval(Scope &scope)const override;
        };
        class Cmp : public BinaryOp
        {
        public:
            using BinaryOp::BinaryOp;
            virtual const char *symbol()const override { return "<=>"; }
            virtual ObjectPtr eval(Scope &scope)const override;
        };
        class Lt : public BinaryOp
        {
        public:
            using BinaryOp::BinaryOp;
            virtual const char *symbol()const override { return "<"; }
            virtual ObjectPtr eval(Scope &scope)const override;
        };
        class Le : public BinaryOp
        {
        public:
            using BinaryOp::BinaryOp;
            virtual const char *symbol()const override { return "<="; }
            virtual ObjectPtr eval(Scope &scope)const override;
        };
        class Gt : public BinaryOp
        {
        public:
            using BinaryOp::BinaryOp;
            virtual const char *symbol()const override { return ">"; }
            virtual ObjectPtr eval(Scope &scope)const override;
        };
        class Ge : public BinaryOp
        {
        public:
            using BinaryOp::BinaryOp;
            virtual const char *symbol()const override { return ">="; }
            virtual ObjectPtr eval(Scope &scope)const override;
        };
    }
}
