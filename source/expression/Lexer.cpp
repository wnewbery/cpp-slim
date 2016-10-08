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

        Token Lexer::next()
        {
            auto start = p;
            skip_ws();

            auto t = token(Token::UNKNOWN);
            t.pos = start;
            auto make_tok = [this, start, &t](size_t size, Token::Type type)
            {
                return p += size, t.type = type, t;
            };
            if (p == end) return make_tok(1, Token::END);
            else if (p > end) error("Unexpected end");

            char peek = *p;
            switch (peek)
            {
            case '(': return make_tok(1, Token::LPAREN);
            case ')': return make_tok(1, Token::RPAREN);
            case '[': return make_tok(1, Token::L_SQ_BRACKET);
            case ']': return make_tok(1, Token::R_SQ_BRACKET);
            case '{': return make_tok(1, Token::L_CURLY_BRACKET);
            case '}': return make_tok(1, Token::R_CURLY_BRACKET);
            case ',': return make_tok(1, Token::COMMA);
            case '+': return make_tok(1, Token::PLUS);
            case '/': return make_tok(1, Token::DIV);
            case '%': return make_tok(1, Token::MOD);
            case '~': return make_tok(1, Token::NOT);
            case '^': return make_tok(1, Token::XOR);
            case '.':
                if (p + 2 < end && p[1] == '.' && p[2] == '.')
                    return make_tok(3, Token::EXCLUSIVE_RANGE);
                else if (p + 1 < end && p[1] == '.')
                    return make_tok(2, Token::INCLUSIVE_RANGE);
                else return make_tok(1, Token::DOT);
            case ':':
                if (p + 1 < end && p[1] == ':') return make_tok(2, Token::CONST_NAV);
                else return make_tok(1, Token::COLON);
            case '?': return make_tok(1, Token::CONDITIONAL);
            case '\'':
            case '\"':
                ++p;
                t.str = { p - 1, 1 };
                t.type = Token::STRING_DELIM;
                return t;
            case '*':
                if (p + 1 >= end) error("Unexpected end");
                if (p[1] == '*') return make_tok(2, Token::POW);
                else return make_tok(1, Token::MUL);
            case '-':
                ++p;
                if (p >= end) error("Unexpected end");
                else if (is_digit(p[0])) return number(start, true);
                else return make_tok(0, Token::MINUS);
            case '&':
                if (p + 1 >= end) error("Unexpected end");
                if (p[1] == '&') return make_tok(2, Token::LOGICAL_AND);
                if (p[1] == '.') return make_tok(2, Token::SAFE_NAV);
                else return make_tok(1, Token::AND);
            case '|':
                if (p + 1 < end && p[1] == '|') return make_tok(2, Token::LOGICAL_OR);
                else return make_tok(1, Token::OR);
            case '!':
                if (p + 1 >= end) error("Unexpected end");
                if (p[1] == '=') return make_tok(2, Token::CMP_NE);
                else return make_tok(1, Token::LOGICAL_NOT);
            case '=':
                if (p + 1 >= end) error("Unexpected end");
                if (p[1] == '=') return make_tok(2, Token::CMP_EQ);
                if (p[1] == '>') return make_tok(2, Token::HASH_KEY_VALUE_SEP);
                else return make_tok(1, Token::UNKNOWN);
            case '<':
                if (p + 1 >= end) error("Unexpected end");
                if (p[1] == '<') return make_tok(2, Token::LSHIFT);
                if (p[1] == '=')
                {
                    if (p + 2 >= end) error("Unexpected end");
                    if (p[2] == '>') return make_tok(3, Token::CMP);
                    else return make_tok(2, Token::CMP_LE);
                }
                else return make_tok(1, Token::CMP_LT);
            case '>':
                if (p + 1 >= end) error("Unexpected end");
                if (p[1] == '>') return make_tok(2, Token::RSHIFT);
                if (p[1] == '=') return make_tok(2, Token::CMP_GE);
                else return make_tok(1, Token::CMP_GT);
            case '@':
                ++p;
                t.str = symbol_str();
                t.type = Token::ATTR_NAME;
                return t;
            default:
                if (is_symbol_start_chr(peek)) return symbol(start);
                else if (is_digit(peek)) return number(start, false);
                else return t.type = Token::UNKNOWN, t;
            }
        }
        Token Lexer::next_str_interp(char delim)
        {
            auto t = token(Token::END);
            auto start = p;
            if (p == end) return ++p, t;
            else if (p > end) error("Unexpected end");

            if (*p == delim) return ++p, t.type = Token::STRING_DELIM, t;
            if (p + 1 < end && p[0] == '#' && p[1] == '{')
            {
                p += 2;
                t.type = Token::STRING_INTERP_START;
                return t;
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

            t.type = Token::STRING_TEXT;
            t.str = std::move(str);
            return t;
        }
        bool Lexer::peek_space()const
        {
            return p < end && (*p == ' ' || *p == '\r' || *p == '\r' || *p == '\n');
        }
        void Lexer::skip_ws()
        {
            while (p < end)
            {
                if (try_newline()) continue;
                if (*p == ' ' || *p == '\t') ++p;
                else break;
            }
        }
        
        Token Lexer::symbol(const char *start)
        {
            assert(is_symbol_start_chr(*p));
            auto t = token(Token::SYMBOL);
            t.pos = start;
            t.str = symbol_str();

            if (p < end)
            {
                if (*p == ':' && (p + 1 >= end || p[1] != ':')) //symbol:: is SYMBOL + CONST_NAV
                {
                    ++p;
                    t.type = Token::HASH_SYMBOL;
                }
                else if(*p == '?') //trailing '?' is part of method symbols
                {
                    t.str += '?';
                    ++p;
                }
            }
            
            return t;
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
            auto t = token(Token::NUMBER);
            t.pos = tok_start;

            auto start = negative ? p - 1 : p;
            while (p < end && is_digit(*p)) ++p;

            if (p + 1 < end && p[0] == '.' && is_digit(p[1])) //decimal
            {
                p += 2;
                while (p < end && is_digit(*p)) ++p;
            }
            t.str = std::string(start, p - start);
            return t;
        }

    }
}
