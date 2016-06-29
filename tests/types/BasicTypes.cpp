#include <boost/test/unit_test.hpp>
#include "Value.hpp"

using namespace slim;
BOOST_AUTO_TEST_SUITE(TestBasicTypes)

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
}

BOOST_AUTO_TEST_CASE(null)
{
    auto null_val = create_object<Null>();

    BOOST_CHECK_EQUAL(null_val, NULL_VALUE);

    BOOST_CHECK_EQUAL("Null", Null::TYPE_NAME);
    BOOST_CHECK_EQUAL("Null", null_val->type_name());
    BOOST_CHECK_EQUAL("null", null_val->to_string());
    BOOST_CHECK_EQUAL(false, null_val->is_true());
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
    BOOST_CHECK_EQUAL(false, a->is_true());
    BOOST_CHECK_EQUAL(true, b->is_true());
    BOOST_CHECK_EQUAL(true, c->is_true());
}

BOOST_AUTO_TEST_SUITE_END()
