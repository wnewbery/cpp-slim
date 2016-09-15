#pragma once
#include "Ast.hpp"
#include "Scope.hpp"
namespace slim
{
    //Note: This might change in the future if not evaluating the AST directly
    typedef expr::ExpressionNode Expression;
    typedef expr::ExpressionNodePtr ExpressionPtr;
}
