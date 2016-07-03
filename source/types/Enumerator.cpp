#include "types/Enumerator.hpp"

namespace slim
{
    const std::string Enumerator::TYPE_NAME = "Enumerator";
    

    std::shared_ptr<Object> Enumerator::each(const FunctionArgs & args)
    {
        return forward(forward_self.get(), args);
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
