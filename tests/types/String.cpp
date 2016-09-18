#include <boost/test/unit_test.hpp>
#include "expression/Parser.hpp"
#include "expression/Ast.hpp"
#include "expression/Lexer.hpp"
#include "expression/Scope.hpp"
#include "types/Array.hpp"
#include "types/Number.hpp"
#include "types/String.hpp"
#include "Error.hpp"

using namespace slim;
using namespace slim::expr;
BOOST_AUTO_TEST_SUITE(TestString)

std::string eval(Scope &scope, const std::string &str)
{
    Lexer lexer(str);
    expr::LocalVarNames vars;
    Parser parser(vars, lexer);
    auto expr = parser.full_expression();
    auto result = expr->eval(scope);
    return result->inspect();
}
std::string eval(const std::string &str)
{
    Scope scope(create_view_model());
    return eval(scope, str);
}

BOOST_AUTO_TEST_CASE(ascii_only)
{
    BOOST_CHECK_EQUAL("true", eval("'abcd'.ascii_only?"));
    BOOST_CHECK_EQUAL("false", eval("'abcd£'.ascii_only?"));
}

BOOST_AUTO_TEST_CASE(inspect_escape)
{
    std::string escaped = "\"\\\\ \\' \\\" \\r \\n \\t\"";
    BOOST_CHECK_EQUAL(escaped, eval(escaped));
}

BOOST_AUTO_TEST_CASE(cmp)
{
    BOOST_CHECK_EQUAL("true", eval("'abcd' == 'abcd'"));
    BOOST_CHECK_EQUAL("false", eval("'abcd' == 'abcD'"));
    BOOST_CHECK_EQUAL("true", eval("'5' == '5'"));
    BOOST_CHECK_EQUAL("false", eval("'5' == 5"));

    BOOST_CHECK_EQUAL("0", eval("'abcd' <=> 'abcd'"));
    BOOST_CHECK_EQUAL("-1", eval("'aacd' <=> 'abcd'"));
    BOOST_CHECK_EQUAL("-1", eval("'abcd' <=> 'abcde'"));
    BOOST_CHECK_EQUAL("-1", eval("'Abcd' <=> 'abcd'"));
    BOOST_CHECK_EQUAL("1", eval("'abcde' <=> 'abcd'"));
    BOOST_CHECK_EQUAL("1", eval("'abcd' <=> 'aacd'"));
    BOOST_CHECK_EQUAL("1", eval("'abcd' <=> 'Abcd'"));


    BOOST_CHECK_EQUAL("0", eval("'abcd'.casecmp 'abcd'"));
    BOOST_CHECK_EQUAL("0", eval("'Abcd'.casecmp 'abcd'"));
    BOOST_CHECK_EQUAL("0", eval("'abcd'.casecmp 'Abcd'"));
    BOOST_CHECK_EQUAL("-1", eval("'aacd'.casecmp 'Abcd'"));
    BOOST_CHECK_EQUAL("1", eval("'accd'.casecmp 'Abcd'"));
    BOOST_CHECK_EQUAL("-1", eval("'Abcd'.casecmp 'abcda'"));
    BOOST_CHECK_EQUAL("1", eval("'abcd'.casecmp 'Abc'"));


    BOOST_CHECK_EQUAL("true", eval("'abcd test'.start_with? 'abcd'"));
    BOOST_CHECK_EQUAL("false", eval("'abcd test'.start_with? 'test'"));
    BOOST_CHECK_EQUAL("false", eval("'abcd test'.start_with? 'cd', 'st'"));
    BOOST_CHECK_EQUAL("true", eval("'abcd test'.start_with? 'cd', 'ab'"));

    BOOST_CHECK_EQUAL("false", eval("'abcd test'.end_with? 'abcd'"));
    BOOST_CHECK_EQUAL("true", eval("'abcd test'.end_with? 'test'"));
    BOOST_CHECK_EQUAL("true", eval("'abcd test'.end_with? 'cd', 'st'"));
    BOOST_CHECK_EQUAL("false", eval("'abcd test'.end_with? 'cd', 'ab'"));

    BOOST_CHECK_EQUAL("true", eval("'abcd test'.include? 'abcd'"));
    BOOST_CHECK_EQUAL("true", eval("'abcd test'.include? 'test'"));
    BOOST_CHECK_EQUAL("false", eval("'abcd test'.include? 'tex'"));
}

BOOST_AUTO_TEST_CASE(convert)
{
    BOOST_CHECK_EQUAL("\"Hello woRLD\"", eval("'hello woRLD'.capitalize"));
    BOOST_CHECK_EQUAL("\"Hello woRLD\"", eval("'Hello woRLD'.capitalize"));
    BOOST_CHECK_EQUAL("\"hello world\"", eval("'hello woRLD'.downcase"));
    BOOST_CHECK_EQUAL("\"HELLO WORLD\"", eval("'hello woRLD'.upcase"));

    BOOST_CHECK_EQUAL("0", eval("'test'.hex"));
    BOOST_CHECK_EQUAL("0", eval("'0'.hex"));
    BOOST_CHECK_EQUAL("0", eval("'0x'.hex"));
    BOOST_CHECK_EQUAL("15", eval("'0xf'.hex"));
    BOOST_CHECK_EQUAL("10", eval("'0x0a'.hex"));
    BOOST_CHECK_EQUAL("26", eval("'0x01A bar'.hex"));
    BOOST_CHECK_EQUAL("-36", eval("'-24'.hex"));
}

BOOST_AUTO_TEST_CASE(size)
{
    BOOST_CHECK_EQUAL("0", eval("''.size"));
    BOOST_CHECK_EQUAL("0", eval("''.length"));

    BOOST_CHECK_EQUAL("4", eval("'test'.size"));
    BOOST_CHECK_EQUAL("4", eval("'test'.length"));

    BOOST_CHECK_EQUAL("true", eval("''.empty?"));
    BOOST_CHECK_EQUAL("false", eval("'test'.empty?"));
}

BOOST_AUTO_TEST_CASE(access)
{
    BOOST_CHECK_EQUAL("2", eval("'hello world'.index 'l'"));
    BOOST_CHECK_EQUAL("nil", eval("'hello world'.index 'x'"));
    BOOST_CHECK_EQUAL("nil", eval("'hello world'.index 'llx'"));
    BOOST_CHECK_EQUAL("2", eval("'hello world'.index 'l', 2"));
    BOOST_CHECK_EQUAL("2", eval("'hello world'.index 'll', 2"));
    BOOST_CHECK_EQUAL("3", eval("'hello world'.index 'l', 3"));
    BOOST_CHECK_EQUAL("nil", eval("'hello world'.index 'll', 3"));
    BOOST_CHECK_EQUAL("9", eval("'hello world'.index 'l', 4"));
    BOOST_CHECK_EQUAL("nil", eval("'hello world'.index 'l', 10"));
    
    BOOST_CHECK_EQUAL("9", eval("'hello world'.rindex 'l'"));
    BOOST_CHECK_EQUAL("nil", eval("'hello world'.rindex 'x'"));
    BOOST_CHECK_EQUAL("nil", eval("'hello world'.rindex 'llx'"));
    BOOST_CHECK_EQUAL("nil", eval("'hello world'.rindex 'l', 1"));
    BOOST_CHECK_EQUAL("2", eval("'hello world'.rindex 'l', 2"));
    BOOST_CHECK_EQUAL("2", eval("'hello world'.rindex 'll', 2"));
    BOOST_CHECK_EQUAL("3", eval("'hello world'.rindex 'l', 3"));
    BOOST_CHECK_EQUAL("2", eval("'hello world'.rindex 'll', 3"));
    BOOST_CHECK_EQUAL("9", eval("'hello world'.rindex 'l', 9"));
    BOOST_CHECK_EQUAL("9", eval("'hello world'.rindex 'l', 10"));


    BOOST_CHECK_EQUAL("[\"hello\", \" \", \"world\"]", eval("'hello world'.partition ' '"));
    BOOST_CHECK_EQUAL("[\"hello world\", \"\", \"\"]", eval("'hello world'.partition 'x'"));

    BOOST_CHECK_EQUAL("[\"hello\", \" \", \"world test\"]", eval("'hello world test'.partition ' '"));
    BOOST_CHECK_EQUAL("[\"hello world\", \" \", \"test\"]", eval("'hello world test'.rpartition ' '"));
    BOOST_CHECK_EQUAL("[\"\", \"\", \"hello world\"]", eval("'hello world'.rpartition 'x'"));

}
BOOST_AUTO_TEST_CASE(slice)
{
    //slice index
    BOOST_CHECK_EQUAL("\"s\"", eval("'test'.slice 2"));
    BOOST_CHECK_EQUAL("\"s\"", eval("'test'[2]"));
    BOOST_CHECK_EQUAL("\"t\"", eval("'test'[-1]"));
    BOOST_CHECK_EQUAL("\"\"", eval("'test'[4]"));
    BOOST_CHECK_EQUAL("nil", eval("'test'[-5]"));
    BOOST_CHECK_EQUAL("nil", eval("'test'[5]"));
    //slice start, length
    BOOST_CHECK_EQUAL("\"st\"", eval("'test'.slice 2, 2"));
    BOOST_CHECK_EQUAL("\"st\"", eval("'test'[2, 2]"));
    BOOST_CHECK_EQUAL("\"t\"", eval("'test'[3, 2]"));
    BOOST_CHECK_EQUAL("\"es\"", eval("'test'[-3, 2]"));
    BOOST_CHECK_EQUAL("\"\"", eval("'test'[4, 2]"));
    BOOST_CHECK_EQUAL("nil", eval("'test'[-5, 2]"));
    BOOST_CHECK_EQUAL("nil", eval("'test'[5, 2]"));
    BOOST_CHECK_EQUAL("\"\"", eval("'test'[2, 0]"));
    BOOST_CHECK_EQUAL("nil", eval("'test'[2, -1]"));
    //slice match_str
    BOOST_CHECK_EQUAL("\"es\"", eval("'test'.slice 'es'"));
    BOOST_CHECK_EQUAL("\"es\"", eval("'test'['es']"));
    BOOST_CHECK_EQUAL("nil", eval("'test'['esx']"));

    BOOST_CHECK_THROW(eval("'test'[true]"), ScriptError);
    BOOST_CHECK_THROW(eval("'test'[5, true]"), ScriptError);
    BOOST_CHECK_THROW(eval("'test'[true, 5]"), ScriptError);
    BOOST_CHECK_THROW(eval("'test'[1, 2, 3]"), ScriptError);
}
BOOST_AUTO_TEST_CASE(split)
{
    BOOST_CHECK_EQUAL("[\"unit\", \"test\", \"string\", \"split\"]", eval("'unit  test\\nstring\\tsplit'.split"));
    BOOST_CHECK_EQUAL("[\"unit\", \"test\", \"string\", \"split\"]", eval("'unit  test\\nstring\\tsplit'.split ' '"));
    BOOST_CHECK_EQUAL("[\"\", \"es\"]", eval("'test'.split 't'"));
    BOOST_CHECK_EQUAL("[\"1\", \"2\", \"3\", \"5\"]", eval("'1,2,3,,5,'.split ','"));
    BOOST_CHECK_EQUAL("[\"\", \"es\", \"\"]", eval("'test'.split 't', -1"));
    BOOST_CHECK_EQUAL("[\"test\"]", eval("'test'.split 't', 1"));
    BOOST_CHECK_EQUAL("[\"test\"]", eval("'test'.split '', 1"));
    BOOST_CHECK_EQUAL("[\"t\", \"e\", \"s\", \"t\"]", eval("'test'.split ''"));
    BOOST_CHECK_EQUAL("[\"1\", \"2\", \"3\", \"\", \"5\", \"\"]", eval("'1,2,3,,5,'.split ',', -1"));
    BOOST_CHECK_EQUAL("[\"1\", \"2\", \"3,,5,\"]", eval("'1,2,3,,5,'.split ',', 3"));
    BOOST_CHECK_EQUAL("[\"1\", \"2\", \"3\", \"\", \"5,\"]", eval("'1,2,3,,5,'.split ',', 5"));
    BOOST_CHECK_EQUAL("[\"1\", \"2\", \"3\", \"\", \"5\", \"\"]", eval("'1,2,3,,5,'.split ',', 10"));
}

BOOST_AUTO_TEST_CASE(justify)
{
    BOOST_CHECK_EQUAL("\"test\"", eval("'test'.center 0"));
    BOOST_CHECK_EQUAL("\"test\"", eval("'test'.center 4"));
    BOOST_CHECK_EQUAL("\"test \"", eval("'test'.center 5"));
    BOOST_CHECK_EQUAL("\" test \"", eval("'test'.center 6"));
    BOOST_CHECK_EQUAL("\"   test   \"", eval("'test'.center 10"));
    BOOST_CHECK_EQUAL("\"---test---\"", eval("'test'.center 10, '-'"));
    BOOST_CHECK_EQUAL("\"-=-test-=-\"", eval("'test'.center 10, '-='"));


    BOOST_CHECK_EQUAL("\"test\"", eval("'test'.ljust 4"));
    BOOST_CHECK_EQUAL("\"test \"", eval("'test'.ljust 5"));
    BOOST_CHECK_EQUAL("\"test-=-\"", eval("'test'.ljust 7, '-='"));

    BOOST_CHECK_EQUAL("\"test\"", eval("'test'.rjust 4"));
    BOOST_CHECK_EQUAL("\" test\"", eval("'test'.rjust 5"));
    BOOST_CHECK_EQUAL("\"-=-test\"", eval("'test'.rjust 7, '-='"));
}

BOOST_AUTO_TEST_CASE(chomp)
{
    BOOST_CHECK_EQUAL("\"test\"", eval("'test'.chomp"));
    BOOST_CHECK_EQUAL("\"test\\nx\"", eval("'test\nx'.chomp"));
    
    BOOST_CHECK_EQUAL("\"test\"", eval("'test\\r'.chomp"));
    BOOST_CHECK_EQUAL("\"test\"", eval("'test\\n'.chomp"));
    BOOST_CHECK_EQUAL("\"test\"", eval("'test\\r\\n'.chomp"));
    BOOST_CHECK_EQUAL("\"test\\n\"", eval("'test\\n\\r'.chomp"));
    BOOST_CHECK_EQUAL("\"test\\n\"", eval("'test\\n\\n'.chomp"));

    BOOST_CHECK_EQUAL("\"test\\r\"", eval("'test\\r'.chomp '\\n'"));
    BOOST_CHECK_EQUAL("\"test\"", eval("'test\\n'.chomp '\\n'"));
    BOOST_CHECK_EQUAL("\"test\\n\"", eval("'test\\n\\n'.chomp '\\n'"));

    BOOST_CHECK_EQUAL("\"test\"", eval("'test\\n\\n'.chomp ''"));
    BOOST_CHECK_EQUAL("\"test\"", eval("'test\\n\\n\\r'.chomp ''"));

    BOOST_CHECK_THROW(eval("''.chomp 1, 2"), ArgumentError);
}

BOOST_AUTO_TEST_CASE(strip)
{
    BOOST_CHECK_EQUAL("\"test\"", eval("'test'.strip"));
    BOOST_CHECK_EQUAL("\"test\"", eval("'test'.lstrip"));
    BOOST_CHECK_EQUAL("\"test\"", eval("'test'.rstrip"));

    BOOST_CHECK_EQUAL("\"\"", eval("' '.strip"));
    BOOST_CHECK_EQUAL("\"\"", eval("'  '.lstrip"));
    BOOST_CHECK_EQUAL("\"\"", eval("' \r'.rstrip"));

    BOOST_CHECK_EQUAL("\"test\"", eval("'\\r\\n\\t \ntest\\r\\n\\t \\n'.strip"));
    BOOST_CHECK_EQUAL("\"test\\r\\n\\t \\n\"", eval("'\\r\\n\\t \\ntest\\r\\n\\t \\n'.lstrip"));
    BOOST_CHECK_EQUAL("\"\\r\\n\\t \\ntest\"", eval("'\\r\\n\\t \\ntest\\r\\n\\t \\n'.rstrip"));
}

BOOST_AUTO_TEST_CASE(ord)
{
    BOOST_CHECK_THROW(eval("''.ord"), ScriptError);
    BOOST_CHECK_EQUAL("97", eval("'a'.ord"));
    BOOST_CHECK_EQUAL("97", eval("'ax'.ord"));
}

BOOST_AUTO_TEST_CASE(reverse)
{
    BOOST_CHECK_EQUAL("\"dlrow olleh\"", eval("'hello world'.reverse"));
}

BOOST_AUTO_TEST_CASE(lines)
{
    BOOST_CHECK_EQUAL("[\"hello world\"]", eval("'hello world'.lines"));
    BOOST_CHECK_EQUAL("[\"hello\\n\", \"world\"]", eval("'hello\nworld'.lines"));

    BOOST_CHECK_EQUAL("[\"hello\\n\", \"world\\n\", \"\\n\", \"lines\"]", eval("'hello\\nworld\\n\\nlines'.lines"));
    BOOST_CHECK_EQUAL("[\"hello\\n\", \"world\\n\", \"\\n\", \"lines\\n\"]", eval("'hello\\nworld\\n\\nlines\\n'.lines"));

    BOOST_CHECK_EQUAL("[\"hello \", \"world\\n\\nlines\\n\"]", eval("'hello world\\n\\nlines\\n'.lines ' '"));

    BOOST_CHECK_EQUAL("[\"hello\\nworld\\n\\n\", \"lines\\n\"]", eval("'hello\\nworld\\n\\nlines\\n'.lines ''"));

    BOOST_CHECK_EQUAL("[\"hello\\nworld\\n\\n\\n\", \"lines\\n\"]", eval("'hello\\nworld\\n\\n\\nlines\\n'.lines ''"));
}

BOOST_AUTO_TEST_CASE(each_line)
{
    class Test : public ViewModel
    {
    public:
        std::string name = "Test";
        std::vector<std::string> lines;

        virtual const std::string& type_name()const override { return name; }
        virtual const MethodTable &method_table()const override
        {
            static const MethodTable table(Object::method_table(),
            { { &Test::test, "test" } });
            return table;
        }
        void test(String *line)
        {
            lines.push_back(line->get_value());
        }
    };
    auto test = create_object<Test>();

    Scope scope(test);

    BOOST_CHECK_EQUAL("\"test\"", eval(scope, "'test'.each_line{|x| test x}"));
    BOOST_REQUIRE_EQUAL(1, test->lines.size());
    BOOST_CHECK_EQUAL("test", test->lines[0]);
    test->lines.clear();

    BOOST_CHECK_EQUAL("\"test\\nline\"", eval(scope, "'test\\nline'.each_line{|x| test x}"));
    BOOST_REQUIRE_EQUAL(2, test->lines.size());
    BOOST_CHECK_EQUAL("test\n", test->lines[0]);
    BOOST_CHECK_EQUAL("line", test->lines[1]);
    test->lines.clear();


    BOOST_CHECK_EQUAL("\"test\\nline\"", eval(scope, "'test\\nline'.each_line ',' {|x| test x}"));
    BOOST_REQUIRE_EQUAL(1, test->lines.size());
    BOOST_CHECK_EQUAL("test\nline", test->lines[0]);
    test->lines.clear();


    BOOST_CHECK_EQUAL("\"test\\nline\"", eval(scope, "'test\\nline'.each_line '' {|x| test x}"));
    BOOST_REQUIRE_EQUAL(1, test->lines.size());
    BOOST_CHECK_EQUAL("test\nline", test->lines[0]);
    test->lines.clear();


    BOOST_CHECK_EQUAL("\"test\\n\\n\\nline\"", eval(scope, "'test\\n\\n\\nline'.each_line '' {|x| test x}"));
    BOOST_REQUIRE_EQUAL(2, test->lines.size());
    BOOST_CHECK_EQUAL("test\n\n\n", test->lines[0]);
    BOOST_CHECK_EQUAL("line", test->lines[1]);
    test->lines.clear();


    BOOST_CHECK_EQUAL("\"test,line\"", eval(scope, "'test,line'.each_line ',' {|x| test x}"));
    BOOST_REQUIRE_EQUAL(2, test->lines.size());
    BOOST_CHECK_EQUAL("test,", test->lines[0]);
    BOOST_CHECK_EQUAL("line", test->lines[1]);
    test->lines.clear();
}

BOOST_AUTO_TEST_SUITE_END()

