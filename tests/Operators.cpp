#include <boost/test/unit_test.hpp>
#include "Operators.hpp"
#include "Error.hpp"
#include "Value.hpp"

using namespace slim;
BOOST_AUTO_TEST_SUITE(TestOperators)

BOOST_AUTO_TEST_CASE(equals)
{
    auto a = make_value(55.0);
    auto a2 = make_value(55.0);
    auto b = make_value(60.0);
    auto c = make_value("55");
    auto d = make_value("test");
    auto d2 = make_value("test");
    //Boolean
    BOOST_CHECK(op_eq(TRUE_VALUE.get(), TRUE_VALUE.get())->is_true());
    BOOST_CHECK(!op_eq(TRUE_VALUE.get(), FALSE_VALUE.get())->is_true());
    BOOST_CHECK(!op_eq(FALSE_VALUE.get(), TRUE_VALUE.get())->is_true());

    BOOST_CHECK(!op_ne(TRUE_VALUE.get(), TRUE_VALUE.get())->is_true());
    BOOST_CHECK(op_ne(TRUE_VALUE.get(), FALSE_VALUE.get())->is_true());
    BOOST_CHECK(op_ne(FALSE_VALUE.get(), TRUE_VALUE.get())->is_true());

    //Null
    BOOST_CHECK(op_eq(NULL_VALUE.get(), NULL_VALUE.get())->is_true());
    BOOST_CHECK(!op_ne(NULL_VALUE.get(), NULL_VALUE.get())->is_true());

    //Null and Boolean
    BOOST_CHECK(!op_eq(TRUE_VALUE.get(), NULL_VALUE.get())->is_true());
    BOOST_CHECK(!op_eq(FALSE_VALUE.get(), NULL_VALUE.get())->is_true());

    //Number
    BOOST_CHECK(op_eq(a.get(), a2.get())->is_true());
    BOOST_CHECK(!op_eq(a.get(), b.get())->is_true());
    BOOST_CHECK(!op_ne(a.get(), a2.get())->is_true());
    BOOST_CHECK(op_ne(a.get(), b.get())->is_true());

    //String
    BOOST_CHECK(op_eq(d.get(), d2.get())->is_true());
    BOOST_CHECK(!op_eq(c.get(), d.get())->is_true());
    BOOST_CHECK(!op_ne(a.get(), a2.get())->is_true());
    BOOST_CHECK(op_ne(c.get(), d.get())->is_true());

    //Number and String
    BOOST_CHECK(!op_eq(a.get(), c.get())->is_true());
    BOOST_CHECK(op_ne(a.get(), c.get())->is_true());
}

BOOST_AUTO_TEST_CASE(rel_cmp)
{
    auto a = make_value(55.0);
    auto b = make_value(60.0);
    auto c = make_value("55");
    auto d = make_value("test");
    auto e = make_value("test55");
    auto f = make_value("56");

    //Number
    BOOST_CHECK(!op_lt(a.get(), a.get())->is_true());
    BOOST_CHECK(op_le(a.get(), a.get())->is_true());
    BOOST_CHECK(!op_gt(a.get(), a.get())->is_true());
    BOOST_CHECK(op_ge(a.get(), a.get())->is_true());

    BOOST_CHECK(op_lt(a.get(), b.get())->is_true());
    BOOST_CHECK(op_le(a.get(), b.get())->is_true());
    BOOST_CHECK(!op_gt(a.get(), b.get())->is_true());
    BOOST_CHECK(!op_ge(a.get(), b.get())->is_true());

    BOOST_CHECK(!op_lt(b.get(), a.get())->is_true());
    BOOST_CHECK(!op_le(b.get(), a.get())->is_true());
    BOOST_CHECK(op_gt(b.get(), a.get())->is_true());
    BOOST_CHECK(op_ge(b.get(), a.get())->is_true());


    //Boolean
    BOOST_CHECK(op_le(TRUE_VALUE.get(), TRUE_VALUE.get())->is_true());
    BOOST_CHECK(op_le(FALSE_VALUE.get(), TRUE_VALUE.get())->is_true());

    BOOST_CHECK(op_lt(FALSE_VALUE.get(), TRUE_VALUE.get())->is_true());
    BOOST_CHECK(!op_lt(TRUE_VALUE.get(), FALSE_VALUE.get())->is_true());

    BOOST_CHECK(op_gt(TRUE_VALUE.get(), FALSE_VALUE.get())->is_true());
    BOOST_CHECK(!op_gt(FALSE_VALUE.get(), TRUE_VALUE.get())->is_true());

    //Null, no-order
    BOOST_CHECK_THROW(op_lt(NULL_VALUE.get(), NULL_VALUE.get()), UnorderableTypeError);

    //Number and Boolean, different types
    BOOST_CHECK_THROW(op_le(FALSE_VALUE.get(), a.get()), UnorderableTypeError);

    //String
    BOOST_CHECK(op_le(c.get(), d.get())->is_true());
    BOOST_CHECK(op_le(c.get(), f.get())->is_true());
    BOOST_CHECK(op_le(d.get(), e.get())->is_true());
}

double to_d(ObjectPtr val)
{
    auto n = dynamic_cast<const Number*>(val.get());
    if (n) return n->get_value();
    BOOST_FAIL("Not a number");
    return 0;
}
BOOST_AUTO_TEST_CASE(binary)
{
    auto a = make_value(2.0);
    auto b = make_value(5.0);
    auto c = make_value("55");
    auto d = make_value("test");

    //mul
    BOOST_CHECK_EQUAL(10.0, to_d(op_mul(a.get(), b.get())));
    BOOST_CHECK_THROW(op_mul(a.get(), c.get()), UnsupportedOperandTypeError);
    BOOST_CHECK_THROW(op_mul(c.get(), d.get()), UnsupportedOperandTypeError);

    //div
    BOOST_CHECK_EQUAL(2.0 / 5.0, to_d(op_div(a.get(), b.get())));
    BOOST_CHECK_EQUAL(5.0 / 2.0, to_d(op_div(b.get(), a.get())));
    BOOST_CHECK_THROW(op_div(a.get(), c.get()), UnsupportedOperandTypeError);
    BOOST_CHECK_THROW(op_div(c.get(), d.get()), UnsupportedOperandTypeError);

    //mod
    BOOST_CHECK_EQUAL(2.0, to_d(op_mod(a.get(), b.get())));
    BOOST_CHECK_EQUAL(1.0, to_d(op_mod(b.get(), a.get())));
    BOOST_CHECK_THROW(op_mod(a.get(), c.get()), UnsupportedOperandTypeError);
    BOOST_CHECK_THROW(op_mod(c.get(), d.get()), UnsupportedOperandTypeError);

    //add
    BOOST_CHECK_EQUAL(7.0, to_d(op_add(a.get(), b.get())));
    BOOST_CHECK_EQUAL(7.0, to_d(op_add(b.get(), a.get())));
    BOOST_CHECK_THROW(op_add(a.get(), c.get()), UnsupportedOperandTypeError);
    BOOST_CHECK_EQUAL("55test", op_add(c.get(), d.get())->to_string());
    BOOST_CHECK_EQUAL("test55", op_add(d.get(), c.get())->to_string());

    //sub
    BOOST_CHECK_EQUAL(-3.0, to_d(op_sub(a.get(), b.get())));
    BOOST_CHECK_EQUAL(3.0, to_d(op_sub(b.get(), a.get())));
    BOOST_CHECK_THROW(op_sub(a.get(), c.get()), UnsupportedOperandTypeError);
    BOOST_CHECK_THROW(op_sub(c.get(), d.get()), UnsupportedOperandTypeError);
}
BOOST_AUTO_TEST_CASE(unary)
{
    auto a = make_value(2.0);
    auto b = make_value(0.0);
    auto c = make_value("55");
    auto d = make_value("");

    //not
    BOOST_CHECK(!op_not(a.get())->is_true());
    BOOST_CHECK(op_not(b.get())->is_true());
    BOOST_CHECK(!op_not(c.get())->is_true());
    BOOST_CHECK(op_not(d.get())->is_true());
    BOOST_CHECK(!op_not(TRUE_VALUE.get())->is_true());
    BOOST_CHECK(op_not(FALSE_VALUE.get())->is_true());

    //negative
    BOOST_CHECK_EQUAL(-2.0, to_d(op_negative(a.get())));
    BOOST_CHECK_THROW(op_negative(c.get()), UnsupportedOperandTypeError);
    BOOST_CHECK_THROW(op_negative(TRUE_VALUE.get()), UnsupportedOperandTypeError);
    BOOST_CHECK_THROW(op_negative(NULL_VALUE.get()), UnsupportedOperandTypeError);
}
BOOST_AUTO_TEST_SUITE_END()
