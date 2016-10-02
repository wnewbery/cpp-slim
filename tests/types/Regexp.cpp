#include <boost/test/unit_test.hpp>
#include "expression/Parser.hpp"
#include "expression/Ast.hpp"
#include "expression/Lexer.hpp"
#include "expression/Scope.hpp"
#include "types/Regexp.hpp"
#include <cmath>

using namespace slim;
using namespace slim::expr;
BOOST_AUTO_TEST_SUITE(TestRegexp)

std::string eval(const std::string &str)
{
    Lexer lexer(str);
    expr::LocalVarNames vars;
    Parser parser(vars, lexer);
    auto expr = parser.full_expression();
    auto model = create_view_model();
    model->add_constant("Regexp", create_object<RegexpType>());
    Scope scope(model);
    return expr->eval(scope)->inspect();
}


BOOST_AUTO_TEST_CASE(match)
{
    BOOST_CHECK_EQUAL("true", eval("Regexp.new('test').match('test') != nil"));
    BOOST_CHECK_EQUAL("true", eval("Regexp.new('test').match('testing') != nil"));
    BOOST_CHECK_EQUAL("true", eval("Regexp.new('test').match('hello test') != nil"));

    BOOST_CHECK_EQUAL("true", eval("Regexp.new('^test$').match('test') != nil"));
    BOOST_CHECK_EQUAL("false", eval("Regexp.new('^test$').match('testing') != nil"));
    BOOST_CHECK_EQUAL("false", eval("Regexp.new('^test$').match('hello test') != nil"));


    BOOST_CHECK_EQUAL("false", eval("Regexp.new('test').match('TEST') != nil"));
    BOOST_CHECK_EQUAL("true", eval("Regexp.new('test', Regexp::IGNORECASE).match('TEST') != nil"));

    BOOST_CHECK_EQUAL("false", eval("Regexp.new('test').match('test', 1) != nil"));
    BOOST_CHECK_EQUAL("true", eval("Regexp.new('test').match('ttest', 1) != nil"));
    BOOST_CHECK_EQUAL("true", eval("Regexp.new('').match('test') != nil"));
    BOOST_CHECK_EQUAL("true", eval("Regexp.new('').match('test', 3) != nil"));
    BOOST_CHECK_EQUAL("true", eval("Regexp.new('').match('test', 4) != nil"));
    BOOST_CHECK_EQUAL("true", eval("Regexp.new('').match('test', -4) != nil"));
    BOOST_CHECK_EQUAL("false", eval("Regexp.new('').match('test', -5) != nil"));
    BOOST_CHECK_EQUAL("false", eval("Regexp.new('').match('test', 5) != nil"));
}

BOOST_AUTO_TEST_CASE(eq)
{
    BOOST_CHECK_EQUAL("true", eval("Regexp.new('test') == Regexp.new('test')"));
    BOOST_CHECK_EQUAL("false", eval("Regexp.new('test') == Regexp.new('testx')"));
    BOOST_CHECK_EQUAL("false", eval("Regexp.new('test') == Regexp.new('te(st)')"));
    BOOST_CHECK_EQUAL("false", eval("Regexp.new('test', Regexp::IGNORECASE) == Regexp.new('test')"));

    BOOST_CHECK_EQUAL("true", eval("Regexp.new('test').hash == Regexp.new('test').hash"));
    BOOST_CHECK_EQUAL("false", eval("Regexp.new('test').hash == Regexp.new('testx').hash"));
}

BOOST_AUTO_TEST_CASE(misc)
{
    BOOST_CHECK_EQUAL("false", eval("Regexp.new('test').casefold?"));
    BOOST_CHECK_EQUAL("true", eval("Regexp.new('test', Regexp::IGNORECASE).casefold?"));

    BOOST_CHECK_EQUAL("0", eval("Regexp.new('test').options"));
    BOOST_CHECK_EQUAL("1", eval("Regexp.new('test', Regexp::IGNORECASE).options"));

    BOOST_CHECK_EQUAL("\"test\"", eval("Regexp.new('test').source"));
    BOOST_CHECK_EQUAL("\"$test\"", eval("Regexp.new('$test').source"));

}


BOOST_AUTO_TEST_CASE(data_eq)
{
    BOOST_CHECK_EQUAL("true", eval("Regexp.new('test').match('hello test') == Regexp.new('test').match('hello test')"));
    BOOST_CHECK_EQUAL("true", eval("Regexp.new('test').match('hello test').hash == Regexp.new('test').match('hello test').hash"));

    BOOST_CHECK_EQUAL("false", eval("Regexp.new('test').match('hello test') == Regexp.new('test').match('test')"));
    BOOST_CHECK_EQUAL("false", eval("Regexp.new('test').match('hello test').hash == Regexp.new('test').match('test').hash"));
}

BOOST_AUTO_TEST_CASE(data_el_ref)
{
    BOOST_CHECK_EQUAL("\"test\"", eval("Regexp.new('test').match('hello test')[0]"));
    BOOST_CHECK_EQUAL("\"test\"", eval("Regexp.new('test').match('hello test')[-1]"));

    BOOST_CHECK_EQUAL("nil", eval("Regexp.new('test').match('hello test')[2]"));
    BOOST_CHECK_EQUAL("nil", eval("Regexp.new('test').match('hello test')[-2]"));

    BOOST_CHECK_EQUAL("\"test\"", eval("Regexp.new('t(es)t').match('hello test')[0]"));
    BOOST_CHECK_EQUAL("\"es\"", eval("Regexp.new('t(es)t').match('hello test')[1]"));

    BOOST_CHECK_EQUAL("[\"test\", \"es\"]", eval("Regexp.new('t(es)t').match('hello test')[0, 2]"));
    BOOST_CHECK_EQUAL("[\"test\", \"es\"]", eval("Regexp.new('t(es)t').match('hello test')[0, 3]"));
    BOOST_CHECK_EQUAL("[\"test\", \"es\", nil]", eval("Regexp.new('t(es)t(ing)?').match('hello test')[0, 3]"));
}

BOOST_AUTO_TEST_CASE(data_captures)
{
    BOOST_CHECK_EQUAL("[]", eval("Regexp.new('test').match('hello test').captures"));
    BOOST_CHECK_EQUAL("[\"e\", \"st\"]", eval("Regexp.new('t(e)(st)').match('hello test').captures"));
    BOOST_CHECK_EQUAL("[\"e\", \"st\", nil]", eval("Regexp.new('t(e)(st)(ing)?').match('hello test').captures"));
}

BOOST_AUTO_TEST_CASE(data_begin)
{
    BOOST_CHECK_EQUAL("0", eval("Regexp.new('test').match('test').begin 0"));
    BOOST_CHECK_EQUAL("6", eval("Regexp.new('test').match('hello test').begin 0"));

    BOOST_CHECK_EQUAL("6", eval("Regexp.new('t(e)(st)').match('hello test').begin 0"));
    BOOST_CHECK_EQUAL("7", eval("Regexp.new('t(e)(st)').match('hello test').begin 1"));
    BOOST_CHECK_EQUAL("8", eval("Regexp.new('t(e)(st)').match('hello test').begin 2"));
    BOOST_CHECK_EQUAL("nil", eval("Regexp.new('t(e)(st)(ing)?').match('hello test').begin 3"));

    BOOST_CHECK_THROW(eval("Regexp.new('test').match('test').begin -1"), IndexError);
    BOOST_CHECK_THROW(eval("Regexp.new('test').match('test').begin 1"), IndexError);
}

BOOST_AUTO_TEST_CASE(data_end)
{
    BOOST_CHECK_EQUAL("4", eval("Regexp.new('test').match('test').end 0"));
    BOOST_CHECK_EQUAL("10", eval("Regexp.new('test').match('hello test').end 0"));

    BOOST_CHECK_EQUAL("10", eval("Regexp.new('t(e)(st)').match('hello test').end 0"));
    BOOST_CHECK_EQUAL("8", eval("Regexp.new('t(e)(st)').match('hello test').end 1"));
    BOOST_CHECK_EQUAL("10", eval("Regexp.new('t(e)(st)').match('hello test').end 2"));
    BOOST_CHECK_EQUAL("nil", eval("Regexp.new('t(e)(st)(ing)?').match('hello test').end 3"));

    BOOST_CHECK_THROW(eval("Regexp.new('test').match('test').end -1"), IndexError);
    BOOST_CHECK_THROW(eval("Regexp.new('test').match('test').end 1"), IndexError);
}

BOOST_AUTO_TEST_CASE(data_offset)
{
    BOOST_CHECK_EQUAL("[0, 4]", eval("Regexp.new('test').match('test').offset 0"));
    BOOST_CHECK_EQUAL("[6, 10]", eval("Regexp.new('test').match('hello test').offset 0"));

    BOOST_CHECK_EQUAL("[6, 10]", eval("Regexp.new('t(e)(st)').match('hello test').offset 0"));
    BOOST_CHECK_EQUAL("[7, 8]", eval("Regexp.new('t(e)(st)').match('hello test').offset 1"));
    BOOST_CHECK_EQUAL("[8, 10]", eval("Regexp.new('t(e)(st)').match('hello test').offset 2"));
    BOOST_CHECK_EQUAL("[nil, nil]", eval("Regexp.new('t(e)(st)(ing)?').match('hello test').offset 3"));

    BOOST_CHECK_THROW(eval("Regexp.new('test').match('test').offset -1"), IndexError);
    BOOST_CHECK_THROW(eval("Regexp.new('test').match('test').offset 1"), IndexError);
}

BOOST_AUTO_TEST_CASE(data_post_match)
{
    BOOST_CHECK_EQUAL("\"\"", eval("Regexp.new('test').match('test').post_match"));
    BOOST_CHECK_EQUAL("\"ing\"", eval("Regexp.new('test').match('testing').post_match"));
}

BOOST_AUTO_TEST_CASE(data_pre_match)
{
    BOOST_CHECK_EQUAL("\"\"", eval("Regexp.new('test').match('test').pre_match"));
    BOOST_CHECK_EQUAL("\"hello \"", eval("Regexp.new('test').match('hello test').pre_match"));
}

//data_regexp
BOOST_AUTO_TEST_CASE(data_size)
{
    BOOST_CHECK_EQUAL("1", eval("Regexp.new('test').match('test').size"));
    BOOST_CHECK_EQUAL("2", eval("Regexp.new('te(st)').match('test').size"));
    BOOST_CHECK_EQUAL("3", eval("Regexp.new('te(st)(ing)').match('testing').size"));
}

BOOST_AUTO_TEST_CASE(data_string)
{
    BOOST_CHECK_EQUAL("\"test\"", eval("Regexp.new('test').match('test').string"));
    BOOST_CHECK_EQUAL("\"testing\"", eval("Regexp.new('test').match('testing').string"));
}

BOOST_AUTO_TEST_CASE(data_to_a)
{
    BOOST_CHECK_EQUAL("[\"test\"]", eval("Regexp.new('test').match('test').to_a"));
    BOOST_CHECK_EQUAL("[\"test\", \"es\", nil]", eval("Regexp.new('t(es)t(ing)?').match('test').to_a"));
}

BOOST_AUTO_TEST_CASE(values_at)
{
    BOOST_CHECK_EQUAL("[]", eval("Regexp.new('test').match('test').values_at"));
    BOOST_CHECK_EQUAL("[\"test\", nil]", eval("Regexp.new('t(es)t(ing)?').match('test').values_at 0, 2"));
    BOOST_CHECK_EQUAL("[\"test\", nil]", eval("Regexp.new('t(es)t(ing)?').match('test').values_at 0, 6"));
}


BOOST_AUTO_TEST_SUITE_END()
