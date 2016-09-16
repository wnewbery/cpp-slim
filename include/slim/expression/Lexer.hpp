#pragma once
#include <string>
#include "Token.hpp"
namespace slim
{
    namespace expr
    {
        struct Token;
        /**@brief Reads tokens from source code for use by the Parser.*/
        class Lexer : public BaseLexer<Token>
        {
        public:
            using BaseLexer::BaseLexer;

            Token next();
            /**STRING_TEXT, STRING_INTERP_START, STRING_TERM*/
            Token next_str_interp(char delim);

            /**Get current position.*/
            const char *get_pos() { return p; }
        private:
            void skip_ws();
            Token symbol(const char *start);
            std::string symbol_str();
            Token number(const char *start, bool negative);

            /**Throw SyntaxError at current position.*/
            [[noreturn]] void error(const std::string &msg)
            {
                throw SyntaxError(_file_name, _line, line_offset(), msg);
            }
        };
    }
}
