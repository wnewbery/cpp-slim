#include "types/Proc.hpp"
#include "expression/Ast.hpp"
#include "expression/Scope.hpp"
#include "Function.hpp"
#include <sstream>

namespace slim
{
    Proc::Proc(
        const expr::ExpressionNode &code,
        const std::vector<SymPtr> &param_names,
        expr::Scope &scope)
        : code(code), param_names(param_names), scope(scope)
    {
    }
    Proc::~Proc()
    {
    }

    ObjectPtr Proc::call(const FunctionArgs & args)
    {
        if (args.size() != param_names.size())
        {
            std::stringstream ss;
            ss << "wrong number of arguments (" << args.size() << " for " << param_names.size() << ")";
            throw ArgumentError(this, "call", ss.str());
        }
        
        expr::Scope new_scope(scope);
        for (size_t i = 0; i < args.size(); ++i) new_scope.set(param_names[i], args[i]);
        
        auto result = code.eval(new_scope);

        return result;
    }

    const MethodTable &Proc::method_table()const
    {
        static const MethodTable table(Object::method_table(),
        {
            { &Proc::call, "call" }
        });
        return table;
    }
}
