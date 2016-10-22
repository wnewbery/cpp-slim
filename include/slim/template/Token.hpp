#pragma once
#include "../BaseLexer.hpp"
namespace slim
{
    namespace tpl
    {
        /**A token in the template source, as read by Lexer. */
        struct Token : public BaseToken
        {
            enum Type
            {
                /**End of input. */
                END,
                /**End of line (if not END, or by defintion consumed by previous symbol). */
                EOL,
                /**Indentation at the start of a line.*/
                INDENT,
                /**An element, atttribute, id or class name.
                 * Allows numbers, letters, underscores and hyphens.
                 */
                NAME,
                /**'#'*/
                TAG_ID,
                /**'.'*/
                TAG_CLASS,
                /**'(', '[' or '{'*/
                ATTR_WRAPPER_START,
                /**'>' symbol after an element name.*/
                ADD_TRAILING_WHITESPACE,
                /**'<' symbol after an element name.*/
                ADD_LEADING_WHITESPACE,
                /**'<>' symbol after an element name.*/
                ADD_LEADING_AND_TRAILING_WHITESPACE,
                /**Text content, either a '`' or '|' line, or text at the end of a tag line,
                 * or code after a '='.
                 */
                TEXT_CONTENT,
                /**'|' at start of line. */
                TEXT_LINE,
                /** '\'' at start of line. */
                TEXT_LINE_WITH_TRAILING_SPACE,
                /** '<' */
                HTML_LINE,
                /** '/' */
                COMMENT_LINE,
                /** '/!' */
                HTML_COMMENT_LINE,
                /** '=' */
                OUTPUT_LINE,
                /** name=. '=' is not included in str. */
                ATTR_NAME,
                /**'*'*/
                SPLAT_ATTR,

                /** '-' control line.*/
                CONTROL_LINE,
                /** 'if' */
                IF,
                /** 'unless' */
                UNLESS,
                /** 'elsif' */
                ELSIF,
                /** 'else' */
                ELSE,
                /** '' */
                EACH_START
            };
            Type type;

            Token() {}
            Token(int line, int offset, const char *pos, std::string &&str, Type type)
                : BaseToken(line, offset, pos, std::move(str)), type(type)
            {}
        };
    }
}
