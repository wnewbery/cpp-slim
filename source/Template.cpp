#include "Template.hpp"
#include "template/Lexer.hpp"
#include "template/Parser.hpp"
#include <fstream>

namespace slim
{
    Template parse_template(const char *str, size_t len)
    {
        tpl::Lexer lexer(str, str + len);
        tpl::Parser parser(lexer);
        return parser.parse();
    }

    Template parse_template(const std::string &source)
    {
        return parse_template(source.c_str(), source.size());
    }

    Template parse_template_file(const std::string &path)
    {
        std::ifstream is(path, std::ios::in | std::ios::binary);
        is.seekg(0, std::ios::end);
        size_t size = is.tellg();
        std::unique_ptr<char[]> str(new char[size]);
        is.seekg(0, std::ios::beg);
        is.read(str.get(), size);

        if (is.tellg() != (std::streampos)size) throw std::runtime_error("Failed to load " + path);

        return parse_template(str.get(), size);
    }
}
