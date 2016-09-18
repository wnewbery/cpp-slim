#pragma once
#include <string>
#include <cassert>
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
            , begin(str), p(str), end(end), line_begin(str), _line(1)
        {
            assert(begin <= end);
            assert(begin <= p && p <= end);
        }
        BaseLexer(const char *str, size_t len) : BaseLexer(str, str + len) {}
        explicit BaseLexer(const std::string &str) : BaseLexer(str.data(), str.size()) {}

        const std::string& file_name()const { return _file_name; }
        /**Set the file name for error reporting.*/
        void file_name(const std::string &file_name)
        {
            _file_name = file_name;
        }

        /**Override the reported position.
         * Useful when parsing embedded languages (e.g. scripts in templates) and already
         * determined the range to parse.
         */
        void set_reported_pos(int line, int offset)
        {
            _line = line;
            line_begin = p - offset + 1;
        }
        /**Get the current line.*/
        int line()const { return _line; }
        /**Get the current line offset.*/
        int line_offset()const { return (int)(p - line_begin) + 1; }
    protected:
        std::string _file_name;
        const char *begin, *p, *end, *line_begin;
        int _line;

        /**Create a basic token with positional info and a type.*/
        Token token(typename Token::Type type)const
        {
            return{ _line, line_offset(), p, std::string(), type };
        }


        /**If next is a newline (\\r, \\n, or \\r\\n), consume it and return true.*/
        bool try_newline()
        {
            if (*p == '\n')
            {
                ++_line;
                ++p;
                line_begin = p;
                return true;
            }
            else if (*p == '\r')
            {
                if (p + 1 < end && p[1] == '\n')
                {
                    ++_line;
                    p += 2;
                    line_begin = p;
                    return true;
                }
                else
                {
                    ++_line;
                    ++p;
                    line_begin = p;
                    return true;
                }
            }
            return false;
        }
    };
}
