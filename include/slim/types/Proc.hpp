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
    /**Callable proc object. This is as callable object used for blocks.*/
    class Proc : public Object
    {
    public:
        static const std::string &name()
        {
            static const std::string TYPE_NAME = "Proc";
            return TYPE_NAME;
        }
        virtual const std::string& type_name()const override { return name(); }

        virtual ObjectPtr call(const FunctionArgs &args)=0;
    protected:
        virtual const MethodTable &method_table()const override;
    };
    /**Proc object for a script code block.*/
    class BlockProc : public Proc
    {
    public:
        BlockProc(
            const expr::ExpressionNode &code,
            const std::vector<SymPtr> &param_names,
            expr::Scope &scope);

        virtual ObjectPtr call(const FunctionArgs &args)override;
    private:
        const expr::ExpressionNode &code;
        const std::vector<SymPtr> &param_names;
        expr::Scope &scope;
    };
    /**Proc object for a c++ function object.*/
    class FunctionProc : public Proc
    {
    public:
        typedef std::function<ObjectPtr(const FunctionArgs &args)> Func;
        FunctionProc(Func func) : func(func) {}
        virtual ObjectPtr call(const FunctionArgs &args)override
        {
            return func(args);
        }
    private:
        Func func;
    };
}
