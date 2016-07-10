#include "BuiltinFunctions.hpp"
#include "types/Array.hpp"
#include "types/Number.hpp"
#include "Maths.hpp"
namespace slim
{
    const FunctionTable BUILTIN_FUNCTIONS =
    {
        //Maths
        { acos, "acos" }, { asin, "asin" }, { atan, "atan" }, { atan2, "atan2" },
        { acosh, "acosh" }, { asinh, "asinh" }, { atanh, "atanh" },
        { cos, "cos" }, { sin, "sin" }, { tan, "tan" },
        { cosh, "cosh" }, { sinh, "sinh" }, { tanh, "tanh" },
        { cbrt, "cbrt" }, { sqrt, "sqrt" }, { exp, "exp" }, { log, "log" }, { log2, "log2" }, { log10, "log10" },
        { erf, "erf" }, { erfc, "erfc" }, { gamma, "gamma" }, { hypot, "hypot" }, { ldexp, "ldexp" }, { lgamma, "lgamma" }
    };
}
