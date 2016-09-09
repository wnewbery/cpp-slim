#include <boost/test/unit_test.hpp>
#include "template/Template.hpp"
#include "template/Lexer.hpp"
#include "template/Parser.hpp"
#include "expression/Scope.hpp"
#include "Value.hpp"
#include "Error.hpp"

using namespace slim;
using namespace slim::tpl;
BOOST_AUTO_TEST_SUITE(TestTemplate)

std::string render_tpl(const char *str, ViewModel &model)
{
    Lexer lexer(str, str + strlen(str));
    Parser parser(lexer);
    return parser.parse().render(BUILTIN_FUNCTIONS, model);
}
std::string render_tpl(const char *str)
{
    ViewModel model;
    return render_tpl(str, model);
}

BOOST_AUTO_TEST_CASE(text)
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

    BOOST_CHECK_EQUAL(
        "<!DOCTYPE html>\n"
        "<html> <p></p></html>",
        render_tpl("html\n  p<"));
    BOOST_CHECK_EQUAL(
        "<!DOCTYPE html>\n"
        "<html><p></p> </html>",
        render_tpl("html\n  p>"));
    BOOST_CHECK_EQUAL(
        "<!DOCTYPE html>\n"
        "<html> <p></p> </html>",
        render_tpl("html\n  p<>"));
}


BOOST_AUTO_TEST_CASE(code_lines)
{
    ViewModel model;
    model.set("a", make_value(10.0));
    model.set("b", make_value("HTML <b>Safe</b>"));

    BOOST_CHECK_EQUAL(
        "<!DOCTYPE html>\n"
        "<p></p>",
        render_tpl("p", model));
    BOOST_CHECK_EQUAL(
        "<!DOCTYPE html>\n"
        "<p>Number: 5</p>",
        render_tpl("p\n  | Number:\n  =<5\n", model));
    BOOST_CHECK_EQUAL(
        "<!DOCTYPE html>\n"
        "<p>25</p>",
        render_tpl("p\n  =5 + 2 * @a\n", model));

    BOOST_CHECK_EQUAL(
        "<!DOCTYPE html>\n"
        "<p>HTML &lt;b&gt;Safe&lt;/b&gt;</p>",
        render_tpl("p\n  =@b\n", model));

    BOOST_CHECK_EQUAL(
        "<!DOCTYPE html>\n"
        "<p>HTML <b>Safe</b></p>",
        render_tpl("p\n  =@b.html_safe\n", model));
}

BOOST_AUTO_TEST_CASE(attributes)
{
    ViewModel model;
    model.set("a", make_value("Test"));
    model.set("b", TRUE_VALUE);
    model.set("c", FALSE_VALUE);
    model.set("d", NIL_VALUE);
    model.set("e", make_value("HTML <b>Safe</b>"));

    BOOST_CHECK_EQUAL(
        "<!DOCTYPE html>\n"
        "<p disabled=\"Test\"></p>",
        render_tpl("p disabled=@a", model));
    BOOST_CHECK_EQUAL(
        "<!DOCTYPE html>\n"
        "<p disabled></p>",
        render_tpl("p disabled=@b", model));
    BOOST_CHECK_EQUAL(
        "<!DOCTYPE html>\n"
        "<p></p>",
        render_tpl("p disabled=@c", model));
    BOOST_CHECK_EQUAL(
        "<!DOCTYPE html>\n"
        "<p></p>",
        render_tpl("p disabled=@d", model));
    BOOST_CHECK_EQUAL(
        "<!DOCTYPE html>\n"
        "<p class=\"a b e\"></p>",
        render_tpl("p.a.b class='e'", model));
    BOOST_CHECK_EQUAL(
        "<!DOCTYPE html>\n"
        "<p class=\"a b Test e f\"></p>",
        render_tpl("p.a.b class=[@a, 'e', 'f']", model));

    BOOST_CHECK_EQUAL(
        "<!DOCTYPE html>\n"
        "<p class=\"HTML &lt;b&gt;Safe&lt;/b&gt;\"></p>",
        render_tpl("p class=@e", model));
    BOOST_CHECK_EQUAL(
        "<!DOCTYPE html>\n"
        "<p class=\"HTML <b>Safe</b>\"></p>",
        render_tpl("p class=@e.html_safe", model));
}

BOOST_AUTO_TEST_CASE(cond_if)
{
    BOOST_CHECK_EQUAL(
        "<!DOCTYPE html>\n"
        "<p>True</p>",
        render_tpl("-if true\n  p True"));
    BOOST_CHECK_EQUAL(
        "<!DOCTYPE html>\n",
        render_tpl("-if false\n  p True"));

    BOOST_CHECK_EQUAL(
        "<!DOCTYPE html>\n"
        "<p>If</p>",
        render_tpl(
            "-if true\n"
            "  p If\n"
            "-else\n"
            "  p Else\n"
        ));
    BOOST_CHECK_EQUAL(
        "<!DOCTYPE html>\n"
        "<p>Else</p>",
        render_tpl(
            "-if false\n"
            "  p If\n"
            "-else\n"
            "  p Else\n"
        ));
    BOOST_CHECK_EQUAL(
        "<!DOCTYPE html>\n"
        "<p>If</p>",
        render_tpl(
            "-if true\n"
            "  p If\n"
            "-elsif true\n"
            "  p Elsif\n"
            "-else\n"
            "  p Else\n"
        ));
    BOOST_CHECK_EQUAL(
        "<!DOCTYPE html>\n"
        "<p>Elsif</p>",
        render_tpl(
            "-if false\n"
            "  p If\n"
            "-elsif true\n"
            "  p Elsif\n"
            "-else\n"
            "  p Else\n"
        ));
    BOOST_CHECK_EQUAL(
        "<!DOCTYPE html>\n"
        "<p>Elsif</p>",
        render_tpl(
            "-if false\n"
            "  p If\n"
            "-elsif true\n"
            "  p Elsif\n"
            "-elsif true\n"
            "  p Elsif2\n"
        ));
    BOOST_CHECK_EQUAL(
        "<!DOCTYPE html>\n"
        "<p>Elsif2</p>",
        render_tpl(
            "-if false\n"
            "  p If\n"
            "-elsif false\n"
            "  p Elsif\n"
            "-elsif true\n"
            "  p Elsif2\n"
        ));
}

BOOST_AUTO_TEST_SUITE_END()
