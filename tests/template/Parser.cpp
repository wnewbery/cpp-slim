#include <boost/test/unit_test.hpp>
#include "template/Template.hpp"
#include "template/Lexer.hpp"
#include "template/Parser.hpp"
#include "Error.hpp"

using namespace slim;
using namespace slim::tpl;
BOOST_AUTO_TEST_SUITE(TestTplParser)

std::string parse_str(const char *str)
{
    Lexer lexer(str, str + strlen(str));
    Parser parser(lexer);
    return parser.parse().to_string();
}

BOOST_AUTO_TEST_CASE(simple_elements)
{
    BOOST_CHECK_EQUAL("", parse_str(""));
    BOOST_CHECK_EQUAL("<p></p>", parse_str("p"));
    BOOST_CHECK_EQUAL("<p><span></span><span></span></p>", parse_str("p\n  span\n  span\n"));
    BOOST_CHECK_EQUAL("<p><span><span></span></span><span></span></p>", parse_str("p\n  span\n    span\n  span\n"));

    BOOST_CHECK_EQUAL(" <p></p>", parse_str("p<"));
    BOOST_CHECK_EQUAL("<p></p> ", parse_str("p>"));
    BOOST_CHECK_EQUAL(" <p></p> ", parse_str("p<>"));
}


BOOST_AUTO_TEST_SUITE_END()
