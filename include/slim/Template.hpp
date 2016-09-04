#pragma once
#include "template/Template.hpp"
#include "expression/Scope.hpp"
#include "BuiltinFunctions.hpp"

namespace slim
{
    Template parse_template(const char *str, size_t len);
    Template parse_template(const std::string &source);
    Template parse_template_file(const std::string &path);
}
