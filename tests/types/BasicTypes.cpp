#include <boost/test/unit_test.hpp>
#include "Value.hpp"
#include "types/Symbol.hpp"
#include "expression/Parser.hpp"
#include "expression/Ast.hpp"
#include "expression/Lexer.hpp"
#include "expression/Scope.hpp"

using namespace slim;
using namespace slim::expr;
BOOST_AUTO_TEST_SUITE(TestBasicTypes)

std::string eval(const std::string &str)
{
    FunctionTable functions;
    Lexer lexer(str);
    Parser parser(functions, lexer);
    auto expr = parser.full_expression();
    Scope scope;
    auto result = expr->eval(scope);
    return result->inspect();
}

BOOST_AUTO_TEST_CASE(boolean)
{
    auto true_val = make_value(true);
    auto false_val = make_value(false);

    BOOST_CHECK_EQUAL(true_val, TRUE_VALUE);
    BOOST_CHECK_EQUAL(false_val, FALSE_VALUE);

    BOOST_CHECK_EQUAL("Boolean", Boolean::TYPE_NAME);
    BOOST_CHECK_EQUAL("Boolean", true_val->type_name());
    BOOST_CHECK_EQUAL("true", true_val->to_string());
    BOOST_CHECK_EQUAL("false", false_val->to_string());
    BOOST_CHECK_EQUAL(true, true_val->is_true());
    BOOST_CHECK_EQUAL(false, false_val->is_true());
    
    BOOST_CHECK_EQUAL("\"true\"", eval("true.to_s"));
    BOOST_CHECK_EQUAL("\"true\"", eval("true.inspect"));
    BOOST_CHECK_EQUAL("1", eval("true.to_i"));
    BOOST_CHECK_EQUAL("0", eval("false.to_i"));
}

BOOST_AUTO_TEST_CASE(nil)
{
    auto nil_val = create_object<Nil>();

    BOOST_CHECK_EQUAL(nil_val, NIL_VALUE);

    BOOST_CHECK_EQUAL("Nil", Nil::TYPE_NAME);
    BOOST_CHECK_EQUAL("Nil", nil_val->type_name());
    BOOST_CHECK_EQUAL("nil", nil_val->to_string());
    BOOST_CHECK_EQUAL(false, nil_val->is_true());
    BOOST_CHECK_EQUAL("\"nil\"", eval("nil.to_s"));
    BOOST_CHECK_EQUAL("\"nil\"", eval("nil.inspect"));
    BOOST_CHECK_EQUAL("0", eval("nil.to_i"));
}

BOOST_AUTO_TEST_CASE(number)
{
    auto a = make_value(5.5);
    auto b = make_value(0.0);
    auto c = make_value(-0.5);

    BOOST_CHECK_EQUAL("Number", Number::TYPE_NAME);
    BOOST_CHECK_EQUAL("Number", a->type_name());
    BOOST_CHECK_EQUAL("5.5", a->to_string());
    BOOST_CHECK_EQUAL("0", b->to_string());
    BOOST_CHECK_EQUAL("-0.5", c->to_string());
    BOOST_CHECK_EQUAL(true, a->is_true());
    BOOST_CHECK_EQUAL(false, b->is_true());
    BOOST_CHECK_EQUAL(true, c->is_true());
    BOOST_CHECK_THROW(eval("5[0]"), NoSuchMethod);
}

BOOST_AUTO_TEST_CASE(string)
{
    auto a = make_value("");
    auto b = make_value("a");
    auto c = make_value("test");

    BOOST_CHECK_EQUAL("String", String::TYPE_NAME);
    BOOST_CHECK_EQUAL("String", a->type_name());
    BOOST_CHECK_EQUAL("", a->to_string());
    BOOST_CHECK_EQUAL("a", b->to_string());
    BOOST_CHECK_EQUAL("test", c->to_string());
    BOOST_CHECK_EQUAL("\"\"", a->inspect());
    BOOST_CHECK_EQUAL("\"a\"", b->inspect());
    BOOST_CHECK_EQUAL("\"test\"", c->inspect());
    BOOST_CHECK_EQUAL(false, a->is_true());
    BOOST_CHECK_EQUAL(true, b->is_true());
    BOOST_CHECK_EQUAL(true, c->is_true());
    BOOST_CHECK_EQUAL("\"t\"", eval("'test'[0]"));
}

BOOST_AUTO_TEST_CASE(symbol)
{
    auto a = slim::symbol("a");
    auto b = slim::symbol("b");
    auto a2 = slim::symbol("a");
    BOOST_CHECK(a == a2);
    BOOST_CHECK(a != b);

    BOOST_CHECK_EQUAL(":a", a->inspect());
    BOOST_CHECK_EQUAL("a", a->to_string());

    BOOST_CHECK(a.get() != (Object*)make_value("a").get());
    BOOST_CHECK(a.get() == (Object*)make_value("a")->to_sym().get());
    BOOST_CHECK(b.get() == (Object*)make_value("b")->to_sym().get());
    BOOST_CHECK(a.get() != (Object*)make_value("b")->to_sym().get());
}

BOOST_AUTO_TEST_CASE(to_f)
{
    BOOST_CHECK_EQUAL(5.5, make_value(5.5)->to_f()->get_value());
    BOOST_CHECK_EQUAL(5.5, make_value("5.5")->to_f()->get_value());
    BOOST_CHECK_EQUAL(5.5, make_value("5.5x")->to_f()->get_value());
    BOOST_CHECK_EQUAL(0.0, make_value("x")->to_f()->get_value());
    BOOST_CHECK_EQUAL(1.0, make_value(true)->to_f()->get_value());
    BOOST_CHECK_EQUAL(0.0, make_value(false)->to_f()->get_value());
    BOOST_CHECK_EQUAL(0.0, NIL_VALUE->to_f()->get_value());
}

BOOST_AUTO_TEST_CASE(to_i)
{
    BOOST_CHECK_EQUAL(5.0, make_value(5.5)->to_i()->get_value());
    BOOST_CHECK_EQUAL(5.0, make_value("5.5")->to_i()->get_value());
    BOOST_CHECK_EQUAL(5.0, make_value("5.5x")->to_i()->get_value());
    BOOST_CHECK_EQUAL(0.0, make_value("x")->to_i()->get_value());
    BOOST_CHECK_EQUAL(1.0, make_value(true)->to_i()->get_value());
    BOOST_CHECK_EQUAL(0.0, make_value(false)->to_i()->get_value());
    BOOST_CHECK_EQUAL(0.0, NIL_VALUE->to_i()->get_value());
}

BOOST_AUTO_TEST_SUITE_END()
