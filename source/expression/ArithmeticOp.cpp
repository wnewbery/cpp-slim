#include "expression/ArithmeticOp.hpp"
#include "expression/Scope.hpp"
#include "types/Object.hpp"
#include "Operators.hpp"
namespace slim
{
    namespace expr
    {
        ObjectPtr Negative::eval(Scope & scope) const
        {
            return op_negative(arg->eval(scope).get());
        }
        ObjectPtr Mul::eval(Scope & scope) const
        {
            auto lhs_v = lhs->eval(scope);
            auto rhs_v = rhs->eval(scope);
            return op_mul(lhs_v.get(), rhs_v.get());
        }
        ObjectPtr Div::eval(Scope & scope) const
        {
            auto lhs_v = lhs->eval(scope);
            auto rhs_v = rhs->eval(scope);
            return op_div(lhs_v.get(), rhs_v.get());
        }
        ObjectPtr Mod::eval(Scope & scope) const
        {
            auto lhs_v = lhs->eval(scope);
            auto rhs_v = rhs->eval(scope);
            return op_mod(lhs_v.get(), rhs_v.get());
        }
        ObjectPtr Add::eval(Scope & scope) const
        {
            auto lhs_v = lhs->eval(scope);
            auto rhs_v = rhs->eval(scope);
            return op_add(lhs_v.get(), rhs_v.get());
        }
        ObjectPtr Sub::eval(Scope & scope) const
        {
            auto lhs_v = lhs->eval(scope);
            auto rhs_v = rhs->eval(scope);
            return op_sub(lhs_v.get(), rhs_v.get());
        }
    }
}
