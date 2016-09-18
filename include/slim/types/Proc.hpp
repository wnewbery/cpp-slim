#pragma once
#include "Object.hpp"
#include <functional>
#include <memory>
#include <vector>
namespace slim
{
    namespace expr
    {
        class ExpressionNode;
        class Scope;
    }
    /**Proc script object. This is as callable object used for blocks.*/
    class Proc : public Object
    {
    public:
        static const std::string TYPE_NAME;
        Proc(
            const expr::ExpressionNode &code,
            const std::vector<SymPtr> &param_names,
            expr::Scope &scope);
        ~Proc();

        virtual const std::string& type_name()const override { return TYPE_NAME; }

        ObjectPtr call(const FunctionArgs &args);
    protected:
        virtual const MethodTable &method_table()const override;
    private:
        const expr::ExpressionNode &code;
        const std::vector<SymPtr> &param_names;
        expr::Scope &scope;
    };
}
