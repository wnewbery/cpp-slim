#include "Operators.hpp"
#include "Error.hpp"
#include "types/Boolean.hpp"
#include "types/Null.hpp"
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
        int rel = imp_cmp(lhs, "<=>", rhs);
        return make_value(rel < 0 ? -1.0 : (rel > 0 ? 1.0 : 0.0));
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

    ObjectPtr op_mul(const Object *lhs, const Object *rhs)
    {
        auto lhs_num = dynamic_cast<const Number*>(lhs);
        auto rhs_num = dynamic_cast<const Number*>(rhs);
        if (lhs_num && rhs_num) return make_value(lhs_num->get_value() * rhs_num->get_value());
        throw UnsupportedOperandTypeError(lhs, "*", rhs);
    }
    ObjectPtr op_div(const Object *lhs, const Object *rhs)
    {
        auto lhs_num = dynamic_cast<const Number*>(lhs);
        auto rhs_num = dynamic_cast<const Number*>(rhs);
        if (lhs_num && rhs_num) return make_value(lhs_num->get_value() / rhs_num->get_value());
        throw UnsupportedOperandTypeError(lhs, "-", rhs);
    }
    ObjectPtr op_mod(const Object *lhs, const Object *rhs)
    {
        auto lhs_num = dynamic_cast<const Number*>(lhs);
        auto rhs_num = dynamic_cast<const Number*>(rhs);
        if (lhs_num && rhs_num) return make_value(std::fmod(lhs_num->get_value(), rhs_num->get_value()));
        throw UnsupportedOperandTypeError(lhs, "%", rhs);
    }

    ObjectPtr op_add(const Object *lhs, const Object *rhs)
    {
        auto lhs_num = dynamic_cast<const Number*>(lhs);
        auto rhs_num = dynamic_cast<const Number*>(rhs);
        if (lhs_num && rhs_num) return make_value(lhs_num->get_value() + rhs_num->get_value());

        auto lhs_str = dynamic_cast<const String*>(lhs);
        auto rhs_str = dynamic_cast<const String*>(rhs);
        if (lhs_str && rhs_str) return make_value(lhs_str->get_value() + rhs_str->get_value());

        throw UnsupportedOperandTypeError(lhs, "+", rhs);
    }
    ObjectPtr op_sub(const Object *lhs, const Object *rhs)
    {
        auto lhs_num = dynamic_cast<const Number*>(lhs);
        auto rhs_num = dynamic_cast<const Number*>(rhs);
        if (lhs_num && rhs_num) return make_value(lhs_num->get_value() - rhs_num->get_value());
        throw UnsupportedOperandTypeError(lhs, "-", rhs);
    }

    ObjectPtr op_not(const Object *rhs)
    {
        return make_value(!rhs->is_true());
    }
    ObjectPtr op_negative(const Object *rhs)
    {
        auto num = dynamic_cast<const Number*>(rhs);
        if (!num) throw UnsupportedOperandTypeError("-", rhs);
        return make_value(-num->get_value());
    }
}
