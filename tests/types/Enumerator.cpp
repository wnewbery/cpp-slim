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
BOOST_AUTO_TEST_SUITE(TestEnumerator)


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

BOOST_AUTO_TEST_CASE(each)
{
    auto model = create_view_model();
    auto data = create_object<TestAccumulator>();
    model->set_attr("data", data);

    eval(model, "[5, 6, 9].each.each{|x| @data.store x}");
    BOOST_CHECK_EQUAL("[5, 6, 9]", data->check());
}

BOOST_AUTO_TEST_CASE(with_index)
{
    auto model = create_view_model();
    auto data = create_object<TestAccumulator>();
    model->set_attr("data", data);

    BOOST_CHECK_EQUAL("[]", eval("[].each.with_index.to_a"));
    BOOST_CHECK_EQUAL("[[1, 0], [5, 1], [3, 2]]", eval("[1, 5, 3].each.with_index.to_a"));
    BOOST_CHECK_EQUAL("[[1, 4], [5, 5], [3, 6]]", eval("[1, 5, 3].each.with_index(4).to_a"));


    eval(model, "[5, 6, 9].each.with_index{|x, i| @data.store i}");
    BOOST_CHECK_EQUAL("[0, 1, 2]", data->check());

    eval(model, "[5, 6, 9].each.with_index(4){|x, i| @data.store i}");
    BOOST_CHECK_EQUAL("[4, 5, 6]", data->check());


    BOOST_CHECK_THROW(eval("[].each.with_index 0, 1, 2"), ArgumentCountError);
}

BOOST_AUTO_TEST_CASE(enumerable)
{
    BOOST_CHECK_EQUAL("[1, 2, 3]", eval("[1, 2, 3].each.to_a"));
    BOOST_CHECK_EQUAL("[[1, 6], 2, [3]]", eval("[[1, 6], 2, [3]].each.to_a"));

    BOOST_CHECK_EQUAL("{1 => 6, 2 => 7, 3 => 8}", eval("[[1, 6], [2, 7], [3, 8]].each.to_h"));
    BOOST_CHECK_THROW(eval("[1].each.to_h"), TypeError);
    BOOST_CHECK_THROW(eval("[[]].each.to_h"), ArgumentError);
    BOOST_CHECK_THROW(eval("[[1]].each.to_h"), ArgumentError);
    BOOST_CHECK_THROW(eval("[[1, 2, 3]].each.to_h"), ArgumentError);
}

BOOST_AUTO_TEST_SUITE_END()

