#pragma once
#include <string>
#include "Error.hpp"
namespace slim
{
    struct BaseToken
    {
        /**1-based line number in source.*/
        int line;
        /**1-based line element offset in source.*/
        int offset;
        /**Position in source of the start of this token.
         * This includes before any skipped whitespace, unlike line and offset.
         */
        const char *pos;
        /**Token string contents (symbol name, text, etc.)*/
        std::string str;

        BaseToken() {}
        BaseToken(int line, int offset, const char *pos, std::string &&str)
            : line(line), offset(offset), pos(pos), str(std::move(str)) {}
    };
    template<class Token>
    class BaseLexer
    {
    public:
        BaseLexer(const char *str, const char *end)
            : _file_name("unknown")
            , begin(str), p(str), end(end), line_begin(str), line(1)
        {}
        BaseLexer(const char *str, size_t len) : BaseLexer(str, str + len) {}
        BaseLexer(const std::string &str) : BaseLexer(str.data(), str.size()) {}

        const std::string& file_name()const { return _file_name; }
    protected:
        std::string _file_name;
        const char *begin, *p, *end, *line_begin;
        int line;

        /**Get the current line offset.*/
        int line_offset()const { return (int)(p - line_begin) + 1; }
        /**Create a basic token with positional info and a type.*/
        Token token(typename Token::Type type)const
        {
            return{ line, line_offset(), p, std::string(), type };
        }


        /**If next is a newline (\r, \n, or \r\n), consume it and return true.*/
        bool try_newline()
        {
            if (*p == '\n')
            {
                ++line;
                ++p;
                line_begin = p;
                return true;
            }
            else if (*p == '\r')
            {
                if (p + 1 < end && p[1] == '\n')
                {
                    ++line;
                    p += 2;
                    line_begin = p;
                    return true;
                }
                else
                {
                    ++line;
                    ++p;
                    line_begin = p;
                    return true;
                }
            }
            return false;
        }
    };
}
