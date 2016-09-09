#pragma once
#include "Ast.hpp"
#include "Function.hpp"
namespace slim
{
    namespace expr
    {
        //Some misc nodes
        class Literal : public ExpressionNode
        {
        public:
            Literal(ObjectPtr value) : value(value) {}
            virtual std::string to_string()const override;
            virtual ObjectPtr eval(Scope &scope)const override;
            ObjectPtr value;
        };
        class Variable : public ExpressionNode
        {
        public:
            Variable(const SymPtr &name) : name(name) {}
            virtual std::string to_string()const override { return name->str(); }
            virtual ObjectPtr eval(Scope &scope)const override;
            SymPtr name;
        };
        class Attribute : public ExpressionNode
        {
        public:
            Attribute(const SymPtr &name) : name(name) {}
            virtual std::string to_string()const override { return "@" + name->str(); }
            virtual ObjectPtr eval(Scope &scope)const override;
            SymPtr name;
        };
        class FuncCall : public ExpressionNode
        {
        public:
            typedef std::vector<std::unique_ptr<ExpressionNode>> Args;
            FuncCall(Args &&args) : args(std::move(args)) {}
            virtual std::string to_string()const override;
            FunctionArgs eval_args(Scope &scope)const;

            Args args;
        };
        class NamedFuncCall : public FuncCall
        {
        public:
            NamedFuncCall(const SymPtr &name, Args &&args) : FuncCall(std::move(args)), name(name) {}
            SymPtr name;
        };
        class GlobalFuncCall : public NamedFuncCall
        {
        public:
            using NamedFuncCall::NamedFuncCall;
            virtual std::string to_string()const override;
            virtual ObjectPtr eval(Scope &scope)const override;
        };
        class MemberFuncCall : public NamedFuncCall
        {
        public:
            MemberFuncCall(ExpressionNodePtr &&lhs, const SymPtr &name, Args &&args)
                : NamedFuncCall(name, std::move(args)), lhs(std::move(lhs))
            {}
            virtual std::string to_string()const override;
            virtual ObjectPtr eval(Scope &scope)const override;

            ExpressionNodePtr lhs;
        };
        class SafeNavMemberFuncCall : public MemberFuncCall
        {
        public:
            using MemberFuncCall::MemberFuncCall;
            virtual std::string to_string()const override;
            virtual ObjectPtr eval(Scope &scope)const override;
        };
        /**[] operator */
        class ElementRefOp : public FuncCall
        {
        public:
            ElementRefOp(ExpressionNodePtr &&lhs, Args &&args)
                : FuncCall(std::move(args)), lhs(std::move(lhs))
            {}
            virtual std::string to_string()const override;
            virtual ObjectPtr eval(Scope &scope)const override;

            ExpressionNodePtr lhs;
        };
        class ArrayLiteral : public FuncCall
        {
        public:
            ArrayLiteral(Args &&args) : FuncCall(std::move(args)) {}
            virtual std::string to_string()const override;
            virtual ObjectPtr eval(Scope &scope)const override;
        };
        class HashLiteral : public FuncCall
        {
        public:
            HashLiteral(Args &&args) : FuncCall(std::move(args)) {}
            virtual std::string to_string()const override;
            virtual ObjectPtr eval(Scope &scope)const override;
        };

        class Block : public ExpressionNode
        {
        public:
            Block(std::vector<SymPtr> &&param_names, std::unique_ptr<ExpressionNode> &&code)
                : param_names(std::move(param_names)), code(std::move(code))
            {}
            virtual std::string to_string()const override;
            virtual ObjectPtr eval(Scope &scope)const override;
        private:
            std::vector<SymPtr> param_names;
            std::unique_ptr<ExpressionNode> code;
        };

        /**Conditional ternary operator. cond ? true : false. */
        class Conditional : public ExpressionNode
        {
        public:
            Conditional(
                std::unique_ptr<ExpressionNode> &&cond,
                std::unique_ptr<ExpressionNode> &&true_expr,
                std::unique_ptr<ExpressionNode> &&false_expr)
                : cond(std::move(cond)), true_expr(std::move(true_expr)), false_expr(std::move(false_expr))
            {}
            virtual std::string to_string()const override;
            virtual ObjectPtr eval(Scope &scope)const override;
        private:
            std::unique_ptr<ExpressionNode> cond;
            std::unique_ptr<ExpressionNode> true_expr;
            std::unique_ptr<ExpressionNode> false_expr;
        };
        /**Interpolated string. This is effectively the same as a series of strong concatenations,
         * using expr::Add, but is kept seperate to allow better rebuilding of the source
         * (to_string), and for identification by other components (e.g. templates that wish to
         * add HTML escaping).
         */
        class InterpolatedString : public ExpressionNode
        {
        public:
            struct Node
            {
                std::unique_ptr<ExpressionNode> expr;
                std::string literal_text;

                Node(std::unique_ptr<ExpressionNode> &&expr)
                    : expr(std::move(expr)), literal_text()
                {}
                Node(std::string &&str)
                    : expr(nullptr), literal_text(std::move(str))
                {}
                Node(const std::string &str)
                    : expr(nullptr), literal_text(str)
                {}
                Node(Node &&) = default;
                Node & operator = (Node &&) = default;
            };
            typedef std::vector<Node> Nodes;

            InterpolatedString(Nodes &&nodes) : nodes(std::move(nodes)) {}
            virtual std::string to_string()const override;
            virtual ObjectPtr eval(Scope &scope)const override;
        private:
            Nodes nodes;
        };
    }
}
