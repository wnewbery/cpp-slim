#include <boost/test/unit_test.hpp>
#include "expression/Parser.hpp"
#include "expression/Ast.hpp"
#include "expression/AstOp.hpp"
#include "expression/Lexer.hpp"
#include "Function.hpp"
#include "Error.hpp"

using namespace slim;
using namespace slim::expr;
BOOST_AUTO_TEST_SUITE(TestExprParser)

ExpressionNodePtr parse(const std::string &str)
{
    Lexer lexer(str);
    FunctionTable functions;
    Parser parser(functions, lexer);
    return parser.parse_expression();
}

template<class T> bool is_node_type(ExpressionNodePtr ptr)
{
    return dynamic_cast<T*>(ptr.get()) != nullptr;
}

BOOST_AUTO_TEST_CASE(single_values)
{
    BOOST_CHECK(is_node_type<Literal>(parse("true")));
    BOOST_CHECK_EQUAL("true", parse("true")->to_string());

    BOOST_CHECK(is_node_type<Literal>(parse("false")));
    BOOST_CHECK_EQUAL("false", parse("false")->to_string());

    BOOST_CHECK(is_node_type<Literal>(parse("nil")));
    BOOST_CHECK_EQUAL("nil", parse("nil")->to_string());

    BOOST_CHECK(is_node_type<Literal>(parse("55")));
    BOOST_CHECK_EQUAL("55", parse("55")->to_string());

    BOOST_CHECK(is_node_type<Literal>(parse("55.5")));
    BOOST_CHECK_EQUAL("55.5", parse("55.5")->to_string());

    BOOST_CHECK(is_node_type<Literal>(parse("'true'")));
    BOOST_CHECK_EQUAL("\"true\"", parse("'true'")->to_string());

    BOOST_CHECK_THROW(parse("55.5.5"), SyntaxError);


    BOOST_CHECK(is_node_type<ArrayLiteral>(parse("[5]")));
    BOOST_CHECK_EQUAL("[]", parse("[]")->to_string());
    BOOST_CHECK_EQUAL("[5]", parse("[5]")->to_string());
    BOOST_CHECK_EQUAL("[2, true]", parse("[2,true]")->to_string());


    BOOST_CHECK(is_node_type<HashLiteral>(parse("{a: 5}")));
    BOOST_CHECK_EQUAL("{}", parse("{}")->to_string());
    BOOST_CHECK_EQUAL("{\"a\" => 5}", parse("{a: 5}")->to_string());
    BOOST_CHECK_EQUAL("{1 => 2, 5 => true}", parse("{1 => 2, 5 => true}")->to_string());

    BOOST_CHECK(is_node_type<Variable>(parse("myvar")));
    BOOST_CHECK_EQUAL("myvar", parse("myvar")->to_string());
}


BOOST_AUTO_TEST_CASE(single_ops)
{
    BOOST_CHECK_EQUAL("(5 && 10)", parse("5 && 10")->to_string());
    BOOST_CHECK_EQUAL("(5 || 10)", parse("5 || 10")->to_string());

    BOOST_CHECK_EQUAL("(5 == 10)", parse("5 == 10")->to_string());
    BOOST_CHECK_EQUAL("(5 != 10)", parse("5 != 10")->to_string());
    BOOST_CHECK_EQUAL("(5 < 10)", parse("5 < 10")->to_string());
    BOOST_CHECK_EQUAL("(5 <= 10)", parse("5 <= 10")->to_string());
    BOOST_CHECK_EQUAL("(5 > 10)", parse("5 > 10")->to_string());
    BOOST_CHECK_EQUAL("(5 >= 10)", parse("5 >= 10")->to_string());

    BOOST_CHECK_EQUAL("(5 << 10)", parse("5 << 10")->to_string());
    BOOST_CHECK_EQUAL("(5 >> 10)", parse("5 >> 10")->to_string());
    BOOST_CHECK_EQUAL("(5 & 10)", parse("5 & 10")->to_string());
    BOOST_CHECK_EQUAL("(5 | 10)", parse("5 | 10")->to_string());
    BOOST_CHECK_EQUAL("(5 ^ 10)", parse("5 ^ 10")->to_string());
    BOOST_CHECK_EQUAL("(~10)", parse("~10")->to_string());

    BOOST_CHECK_EQUAL("(5 + 10)", parse("5 + 10")->to_string());
    BOOST_CHECK_EQUAL("(5 - 10)", parse("5 - 10")->to_string());

    BOOST_CHECK_EQUAL("(5 * 10)", parse("5 * 10")->to_string());
    BOOST_CHECK_EQUAL("(5 / 10)", parse("5 / 10")->to_string());
    BOOST_CHECK_EQUAL("(5 % 10)", parse("5 % 10")->to_string());

    BOOST_CHECK_EQUAL("(!10)", parse("! 10")->to_string());
    BOOST_CHECK_EQUAL("-10", parse("-10")->to_string());
    BOOST_CHECK_EQUAL("10", parse("+10")->to_string());
}

BOOST_AUTO_TEST_CASE(associativity_single)
{
    // binary, left to right
    BOOST_CHECK_EQUAL("(((5 && 10) && 5) && true)", parse("5 && 10 && 5 && true")->to_string());

    // unary, right to left
    BOOST_CHECK_EQUAL("(-(!(-5)))", parse("-!-+5")->to_string());
}

BOOST_AUTO_TEST_CASE(grouping)
{
    BOOST_CHECK_EQUAL("(5 - 5)", parse("(5 - 5)")->to_string());
    BOOST_CHECK_EQUAL("(5 - 5)", parse("(((5) - 5))")->to_string()); //because groups dont exist in the AST
    BOOST_CHECK_EQUAL("((5 - 5) * 6)", parse("(5 - 5) * 6")->to_string());
    BOOST_CHECK_EQUAL("(-((5 - 5) * 6))", parse("-((5 - 5) * 6)")->to_string());

    BOOST_CHECK_EQUAL("[2, (5 + 5), a.f()]", parse("[2,5+5,a.f]")->to_string());
}

BOOST_AUTO_TEST_CASE(method_call)
{
    BOOST_CHECK_EQUAL("a.f()", parse("a.f")->to_string());
    BOOST_CHECK_EQUAL("a.f()", parse("a.f()")->to_string());
    BOOST_CHECK_EQUAL("a.f(5)", parse("a.f(5)")->to_string()); //because groups dont exist in the AST
    BOOST_CHECK_EQUAL("a.f(5, true)", parse("a.f(5, true)")->to_string());
    BOOST_CHECK_EQUAL("a.f().g()", parse("a.f.g")->to_string());
    BOOST_CHECK_EQUAL("a.f(5).g()", parse("a.f(5).g")->to_string());

    BOOST_CHECK_EQUAL("a.f(2, 3, 4)", parse("a.f 2, 3, 4")->to_string());
    BOOST_CHECK_EQUAL("a.f(2, 3, (4 + 4))", parse("a.f 2, 3, 4 + 4")->to_string());
    BOOST_CHECK_EQUAL("(a.f(2, 3, 4) + 4)", parse("(a.f 2, 3, 4) + 4")->to_string());

    BOOST_CHECK_EQUAL("a&.f(2, 3, 4)", parse("a&.f 2, 3, 4")->to_string());

    BOOST_CHECK_EQUAL("a[5]", parse("a[5]")->to_string());
    BOOST_CHECK_EQUAL("a[5][5, 10]", parse("a[5][5, 10]")->to_string());
    BOOST_CHECK_EQUAL("a[5][5, 10].to_i()", parse("a[5][5, 10].to_i")->to_string());

    BOOST_CHECK_EQUAL("a.f()[5]", parse("a.f[5]")->to_string());
    BOOST_CHECK_EQUAL("a.f()[5, 7]", parse("a.f[5, 7]")->to_string());
    BOOST_CHECK_EQUAL("a.f()[5, 7].g()", parse("a.f[5, 7].g")->to_string());
    BOOST_CHECK_EQUAL("a.f()[5, (7 + 8)].g()", parse("a.f[5, 7 + 8].g")->to_string());

    BOOST_CHECK_EQUAL("a.f({|| 5})", parse("a.f{|| 5}")->to_string());
    BOOST_CHECK_EQUAL("a.f(4, {|x| x})", parse("a.f(4){|x| x}")->to_string());
    BOOST_CHECK_EQUAL("a.f({|x, y| (x * y)})", parse("a.f{|x, y| x * y}")->to_string());
}

BOOST_AUTO_TEST_CASE(precedence)
{
    BOOST_CHECK_EQUAL("((5 - 5) && 10)", parse("5 - 5 && 10")->to_string());
    BOOST_CHECK_EQUAL("(((5 - 5) == 0) && (10 != nil))", parse("5 - 5 == 0 && 10 != nil")->to_string());
    BOOST_CHECK_EQUAL("((5 < 10) == (15 >= 10))", parse("5 < 10 == 15 >= 10")->to_string());
    BOOST_CHECK_EQUAL("((5 - (-5)) == 0)", parse("5 - - 5 == 0")->to_string());
    BOOST_CHECK_EQUAL("(c + (m * x))", parse("c + m * x")->to_string());
    BOOST_CHECK_EQUAL("((m * x) + c)", parse("m * x + c")->to_string());
    BOOST_CHECK_EQUAL("((-(5 - 5)) * 6)", parse("-(5 - 5) * 6")->to_string());
    BOOST_CHECK_EQUAL("((5 && 1) || (0 && 7))", parse("5 && 1 || 0 && 7")->to_string());
}

BOOST_AUTO_TEST_CASE(basic_syntax_errors)
{
    //empty expr
    BOOST_CHECK_THROW(parse(""), SyntaxError);
    BOOST_CHECK_THROW(parse("5 + ()"), SyntaxError);
    //adjacent values
    BOOST_CHECK_THROW(parse("5 true"), SyntaxError);
    BOOST_CHECK_THROW(parse("(10 + 4) x"), SyntaxError);
    BOOST_CHECK_THROW(parse("(10 + 4) (54 / 5)"), SyntaxError);
    //unary operators, missing value
    BOOST_CHECK_THROW(parse("-"), SyntaxError);
    BOOST_CHECK_THROW(parse("- == 5"), SyntaxError);
    BOOST_CHECK_THROW(parse("5 == -"), SyntaxError);
    //binary operators, missing value
    BOOST_CHECK_THROW(parse("=="), SyntaxError);
    BOOST_CHECK_THROW(parse("5 =="), SyntaxError);
    BOOST_CHECK_THROW(parse("== 5"), SyntaxError);
    BOOST_CHECK_THROW(parse("5 == < 5"), SyntaxError);
    //unmatched group parenthesis
    BOOST_CHECK_THROW(parse("(5 + 5"), SyntaxError);
    BOOST_CHECK_THROW(parse("5 + 5)"), SyntaxError);
    BOOST_CHECK_THROW(parse("((5 + 5) * 8"), SyntaxError);
    //method call
    BOOST_CHECK_THROW(parse("."), SyntaxError);
    BOOST_CHECK_THROW(parse("a."), SyntaxError);
    BOOST_CHECK_THROW(parse("a.f("), SyntaxError);
    BOOST_CHECK_THROW(parse("a.f(b"), SyntaxError);
    BOOST_CHECK_THROW(parse("a.f(b,"), SyntaxError);
    BOOST_CHECK_THROW(parse("a.f(b,)"), SyntaxError);
    BOOST_CHECK_THROW(parse("a.f(b,,c)"), SyntaxError);
    BOOST_CHECK_THROW(parse("a.f(b)."), SyntaxError);

    BOOST_CHECK_THROW(parse("a[]"), SyntaxError);
    BOOST_CHECK_THROW(parse("a[a,]"), SyntaxError);
    BOOST_CHECK_THROW(parse("a[,a]"), SyntaxError);
    BOOST_CHECK_THROW(parse("a["), SyntaxError);
    BOOST_CHECK_THROW(parse("a[a"), SyntaxError);
    //array
    BOOST_CHECK_THROW(parse("["), SyntaxError);
    BOOST_CHECK_THROW(parse("]"), SyntaxError);
    BOOST_CHECK_THROW(parse("[,]"), SyntaxError);
    BOOST_CHECK_THROW(parse("[5"), SyntaxError);
    BOOST_CHECK_THROW(parse("[5,"), SyntaxError);
    BOOST_CHECK_THROW(parse("[5,]"), SyntaxError);
    //hash
    BOOST_CHECK_THROW(parse("{"), SyntaxError);
    BOOST_CHECK_THROW(parse("}"), SyntaxError);
    BOOST_CHECK_THROW(parse("{,}"), SyntaxError);
    BOOST_CHECK_THROW(parse("{5"), SyntaxError);
    BOOST_CHECK_THROW(parse("{5 =>"), SyntaxError);
    BOOST_CHECK_THROW(parse("{5 => 6"), SyntaxError);
    BOOST_CHECK_THROW(parse("{5 => 6,"), SyntaxError);
    BOOST_CHECK_THROW(parse("{5 => 6,}"), SyntaxError);
}

BOOST_AUTO_TEST_SUITE_END()

