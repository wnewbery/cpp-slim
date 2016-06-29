#include "expression/CmpOp.hpp"
#include "expression/Scope.hpp"
#include "types/Object.hpp"
#include "Operators.hpp"
namespace slim
{
    namespace expr
    {
        ObjectPtr Eq::eval(Scope & scope) const
        {
            auto lhs_v = lhs->eval(scope);
            auto rhs_v = rhs->eval(scope);
            return op_eq(lhs_v.get(), rhs_v.get());
        }
        ObjectPtr Ne::eval(Scope & scope) const
        {
            auto lhs_v = lhs->eval(scope);
            auto rhs_v = rhs->eval(scope);
            return op_ne(lhs_v.get(), rhs_v.get());
        }
        ObjectPtr Lt::eval(Scope & scope) const
        {
            auto lhs_v = lhs->eval(scope);
            auto rhs_v = rhs->eval(scope);
            return op_lt(lhs_v.get(), rhs_v.get());
        }
        ObjectPtr Le::eval(Scope & scope) const
        {
            auto lhs_v = lhs->eval(scope);
            auto rhs_v = rhs->eval(scope);
            return op_le(lhs_v.get(), rhs_v.get());
        }
        ObjectPtr Gt::eval(Scope & scope) const
        {
            auto lhs_v = lhs->eval(scope);
            auto rhs_v = rhs->eval(scope);
            return op_gt(lhs_v.get(), rhs_v.get());
        }
        ObjectPtr Ge::eval(Scope & scope) const
        {
            auto lhs_v = lhs->eval(scope);
            auto rhs_v = rhs->eval(scope);
            return op_ge(lhs_v.get(), rhs_v.get());
        }
    }
}
