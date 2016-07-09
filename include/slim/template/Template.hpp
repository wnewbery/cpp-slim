#pragma once
#include <memory>
#include <string>
#include <vector>
namespace slim
{
    namespace expr
    {
        class Scope;
    }
    typedef expr::Scope ViewModel;

    namespace tpl
    {
        /**@brief A part of a template.
         * This is essentially an abstract syntax tree with all the adjacent plain text nodes merged.
         */
        class TemplatePart
        {
        public:
            virtual ~TemplatePart() {};

            /**Converts the template part into a string representation, mainly for debugging.
             * Because the origenal template structure has all ready been lost, as it was converted
             * to plain HTML fragments with script blocks, the output will use the Ruby ERB syntax,
             * without any whitespace formatting.
             */
            virtual std::string to_string()const = 0;
            /**Renders this part to the buffer, using the specified variable scope.*/
            virtual void render(std::string &buffer, expr::Scope &scope)const = 0;
        };
    }
    /**@brief A parsed template, ready to be rendered using variables in a ViewModel.*/
    class Template
    {
    public:
        Template(std::unique_ptr<tpl::TemplatePart> &&root)
            : root(std::move(root))
        {}

        /**Render this template.
         * @param model Contains the variables for script blocks within the template.
         * @param doctype If true, prefix the HTML5 doctype.
         */
        std::string render(ViewModel &model, bool doctype = true)
        {
            std::string buffer;
            if (doctype) buffer += "<!DOCTYPE html>\n";
            root->render(buffer, model);
            return buffer;
        }
    private:
        /**The root TemplatePart part. Most likely a TemplatePartsList, but this is not garunteed.*/
        std::unique_ptr<tpl::TemplatePart> root;
    };
}
