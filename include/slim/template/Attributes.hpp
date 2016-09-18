#pragma once
#include <string>
#include <vector>
namespace slim
{
    namespace tpl
    {
        /**If a tag attribute has an array of values, this function builds the single string value
         * for the HTML/XML by space-seperating name.
         */
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
        /**Creates the HTML/XML string for a named attribute with a string value.*/
        inline std::string attr_str(const std::string &name, const std::vector<std::string> &vec)
        {
            return ' ' + name + '=' + '"' + merge_attr_values(vec) + '"';
        }
    }
}
