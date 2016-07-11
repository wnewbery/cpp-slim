#pragma once
#include <string>
namespace slim
{
    namespace expr
    {
        struct Token;
        /**@brief Reads tokens from source code for use by the Parser.*/
        class Lexer
        {
        public:
            Lexer(const char *str, size_t len) : start(str), p(str), end(str + len) {}
            Lexer(const std::string &str) : Lexer(str.data(), str.size()) {}
            Lexer(const char *str, const char *end) : start(str), p(str), end(end) {}

            Token next();
            /**STRING_TEXT, STRING_INTERP_START, STRING_TERM*/
            Token next_str_interp(char delim);

        private:
            const char *start, *p, *end;

            [[noreturn]] void error(const std::string &msg);
            void skip_ws();
            Token symbol(const char *start);
            Token number(const char *start, bool negative);
        };
    }
}
