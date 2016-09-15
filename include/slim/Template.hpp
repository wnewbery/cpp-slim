#pragma once
#include "template/Template.hpp"
#include "expression/Scope.hpp"
#include "Function.hpp"

namespace slim
{
    /**Parses a template from a source string.*/
    Template parse_template(const char *str, size_t len);
    /**Parses a template from a source string.*/
    Template parse_template(const std::string &source);
    /**Parses a template from a source file.*/
    Template parse_template_file(const std::string &path);
}
