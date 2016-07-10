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
            /**Next part of a tag content after a tag name.
             * Currently only END, EOL, or whitespace controls, but will later include:
             *    - tag id
             *    - class name
             *    - attribute name ("{NAME}=")
             *    - text content
             *    - dynamic content ("= ...")
             *    - close tag ("/")
             */
            Token next_tag_content();

        private:
            const char *begin, *p, *end;
            int line;

            /**If next is a newline (\r, \n, or \r\n), consume it and return true.*/
            bool try_newline();
            [[noreturn]] void error(const std::string &msg);
        };
    }
}
