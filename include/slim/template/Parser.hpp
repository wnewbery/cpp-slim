#pragma once
#include <memory>
#include <stack>
#include <string>
#include <vector>
#include "Token.hpp"
namespace slim
{
    class Template;
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
            /**Stack frame to track input tags and indent levels. This is because there is no
             * explicit, and named element terminator like in HTML/XML, instead closing tags
             * need to be created when the input reduces its indentation.
             * 
             * Frames are created by parse_tag and destroyed by close_tags.
             */
            struct InputFrame
            {
                /**Indentation of the opening tag, so can determine how many elements to close
                 * when indentation is reduced.
                 */
                int indent;
                /**If this element has a trialing space after it. Leading spaces are add
                 * immediately to output when the frame is created, but trailing spaces
                 * must be added later after the closing tag.
                 */
                bool trailing_space;
                /**Name of the element tag that created this frame, to use when creating the
                 * closing tag.
                 */
                std::string tagname;

                InputFrame(int indent, bool trailing_space, const std::string &tagname)
                    : indent(indent), trailing_space(trailing_space), tagname(tagname)
                {}
            };
            //TODO: output_stack is not actually used as a stack yet, since control blocks are not
            //implemented. This is a placeholder ready for them, and currently all output is simply
            //added to the first output frame.
            /**Sequence of output parts within a single control block (if/loop/etc).
             * When creating a new control block, a new stack frame is created for the contents of
             * that block.
             */
            struct OutputFrame
            {
                /**Indent level of the control block that created this frame.
                 * The control block ends as soon as indentation equal or less than this occurs.
                 */
                int indent;
                /**The sequence of completed template parts reading to add to the template or
                 * parent frame control block.
                 */
                std::vector<std::unique_ptr<TemplatePart>> contents;
                /**Text content to go after contents. When adding a non-text part, any text in
                 * this buffer is first converted to a TemplateText part.
                 */
                std::string text_content;
            };

            Lexer &lexer;
            std::stack<InputFrame> input_stack;
            std::stack<OutputFrame> output_stack;
            /**The most recently read token by lexer.*/
            Token current_token;

            /**Parse lines until end, or an indentation less than the current output frame indent
             * occurs.
             * This is the main parser loop, used by "parse" (parse then checks Token::END was
             * reached, and builds the final Template object) from the last output_stack frame.
             */
            void parse_lines();
            /**Parse a line. Multiple lines may be parsed if a line-continuation is encountered,
             * or if recurisng into control blocks ('-' lines).
             * 
             * Thus this forms the main parser "loop" body.
             */
            void parse_line();
            /**Parse a tag found by parse_line.
             * current_token should still be on the tag start token.
             * Parse tag completes the line (ends with END or EOL).
             */
            void parse_tag(int indent);
            /**Get buffer to write text content to top of output stack.*/
            std::string& txt_output_buf();
            /**Close tags in input_stack with equal or greater indent.*/
            void close_tags(int indent);
            /**Finish the topmost output frame, converting it into a single TemplatePart.*/
            std::unique_ptr<TemplatePart> finish_output_frame();
        };
    }
}
