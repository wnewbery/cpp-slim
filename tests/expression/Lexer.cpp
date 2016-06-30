#include <boost/test/unit_test.hpp>
#include "expression/Lexer.hpp"
#include "expression/Token.hpp"

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


    BOOST_CHECK_EQUAL(Token::LPAREN, single_token("(").type);
    BOOST_CHECK_EQUAL(Token::RPAREN, single_token(")").type);
    BOOST_CHECK_EQUAL(Token::COMMA, single_token(",").type);
    BOOST_CHECK_EQUAL(Token::DOT, single_token(".").type);
    BOOST_CHECK_EQUAL(Token::PLUS, single_token("+").type);
    BOOST_CHECK_EQUAL(Token::MINUS, single_token("-").type);
    BOOST_CHECK_EQUAL(Token::MUL, single_token("*").type);
    BOOST_CHECK_EQUAL(Token::DIV, single_token("/").type);
    BOOST_CHECK_EQUAL(Token::MOD, single_token("%").type);
    BOOST_CHECK_EQUAL(Token::LOGICAL_NOT, single_token("! ").type);
    BOOST_CHECK_EQUAL(Token::LOGICAL_AND, single_token("&&").type);
    BOOST_CHECK_EQUAL(Token::LOGICAL_OR, single_token("||").type);
    BOOST_CHECK_EQUAL(Token::CMP_EQ, single_token("==").type);
    BOOST_CHECK_EQUAL(Token::CMP_NE, single_token("!=").type);
    BOOST_CHECK_EQUAL(Token::CMP_LT, single_token("< ").type);
    BOOST_CHECK_EQUAL(Token::CMP_LE, single_token("<=").type);
    BOOST_CHECK_EQUAL(Token::CMP_GT, single_token("> ").type);
    BOOST_CHECK_EQUAL(Token::CMP_GE, single_token(">=").type);

    auto tok_str = single_token("'string \\\\ \\' \\\" \\t \\n \\r end'");
    BOOST_CHECK_EQUAL(Token::STRING, tok_str.type);
    BOOST_CHECK_EQUAL("string \\ \' \" \t \n \r end", tok_str.str);

    auto tok_number = single_token("054335.2250");
    BOOST_CHECK_EQUAL(Token::NUMBER, tok_number.type);
    BOOST_CHECK_EQUAL("054335.2250", tok_number.str);

    auto tok_symbol = single_token("test_func55");
    BOOST_CHECK_EQUAL(Token::SYMBOL, tok_symbol.type);
    BOOST_CHECK_EQUAL("test_func55", tok_symbol.str);
}

BOOST_AUTO_TEST_SUITE_END()
