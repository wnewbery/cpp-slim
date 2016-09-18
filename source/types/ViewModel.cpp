#include "types/ViewModel.hpp"
#include "types/HtmlSafeString.hpp"
#include "types/Nil.hpp"
#include "types/Proc.hpp"
#include "types/Symbol.hpp"
#include "Function.hpp"
#include "FunctionHelpers.hpp"

namespace slim
{
    const std::string ViewModel::TYPE_NAME = "ViewModel";

    ViewModel::ViewModel() {}
    ViewModel::~ViewModel() {}

    const MethodTable &ViewModel::method_table()const
    {
        static const MethodTable table(Object::method_table(),
        {
            { &ViewModel::content_for, "content_for" },
            { &ViewModel::yield, "yield" }
        });
        return table;
    }


    ObjectPtr ViewModel::get_constant(SymPtr name)
    {
        auto it = constants.find(name);
        if (it != constants.end()) return it->second;
        else throw NoConstantError(this, name);
    }

    void ViewModel::add_constant(SymPtr name, ObjectPtr constant)
    {
        constants[name] = constant;
    }
    void ViewModel::add_constant(const std::string &name, ObjectPtr constant)
    {
        add_constant(symbol(name), constant);
    }


    ObjectPtr ViewModel::get_attr(SymPtr name)
    {
        auto it = attrs.find(name);
        return it != attrs.end() ? it->second : NIL_VALUE;
    }

    void ViewModel::set_attr(SymPtr name, ObjectPtr value)
    {
        attrs[name] = value;
    }
    void ViewModel::set_attr(const std::string &name, ObjectPtr value)
    {
        set_attr(symbol(name), value);
    }

    void ViewModel::content_for(SymPtr name, std::shared_ptr<Proc> proc)
    {
        content_for_store[name] = proc->call({});
    }

    std::shared_ptr<HtmlSafeString> ViewModel::yield(const FunctionArgs &args)
    {
        SymPtr name;
        unpack<0>(args, &name);
        if (!name)
        {
            return main_content;
        }
        else
        {
            auto it = content_for_store.find(name);
            if (it != content_for_store.end()) return coerce<HtmlSafeString>(it->second);
            else return create_object<HtmlSafeString>();
        }
    }
    void ViewModel::set_main_content(std::shared_ptr<HtmlSafeString> content)
    {
        main_content = content;
    }
}
