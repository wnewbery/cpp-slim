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

BOOST_AUTO_TEST_CASE(code_lines)
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
    BOOST_CHECK_EQUAL("<div<%=attr('id', (@x + \"-name\"))%>></div>", parse_str("div id=@x + \"-name\""));
    BOOST_CHECK_EQUAL("<div<%=attr('id', \"x-#{@name}\")%>></div>", parse_str("div id=\"x-#{@name}\""));
    BOOST_CHECK_EQUAL("<div disabled></div>", parse_str("div disabled=true"));
    BOOST_CHECK_EQUAL("<div></div>", parse_str("div disabled=false"));
    BOOST_CHECK_EQUAL("<div></div>", parse_str("div disabled=nil"));
    BOOST_CHECK_EQUAL("<div class=\"a b c d\"></div>", parse_str("div.a.b class=\"c d\""));
    BOOST_CHECK_EQUAL("<div<%=attr('class', 'a', 'b', @d)%>></div>", parse_str("div.a.b class=@d"));
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

BOOST_AUTO_TEST_SUITE_END()
