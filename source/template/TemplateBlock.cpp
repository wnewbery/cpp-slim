#include "TemplateBlock.hpp"
#include "template/TemplatePart.hpp"
#include "types/HtmlSafeString.hpp"
#include "expression/AstOp.hpp"
#include "expression/Scope.hpp"
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

        ObjectPtr TemplateCaptureBlock::eval(expr::Scope &scope)const
        {
            std::string str;
            tpl->render(str, scope);
            return create_object<HtmlSafeString>(std::move(str));
        }

        ObjectPtr TemplateOutputBlock::eval(expr::Scope &scope)const
        {
            static auto SYM_output_buffer = symbol("output_buffer");
            auto &str = coerce<String>(scope.get(SYM_output_buffer))->get_value();
            tpl->render(str, scope);
            return NIL_VALUE;
        }

        std::unique_ptr<expr::ExpressionNode> create_tpl_capture_block(
            std::vector<SymPtr> &&param_names,
            std::unique_ptr<TemplatePart> &&tpl)
        {
            auto tpl_block = slim::make_unique<TemplateCaptureBlock>(std::move(tpl));
            return slim::make_unique<expr::Block>(std::move(param_names), std::move(tpl_block));
        }

        std::unique_ptr<expr::ExpressionNode> create_tpl_output_block(
            std::vector<SymPtr> &&param_names,
            std::unique_ptr<TemplatePart> &&tpl)
        {
            auto tpl_block = slim::make_unique<TemplateOutputBlock>(std::move(tpl));
            return slim::make_unique<expr::Block>(std::move(param_names), std::move(tpl_block));
        }
    }
}
