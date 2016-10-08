#include <boost/test/unit_test.hpp>
#include "expression/Parser.hpp"
#include "expression/Ast.hpp"
#include "expression/Lexer.hpp"
#include "expression/Scope.hpp"

using namespace slim;
using namespace slim::expr;
BOOST_AUTO_TEST_SUITE(TestRange)

std::string eval(const std::string &str)
{
    Lexer lexer(str);
    expr::LocalVarNames vars;
    Parser parser(vars, lexer);
    auto expr = parser.full_expression();
    auto model = create_view_model();
    Scope scope(model);
    return expr->eval(scope)->inspect();
}

BOOST_AUTO_TEST_CASE(base)
{
    BOOST_CHECK_EQUAL("\"0..10\"", eval("(0..10).inspect"));
    BOOST_CHECK_EQUAL("\"0..10\"", eval("(0..10).to_s"));
    BOOST_CHECK_EQUAL("\"0...10\"", eval("(0...10).inspect"));
    BOOST_CHECK_EQUAL("\"0...10\"", eval("(0...10).to_s"));
}

BOOST_AUTO_TEST_CASE(eq)
{
    BOOST_CHECK_EQUAL("true", eval("(0..10) == (0..10)"));
    BOOST_CHECK_EQUAL("true", eval("(0..10).hash == (0..10).hash"));
    BOOST_CHECK_EQUAL("true", eval("(0...10) == (0...10)"));
    BOOST_CHECK_EQUAL("true", eval("(0...10).hash == (0...10).hash"));
    BOOST_CHECK_EQUAL("false", eval("(0..11) == (0..10)"));
    BOOST_CHECK_EQUAL("false", eval("(0..11).hash == (0..10).hash"));
    BOOST_CHECK_EQUAL("false", eval("(1..10) == (0..10)"));
    BOOST_CHECK_EQUAL("false", eval("(1..10).hash == (0..10).hash"));
    BOOST_CHECK_EQUAL("false", eval("(0...10) == (0..10)"));
    BOOST_CHECK_EQUAL("false", eval("(0...10).hash == (0..10).hash"));
    BOOST_CHECK_EQUAL("false", eval("(0..10) == (0...11)"));
    BOOST_CHECK_EQUAL("false", eval("(0..10).hash == (0...11).hash"));
}

BOOST_AUTO_TEST_CASE(begin)
{
    BOOST_CHECK_EQUAL("0", eval("(0..10).begin"));
    BOOST_CHECK_EQUAL("2", eval("(2..10).begin"));
    BOOST_CHECK_EQUAL("2", eval("(2...10).begin"));
}

BOOST_AUTO_TEST_CASE(cover_q)
{
    BOOST_CHECK_EQUAL("true", eval("(0..10).cover? 0"));
    BOOST_CHECK_EQUAL("true", eval("(0..10).cover? 5"));
    BOOST_CHECK_EQUAL("true", eval("(0..10).cover? 10"));
    BOOST_CHECK_EQUAL("false", eval("(0...10).cover? 10"));
}

BOOST_AUTO_TEST_CASE(each)
{
    BOOST_CHECK_EQUAL("[0, 1, 2, 3, 4]", eval("(0..4).each.to_a"));
    BOOST_CHECK_EQUAL("[2, 3, 4]", eval("(2..4).each.to_a"));
    BOOST_CHECK_EQUAL("[2, 3]", eval("(2...4).each.to_a"));

    BOOST_CHECK_EQUAL("[4]", eval("(4..4).each.to_a"));
    BOOST_CHECK_EQUAL("[]", eval("(4...4).each.to_a"));
    BOOST_CHECK_EQUAL("[]", eval("(5...4).each.to_a"));
}

BOOST_AUTO_TEST_CASE(end)
{
    BOOST_CHECK_EQUAL("10", eval("(2..10).end"));
    BOOST_CHECK_EQUAL("11", eval("(2..11).end"));
    BOOST_CHECK_EQUAL("11", eval("(2...11).end"));
}

BOOST_AUTO_TEST_CASE(exclude_end_q)
{
    BOOST_CHECK_EQUAL("false", eval("(2..11).exclude_end?"));
    BOOST_CHECK_EQUAL("true", eval("(2...11).exclude_end?"));
}

BOOST_AUTO_TEST_CASE(first)
{
    BOOST_CHECK_EQUAL("2", eval("(2..11).first"));
    BOOST_CHECK_EQUAL("[2]", eval("(2..11).first 1"));
    BOOST_CHECK_EQUAL("[2, 3, 4, 5]", eval("(2..11).first 4"));
    BOOST_CHECK_EQUAL("[2, 3, 4]", eval("(2..4).first 3"));
    BOOST_CHECK_EQUAL("[2, 3]", eval("(2...4).first 3"));
}

BOOST_AUTO_TEST_CASE(include_q)
{
    BOOST_CHECK_EQUAL("true", eval("(0..10).include? 0"));
    BOOST_CHECK_EQUAL("true", eval("(0..10).include? 5"));
    BOOST_CHECK_EQUAL("true", eval("(0..10).include? 10"));
    BOOST_CHECK_EQUAL("false", eval("(0...10).include? 10"));
}

BOOST_AUTO_TEST_CASE(last)
{
    BOOST_CHECK_EQUAL("11", eval("(2..11).last"));
    BOOST_CHECK_EQUAL("11", eval("(2...11).last"));
    BOOST_CHECK_EQUAL("[10]", eval("(2...11).last 1"));
    BOOST_CHECK_EQUAL("[8, 9, 10, 11]", eval("(2..11).last 4"));
    BOOST_CHECK_EQUAL("[3, 4, 5]", eval("(2..5).last 3"));
    BOOST_CHECK_EQUAL("[2, 3, 4]", eval("(2...5).last 3"));
    BOOST_CHECK_EQUAL("[2, 3, 4, 5]", eval("(2..5).last 5"));
    BOOST_CHECK_EQUAL("[2, 3, 4]", eval("(2...5).last 5"));
}

BOOST_AUTO_TEST_CASE(max)
{
    BOOST_CHECK_EQUAL("11", eval("(2..11).max"));
    BOOST_CHECK_EQUAL("10", eval("(2...11).max"));
    BOOST_CHECK_EQUAL("11", eval("(11..11).max"));
    BOOST_CHECK_EQUAL("nil", eval("(11...11).max"));
    BOOST_CHECK_EQUAL("nil", eval("(6..5).max"));
    BOOST_CHECK_EQUAL("nil", eval("(6...6).max"));
}

BOOST_AUTO_TEST_CASE(min)
{
    BOOST_CHECK_EQUAL("2", eval("(2..11).min"));
    BOOST_CHECK_EQUAL("2", eval("(2...11).min"));
    BOOST_CHECK_EQUAL("11", eval("(11..11).min"));
    BOOST_CHECK_EQUAL("nil", eval("(11...11).min"));
    BOOST_CHECK_EQUAL("nil", eval("(6..5).min"));
    BOOST_CHECK_EQUAL("nil", eval("(6...6).min"));
}

BOOST_AUTO_TEST_CASE(size)
{
    BOOST_CHECK_EQUAL("1", eval("(2..2).size"));
    BOOST_CHECK_EQUAL("0", eval("(2...2).size"));
    BOOST_CHECK_EQUAL("9", eval("(2..10).size"));
    BOOST_CHECK_EQUAL("8", eval("(2...10).size"));
    BOOST_CHECK_EQUAL("0", eval("(3..2).size"));

    BOOST_CHECK_EQUAL("1", eval("(2..(2.5)).size"));
    BOOST_CHECK_EQUAL("1", eval("(2...(2.5)).size"));
}

BOOST_AUTO_TEST_CASE(step)
{
    BOOST_CHECK_EQUAL("[0, 1, 2]", eval("(0..2).step.to_a"));
    BOOST_CHECK_EQUAL("[0, 0.5, 1, 1.5, 2]", eval("(0..2).step(0.5).to_a"));
    BOOST_CHECK_EQUAL("[0, 0.5, 1, 1.5]", eval("(0...2).step(0.5).to_a"));
}

BOOST_AUTO_TEST_SUITE_END()

