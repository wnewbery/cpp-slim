#pragma once
#include <string>
namespace slim
{
    namespace expr
    {
        struct Token
        {
            enum Type
            {
                END = 0,

                STRING,
                NUMBER,
                SYMBOL,

                LPAREN,
                RPAREN,
                COMMA,
                DOT,

                PLUS,
                MINUS,
                MUL,
                DIV,
                MOD,

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
        };
    }
}
