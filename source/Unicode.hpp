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
}
