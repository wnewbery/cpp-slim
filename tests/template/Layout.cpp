#include <boost/test/unit_test.hpp>
#include "Template.hpp"
#include "types/ViewModel.hpp"
#include "types/HtmlSafeString.hpp"
#include "Util.hpp"

using namespace slim;
using namespace slim::tpl;
BOOST_AUTO_TEST_SUITE(TestLayout)


BOOST_AUTO_TEST_CASE(content_for)
{
    auto tpl = parse_template(
        "= content_for :head do\n"
        "  p head content\n"
        "\n"
        );
    auto mv = create_view_model();
    auto html = tpl.render(mv, false);

    BOOST_CHECK_EQUAL("", html);
    BOOST_CHECK_EQUAL("<p>head content</p>", mv->yield({symbol("head") })->get_value());
    BOOST_CHECK_EQUAL("", mv->yield({symbol("x")})->get_value());
}

BOOST_AUTO_TEST_CASE(yield)
{
    auto tpl = parse_template(
        "= content_for :head do\n"
        "  p head content\n"
        "\n"
        "p main content\n"
    );
    auto layout = parse_template(
        "head\n"
        "  title header\n"
        "  =yield :head\n"
        "=yield\n"
        "footer footer\n"
        "=yield :final\n"
    );
    auto mv = create_view_model();
    auto html = tpl.render_layout(layout, mv, true);
    BOOST_CHECK_EQUAL(
        "<!DOCTYPE html>\n"
        "<head><title>header</title><p>head content</p></head>"
        "<p>main content</p>"
        "<footer>footer</footer>"
        , html);
}

BOOST_AUTO_TEST_SUITE_END()
