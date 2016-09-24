#include <boost/test/unit_test.hpp>
#include "expression/Parser.hpp"
#include "expression/Ast.hpp"
#include "expression/Lexer.hpp"
#include "expression/Scope.hpp"
#include "types/Array.hpp"
#include "types/Number.hpp"
#include "types/String.hpp"
#include "../TestAccumulator.hpp"
#include "Error.hpp"

using namespace slim;
using namespace slim::expr;
BOOST_AUTO_TEST_SUITE(TestEnumerable)


std::string eval(Ptr<ViewModel> model, const std::string &str)
{
    Scope scope(model);
    Lexer lexer(str);
    expr::LocalVarNames vars;
    for (auto x : scope) vars.add(x.first->str());
    Parser parser(vars, lexer);
    auto expr = parser.full_expression();
    auto result = expr->eval(scope);
    return result->inspect();
}
std::string eval(const std::string &str)
{
    return eval(create_view_model(), str);
}

BOOST_AUTO_TEST_CASE(all)
{
    BOOST_CHECK_EQUAL("true", eval("[].all?"));
    BOOST_CHECK_EQUAL("true", eval("[1, 2, 3].all?"));
    BOOST_CHECK_EQUAL("false", eval("[1, 2, nil, 3].all?"));

    BOOST_CHECK_EQUAL("true", eval("[1, 2, nil, 3].all? {|x| x != 5}"));
    BOOST_CHECK_EQUAL("false", eval("[5, 2, nil, 3].all? {|x| x != 5}"));
}

BOOST_AUTO_TEST_CASE(any)
{
    BOOST_CHECK_EQUAL("false", eval("[].any?"));
    BOOST_CHECK_EQUAL("false", eval("[nil, false, nil].any?"));
    BOOST_CHECK_EQUAL("true", eval("[1, 2, 3].any?"));
    BOOST_CHECK_EQUAL("true", eval("[1, 2, nil, 3].any?"));

    BOOST_CHECK_EQUAL("true", eval("[1, 2, nil, 3].any? {|x| x != 5}"));
    BOOST_CHECK_EQUAL("true", eval("[5, 2, nil, 3].any? {|x| x != 5}"));
    BOOST_CHECK_EQUAL("false", eval("[5, 5].any? {|x| x != 5}"));
}

BOOST_AUTO_TEST_CASE(count)
{
    BOOST_CHECK_EQUAL("0", eval("[].count"));
    BOOST_CHECK_EQUAL("4", eval("[1,2,3,5].count"));
    BOOST_CHECK_EQUAL("4", eval("[1,2,3,5].each.count"));

    BOOST_CHECK_EQUAL("1", eval("[1,2,3,5].count 5"));
    BOOST_CHECK_EQUAL("2", eval("[1,5,3,5].count 5"));

    BOOST_CHECK_EQUAL("2", eval("[1,2,3,5].count {|x| x > 2}"));

    BOOST_CHECK_THROW(eval("[].count 1, 2"), ArgumentCountError);
}

BOOST_AUTO_TEST_CASE(drop)
{
    BOOST_CHECK_EQUAL("[]", eval("[].drop 1"));
    BOOST_CHECK_EQUAL("[2, 3, 5]", eval("[1,2,3,5].drop 1"));
    BOOST_CHECK_EQUAL("[5]", eval("[1,2,3,5].drop 3"));
    BOOST_CHECK_EQUAL("[]", eval("[1].drop 3"));
}

BOOST_AUTO_TEST_CASE(drop_while)
{
    BOOST_CHECK_EQUAL("[]", eval("[].drop_while {|x| x < 5}"));
    BOOST_CHECK_EQUAL("[5]", eval("[1,2,3,5].drop_while {|x| x < 5}"));
    BOOST_CHECK_EQUAL("[5, 3, 2]", eval("[1,2,3,5,3,2].drop_while {|x| x < 5}"));

    BOOST_CHECK_EQUAL("[5, 3, 2]", eval("[1,2,3,5,3,2].drop_while.each {|x| x < 5}"));
    BOOST_CHECK_EQUAL("[5, 3, 2]", eval("[1,2,3,5,3,2].each.drop_while {|x| x < 5}"));
}

BOOST_AUTO_TEST_CASE(map)
{
    BOOST_CHECK_EQUAL("[2, 4, 8, 10]", eval("[1,2,4,5].map{|x| x*2}"));
    BOOST_CHECK_EQUAL("[2, 4, 8, 10]", eval("[1,2,4,5].collect{|x| x*2}"));

    BOOST_CHECK_EQUAL("[2, 4, 8, 10]", eval("[1,2,4,5].each.map{|x| x*2}"));
    BOOST_CHECK_EQUAL("[2, 4, 8, 10]", eval("[1,2,4,5].map.each{|x| x*2}"));
    BOOST_CHECK_EQUAL("[2, 4, 8, 10]", eval("[1,2,4,5].map.map{|x| x*2}"));
}

BOOST_AUTO_TEST_CASE(to_a)
{
    BOOST_CHECK_EQUAL("[1, 2, 3]", eval("[1, 2, 3].to_a"));
    BOOST_CHECK_EQUAL("[[1, 6], 2, [3]]", eval("[[1, 6], 2, [3]].to_a"));
}

BOOST_AUTO_TEST_CASE(to_h)
{
    BOOST_CHECK_EQUAL("{1 => 6, 2 => 7, 3 => 8}", eval("[[1, 6], [2, 7], [3, 8]].to_h"));
    BOOST_CHECK_THROW(eval("[1].to_h"), TypeError);
    BOOST_CHECK_THROW(eval("[[]].to_h"), ArgumentError);
    BOOST_CHECK_THROW(eval("[[1]].to_h"), ArgumentError);
    BOOST_CHECK_THROW(eval("[[1, 2, 3]].to_h"), ArgumentError);
}

BOOST_AUTO_TEST_SUITE_END()

