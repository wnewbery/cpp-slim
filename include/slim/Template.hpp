#pragma once
#include "template/Template.hpp"
#include "expression/Scope.hpp"
#include "Function.hpp"

namespace slim
{
    Template parse_template(const char *str, size_t len, const FunctionTable &functions);
    Template parse_template(const std::string &source, const FunctionTable &functions);
    Template parse_template_file(const std::string &path, const FunctionTable &functions);
}
