#pragma once
#include "Object.hpp"
namespace slim
{
    class Proc;
    class HtmlSafeString;
    /**"self" object for rendering templates.
     *
     * The ViewModel holds all the attributes, methods and constants.
     */
    class ViewModel : public Object
    {
    public:
        ViewModel();
        ~ViewModel();

        static const std::string &name()
        {
            static const std::string TYPE_NAME = "ViewModel";
            return TYPE_NAME;
        }
        virtual const std::string& type_name()const override { return name(); }


        virtual ObjectPtr get_constant(SymPtr name)override;
        void add_constant(SymPtr name, ObjectPtr constant);
        void add_constant(const std::string &name, ObjectPtr constant);


        ObjectPtr get_attr(SymPtr name);
        void set_attr(SymPtr name, ObjectPtr value);
        void set_attr(const std::string &name, ObjectPtr value);

        void content_for(SymPtr name, std::shared_ptr<Proc> proc);
        std::shared_ptr<HtmlSafeString> yield(const FunctionArgs &args);
        /**Used by Template to store the content for the layout Template.*/
        void set_main_content(std::shared_ptr<HtmlSafeString> content);
    protected:
        ObjectMap attrs;
        ObjectMap constants;
        /**Content from the main view for a "yield" in layout.*/
        std::shared_ptr<HtmlSafeString> main_content;
        /**Save all the rendered content_for blocks for yield.*/
        ObjectMap content_for_store;

        virtual const MethodTable &method_table()const override;
    };

    typedef std::shared_ptr<ViewModel> ViewModelPtr;
    inline ViewModelPtr create_view_model()
    {
        return create_object<ViewModel>();
    }
}
