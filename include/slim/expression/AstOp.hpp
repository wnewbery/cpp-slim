#pragma once
#include "Ast.hpp"
#include "Function.hpp"
namespace slim
{
    namespace tpl
    {
        class TemplatePart;
    }
    namespace expr
    {
        //Some misc nodes
        /**Literal values created at compile time such as true/false, nil and basic strings.
         * Note that arrays, hash, interpolated strings, etc. do not use this node, as they require
         * runtime execution to fully construct.
         */
        class Literal : public ExpressionNode
        {
        public:
            Literal(ObjectPtr value) : value(value) {}
            virtual std::string to_string()const override;
            virtual ObjectPtr eval(Scope &scope)const override;
            ObjectPtr value;
        };
        /**Gets a variable value.*/
        class Variable : public ExpressionNode
        {
        public:
            Variable(const SymPtr &name) : name(name) {}
            virtual std::string to_string()const override { return name->str(); }
            virtual ObjectPtr eval(Scope &scope)const override;
            SymPtr name;
        };
        /**Assigns a variable value.*/
        class Assignment : public ExpressionNode
        {
        public:
            Assignment(const SymPtr &name, ExpressionNodePtr &&expr)
                : name(name), expr(std::move(expr))
            {}
            virtual std::string to_string()const override;
            virtual ObjectPtr eval(Scope &scope)const override;
            SymPtr name;
            ExpressionNodePtr expr;
        };
        /**Gets an attribute value (on "self").*/
        class Attribute : public ExpressionNode
        {
        public:
            Attribute(const SymPtr &name) : name(name) {}
            virtual std::string to_string()const override { return "@" + name->str(); }
            virtual ObjectPtr eval(Scope &scope)const override;
            SymPtr name;
        };
        /**Gets a constant value (on "self").*/
        class GlobalConstant : public ExpressionNode
        {
        public:
            GlobalConstant(const SymPtr &name) : name(name) {}
            virtual std::string to_string()const override { return name->str(); }
            virtual ObjectPtr eval(Scope &scope)const override;
            SymPtr name;
        };
        /**"::" binary operator.*/
        class ConstantNav : public ExpressionNode
        {
        public:
            ConstantNav(ExpressionNodePtr &&lhs, const SymPtr &name)
                : lhs(std::move(lhs)), name(std::move(name))
            {}
            virtual std::string to_string()const override;
            virtual ObjectPtr eval(Scope &scope)const override;

            ExpressionNodePtr lhs;
            SymPtr name;
        };
        /**Abstract base for function calls. Handles the function arguments.*/
        class FuncCall : public ExpressionNode
        {
        public:
            /**Array of expression that evalutes to an array of values (FunctionArgs).*/
            typedef std::vector<std::unique_ptr<ExpressionNode>> Args;

            FuncCall(Args &&args) : args(std::move(args)) {}
            virtual std::string to_string()const override;
            FunctionArgs eval_args(Scope &scope)const;

            Args args;
        };
        /**Abstract base class for calling methods by their name.*/
        class MethodCall : public FuncCall
        {
        public:
            MethodCall(const SymPtr &name, Args &&args) : FuncCall(std::move(args)), name(name) {}
            SymPtr name;
        };
        /**Calls a method without an explicit context (will use "self").*/
        class GlobalFuncCall : public MethodCall
        {
        public:
            using MethodCall::MethodCall;
            virtual std::string to_string()const override;
            virtual ObjectPtr eval(Scope &scope)const override;
        };
        /**Calls a method on some dynamic value.*/
        class MemberFuncCall : public MethodCall
        {
        public:
            MemberFuncCall(ExpressionNodePtr &&lhs, const SymPtr &name, Args &&args)
                : MethodCall(name, std::move(args)), lhs(std::move(lhs))
            {}
            virtual std::string to_string()const override;
            virtual ObjectPtr eval(Scope &scope)const override;

            ExpressionNodePtr lhs;
        };
        /**Calls a method on some dynamic value, unless that value is "nil", in which case the
         * function arguments are not evaulated, no method call is attempted, and "nil" is returned.
         */
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
        
        /**[...] array literal. Each argument becomes an array element.*/
        class ArrayLiteral : public FuncCall
        {
        public:
            ArrayLiteral(Args &&args) : FuncCall(std::move(args)) {}
            virtual std::string to_string()const override;
            virtual ObjectPtr eval(Scope &scope)const override;
        };
        /**{...} hash literal. Each pair of arguments becomes a key-value pair.*/
        class HashLiteral : public FuncCall
        {
        public:
            HashLiteral(Args &&args) : FuncCall(std::move(args)) {}
            virtual std::string to_string()const override;
            virtual ObjectPtr eval(Scope &scope)const override;
        };

        class RangeOp : public BinaryOp
        {
        public:
            using BinaryOp::BinaryOp;
        };
        /**x..y range*/
        class InclusiveRangeOp : public RangeOp
        {
        public:
            using RangeOp::RangeOp;
            virtual const char *symbol()const override { return ".."; }
            virtual ObjectPtr eval(Scope &scope)const override;
        };
        /**x...y range*/
        class ExclusiveRangeOp : public RangeOp
        {
        public:
            using RangeOp::RangeOp;
            virtual const char *symbol()const override { return "..."; }
            virtual ObjectPtr eval(Scope &scope)const override;
        };

        /**A code block that creates a Proc which captures the local variables in the blocks scope.*/
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
        /**Regex literal using an InterpolatedString.*/
        class InterpolatedRegex : public ExpressionNode
        {
        public:
            InterpolatedRegex(std::unique_ptr<InterpolatedString> &&src, int opts)
                : src(std::move(src)), opts(opts)
            {}
            virtual std::string to_string()const override;
            virtual ObjectPtr eval(Scope &scope)const override;
        private:
            std::unique_ptr<InterpolatedString> src;
            int opts;
        };
    }
}
