#include "template/Template.hpp"
#include "expression/Scope.hpp"
namespace slim
{
    std::string Template::render(ViewModel &model, bool doctype)
    {
        std::string buffer;
        if (doctype) buffer += "<!DOCTYPE html>\n";
        expr::Scope scope(model);
        root->render(buffer, scope);
        return buffer;
    }
}
