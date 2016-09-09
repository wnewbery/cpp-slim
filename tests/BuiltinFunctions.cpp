#include <boost/test/unit_test.hpp>
#include "expression/Parser.hpp"
#include "expression/Ast.hpp"
#include "expression/Lexer.hpp"
#include "expression/Scope.hpp"
#include "BuiltinFunctions.hpp"
#include "Error.hpp"

using namespace slim;
using namespace slim::expr;
BOOST_AUTO_TEST_SUITE(TestBuiltinFunctions)

std::string eval(const std::string &str, Scope &scope)
{
    Lexer lexer(str);
    expr::LocalVarNames vars;
    for (auto x : scope) vars.add(x.first->str());
    Parser parser(vars, lexer);
    auto expr = parser.full_expression();
    auto result = expr->eval(scope);
    return result->to_string();
}
std::string eval(const std::string &str)
{
    ScopeAttributes attrs;
    Scope scope(BUILTIN_FUNCTIONS, attrs);
    return eval(str, scope);
}

BOOST_AUTO_TEST_CASE(binding)
{
    BOOST_CHECK_EQUAL("2", eval("sqrt(4)"));
    BOOST_CHECK_THROW(eval("sqrt()"), InvalidArgument);
    BOOST_CHECK_THROW(eval("sqrt(4, 5)"), InvalidArgument);
    BOOST_CHECK_THROW(eval("sqrt('Not a number')"), InvalidArgument);
}

BOOST_AUTO_TEST_SUITE_END()

