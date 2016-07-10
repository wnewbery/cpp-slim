#include "template/Lexer.hpp"
#include "template/Token.hpp"
#include "Error.hpp"
namespace slim
{
    namespace tpl
    {
        namespace
        {
            bool is_name_chr(char c)
            {
                return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                    c == '-' || c == '_';
            }
        }

        Lexer::Lexer(const char *begin, const char *end)
            : begin(begin), p(begin), end(end), line(1)
        {}

        Token Lexer::next_indent()
        {
            if (p > end) error("Unexpected end");
            while (true)
            {
                auto start = p;
                while (p < end && *p == ' ') ++p;

                if (p == end) return { Token::END };
                else if (*p == '\t') error("Tabs are not allowed for indentation, only spaces");
                else if (try_newline()) continue;
                else return { Token::INDENT, std::string(start, p - start) };
            }
        }

        Token Lexer::next_name()
        {
            if (p > end) error("Unexpected end");
            auto start = p;
            while (p < end && is_name_chr(*p)) ++p;
            if (p == start) error("Expected name");
            return { Token::NAME, std::string(start, p - start) };
        }

        void Lexer::next_line()
        {
            if (!try_newline()) error("Expected newline");
        }

        bool Lexer::try_newline()
        {
            if (*p == '\n')
            {
                ++line;
                ++p;
                return true;
            }
            else if (*p == '\r')
            {
                if (p + 1 < end && p[1] == '\n')
                {
                    ++line;
                    p += 2;
                    return true;
                }
                else
                {
                    ++line;
                    ++p;
                    return true;
                }
            }
            return false;
        }

        void Lexer::error(const std::string &msg)
        {
            throw TemplateSyntaxError(msg);
        }
    }
}
