#include <boost/test/unit_test.hpp>
#include "expression/Lexer.hpp"
#include "expression/Token.hpp"
#include "Error.hpp"

using namespace slim;
using namespace slim::expr;
BOOST_AUTO_TEST_SUITE(TestExprLexer)

Token single_token(const std::string &str)
{
    Lexer lexer(str);
    return lexer.next();
}

BOOST_AUTO_TEST_CASE(single_tokens)
{
    BOOST_CHECK_EQUAL(Token::END, single_token("").type);
    BOOST_CHECK_EQUAL(Token::END, single_token(" \t\r\n").type);
    BOOST_CHECK_EQUAL(Token::END, single_token("").type);


    BOOST_CHECK_EQUAL(Token::HASH_KEY_VALUE_SEP, single_token("=>").type);
    BOOST_CHECK_EQUAL(Token::LPAREN, single_token("(").type);
    BOOST_CHECK_EQUAL(Token::RPAREN, single_token(")").type);
    BOOST_CHECK_EQUAL(Token::L_SQ_BRACKET, single_token("[").type);
    BOOST_CHECK_EQUAL(Token::R_SQ_BRACKET, single_token("]").type);
    BOOST_CHECK_EQUAL(Token::L_CURLY_BRACKET, single_token("{").type);
    BOOST_CHECK_EQUAL(Token::R_CURLY_BRACKET, single_token("}").type);
    BOOST_CHECK_EQUAL(Token::COMMA, single_token(",").type);
    BOOST_CHECK_EQUAL(Token::DOT, single_token(".").type);
    BOOST_CHECK_EQUAL(Token::SAFE_NAV, single_token("&.").type);
    BOOST_CHECK_EQUAL(Token::COLON, single_token(":").type);
    BOOST_CHECK_EQUAL(Token::CONST_NAV, single_token("::").type);
    BOOST_CHECK_EQUAL(Token::CONDITIONAL, single_token("?").type);
    BOOST_CHECK_EQUAL(Token::PLUS, single_token("+").type);
    BOOST_CHECK_EQUAL(Token::MINUS, single_token("- ").type);
    BOOST_CHECK_EQUAL(Token::MUL, single_token("* ").type);
    BOOST_CHECK_EQUAL(Token::POW, single_token("**").type);
    BOOST_CHECK_EQUAL(Token::DIV, single_token("/").type);
    BOOST_CHECK_EQUAL(Token::MOD, single_token("%").type);
    BOOST_CHECK_EQUAL(Token::AND, single_token("& ").type);
    BOOST_CHECK_EQUAL(Token::OR, single_token("| ").type);
    BOOST_CHECK_EQUAL(Token::XOR, single_token("^ ").type);
    BOOST_CHECK_EQUAL(Token::NOT, single_token("~ ").type);
    BOOST_CHECK_EQUAL(Token::LSHIFT, single_token("<<").type);
    BOOST_CHECK_EQUAL(Token::RSHIFT, single_token(">>").type);
    BOOST_CHECK_EQUAL(Token::LOGICAL_NOT, single_token("! ").type);
    BOOST_CHECK_EQUAL(Token::LOGICAL_AND, single_token("&&").type);
    BOOST_CHECK_EQUAL(Token::LOGICAL_OR, single_token("||").type);
    BOOST_CHECK_EQUAL(Token::CMP_EQ, single_token("== ").type);
    BOOST_CHECK_EQUAL(Token::CMP_NE, single_token("!= ").type);
    BOOST_CHECK_EQUAL(Token::CMP, single_token("<=>").type);
    BOOST_CHECK_EQUAL(Token::CMP_LT, single_token("<  ").type);
    BOOST_CHECK_EQUAL(Token::CMP_LE, single_token("<= ").type);
    BOOST_CHECK_EQUAL(Token::CMP_GT, single_token(">  ").type);
    BOOST_CHECK_EQUAL(Token::CMP_GE, single_token(">= ").type);
    BOOST_CHECK_EQUAL(Token::CMP, single_token("<=>").type);

    BOOST_CHECK_EQUAL(Token::STRING_DELIM, single_token("'").type);
    BOOST_CHECK_EQUAL(Token::STRING_DELIM, single_token("\"").type);

    auto tok = Lexer("string \\\\ \\' \\\" \\t \\n \\r # \\#{} end\"").next_str_interp('"');
    BOOST_CHECK_EQUAL(Token::STRING_TEXT, tok.type);
    BOOST_CHECK_EQUAL("string \\ \' \" \t \n \r # #{} end", tok.str);

    BOOST_CHECK_EQUAL(Token::STRING_TEXT, Lexer("'").next_str_interp('"').type);
    BOOST_CHECK_EQUAL(Token::STRING_DELIM, Lexer("'").next_str_interp('\'').type);
    BOOST_CHECK_EQUAL(Token::STRING_INTERP_START, Lexer("#{").next_str_interp('"').type);

    tok = single_token("054335.2250");
    BOOST_CHECK_EQUAL(Token::NUMBER, tok.type);
    BOOST_CHECK_EQUAL("054335.2250", tok.str);

    tok = single_token("054335.0");
    BOOST_CHECK_EQUAL(Token::NUMBER, tok.type);
    BOOST_CHECK_EQUAL("054335.0", tok.str);

    tok = single_token("054335");
    BOOST_CHECK_EQUAL(Token::NUMBER, tok.type);
    BOOST_CHECK_EQUAL("054335", tok.str);

    tok = single_token("-5.5");
    BOOST_CHECK_EQUAL(Token::NUMBER, tok.type);
    BOOST_CHECK_EQUAL("-5.5", tok.str);

    tok = single_token("test_func55");
    BOOST_CHECK_EQUAL(Token::SYMBOL, tok.type);
    BOOST_CHECK_EQUAL("test_func55", tok.str);

    tok = single_token("empty?");
    BOOST_CHECK_EQUAL(Token::SYMBOL, tok.type);
    BOOST_CHECK_EQUAL("empty?", tok.str);

    tok = single_token("test_func55:");
    BOOST_CHECK_EQUAL(Token::HASH_SYMBOL, tok.type);
    BOOST_CHECK_EQUAL("test_func55", tok.str);

    tok = single_token("test_func55::"); //double colon is constant lookup, not hash key symbol
    BOOST_CHECK_EQUAL(Token::SYMBOL, tok.type);
    BOOST_CHECK_EQUAL("test_func55", tok.str);

    tok = single_token("@my_attr");
    BOOST_CHECK_EQUAL(Token::ATTR_NAME, tok.type);
    BOOST_CHECK_EQUAL("my_attr", tok.str);
    
    // invalid tokens
    BOOST_CHECK_EQUAL(Token::UNKNOWN, single_token("= ").type);
    BOOST_CHECK_EQUAL(Token::UNKNOWN, single_token("=+").type);
    BOOST_CHECK_THROW(single_token("@").type, SyntaxError);
    BOOST_CHECK_THROW(Lexer("\\g'").next_str_interp('\''), SyntaxError);
}

BOOST_AUTO_TEST_SUITE_END()
