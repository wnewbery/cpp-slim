#pragma once
#include <string>
namespace slim
{
    namespace expr
    {
        class Scope;
    }
    namespace tpl
    {
        /**@brief A part of a template.
         * This is essentially an abstract syntax tree with all the adjacent plain text nodes merged.
         */
        class TemplatePart
        {
        public:
            virtual ~TemplatePart() {};

            /**See Template::to_string. */
            virtual std::string to_string()const = 0;
            /**Renders this part to the buffer, using the specified variable scope.*/
            virtual void render(std::string &buffer, expr::Scope &scope)const = 0;
        };
    }
}
