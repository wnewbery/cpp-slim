#include <slim/Template.hpp>
#include "Vector2.hpp"
#include <fstream>

int main()
{
    auto model = slim::create_view_model();
    model->add_constant("Vector2", slim::create_object<slim::SimpleClass<Vector2>>());
    model->set_attr("pos", slim::create_object<Vector2>(50, 30));
    model->set_attr("move", slim::create_object<Vector2>(5,5));

    auto tpl = slim::parse_template_file("view.html.slim");
    auto html = tpl.render(model);

    std::fstream fs("output.html", std::ios::out | std::ios::binary);
    fs.write(html.data(), html.size());

    return 0;
}
