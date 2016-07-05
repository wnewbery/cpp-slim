#include "types/Symbol.hpp"
#include "types/String.hpp"
#include <unordered_map>

namespace slim
{
    const std::string Symbol::TYPE_NAME = "Symbol";

    std::shared_ptr<Symbol> symbol(const std::string & str)
    {
        static std::unordered_map<std::string, std::shared_ptr<Symbol>> map;

        auto x = map.emplace(str, nullptr);
        if (x.second)
        {
            x.first->second.reset(new Symbol(make_value(str)));
        }

        return x.first->second;
    }
    std::shared_ptr<Symbol> symbol(std::shared_ptr<String> str)
    {
        return symbol(str->get_value());
    }

    Symbol::Symbol(std::shared_ptr<String> str) : str(str) {}
    Symbol::~Symbol() {}
    std::string Symbol::to_string() const
    {
        return str->to_string();
    }
    std::string Symbol::inspect() const
    {
        return ":" + str->to_string();
    }
    ObjectPtr Symbol::to_string_obj() const
    {
        return str;
    }
    size_t Symbol::hash() const
    {
        return std::hash<ObjectPtr>()(str);
    }
    int Symbol::cmp(const Object * rhs) const
    {
        return str->cmp(coerce<Symbol>(rhs)->str.get());
    }
    const char *Symbol::c_str()const
    {
        return str->get_value().c_str();
    }

    const MethodTable & Symbol::method_table() const
    {
        static const MethodTable table(Object::method_table(),
        {
        });
        return table;
    }
}
