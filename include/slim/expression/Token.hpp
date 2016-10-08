#pragma once
#include "../BaseLexer.hpp"
namespace slim
{
    namespace expr
    {
        /**@brief Token read by Lexer.
         * Each token has a type, and possibly an associated source string (e.g. number).
         */
        struct Token : public BaseToken
        {
            enum Type
            {
                END = 0,
                /**String start/terminator*/
                STRING_DELIM,
                /**Literal text in string.*/
                STRING_TEXT,
                /** #{ */
                STRING_INTERP_START,
                /**A non-negative number literal. Unparsed value stored in str.*/
                NUMBER,
                /**A symbol.
                 * May be a value such as 'true', a variable name, function name, etc.
                 */
                SYMBOL,
                /**An attribute name, using \@symbol.*/
                ATTR_NAME,
                /**A symbol followed directly by a colon. Used for hashes. e.g:
                 * key: "value"
                 */
                HASH_SYMBOL,
                /** '=>' symbol used to seperate hash literal key-values. */
                HASH_KEY_VALUE_SEP,

                LPAREN,
                RPAREN,
                L_SQ_BRACKET,
                R_SQ_BRACKET,
                L_CURLY_BRACKET,
                R_CURLY_BRACKET,
                COMMA,
                DOT,
                /**.. inclusive range.*/
                INCLUSIVE_RANGE,
                /**... exclusive range.*/
                EXCLUSIVE_RANGE,
                /**::, double colon for type::Const, e.g. Math::PI*/
                CONST_NAV,
                /**&.*/
                SAFE_NAV,
                COLON,
                /**?*/
                CONDITIONAL,

                PLUS,
                MINUS,
                MUL,
                POW,
                DIV,
                MOD,

                AND,
                OR,
                XOR,
                NOT,
                LSHIFT,
                RSHIFT,

                CMP,
                CMP_EQ,
                CMP_NE,
                CMP_LT,
                CMP_LE,
                CMP_GT,
                CMP_GE,

                LOGICAL_NOT,
                LOGICAL_AND,
                LOGICAL_OR,

                /**Empty token for unknown content.*/
                UNKNOWN
            };
            Type type;

            Token() {}
            Token(int line, int offset, const char *pos, std::string &&str, Type type)
                : BaseToken(line, offset, pos, std::move(str)), type(type)
            {}
        };
    }
}
