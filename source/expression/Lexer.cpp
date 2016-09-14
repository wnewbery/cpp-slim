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
            auto start = p;
            auto make_tok = [this, start](Token::Type t)
            {
                return ++p, Token(start, t);
            };

            skip_ws();
            if (p == end) return ++p, make_tok(Token::END);
            else if (p > end) error("Unexpected end");

            char peek = *p;
            switch (peek)
            {
            case '(': return make_tok(Token::LPAREN);
            case ')': return make_tok(Token::RPAREN);
            case '[': return make_tok(Token::L_SQ_BRACKET);
            case ']': return make_tok(Token::R_SQ_BRACKET);
            case '{': return make_tok(Token::L_CURLY_BRACKET);
            case '}': return make_tok(Token::R_CURLY_BRACKET);
            case ',': return make_tok(Token::COMMA);
            case '.': return make_tok(Token::DOT);
            case '+': return make_tok(Token::PLUS);
            case '/': return make_tok(Token::DIV);
            case '%': return make_tok(Token::MOD);
            case '~': return make_tok(Token::NOT);
            case '^': return make_tok(Token::XOR);
            case ':':
                if (p + 1 < end && p[1] == ':') return p += 2, Token(start, Token::CONST_NAV);
                else return make_tok(Token::COLON);
            case '?': return make_tok(Token::CONDITIONAL);
            case '\'':
            case '\"':
                ++p;
                return {start, Token::STRING_DELIM, { p - 1, 1 }};
            case '*':
                if (p + 1 >= end) error("Unexpected end");
                if (p[1] == '*') return p += 2, Token(start, Token::POW);
                else return make_tok(Token::MUL);
            case '-':
                ++p;
                if (p >= end) error("Unexpected end");
                else if (is_digit(p[0])) return number(start, true);
                else return {start, Token::MINUS};
            case '&':
                if (p + 1 >= end) error("Unexpected end");
                if (p[1] == '&') return p += 2, Token(start, Token::LOGICAL_AND);
                if (p[1] == '.') return p += 2, Token(start, Token::SAFE_NAV);
                else return make_tok(Token::AND);
            case '|':
                if (p + 1 < end && p[1] == '|') return p += 2, Token(start, Token::LOGICAL_OR);
                else return make_tok(Token::OR);
            case '!':
                if (p + 1 >= end) error("Unexpected end");
                if (p[1] == '=') return p += 2, Token(start, Token::CMP_NE);
                else return make_tok(Token::LOGICAL_NOT);
            case '=':
                if (p + 1 >= end) error("Unexpected end");
                if (p[1] == '=') return p += 2, Token(start, Token::CMP_EQ);
                if (p[1] == '>') return p += 2, Token(start, Token::HASH_KEY_VALUE_SEP);
                else return make_tok(Token::UNKNOWN);
            case '<':
                if (p + 1 >= end) error("Unexpected end");
                if (p[1] == '<') return p += 2, Token(start, Token::LSHIFT);
                if (p[1] == '=')
                {
                    if (p + 2 >= end) error("Unexpected end");
                    if (p[2] == '>') return p += 3, Token(start, Token::CMP);
                    else return p += 2, Token(start, Token::CMP_LE);
                }
                else return make_tok(Token::CMP_LT);
            case '>':
                if (p + 1 >= end) error("Unexpected end");
                if (p[1] == '>') return p += 2, Token(start, Token::RSHIFT);
                if (p[1] == '=') return p += 2, Token(start, Token::CMP_GE);
                else return make_tok(Token::CMP_GT);
            case '@':
                ++p;
                return Token(start, Token::ATTR_NAME, symbol_str());
            default:
                if (is_symbol_start_chr(peek)) return symbol(start);
                else if (is_digit(peek)) return number(start, false);
                else return {start, Token::UNKNOWN};
            }
        }
        Token Lexer::next_str_interp(char delim)
        {
            auto start = p;
            if (p == end) return ++p, Token(start, Token::END);
            else if (p > end) error("Unexpected end");

            if (*p == delim) return ++p, Token(start, Token::STRING_DELIM);
            if (p + 1 < end && p[0] == '#' && p[1] == '{')
            {
                p += 2;
                return {start, Token::STRING_INTERP_START};
            }

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

            return{ start, Token::STRING_TEXT, str };
        }
        void Lexer::skip_ws()
        {
            while (p < end && is_ws_chr(*p)) ++p;
        }
        
        Token Lexer::symbol(const char *start)
        {
            assert(is_symbol_start_chr(*p));
            auto name = symbol_str();
            auto type = Token::SYMBOL;

            if (p < end)
            {
                if (*p == ':' && (p + 1 >= end || p[1] != ':')) //symbol:: is SYMBOL + CONST_NAV
                {
                    ++p;
                    type = Token::HASH_SYMBOL;
                }
                else if(*p == '?') //trailing '?' is part of method symbols
                {
                    name += '?';
                    ++p;
                }
            }
            return { start, type, name };
        }
        std::string Lexer::symbol_str()
        {
            auto sym_start = p;
            while (p < end && is_symbol_chr(*p)) ++p;
            auto sym_end = p;
            if (sym_start == sym_end) error("Expected symbol name");
            return {sym_start, sym_end};
        }
        Token Lexer::number(const char *tok_start, bool negative)
        {
            assert((is_digit(*p) && !negative) || (p[-1] =='-' && negative));
            auto start = negative ? p - 1 : p;
            while (p < end && is_digit(*p)) ++p;

            if (p + 1 < end && p[0] == '.' && is_digit(p[1])) //decimal
            {
                p += 2;
                while (p < end && is_digit(*p)) ++p;
            }

            return{ tok_start, Token::NUMBER, std::string(start, p - start) };
        }

    }
}
