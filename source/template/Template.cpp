#include "template/Template.hpp"
#include "expression/Scope.hpp"
namespace slim
{
    std::string Template::render(const FunctionTable &globals, ViewModel &model, bool doctype)
    {
        std::string buffer;
        if (doctype) buffer += "<!DOCTYPE html>\n";
        expr::Scope scope(globals, model);
        root->render(buffer, scope);
        return buffer;
    }
}
