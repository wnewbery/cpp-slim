#include "Operators.hpp"
#include "Error.hpp"
#include "types/Boolean.hpp"
#include "types/Nil.hpp"
#include "types/Number.hpp"
#include "types/String.hpp"
namespace slim
{
    bool imp_eq(const Object *lhs, const Object *rhs)
    {
        auto &lhs_t = typeid(*lhs);
        auto &rhs_t = typeid(*rhs);
        if (lhs_t == rhs_t) return lhs->eq(rhs);
        else return false;
    }
    int imp_cmp(const Object *lhs, const char *op, const Object *rhs)
    {
        auto &lhs_t = typeid(*lhs);
        auto &rhs_t = typeid(*rhs);
        if (lhs_t == rhs_t) return lhs->cmp(rhs);
        else throw UnorderableTypeError(lhs, op, rhs);
    }
    void eq_operands(const Object *lhs, const char *op, const Object *rhs)
    {
        if (typeid(*lhs) != typeid(*rhs))
            throw UnsupportedOperandTypeError(lhs, op, rhs);
    }

    bool eq(const Object *lhs, const Object *rhs)
    {
        return imp_eq(lhs, rhs);
    }
    int cmp(const Object *lhs, const Object *rhs)
    {
        int rel = imp_cmp(lhs, "<=>", rhs);
        return rel < 0 ? -1 : (rel > 0 ? 1 : 0);
    }

    ObjectPtr op_eq(const Object *lhs, const Object *rhs)
    {
        return make_value(imp_eq(lhs, rhs));
    }
    ObjectPtr op_ne(const Object *lhs, const Object *rhs)
    {
        return make_value(!imp_eq(lhs, rhs));
    }
    ObjectPtr op_cmp(const Object *lhs, const Object *rhs)
    {
        return make_value((double)cmp(lhs, rhs));
    }
     ObjectPtr op_lt(const Object *lhs, const Object *rhs)
    {
        return make_value(imp_cmp(lhs, "<", rhs) < 0);
    }
    ObjectPtr op_le(const Object *lhs, const Object *rhs)
    {
        return make_value(imp_cmp(lhs, "<=", rhs) <= 0);
    }
    ObjectPtr op_gt(const Object *lhs, const Object *rhs)
    {
        return make_value(imp_cmp(lhs, ">", rhs) > 0);
    }
    ObjectPtr op_ge(const Object *lhs, const Object *rhs)
    {
        return make_value(imp_cmp(lhs, ">=", rhs) >= 0);
    }

    ObjectPtr op_not(Object *rhs)
    {
        return make_value(!rhs->is_true());
    }
}
