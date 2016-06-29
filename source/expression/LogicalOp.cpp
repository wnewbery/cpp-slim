#include "expression/LogicalOp.hpp"
#include "expression/Scope.hpp"
#include "types/Object.hpp"
#include "Operators.hpp"
namespace slim
{
    namespace expr
    {
        ObjectPtr LogicalNot::eval(Scope & scope) const
        {
            auto arg_v = arg->eval(scope);
            return op_not(arg_v.get());
        }

        ObjectPtr LogicalAnd::eval(Scope & scope) const
        {
            auto lhs_v = lhs->eval(scope);
            if (lhs_v->is_true()) return rhs->eval(scope);
            else return lhs_v;
        }

        ObjectPtr LogicalOr::eval(Scope & scope) const
        {
            auto lhs_v = lhs->eval(scope);
            if (!lhs_v->is_true()) return rhs->eval(scope);
            else return lhs_v;
        }

    }
}
