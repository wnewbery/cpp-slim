#pragma once
#include <memory>
namespace slim
{
    class Object;
    typedef std::shared_ptr<Object> ObjectPtr;

    bool eq(const Object *lhs, const Object *rhs);
    int cmp(const Object *lhs, const Object *rhs);

    ObjectPtr op_eq(const Object *lhs, const Object *rhs);
    ObjectPtr op_ne(const Object *lhs, const Object *rhs);
    ObjectPtr op_cmp(const Object *lhs, const Object *rhs);
    ObjectPtr op_lt(const Object *lhs, const Object *rhs);
    ObjectPtr op_le(const Object *lhs, const Object *rhs);
    ObjectPtr op_gt(const Object *lhs, const Object *rhs);
    ObjectPtr op_ge(const Object *lhs, const Object *rhs);

    ObjectPtr op_not(Object *rhs);
}
