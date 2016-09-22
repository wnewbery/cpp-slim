#pragma once
#include <cstdint>

namespace slim
{
    /**Decodes a UTF8 code point from the null terminated string str.
      * @param str The null terminated string.
      * @param out Set to the decoded code point.
      * @param elements Number of elements in str consumed for the codepoint.
      * @throw std::runtime_error if str is an invalid codepoint.
      */
    void utf8_decode(const char *str, uint32_t *out, unsigned *elements);
    /**utf8_decode but returns false instead of throwing.*/
    bool try_utf8_decode(const char *str, uint32_t *out, unsigned *elements);
    /**Returns true if the element is a UTF-8 leading element.*/
    bool utf8_is_leading(char c);
    /**Returns the byte length of the UTF-8 codepoint based on a leading element.*/
    unsigned short utf8_next_len(char c);
}
