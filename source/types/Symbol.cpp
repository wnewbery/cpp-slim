#include "types/Symbol.hpp"
#include "types/String.hpp"
#include "Function.hpp"
#include <unordered_map>

namespace slim
{
    const std::string Symbol::TYPE_NAME = "Symbol";

    SymPtr symbol(const std::string & str)
    {
        static std::unordered_map<std::string, std::shared_ptr<Symbol>> map;

        auto x = map.emplace(str, nullptr);
        if (x.second)
        {
            x.first->second.reset(new Symbol(make_value(str)));
        }

        return x.first->second;
    }
    SymPtr symbol(std::shared_ptr<String> str)
    {
        return symbol(str->get_value());
    }

    Symbol::Symbol(std::shared_ptr<String> str) : _str(str) {}
    Symbol::~Symbol() {}
    std::string Symbol::to_string() const
    {
        return _str->to_string();
    }
    std::string Symbol::inspect() const
    {
        return ":" + _str->to_string();
    }
    ObjectPtr Symbol::to_string_obj() const
    {
        return _str;
    }
    size_t Symbol::hash() const
    {
        return std::hash<ObjectPtr>()(_str);
    }
    int Symbol::cmp(const Object * rhs) const
    {
        return _str->cmp(coerce<Symbol>(rhs)->_str.get());
    }
    const std::string &Symbol::str()const
    {
        return _str->get_value();
    }
    const char *Symbol::c_str()const
    {
        return str().c_str();
    }

    const MethodTable & Symbol::method_table() const
    {
        static const MethodTable table(Object::method_table(),
        {
        });
        return table;
    }
}
