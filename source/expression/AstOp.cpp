#include "expression/AstOp.hpp"
#include "expression/Scope.hpp"
#include "types/Object.hpp"
#include "types/String.hpp"
#include <sstream>
namespace slim
{
    namespace expr
    {
        std::string Literal::to_string() const
        {
            if (auto str = dynamic_cast<const String*>(value.get()))
            {
                return "\"" + value->to_string() + "\"";
            }
            else return value->to_string();
        }
        ObjectPtr Literal::eval(Scope & scope) const
        {
            return value;
        }

        ObjectPtr Variable::eval(Scope & scope) const
        {
            return scope.get(name);
        }

        std::string FuncCall::to_string() const
        {
            std::stringstream ss;
            ss << "(";
            if (!args.empty()) ss << args[0]->to_string();
            for (size_t i = 1; i < args.size(); ++i)
                ss << ", " << args[i]->to_string();
            ss << ")";
            return ss.str();
        }
        FunctionArgs FuncCall::eval_args(Scope & scope) const
        {
            FunctionArgs ret;
            for (auto &arg : args) ret.push_back(arg->eval(scope));
            return ret;
        }

        std::string GlobalFuncCall::to_string() const
        {
            return function.name + FuncCall::to_string();
        }
        ObjectPtr GlobalFuncCall::eval(Scope & scope) const
        {
            auto args = eval_args(scope);
            return function(args);
        }

        std::string MemberFuncCall::to_string() const
        {
            return "." + name + FuncCall::to_string();
        }
        ObjectPtr MemberFuncCall::eval(Scope & scope) const
        {
            auto self = lhs->eval(scope);
            auto args = eval_args(scope);
            //TODO: Implement member methods
            return NULL_VALUE;
        }
    }
}
