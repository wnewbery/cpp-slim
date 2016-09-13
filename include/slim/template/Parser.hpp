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
            class OutputFrame
            {
            public:
                OutputFrame() : contents(), text_content(), _in_tag(false) {}
                OutputFrame& operator << (const std::string &text_content);
                OutputFrame& operator << (char txt_chr);
                OutputFrame& operator << (std::unique_ptr<expr::ExpressionNode> &&expr);
                OutputFrame& operator << (std::unique_ptr<TemplatePart> &&part);
                
                std::unique_ptr<TemplatePart> make_tpl();

                bool in_tag()const { return _in_tag; }
                void set_in_tag(bool b = true) { _in_tag = b; }
            private:
                /**The sequence of completed template parts reading to add to the template or
                 * parent frame control block.
                 */
                std::vector<std::unique_ptr<TemplatePart>> contents;
                /**Text content to go after contents. When adding a non-text part, any text in
                 * this buffer is first converted to a TemplateText part.
                 */
                std::string text_content;
                /**Currently in a start tag. This will need top be closed with a '<' before other
                 * output.
                 */
                bool _in_tag;

                void handle_in_tag();
            };
            struct ParsedCodeLine
            {
                bool leading_space = false;
                bool trailing_space = false;
                std::unique_ptr<expr::ExpressionNode> expr;
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

            /**Parse code starting with '-', '=', etc. and handling '\' and ',' line continuation.*/
            std::unique_ptr<expr::ExpressionNode> parse_code_lines();
            /**Parses a single code fragment and writes it to output.*/
            void parse_code_output(OutputFrame &output);
            /**Parses a single code line and returns it.*/
            ParsedCodeLine parse_code_output();
            /**Writes the code output parsed by parse_code_output.*/
            void add_code_output(ParsedCodeLine &code, OutputFrame &output);
            /**Parses a code line, and uses parse_lines to build a block for the final method call.*/
            void parse_code_line(int base_indent, OutputFrame &output);


        };
    }
}
