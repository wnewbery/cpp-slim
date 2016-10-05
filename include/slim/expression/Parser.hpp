#pragma once
#include "Token.hpp"
#include "Scope.hpp"
#include "../Function.hpp"
#include <memory>
#include <vector>
namespace slim
{
    namespace expr
    {
        class Lexer;
        class ExpressionNode;
        typedef std::unique_ptr<ExpressionNode> ExpressionNodePtr;
        struct InterpolatedStringNode;
        typedef std::vector<InterpolatedStringNode> InterpolatedStringNodes;

        /**Parsers tokens from the Lexer into an abstract syntax tree.*/
        class Parser
        {
        public:
            Parser(const LocalVarNames &vars, Lexer &lexer);

            /**Parse the entire source as a complete expression.
             * Same as expression, but anything other than Token::END raises an error.
             */
            ExpressionNodePtr full_expression();

            /**Parses a complete expression, stopping at as logic point.
             *   - end of file.
             *   - Unmatched ')'.
             *   - Unexpected ','.
             *   - Unexpected operator or value.
             *   - Unknown token.
             */
            ExpressionNodePtr expression();

            /**Parse a list of parameters in a |params| for blocks.
             * Also used explicitly by the template parser as "do |params|" or "{|params|"
             * is not a valid expression due to the lack of a body and end.
             */
            std::vector<SymPtr> param_list();

            /**Gets the last token read from lexer.*/
            const Token& get_last_token()const { return current_token; }
            const LocalVarNames& get_var_names()const { return vars; }
        private:
            //State
            Lexer &lexer;
            Token current_token;
            /**Variables in current scope.
             * blocks have own scope, so block() updates and reverts this internally.
             */
            LocalVarNames vars;

            /**Advance to the next token.*/
            void next();
            /**A symbol starting with a capital letter is a constant by default.*/
            bool is_constant(const std::string &str)const
            {
                assert(!str.empty());
                return str[0] >= 'A' && str[0] <= 'Z';
            }

            /** '(' expression ')' */
            ExpressionNodePtr sub_expression();
            /**A literal value, variable, or global function call.
             * Value does not need to worry about in_cond_op for symbols as "? :" and ": :"
             * would be invalid anyway, so is always safe to parse ":symbol".
             * But it is needed to avoid parsing "a :x" as a function with 1 argument.
             * See conditional_op.
             */
            ExpressionNodePtr value(bool in_cond_op);

            struct BaseInterpString;
            /** Interpolated string or regex core.*/
            BaseInterpString base_interp_string(char delim);
            /** An interpolated string literal. */
            ExpressionNodePtr interp_string();
            /** An interpolated /regex/ literal.*/
            ExpressionNodePtr regex_literal();

            /** Array [a, b, c] literal */
            ExpressionNodePtr array_literal();
            /** Hash {key: value, 'key2' => 5} literal */
            ExpressionNodePtr hash_literal();

            bool is_func_arg_start()const;
            /** '(' expression, expression ')' */
            std::vector<ExpressionNodePtr> func_args(bool in_cond_op);
            /** expression, expression
             * Used for function calls, and element references [args].
             */
            std::vector<ExpressionNodePtr> func_args_inner();
            /** Hash literal without {}, used for named parameters for functions. */
            ExpressionNodePtr func_hash_args_inner(ExpressionNodePtr &&first_key);
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
            void next_binary_op(bool in_cond_op, ExpressionNodePtr &lhs, U get_rhs);

            /** cond ? true : false
             * 
             * The "in_cond_op" boolean passed to all lower precedence functions is to ban them
             * from consuming a ':' without first having brackets.
             * 
             * "cond ? :x : :y"
             * This is fine because "? :" and ": ", is not valid anyway, so x and y are definately symbols.
             * "cond ? x :b"
             * This however would break, since it would consider :b as an argument to a function x,
             * and then conditional_op would not find its ':' or false expression.
             * Banning ':' with in_cond_op makes this "cond ? (x) : (b)".
             * 
             * "cond ? f :x :b"
             * This would also be ambiguous even with an intelligent look-ahead.
             * "f(:x) : b" or "f : x(:b)"
             * Instead with in_cond_op, parsed as "cond ? (f) : (x)", then the ":b" will cause a
             * syntax error as an unexpected symbol.
             */
            ExpressionNodePtr conditional_op();
            /** || */
            ExpressionNodePtr logical_or_op(bool in_cond_op);
            /** && */
            ExpressionNodePtr logical_and_op(bool in_cond_op);
            /** == != */
            ExpressionNodePtr equality_op(bool in_cond_op);
            /** < <= > >= */
            ExpressionNodePtr cmp_op(bool in_cond_op);
            /** | ^ */
            ExpressionNodePtr bitor_op(bool in_cond_op);
            /** & */
            ExpressionNodePtr bitand_op(bool in_cond_op);
            /** << >> */
            ExpressionNodePtr bitshift_op(bool in_cond_op);
            /** + -*/
            ExpressionNodePtr add_op(bool in_cond_op);
            /** * / % */
            ExpressionNodePtr mul_op(bool in_cond_op);
            /** ! + - ~ */
            ExpressionNodePtr unary_op(bool in_cond_op);
            /** ** */
            ExpressionNodePtr pow_op(bool in_cond_op);
            /** .func(args) */
            ExpressionNodePtr member_func(bool in_cond_op);


            /**Throw syntax error at current position.*/
            [[noreturn]] void error(const std::string &msg)const;
            /**Parse current token as a number.*/
            double parse_num()const;
        };
    }
}
