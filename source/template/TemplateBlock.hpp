#pragma once
#include "expression/Ast.hpp"
#include "types/Proc.hpp"
namespace slim
{
    namespace tpl
    {
        class TemplatePart;

        /**A block which contains a template fragment. Can be used to create a expr::Block.
         * When evaluated returns a HtmlSafeString.
         */
        class TemplateBlock : public expr::ExpressionNode
        {
        public:
            TemplateBlock(std::unique_ptr<TemplatePart> &&tpl);
            ~TemplateBlock();
            virtual std::string to_string()const override;
            virtual ObjectPtr eval(expr::Scope &scope)const override;
        private:
            std::unique_ptr<TemplatePart> tpl;
        };

        /**Creates a expr::Block node containing a TemplateBlock.*/
        std::unique_ptr<expr::ExpressionNode> create_tpl_block(
            std::vector<SymPtr> &&param_names,
            std::unique_ptr<TemplatePart> &&tpl);
    }
}
