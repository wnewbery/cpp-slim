#include "template/Lexer.hpp"
#include "template/Token.hpp"
#include "Error.hpp"
#include <cassert>
#include <cstring>
namespace slim
{
    namespace tpl
    {
        namespace
        {
            constexpr bool is_name_chr(char c)
            {
                return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                    c == '-' || c == '_';
            }

            static_assert(
                !is_name_chr('\n'),
                "Allow newlines in any symbol will break line counting.");
        }

        Token Lexer::next_indent()
        {
            auto t = token(Token::END);
            if (p > end) error("Unexpected end");
            while (true)
            {
                auto start = p;
                while (p < end && *p == ' ') ++p;

                if (p == end) return ++p, t;
                else if (*p == '\t') error("Tabs are not allowed for indentation, only spaces");
                else if (try_newline()) continue;
                else
                {
                    t.type = Token::INDENT;
                    t.str = std::string(start, p - start);
                    return t;
                }
            }
        }

        Token Lexer::next_name()
        {
            auto t = token(Token::NAME);
            if (p > end) error("Unexpected end");
            auto start = p;
            while (p < end && is_name_chr(*p)) ++p;
            if (p == start) error("Expected name");
            t.str = std::string(start, p - start);
            return t;
        }

        Token Lexer::next_line()
        {
            auto t = token(Token::END);
            if (p > end) error("Unexpected end");
            if (p == end) return ++p, t;
            if (!try_newline()) error("Expected newline");
            t.type = Token::EOL;
            return t;
        }

        Token Lexer::next_line_start()
        {
            auto t = token(Token::END);
            if (p >= end) error("Unexpected end");
            switch (*p)
            {
            case '|':  ++p; t.type = Token::TEXT_LINE; break;
            case '\'': ++p; t.type = Token::TEXT_LINE_WITH_TRAILING_SPACE; break;
            case '<': ++p; t.type = Token::HTML_LINE; break;
            case '#': ++p; t.type = Token::TAG_ID; break;
            case '.': ++p; t.type = Token::TAG_CLASS; break;
            case '=': ++p; t.type = Token::OUTPUT_LINE; break;
            case '-': ++p; t.type = Token::CONTROL_LINE; break;
            case '/':
                if (p + 1 < end && p[1] == '!') p += 2, t.type = Token::HTML_COMMENT_LINE;
                else ++p, t.type = Token::COMMENT_LINE;
                break;
            default:
                if (is_name_chr(*p)) return next_name();
                else error("Unexpected symbol at line start");
                break;
            }
            return t;
        }

        Token Lexer::next_tag_content()
        {
            auto t = token(Token::END);
            if (p > end) error("Unexpected end");
            if (p == end) return ++p, t;
            switch (*p)
            {
            case '<':
                if (p + 1 < end && p[1] == '>')
                    p += 2, t.type = Token::ADD_LEADING_AND_TRAILING_WHITESPACE;
                else ++p, t.type = Token::ADD_LEADING_WHITESPACE;
                return t;
            case '>': ++p; t.type = Token::ADD_TRAILING_WHITESPACE; return t;
            case '#': ++p; t.type = Token::TAG_ID; return t;
            case '.': ++p; t.type = Token::TAG_CLASS; return t;
            }

            skip_spaces();
            t = token(Token::END);
            //if reached end of line
            if (p == end) return ++p, t;
            if (try_newline())
            {
                t.type = Token::EOL;
                return t;
            };

            //attributes, text content, or dynamic content
            //dynamic content starts with a '=', attribute names have a '=' after,
            //otherwise is just text
            if (*p == '=')
            {
                ++p;
                t.type = Token::OUTPUT_LINE;
                return t;
            }
            else if (*p == '*')
            {
                ++p;
                t.type = Token::SPLAT_ATTR;
                return t;
            }
            else if (*p == '(' || *p == '[' || *p == '{')
            {
                t.str.assign(p, 1);
                ++p;
                t.type = Token::ATTR_WRAPPER_START;
                return t;
            }

            auto start = p;
            //try to find attribute first
            while (p < end && is_name_chr(*p)) ++p;
            if (p < end && *p == '=')
            {
                assert(p > start);
                ++p;
                t.type = Token::ATTR_NAME;
                t.str = std::string(start, p - start - 1);
                return t;
            }
            //just text, go to end of line
            auto p2 = p; //p gets updated by try_newline
            while (p < end && !try_newline()) ++p, ++p2;

            t.type = Token::TEXT_CONTENT;
            t.str = std::string(start, p2 - start);
            return t;
        }
        std::string Lexer::next_wrapped_attr_name(char start_delim)
        {
            do { skip_spaces(); }
            while (try_newline());

            if (p >= end) error("Unexpected end");
            char c = *p;
            switch (start_delim)
            {
            case '(':
                if (c == ')') return ++p, std::string();
                break;
            case '[':
                if (c == ']') return ++p, std::string();
                break;
            case '{':
                if (c == '}') return ++p, std::string();
                break;
            default: assert(false); std::terminate();
            }
            auto tok = next_name();
            assert(tok.str.size() > 0);
            return tok.str;
        }
        void Lexer::next_wrapped_attr_assignment()
        {
            do { skip_spaces(); } while (try_newline());
            if (p >= end) error("Unexpected end");
            if (*p == '=') ++p;
            else error("Expected '=' for attribute example");
        }

        Token Lexer::next_text_content()
        {
            auto t = token(Token::END);
            if (p > end) error("Unexpected end");
            if (p == end) return ++p, t;
            if (*p == ' ') ++p; //ignore single leading space
            auto start = p;
            auto p2 = p; //p gets updated by try_newline
            while (p < end && !try_newline()) ++p, ++p2;
            t.type = Token::TEXT_CONTENT;
            t.str = std::string(start, p2 - start);
            return t;
        }

        Token Lexer::next_text_line()
        {
            auto t = token(Token::END);
            if (p > end) error("Unexpected end");
            if (p == end) return ++p, t;

            auto start = p;
            while (p < end && !try_newline()) ++p;
            t.type = Token::TEXT_CONTENT;
            t.str = std::string(start, p - start);
            return t;
        }

        Token Lexer::next_whitespace_control()
        {
            auto t = token(Token::END);
            if (p > end) error("Unexpected end");
            if (p == end) return t;
            switch (*p)
            {
            case '<':
                if (p + 1 < end && p[1] == '>')
                {
                    p += 2;
                    t.type = Token::ADD_LEADING_AND_TRAILING_WHITESPACE;
                }
                else
                {
                    ++p;
                    t.type = Token::ADD_LEADING_WHITESPACE;
                }
                return t;
            case '>':
                ++p;
                t.type = Token::ADD_TRAILING_WHITESPACE;
                return t;
            default: return t;
            }
        }

        Token Lexer::control_code_start()
        {
            skip_spaces();
            auto t = token(Token::END);
            if (p >= end) error("Unexpected end");
            
            if (starts_with("if ")) t.type = Token::IF;
            else if (starts_with("elsif ")) t.type = Token::ELSIF;
            else if (starts_with("else")) t.type = Token::ELSE;
            else if (starts_with("unless ")) t.type = Token::UNLESS;
            else t.type = Token::EACH_START;
            return t;
        }

        bool Lexer::try_control_line()
        {
            if (p < end && *p == '-')
            {
                ++p;
                return true;
            }
            else return false;
        }

        void Lexer::set_pos(const char *p)
        {
            assert(p >= this->begin && p <= this->end);
            this->p = p;
        }

        bool Lexer::starts_with(const std::string &str)
        {
            if (p + str.size() <= end && memcmp(p, str.data(), str.size()) == 0)
            {
                p += str.size();
                return true;
            }
            else return false;
        }

        void Lexer::skip_spaces()
        {
            while (p < end && (*p == ' ' ||  *p == '\t')) ++p;
        }
    }
}
