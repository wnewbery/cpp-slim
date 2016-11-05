#include <boost/test/unit_test.hpp>
#include "Template.hpp"
#include "types/ViewModel.hpp"
#include "types/Hash.hpp"
#include "types/HtmlSafeString.hpp"
#include "Util.hpp"

using namespace slim;
using namespace slim::tpl;
BOOST_AUTO_TEST_SUITE(TestPartial)


BOOST_AUTO_TEST_CASE(partial)
{
    auto partial_tpl = parse_template(
        "p Partial\n"
        "p=a + b\n",
        {"a", "b"});
    auto tpl = parse_template(
        "p Main Template\n"
        "=@partial.partial({a: 5, b: 10})");

    class Partial : public slim::Object
    {
    public:
        static const std::string &name()
        {
            static const std::string TYPE_NAME = "Partial";
            return TYPE_NAME;
        }
        virtual const std::string& type_name()const override { return name(); }

        Partial(Template *tpl) : tpl(tpl) {}
        Ptr<HtmlSafeString> partial(Hash *locals)
        {
            expr::Scope scope(create_view_model());
            scope.set(locals);
            auto buffer = tpl->render_partial(scope);
            return create_object<HtmlSafeString>(buffer);
        };
    protected:
        virtual const slim::MethodTable &method_table()const override
        {
            static const slim::MethodTable table(slim::Object::method_table(),
            {{&Partial::partial, "partial"}});
            return table;
        }
    private:
        Template *tpl;
    };

    auto mv = create_view_model();
    mv->set_attr("partial", create_object<Partial>(&partial_tpl));

    auto html = tpl.render(mv, false);
    BOOST_CHECK_EQUAL(
        "<p>Main Template</p>"
        "<p>Partial</p>"
        "<p>15</p>",
        html);
}

BOOST_AUTO_TEST_SUITE_END()
