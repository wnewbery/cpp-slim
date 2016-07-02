#include "types/Object.hpp"
#include "types/Boolean.hpp"
#include "types/Null.hpp"
#include "types/Number.hpp"
#include "types/String.hpp"
#include "Error.hpp"

namespace slim
{
    const std::string Boolean::TYPE_NAME = "Boolean";
    const std::string Null::TYPE_NAME = "Null";
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

    ObjectPtr Object::mul(Object * rhs)
    {
        throw NoSuchMethod(this, "*");
    }
    ObjectPtr Object::div(Object * rhs)
    {
        throw NoSuchMethod(this, "/");
    }
    ObjectPtr Object::mod(Object * rhs)
    {
        throw NoSuchMethod(this, "%");
    }
    ObjectPtr Object::add(Object * rhs)
    {
        throw NoSuchMethod(this, "+");
    }
    ObjectPtr Object::sub(Object * rhs)
    {
        throw NoSuchMethod(this, "-");
    }
    ObjectPtr Object::negate()
    {
        throw NoSuchMethod(this, "-negate");
    }

    ObjectPtr Object::call_method(const std::string &name, const FunctionArgs &args)
    {
        auto &method = method_table().get(this, name);
        return method(this, args);
    }
    ObjectPtr Object::to_string_obj()const
    {
        return make_value(to_string());
    }
    const MethodTable &Object::method_table()const
    {
        static const MethodTable table =
        {
            { &Object::to_string_obj, "to_s" },
            { &Object::to_string_obj, "inspect" }
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
    const MethodTable &Null::method_table()const
    {
        static const MethodTable table(Object::method_table(),
        {
            { &Null::to_f, "to_f" },
            { &Null::to_f, "to_d" },
            { &Null::to_i, "to_i" }
        });
        return table;
    }
    const MethodTable &String::method_table()const
    {
        static const MethodTable table(Object::method_table(),
        {
            { &String::to_f, "to_f" },
            { &String::to_f, "to_d" },
            { &String::to_i, "to_i" }
        });
        return table;
    }

    ObjectPtr String::add(Object *rhs)
    {
        return make_value(v + coerce<String>(rhs)->get_value());
    }

    //to_f
    std::shared_ptr<Number> Null::to_f()
    {
        return make_value(0.0);
    }
    std::shared_ptr<Number> Boolean::to_f()
    {
        return make_value(b ? 1.0 : 0.0);
    }
    std::shared_ptr<Number> String::to_f()
    {
        double d = 0;
        try { d = std::stod(v.c_str()); }
        catch (const std::exception &) {}
        return make_value(d);
    }

    //to_i
    std::shared_ptr<Number> Null::to_i()
    {
        return make_value(0.0);
    }
    std::shared_ptr<Number> Boolean::to_i()
    {
        return make_value(b ? 1.0 : 0.0);
    }
    std::shared_ptr<Number> String::to_i()
    {
        int i = 0;
        try { i = std::stoi(v.c_str()); }
        catch (const std::exception &) {}
        return make_value((double)i);
    }


}

