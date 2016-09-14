#include <boost/test/unit_test.hpp>
#include "expression/Parser.hpp"
#include "expression/Ast.hpp"
#include "expression/Lexer.hpp"
#include "expression/Scope.hpp"
#include "types/Symbol.hpp"

using namespace slim;
using namespace slim::expr;

ObjectPtr eval2(const std::string &str)
{
    Lexer lexer(str);
    expr::LocalVarNames vars;
    Parser parser(vars, lexer);
    auto expr = parser.full_expression();
    ScopeAttributes attrs;
    Scope scope(attrs);
    return expr->eval(scope);
}
std::string eval(const std::string &str)
{
    return eval2(str)->inspect();
}

BOOST_AUTO_TEST_SUITE(TestSymbol)

BOOST_AUTO_TEST_CASE(test)
{
    BOOST_CHECK(symbol(make_value("sym")) == symbol("sym"));
    BOOST_CHECK(symbol("sym") == eval2(":sym"));
    BOOST_CHECK_EQUAL("sym", symbol("sym")->c_str());
    BOOST_CHECK_EQUAL(":sym", eval(":sym"));
    BOOST_CHECK_EQUAL("\"sym\"", eval(":sym.to_s"));
    BOOST_CHECK_EQUAL("true", eval(":a == :a"));
    BOOST_CHECK_EQUAL("false", eval(":a == :b"));
    BOOST_CHECK_EQUAL("0", eval(":a <=> :a"));
    BOOST_CHECK_EQUAL("-1", eval(":a <=> :b"));
    BOOST_CHECK_EQUAL("1", eval(":b <=> :a"));
}
BOOST_AUTO_TEST_SUITE_END()
