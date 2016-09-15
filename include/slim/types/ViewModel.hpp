#pragma once
#include "Object.hpp"
namespace slim
{
    /**"self" object for rendering templates.
     *
     * The ViewModel holds all the attributes, methods and constants.
     */
    class ViewModel : public Object
    {
    public:
        static const std::string TYPE_NAME;

        ViewModel();
        ~ViewModel();

        virtual const std::string& type_name()const override { return TYPE_NAME; }


        virtual ObjectPtr get_constant(SymPtr name)override;
        void add_constant(SymPtr name, ObjectPtr constant);
        void add_constant(const std::string &name, ObjectPtr constant);


        ObjectPtr get_attr(SymPtr name);
        void set_attr(SymPtr name, ObjectPtr value);
        void set_attr(const std::string &name, ObjectPtr value);
    protected:
        ObjectMap attrs;
        ObjectMap constants;
    };

    typedef std::shared_ptr<ViewModel> ViewModelPtr;
    inline ViewModelPtr create_view_model()
    {
        return create_object<ViewModel>();
    }
}
