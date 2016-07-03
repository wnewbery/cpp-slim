#pragma once
#include <string>
namespace slim
{
    namespace expr
    {
        /**@brief Token read by Lexer.
         * Each token has a type, and possibly an associated source string (e.g. number).
         */
        struct Token
        {
            enum Type
            {
                END = 0,
                /**A string literal. Value stored in str.*/
                STRING,
                /**A number literal. Unparsed value stored in str.*/
                NUMBER,
                /**A symbol.
                 * May be a value such as 'true', a variable name, function name, etc.
                 */
                SYMBOL,
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

                PLUS,
                MINUS,
                MUL,
                DIV,
                MOD,

                CMP,
                CMP_EQ,
                CMP_NE,
                CMP_LT,
                CMP_LE,
                CMP_GT,
                CMP_GE,

                LOGICAL_NOT,
                LOGICAL_AND,
                LOGICAL_OR
            };

            Token(Type type) : type(type), str() {}
            Token(Type type, const std::string &str) : type(type), str(str) {}

            Type type;
            std::string str;

            bool value_start()const
            {
                switch(type)
                {
                case STRING:
                case NUMBER:
                case SYMBOL:
                case LPAREN:
                    return true;
                default:
                    return false;
                }
            }
        };
    }
}
