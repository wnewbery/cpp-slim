#include <boost/test/unit_test.hpp>
#include "Util.hpp"

BOOST_AUTO_TEST_SUITE(TestUtil)

BOOST_AUTO_TEST_CASE(html_escape)
{
    BOOST_CHECK_EQUAL(
        "&amp; &lt; &gt; &quot; &#39;",
        slim::html_escape("& < > \" '"));
}

BOOST_AUTO_TEST_SUITE_END()
