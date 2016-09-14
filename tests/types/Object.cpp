#include <boost/test/unit_test.hpp>
#include "expression/Parser.hpp"
#include "expression/Ast.hpp"
#include "expression/Lexer.hpp"
#include "expression/Scope.hpp"
#include "types/Object.hpp"
#include <regex>

using namespace slim;
using namespace slim::expr;


BOOST_AUTO_TEST_SUITE(TestObject)

/**Simplest valid object.*/
class Test : public Object
{
public:
    std::string name = "Test";
    virtual const std::string& type_name()const override { return name; }
};

std::string eval(const std::string &str)
{
    Lexer lexer(str);
    expr::LocalVarNames vars;
    Parser parser(vars, lexer);
    auto expr = parser.full_expression();
    ScopeAttributes attrs;
    attrs.set("b", create_object<Test>());
    Scope scope(attrs);
    scope.set("self", create_object<Test>());
    return expr->eval(scope)->inspect();
}

BOOST_AUTO_TEST_CASE(test)
{
    auto obj = create_object<Test>();
    BOOST_CHECK_EQUAL("Test", obj->type_name());
    BOOST_CHECK_EQUAL("true", eval("self == self"));
    BOOST_CHECK_EQUAL("false", eval("self == @b"));
    BOOST_CHECK_EQUAL("false", eval("self != self"));
    BOOST_CHECK_EQUAL("true", eval("self != @b"));
    BOOST_CHECK(std::regex_match(obj->inspect(), std::regex("#<Test: [0-9a-f]{8,16}>")));
    BOOST_CHECK_EQUAL("true", eval("self.to_s == self.inspect"));
    BOOST_CHECK_THROW(eval("self <=> @b"), UnorderableTypeError);
    BOOST_CHECK_THROW(eval("self < @b"), UnorderableTypeError);
    BOOST_CHECK_THROW(eval("self > @b"), UnorderableTypeError);
    BOOST_CHECK_THROW(eval("self[0]"), NoSuchMethod);
    BOOST_CHECK_THROW(eval("self + @b"), NoSuchMethod);
    BOOST_CHECK_THROW(as_number(obj), TypeError);

    //All objects work as hash keys
    BOOST_CHECK_EQUAL("1", eval("{self => 1, @b => 2}[self]"));
    BOOST_CHECK_EQUAL("2", eval("{self => 1, @b => 2}[@b]"));
}
BOOST_AUTO_TEST_SUITE_END()
