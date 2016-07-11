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
    BOOST_CHECK_EQUAL(lexer("\r").next_line().type, Token::EOL);
    BOOST_CHECK_EQUAL(lexer("\n").next_line().type, Token::EOL);
    BOOST_CHECK_EQUAL(lexer("\r\n").next_line().type, Token::EOL);
    BOOST_CHECK_THROW(lexer("a\n").next_line(), TemplateSyntaxError);

    auto a = lexer("\n\r\r\n");
    BOOST_CHECK_EQUAL(a.next_line().type, Token::EOL);
    BOOST_CHECK_EQUAL(a.next_line().type, Token::EOL);
    BOOST_CHECK_EQUAL(a.next_line().type, Token::EOL);
    BOOST_CHECK_EQUAL(a.next_line().type, Token::END);
    BOOST_CHECK_THROW(a.next_line(), TemplateSyntaxError);
}

BOOST_AUTO_TEST_CASE(next_line_start)
{
    BOOST_CHECK_EQUAL(Token::TEXT_LINE, lexer("|").next_line_start().type);
    BOOST_CHECK_EQUAL(Token::TEXT_LINE_WITH_TRAILING_SPACE, lexer("'").next_line_start().type);
    BOOST_CHECK_EQUAL(Token::HTML_LINE, lexer("<").next_line_start().type);
    BOOST_CHECK_EQUAL(Token::COMMENT_LINE, lexer("/").next_line_start().type);
    BOOST_CHECK_EQUAL(Token::HTML_COMMENT_LINE, lexer("/!").next_line_start().type);
    BOOST_CHECK_EQUAL(Token::NAME, lexer("div").next_line_start().type);
    BOOST_CHECK_EQUAL(Token::TAG_ID, lexer("#").next_line_start().type);
    BOOST_CHECK_EQUAL(Token::TAG_CLASS, lexer(".").next_line_start().type);
    BOOST_CHECK_EQUAL(Token::OUTPUT_LINE, lexer("=").next_line_start().type);
    BOOST_CHECK_THROW(lexer("").next_line_start(), TemplateSyntaxError);
    BOOST_CHECK_THROW(lexer("@").next_line_start(), TemplateSyntaxError);
}

BOOST_AUTO_TEST_CASE(next_tag_content)
{
    Token tok;
    BOOST_CHECK_EQUAL(lexer("").next_tag_content().type, Token::END);
    BOOST_CHECK_EQUAL(lexer("\r\n").next_tag_content().type, Token::EOL);
    BOOST_CHECK_EQUAL(lexer(">").next_tag_content().type, Token::ADD_TRAILING_WHITESPACE);
    BOOST_CHECK_EQUAL(lexer("<").next_tag_content().type, Token::ADD_LEADING_WHITESPACE);
    BOOST_CHECK_EQUAL(lexer("<>").next_tag_content().type, Token::ADD_LEADING_AND_TRAILING_WHITESPACE);
    BOOST_CHECK_EQUAL(lexer("#").next_tag_content().type, Token::TAG_ID);
    BOOST_CHECK_EQUAL(lexer(".").next_tag_content().type, Token::TAG_CLASS);
    BOOST_CHECK_EQUAL(lexer("=").next_tag_content().type, Token::OUTPUT_LINE);
    BOOST_CHECK_EQUAL(lexer("   =").next_tag_content().type, Token::OUTPUT_LINE);
    
    tok = lexer("text").next_tag_content();
    BOOST_CHECK_EQUAL(Token::TEXT_CONTENT, tok.type);
    BOOST_CHECK_EQUAL("text", tok.str);

    tok = lexer("text content\n").next_tag_content();
    BOOST_CHECK_EQUAL(Token::TEXT_CONTENT, tok.type);
    BOOST_CHECK_EQUAL("text content", tok.str);

    tok = lexer("   \t  text content\n").next_tag_content();
    BOOST_CHECK_EQUAL(Token::TEXT_CONTENT, tok.type);
    BOOST_CHECK_EQUAL("text content", tok.str);

    tok = lexer("text  <content> = symbols\nnext line").next_tag_content();
    BOOST_CHECK_EQUAL(Token::TEXT_CONTENT, tok.type);
    BOOST_CHECK_EQUAL("text  <content> = symbols", tok.str);


    BOOST_CHECK_THROW(lexer("value=x").next_tag_content(), TemplateSyntaxError);

    auto a = lexer("Text");
    BOOST_CHECK_EQUAL(a.next_tag_content().type, Token::TEXT_CONTENT);
    BOOST_CHECK_EQUAL(a.next_tag_content().type, Token::END);
    BOOST_CHECK_THROW(a.next_tag_content(), TemplateSyntaxError);
}

BOOST_AUTO_TEST_CASE(next_text_content)
{
    auto a = lexer("Text");
    BOOST_CHECK_EQUAL(a.next_text_content().type, Token::TEXT_CONTENT);
    BOOST_CHECK_EQUAL(a.next_text_content().type, Token::END);
    BOOST_CHECK_THROW(a.next_text_content(), TemplateSyntaxError);

    Token tok;
    BOOST_CHECK_EQUAL(lexer("").next_text_content().type, Token::END);
    
    tok = lexer("text content").next_text_content();
    BOOST_CHECK_EQUAL(Token::TEXT_CONTENT, tok.type);
    BOOST_CHECK_EQUAL("text content", tok.str);

    tok = lexer("   \t  text content\n").next_text_content();
    BOOST_CHECK_EQUAL(Token::TEXT_CONTENT, tok.type);
    BOOST_CHECK_EQUAL("  \t  text content", tok.str);
}

BOOST_AUTO_TEST_CASE(next_whitespace_control)
{
    BOOST_CHECK_EQUAL(lexer("").next_whitespace_control().type, Token::END);
    BOOST_CHECK_EQUAL(lexer("\r\n").next_whitespace_control().type, Token::END);
    BOOST_CHECK_EQUAL(lexer("x").next_whitespace_control().type, Token::END);
    BOOST_CHECK_EQUAL(lexer(">").next_whitespace_control().type, Token::ADD_TRAILING_WHITESPACE);
    BOOST_CHECK_EQUAL(lexer("<").next_whitespace_control().type, Token::ADD_LEADING_WHITESPACE);
    BOOST_CHECK_EQUAL(lexer("<>").next_whitespace_control().type, Token::ADD_LEADING_AND_TRAILING_WHITESPACE);
}

BOOST_AUTO_TEST_SUITE_END()
