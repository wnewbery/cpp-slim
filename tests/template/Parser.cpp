#include <boost/test/unit_test.hpp>
#include "template/Template.hpp"
#include "template/Lexer.hpp"
#include "template/Parser.hpp"
#include "Error.hpp"

using namespace slim;
using namespace slim::tpl;
BOOST_AUTO_TEST_SUITE(TestTplParser)

std::string parse_str(const char *str)
{
    Lexer lexer(str, str + strlen(str));
    lexer.file_name("inline_test.html.slim");
    Parser parser(lexer);
    return parser.parse().to_string();
}

BOOST_AUTO_TEST_CASE(simple_elements)
{
    BOOST_CHECK_EQUAL("", parse_str(""));
    BOOST_CHECK_EQUAL("<p></p>", parse_str("p"));
    BOOST_CHECK_EQUAL("<p><span></span><span></span></p>", parse_str("p\n  span\n  span\n"));
    BOOST_CHECK_EQUAL("<p><span><span></span></span><span></span></p>", parse_str("p\n  span\n    span\n  span\n"));

    //whitespace
    BOOST_CHECK_EQUAL(" <p></p>", parse_str("p<"));
    BOOST_CHECK_EQUAL("<p></p> ", parse_str("p>"));
    BOOST_CHECK_EQUAL(" <p></p> ", parse_str("p<>"));

    //text content
    BOOST_CHECK_EQUAL("<p>Hello World</p>", parse_str("p Hello World"));
    BOOST_CHECK_EQUAL(" <p>Hello World</p>", parse_str("p< Hello World"));
    BOOST_CHECK_EQUAL("<p>Hello World</p> ", parse_str("p> Hello World"));
    BOOST_CHECK_EQUAL(" <p>Hello World</p> ", parse_str("p<> Hello World"));
}

BOOST_AUTO_TEST_CASE(text_lines)
{
    BOOST_CHECK_EQUAL("<p></p>", parse_str("p"));
    BOOST_CHECK_EQUAL("<p></p>", parse_str("p\n  |\n"));
    BOOST_CHECK_EQUAL("<p></p>", parse_str("p\n  |"));
    BOOST_CHECK_EQUAL(
        "<p>Hello World</p>",
        parse_str("p\n  | Hello World"));
    BOOST_CHECK_EQUAL(
        "<p>  Hello World</p>",
        parse_str("p\n  |   Hello World"));
    BOOST_CHECK_EQUAL(
        "<p>Hello World  </p>",
        parse_str("p\n  | Hello World  "));
    BOOST_CHECK_EQUAL(
        "<p>Hello World </p>",
        parse_str("p\n  ' Hello World"));
    BOOST_CHECK_EQUAL(
        "<p>Hello World</p>",
        parse_str("p\n  ' Hello\n  | World"));

    BOOST_CHECK_EQUAL(
        "<p>Hello World</p>",
        parse_str(
            "p\n"
            "  | Hel\n"
            "    lo\n"
            "     World"));

    BOOST_CHECK_EQUAL(
        "<p></p>",
        parse_str("p\n  / Hello\n     World"));
    BOOST_CHECK_EQUAL(
        "<p><!--Hello World--></p>",
        parse_str("p\n  /! Hello\n     World"));
    BOOST_CHECK_EQUAL(
        "<p>Hello <strong>World</strong></p>",
        parse_str("<p>Hello <strong>World</strong></p>"));
}

BOOST_AUTO_TEST_CASE(interpolated_text_lines)
{
    BOOST_CHECK_EQUAL(
        "<p>Hello #{@name}</p>",
        parse_str("p Hello \\#{@name}"));

    BOOST_CHECK_EQUAL(
        "<p>Hello <%= @name %></p>",
        parse_str("p Hello #{@name}"));
    BOOST_CHECK_EQUAL(
        "<p>Hello <%= @name %></p>",
        parse_str("p\n  | Hello #{@name}"));
    BOOST_CHECK_EQUAL(
        "<p>Hello <%= @name %> </p>",
        parse_str("p\n  ' Hello #{@name}"));
    BOOST_CHECK_EQUAL(
        "<p>Hello <%= @name %></p>",
        parse_str("<p>Hello #{@name}</p>"));

    BOOST_CHECK_EQUAL(
        "<p>Hello <%= \"nested #{@name}\" %></p>",
        parse_str("p Hello #{\"nested #{@name}\"}"));
}

BOOST_AUTO_TEST_CASE(code_output_lines)
{
    BOOST_CHECK_EQUAL("<p></p>", parse_str("p"));
    BOOST_CHECK_EQUAL("<p><%= 5 %></p>", parse_str("p\n  =5\n"));
    BOOST_CHECK_EQUAL("<p> <%= 5 %></p>", parse_str("p\n  =<5\n"));
    BOOST_CHECK_EQUAL("<p><%= 5 %> </p>", parse_str("p\n  =>5\n"));
    BOOST_CHECK_EQUAL("<p> <%= 5 %> </p>", parse_str("p\n  =<>5\n"));
    BOOST_CHECK_EQUAL("<p><%= (5 + (2 * @a)) %></p>", parse_str("p\n  =5 + 2 * @a\n"));
    BOOST_CHECK_THROW(parse_str("="), TemplateSyntaxError);
    BOOST_CHECK_THROW(parse_str("=\n"), TemplateSyntaxError);

    //inline with element, same but same line
    BOOST_CHECK_EQUAL("<p><%= 5 %></p>", parse_str("p = 5\n"));
    BOOST_CHECK_EQUAL("<p><%= 5 %> </p>", parse_str("p => 5\n"));
    BOOST_CHECK_THROW(parse_str("p ="), TemplateSyntaxError);
    BOOST_CHECK_THROW(parse_str("p =\n"), TemplateSyntaxError);

    //multi-line
    BOOST_CHECK_EQUAL("<%= (5 + 2) %>", parse_str("= 5\\\n  + 2"));
    BOOST_CHECK_EQUAL("<p><%= (5 + 2) %></p>", parse_str("p= 5\\\n  + 2"));

    BOOST_CHECK_EQUAL("<%= @a.f(5, 2) %>", parse_str("= @a.f 5,\n  2"));
    BOOST_CHECK_EQUAL("<p><%= @a.f(5, 2) %></p>", parse_str("p= @a.f 5,\n  2"));
}

BOOST_AUTO_TEST_CASE(code_lines)
{
    BOOST_CHECK_EQUAL("<p><% x = 5 %></p>", parse_str("p\n  ruby: x = 5\n"));
    BOOST_CHECK_EQUAL("<p><% x = 5 %><%= x %></p>", parse_str("p\n  ruby: x = 5\n  =x"));
}

BOOST_AUTO_TEST_CASE(void_tags)
{
    BOOST_CHECK_EQUAL("<br/>", parse_str("br"));
    BOOST_CHECK_EQUAL("<br/><p></p>", parse_str("br\np"));
    BOOST_CHECK_THROW(parse_str("br\n  p"), TemplateSyntaxError);
}


BOOST_AUTO_TEST_CASE(id_class_shortcut)
{
    BOOST_CHECK_EQUAL("<div></div>", parse_str("div"));
    BOOST_CHECK_EQUAL("<div class=\"red\"></div>", parse_str("div.red"));
    BOOST_CHECK_EQUAL("<div class=\"red blue\"></div>", parse_str("div.red.blue"));
    BOOST_CHECK_EQUAL("<div id=\"head\"></div>", parse_str("div#head"));
    BOOST_CHECK_EQUAL("<div id=\"head\" class=\"red green\">Hello</div>", parse_str("div#head.red.green Hello"));
    BOOST_CHECK_EQUAL("<div id=\"head\">Hello World</div>", parse_str("#head Hello World"));
    BOOST_CHECK_EQUAL("<div class=\"red green\">Hello World</div>", parse_str(".red.green Hello World"));
    BOOST_CHECK_EQUAL("<div id=\"x-y\" class=\"x_y\"></div>", parse_str("#x-y.x_y"));

    BOOST_CHECK_THROW(parse_str("div#"), TemplateSyntaxError);
    BOOST_CHECK_THROW(parse_str("div."), TemplateSyntaxError);
    BOOST_CHECK_THROW(parse_str("#"), TemplateSyntaxError);
    BOOST_CHECK_THROW(parse_str("."), TemplateSyntaxError);
}

BOOST_AUTO_TEST_CASE(attributes)
{
    BOOST_CHECK_EQUAL("<div<%=attr('id', @x)%>></div>", parse_str("div id=@x"));
    BOOST_CHECK_EQUAL("<div id=\"x\"></div>", parse_str("div id=\"x\""));
    BOOST_CHECK_EQUAL("<div<%=attr('id', (@x + \"-name\"))%>></div>", parse_str("div id=(@x + \"-name\")"));
    BOOST_CHECK_EQUAL("<div<%=attr('id', \"x-#{@name}\")%>></div>", parse_str("div id=\"x-#{@name}\""));
    BOOST_CHECK_EQUAL("<div disabled></div>", parse_str("div disabled=true"));
    BOOST_CHECK_EQUAL("<div></div>", parse_str("div disabled=false"));
    BOOST_CHECK_EQUAL("<div></div>", parse_str("div disabled=nil"));
    BOOST_CHECK_EQUAL("<div class=\"a b c d\"></div>", parse_str("div.a.b class=\"c d\""));
    BOOST_CHECK_EQUAL("<div<%=attr('class', 'a', 'b', @d)%>></div>", parse_str("div.a.b class=@d"));

    BOOST_CHECK_EQUAL("<div<%=attr('data-id', func())%><%=attr('data-name', @b)%>></div>",
        parse_str("div data-id=func data-name=@b"));
}

BOOST_AUTO_TEST_CASE(wrapped_attributes)
{
    BOOST_CHECK_EQUAL("<div<%=attr('id', @x)%>></div>", parse_str("div ( id = @x )"));
    BOOST_CHECK_EQUAL("<div<%=attr('id', @x)%>></div>", parse_str("div [ id = @x ]"));
    BOOST_CHECK_EQUAL("<div<%=attr('id', @x)%>></div>", parse_str("div { id = @x }"));
    BOOST_CHECK_EQUAL("<div<%=attr('id', @x)%>>Test</div>", parse_str("div { id = @x } Test"));
    BOOST_CHECK_EQUAL("<div></div>", parse_str("div {}"));
    BOOST_CHECK_EQUAL("<div<%=attr('id', @x)%>></div>", parse_str("div (\n  id =\n@x )"));
    BOOST_CHECK_EQUAL("<div<%=attr('id', @x)%><%=attr('title', @y)%>></div>", parse_str("div { id = @x title=@y }"));


    BOOST_CHECK_EQUAL("<div<%=attr('data-id', func())%><%=attr('data-name', @b)%>></div>",
        parse_str("div [data-id=func data-name = @b]"));

    BOOST_CHECK_THROW(parse_str("div {"), TemplateSyntaxError);
    BOOST_CHECK_THROW(parse_str("div { id"), TemplateSyntaxError);
    BOOST_CHECK_THROW(parse_str("div { id ="), SyntaxError);
    BOOST_CHECK_THROW(parse_str("div { id = 5"), TemplateSyntaxError);
    BOOST_CHECK_THROW(parse_str("div { id 5}"), TemplateSyntaxError);
}

BOOST_AUTO_TEST_CASE(cond)
{
    //if
    BOOST_CHECK_EQUAL(
        "<% if true %>"
        "<img/>"
        "<% end %>",
        parse_str(
            "-if true\n"
            "  img\n"
        ));

    //if else
    BOOST_CHECK_EQUAL(
        "<% if true %>"
        "<img/>"
        "<% else %>"
        "<hr/>"
        "<% end %>",
        parse_str(
            "-if true\n"
            "  img\n"
            "-else\n"
            "  hr\n"
        ));

    //if elsif
    BOOST_CHECK_EQUAL(
        "<% if true %>"
        "<img/>"
        "<% elsif false %>"
        "<br/>"
        "<% end %>",
        parse_str(
            "-if true\n"
            "  img\n"
            "-elsif false\n"
            "  br\n"
        ));

    //if elsif else
    BOOST_CHECK_EQUAL(
        "<% if true %>"
        "<img/>"
        "<% elsif false %>"
        "<br/>"
        "<% else %>"
        "<hr/>"
        "<% end %>",
        parse_str(
            "-if true\n"
            "  img\n"
            "-elsif false\n"
            "  br\n"
            "-else\n"
            "  hr\n"
        ));

    //if elsif elsif else
    BOOST_CHECK_EQUAL(
        "<% if true %>"
        "<img/>"
        "<% elsif false %>"
        "<br/>"
        "<% elsif nil %>"
        "<br/>"
        "<% else %>"
        "<hr/>"
        "<% end %>",
        parse_str(
            "-if true\n"
            "  img\n"
            "-elsif false\n"
            "  br\n"
            "-elsif nil\n"
            "  br\n"
            "-else\n"
            "  hr\n"
        ));

    //if if
    BOOST_CHECK_EQUAL(
        "<% if true %>"
        "<img/>"
        "<% end %>"
        "<% if false %>"
        "<br/>"
        "<% end %>",
        parse_str(
            "-if true\n"
            "  img\n"
            "-if false\n"
            "  br\n"
        ));


    BOOST_CHECK_THROW(parse_str("-elsif true"), TemplateSyntaxError);
    BOOST_CHECK_THROW(parse_str("-else"), TemplateSyntaxError);
    BOOST_CHECK_THROW(parse_str("-if true\n  p\n-else x\n  p\n"), TemplateSyntaxError);
}

BOOST_AUTO_TEST_CASE(each)
{
    BOOST_CHECK_EQUAL(
        "<% [0, 1, 2, 3].each({|| %>"
        "<img/>"
        "<%}) %>",
        parse_str(
            "-[0,1,2,3].each do\n"
            "  img\n"
        ));
    BOOST_CHECK_EQUAL(
        "<% [0, 1, 2, 3].each({|| %>"
        "<img/>"
        "<%}) %>",
        parse_str(
            "-[0,1,2,3].each do ||\n"
            "  img\n"
        ));
    BOOST_CHECK_EQUAL(
        "<% [0, 1, 2, 3].each({|x| %>"
        "<img<%=attr('width', x)%>/>"
        "<%}) %>",
        parse_str(
            "-[0,1,2,3].each do |x|\n"
            "  img width=x\n"
        ));

    BOOST_CHECK_THROW(parse_str("-'not a method' do\ndiv"), TemplateSyntaxError);
    BOOST_CHECK_THROW(parse_str("-[0,1,2]\ndiv"), TemplateSyntaxError);
}

BOOST_AUTO_TEST_CASE(syntax_error_info)
{
    //start
    try
    {
        parse_str("\t");
        BOOST_FAIL("Expected SyntaxError");
    }
    catch (const TemplateSyntaxError &e)
    {
        BOOST_CHECK_EQUAL(1, e.line());
        BOOST_CHECK_EQUAL(1, e.offset());
        BOOST_CHECK_EQUAL("inline_test.html.slim", e.file_name());
    }
    //offset, syntax
    try
    {
        auto str = parse_str("p.");
        BOOST_FAIL("Expected SyntaxError");
    }
    catch (const TemplateSyntaxError &e)
    {
        BOOST_CHECK_EQUAL(1, e.line());
        BOOST_CHECK_EQUAL(3, e.offset());
        BOOST_CHECK_EQUAL("inline_test.html.slim", e.file_name());
    }
    //lines, syntax
    try
    {
        parse_str("p Hello World\np Hello #{@name");
        BOOST_FAIL("Expected SyntaxError");
    }
    catch (const TemplateSyntaxError &e)
    {
        BOOST_CHECK_EQUAL(2, e.line());
        BOOST_CHECK_EQUAL(3, e.offset());
        BOOST_CHECK_EQUAL("inline_test.html.slim", e.file_name());
    }
    //parser
    try
    {
        parse_str("= testing \\\n");
        BOOST_FAIL("Expected SyntaxError");
    }
    catch (const TemplateSyntaxError &e)
    {
        BOOST_CHECK_EQUAL(2, e.line());
        BOOST_CHECK_EQUAL(1, e.offset());
        BOOST_CHECK_EQUAL("inline_test.html.slim", e.file_name());
    }
    //in code
    try
    {
        parse_str(
            "p\n"
            "= error )\n"
        );
        BOOST_FAIL("Expected SyntaxError");
    }
    catch (const TemplateSyntaxError &)
    {
        BOOST_FAIL("Expected SyntaxError not TemplateSyntaxError");
    }
    catch (const SyntaxError &e)
    {
        BOOST_CHECK_EQUAL(2, e.line());
        BOOST_CHECK_EQUAL(9, e.offset());
        BOOST_CHECK_EQUAL("inline_test.html.slim", e.file_name());
    }
    //in code with offset
    try
    {
        parse_str(
            "p\n"
            "  = error )\n"
        );
        BOOST_FAIL("Expected SyntaxError");
    }
    catch (const TemplateSyntaxError &)
    {
        BOOST_FAIL("Expected SyntaxError not TemplateSyntaxError");
    }
    catch (const SyntaxError &e)
    {
        BOOST_CHECK_EQUAL(2, e.line());
        BOOST_CHECK_EQUAL(11, e.offset());
        BOOST_CHECK_EQUAL("inline_test.html.slim", e.file_name());
    }
    //in tag with offset
    try
    {
        parse_str(
            "p\n"
            "p = error )\n"
        );
        BOOST_FAIL("Expected SyntaxError");
    }
    catch (const TemplateSyntaxError &)
    {
        BOOST_FAIL("Expected SyntaxError not TemplateSyntaxError");
    }
    catch (const SyntaxError &e)
    {
        BOOST_CHECK_EQUAL(2, e.line());
        BOOST_CHECK_EQUAL(11, e.offset());
        BOOST_CHECK_EQUAL("inline_test.html.slim", e.file_name());
    }
    //in multiline tag
    try
    {
        parse_str(
            "p\n"
            "p = error\\\n"
            "   !\n"
        );
        BOOST_FAIL("Expected SyntaxError");
    }
    catch (const TemplateSyntaxError &)
    {
        BOOST_FAIL("Expected SyntaxError not TemplateSyntaxError");
    }
    catch (const SyntaxError &e)
    {
        BOOST_CHECK_EQUAL(3, e.line());
        BOOST_CHECK_EQUAL(4, e.offset());
        BOOST_CHECK_EQUAL("inline_test.html.slim", e.file_name());
    }
    //in multiline do
    try
    {
        parse_str(
            "p\n"
            "= content_for x\\\n"
            "    do |x, !|\n"
            "  p Hello\n"
        );
        BOOST_FAIL("Expected SyntaxError");
    }
    catch (const TemplateSyntaxError &)
    {
        BOOST_FAIL("Expected SyntaxError not TemplateSyntaxError");
    }
    catch (const SyntaxError &e)
    {
        BOOST_CHECK_EQUAL(3, e.line());
        BOOST_CHECK_EQUAL(12, e.offset());
        BOOST_CHECK_EQUAL("inline_test.html.slim", e.file_name());
    }
}

BOOST_AUTO_TEST_SUITE_END()
