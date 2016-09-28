#include "types/Enumerator.hpp"
#include "types/Array.hpp"
#include "types/Hash.hpp"
#include "types/Proc.hpp"
#include "Function.hpp"
#include <sstream>

namespace slim
{
    const MethodTable & Enumerator::method_table() const
    {
        static const MethodTable table = MethodTable(Object::method_table())
            .add_all(Enumerable::get_methods<Enumerator>())
            .add_all({
                {&Enumerator::with_index , "with_index"}
            });
        return table;
    }

    ObjectPtr Enumerator::with_index(const FunctionArgs &args)
    {
        Proc *proc = nullptr;
        int offset = 0;

        if (args.size() == 1)
        {
            proc = dynamic_cast<Proc*>(args[0].get());
            if (!proc) offset = (int)coerce<Number>(args[0])->get_value();
        }
        else if (args.size() == 2)
        {
            offset = (int)coerce<Number>(args[0])->get_value();
            proc = coerce<Proc>(args[1]).get();
        }
        else if (args.size() >= 3) throw ArgumentCountError(args.size(), 0, 2);

        if (proc)
        {
            int i = offset;
            return each_single({}, [&i, proc](Object *arg) {
                return proc->call({ arg->shared_from_this(), make_value(i++) });
            });
        }
        else
        {
            return make_enumerator(this, { &Enumerator::with_index, "with_index" }, {make_value(offset)});
        }
    }

    ObjectPtr MethodEnumerator::each(const FunctionArgs &args2)
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

    ObjectPtr FunctionEnumerator::each(const FunctionArgs &args2)
    {
        Proc *proc = nullptr;
        if (!args2.empty()) proc = dynamic_cast<Proc*>(args2.back().get());

        if (proc)
        {
            FunctionArgs all_args = args;
            all_args.insert(all_args.end(), args2.begin(), args2.end());
            return func(all_args);
        }
        else
        {
            return make_enumerator(this, { &Enumerator::each, "each" }, args2);
        }
    }
}
