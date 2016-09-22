#include "types/Enumerator.hpp"
#include "types/Proc.hpp"
#include "Function.hpp"

namespace slim
{
    std::shared_ptr<Object> Enumerator::each(const FunctionArgs &args2)
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

    const MethodTable & Enumerator::method_table() const
    {
        static const MethodTable table(Object::method_table(),
        {
            { &Enumerator::each, "each" }
        });
        return table;
    }
}
