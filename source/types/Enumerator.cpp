#include "types/Enumerator.hpp"
#include "types/Array.hpp"
#include "types/Hash.hpp"
#include "types/Proc.hpp"
#include "Function.hpp"
#include <sstream>

namespace slim
{
    ObjectPtr Enumerator::each(const FunctionArgs &args2)
    {
        Proc *proc = nullptr;
        if (!args2.empty()) proc = dynamic_cast<Proc*>(args2.back().get());

        if (proc)
        {
            FunctionArgs all_args = args;
            all_args.insert(all_args.end(), args2.begin(), args2.end());
            return forward(forward_self.get(), all_args);
        }
        else
        {
            return make_enumerator(this, { &Enumerator::each, "each" }, args2);
        }
    }
    ObjectPtr Enumerator::each2(
        const FunctionArgs &args,
        std::function<ObjectPtr(const FunctionArgs &args)> func)
    {
        auto args2 = args;
        args2.push_back(create_object<FunctionProc>(func));
        return each(args2);
    }

    Ptr<Array> Enumerator::to_a(const FunctionArgs &args)
    {
        auto ret = create_object<Array>();
        each2(args, [ret](const FunctionArgs &args)
        {
            if (args.size() == 1) ret->push_back(args[0]);
            else ret->push_back(make_value(args));
            return NIL_VALUE;
        });
        return ret;
    }

    Ptr<Hash> Enumerator::to_h(const FunctionArgs &args)
    {
        auto ret = create_object<Hash>();
        each2(args, [ret](const FunctionArgs &args)
        {
            if (args.size() == 2) ret->set(args[0], args[1]);
            else if (args.size() == 1)
            {
                auto arr = coerce<Array>(args[0]);
                if (arr->get_value().size() == 2)
                {
                    ret->set(arr->get_value()[0], arr->get_value()[1]);
                }
                else
                {
                    std::stringstream ss;
                    ss << "wrong array length. expected 2, was " << arr->get_value().size();
                    throw ArgumentError(ss.str());
                }
            }
            else throw ArgumentCountError(args.size(), 2, 2);
            return NIL_VALUE;
        });
        return ret;
    }

    const MethodTable & Enumerator::method_table() const
    {
        static const MethodTable table(Object::method_table(),
        {
            { &Enumerator::each, "each" },
            { &Enumerator::to_a, "to_a" },
            { &Enumerator::to_h, "to_h" }
        });
        return table;
    }
}
