#include "Template.hpp"
#include "template/Lexer.hpp"
#include "template/Parser.hpp"
#include <fstream>

namespace slim
{
    Template parse_template(const char *str, size_t len, const FunctionTable &functions)
    {
        tpl::Lexer lexer(str, str + len);
        tpl::Parser parser(lexer, functions);
        return parser.parse();
    }

    Template parse_template(const std::string &source, const FunctionTable &functions)
    {
        return parse_template(source.c_str(), source.size(), functions);
    }

    Template parse_template_file(const std::string &path, const FunctionTable &functions)
    {
        std::ifstream is(path, std::ios::in | std::ios::binary);
        is.seekg(0, std::ios::end);
        size_t size = is.tellg();
        std::unique_ptr<char[]> str(new char[size]);
        is.seekg(0, std::ios::beg);
        is.read(str.get(), size);

        if (is.tellg() != (std::streampos)size) throw std::runtime_error("Failed to load " + path);

        return parse_template(str.get(), size, functions);
    }
}
