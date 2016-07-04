#include "expression/AstOp.hpp"
#include "expression/Scope.hpp"
#include "types/Object.hpp"
#include "types/Array.hpp"
#include "types/Hash.hpp"
#include "types/String.hpp"
#include "types/Proc.hpp"
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
            if (!args.empty()) ss << args[0]->to_string();
            for (size_t i = 1; i < args.size(); ++i)
                ss << ", " << args[i]->to_string();
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
            return function.name + "(" + FuncCall::to_string() + ")";
        }
        ObjectPtr GlobalFuncCall::eval(Scope & scope) const
        {
            auto args = eval_args(scope);
            return function(args);
        }

        std::string MemberFuncCall::to_string() const
        {
            return lhs->to_string() + "." + name + "(" + FuncCall::to_string() + ")";
        }
        ObjectPtr MemberFuncCall::eval(Scope & scope) const
        {
            auto self = lhs->eval(scope);
            auto args = eval_args(scope);
            return self->call_method(name, args);
        }

        std::string SafeNavMemberFuncCall::to_string() const
        {
            return lhs->to_string() + "&." + name + "(" + FuncCall::to_string() + ")";
        }
        ObjectPtr SafeNavMemberFuncCall::eval(Scope & scope) const
        {
            auto self = lhs->eval(scope);
            if (self == NIL_VALUE) return NIL_VALUE;
            else
            {
                auto args = eval_args(scope);
                return self->call_method(name, args);
            }
        }

        std::string ElementRefOp::to_string() const
        {
            return lhs->to_string() + "[" + FuncCall::to_string() + "]";
        }
        ObjectPtr ElementRefOp::eval(Scope & scope) const
        {
            auto self = lhs->eval(scope);
            auto args = eval_args(scope);
            return self->el_ref(args);
        }
        std::string ArrayLiteral::to_string() const
        {
            return "[" + FuncCall::to_string() + "]";
        }
        ObjectPtr ArrayLiteral::eval(Scope & scope) const
        {
            auto args = eval_args(scope);
            return make_array(args);
        }
        std::string HashLiteral::to_string() const
        {
            std::stringstream ss;
            ss << "{";
            for (auto i = args.begin(); i != args.end();)
            {
                if (i != args.begin()) ss << ", ";
                ss << (*i++)->to_string();
                ss << " => ";
                ss << (*i++)->to_string();
            }
            ss << "}";
            return ss.str();
        }
        ObjectPtr HashLiteral::eval(Scope & scope) const
        {
            auto args = eval_args(scope);
            return make_hash(args);
        }

        std::string Block::to_string() const
        {
            std::stringstream ss;
            ss << "{|";
            if (!param_names.empty()) ss << param_names[0];
            for (size_t i = 1; i < param_names.size(); ++i)
                ss << ", " << param_names[i];
            ss << "| ";
            ss << code->to_string();
            ss << "}";
            return ss.str();
        }
        ObjectPtr Block::eval(Scope & scope) const
        {
            return std::make_shared<Proc>(*code, param_names, scope);
        }

        std::string Conditional::to_string() const
        {
            return "(" + cond->to_string() + " ? " + true_expr->to_string() + " : " + false_expr->to_string() + ")";
        }
        ObjectPtr Conditional::eval(Scope & scope) const
        {
            if (cond->eval(scope)->is_true())
            {
                return true_expr->eval(scope);
            }
            else
            {
                return false_expr->eval(scope);
            }
        }
    }
}
