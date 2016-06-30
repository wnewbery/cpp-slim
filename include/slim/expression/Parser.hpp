#pragma once
#include "Token.hpp"
#include <memory>
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
            Parser(Lexer &lexer);

            /**Parse the entire source as a complete expression. */
            ExpressionNodePtr parse_expression();
        private:
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

            /** && || */
            ExpressionNodePtr op1();
            /** == != */
            ExpressionNodePtr op2();
            /** < <= > >= */
            ExpressionNodePtr op3();
            /** + -*/
            ExpressionNodePtr op4();
            /** * / % */
            ExpressionNodePtr op5();
            /** ! + - */
            ExpressionNodePtr op6();

        };
    }
}
