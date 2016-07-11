#include <boost/test/unit_test.hpp>
#include "expression/Parser.hpp"
#include "expression/Ast.hpp"
#include "expression/Lexer.hpp"
#include "expression/Scope.hpp"
#include "types/Number.hpp"
#include "Error.hpp"

using namespace slim;
using namespace slim::expr;
BOOST_AUTO_TEST_SUITE(TestNumber)

std::string eval(const std::string &str, Scope &scope)
{
    FunctionTable functions;
    Lexer lexer(str);
    Parser parser(functions, lexer);
    auto expr = parser.full_expression();
    auto result = expr->eval(scope);
    return result->to_string();
}
std::string eval(const std::string &str)
{
    Scope scope;
    return eval(str, scope);
}

BOOST_AUTO_TEST_CASE(basic_methods)
{
    BOOST_CHECK_EQUAL("inf", make_value(INFINITY)->to_string());
    BOOST_CHECK_EQUAL("-inf", make_value(-INFINITY)->to_string());
    Scope scope;
    scope.set("inf", make_value(INFINITY));
    scope.set("ninf", make_value(-INFINITY));
    scope.set("min", make_value(-std::numeric_limits<double>::max()));
    scope.set("max", make_value(std::numeric_limits<double>::max()));

    std::string min_s = make_value(-std::numeric_limits<double>::max())->to_string();
    std::string max_s = make_value(std::numeric_limits<double>::max())->to_string();

    BOOST_CHECK_EQUAL("0", eval("0.abs"));
    BOOST_CHECK_EQUAL("5", eval("5.0.abs"));
    BOOST_CHECK_EQUAL("5.5", eval("5.5.abs"));
    BOOST_CHECK_EQUAL("5", eval("-5.abs"));
    BOOST_CHECK_EQUAL("5", eval("-5.0.abs"));
    BOOST_CHECK_EQUAL("5.5", eval("-5.5.abs"));
    BOOST_CHECK_EQUAL("inf", eval("inf.abs", scope));
    BOOST_CHECK_EQUAL("inf", eval("ninf.abs", scope));

    BOOST_CHECK_EQUAL("inf", eval("inf.next_float", scope));
    BOOST_CHECK_EQUAL("inf", eval("max.next_float", scope));
    BOOST_CHECK_EQUAL(min_s, eval("ninf.next_float", scope));

    BOOST_CHECK_EQUAL(max_s, eval("inf.prev_float", scope));
    BOOST_CHECK_EQUAL("-inf", eval("ninf.prev_float", scope));
    BOOST_CHECK_EQUAL("-inf", eval("min.prev_float", scope));
}

BOOST_AUTO_TEST_CASE(operators)
{
    BOOST_CHECK_EQUAL("5", eval("+5"));
    BOOST_CHECK_EQUAL("-5", eval("-5"));
    BOOST_CHECK_EQUAL("false", eval("!5"));

    BOOST_CHECK_EQUAL("5", eval("1 + 4"));
    BOOST_CHECK_EQUAL("5", eval("6 - 1"));
    BOOST_CHECK_EQUAL("6", eval("2 * 3"));
    BOOST_CHECK_EQUAL("6", eval("24 / 4"));
    BOOST_CHECK_EQUAL("6", eval("16 % 10"));
    BOOST_CHECK_EQUAL("9", eval("3 ** 2"));


    BOOST_CHECK_EQUAL("8", eval("2 << 2"));
    BOOST_CHECK_EQUAL("1", eval("2 >> 1"));
    BOOST_CHECK_EQUAL("0", eval("2 >> 3"));
    BOOST_CHECK_EQUAL("2", eval("7 & 2"));
    BOOST_CHECK_EQUAL("7", eval("6 | 3"));
    BOOST_CHECK_EQUAL("10", eval("8 ^ 2"));
    BOOST_CHECK_EQUAL("8", eval("10 ^ 2"));
    BOOST_CHECK_EQUAL("-9", eval("~8"));
}

BOOST_AUTO_TEST_CASE(rounding)
{
    BOOST_CHECK_EQUAL("0", eval("0.ceil"));
    BOOST_CHECK_EQUAL("6", eval("5.4.ceil"));
    BOOST_CHECK_EQUAL("6", eval("5.6.ceil"));
    BOOST_CHECK_EQUAL("-5", eval("-5.3.ceil"));
    BOOST_CHECK_EQUAL("-5", eval("-5.6.ceil"));

    BOOST_CHECK_EQUAL("0", eval("0.floor"));
    BOOST_CHECK_EQUAL("5", eval("5.4.floor"));
    BOOST_CHECK_EQUAL("5", eval("5.6.floor"));
    BOOST_CHECK_EQUAL("-6", eval("-5.3.floor"));
    BOOST_CHECK_EQUAL("-6", eval("-5.6.floor"));

    BOOST_CHECK_EQUAL("0", eval("0.round"));
    BOOST_CHECK_EQUAL("5", eval("5.4.round"));
    BOOST_CHECK_EQUAL("6", eval("5.6.round"));
    BOOST_CHECK_EQUAL("-5", eval("-5.3.round"));
    BOOST_CHECK_EQUAL("-6", eval("-5.6.round"));

    BOOST_CHECK_EQUAL("0", eval("0.round(2)"));
    BOOST_CHECK_EQUAL("5.4", eval("5.4.round(2)"));
    BOOST_CHECK_EQUAL("5.6", eval("5.63.round(2)"));
    BOOST_CHECK_EQUAL("5.7", eval("5.66.round(2)"));
    BOOST_CHECK_EQUAL("-5.3", eval("-5.3.round(2)"));
    BOOST_CHECK_EQUAL("-5.6", eval("-5.63.round(2)"));
    BOOST_CHECK_EQUAL("-5.7", eval("-5.66.round(2)"));

    BOOST_CHECK_EQUAL("0", eval("0.round(-1)"));
    BOOST_CHECK_EQUAL("5", eval("5.4.round(-1)"));
    BOOST_CHECK_EQUAL("6", eval("5.63.round(-1)"));
    BOOST_CHECK_EQUAL("6", eval("5.66.round(-1)"));
    BOOST_CHECK_EQUAL("-5", eval("-5.3.round(-1)"));
    BOOST_CHECK_EQUAL("-6", eval("-5.63.round(-1)"));
    BOOST_CHECK_EQUAL("-6", eval("-5.66.round(-1)"));

    BOOST_CHECK_EQUAL("0", eval("0.round(-1)"));
    BOOST_CHECK_EQUAL("50", eval("54.round(-1)"));
    BOOST_CHECK_EQUAL("500", eval("543.round(-1)"));
    BOOST_CHECK_EQUAL("600", eval("566.round(-1)"));
    BOOST_CHECK_EQUAL("-500", eval("-539.round(-1)"));
    BOOST_CHECK_EQUAL("-500", eval("-543.round(-1)"));
    BOOST_CHECK_EQUAL("-600", eval("-566.round(-1)"));
}

BOOST_AUTO_TEST_SUITE_END()

