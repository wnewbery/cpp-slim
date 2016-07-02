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

            Token next();

        private:
            const char *start, *p, *end;

            [[noreturn]] void error(const std::string &msg);
            void skip_ws();
            Token quoted_string();
            Token symbol();
            Token number(bool negative);
        };
    }
}
