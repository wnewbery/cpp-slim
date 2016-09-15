#include "template/Template.hpp"
#include "template/TemplatePart.hpp"
#include "expression/Scope.hpp"
namespace slim
{
    Template::Template(std::unique_ptr<tpl::TemplatePart> &&root)
        : root(std::move(root))
    {}

    Template::~Template()
    {}

    Template::Template(Template &&) = default;
    Template& Template::operator = (Template &&) = default;

    std::string Template::render(ViewModelPtr model, bool doctype)
    {
        std::string buffer;
        if (doctype) buffer += "<!DOCTYPE html>\n";
        expr::Scope scope(model);
        root->render(buffer, scope);
        return buffer;
    }
    std::string Template::to_string()const
    {
        return root->to_string();
    }
}
