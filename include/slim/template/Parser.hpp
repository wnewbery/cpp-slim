#pragma once
#include <memory>
#include <stack>
#include <string>
#include <vector>
#include "Function.hpp"
#include "Token.hpp"
#include "expression/Scope.hpp"
namespace slim
{
    class Template;
    namespace expr
    {
        class ExpressionNode;
    }
    namespace tpl
    {
        class Lexer;
        class TemplatePart;
        class Parser
        {
        public:
            Parser(Lexer &lexer);
            ~Parser();

            Template parse();
        private:
            //TODO: output_stack is not actually used as a stack yet, since control blocks are not
            //implemented. This is a placeholder ready for them, and currently all output is simply
            //added to the first output frame.
            /**Sequence of output parts within a single control block (if/loop/etc).
             * When creating a new control block, a new stack frame is created for the contents of
             * that block.
             */
            class OutputFrame;
            struct WhiteSpaceControl
            {
                bool leading_space = false;
                bool trailing_space = false;
            };

            Lexer &lexer;
            Token current_token;
            /**Names of local variables as these are shared between seperate expressions.
             * Blocks in "-x expr.each do |a, b, c|" lines can create new scopes, which is handled
             * internally by parse_control_code().
             */
            expr::LocalVarNames local_vars;

            int current_indent();

            void parse_lines(int base_indent, OutputFrame &output);

            /**Parses a line as text, including following indented text lines.
             * Used for verbatim text, html, template comments and html comments.
             */
            std::string parse_text_line(int base_indent);
            /**Parses a tag line. Calls parse_lines to create the tags contents.*/
            void parse_tag(int base_indent, OutputFrame &output);
            /**Parses a control code line. Calls parse_lines to create the blocks contrents.*/
            void parse_control_code(int base_indent, OutputFrame &output);

            /**Parse code starting with '-', '=', etc. as an expression.
             * Uses parse_code_src to get the source code.
             */
            std::unique_ptr<expr::ExpressionNode> parse_code_expr();
            /**Parse lines startin withg '-', '=', etc., handling line continuation '\' and ','.
             * Returns the script source string.
             */
            std::string parse_code_src();
            /**Parse a code line, with possible ',' or '\' line continuation, and any ending "do"
             * block.
             * The next token should be code text, as parsed by parse_code_src.
             * @param had_do Output. Set to true if "do" was encountered at the end of the parsed
             * source.
             * @param param_names If had_do is true, then the names of any parameters for the block.
             */
            void parse_code_line_expr(
                std::unique_ptr<expr::ExpressionNode> *expr,
                bool *had_do,
                std::vector<Ptr<Symbol>> *param_names);
            /**Parses a code line, and uses parse_lines to build a block for the final method call.*/
            void parse_code_line(int base_indent, OutputFrame &output);

            /**Parse '<' and '>' and return them.*/
            WhiteSpaceControl parse_ws_control();

            /**Looks for interpolated #{} blocks in the string, and adds the fragments to the output.
             * Used for "|" test lines, literal HTML and tag text content.
             */
            void add_interpolated_text(const std::string &text, OutputFrame &output);
            
            /**Throw syntax error at current position.*/
            [[noreturn]] void error(const std::string &msg);
        };
    }
}
