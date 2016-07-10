#pragma once
#include <string>
namespace slim
{
    namespace tpl
    {
        struct Token;
        /**@brief Lexer for templates.
         * Unfortunately the template syntax has a lot of context, so publically exposed seperate
         * token readers.
         */
        class Lexer
        {
        public:
            Lexer(const char *begin, const char *end);

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
             */
            Token next_line_start();
            /**Next part of a tag content after a tag name.
             * May return:
             *    - END
             *    - EOL
             *    - NAME
             *    - ADD_*_WHITESPACE
             *    - TEXT_CONTENT
             *    - TAG_ID
             *    - TAG_CLASS
             * Will later include:
             *    - attribute name ("{NAME}=")
             *    - text content
             *    - dynamic content ("= ...")
             *    - close tag ("/")
             */
            Token next_tag_content();
            /**Rest of line as text.*/
            Token next_text_content();

        private:
            const char *begin, *p, *end;
            int line;

            /**If next is a newline (\r, \n, or \r\n), consume it and return true.*/
            bool try_newline();
            /**Skip any ' ' or '\t'.*/
            void skip_spaces();

            [[noreturn]] void error(const std::string &msg);
        };
    }
}
