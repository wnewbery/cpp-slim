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

std::string match_inspect(const std::string &pattern, const std::string &str)
{
    size_t max_digits = sizeof(void*) * 2;
    for (size_t i = 0, j = 0; i < pattern.size(); ++i)
    {
        if (j == str.size()) return str;

        if (pattern[i] == '*')
        {
            for (size_t k = 0; k < max_digits; ++k, ++j)
            {
                if (j == str.size()) return str;
                auto c = str[j];
                if ((c < '0' || c > '9') && (c < 'a' || c > 'f')) break;
            }
        }
        else
        {
            if (pattern[i] != str[j]) return str;
            ++j;
        }
    }
    return "";
}

BOOST_AUTO_TEST_CASE(test)
{
    auto obj = create_object<Test>();
    BOOST_CHECK_EQUAL("Test", obj->type_name());
    BOOST_CHECK_EQUAL("1", eval("self ? 1 : 2"));
    BOOST_CHECK_EQUAL("true", eval("self == self"));
    BOOST_CHECK_EQUAL("false", eval("self == @b"));
    BOOST_CHECK_EQUAL("false", eval("self != self"));
    BOOST_CHECK_EQUAL("true", eval("self != @b"));
    BOOST_CHECK_EQUAL("", match_inspect("#<Test: *>", obj->inspect()));
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
