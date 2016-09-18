#include "types/Object.hpp"
#include "types/Boolean.hpp"
#include "types/Nil.hpp"
#include "types/Number.hpp"
#include "types/String.hpp"
#include "Function.hpp"
#include "Error.hpp"
#include <iomanip>
#include <sstream>

namespace slim
{
    const std::string Boolean::TYPE_NAME = "Boolean";
    const std::string Nil::TYPE_NAME = "Nil";

    const std::shared_ptr<Nil> NIL_VALUE = std::make_shared<Nil>();
    const std::shared_ptr<Boolean> TRUE_VALUE = std::make_shared<Boolean>(true);
    const std::shared_ptr<Boolean> FALSE_VALUE = std::make_shared<Boolean>(false);

    std::string Object::inspect()const
    {
        std::stringstream ss;
        ss << "#<" << type_name() << ":0x";
        ss << std::hex << std::setw((int)sizeof(void*) * 2) << std::setfill('0') << std::right;
        ss << ((uintptr_t)this);
        ss << ">";
        return ss.str();
    }
    bool Object::eq(const Object *rhs)const
    {
        return this == rhs; //identity
    }
    size_t Object::hash()const
    {
        return detail::hash(this);
    }
    int Object::cmp(const Object *rhs)const
    {
        throw UnorderableTypeError(this, "cmp", rhs);
    }

    ObjectPtr Object::el_ref(const FunctionArgs & args)
    {
        throw NoMethodError(this, "[]");
    }

    ObjectPtr Object::mul(Object * rhs)
    {
        throw NoMethodError(this, "*");
    }
    ObjectPtr Object::div(Object * rhs)
    {
        throw NoMethodError(this, "/");
    }
    ObjectPtr Object::mod(Object * rhs)
    {
        throw NoMethodError(this, "%");
    }
    ObjectPtr Object::pow(Object * rhs)
    {
        throw NoMethodError(this, "**");
    }
    ObjectPtr Object::add(Object * rhs)
    {
        throw NoMethodError(this, "+");
    }
    ObjectPtr Object::sub(Object * rhs)
    {
        throw NoMethodError(this, "-");
    }
    ObjectPtr Object::negate()
    {
        throw NoMethodError(this, "-negate");
    }
    ObjectPtr Object::bit_lshift(Object *rhs)
    {
        throw NoMethodError(this, "<<");
    }
    ObjectPtr Object::bit_rshift(Object *rhs)
    {
        throw NoMethodError(this, ">>");
    }
    ObjectPtr Object::bit_and(Object *rhs)
    {
        throw NoMethodError(this, "&");
    }
    ObjectPtr Object::bit_or(Object *rhs)
    {
        throw NoMethodError(this, "|");
    }
    ObjectPtr Object::bit_xor(Object *rhs)
    {
        throw NoMethodError(this, "^");
    }
    ObjectPtr Object::bit_not()
    {
        throw NoMethodError(this, "~");
    }

    const Method *Object::find_method(SymPtr name)const
    {
        return method_table().find(name);
    }
    ObjectPtr Object::call_method(SymPtr name, const FunctionArgs &args)
    {
        auto method = find_method(name);
        if (method) return (*method)(this, args);
        else throw NoMethodError(this, name.get());
    }
    ObjectPtr Object::get_constant(SymPtr)
    {
        throw TypeError(this->inspect() + " is not a class/module");
    }

    std::shared_ptr<String> Object::to_string_obj()
    {
        return make_value(to_string());
    }
    ObjectPtr Object::inspect_obj()
    {
        return make_value(inspect());
    }
    const MethodTable &Object::method_table()const
    {
        static const MethodTable table =
        {
            { &Object::to_string_obj, "to_s" },
            { &Object::inspect_obj, "inspect" }
        };
        return table;
    }

    double as_number(const Object *obj)
    {
        auto n = dynamic_cast<const Number*>(obj);
        if (n) return n->get_value();
        else throw TypeError("Expected number, got " + obj->type_name());
    }

    const MethodTable &Boolean::method_table()const
    {
        static const MethodTable table(Object::method_table(),
        {
            { &Boolean::to_f, "to_f" },
            { &Boolean::to_f, "to_d" },
            { &Boolean::to_i, "to_i" }
        });
        return table;
    }
    const MethodTable &Nil::method_table()const
    {
        static const MethodTable table(Object::method_table(),
        {
            { &Nil::to_f, "to_f" },
            { &Nil::to_f, "to_d" },
            { &Nil::to_i, "to_i" }
        });
        return table;
    }

    ObjectPtr String::add(Object *rhs)
    {
        return make_value(v + coerce<String>(rhs)->get_value());
    }

    //to_f
    std::shared_ptr<Number> Nil::to_f()
    {
        return make_value(0.0);
    }
    std::shared_ptr<Number> Boolean::to_f()
    {
        return make_value(b ? 1.0 : 0.0);
    }

    //to_i
    std::shared_ptr<Number> Nil::to_i()
    {
        return make_value(0.0);
    }
    std::shared_ptr<Number> Boolean::to_i()
    {
        return make_value(b ? 1.0 : 0.0);
    }


}

