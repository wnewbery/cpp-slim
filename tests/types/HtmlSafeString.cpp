#include <boost/test/unit_test.hpp>
#include "expression/Parser.hpp"
#include "expression/Ast.hpp"
#include "expression/Lexer.hpp"
#include "expression/Scope.hpp"
#include "types/HtmlSafeString.hpp"
#include "Error.hpp"
#include "Util.hpp"

using namespace slim;
using namespace slim::expr;
BOOST_AUTO_TEST_SUITE(TestHtmlSafeString)

ObjectPtr eval_obj(const std::string &str)
{
    Scope scope;
    FunctionTable functions;
    Lexer lexer(str);
    Parser parser(functions, lexer);
    auto expr = parser.full_expression();
    auto result = expr->eval(scope);
    return result;
}
std::string eval(const std::string &str)
{
    return eval_obj(str)->inspect();
}

BOOST_AUTO_TEST_CASE(html_safe)
{
    BOOST_CHECK_EQUAL("\"hello <b>world</b>\"", eval("'hello <b>world</b>'.html_safe"));
    BOOST_CHECK_NO_THROW(coerce<HtmlSafeString>(eval_obj("'a'.html_safe")));
    BOOST_CHECK_NO_THROW(coerce<String>(eval_obj("'a'.html_safe")));
    BOOST_CHECK(dynamic_cast<HtmlSafeString*>(eval_obj("'a'.html_safe").get()));
    BOOST_CHECK(dynamic_cast<String*>(eval_obj("'a'.html_safe").get()));
    BOOST_CHECK_EQUAL("HtmlSafeString", eval_obj("'a'.html_safe")->type_name());
}

BOOST_AUTO_TEST_SUITE_END()
