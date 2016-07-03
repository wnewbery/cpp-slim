#include <boost/test/unit_test.hpp>
#include "expression/Parser.hpp"
#include "expression/Ast.hpp"
#include "expression/Lexer.hpp"
#include "expression/Scope.hpp"
#include "types/Array.hpp"
#include "types/Hash.hpp"
#include "types/Number.hpp"
#include "types/String.hpp"
#include "Error.hpp"

using namespace slim;
using namespace slim::expr;
BOOST_AUTO_TEST_SUITE(TestHash)

std::string eval(const std::string &str, Scope &scope)
{
    FunctionTable functions;
    Lexer lexer(str);
    Parser parser(functions, lexer);
    auto expr = parser.parse_expression();
    auto result = expr->eval(scope);
    return result->to_string();
}
std::string eval(const std::string &str)
{
    Scope scope;
    return eval(str, scope);
}

std::shared_ptr<Hash> make_hash2(const std::unordered_map<std::string, double> &map)
{
    ObjectMap map2;
    for (auto &i : map)
    {
        map2[make_value(i.first)] = make_value(i.second);
    }
    return make_hash(std::move(map2));
}

BOOST_AUTO_TEST_CASE(compare)
{
    Scope scope;
    scope.set("a", make_hash2({}));
    scope.set("b", make_hash2({ {"a", 5.0}, {"b", 10.0} }));
    scope.set("c", make_hash2({ {"a", 5.0}, {"c", 15} }));


    BOOST_CHECK_EQUAL("true", eval("a == a", scope));
    BOOST_CHECK_EQUAL("true", eval("a == {}", scope));
    BOOST_CHECK_EQUAL("false", eval("a == b", scope));
    BOOST_CHECK_EQUAL("true", eval("a != b", scope));

    BOOST_CHECK_EQUAL("true", eval("c == {a: 5, c: 15}", scope));
    BOOST_CHECK_EQUAL("false", eval("c != {a: 5, c: 15}", scope));

    BOOST_CHECK_THROW(eval("c < c2", scope), UnorderableTypeError);
    BOOST_CHECK_THROW(eval("c <= c2", scope), UnorderableTypeError);
    BOOST_CHECK_THROW(eval("c > c2", scope), UnorderableTypeError);
    BOOST_CHECK_THROW(eval("c >= c2", scope), UnorderableTypeError);
    BOOST_CHECK_THROW(eval("c <=> c2", scope), UnorderableTypeError);
}
BOOST_AUTO_TEST_CASE(basic_methods)
{
    Scope scope;
    //BOOST_CHECK_EQUAL("true", eval("a.frozen?", scope));
    //BOOST_CHECK_EQUAL("true", eval("a.empty?", scope));
    //BOOST_CHECK_EQUAL("false", eval("b.empty?", scope));

    //size
    BOOST_CHECK_EQUAL("0", eval("{}.length", scope));
    BOOST_CHECK_EQUAL("0", eval("{}.size", scope));
    BOOST_CHECK_EQUAL("2", eval("{a:5, b: 6}.length", scope));
    BOOST_CHECK_EQUAL("2", eval("{a:5, b: 6}.size", scope));
}

BOOST_AUTO_TEST_CASE(basic_access)
{
    Scope scope;
    scope.set("a", make_hash2({}));
    scope.set("b", make_hash2({ { "a", 5.0 },{ "b", 10.0 } }));
    scope.set("c", make_hash2({ { "a", 5.0 },{ "c", 15}, {"10", 20} }));

    //[key]
    BOOST_CHECK_EQUAL("nil", eval("c['x']", scope));
    BOOST_CHECK_EQUAL("5", eval("c['a']", scope));
    BOOST_CHECK_EQUAL("20", eval("c['10']", scope));
    BOOST_CHECK_EQUAL("nil", eval("c[10]", scope));
    //fetch(index)
    BOOST_CHECK_EQUAL("5", eval("c.fetch('a')", scope));
    BOOST_CHECK_THROW(eval("c.fetch('x')", scope), KeyError);
    BOOST_CHECK_THROW(eval("c.fetch(10)", scope), KeyError);
    //fetch(index, default)
    BOOST_CHECK_EQUAL("2", eval("a.fetch('a', 2)", scope));
    BOOST_CHECK_EQUAL("20", eval("c.fetch('10', 88)", scope));
    BOOST_CHECK_EQUAL("88", eval("c.fetch(10, 88)", scope));

    //key?
    //BOOST_CHECK_EQUAL("true", eval("c.key?('10')", scope));
    //BOOST_CHECK_EQUAL("false", eval("c.key?(10)", scope));

    //value?
    //BOOST_CHECK_EQUAL("true", eval("c.value?(5)", scope));
    //BOOST_CHECK_EQUAL("false", eval("c.value?('5')", scope));

    BOOST_CHECK_EQUAL("[\"a\", \"b\"]", eval("b.keys", scope));
    BOOST_CHECK_EQUAL("[5, 10]", eval("b.values", scope));
}

BOOST_AUTO_TEST_CASE(invert)
{
    BOOST_CHECK_EQUAL("{5 => \"a\", [15, 20] => \"b\"}", eval("{a: 5, b: [15, 20]}.invert"));
}

BOOST_AUTO_TEST_CASE(flatten)
{
    BOOST_CHECK_EQUAL("[\"a\", 5, \"b\", [15, 20, [-1, -2]]]", eval("{a: 5, b: [15, 20, [-1, -2]]}.flatten"));
    BOOST_CHECK_EQUAL("[\"a\", 5, \"b\", 15, 20, [-1, -2]]", eval("{a: 5, b: [15, 20, [-1, -2]]}.flatten 2"));
    BOOST_CHECK_EQUAL("[\"a\", 5, \"b\", 15, 20, -1, -2]", eval("{a: 5, b: [15, 20, [-1, -2]]}.flatten 3"));
}

BOOST_AUTO_TEST_CASE(to_a)
{
    BOOST_CHECK_EQUAL("[[\"a\", 5], [\"b\", 10]]", eval("{a: 5, b: 10}.to_a"));
}

BOOST_AUTO_TEST_CASE(merge)
{
    BOOST_CHECK_EQUAL("{\"a\" => 5, \"b\" => 15, \"c\" => 20}", eval("{a: 5, b: 10}.merge({c: 20, b: 15})"));
}

BOOST_AUTO_TEST_SUITE_END()

