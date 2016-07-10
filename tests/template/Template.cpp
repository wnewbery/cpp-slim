#include <boost/test/unit_test.hpp>
#include "template/Template.hpp"
#include "template/Lexer.hpp"
#include "template/Parser.hpp"
#include "expression/Scope.hpp"
#include "Error.hpp"

using namespace slim;
using namespace slim::tpl;
BOOST_AUTO_TEST_SUITE(TestTemplate)

std::string render_tpl(const char *str)
{
    Lexer lexer(str, str + strlen(str));
    Parser parser(lexer);
    ViewModel model;
    return parser.parse().render(model);
}

BOOST_AUTO_TEST_CASE(simple_elements)
{
    BOOST_CHECK_EQUAL("<!DOCTYPE html>\n", render_tpl(""));
    BOOST_CHECK_EQUAL(
        "<!DOCTYPE html>\n"
        "<html></html>",
        render_tpl("html"));
    BOOST_CHECK_EQUAL(
        "<!DOCTYPE html>\n"
        "<html><p></p><p></p></html>",
        render_tpl("html\n  p\n  p\n"));
    BOOST_CHECK_EQUAL(
        "<!DOCTYPE html>\n"
        "<html><p><span></span></p><p></p></html>",
        render_tpl("html\n  p\n    span\n  p\n"));
}


BOOST_AUTO_TEST_SUITE_END()
