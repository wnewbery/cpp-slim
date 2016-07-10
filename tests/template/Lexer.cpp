#include <boost/test/unit_test.hpp>
#include "template/Lexer.hpp"
#include "template/Token.hpp"
#include "Error.hpp"

using namespace slim;
using namespace slim::tpl;
BOOST_AUTO_TEST_SUITE(TestTplLexer)

Lexer lexer(const char *str)
{
    return Lexer(str, str + strlen(str));
}
BOOST_AUTO_TEST_CASE(next_indent)
{
    Token tok;
    //Skip blank lines, reaching source end is OK
    BOOST_CHECK_EQUAL(Token::END, lexer("").next_indent().type);
    BOOST_CHECK_EQUAL(Token::END, lexer("     ").next_indent().type);
    BOOST_CHECK_EQUAL(Token::END, lexer("    \n    \r    \r\n    ").next_indent().type);

    //read upto next non-whitespace
    tok = lexer("    div").next_indent();
    BOOST_CHECK_EQUAL(Token::INDENT, tok.type);
    BOOST_CHECK_EQUAL("    ", tok.str);

    //indent token created does not include skipped lines
    tok = lexer("    \n\n      div").next_indent();
    BOOST_CHECK_EQUAL(Token::INDENT, tok.type);
    BOOST_CHECK_EQUAL("      ", tok.str);

    //tabs are banned
    BOOST_CHECK_THROW(lexer("    \tdiv").next_indent(), TemplateSyntaxError);
}

BOOST_AUTO_TEST_CASE(next_name)
{
    Token tok;
    tok = lexer("div").next_name();
    BOOST_CHECK_EQUAL(Token::NAME, tok.type);
    BOOST_CHECK_EQUAL("div", tok.str);

    tok = lexer("div-55").next_name();
    BOOST_CHECK_EQUAL(Token::NAME, tok.type);
    BOOST_CHECK_EQUAL("div-55", tok.str);

    tok = lexer("my_tag").next_name();
    BOOST_CHECK_EQUAL(Token::NAME, tok.type);
    BOOST_CHECK_EQUAL("my_tag", tok.str);

    tok = lexer("div.").next_name();
    BOOST_CHECK_EQUAL(Token::NAME, tok.type);
    BOOST_CHECK_EQUAL("div", tok.str);

    tok = lexer("div=").next_name();
    BOOST_CHECK_EQUAL(Token::NAME, tok.type);
    BOOST_CHECK_EQUAL("div", tok.str);

    tok = lexer("div ").next_name();
    BOOST_CHECK_EQUAL(Token::NAME, tok.type);
    BOOST_CHECK_EQUAL("div", tok.str);

}

BOOST_AUTO_TEST_CASE(next_line)
{
    BOOST_CHECK_NO_THROW(lexer("\r").next_line());
    BOOST_CHECK_NO_THROW(lexer("\n").next_line());
    BOOST_CHECK_NO_THROW(lexer("\r\n").next_line());
    BOOST_CHECK_THROW(lexer("a\n").next_line(), TemplateSyntaxError);

    auto a = lexer("\n\r\r\n");
    BOOST_CHECK_NO_THROW(a.next_line());
    BOOST_CHECK_NO_THROW(a.next_line());
    BOOST_CHECK_NO_THROW(a.next_line());
    BOOST_CHECK_THROW(a.next_line(), TemplateSyntaxError);
}

BOOST_AUTO_TEST_SUITE_END()
