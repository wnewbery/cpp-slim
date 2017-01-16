#include <boost/test/unit_test.hpp>
#include "expression/Parser.hpp"
#include "expression/Ast.hpp"
#include "expression/Lexer.hpp"
#include "expression/Scope.hpp"
#include "types/Time.hpp"

namespace
{
    boost::test_tools::predicate_result time_eq(time_t lhs, time_t rhs)
    {
        boost::test_tools::predicate_result result = true;
        if (lhs < rhs - 1 || lhs > rhs + 1)
        {
            result = false;
            result.message() << "within a 1 second delta has failed" << " [" << lhs << "!=" << rhs << "]";
        }
        return result;
    }
}
#define CHECK_TIME_EQUAL(lhs, rhs) BOOST_TEST(time_eq(lhs, rhs))

using namespace slim;
using namespace slim::expr;
BOOST_AUTO_TEST_SUITE(TestTime)


Ptr<Object> eval_o(const std::string &str)
{
    Lexer lexer(str);
    expr::LocalVarNames vars;
    Parser parser(vars, lexer);
    auto expr = parser.full_expression();
    auto model = create_view_model();
    model->add_constant("Time", create_object<TimeType>());
    Scope scope(model);
    return expr->eval(scope);
}
std::string eval(const std::string &str)
{
    return eval_o(str)->inspect();
}
time_t eval_time(const std::string &str)
{
    return coerce<Time>(eval_o(str))->get_value();
}
std::string time_str()
{
    return std::to_string(time(nullptr));
}

BOOST_AUTO_TEST_CASE(creation)
{
    CHECK_TIME_EQUAL(time(nullptr), eval_time("Time.now"));
    CHECK_TIME_EQUAL(time(nullptr), eval_time("Time.new"));
    CHECK_TIME_EQUAL(100, eval_time("Time.at(100)"));
}
BOOST_AUTO_TEST_CASE(create_utc)
{
    CHECK_TIME_EQUAL(1484484256, eval_time("Time.utc(2017, 'jan', 15, 12, 44, 16)"));

    CHECK_TIME_EQUAL(1451606400, eval_time("Time.utc(2016, 'jan', 1, 0, 0, 0)"));
    CHECK_TIME_EQUAL(1454284800, eval_time("Time.utc(2016, 'feb', 1, 0, 0, 0)"));
    CHECK_TIME_EQUAL(1456790400, eval_time("Time.utc(2016, 'mar', 1, 0, 0, 0)"));
    CHECK_TIME_EQUAL(1459468800, eval_time("Time.utc(2016, 'apr', 1, 0, 0, 0)"));
    CHECK_TIME_EQUAL(1462060800, eval_time("Time.utc(2016, 'may', 1, 0, 0, 0)"));
    CHECK_TIME_EQUAL(1464739200, eval_time("Time.utc(2016, 'jun', 1, 0, 0, 0)"));
    CHECK_TIME_EQUAL(1467331200, eval_time("Time.utc(2016, 'jul', 1, 0, 0, 0)"));
    CHECK_TIME_EQUAL(1470009600, eval_time("Time.utc(2016, 'aug', 1, 0, 0, 0)"));
    CHECK_TIME_EQUAL(1472688000, eval_time("Time.utc(2016, 'sep', 1, 0, 0, 0)"));
    CHECK_TIME_EQUAL(1475280000, eval_time("Time.utc(2016, 'oct', 1, 0, 0, 0)"));
    CHECK_TIME_EQUAL(1477958400, eval_time("Time.utc(2016, 'nov', 1, 0, 0, 0)"));
    CHECK_TIME_EQUAL(1480550400, eval_time("Time.utc(2016, 'dec', 1, 0, 0, 0)"));

    CHECK_TIME_EQUAL(1480550400, eval_time("Time.gm(2016, 'dec', 1, 0, 0, 0)"));


    CHECK_TIME_EQUAL(1451606400, eval_time("Time.utc(2016, 1, 1, 0, 0, 0)"));
    CHECK_TIME_EQUAL(1451606400, eval_time("Time.utc(2016)"));
    CHECK_TIME_EQUAL(1451606400, eval_time("Time.utc(2016, nil, nil, nil, nil, nil)"));
    CHECK_TIME_EQUAL(1462060800, eval_time("Time.utc(2016, 5, 1, 0, 0, 0)"));

    BOOST_CHECK_THROW(eval("Time.utc(2016, 'unknown')"), ArgumentError);
    BOOST_CHECK_THROW(eval("Time.utc(1899)"), ArgumentError);
    BOOST_CHECK_THROW(eval("Time.utc(3001)"), ArgumentError);
    BOOST_CHECK_THROW(eval("Time.utc(2016, 0)"), ArgumentError);
    BOOST_CHECK_THROW(eval("Time.utc(2016, 13)"), ArgumentError);
    BOOST_CHECK_THROW(eval("Time.utc(2016, 'jan', 0)"), ArgumentError);
    BOOST_CHECK_THROW(eval("Time.utc(2016, 'jan', 32)"), ArgumentError);
    BOOST_CHECK_THROW(eval("Time.utc(2016, 'jan', 1, -1)"), ArgumentError);
    BOOST_CHECK_THROW(eval("Time.utc(2016, 'jan', 1, 25)"), ArgumentError);
    BOOST_CHECK_THROW(eval("Time.utc(2016, 'jan', 1, 1, -1)"), ArgumentError);
    BOOST_CHECK_THROW(eval("Time.utc(2016, 'jan', 1, 1, 61)"), ArgumentError);
    BOOST_CHECK_THROW(eval("Time.utc(2016, 'jan', 1, 1, 1, -1)"), ArgumentError);
    BOOST_CHECK_THROW(eval("Time.utc(2016, 'jan', 1, 1, 1, 62)"), ArgumentError);
}

BOOST_AUTO_TEST_CASE(create_new_with_offset)
{
    // Year, month, day, hour, minute, second is same as Time.utc

    CHECK_TIME_EQUAL(1451606400, eval_time("Time.new(2016, 'jan', 1, 0, 0, 0)"));
    CHECK_TIME_EQUAL(1451606400, eval_time("Time.new(2016, 'jan', 1, 0, 0, 0, '+00:00')"));
    CHECK_TIME_EQUAL(1451606400, eval_time("Time.new(2016, 'jan', 1, 0, 0, 0, '-00:00')"));
    CHECK_TIME_EQUAL(1451606400 - 60*60*4, eval_time("Time.new(2016, 'jan', 1, 0, 0, 0, '+04:00')"));
    CHECK_TIME_EQUAL(1451606400 + 60*60*4, eval_time("Time.new(2016, 'jan', 1, 0, 0, 0, '-04:00')"));

    BOOST_CHECK_THROW(eval("Time.new(2016, 'jan', 1, 0, 0, 0, '04:00')"), ArgumentError);
    BOOST_CHECK_THROW(eval("Time.new(2016, 'jan', 1, 0, 0, 0, '')"), ArgumentError);
    BOOST_CHECK_THROW(eval("Time.new(2016, 'jan', 1, 0, 0, 0, '+XX:00')"), ArgumentError);
    BOOST_CHECK_THROW(eval("Time.new(2016, 'jan', 1, 0, 0, 0, '+00-00')"), ArgumentError);
    BOOST_CHECK_THROW(eval("Time.new(2016, 'jan', 1, 0, 0, 0, '+00')"), ArgumentError);
}

BOOST_AUTO_TEST_CASE(cmp)
{
    BOOST_CHECK_EQUAL("true", eval("Time.at(1000) == Time.at(1000)"));
    BOOST_CHECK_EQUAL("false", eval("Time.at(1000) == Time.at(1200)"));

    BOOST_CHECK_EQUAL("0", eval("Time.at(1000) <=> Time.at(1000)"));
    BOOST_CHECK_EQUAL("-1", eval("Time.at(900) <=> Time.at(1000)"));
    BOOST_CHECK_EQUAL("1", eval("Time.at(1000) <=> Time.at(900)"));
}

BOOST_AUTO_TEST_CASE(addition)
{
    BOOST_CHECK_EQUAL(1500, eval_time("Time.at(1000) + 500"));
    BOOST_CHECK_EQUAL(500, eval_time("Time.at(1000) - 500"));
}

BOOST_AUTO_TEST_CASE(no_timezones)
{
    BOOST_CHECK_EQUAL("false", eval("Time.now.dst?"));
    BOOST_CHECK_EQUAL("false", eval("Time.now.isdst"));
    BOOST_CHECK_EQUAL("true", eval("Time.now.gmt?"));
    BOOST_CHECK_EQUAL("true", eval("Time.now.utc?"));
    BOOST_CHECK_EQUAL("0", eval("Time.now.gmt_offset"));
    BOOST_CHECK_EQUAL("0", eval("Time.now.utc_offset"));
    BOOST_CHECK_EQUAL("0", eval("Time.now.gmtoff"));
    BOOST_CHECK_EQUAL("\"UTC\"", eval("Time.now.zone"));
}

BOOST_AUTO_TEST_CASE(accessors)
{
    BOOST_CHECK_EQUAL("2017", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).year"));
    BOOST_CHECK_EQUAL("1", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).mon"));
    BOOST_CHECK_EQUAL("1", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).month"));
    BOOST_CHECK_EQUAL("15", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).day"));
    BOOST_CHECK_EQUAL("15", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).mday"));
    BOOST_CHECK_EQUAL("15", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).yday"));
    BOOST_CHECK_EQUAL("0", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).wday"));
    BOOST_CHECK_EQUAL("12", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).hour"));
    BOOST_CHECK_EQUAL("44", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).min"));
    BOOST_CHECK_EQUAL("16", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).sec"));
    BOOST_CHECK_EQUAL("16", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).tv_sec"));
    BOOST_CHECK_EQUAL("0", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).nsec"));
    BOOST_CHECK_EQUAL("0", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).tv_nsec"));
    BOOST_CHECK_EQUAL("0", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).usec"));
    BOOST_CHECK_EQUAL("0", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).tv_usec"));
    BOOST_CHECK_EQUAL("0", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).subsec"));


    BOOST_CHECK_EQUAL("true", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).sunday?"));
    BOOST_CHECK_EQUAL("false", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).monday?"));

    BOOST_CHECK_EQUAL("true", eval("Time.utc(2017, 'jan', 16, 12, 44, 16).monday?"));
    BOOST_CHECK_EQUAL("true", eval("Time.utc(2017, 'jan', 17, 12, 44, 16).tuesday?"));
    BOOST_CHECK_EQUAL("true", eval("Time.utc(2017, 'jan', 18, 12, 44, 16).wednesday?"));
    BOOST_CHECK_EQUAL("true", eval("Time.utc(2017, 'jan', 19, 12, 44, 16).thursday?"));
    BOOST_CHECK_EQUAL("true", eval("Time.utc(2017, 'jan', 20, 12, 44, 16).friday?"));
    BOOST_CHECK_EQUAL("true", eval("Time.utc(2017, 'jan', 21, 12, 44, 16).saturday?"));

    BOOST_CHECK_EQUAL("[16, 44, 12, 15, 1, 2017, 0, 15, false, \"UTC\"]",
        eval("Time.utc(2017, 'jan', 15, 12, 44, 16).to_a"));
}

BOOST_AUTO_TEST_CASE(format)
{
    BOOST_CHECK_EQUAL(
        "\"Sun Jan 15 12:44:16 2017\"",
        eval("Time.utc(2017, 'jan', 15, 12, 44, 16).asctime"));
    BOOST_CHECK_EQUAL(
        "\"Sun Jan 15 12:44:16 2017\"",
        eval("Time.utc(2017, 'jan', 15, 12, 44, 16).ctime"));
    BOOST_CHECK_EQUAL(
        "2017-01-15 12:44:16 +0000",
        eval("Time.utc(2017, 'jan', 15, 12, 44, 16)"));

    // Year
    BOOST_CHECK_EQUAL("\"2017\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%Y'"));
    BOOST_CHECK_EQUAL("\"20\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%C'"));
    BOOST_CHECK_EQUAL("\"17\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%y'"));
    // Month
    BOOST_CHECK_EQUAL("\"01\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%m'"));
    BOOST_CHECK_EQUAL("\"12\"", eval("Time.utc(2017, 'dec', 15, 12, 44, 16).strftime '%m'"));
    BOOST_CHECK_EQUAL("\"January\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%B'"));
    BOOST_CHECK_EQUAL("\"Jan\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%b'"));
    BOOST_CHECK_EQUAL("\"Jan\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%h'"));
    // Day
    BOOST_CHECK_EQUAL("\"01\"", eval("Time.utc(2017, 'jan',  1, 12, 44, 16).strftime '%d'"));
    BOOST_CHECK_EQUAL("\"15\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%d'"));
    BOOST_CHECK_EQUAL("\" 1\"", eval("Time.utc(2017, 'jan',  1, 12, 44, 16).strftime '%e'"));
    BOOST_CHECK_EQUAL("\"015\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%j'"));
    BOOST_CHECK_EQUAL("\"365\"", eval("Time.utc(2017, 'dec', 31, 12, 44, 16).strftime '%j'"));

    BOOST_CHECK_EQUAL("\"Sunday\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%A'"));
    BOOST_CHECK_EQUAL("\"Sun\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%a'"));
    BOOST_CHECK_EQUAL("\"7\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%u'"));
    BOOST_CHECK_EQUAL("\"0\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%w'"));
    // Hour
    BOOST_CHECK_EQUAL("\"12\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%H'"));
    BOOST_CHECK_EQUAL("\"05\"", eval("Time.utc(2017, 'jan', 15,  5, 44, 16).strftime '%H'"));
    BOOST_CHECK_EQUAL("\"12\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%k'"));
    BOOST_CHECK_EQUAL("\" 5\"", eval("Time.utc(2017, 'jan', 15,  5, 44, 16).strftime '%k'"));
    BOOST_CHECK_EQUAL("\"12\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%I'"));
    BOOST_CHECK_EQUAL("\"12\"", eval("Time.utc(2017, 'jan', 15, 24, 44, 16).strftime '%I'"));
    BOOST_CHECK_EQUAL("\"03\"", eval("Time.utc(2017, 'jan', 15, 15, 44, 16).strftime '%I'"));
    BOOST_CHECK_EQUAL("\"12\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%l'"));
    BOOST_CHECK_EQUAL("\"12\"", eval("Time.utc(2017, 'jan', 15,  0, 44, 16).strftime '%l'"));
    BOOST_CHECK_EQUAL("\" 3\"", eval("Time.utc(2017, 'jan', 15, 15, 44, 16).strftime '%l'"));
    BOOST_CHECK_EQUAL("\"pm\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%P'"));
    BOOST_CHECK_EQUAL("\"am\"", eval("Time.utc(2017, 'jan', 15,  0, 44, 16).strftime '%P'"));
    BOOST_CHECK_EQUAL("\"PM\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%p'"));
    BOOST_CHECK_EQUAL("\"AM\"", eval("Time.utc(2017, 'jan', 15,  0, 44, 16).strftime '%p'"));
    // Minute
    BOOST_CHECK_EQUAL("\"44\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%M'"));
    BOOST_CHECK_EQUAL("\"05\"", eval("Time.utc(2017, 'jan', 15, 12,  5, 16).strftime '%M'"));
    // Second
    BOOST_CHECK_EQUAL("\"16\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%S'"));
    BOOST_CHECK_EQUAL("\"04\"", eval("Time.utc(2017, 'jan', 15, 12, 44,  4).strftime '%S'"));
    // Millisecond
    BOOST_CHECK_EQUAL("\"000\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%L'"));
    // Fractional seconds
    BOOST_CHECK_EQUAL("\"000000000\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%N'"));
    // Time zone
    BOOST_CHECK_EQUAL("\"+0000\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%z'"));
    BOOST_CHECK_EQUAL("\"+00:00\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%:z'"));
    BOOST_CHECK_EQUAL("\"UTC\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%Z'"));

    // Literal
    BOOST_CHECK_EQUAL("\"%\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%'"));
    BOOST_CHECK_EQUAL("\"%\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%%'"));
    BOOST_CHECK_EQUAL("\"%Q\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%Q'"));
    BOOST_CHECK_EQUAL("\"\\t\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%t'"));
    BOOST_CHECK_EQUAL("\"\\n\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%n'"));

    // Flags
    BOOST_CHECK_EQUAL("\"6\"", eval("Time.utc(2017, 'jan', 15,  6, 44, 16).strftime '%-H'"));
    BOOST_CHECK_EQUAL("\" 6\"", eval("Time.utc(2017, 'jan', 15,  6, 44, 16).strftime '%_H'"));
    BOOST_CHECK_EQUAL("\"06\"", eval("Time.utc(2017, 'jan', 15,  6, 44, 16).strftime '%0H'"));
    BOOST_CHECK_EQUAL("\"006\"", eval("Time.utc(2017, 'jan', 15,  6, 44, 16).strftime '%3H'"));
    BOOST_CHECK_EQUAL("\"  6\"", eval("Time.utc(2017, 'jan', 15,  6, 44, 16).strftime '%_3H'"));
    BOOST_CHECK_EQUAL("\"06\"", eval("Time.utc(2017, 'jan', 15,  6, 44, 16).strftime '%_0H'"));

    BOOST_CHECK_EQUAL("\"000\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%3N'"));

    BOOST_CHECK_EQUAL("\"SUNDAY\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%^A'"));
    BOOST_CHECK_EQUAL("\"  SUNDAY\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%^8A'"));
    BOOST_CHECK_EQUAL("\"00SUNDAY\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%0^8A'"));
    BOOST_CHECK_EQUAL("\"  Sunday\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%-8A'"));

    BOOST_CHECK_EQUAL("\"06\"", eval("Time.utc(2017, 'jan', 15,  6, 44, 16).strftime '%EH'"));
    BOOST_CHECK_EQUAL("\"06\"", eval("Time.utc(2017, 'jan', 15,  6, 44, 16).strftime '%OH'"));

    // Specials
    BOOST_CHECK_EQUAL("\"Sun Jan 15 12:44:16 2017\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%c'"));
    BOOST_CHECK_EQUAL("\"      Sun Jan 15 12:44:16 2017\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%30c'"));

    BOOST_CHECK_EQUAL("\"01/15/17\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%D'"));
    BOOST_CHECK_EQUAL("\"01/15/17\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%x'"));
    BOOST_CHECK_EQUAL("\"  01/15/17\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%10D'"));

    BOOST_CHECK_EQUAL("\"2017-01-15\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%F'"));
    BOOST_CHECK_EQUAL("\"    2017-01-15\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%14F'"));

    BOOST_CHECK_EQUAL("\"15-JAN-2017\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%v'"));
    BOOST_CHECK_EQUAL("\"   15-JAN-2017\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%14v'"));

    BOOST_CHECK_EQUAL("\"12:44:16 PM\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%r'"));
    BOOST_CHECK_EQUAL("\"03:44:16 PM\"", eval("Time.utc(2017, 'jan', 15, 15, 44, 16).strftime '%r'"));
    BOOST_CHECK_EQUAL("\"   12:44:16 PM\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%14r'"));

    BOOST_CHECK_EQUAL("\"12:44\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%R'"));
    BOOST_CHECK_EQUAL("\"         12:44\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%14R'"));

    BOOST_CHECK_EQUAL("\"12:44:16\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%T'"));
    BOOST_CHECK_EQUAL("\"12:44:16\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%X'"));
    BOOST_CHECK_EQUAL("\"      12:44:16\"", eval("Time.utc(2017, 'jan', 15, 12, 44, 16).strftime '%14T'"));
}

BOOST_AUTO_TEST_SUITE_END()
