#include "types/Enumerable.hpp"
#include "types/Enumerator.hpp"
#include "types/Array.hpp"
#include "types/Hash.hpp"
#include "types/Proc.hpp"
#include "Function.hpp"
#include <sstream>

namespace slim
{
    ObjectPtr Enumerable::each2(
        const FunctionArgs &args,
        std::function<ObjectPtr(const FunctionArgs &args)> func)
    {
        auto args2 = args;
        args2.push_back(create_object<FunctionProc>(func));
        return each(args2);
    }

    Ptr<Boolean> Enumerable::all_q(const FunctionArgs &args)
    {
        Proc *proc = nullptr;
        try_unpack<0>(args, &proc);
        Ptr<Boolean> ret = TRUE_VALUE;
        if (proc)
        {
            each2({}, [&ret, proc](const FunctionArgs &args) {
                if(!proc->call(args)->is_true())
                {
                    ret = FALSE_VALUE;
                    throw BreakException();
                }
                else return NIL_VALUE;
            });
        }
        else
        {
            each2({}, [&ret](const FunctionArgs &args) {
                if (args.size() == 1 && !args[0]->is_true())
                {
                    ret = FALSE_VALUE;
                    throw BreakException();
                }
                else return NIL_VALUE;
            });
        }
        return ret;
    }

    Ptr<Boolean> Enumerable::any_q(const FunctionArgs &args)
    {
        Proc *proc = nullptr;
        try_unpack<0>(args, &proc);
        Ptr<Boolean> ret = FALSE_VALUE;
        if (proc)
        {
            each2({}, [&ret, proc](const FunctionArgs &args) {
                if (proc->call(args)->is_true())
                {
                    ret = TRUE_VALUE;
                    throw BreakException();
                }
                else return NIL_VALUE;
            });
        }
        else
        {
            each2({}, [&ret](const FunctionArgs &args) {
                if ((args.size() > 1 || args[0]->is_true()))
                {
                    ret = TRUE_VALUE;
                    throw BreakException();
                }
                else return NIL_VALUE;
            });
        }
        return ret;
    }

    Ptr<Array> Enumerable::to_a(const FunctionArgs &args)
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

    Ptr<Hash> Enumerable::to_h(const FunctionArgs &args)
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
}
