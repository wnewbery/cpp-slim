#pragma once
#include <functional>
#include <vector>
namespace slim
{
    class Object;
    typedef std::shared_ptr<Object> ObjectPtr;
    typedef std::vector<ObjectPtr> FunctionArgs;
    typedef std::function<ObjectPtr(const FunctionArgs&)> Function2;
    struct Function
    {
        std::string name;
        Function2 f;

        ObjectPtr operator()(const FunctionArgs &args)const
        {
            return f(args);
        }
    };
}
