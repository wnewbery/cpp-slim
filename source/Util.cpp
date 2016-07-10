#include "Util.hpp"
namespace slim
{
    std::string html_encode(const std::string &str)
    {
        std::string buf;
        buf.reserve(str.size());
        for (auto c : str)
        {
            switch (c)
            {
            case '&': buf += "&amp;"; break;
            case '<': buf += "&lt;"; break;
            case '>': buf += "&gt;"; break;
            case '"': buf += "&quot;"; break;
            case '\'': buf += "&#39;"; break;
            default: buf += c; break;
            }
        }
        return buf;
    }
}
