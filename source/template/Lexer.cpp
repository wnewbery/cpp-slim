#include "template/Lexer.hpp"
#include "template/Token.hpp"
#include "Error.hpp"
#include <cassert>
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

                if (p == end) return ++p, Token::END;
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

        Token Lexer::next_line()
        {
            if (p > end) error("Unexpected end");
            if (p == end) return ++p, Token::END;
            if (!try_newline()) error("Expected newline");
            return Token::EOL;
        }

        Token Lexer::next_line_start()
        {
            if (p >= end) error("Unexpected end");
            switch (*p)
            {
            case '|':  ++p; return Token::TEXT_LINE;
            case '\'': ++p; return Token::TEXT_LINE_WITH_TRAILING_SPACE;
            case '<': ++p; return Token::HTML_LINE;
            case '/':
                if (p + 1 < end && p[1] == '!') return p += 2, Token::HTML_COMMENT_LINE;
                else return ++p, Token::COMMENT_LINE;
            default:
                if (is_name_chr(*p)) return next_name();
                else error("Unexpected symbol at line start");
            }
        }

        Token Lexer::next_tag_content()
        {
            if (p > end) error("Unexpected end");
            if (p == end) return ++p, Token::END;
            if (*p == '<')
            {
                if (p + 1 < end && p[1] == '>')
                    return p += 2, Token::ADD_LEADING_AND_TRAILING_WHITESPACE;
                else return ++p, Token::ADD_LEADING_WHITESPACE;
            }
            else if (*p == '>')
            {
                ++p;
                return Token::ADD_TRAILING_WHITESPACE;
            }

            skip_spaces();
            //if reached end of line
            if (p == end) return ++p, Token::END;
            if (try_newline()) return{ Token::EOL };

            //attributes, text content, or dynamic content
            //dynamic content starts with a '=', attribute names have a '=' after,
            //otherwise is just text
            if (*p == '=') error("Dynamic element content not implemented");

            auto start = p;
            //try to find attribute first
            while (p < end && is_name_chr(*p)) ++p;
            if (p < end && *p == '=')
            {
                assert(p > start);
                error("Element attributes not implemented");
            }
            //just text, go to end of line
            auto p2 = p; //p gets updated by try_newline
            while (p < end && !try_newline()) ++p, ++p2;

            return{ Token::TEXT_CONTENT, std::string(start, p2 - start) };
        }

        Token Lexer::next_text_content()
        {
            if (p > end) error("Unexpected end");
            if (p == end) return ++p, Token::END;
            if (*p == ' ') ++p; //ignore single leading space
            auto start = p;
            auto p2 = p; //p gets updated by try_newline
            while (p < end && !try_newline()) ++p, ++p2;
            return{ Token::TEXT_CONTENT, std::string(start, p2 - start) };
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

        void Lexer::skip_spaces()
        {
            while (p < end && (*p == ' ' ||  *p == '\t')) ++p;
        }

        void Lexer::error(const std::string &msg)
        {
            throw TemplateSyntaxError(msg);
        }
    }
}
