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
    Lexer lexer(str);
    expr::LocalVarNames vars;
    for (auto x : scope) vars.add(x.first->str());
    Parser parser(vars, lexer);
    auto expr = parser.full_expression();
    auto result = expr->eval(scope);
    return result->inspect();
}
std::string eval(const std::string &str)
{
    Scope scope(create_view_model());
    return eval(str, scope);
}

std::shared_ptr<Hash> make_hash2(const std::vector<std::pair<std::string, double>> &map)
{
    auto map2 = create_object<Hash>();
    for (auto &i : map)
    {
        map2->set(make_value(i.first), make_value(i.second));
    }
    return map2;
}

BOOST_AUTO_TEST_CASE(dup)
{
    Scope scope(create_view_model());
    auto a = make_hash2({ { "a", 5.0 }, { "x", 10 }, { "b", 1} });
    scope.set("a", a);

    BOOST_CHECK_EQUAL("true", eval("a == a.dup", scope));
    BOOST_CHECK_EQUAL("[\"a\", \"x\", \"b\"]", eval("a.dup.keys", scope));
    BOOST_CHECK_EQUAL("[5, 10, 1]", eval("a.dup.values", scope));
    BOOST_CHECK(a != a->dup());
}

BOOST_AUTO_TEST_CASE(compare)
{
    Scope scope(create_view_model());
    scope.set("a", make_hash2({}));
    scope.set("b", make_hash2({ { "a", 5.0 },{ "b", 10.0 } }));
    scope.set("c", make_hash2({ { "a", 5.0 },{ "c", 15 } }));
    scope.set("c2", NIL_VALUE);


    BOOST_CHECK_EQUAL("true", eval("a == a", scope));
    BOOST_CHECK_EQUAL("true", eval("a == {}", scope));
    BOOST_CHECK_EQUAL("false", eval("a == b", scope));
    BOOST_CHECK_EQUAL("true", eval("a != b", scope));

    BOOST_CHECK_EQUAL("true", eval("c == {'a' => 5, 'c' => 15}", scope));
    BOOST_CHECK_EQUAL("false", eval("c != {'a' => 5, 'c' => 15}", scope));

    BOOST_CHECK_THROW(eval("c < c2", scope), UnorderableTypeError);
    BOOST_CHECK_THROW(eval("c <= c2", scope), UnorderableTypeError);
    BOOST_CHECK_THROW(eval("c > c2", scope), UnorderableTypeError);
    BOOST_CHECK_THROW(eval("c >= c2", scope), UnorderableTypeError);
    BOOST_CHECK_THROW(eval("c <=> c2", scope), UnorderableTypeError);
}
BOOST_AUTO_TEST_CASE(basic_methods)
{
    BOOST_CHECK_EQUAL("true", eval("{}.empty?"));
    BOOST_CHECK_EQUAL("false", eval("{a: 5}.empty?"));

    //size
    BOOST_CHECK_EQUAL("0", eval("{}.length"));
    BOOST_CHECK_EQUAL("0", eval("{}.size"));
    BOOST_CHECK_EQUAL("2", eval("{a:5, b: 6}.length"));
    BOOST_CHECK_EQUAL("2", eval("{a:5, b: 6}.size"));

    //to_s, inspect
    BOOST_CHECK_EQUAL("\"{1 => 2}\"", eval("{1 => 2}.to_s"));
    BOOST_CHECK_EQUAL("\"{1 => 2}\"", eval("{1 => 2}.inspect"));
}

BOOST_AUTO_TEST_CASE(basic_access)
{
    Scope scope(create_view_model());
    scope.set("a", make_hash2({}));
    scope.set("b", make_hash2({ { "a", 5.0 },{ "b", 10.0 } }));
    scope.set("c", make_hash2({ { "a", 5.0 },{ "c", 15 },{ "10", 20 } }));

    //[key]
    BOOST_CHECK_EQUAL("nil", eval("c['x']", scope));
    BOOST_CHECK_EQUAL("5", eval("c['a']", scope));
    BOOST_CHECK_EQUAL("20", eval("c['10']", scope));
    BOOST_CHECK_EQUAL("nil", eval("c[10]", scope));
    BOOST_CHECK_EQUAL("1", eval("{{a: 5} => 1, y: 2}[{a: 5}]"));
    BOOST_CHECK_EQUAL("nil", eval("{{a: 4} => 1, y: 2}[{a: 5}]"));
    //fetch(index)
    BOOST_CHECK_EQUAL("5", eval("c.fetch('a')", scope));
    BOOST_CHECK_THROW(eval("c.fetch('x')", scope), KeyError);
    BOOST_CHECK_THROW(eval("c.fetch(10)", scope), KeyError);
    //fetch(index, default)
    BOOST_CHECK_EQUAL("2", eval("a.fetch('a', 2)", scope));
    BOOST_CHECK_EQUAL("20", eval("c.fetch('10', 88)", scope));
    BOOST_CHECK_EQUAL("88", eval("c.fetch(10, 88)", scope));
    
    
    //key
    BOOST_CHECK_EQUAL(":a", eval("{b: 6, a: 4}.key 4"));
    BOOST_CHECK_EQUAL(":a", eval("{b: 6, a: 4, c: 4}.key 4"));
    BOOST_CHECK_EQUAL("nil", eval("{b: 6, a: 4, c: 4}.key 5"));

    //key?
    BOOST_CHECK_EQUAL("true", eval("c.key?('10')", scope));
    BOOST_CHECK_EQUAL("false", eval("c.key?(10)", scope));

    //value?
    BOOST_CHECK_EQUAL("true", eval("c.value?(5)", scope));
    BOOST_CHECK_EQUAL("false", eval("c.value?('5')", scope));

    BOOST_CHECK_EQUAL("[\"a\", \"b\"]", eval("b.keys", scope));
    BOOST_CHECK_EQUAL("[5, 10]", eval("b.values", scope));
}

BOOST_AUTO_TEST_CASE(invert)
{
    BOOST_CHECK_EQUAL("{5 => :a, [15, 20] => :b}", eval("{a: 5, b: [15, 20]}.invert"));
}

BOOST_AUTO_TEST_CASE(flatten)
{
    BOOST_CHECK_EQUAL("[:a, 5, :b, [15, 20, [-1, -2]]]", eval("{a: 5, b: [15, 20, [-1, -2]]}.flatten"));
    BOOST_CHECK_EQUAL("[:a, 5, :b, 15, 20, [-1, -2]]", eval("{a: 5, b: [15, 20, [-1, -2]]}.flatten 2"));
    BOOST_CHECK_EQUAL("[:a, 5, :b, 15, 20, -1, -2]", eval("{a: 5, b: [15, 20, [-1, -2]]}.flatten 3"));
}

BOOST_AUTO_TEST_CASE(convert)
{
    BOOST_CHECK_EQUAL("[[:a, 5], [:b, 10]]", eval("{a: 5, b: 10}.to_a"));
    BOOST_CHECK_EQUAL("{:a => 5, :b => 10}", eval("{a: 5, b: 10}.to_h"));
}

BOOST_AUTO_TEST_CASE(merge)
{
    BOOST_CHECK_EQUAL("{:a => 5, :b => 15, :c => 20}", eval("{a: 5, b: 10}.merge({c: 20, b: 15})"));
}

BOOST_AUTO_TEST_CASE(each)
{
    BOOST_CHECK_EQUAL("[[5, 7], [3, 2], [1, 7]]", eval("{5 => 7, 3 => 2, 1 => 7}.each.to_a"));
    BOOST_CHECK_EQUAL("[5, 3, 1]", eval("{5 => 7, 3 => 2, 1 => 7}.each_key.to_a"));
    BOOST_CHECK_EQUAL("[7, 2, 7]", eval("{5 => 7, 3 => 2, 1 => 7}.each_value.to_a"));
}

BOOST_AUTO_TEST_SUITE_END()

