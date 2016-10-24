#pragma once
#include <string>
#include "Token.hpp"
#include "../BaseLexer.hpp"
namespace slim
{
    namespace tpl
    {
        struct Token;
        /**@brief Lexer for templates.
         * Unfortunately the template syntax has a lot of context, so publically exposed seperate
         * token readers.
         */
        class Lexer : public BaseLexer<Token>
        {
        public:
            using BaseLexer::BaseLexer;

            /**The number of spaces at the start of the next line.
             * Empty lines are skiped and ignored, tabs are banned (SyntaxError)
             *
             * Note, assumes currently positioned at a line start.
             */
            Token next_indent();
            /**Reads a Token::NAME, else error.*/
            Token next_name();
            /**Go to next line. Error if not at the end of a line, or the end of the document
             * already.
             */
            Token next_line();
            /**Start of a line after indent.
             *
             *    - TEXT_LINE
             *    - TEXT_LINE_WITH_TRAILING_SPACE
             *    - HTML_LINE
             *    - COMMENT_LINE
             *    - HTML_COMMENT_LINE
             *    - NAME (tag)
             *    - TAG_ID
             *    - TAG_CLASS
             *    - OUTPUT_LINE
             *    - CONTROL_LINE
             */
            Token next_line_start();
            /**Next part of a tag content after a tag name.
             * May return:
             *    - ADD_*_WHITESPACE
             *    - ATTR_NAME
             *    - ATTR_WRAPPER_START
             *    - SPLAT_ATTR
             *    - END
             *    - EOL
             *    - NAME
             *    - OUTPUT_LINE
             *    - TAG_CLASS
             *    - TAG_ID
             *    - TEXT_CONTENT
             * Will later include:
             *    - close tag ("/")
             */
            Token next_tag_content();
            /**
             * @param start_delim Delimiter from ATTR_WRAPPER_START
             * @return Empty string if end is found, else attribute name.
             */
            std::string next_wrapped_attr_name(char start_delim);
            /**Advance past the '=' after a wrapped attribute name.
             * An error is thrown if any other non-whitespace character is found.
             */
            void next_wrapped_attr_assignment();
            /**Attribute value source.
             *
             * Either a single word ending with a newline or space, or a balanced delimited block.
             * Delimiters are: '"', "'", '(' => ')', '[' => ']', '{' => '}'.
             *
             * An error is thrown if there is no source.
             */
            std::string next_attr_code_src();

            /**Rest of line as text.*/
            Token next_text_content();
            /**Rest of the line, including the line terminator if present.
             * Used for script parsing because modifying the source makes script error positions
             * wrong.
             */
            Token next_text_line();
            /**Reads '<', '>', or '<>', else 'END' even if not actually end of source.*/
            Token next_whitespace_control();
            /**Reads the first token from a control code line.
             * IF, UNLESS or EACH_START.
             */
            Token control_code_start();
            /**See if the next token is a '-', if so return true and advance.*/
            bool try_control_line();

            /**Get the current position in the source input.
             * Used so that another lexer, such as expr::Lexer can be used to parse a block.
             */
            const char *get_pos()const { return p; }
            /**Get the end of the source, to go with get_pos.*/
            const char *get_end()const { return end; }
            /**Set the current position in the source input.
             * Used after some source was consumed by an expression, as this Lexer can not handle
             * that syntax.
             */
            void set_pos(const char *p);
        private:
            /**If next matches str, return true and advance p by strlen(str).*/
            bool starts_with(const std::string &str);
            /**Skip any ' ' or '\t'.*/
            void skip_spaces();

            /**Throw SyntaxError at current position.*/
            [[noreturn]] void error(const std::string &msg)
            {
                throw TemplateSyntaxError(_file_name, _line, line_offset(), msg);
            }
        };
    }
}
