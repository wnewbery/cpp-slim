#pragma once
#include "types/Array.hpp"
#include "types/Nil.hpp"
#include "types/Object.hpp"
#include "Function.hpp"

/**Stores each call invocation.*/
class TestAccumulator : public slim::Object
{
public:
    static const std::string &name()
    {
        static const std::string TYPE_NAME = "TestAccumulator";
        return TYPE_NAME;
    }
    virtual const std::string& type_name()const override { return name(); }

    std::vector<slim::ObjectPtr> data;

    virtual std::string inspect()const override
    {
        return make_value(data)->inspect();
    }
    std::string check()
    {
        auto str = inspect();
        data.clear();
        return str;
    }

    virtual const slim::MethodTable &method_table()const override
    {
        static const slim::MethodTable table(slim::Object::method_table(),
        { { &TestAccumulator::store, "store" } });
        return table;
    }

    void store(const slim::FunctionArgs &args)
    {
        if (args.empty()) data.push_back(slim::NIL_VALUE);
        else if (args.size() == 1) data.push_back(args[0]);
        else data.push_back(make_value(args));
    }
};
