#include "TemplateBlock.hpp"
#include "Template.hpp"
#include "types/HtmlSafeString.hpp"
#include "expression/AstOp.hpp"
#include "Util.hpp"
namespace slim
{
    namespace tpl
    {
        TemplateBlock::TemplateBlock(std::unique_ptr<TemplatePart> &&tpl)
            : tpl(std::move(tpl))
        {}
        TemplateBlock::~TemplateBlock() {}

        std::string TemplateBlock::to_string()const
        {
            return "%>" + tpl->to_string() + "<%";
        }
        ObjectPtr TemplateBlock::eval(expr::Scope &scope)const
        {
            std::string str;
            tpl->render(str, scope);
            return create_object<HtmlSafeString>(std::move(str));
        }

        std::unique_ptr<expr::ExpressionNode> create_tpl_block(
            std::vector<SymPtr> &&param_names,
            std::unique_ptr<TemplatePart> &&tpl)
        {
            auto tpl_block = slim::make_unique<TemplateBlock>(std::move(tpl));
            return slim::make_unique<expr::Block>(std::move(param_names), std::move(tpl_block));
        }
    }
}
