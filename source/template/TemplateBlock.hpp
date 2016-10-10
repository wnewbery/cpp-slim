#pragma once
#include "expression/Ast.hpp"
#include "types/Proc.hpp"
namespace slim
{
    namespace tpl
    {
        class TemplatePart;

        /**A block which contains a template fragment. Can be used to create a expr::Block.*/
        class TemplateBlock : public expr::ExpressionNode
        {
        public:
            TemplateBlock(std::unique_ptr<TemplatePart> &&tpl);
            ~TemplateBlock();
            virtual std::string to_string()const override;
        protected:
            std::unique_ptr<TemplatePart> tpl;
        };
        /**When evaluated returns a HtmlSafeString.*/
        class TemplateCaptureBlock : public TemplateBlock
        {
        public:
            using TemplateBlock::TemplateBlock;
            virtual ObjectPtr eval(expr::Scope &scope)const override;
        };
        /**When evaluated, renders to a local variable named output_buffer.*/
        class TemplateOutputBlock : public TemplateBlock
        {
        public:
            using TemplateBlock::TemplateBlock;
            virtual ObjectPtr eval(expr::Scope &scope)const override;
        };

        /**Creates a expr::Block node containing a TemplateCaptureBlock.*/
        std::unique_ptr<expr::ExpressionNode> create_tpl_capture_block(
            std::vector<SymPtr> &&param_names,
            std::unique_ptr<TemplatePart> &&tpl);

        /**Creates a expr::Block node containing a TemplateOutputBlock.*/
        std::unique_ptr<expr::ExpressionNode> create_tpl_output_block(
            std::vector<SymPtr> &&param_names,
            std::unique_ptr<TemplatePart> &&tpl);
    }
}
