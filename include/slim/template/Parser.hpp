#pragma once
#include <memory>
#include <stack>
#include <string>
#include <vector>
#include "Token.hpp"
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

            Lexer &lexer;
            Token current_token;

            int current_indent();

            void parse_lines(int base_indent, OutputFrame &output);

            std::string parse_text_line(int base_indent);
            void parse_tag(int base_indent, OutputFrame &output);
            void parse_code_output(OutputFrame &output);
            void parse_control_code(int base_indent, OutputFrame &output);

            /**Parse code starting with '-', '=', etc. and handling '\' and ',' line continuation.*/
            std::unique_ptr<expr::ExpressionNode> parse_code_lines();
        };
    }
}
