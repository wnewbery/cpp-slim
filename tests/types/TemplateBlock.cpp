#include <boost/test/unit_test.hpp>
#include "Template.hpp"
#include "expression/Scope.hpp"
#include "types/HtmlSafeString.hpp"
#include "types/Proc.hpp"
#include "Util.hpp"

using namespace slim;
using namespace slim::tpl;
/**Test the TemplateBlock and associated logic used to implement code output lines that accept a block.
 *
 * @verbatim
 * =content_tag "a"
 *   = @title
 * @endverbatim
 */
BOOST_AUTO_TEST_SUITE(TestTemplateBlock)

class Test : public Object
{
public:
    std::string name = "Test";
    virtual const std::string& type_name()const override { return name; }
    virtual const MethodTable &method_table()const override
    {
        static const MethodTable table(Object::method_table(),
        {
            { &Test::content_tag, "content_tag" },
            { &Test::with_params, "with_params" }
        });
        return table;
    }
    StringPtr content_tag(String *name, Proc *content)
    {
        std::stringstream ss;
        ss << '<' << name->get_value() << '>';
        ss << html_encode(content->call({}));
        ss << "</" << name->get_value() << '>';
        return create_object<HtmlSafeString>(ss.str());
    }
    ObjectPtr with_params(Proc *content)
    {
        return content->call({make_value("x"), make_value(55)});
    }
};

std::string parse_str(const std::string &src)
{
    return parse_template(src).to_string();
}
std::string render_tpl(const std::string &src, ViewModel &model)
{
    return parse_template(src).render(model, false);
}

BOOST_AUTO_TEST_CASE(parse)
{
    BOOST_CHECK_EQUAL("<%= func({|| %>content<%}) %>", parse_str("=func\n  | content"));
    BOOST_CHECK_EQUAL("<%= func({|| %>content<%}) %><br/>", parse_str("=func\n  | content\nbr"));
    BOOST_CHECK_EQUAL("<%= f().g({|| %>content<%}) %>", parse_str("=f.g\n  | content"));
    BOOST_CHECK_EQUAL("<%= f().g({|| %>content<%}) %><br/>", parse_str("=f.g\n  | content\nbr"));

    BOOST_CHECK_THROW(parse_str("=@attr\n  | content"), TemplateSyntaxError);
}

BOOST_AUTO_TEST_CASE(parse_params)
{
    BOOST_CHECK_EQUAL("<%= func({|| %>content<%}) %>", parse_str("=func do\n  | content"));
    BOOST_CHECK_EQUAL("<%= func({|| %>content<%}) %>", parse_str("=func do ||\n  | content"));
    BOOST_CHECK_EQUAL("<%= func({|a| %>content<%}) %>", parse_str("=func do |a|\n  | content"));
}

BOOST_AUTO_TEST_CASE(render)
{
    ViewModel model;
    model.set("test", create_object<Test>());
    model.set("a", make_value(10.0));

    BOOST_CHECK_EQUAL(
        "<p>10</p>",
        render_tpl("=@test.content_tag 'p'\n  =@a", model));
    //TODO:  Current Proc params must match exactly
    //BOOST_CHECK_EQUAL(
    //    "test",
    //    render_tpl("=@test.with_params do\n  |test", model));
    //BOOST_CHECK_EQUAL(
    //    "test",
    //    render_tpl("=@test.with_params do ||\n  |test", model));
    //BOOST_CHECK_EQUAL(
    //    "x",
    //    render_tpl("=@test.with_params do |x|\n  =x", model));
    BOOST_CHECK_EQUAL(
        "x=55",
        render_tpl("=@test.with_params do |name, val|\n  ='#{name}=#{val}'", model));
}



BOOST_AUTO_TEST_SUITE_END()
