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

    Ptr<Number> Enumerable::count(const FunctionArgs &args)
    {
        unsigned count = 0;
        if (args.empty())
        {
            each2({}, [&count](const FunctionArgs &args) {
                return ++count, NIL_VALUE;
            });
        }
        else if (args.size() == 1)
        {
            auto proc = dynamic_cast<Proc*>(args[0].get());
            if (proc)
            {
                each2({}, [proc, &count](const FunctionArgs &args) {
                    if (proc->call(args)->is_true()) ++count;
                    return NIL_VALUE;
                });
            }
            else
            {
                auto itm = args[0].get();
                each2({}, [itm, &count](const FunctionArgs &args) {
                    if (itm->eq(args[0].get())) ++count;
                    return NIL_VALUE;
                });
            }
        }
        else throw ArgumentCountError(args.size(), 0, 1);
        return make_value(count);
    }

    ObjectPtr Enumerable::map(const FunctionArgs &args)
    {
        Proc *proc = nullptr;
        unpack<0>(args, &proc);
        if (proc)
        {
            try
            {
                auto ret = create_object<Array>();
                each2({}, [ret, proc](const FunctionArgs &args) {
                    ret->push_back(proc->call(args));
                    return NIL_VALUE;
                });
                return ret;
            }
            catch (const BreakException &e) { return e.value; }
        }
        else return make_enumerator(this, &Enumerable::map, "map");
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
