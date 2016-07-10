#include <boost/test/unit_test.hpp>
#include "FunctionHelpers.hpp"
#include "types/Array.hpp"

using namespace slim;
BOOST_AUTO_TEST_SUITE(TestFunctionHelpers)

BOOST_AUTO_TEST_CASE(try_unpack_arg_cpp_types)
{
    auto obj_a = make_value("Hello World");
    auto obj_b = make_value(55.5);
    auto obj_c = make_value(true);

    std::string str = "default";
    BOOST_CHECK_EQUAL(false, try_unpack_arg(obj_b, &str));
    BOOST_CHECK_EQUAL("default", str);
    BOOST_CHECK_EQUAL(true, try_unpack_arg(obj_a, &str));
    BOOST_CHECK_EQUAL("Hello World", str);


    double n = 45.5;
    BOOST_CHECK_EQUAL(false, try_unpack_arg(obj_a, &n));
    BOOST_CHECK_EQUAL(45.5, n);
    BOOST_CHECK_EQUAL(true, try_unpack_arg(obj_b, &n));
    BOOST_CHECK_EQUAL(55.5, n);


    int i = 10;
    BOOST_CHECK_EQUAL(false, try_unpack_arg(obj_a, &i));
    BOOST_CHECK_EQUAL(10, i);
    BOOST_CHECK_EQUAL(true, try_unpack_arg(obj_b, &i));
    BOOST_CHECK_EQUAL(55, i);


    bool b = false;
    BOOST_CHECK_EQUAL(false, try_unpack_arg(obj_a, &b));
    BOOST_CHECK_EQUAL(false, b);
    BOOST_CHECK_EQUAL(true, try_unpack_arg(obj_c, &b));
    BOOST_CHECK_EQUAL(true, b);
}

BOOST_AUTO_TEST_CASE(try_unpack_arg_script_types)
{
    auto obj_a = make_value("Hello World");
    auto obj_b = make_value(55.5);


    std::shared_ptr<Number> n_mem = make_value(40.0);
    auto n = n_mem.get();
    BOOST_CHECK_EQUAL(false, try_unpack_arg(obj_a, &n));
    BOOST_CHECK(n == n_mem.get());
    BOOST_CHECK_EQUAL(40.0, n->get_value());
    BOOST_CHECK_EQUAL(true, try_unpack_arg(obj_b, &n));
    BOOST_CHECK(n != n_mem.get());
    BOOST_CHECK_EQUAL(55.5, n->get_value());

    String *str = nullptr;
    BOOST_CHECK_EQUAL(false, try_unpack_arg(obj_b, &str));
    BOOST_CHECK(nullptr == str);
    BOOST_CHECK_EQUAL(true, try_unpack_arg(obj_a, &str));
    BOOST_CHECK_EQUAL("Hello World", str->to_string());
}

BOOST_AUTO_TEST_CASE(test_try_unpack)
{
    FunctionArgs args = {make_value(55.5), make_value("Hello World")};
    
    double n = 30;
    double n2 = 60;
    std::string str = "Test";
    //missing arg
    BOOST_CHECK_EQUAL(false, try_unpack(args, &n, &str, &n2));
    BOOST_CHECK_EQUAL(30, n);
    BOOST_CHECK_EQUAL(60, n2);
    BOOST_CHECK_EQUAL("Test", str);

    //success
    BOOST_CHECK_EQUAL(true, try_unpack(args, &n, &str));
    BOOST_CHECK_EQUAL(55.5, n);
    BOOST_CHECK_EQUAL("Hello World", str);

    //opt arg, default last
    n = 30;
    str = "Test";
    BOOST_CHECK_EQUAL(true, try_unpack<2>(args, &n, &str, &n2));
    BOOST_CHECK_EQUAL(55.5, n);
    BOOST_CHECK_EQUAL("Hello World", str);
    BOOST_CHECK_EQUAL(60, n2);

    //opt arg, have all
    n = 30;
    str = "Test";
    BOOST_CHECK_EQUAL(true, try_unpack<1>(args, &n, &str));
    BOOST_CHECK_EQUAL(55.5, n);
    BOOST_CHECK_EQUAL("Hello World", str);

    //right count, but one is wrong type
    n = 30;
    n2 = 55;
    BOOST_CHECK_EQUAL(false, try_unpack<1>(args, &n, &n2));
    BOOST_CHECK_EQUAL(30, n);
    BOOST_CHECK_EQUAL(55, n2);
}

BOOST_AUTO_TEST_CASE(test_unpack)
{
    //like try_unpack, but throws InvalidArgumentCount or InvalidArgument instead of return false

    FunctionArgs args = { make_value(55.5), make_value("Hello World") };

    double n = 30;
    double n2 = 60;
    std::string str = "Test";
    //missing arg
    BOOST_CHECK_THROW(unpack(args, &n, &str, &n2), InvalidArgumentCount);
    BOOST_CHECK_EQUAL(30, n);
    BOOST_CHECK_EQUAL(60, n2);
    BOOST_CHECK_EQUAL("Test", str);

    //right count, but one is wrong type
    n = 30;
    n2 = 55;
    BOOST_CHECK_THROW(unpack<1>(args, &n, &n2), InvalidArgument);
    BOOST_CHECK_EQUAL(30, n);
    BOOST_CHECK_EQUAL(55, n2);
}

BOOST_AUTO_TEST_SUITE_END()
