#include "expression/Lexer.hpp"
#include "expression/Token.hpp"
#include "Error.hpp"
#include <cassert>
namespace slim
{
    namespace expr
    {
        namespace
        {
            bool is_ws_chr(char c)
            {
                return c == ' ' || c == '\t' || c == '\r' || c == '\n';
            }
            bool is_digit(char c)
            {
                return c >= '0' && c <= '9';
            }
            bool is_symbol_start_chr(char c)
            {
                return c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
            }
            bool is_symbol_chr(char c)
            {
                return is_symbol_start_chr(c) || is_digit(c);
            }
        }
        
        void Lexer::error(const std::string &msg)
        {
            throw SyntaxError(msg);
        }

        Token Lexer::next()
        {
            skip_ws();
            if (p == end) return ++p, Token::END;
            else if (p > end) error("Unexpected end");

            char peek = *p;
            switch (peek)
            {
            case '(': ++p; return Token::LPAREN;
            case ')': ++p; return Token::RPAREN;
            case '[': ++p; return Token::L_SQ_BRACKET;
            case ']': ++p; return Token::R_SQ_BRACKET;
            case '{': ++p; return Token::L_CURLY_BRACKET;
            case '}': ++p; return Token::R_CURLY_BRACKET;
            case ',': ++p; return Token::COMMA;
            case '.': ++p; return Token::DOT;
            case '+': ++p; return Token::PLUS;
            case '/': ++p; return Token::DIV;
            case '%': ++p; return Token::MOD;
            case '~': ++p; return Token::NOT;
            case '^': ++p; return Token::XOR;
            case ':': ++p; return Token::COLON;
            case '?': ++p; return Token::CONDITIONAL;
            case '\'':
            case '\"':
                ++p;
                return {Token::STRING_DELIM, { p - 1, 1 }};
            case '*':
                if (p + 1 >= end) error("Unexpected end");
                if (p[1] == '*') return p += 2, Token::POW;
                else return ++p, Token::MUL;
            case '-':
                ++p;
                if (p >= end) error("Unexpected end");
                else if (is_digit(p[0])) return number(true);
                else return Token::MINUS;
            case '&':
                if (p + 1 >= end) error("Unexpected end");
                if (p[1] == '&') return p += 2, Token::LOGICAL_AND;
                if (p[1] == '.') return p += 2, Token::SAFE_NAV;
                else return ++p, Token::AND;
            case '|':
                if (p + 1 >= end) error("Unexpected end");
                if (p[1] == '|') return p += 2, Token::LOGICAL_OR;
                else return ++p, Token::OR;
            case '!':
                if (p + 1 >= end) error("Unexpected end");
                if (p[1] == '=') return p += 2, Token::CMP_NE;
                else return ++p, Token::LOGICAL_NOT;
            case '=':
                if (p + 1 >= end) error("Unexpected end");
                if (p[1] == '=') return p += 2, Token::CMP_EQ;
                if (p[1] == '>') return p += 2, Token::HASH_KEY_VALUE_SEP;
                error("Expected == or =>");
            case '<':
                if (p + 1 >= end) error("Unexpected end");
                if (p[1] == '<') return p += 2, Token::LSHIFT;
                if (p[1] == '=')
                {
                    if (p + 2 >= end) error("Unexpected end");
                    if (p[2] == '>') return p += 3, Token::CMP;
                    else return p += 2, Token::CMP_LE;
                }
                else return ++p, Token::CMP_LT;
            case '>':
                if (p + 1 >= end) error("Unexpected end");
                if (p[1] == '>') return p += 2, Token::RSHIFT;
                if (p[1] == '=') return p += 2, Token::CMP_GE;
                else return ++p, Token::CMP_GT;
            default:
                if (is_symbol_start_chr(peek)) return symbol();
                else if (is_digit(peek)) return number(false);
                else error(std::string("Unexpected: ") + peek);
            }
        }
        Token Lexer::next_str_interp(char delim)
        {
            if (p == end) return ++p, Token::END;
            else if (p > end) error("Unexpected end");

            if (*p == delim) return ++p, Token::STRING_DELIM;
            if (p + 1 < end && p[0] == '#' && p[1] == '{') return p += 2, Token::STRING_INTERP_START;

            std::string str;
            while (p < end)
            {
                if (*p == '\\')
                {
                    if (p + 1 >= end) error("Unexpected end in string");
                    switch (p[1])
                    {
                    case '\\': str.push_back('\\'); break;
                    case '\'': str.push_back('\''); break;
                    case '"': str.push_back('\"'); break;
                    case 't': str.push_back('\t'); break;
                    case 'r': str.push_back('\r'); break;
                    case 'n': str.push_back('\n'); break;
                    case '#': str.push_back('#'); break;
                    default: error(std::string("Unknown string escape code ") + p[1]);
                    }
                    p += 2;
                }
                else if (*p == '#' && p + 1 < end && p[1] == '{')
                {
                    break; //next token will be STRING_INTERP_START
                }
                else if (*p == delim)
                {
                    break; //next token will be STRING_DELIM
                }
                else
                {
                    str.push_back(*p);
                    ++p;
                }
            }

            return{ Token::STRING_TEXT, str };
        }
        void Lexer::skip_ws()
        {
            while (p < end && is_ws_chr(*p)) ++p;
        }
        
        Token Lexer::symbol()
        {
            assert(is_symbol_start_chr(*p));
            auto sym_start = p;
            while (p < end && is_symbol_chr(*p)) ++p;
            auto sym_end = p;
            auto type = Token::SYMBOL;

            if (p < end)
            {
                if (*p == ':')
                {
                    ++p;
                    type = Token::HASH_SYMBOL;
                }
                else if(*p == '?') //trailing '?' is part of method symbols
                {
                    sym_end = ++p;
                }
            }
            return { type, std::string(sym_start, sym_end - sym_start) };
        }
        Token Lexer::number(bool negative)
        {
            assert((is_digit(*p) && !negative) || (p[-1] =='-' && negative));
            auto start = negative ? p - 1 : p;
            while (p < end && is_digit(*p)) ++p;

            if (p + 1 < end && p[0] == '.' && is_digit(p[1])) //decimal
            {
                p += 2;
                while (p < end && is_digit(*p)) ++p;
            }

            return{ Token::NUMBER, std::string(start, p - start) };
        }

    }
}
