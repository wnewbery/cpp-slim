#include "Unicode.hpp"
#include <stdexcept>

namespace slim
{
    /**Decode 10xx xxx element.*/
    uint32_t utf8_trailing(unsigned char el)
    {
        if ((el & 0xC0) != 0x80) throw std::runtime_error("Invalid UTF-8 trailing element.");
        return el & 0x3F;
    }

    void utf8_decode(const char *_str, uint32_t *out, unsigned *elements)
    {
        auto str = (const unsigned char*)_str;
        auto c0 = str[0];
        if (c0 < 0x80) //0xxx xxxx
        {
            *elements = 1;
            *out = c0;
        }
        else if (c0 < 0xC0) //10xx  xxxx (invalid)
        {
            throw std::runtime_error("Unexpected UTF-8 trailing element");
        }
        else if (c0 < 0xE0) // 110x xxxx
        {
            *elements = 2;
            *out = ((c0 & 0x1F) << 6) | utf8_trailing(str[1]);
        }
        else if (c0 < 0xF0) // 1110 xxxx
        {
            *elements = 3;
            *out = ((c0 & 0x0F) << 12)
                | (utf8_trailing(str[1]) << 6)
                | utf8_trailing(str[2]);
        }
        else if (c0 < 0xF8) // 1111 0xxx
        {
            *elements = 4;
            *out = ((c0 & 0x07) << 18)
                | (utf8_trailing(str[1]) << 12)
                | (utf8_trailing(str[2]) << 6)
                | utf8_trailing(str[3]);
        }
        else // 1111 1xxx (invalid)
        {
            throw std::runtime_error("Invalid UTF-8 leading element");
        }
    }

    bool try_utf8_decode(const char *str, uint32_t *out, unsigned *elements)
    {
        try
        {
            utf8_decode(str, out, elements);
            return true;
        }
        catch (const std::exception &)
        {
            return false;
        }
    }

    bool utf8_is_leading(char c)
    {
        auto uc = (unsigned char)c;
        return uc < 0x80 || uc >= 0xC0;
    }

    unsigned short utf8_next_len(char c)
    {
        auto uc = (unsigned char)c;
        if (uc < 0xC0) return 1;
        else if (uc < 0xE0) return 2;
        else if (uc < 0xF0) return 3;
        else if (uc < 0xF8) return 4;
        else return 1;
    }
}
