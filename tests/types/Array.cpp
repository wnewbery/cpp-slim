#include <boost/test/unit_test.hpp>
#include "expression/Parser.hpp"
#include "expression/Ast.hpp"
#include "expression/Lexer.hpp"
#include "expression/Scope.hpp"
#include "types/Array.hpp"
#include "types/Number.hpp"
#include "types/String.hpp"
#include "../TestAccumulator.hpp"
#include "Error.hpp"

using namespace slim;
using namespace slim::expr;
BOOST_AUTO_TEST_SUITE(TestArray)

ObjectPtr eval_obj(const std::string &str, Scope &scope)
{
    Lexer lexer(str);
    expr::LocalVarNames vars;
    for (auto x : scope) vars.add(x.first->str());
    Parser parser( vars, lexer);
    auto expr = parser.full_expression();
    auto result = expr->eval(scope);
    return result;
}
std::string eval(const std::string &str, Scope &scope)
{
    return eval_obj(str, scope)->inspect();
}
std::string eval(const std::string &str)
{
    Scope scope(create_view_model());
    return eval(str, scope);
}
std::string eval(Ptr<ViewModel> model, const std::string &str)
{
    Scope scope(model);
    return eval(str, scope);
}

std::shared_ptr<Array> make_array2(const std::vector<double> &arr)
{
    std::vector<ObjectPtr> arr2;
    for (auto d : arr) arr2.push_back(make_value(d));
    return create_object<Array>(std::move(arr2));
}

BOOST_AUTO_TEST_CASE(compare)
{
    Scope scope(create_view_model());
    scope.set("a", make_array({}));
    scope.set("b", make_array2({ 5.0, 10.0 }));
    scope.set("c", make_array2({ 5.0, 10.0, 5.0 }));
    scope.set("c2", make_array2({ 5.0, 10.0, 5.0 }));
    scope.set("d", make_array2({ 5.0, 12.0 }));

    BOOST_CHECK_EQUAL("true", eval("a == a", scope));
    BOOST_CHECK_EQUAL("false", eval("a == b", scope));

    BOOST_CHECK_EQUAL("true", eval("c == c2", scope));
    BOOST_CHECK_EQUAL("false", eval("c != c2", scope));
    BOOST_CHECK_EQUAL("false", eval("c < c2", scope));
    BOOST_CHECK_EQUAL("true", eval("c <= c2", scope));
    BOOST_CHECK_EQUAL("false", eval("c > c2", scope));
    BOOST_CHECK_EQUAL("true", eval("c >= c2", scope));
    BOOST_CHECK_EQUAL("0", eval("c <=> c2", scope));

    BOOST_CHECK_EQUAL("-1", eval("a <=> b", scope));
    BOOST_CHECK_EQUAL("1", eval("b <=> a", scope));

    BOOST_CHECK_EQUAL("-1", eval("b <=> d", scope));
    BOOST_CHECK_EQUAL("1", eval("d <=> b", scope));
}
BOOST_AUTO_TEST_CASE(basic_methods)
{
    Scope scope(create_view_model());
    scope.set("a", make_array({}));
    scope.set("b", make_array2({ 5.0, 10.0 }));
    scope.set("c", make_array2({ 5.0, 10.0, 5.0 }));

    BOOST_CHECK_EQUAL("true", eval("a.frozen?", scope));
    BOOST_CHECK_EQUAL("true", eval("a.empty?", scope));
    BOOST_CHECK_EQUAL("false", eval("b.empty?", scope));

    //to_s, inspect
    BOOST_CHECK_EQUAL("\"[2, 1, \\\"a\\\"]\"", eval("[2, 1, 'a'].to_s"));
    BOOST_CHECK_EQUAL("\"[2, 1, \\\"a\\\"]\"", eval("[2, 1, 'a'].inspect"));

    //size
    BOOST_CHECK_EQUAL("0", eval("a.length", scope));
    BOOST_CHECK_EQUAL("0", eval("a.size", scope));
    BOOST_CHECK_EQUAL("2", eval("b.length", scope));
    BOOST_CHECK_EQUAL("2", eval("b.size", scope));

    BOOST_CHECK_EQUAL("0", eval("a.count", scope));
    BOOST_CHECK_EQUAL("2", eval("b.count", scope));
    BOOST_CHECK_EQUAL("0", eval("a.count(5)", scope));
    BOOST_CHECK_EQUAL("1", eval("b.count(5)", scope));
    BOOST_CHECK_EQUAL("2", eval("c.count(5)", scope));
    BOOST_CHECK_EQUAL("0", eval("c.count(7)", scope));
    
    //include?
    BOOST_CHECK_EQUAL("true", eval("b.include? 5", scope));
    BOOST_CHECK_EQUAL("false", eval("b.include? 9", scope));
    
    //reverse
    BOOST_CHECK_EQUAL("[]", eval("a.reverse", scope));
    BOOST_CHECK_EQUAL("[10, 5]", eval("b.reverse", scope));
    
    //sort
    BOOST_CHECK_EQUAL("[5, 5, 10]", eval("c.sort", scope));
}
BOOST_AUTO_TEST_CASE(add)
{
    BOOST_CHECK_EQUAL("[5, 10]", eval("[] + [5, 10]"));
    BOOST_CHECK_EQUAL("[5, 10, 5, 10, 5]", eval("[5, 10] + [5, 10, 5]"));
    BOOST_CHECK_EQUAL("[5, 10, 5, 5, 10]", eval("[5, 10, 5] + [5, 10]"));
}
BOOST_AUTO_TEST_CASE(sub)
{
    Scope scope(create_view_model());
    scope.set("a", make_array({}));
    scope.set("b", make_array2({ 5.0, 10.0 }));
    scope.set("c", make_array2({ 5.0, 10.0, 5.0, 7.0 }));

    BOOST_CHECK_EQUAL("[]", eval("a - b", scope));
    BOOST_CHECK_EQUAL("[5, 10]", eval("b - a", scope));
    BOOST_CHECK_EQUAL("[7]", eval("c - b", scope));
    BOOST_CHECK_EQUAL("[]", eval("b - c", scope));
}

BOOST_AUTO_TEST_CASE(basic_access)
{
    Scope scope(create_view_model());
    scope.set("a", make_array2({ 1.0, 2.0, 3.0, 5.0, 8.0, 11.0 }));
    scope.set("b", make_array({}));
    //at(index)
    BOOST_CHECK_EQUAL("nil", eval("b.at(0)", scope));
    BOOST_CHECK_EQUAL("1", eval("a.at(0)", scope));
    BOOST_CHECK_EQUAL("5", eval("a.at(3)", scope));
    BOOST_CHECK_EQUAL("8", eval("a.at(-2)", scope));
    BOOST_CHECK_EQUAL("nil", eval("a.at(6)", scope));
    BOOST_CHECK_EQUAL("1", eval("a.at(-6)", scope));
    BOOST_CHECK_EQUAL("nil", eval("a.at(-7)", scope));
    //fetch(index)
    BOOST_CHECK_EQUAL("2", eval("a.fetch(1)", scope));
    BOOST_CHECK_THROW(eval("a.fetch(-1)", scope), IndexError);
    BOOST_CHECK_THROW(eval("a.fetch(6)", scope), IndexError);
    //fetch(index, default)
    BOOST_CHECK_EQUAL("2", eval("a.fetch(1)", scope));
    BOOST_CHECK_EQUAL("88", eval("a.fetch(-1, 88)", scope));
    BOOST_CHECK_EQUAL("88", eval("a.fetch(6, 88)", scope));
    BOOST_CHECK_EQUAL("88", eval("b.fetch(0, 88)", scope));
    //first()
    BOOST_CHECK_EQUAL("1", eval("a.first", scope));
    BOOST_CHECK_EQUAL("nil", eval("b.first", scope));
    
    BOOST_CHECK_THROW(eval("[].first 1, 1"), ArgumentError);
    //first(n)
    BOOST_CHECK_EQUAL("[1, 2, 3]", eval("a.first(3)", scope));
    BOOST_CHECK_EQUAL("[1, 2, 3, 5, 8, 11]", eval("a.first(8)", scope));
    BOOST_CHECK_EQUAL("[]", eval("b.first(3)", scope));
    //last()
    BOOST_CHECK_EQUAL("11", eval("a.last", scope));
    BOOST_CHECK_EQUAL("nil", eval("b.last", scope));
    
    BOOST_CHECK_THROW(eval("[].last 1, 1"), ArgumentError);
    //last(n)
    BOOST_CHECK_EQUAL("[5, 8, 11]", eval("a.last(3)", scope));
    BOOST_CHECK_EQUAL("[1, 2, 3, 5, 8, 11]", eval("a.last(8)", scope));
    BOOST_CHECK_EQUAL("[]", eval("b.last(3)", scope));

    //slice(index)
    BOOST_CHECK_THROW(eval("[].slice 1, 1, 1"), ArgumentError);
    
    BOOST_CHECK_EQUAL("2", eval("a.slice(1)", scope));
    BOOST_CHECK_EQUAL("11", eval("a.slice(-1)", scope));
    BOOST_CHECK_EQUAL("nil", eval("a.slice(6)", scope));
    BOOST_CHECK_EQUAL("nil", eval("a.slice(-7)", scope));
    BOOST_CHECK_EQUAL("2", eval("a[1]", scope));
    //slice(start, index)
    BOOST_CHECK_EQUAL("[2, 3, 5]", eval("a.slice(1, 3)", scope));
    BOOST_CHECK_EQUAL("[8, 11]", eval("a.slice(4, 8)", scope));
    BOOST_CHECK_EQUAL("[11]", eval("a.slice(-1, 3)", scope));
    BOOST_CHECK_EQUAL("[1, 2, 3, 5, 8, 11]", eval("a.slice(-6, 10)", scope));
    BOOST_CHECK_EQUAL("[]", eval("a.slice(6, 3)", scope));
    BOOST_CHECK_EQUAL("[1, 2, 3]", eval("a.slice(-6, 3)", scope));
    BOOST_CHECK_EQUAL("nil", eval("a.slice(-7, 3)", scope));
    BOOST_CHECK_EQUAL("nil", eval("a.slice(7, 3)", scope));

    BOOST_CHECK_EQUAL("[1, 2, 3, 5, 8, 11]", eval("a[-6, 10]", scope));
    BOOST_CHECK_EQUAL("nil", eval("a[7, 3]", scope));
    //TODO: slice(range)

    //take(count)
    BOOST_CHECK_EQUAL("[1]", eval("a.take(1)", scope));
    BOOST_CHECK_EQUAL("[1, 2, 3]", eval("a.take(3)", scope));
    BOOST_CHECK_EQUAL("[1, 2, 3, 5, 8, 11]", eval("a.take(6)", scope));
    BOOST_CHECK_EQUAL("[1, 2, 3, 5, 8, 11]", eval("a.take(10)", scope));

    //values_at
    //TODO: With ranges
    BOOST_CHECK_EQUAL("[1, 3, 8]", eval("a.values_at(0, 2, 4)", scope));
    BOOST_CHECK_EQUAL("[1, 8, 3]", eval("a.values_at(0, 4, 2)", scope));
    BOOST_CHECK_EQUAL("[1, 8, 3, nil]", eval("a.values_at(0, 4, 2, 10)", scope));;
}

BOOST_AUTO_TEST_CASE(assoc)
{
    auto s1 = make_array({make_value("colors"), make_value("red"), make_value("blue"), make_value("green")});
    auto s2 = make_array({make_value("letters"), make_value("a"), make_value("b"), make_value("c")});
    auto s3 = make_value("foo");

    Scope scope(create_view_model());
    scope.set("a", make_array({s1, s2, s3}));
    //assoc
    BOOST_CHECK_EQUAL("[\"letters\", \"a\", \"b\", \"c\"]", eval("a.assoc('letters')", scope));
    BOOST_CHECK_EQUAL("nil", eval("a.assoc('foo')", scope));
    BOOST_CHECK_EQUAL("nil", eval("a.assoc('red')", scope));
    //rassoc
    BOOST_CHECK_EQUAL("nil", eval("a.rassoc('letters')", scope));
    BOOST_CHECK_EQUAL("nil", eval("a.rassoc('foo')", scope));
    BOOST_CHECK_EQUAL("[\"colors\", \"red\", \"blue\", \"green\"]", eval("a.rassoc('red')", scope));
}

BOOST_AUTO_TEST_CASE(flatten)
{
    BOOST_CHECK_EQUAL("[1, 2, 3]", eval("[1, 2, 3].flatten"));
    BOOST_CHECK_EQUAL("[1, 2, 3]", eval("[[1, [2, 3]]].flatten"));
    BOOST_CHECK_EQUAL("[1, [2, 3]]", eval("[[1, [2, 3]]].flatten 1"));
    BOOST_CHECK_THROW(eval("[1, 2, 3].flatten 1,1"), ArgumentError);
}

BOOST_AUTO_TEST_CASE(compact)
{
    Scope scope(create_view_model());
    scope.set("a", make_array({ make_value(1.0), make_value(5.0), make_value(5.0), make_value(0.0) }));
    scope.set("b", make_array({ NIL_VALUE, make_value(1.0), make_value(5.0), NIL_VALUE, make_value(5.0), make_value(0.0) }));

    BOOST_CHECK_EQUAL("[1, 5, 5, 0]", eval("a.compact()", scope));
    BOOST_CHECK_EQUAL("[1, 5, 5, 0]", eval("b.compact()", scope));
}
BOOST_AUTO_TEST_CASE(index)
{
    Scope scope(create_view_model());
    scope.set("a", make_array2({ 1.0, 2.0, 3.0, 5.0, 8.0, 5.0, 1.0 }));
    //index
    BOOST_CHECK_EQUAL("3", eval("a.index(5)", scope));
    BOOST_CHECK_EQUAL("nil", eval("a.index(5.5)", scope));
    //TODO: index block
    //TODO: index enumerator

    //rindex
    BOOST_CHECK_EQUAL("5", eval("a.rindex(5)", scope));
    BOOST_CHECK_EQUAL("nil", eval("a.rindex(5.5)", scope));
}
BOOST_AUTO_TEST_CASE(join)
{
    Scope scope(create_view_model());
    scope.set("a", make_array2({ 1.0, 2.0, 3.0, 5.0, 8.0, 11.0 }));
    BOOST_CHECK_EQUAL("\"1, 2, 3, 5, 8, 11\"", eval("a.join(', ')", scope));
}
BOOST_AUTO_TEST_CASE(rotate)
{
    Scope scope(create_view_model());
    scope.set("a", make_array2({ 1.0, 2.0, 3.0 }));
    scope.set("b", make_array2({}));


    BOOST_CHECK_EQUAL("[]", eval("b.rotate(0)", scope));
    BOOST_CHECK_EQUAL("[]", eval("b.rotate(1)", scope));
    BOOST_CHECK_EQUAL("[]", eval("b.rotate(-5)", scope));

    BOOST_CHECK_EQUAL("[1, 2, 3]", eval("a.rotate(0)", scope));
    BOOST_CHECK_EQUAL("[2, 3, 1]", eval("a.rotate(1)", scope));
    BOOST_CHECK_EQUAL("[3, 1, 2]", eval("a.rotate(2)", scope));
    BOOST_CHECK_EQUAL("[3, 1, 2]", eval("a.rotate(5)", scope));
    BOOST_CHECK_EQUAL("[3, 1, 2]", eval("a.rotate(-1)", scope));
    BOOST_CHECK_EQUAL("[2, 3, 1]", eval("a.rotate(-2)", scope));
    BOOST_CHECK_EQUAL("[2, 3, 1]", eval("a.rotate(-5)", scope));
    
    BOOST_CHECK_THROW(eval("[].rotate 1, 1"), ArgumentError);
}

BOOST_AUTO_TEST_CASE(uniq)
{
    Scope scope(create_view_model());
    scope.set("a", make_array({ make_value("5"), make_value(5.0), make_value(5.0), make_value("z") }));
    BOOST_CHECK_EQUAL("[\"5\", 5, \"z\"]", eval("a.uniq", scope));
}

BOOST_AUTO_TEST_CASE(enumerate)
{
    auto model = create_view_model();
    auto data = create_object<TestAccumulator>();
    model->set_attr("data", data);

    eval(model, "[].each{|x| @data.store x}");
    BOOST_CHECK_EQUAL("[]", data->check());

    eval(model, "[1, 2, 3].each{|x| @data.store x}");
    BOOST_CHECK_EQUAL("[1, 2, 3]", data->check());

    BOOST_CHECK_THROW(eval("[].each 1, 1"), ArgumentError);

    eval(model, "[5, 6, 9].each.each{|x| @data.store x}");
    BOOST_CHECK_EQUAL("[5, 6, 9]", data->check());}

BOOST_AUTO_TEST_SUITE_END()

