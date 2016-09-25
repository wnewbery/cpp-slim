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
BOOST_AUTO_TEST_SUITE(TestEnumerable)


std::string eval(Ptr<ViewModel> model, const std::string &str)
{
    Scope scope(model);
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
    return eval(create_view_model(), str);
}

BOOST_AUTO_TEST_CASE(all)
{
    BOOST_CHECK_EQUAL("true", eval("[].all?"));
    BOOST_CHECK_EQUAL("true", eval("[1, 2, 3].all?"));
    BOOST_CHECK_EQUAL("false", eval("[1, 2, nil, 3].all?"));

    BOOST_CHECK_EQUAL("true", eval("[1, 2, nil, 3].all? {|x| x != 5}"));
    BOOST_CHECK_EQUAL("false", eval("[5, 2, nil, 3].all? {|x| x != 5}"));
}

BOOST_AUTO_TEST_CASE(any)
{
    BOOST_CHECK_EQUAL("false", eval("[].any?"));
    BOOST_CHECK_EQUAL("false", eval("[nil, false, nil].any?"));
    BOOST_CHECK_EQUAL("true", eval("[1, 2, 3].any?"));
    BOOST_CHECK_EQUAL("true", eval("[1, 2, nil, 3].any?"));

    BOOST_CHECK_EQUAL("true", eval("[1, 2, nil, 3].any? {|x| x != 5}"));
    BOOST_CHECK_EQUAL("true", eval("[5, 2, nil, 3].any? {|x| x != 5}"));
    BOOST_CHECK_EQUAL("false", eval("[5, 5].any? {|x| x != 5}"));
}

BOOST_AUTO_TEST_CASE(count)
{
    BOOST_CHECK_EQUAL("0", eval("[].count"));
    BOOST_CHECK_EQUAL("4", eval("[1,2,3,5].count"));
    BOOST_CHECK_EQUAL("4", eval("[1,2,3,5].each.count"));

    BOOST_CHECK_EQUAL("1", eval("[1,2,3,5].count 5"));
    BOOST_CHECK_EQUAL("2", eval("[1,5,3,5].count 5"));

    BOOST_CHECK_EQUAL("2", eval("[1,2,3,5].count {|x| x > 2}"));

    BOOST_CHECK_THROW(eval("[].count 1, 2"), ArgumentCountError);
}

BOOST_AUTO_TEST_CASE(drop)
{
    BOOST_CHECK_EQUAL("[]", eval("[].drop 1"));
    BOOST_CHECK_EQUAL("[2, 3, 5]", eval("[1,2,3,5].drop 1"));
    BOOST_CHECK_EQUAL("[5]", eval("[1,2,3,5].drop 3"));
    BOOST_CHECK_EQUAL("[]", eval("[1].drop 3"));
}

BOOST_AUTO_TEST_CASE(drop_while)
{
    BOOST_CHECK_EQUAL("[]", eval("[].drop_while {|x| x < 5}"));
    BOOST_CHECK_EQUAL("[5]", eval("[1,2,3,5].drop_while {|x| x < 5}"));
    BOOST_CHECK_EQUAL("[5, 3, 2]", eval("[1,2,3,5,3,2].drop_while {|x| x < 5}"));

    BOOST_CHECK_EQUAL("[5, 3, 2]", eval("[1,2,3,5,3,2].drop_while.each {|x| x < 5}"));
    BOOST_CHECK_EQUAL("[5, 3, 2]", eval("[1,2,3,5,3,2].each.drop_while {|x| x < 5}"));
}

BOOST_AUTO_TEST_CASE(each_with_index)
{
    BOOST_CHECK_EQUAL("[]", eval("[].each_with_index.to_a"));
    BOOST_CHECK_EQUAL("[[5, 0], [6, 1], [7, 2]]", eval("[5,6,7].each_with_index.to_a"));
    BOOST_CHECK_EQUAL("{5 => 0, 6 => 1, 7 => 2}", eval("[5,6,7].each_with_index.to_h"));

    BOOST_CHECK_EQUAL("[[\"te,\", 0], [\"st\", 1]]", eval("'te,st'.each_line.each_with_index(',').to_a"));
    BOOST_CHECK_EQUAL("[[\"te,\", 0], [\"st\", 1]]", eval("'te,st'.each_line.each_with_index.to_a(',')"));
    BOOST_CHECK_EQUAL("{\"te,\" => 0, \"st\" => 1}", eval("'te,st'.each_line.each_with_index.to_h(',')"));
}

BOOST_AUTO_TEST_CASE(find)
{
    BOOST_CHECK_EQUAL("nil", eval("[].find{|x| x > 5}"));
    BOOST_CHECK_EQUAL("0", eval("[].find(0){|x| x > 5}"));

    BOOST_CHECK_EQUAL("5", eval("[1, 3, 5, 2].find{|x| x >= 5}"));
    BOOST_CHECK_EQUAL("5", eval("[1, 3, 5, 2].find(0){|x| x >= 5}"));
    BOOST_CHECK_EQUAL("0", eval("[1, 3, 5, 2].find(0){|x| x >= 6}"));

    BOOST_CHECK_EQUAL("5", eval("[1, 3, 5, 2].find.each{|x| x >= 5}"));
    BOOST_CHECK_EQUAL("nil", eval("[1, 3, 5, 2].find.each{|x| x >= 6}"));
    BOOST_CHECK_EQUAL("5", eval("[1, 3, 5, 2].find(0).each{|x| x >= 5}"));
    BOOST_CHECK_EQUAL("0", eval("[1, 3, 5, 2].find(0).each{|x| x >= 6}"));

    BOOST_CHECK_EQUAL("2", eval("[1, 3, 5, 2].find.each_with_index{|x, i| i == 3}"));
    BOOST_CHECK_EQUAL("nil", eval("[1, 3, 5, 2].find.each_with_index{|x, i| i == 4}"));
    BOOST_CHECK_EQUAL("2", eval("[1, 3, 5, 2].find(0).each_with_index{|x, i| i == 3}"));
    BOOST_CHECK_EQUAL("0", eval("[1, 3, 5, 2].find(0).each_with_index{|x, i| i == 4}"));

    BOOST_CHECK_EQUAL("[2, 3]", eval("[1, 3, 5, 2].each_with_index.find{|x, i| i == 3}"));
    BOOST_CHECK_EQUAL("nil", eval("[1, 3, 5, 2].each_with_index.find{|x, i| i == 4}"));
    BOOST_CHECK_EQUAL("[2, 3]", eval("[1, 3, 5, 2].each_with_index.find(0){|x, i| i == 3}"));
    BOOST_CHECK_EQUAL("0", eval("[1, 3, 5, 2].each_with_index.find(0){|x, i| i == 4}"));
}

BOOST_AUTO_TEST_CASE(find_index)
{
    BOOST_CHECK_EQUAL("nil", eval("[].find_index{|x| x > 5}"));
    //with proc
    BOOST_CHECK_EQUAL("2", eval("[1, 3, 5, 2].find_index{|x| x >= 5}"));

    BOOST_CHECK_EQUAL("2", eval("[1, 3, 5, 2].find_index.each{|x| x >= 5}"));
    BOOST_CHECK_EQUAL("nil", eval("[1, 3, 5, 2].find_index.each{|x| x >= 6}"));

    BOOST_CHECK_EQUAL("3", eval("[1, 3, 5, 2].find_index.each_with_index{|x, i| i == 3}"));
    BOOST_CHECK_EQUAL("nil", eval("[1, 3, 5, 2].find_index.each_with_index{|x, i| i == 4}"));

    BOOST_CHECK_EQUAL("3", eval("[1, 3, 5, 2].each_with_index.find_index{|x, i| i == 3}"));
    BOOST_CHECK_EQUAL("nil", eval("[1, 3, 5, 2].each_with_index.find_index{|x, i| i == 4}"));
    //with value
    BOOST_CHECK_EQUAL("2", eval("[1, 3, 5, 2].find_index 5"));
    BOOST_CHECK_EQUAL("2", eval("[1, 5, 5, 2].each_with_index.find_index([5, 2])"));

    BOOST_CHECK_THROW(eval("[].find_index 1, 2"), ArgumentCountError);
}

BOOST_AUTO_TEST_CASE(flat_map)
{
    BOOST_CHECK_EQUAL("[]", eval("[].flat_map{|x| x*2}"));
    BOOST_CHECK_EQUAL("[2, 10, 4]", eval("[1, 5, 2].flat_map{|x| x*2}"));
    BOOST_CHECK_EQUAL("[1, 1, 5, 5, 2, 2]", eval("[1, 5, 2].flat_map{|x| [x,x]}"));
    BOOST_CHECK_EQUAL("[1, 1, 5, 5, 2, 2]", eval("[1, 5, 2].flat_map.each{|x| [x,x]}"));
}

BOOST_AUTO_TEST_CASE(group_by)
{
    BOOST_CHECK_EQUAL("{}", eval("[].group_by{|x| (x / 5).to_i}"));
    BOOST_CHECK_EQUAL("{0 => [1, 3], 2 => [11], 1 => [7]}", eval("[1, 11, 3, 7].group_by{|x| (x / 5).to_i}"));
    BOOST_CHECK_EQUAL("{0 => [1, 3], 2 => [11], 1 => [7]}", eval("[1, 11, 3, 7].group_by.each{|x| (x / 5).to_i}"));
    BOOST_CHECK_EQUAL("{0 => [1, 3], 1 => [11, 7]}", eval("[1, 11, 3, 7].group_by.each_with_index{|x,i| i % 2}"));
}

BOOST_AUTO_TEST_CASE(map)
{
    BOOST_CHECK_EQUAL("[2, 4, 8, 10]", eval("[1,2,4,5].map{|x| x*2}"));
    BOOST_CHECK_EQUAL("[2, 4, 8, 10]", eval("[1,2,4,5].collect{|x| x*2}"));

    BOOST_CHECK_EQUAL("[2, 4, 8, 10]", eval("[1,2,4,5].each.map{|x| x*2}"));
    BOOST_CHECK_EQUAL("[2, 4, 8, 10]", eval("[1,2,4,5].map.each{|x| x*2}"));
    BOOST_CHECK_EQUAL("[2, 4, 8, 10]", eval("[1,2,4,5].map.map{|x| x*2}"));
}

BOOST_AUTO_TEST_CASE(max)
{
    BOOST_CHECK_EQUAL("nil", eval("[].max"));
    BOOST_CHECK_EQUAL("5", eval("[1,5,3,5].max"));

    BOOST_CHECK_EQUAL("[]", eval("[].max 0"));
    BOOST_CHECK_EQUAL("[]", eval("[].max 2"));
    BOOST_CHECK_EQUAL("[5, 5]", eval("[1,5,3,5].max 2"));
    BOOST_CHECK_EQUAL("[5, 3]", eval("[1,5,3].max 2"));
    BOOST_CHECK_EQUAL("[5, 3]", eval("[5,3].max 3"));


    BOOST_CHECK_EQUAL("nil", eval("[].max {|a, b| a.abs <=> b.abs}"));
    BOOST_CHECK_EQUAL("-5", eval("[4, -5].max {|a, b| a.abs <=> b.abs}"));
    BOOST_CHECK_EQUAL("[6, -5]", eval("[4, -5, 6, 3].max 2 {|a, b| a.abs <=> b.abs}"));
}

BOOST_AUTO_TEST_CASE(max_by)
{
    BOOST_CHECK_EQUAL("nil", eval("[].max_by{|x| x.abs}"));
    BOOST_CHECK_EQUAL("-6", eval("[1,5,-6,5].max_by{|x| x.abs}"));
    BOOST_CHECK_EQUAL("-6", eval("[1,5,-6,5].max_by.each{|x| x.abs}"));

    BOOST_CHECK_EQUAL("[-6, 5]", eval("[1,5,-6,5].max_by 2{|x| x.abs}"));
}

BOOST_AUTO_TEST_CASE(min_by)
{
    BOOST_CHECK_EQUAL("nil", eval("[].min_by{|x| x.abs}"));
    BOOST_CHECK_EQUAL("-1", eval("[-1,1,-6,5].min_by{|x| x.abs}"));
    BOOST_CHECK_EQUAL("1", eval("[1,5,-6,5].min_by.each{|x| x.abs}"));

    BOOST_CHECK_EQUAL("[1, 5]", eval("[1,5,-6,5].min_by 2{|x| x.abs}"));
}

BOOST_AUTO_TEST_CASE(minmax)
{
    BOOST_CHECK_EQUAL("[nil, nil]", eval("[].minmax"));
    BOOST_CHECK_EQUAL("[1, 1]", eval("[1].minmax"));
    BOOST_CHECK_EQUAL("[0, 6]", eval("[1, 6, 2, 0, 4].minmax"));

    BOOST_CHECK_EQUAL("[0, -6]", eval("[1, -6, 2, 0, 4].minmax{|a, b| a.abs <=> b.abs}"));
}

BOOST_AUTO_TEST_CASE(minmax_by)
{
    BOOST_CHECK_EQUAL("[nil, nil]", eval("[].minmax_by{|x| x.abs}"));
    BOOST_CHECK_EQUAL("[1, 1]", eval("[1].minmax_by{|x| x.abs}"));
    BOOST_CHECK_EQUAL("[0, -6]", eval("[1, -6, -2, 0, 4].minmax_by{|x| x.abs}"));

    BOOST_CHECK_EQUAL("[0, -6]", eval("[1, -6, -2, 0, 4].minmax_by.each{|x| x.abs}"));
}

BOOST_AUTO_TEST_CASE(min)
{
    BOOST_CHECK_EQUAL("nil", eval("[].min"));
    BOOST_CHECK_EQUAL("1", eval("[1,5,3,5].min"));

    BOOST_CHECK_EQUAL("[]", eval("[].min 0"));
    BOOST_CHECK_EQUAL("[]", eval("[].min 2"));
    BOOST_CHECK_EQUAL("[1, 1]", eval("[1,5,3,1].min 2"));
    BOOST_CHECK_EQUAL("[1, 3]", eval("[1,5,3].min 2"));
    BOOST_CHECK_EQUAL("[3, 5]", eval("[5,3].min 3"));


    BOOST_CHECK_EQUAL("nil", eval("[].min {|a, b| a.abs <=> b.abs}"));
    BOOST_CHECK_EQUAL("4", eval("[4, -5].min {|a, b| a.abs <=> b.abs}"));
    BOOST_CHECK_EQUAL("[3, -4]", eval("[-4, -5, 6, 3].min 2 {|a, b| a.abs <=> b.abs}"));
}

BOOST_AUTO_TEST_CASE(reduce)
{
    BOOST_CHECK_EQUAL("nil", eval("[].reduce{|memo, val| memo + val}"));
    BOOST_CHECK_EQUAL("2", eval("[].reduce 2 {|memo, val| memo + val}"));
    BOOST_CHECK_EQUAL("5", eval("[5].reduce{|memo, val| memo + val}"));
    BOOST_CHECK_EQUAL("7", eval("[5].reduce 2 {|memo, val| memo + val}"));
    BOOST_CHECK_EQUAL("6", eval("[1, 2, 3].reduce{|memo, val| memo + val}"));
}

BOOST_AUTO_TEST_CASE(reject)
{
    BOOST_CHECK_EQUAL("[]", eval("[1, 5, 3, 7].reject{|x| true}"));
    BOOST_CHECK_EQUAL("[1, 5, 3, 7]", eval("[1, 5, 3, 7].reject{|x| x > 9}"));
    BOOST_CHECK_EQUAL("[1, 3]", eval("[1, 5, 3, 7].reject{|x| x > 4}"));
    BOOST_CHECK_EQUAL("[1, 3]", eval("[1, 5, 3, 7].reject.each{|x| x > 4}"));

    BOOST_CHECK_EQUAL("[1, 5]", eval("[1, 5, 3, 7].reject.each_with_index{|x, i| i > 1}"));
    BOOST_CHECK_EQUAL("[[1, 0], [5, 1]]", eval("[1, 5, 3, 7].each_with_index.reject{|x, i| i > 1}"));
}

BOOST_AUTO_TEST_CASE(select)
{
    BOOST_CHECK_EQUAL("[]", eval("[1, 5, 3, 7].select{|x| x > 9}"));
    BOOST_CHECK_EQUAL("[5, 7]", eval("[1, 5, 3, 7].select{|x| x > 4}"));
    BOOST_CHECK_EQUAL("[5, 7]", eval("[1, 5, 3, 7].select.each{|x| x > 4}"));

    BOOST_CHECK_EQUAL("[3, 7]", eval("[1, 5, 3, 7].select.each_with_index{|x, i| i > 1}"));
    BOOST_CHECK_EQUAL("[[3, 2], [7, 3]]", eval("[1, 5, 3, 7].each_with_index.select{|x, i| i > 1}"));
}

BOOST_AUTO_TEST_CASE(to_a)
{
    BOOST_CHECK_EQUAL("[1, 2, 3]", eval("[1, 2, 3].to_a"));
    BOOST_CHECK_EQUAL("[[1, 6], 2, [3]]", eval("[[1, 6], 2, [3]].to_a"));
    BOOST_CHECK_EQUAL("[[1, 6], 2, [3]]", eval("[[1, 6], 2, [3]].entries"));

    BOOST_CHECK_EQUAL("[\"te,\", \"st\"]", eval("'te,st'.each_line.to_a(',')"));
}

BOOST_AUTO_TEST_CASE(to_h)
{
    BOOST_CHECK_EQUAL("{1 => 6, 2 => 7, 3 => 8}", eval("[[1, 6], [2, 7], [3, 8]].to_h"));
    BOOST_CHECK_THROW(eval("[1].to_h"), TypeError);
    BOOST_CHECK_THROW(eval("[[]].to_h"), ArgumentError);
    BOOST_CHECK_THROW(eval("[[1]].to_h"), ArgumentError);
    BOOST_CHECK_THROW(eval("[[1, 2, 3]].to_h"), ArgumentError);
}

BOOST_AUTO_TEST_SUITE_END()

