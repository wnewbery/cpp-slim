#include "expression/Parser.hpp"
#include "expression/Lexer.hpp"
#include "expression/Token.hpp"
#include "expression/Ast.hpp"

#include "expression/AstOp.hpp"
#include "expression/ArithmeticOp.hpp"
#include "expression/CmpOp.hpp"
#include "expression/LogicalOp.hpp"
#include "expression/Scope.hpp"

#include "types/Boolean.hpp"
#include "types/Nil.hpp"
#include "types/Number.hpp"
#include "types/String.hpp"
#include "types/Symbol.hpp"

#include "Error.hpp"
#include "Util.hpp"

#include <cassert>

namespace slim
{
    namespace expr
    {
        namespace
        {
            double parse_num(const std::string &str)
            {
                size_t count = 0;
                double ret;
                try
                {
                    ret = std::stod(str, &count);
                }
                catch (const std::exception &) { count = 0; }

                if (!count || count != str.size()) throw SyntaxError("Invalid number: " + str);
                return ret;
            }
        }
        Parser::Parser(const LocalVarNames &vars, Lexer &lexer)
            : lexer(lexer), current_token(nullptr, Token::END), vars(vars)
        {
            next();
        }

        ExpressionNodePtr Parser::full_expression()
        {
            auto ret = expression();
            if (current_token.type != Token::END) throw SyntaxError("Expected end");
            return ret;
        }
        void Parser::next()
        {
            current_token = lexer.next();
        }
        ExpressionNodePtr Parser::expression()
        {
            return conditional_op();
        }

        std::vector<SymPtr> Parser::param_list()
        {
            std::vector<SymPtr> out;
            if (current_token.type == Token::LOGICAL_OR)
            {   //Lexer picked up "||" as logical or operator, but it is also an empty param lsit
                next();
            }
            else if (current_token.type == Token::OR)
            {   //Have "|", may have "| |" or, one or more params
                next();
                if (current_token.type == Token::OR)
                {   //Had "| |"
                    next();
                }
                else
                {
                    while (true)
                    {
                        if (current_token.type != Token::SYMBOL) throw SyntaxError("Expected symbol");
                        out.push_back(symbol(current_token.str));
                        next();

                        if (current_token.type == Token::OR)
                        {
                            next();
                            break;
                        }
                        else if (current_token.type == Token::COMMA)
                        {
                            next();
                            continue;
                        }
                        else throw SyntaxError("Expected ',' or '|'");
                    }
                }
            }
            //else no list, no params
            return out;
        }

        ExpressionNodePtr Parser::sub_expression()
        {
            if (current_token.type != Token::LPAREN) throw SyntaxError("Expected '('");
            next();
            auto ret = expression();
            if (current_token.type != Token::RPAREN) throw SyntaxError("Expected ')'");
            next();
            return ret;
        }
        ExpressionNodePtr Parser::value(bool in_cond_op)
        {
            auto lit = [this](ObjectPtr val) { return next(), slim::make_unique<Literal>(val); };
            switch (current_token.type)
            {
            case Token::STRING_DELIM: return interp_string();
            case Token::NUMBER: return lit(make_value(parse_num(current_token.str)));
            case Token::SYMBOL:
                if (current_token.str == "true") return lit(TRUE_VALUE);
                else if (current_token.str == "false") return lit(FALSE_VALUE);
                else if (current_token.str == "nil") return lit(NIL_VALUE);
                else
                {
                    auto name = current_token.str;
                    next();
                    if (current_token.type == Token::LPAREN ||
                        current_token.type == Token::L_CURLY_BRACKET ||
                        (!in_cond_op && is_func_arg_start()))
                    {   //local variables are not callable, so must be method
                        FuncCall::Args args = func_args(false);
                        return slim::make_unique<GlobalFuncCall>(symbol(name), std::move(args));
                    }
                    else if (vars.is_var(name))
                    {   //variables take priority over methods if they exist
                        return slim::make_unique<Variable>(symbol(name));
                    }
                    else
                    {   //method call with no args
                        return slim::make_unique<GlobalFuncCall>(symbol(name), FuncCall::Args());
                    }
                }
            case Token::ATTR_NAME:
            {
                auto name = current_token.str;
                next();
                return slim::make_unique<Attribute>(symbol(name));
            }
            case Token::LPAREN: return sub_expression();
            case Token::L_SQ_BRACKET: return array_literal();
            case Token::L_CURLY_BRACKET: return hash_literal();
            case Token::COLON:
                next();
                if (current_token.type != Token::SYMBOL) throw SyntaxError("Expected symbol");
                return lit(symbol(current_token.str));
            default: throw SyntaxError("Expected value");
            }
        }

        ExpressionNodePtr Parser::interp_string()
        {
            assert(current_token.type == Token::STRING_DELIM);
            InterpolatedString::Nodes parts;
            bool interp = false;
            char delim = current_token.str[0];

            do
            {
                current_token = lexer.next_str_interp(delim);
                switch (current_token.type)
                {
                case Token::STRING_DELIM: break;
                case Token::STRING_TEXT:
                    parts.emplace_back(current_token.str);
                    break;
                case Token::STRING_INTERP_START:
                {
                    interp = true;
                    next();
                    auto expr = expression();
                    if (current_token.type != Token::R_CURLY_BRACKET) throw SyntaxError("Expected '}'");
                    parts.emplace_back(std::move(expr));
                    break;
                }
                default: throw SyntaxError("Unexpected token in interpolated string");
                }
            }
            while (current_token.type != Token::STRING_DELIM);
            next();

            if (parts.empty()) return slim::make_unique<Literal>(make_value(""));

            if (!interp)
            {
                //single string text
                assert(parts.size() <= 1);
                assert(!parts[0].expr);
                return slim::make_unique<Literal>(make_value(parts[0].literal_text));
            }

            return slim::make_unique<InterpolatedString>(std::move(parts));
        }

        ExpressionNodePtr Parser::array_literal()
        {
            assert(current_token.type == Token::L_SQ_BRACKET);
            next();
            if (current_token.type == Token::R_SQ_BRACKET)
            {
                next();
                return slim::make_unique<ArrayLiteral>(FuncCall::Args());
            }
            
            FuncCall::Args args;
            while (true)
            {
                args.push_back(expression());
                if (current_token.type == Token::COMMA) next();
                else if (current_token.type == Token::R_SQ_BRACKET)
                {
                    next();
                    return slim::make_unique<ArrayLiteral>(std::move(args));
                }
                else throw SyntaxError("Expected ']'");
            }
        }

        ExpressionNodePtr Parser::hash_literal()
        {
            assert(current_token.type == Token::L_CURLY_BRACKET);
            next();
            if (current_token.type == Token::R_CURLY_BRACKET)
            {
                next();
                return slim::make_unique<HashLiteral>(FuncCall::Args());
            }

            FuncCall::Args args;
            while (true)
            {
                //key_symbol: or key_expr =>
                if (current_token.type == Token::HASH_SYMBOL)
                {
                    args.push_back(slim::make_unique<Literal>(symbol(current_token.str)));
                    next();
                }
                else
                {
                    args.push_back(expression());
                    if (current_token.type != Token::HASH_KEY_VALUE_SEP) throw SyntaxError("Expected =>");
                    next();
                }
                //value
                args.push_back(expression());

                if (current_token.type == Token::COMMA) next();
                else if (current_token.type == Token::R_CURLY_BRACKET)
                {
                    next();
                    return slim::make_unique<HashLiteral>(std::move(args));
                }
                else throw SyntaxError("Expected '}'");
            }
        }

        bool Parser::is_func_arg_start()const
        {
            switch (current_token.type)
            {
            case Token::COLON: // assuming :symbol
            case Token::HASH_SYMBOL:
            case Token::NUMBER:
            case Token::STRING_DELIM:
            case Token::SYMBOL:
            case Token::ATTR_NAME:
                return true;
            default:
                return false;
            }
        }
        std::vector<ExpressionNodePtr> Parser::func_args(bool in_cond_op)
        {
            bool parens;
            if (current_token.type == Token::LPAREN)
            {
                parens = true;
                next();
                if (current_token.type == Token::RPAREN)
                {
                    next();
                    return {};
                }
            }
            else if (in_cond_op)
            {
                throw SyntaxError(
                    "Function calls within a conditional operators right side "
                    "expression must use parenthesis");
            }
            else parens = false;

            FuncCall::Args args;
            if (parens || is_func_arg_start())
            {
                args = func_args_inner();
                if (parens) 
                {
                    if (current_token.type != Token::RPAREN)
                        throw SyntaxError("Expected ')'");
                    next();
                }
            }

            if (current_token.type == Token::L_CURLY_BRACKET)
                args.push_back(block());

            return args;
        }
        std::vector<ExpressionNodePtr> Parser::func_args_inner()
        {
            std::vector<ExpressionNodePtr> args;
            while (true)
            {
                if (current_token.type == Token::HASH_SYMBOL)
                {
                    auto key = slim::make_unique<Literal>(symbol(current_token.str));
                    next();
                    args.push_back(func_hash_args_inner(std::move(key)));
                    return args;
                }
                auto expr = expression();
                if (current_token.type == Token::HASH_KEY_VALUE_SEP)
                {
                    next();
                    args.push_back(func_hash_args_inner(std::move(expr)));
                    return args;
                }

                args.push_back(std::move(expr));
                if (current_token.type == Token::COMMA) next();
                else return args;
            }
        }
        ExpressionNodePtr Parser::func_hash_args_inner(ExpressionNodePtr &&first_key)
        {
            FuncCall::Args args;
            assert(first_key);
            
            args.push_back(std::move(first_key));
            args.push_back(expression());

            while (current_token.type == Token::COMMA)
            {
                next();
                //key_symbol: or key_expr =>
                if (current_token.type == Token::HASH_SYMBOL)
                {
                    args.push_back(slim::make_unique<Literal>(symbol(current_token.str)));
                    next();
                }
                else
                {
                    args.push_back(expression());
                    if (current_token.type != Token::HASH_KEY_VALUE_SEP) throw SyntaxError("Expected =>");
                    next();
                }
                //value
                args.push_back(expression());
            }
            return slim::make_unique<HashLiteral>(std::move(args));
        }

        ExpressionNodePtr Parser::block()
        {
            assert(current_token.type == Token::L_CURLY_BRACKET);
            next();

            std::vector<SymPtr> params = param_list();
            auto old_vars = vars; //save the current variable set, new variables will only exist within the block
            for (auto &param : params)
                vars.add(param->str());

            auto expr = expression();

            if (current_token.type != Token::R_CURLY_BRACKET) throw SyntaxError("Expected '}'");
            next();

            vars = old_vars; //Remove any variables from within the block
            return slim::make_unique<Block>(std::move(params), std::move(expr));
        }

        template<class T, class U>
        void Parser::next_binary_op(bool in_cond_op, ExpressionNodePtr &lhs, U get_rhs)
        {
            next();
            auto rhs = (this->*get_rhs)(in_cond_op);
            lhs = slim::make_unique<T>(std::move(lhs), std::move(rhs));
        }

        ExpressionNodePtr Parser::conditional_op()
        {
            auto lhs = logical_or_op(false);
            if (current_token.type == Token::CONDITIONAL)
            {
                next();
                auto true_expr = logical_or_op(true);

                if (current_token.type != Token::COLON) throw SyntaxError("Expected ':'");
                next();

                auto false_expr = conditional_op();

                lhs = slim::make_unique<Conditional>(std::move(lhs), std::move(true_expr), std::move(false_expr));
            }
            return lhs;
        }

        ExpressionNodePtr Parser::logical_or_op(bool in_cond_op)
        {
            auto lhs = logical_and_op(in_cond_op);
            while (true)
            {
                switch (current_token.type)
                {
                case Token::LOGICAL_OR: next_binary_op<LogicalOr>(in_cond_op, lhs, &Parser::logical_and_op); break;
                default: return lhs;
                }
            }
        }

        ExpressionNodePtr Parser::logical_and_op(bool in_cond_op)
        {
            auto lhs = equality_op(in_cond_op);
            while (true)
            {
                switch (current_token.type)
                {
                case Token::LOGICAL_AND: next_binary_op<LogicalAnd>(in_cond_op, lhs, &Parser::equality_op); break;
                default: return lhs;
                }
            }
        }

        ExpressionNodePtr Parser::equality_op(bool in_cond_op)
        {
            auto lhs = cmp_op(in_cond_op);
            while (true)
            {
                switch (current_token.type)
                {
                case Token::CMP: next_binary_op<Cmp>(in_cond_op, lhs, &Parser::cmp_op); break;
                case Token::CMP_EQ: next_binary_op<Eq>(in_cond_op, lhs, &Parser::cmp_op); break;
                case Token::CMP_NE: next_binary_op<Ne>(in_cond_op, lhs, &Parser::cmp_op); break;
                default: return lhs;
                }
            }
        }

        ExpressionNodePtr Parser::cmp_op(bool in_cond_op)
        {
            auto lhs = bitor_op(in_cond_op);
            while (true)
            {
                switch (current_token.type)
                {
                case Token::CMP_LT: next_binary_op<Lt>(in_cond_op, lhs, &Parser::bitor_op); break;
                case Token::CMP_LE: next_binary_op<Le>(in_cond_op, lhs, &Parser::bitor_op); break;
                case Token::CMP_GT: next_binary_op<Gt>(in_cond_op, lhs, &Parser::bitor_op); break;
                case Token::CMP_GE: next_binary_op<Ge>(in_cond_op, lhs, &Parser::bitor_op); break;
                default: return lhs;
                }
            }
        }

        ExpressionNodePtr Parser::bitor_op(bool in_cond_op)
        {
            auto lhs = bitand_op(in_cond_op);
            while (true)
            {
                switch (current_token.type)
                {
                case Token::XOR: next_binary_op<Xor>(in_cond_op, lhs, &Parser::bitand_op); break;
                case Token::OR: next_binary_op<Or>(in_cond_op, lhs, &Parser::bitand_op); break;
                default: return lhs;
                }
            }
        }
        ExpressionNodePtr Parser::bitand_op(bool in_cond_op)
        {
            auto lhs = bitshift_op(in_cond_op);
            while (true)
            {
                switch (current_token.type)
                {
                case Token::AND: next_binary_op<And>(in_cond_op, lhs, &Parser::bitshift_op); break;
                default: return lhs;
                }
            }
        }
        ExpressionNodePtr Parser::bitshift_op(bool in_cond_op)
        {
            auto lhs = add_op(in_cond_op);
            while (true)
            {
                switch (current_token.type)
                {
                case Token::LSHIFT: next_binary_op<Lshift>(in_cond_op, lhs, &Parser::add_op); break;
                case Token::RSHIFT: next_binary_op<Rshift>(in_cond_op, lhs, &Parser::add_op); break;
                default: return lhs;
                }
            }
        }

        ExpressionNodePtr Parser::add_op(bool in_cond_op)
        {
            auto lhs = mul_op(in_cond_op);
            while (true)
            {
                switch (current_token.type)
                {
                case Token::PLUS: next_binary_op<Add>(in_cond_op, lhs, &Parser::mul_op); break;
                case Token::MINUS: next_binary_op<Sub>(in_cond_op, lhs, &Parser::mul_op); break;
                default: return lhs;
                }
            }
        }

        ExpressionNodePtr Parser::mul_op(bool in_cond_op)
        {
            auto lhs = unary_op(in_cond_op);
            while (true)
            {
                switch (current_token.type)
                {
                case Token::MUL: next_binary_op<Mul>(in_cond_op, lhs, &Parser::unary_op); break;
                case Token::DIV: next_binary_op<Div>(in_cond_op, lhs, &Parser::unary_op); break;
                case Token::MOD: next_binary_op<Mod>(in_cond_op, lhs, &Parser::unary_op); break;
                default: return lhs;
                }
            }
        }

        ExpressionNodePtr Parser::unary_op(bool in_cond_op)
        {
            ExpressionNodePtr rhs;
            while (true)
            {
                switch (current_token.type)
                {
                case Token::PLUS:
                    next();
                    return unary_op(in_cond_op);
                case Token::MINUS:
                    next();
                    rhs = unary_op(in_cond_op);
                    return slim::make_unique<Negative>(std::move(rhs));
                case Token::NOT:
                    next();
                    rhs = unary_op(in_cond_op);
                    return slim::make_unique<Not>(std::move(rhs));
                case Token::LOGICAL_NOT:
                    next();
                    rhs = unary_op(in_cond_op);
                    return slim::make_unique<LogicalNot>(std::move(rhs));
                default: return pow_op(in_cond_op);
                }
            }
        }

        ExpressionNodePtr Parser::pow_op(bool in_cond_op)
        {
            auto lhs = member_func(in_cond_op);
            while (current_token.type == Token::POW)
            {
                next_binary_op<Pow>(in_cond_op, lhs, &Parser::member_func); break;
            }
            return lhs;
        }

        ExpressionNodePtr Parser::member_func(bool in_cond_op)
        {
            auto lhs = value(in_cond_op);
            while (true)
            {
                if (current_token.type == Token::DOT)
                {
                    next();
                    if (current_token.type != Token::SYMBOL) throw SyntaxError("Expected symbol");
                    auto name = symbol(current_token.str);

                    next();
                    auto args = func_args(in_cond_op);
                    lhs = slim::make_unique<MemberFuncCall>(std::move(lhs), std::move(name), std::move(args));
                }
                else if (current_token.type == Token::SAFE_NAV)
                {
                    next();
                    if (current_token.type != Token::SYMBOL) throw SyntaxError("Expected symbol");
                    auto name = symbol(current_token.str);

                    next();
                    auto args = func_args(in_cond_op);
                    lhs = slim::make_unique<SafeNavMemberFuncCall>(std::move(lhs), std::move(name), std::move(args));
                }
                else if (current_token.type == Token::L_SQ_BRACKET)
                {
                    next();
                    FuncCall::Args args = func_args_inner();
                    if (current_token.type != Token::R_SQ_BRACKET)
                        throw SyntaxError("Expected ']'");

                    next();
                    lhs = slim::make_unique<ElementRefOp>(std::move(lhs), std::move(args));
                }
                else break;
            }
            return lhs;
        }
    }
}

