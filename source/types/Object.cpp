#include "types/Object.hpp"
#include "types/Boolean.hpp"
#include "types/Null.hpp"
#include "types/Number.hpp"
#include "types/String.hpp"
#include "Error.hpp"
#include <sstream>

namespace slim
{
    const std::string Boolean::TYPE_NAME = "Boolean";
    const std::string Null::TYPE_NAME = "Null";
    const std::string Number::TYPE_NAME = "Number";
    const std::string String::TYPE_NAME = "String";

    const std::shared_ptr<Null> NULL_VALUE = std::make_shared<Null>();
    const std::shared_ptr<Boolean> TRUE_VALUE = std::make_shared<Boolean>(true);
    const std::shared_ptr<Boolean> FALSE_VALUE = std::make_shared<Boolean>(false);

    bool Object::eq(const Object *rhs)const
    {
        return this == rhs; //identity
    }
    int Object::cmp(const Object *rhs)const
    {
        throw UnorderableTypeError(this, "cmp", rhs);
    }

    std::string Number::to_string()const
    {
        std::stringstream ss;
        ss << v;
        return ss.str();
    }


}

