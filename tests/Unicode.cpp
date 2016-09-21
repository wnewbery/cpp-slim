#include <boost/test/unit_test.hpp>
#include "Unicode.hpp"

BOOST_AUTO_TEST_SUITE(TestUnicode)

BOOST_AUTO_TEST_CASE(utf8_decode)
{
    uint32_t cp;
    unsigned elements;

    BOOST_CHECK_NO_THROW(slim::utf8_decode("\x24", &cp, &elements));
    BOOST_CHECK_EQUAL(1, elements);
    BOOST_CHECK_EQUAL(0x00024, cp); //USD

    BOOST_CHECK_NO_THROW(slim::utf8_decode("\xC2\xA2", &cp, &elements));
    BOOST_CHECK_EQUAL(2, elements);
    BOOST_CHECK_EQUAL(0x000A2, cp); //Cent

    BOOST_CHECK_NO_THROW(slim::utf8_decode("\xE2\x82\xAC", &cp, &elements));
    BOOST_CHECK_EQUAL(3, elements);
    BOOST_CHECK_EQUAL(0x020AC, cp); //Euro

    BOOST_CHECK_NO_THROW(slim::utf8_decode("\xF0\x90\x8D\x88", &cp, &elements));
    BOOST_CHECK_EQUAL(4, elements);
    BOOST_CHECK_EQUAL(0x10348, cp); //Hwair

    BOOST_CHECK_THROW(slim::utf8_decode("\xC0", &cp, &elements), std::runtime_error);
    BOOST_CHECK_THROW(slim::utf8_decode("\xC2\x00-", &cp, &elements), std::runtime_error);
    BOOST_CHECK_THROW(slim::utf8_decode("\xC2\xFF-", &cp, &elements), std::runtime_error);
    BOOST_CHECK_THROW(slim::utf8_decode("\xFF-----", &cp, &elements), std::runtime_error);
}

BOOST_AUTO_TEST_SUITE_END()
