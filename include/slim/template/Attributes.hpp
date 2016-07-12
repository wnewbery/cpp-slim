#pragma once
#include <string>
#include <vector>
namespace slim
{
    namespace tpl
    {
        inline std::string merge_attr_values(const std::vector<std::string> &vec)
        {
            std::string out;
            for (size_t i = 0; i < vec.size(); ++i)
            {
                if (i > 0) out += ' ';
                out += vec[i];
            }
            return out;
        }
        inline std::string attr_str(const std::string &name, const std::vector<std::string> &vec)
        {
            return ' ' + name + '=' + '"' + merge_attr_values(vec) + '"';
        }
    }
}
