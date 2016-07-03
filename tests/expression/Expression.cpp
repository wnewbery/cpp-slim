#include <boost/test/unit_test.hpp>
#include "expression/Parser.hpp"
#include "expression/Ast.hpp"
#include "expression/Lexer.hpp"
#include "expression/Scope.hpp"
#include "Operators.hpp"
#include "Value.hpp"
#include "Error.hpp"

using namespace slim;
using namespace slim::expr;
BOOST_AUTO_TEST_SUITE(TestExpr)

std::string eval(const std::string &str, const FunctionTable &functions, Scope &scope)
{
    Lexer lexer(str);
    Parser parser(functions, lexer);
    auto expr = parser.parse_expression();
    auto result = expr->eval(scope);
    return result->inspect();
}
std::string eval(const std::string &str)
{
    FunctionTable functions;
    Scope scope;
    return eval(str, functions, scope);
}

BOOST_AUTO_TEST_CASE(literals)
{
    BOOST_CHECK_EQUAL("true", eval("true"));
    BOOST_CHECK_EQUAL("false", eval("false"));
    BOOST_CHECK_EQUAL("nil", eval("nil"));
    BOOST_CHECK_EQUAL("\"Hello World\"", eval("'Hello World'"));
    BOOST_CHECK_EQUAL("60", eval("60"));
}

BOOST_AUTO_TEST_CASE(variables)
{
    FunctionTable functions;
    Scope scope;
    scope.set("test", make_value(55.0));
    BOOST_CHECK_EQUAL("55", eval("test", functions, scope));
    BOOST_CHECK_EQUAL("nil", eval("unset", functions, scope));
}

BOOST_AUTO_TEST_CASE(operators)
{
    //unary
    BOOST_CHECK_EQUAL("-55", eval("-55"));
    BOOST_CHECK_EQUAL("55", eval("+55"));
    BOOST_CHECK_EQUAL("false", eval("!55"));
    BOOST_CHECK_EQUAL("false", eval("!true"));
    BOOST_CHECK_EQUAL("true", eval("!false"));
    BOOST_CHECK_EQUAL("true", eval("!nil"));

    //binary
    BOOST_CHECK_EQUAL("10", eval("55 && 10"));
    BOOST_CHECK_EQUAL("0", eval("55 && 0"));
    BOOST_CHECK_EQUAL("0", eval("0 && 10"));
    BOOST_CHECK_EQUAL("0", eval("0 && 0"));
    BOOST_CHECK_EQUAL("55", eval("55 || 10"));
    BOOST_CHECK_EQUAL("55", eval("55 || 0"));
    BOOST_CHECK_EQUAL("10", eval("0 || 10"));
    BOOST_CHECK_EQUAL("0", eval("0 || 0"));

    BOOST_CHECK_EQUAL("false", eval("55 == 10"));
    BOOST_CHECK_EQUAL("true", eval("10 == 10"));
    BOOST_CHECK_EQUAL("true", eval("55 != 10"));
    BOOST_CHECK_EQUAL("false", eval("10 != 10"));

    BOOST_CHECK_EQUAL("1", eval("55 <=> 10"));
    BOOST_CHECK_EQUAL("0", eval("10 <=> 10"));
    BOOST_CHECK_EQUAL("-1", eval("5 <=> 10"));

    BOOST_CHECK_EQUAL("false", eval("55 < 10"));
    BOOST_CHECK_EQUAL("false", eval("10 < 10"));
    BOOST_CHECK_EQUAL("true", eval("5 < 10"));
    BOOST_CHECK_EQUAL("false", eval("55 <= 10"));
    BOOST_CHECK_EQUAL("true", eval("10 <= 10"));
    BOOST_CHECK_EQUAL("true", eval("5 <= 10"));

    BOOST_CHECK_EQUAL("true", eval("55 > 10"));
    BOOST_CHECK_EQUAL("false", eval("10 > 10"));
    BOOST_CHECK_EQUAL("false", eval("5 > 10"));
    BOOST_CHECK_EQUAL("true", eval("55 >= 10"));
    BOOST_CHECK_EQUAL("true", eval("10 >= 10"));
    BOOST_CHECK_EQUAL("false", eval("5 >= 10"));

    BOOST_CHECK_EQUAL("65", eval("55 + 10"));
    BOOST_CHECK_EQUAL("45", eval("55 - 10"));
    BOOST_CHECK_EQUAL("550", eval("55 * 10"));
    BOOST_CHECK_EQUAL("5.5", eval("55 / 10"));
    BOOST_CHECK_EQUAL("5", eval("55 % 10"));
}

BOOST_AUTO_TEST_CASE(precedence)
{
    BOOST_CHECK_EQUAL("true", eval("20 == 100 / 5"));
    BOOST_CHECK_EQUAL("false", eval("20 == 100 / 6"));
    BOOST_CHECK_EQUAL("true", eval("20 == 100 == false"));
    BOOST_CHECK_EQUAL("true", eval("(20 == 100) == false"));
    BOOST_CHECK_EQUAL("false", eval("20 == (100 == false)"));
    BOOST_CHECK_EQUAL("1", eval("5 && 1 || 0 && 7"));
    BOOST_CHECK_EQUAL("7", eval("((5 && 1) || 0) && 7"));
}

BOOST_AUTO_TEST_CASE(global_func)
{
    auto func = [](const FunctionArgs &args) -> ObjectPtr
    {
        if (args.size() != 2) throw InvalidArgument("test");
        return args[0]->add(args[1].get());
    };
    FunctionTable functions = {{func, "func"}};

    Scope scope;
    scope.set("x", make_value(55.0));

    BOOST_CHECK_EQUAL("60", eval("func(x, 5)", functions, scope));
    BOOST_CHECK_EQUAL("60", eval("func x, 5", functions, scope));
    BOOST_CHECK_THROW(eval("func2()", functions, scope), NoSuchMethod);
    BOOST_CHECK_THROW(eval("func()", functions, scope), InvalidArgument);
    BOOST_CHECK_THROW(eval("func(10, 20, 30)", functions, scope), InvalidArgument);
    BOOST_CHECK_THROW(eval("func 10, 20, 30", functions, scope), InvalidArgument);
}

BOOST_AUTO_TEST_CASE(member_func)
{
    BOOST_CHECK_EQUAL("\"200\"", eval("20.to_s + '0'"));
    BOOST_CHECK_EQUAL("\"200\"", eval("20.to_s() + '0'"));
    BOOST_CHECK_EQUAL("\"300\"", eval("(20 + 10).to_s() + '0'"));

    BOOST_CHECK_EQUAL("200", eval("'20.5'.to_i * 10"));
    BOOST_CHECK_EQUAL("205", eval("'20.5'.to_f * 10"));

    BOOST_CHECK_THROW(eval("'x'.invalid"), NoSuchMethod);
}

BOOST_AUTO_TEST_CASE(runtime_error)
{
    BOOST_CHECK_THROW(eval("5 < true"), UnorderableTypeError);
    BOOST_CHECK_THROW(eval("5 < '5'"), UnorderableTypeError);
    BOOST_CHECK_THROW(eval("nil < nil"), UnorderableTypeError);

    BOOST_CHECK_THROW(eval("nil + nil"), NoSuchMethod);
    BOOST_CHECK_THROW(eval("nil - nil"), NoSuchMethod);
    BOOST_CHECK_THROW(eval("- nil"), NoSuchMethod);

    BOOST_CHECK_THROW(eval("5 + nil"), TypeError);
    BOOST_CHECK_THROW(eval("5 + true"), TypeError);
    BOOST_CHECK_THROW(eval("5 + 'str'"), TypeError);
    BOOST_CHECK_THROW(eval("'str' + 5"), TypeError);
}

BOOST_AUTO_TEST_CASE(short_circuit)
{
    //the operations on the right will cause an error if executed
    BOOST_CHECK_EQUAL("0", eval("0 && 5 + 'str'"));
    BOOST_CHECK_EQUAL("5", eval("5 || 5 + 'str'"));
}

BOOST_AUTO_TEST_SUITE_END()

