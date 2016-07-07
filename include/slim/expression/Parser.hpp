#pragma once
#include "Token.hpp"
#include "Function.hpp"
#include <memory>
#include <vector>
namespace slim
{
    namespace expr
    {
        class Lexer;
        class ExpressionNode;
        typedef std::unique_ptr<ExpressionNode> ExpressionNodePtr;

        /**Parsers tokens from the Lexer into an abstract syntax tree.*/
        class Parser
        {
        public:
            Parser(const FunctionTable &global_functions, Lexer &lexer);

            /**Parse the entire source as a complete expression. */
            ExpressionNodePtr parse_expression();
        private:
            const FunctionTable &global_functions;
            Lexer &lexer;
            Token current_token;

            /**Advance to the next token.*/
            void next();

            /**Parses a complete expression, stopping on a ',', ')', or end of file.*/
            ExpressionNodePtr expression();
            /** '(' expression ')' */
            ExpressionNodePtr sub_expression();
            /** A literal value, variable, or global function call. */
            ExpressionNodePtr value();
            /** An interpolated string literal. */
            ExpressionNodePtr interp_string();
            /** Array [a, b, c] literal */
            ExpressionNodePtr array_literal();
            /** Hash {key: value, 'key2' => 5} literal */
            ExpressionNodePtr hash_literal();

            bool is_func_arg_start()const;
            /** '(' expression, expression ')' */
            std::vector<ExpressionNodePtr> func_args();
            /** expression, expression
             * Used for function calls, and element references [args].
             */
            std::vector<ExpressionNodePtr> func_args_inner();
            /**Parses a "{|args| expr}" block limited to a single expression.*/
            ExpressionNodePtr block();

            /**Helper that constructs a binary operator node of type T.
             * 
             * current_token is expected to still be the token for operator T.
             * 
             * @param lhs
             * The already parsed left side of the operator.
             * Will be replaced by the resulting binary operator node.
             * @param get_rhs The member function pointer to parse the right side of the operator.
             */
            template<class T, class U>
            void next_binary_op(ExpressionNodePtr &lhs, U get_rhs);

            /** cond ? true : false */
            ExpressionNodePtr conditional_op();
            /** || */
            ExpressionNodePtr logical_or_op();
            /** && */
            ExpressionNodePtr logical_and_op();
            /** == != */
            ExpressionNodePtr equality_op();
            /** < <= > >= */
            ExpressionNodePtr cmp_op();
            /** | ^ */
            ExpressionNodePtr bitor_op();
            /** & */
            ExpressionNodePtr bitand_op();
            /** << >> */
            ExpressionNodePtr bitshift_op();
            /** + -*/
            ExpressionNodePtr add_op();
            /** * / % */
            ExpressionNodePtr mul_op();
            /** ! + - ~ */
            ExpressionNodePtr unary_op();
            /** ** */
            ExpressionNodePtr pow_op();
            /** .func(args) */
            ExpressionNodePtr member_func();

        };
    }
}
