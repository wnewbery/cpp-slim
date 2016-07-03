#include "expression/Parser.hpp"
#include "expression/Lexer.hpp"
#include "expression/Token.hpp"
#include "expression/Ast.hpp"

#include "expression/AstOp.hpp"
#include "expression/ArithmeticOp.hpp"
#include "expression/CmpOp.hpp"
#include "expression/LogicalOp.hpp"

#include "types/Boolean.hpp"
#include "types/Null.hpp"
#include "types/Number.hpp"
#include "types/String.hpp"

#include "Error.hpp"

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
        Parser::Parser(const FunctionTable &global_functions, Lexer &lexer)
            : global_functions(global_functions), lexer(lexer), current_token(Token::END)
        {
            
        }

        ExpressionNodePtr Parser::parse_expression()
        {
            next();
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
            return logical_or_op();
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
        ExpressionNodePtr Parser::value()
        {
            auto lit = [this](ObjectPtr val) { return next(), std::make_unique<Literal>(val); };
            switch (current_token.type)
            {
            case Token::STRING: return lit(make_value(current_token.str));
            case Token::NUMBER: return lit(make_value(parse_num(current_token.str)));
            case Token::SYMBOL:
                if (current_token.str == "true") return lit(TRUE_VALUE);
                else if (current_token.str == "false") return lit(FALSE_VALUE);
                else if (current_token.str == "null") return lit(NULL_VALUE);
                else
                {
                    auto name = current_token.str;
                    next();
                    if (current_token.type == Token::LPAREN || is_func_arg_start())
                    {
                        auto &f = global_functions.get(name);
                        FuncCall::Args args = func_args();
                        return std::make_unique<GlobalFuncCall>(f, std::move(args));
                    }
                    else return std::make_unique<Variable>(name);
                }
            case Token::LPAREN: return sub_expression();
            case Token::L_SQ_BRACKET: return array_literal();
            case Token::L_CURLY_BRACKET: return hash_literal();
            default: throw SyntaxError("Expected value");
            }
        }

        ExpressionNodePtr Parser::array_literal()
        {
            assert(current_token.type == Token::L_SQ_BRACKET);
            next();
            if (current_token.type == Token::R_SQ_BRACKET)
            {
                next();
                return std::make_unique<ArrayLiteral>(FuncCall::Args());
            }
            
            FuncCall::Args args;
            while (true)
            {
                args.push_back(expression());
                if (current_token.type == Token::COMMA) next();
                else if (current_token.type == Token::R_SQ_BRACKET)
                {
                    next();
                    return std::make_unique<ArrayLiteral>(std::move(args));
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
                return std::make_unique<HashLiteral>(FuncCall::Args());
            }

            FuncCall::Args args;
            while (true)
            {
                //key_symbol: or key_expr =>
                if (current_token.type == Token::HASH_SYMBOL)
                {
                    args.push_back(std::make_unique<Literal>(make_value(current_token.str)));
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
                    return std::make_unique<HashLiteral>(std::move(args));
                }
                else throw SyntaxError("Expected '}'");
            }
        }

        bool Parser::is_func_arg_start()const
        {
            switch (current_token.type)
            {
            case Token::STRING:
            case Token::NUMBER:
            case Token::SYMBOL:
                return true;
            default:
                return false;
            }
        }
        std::vector<ExpressionNodePtr> Parser::func_args()
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
            else if (!is_func_arg_start()) return {};
            else parens = false;

            auto args = func_args_inner();

            if (parens)
            {
                if (current_token.type != Token::RPAREN)
                    throw SyntaxError("Expected ')'");
                next();
            }

            return args;
        }
        std::vector<ExpressionNodePtr> Parser::func_args_inner()
        {
            std::vector<ExpressionNodePtr> args;
            while (true)
            {
                args.push_back(expression());
                if (current_token.type == Token::COMMA) next();
                else return args;
            }
        }

        template<class T> ExpressionNodePtr Parser::binary_op(ExpressionNodePtr &&lhs)
        {
            next();
            auto rhs = op2();
            return std::make_unique<LogicalAnd>(std::move(lhs), std::move(rhs));
        }

        template<class T, class U>
        void Parser::next_binary_op(ExpressionNodePtr &lhs, U get_rhs)
        {
            next();
            auto rhs = (this->*get_rhs)();
            lhs = std::make_unique<T>(std::move(lhs), std::move(rhs));
        }

        ExpressionNodePtr Parser::logical_or_op()
        {
            auto lhs = logical_and_op();
            while (true)
            {
                switch (current_token.type)
                {
                case Token::LOGICAL_OR: next_binary_op<LogicalOr>(lhs, &Parser::logical_and_op); break;
                default: return lhs;
                }
            }
        }

        ExpressionNodePtr Parser::logical_and_op()
        {
            auto lhs = equality_op();
            while (true)
            {
                switch (current_token.type)
                {
                case Token::LOGICAL_AND: next_binary_op<LogicalAnd>(lhs, &Parser::equality_op); break;
                default: return lhs;
                }
            }
        }

        ExpressionNodePtr Parser::equality_op()
        {
            auto lhs = cmp_op();
            while (true)
            {
                switch (current_token.type)
                {
                case Token::CMP: next_binary_op<Cmp>(lhs, &Parser::cmp_op); break;
                case Token::CMP_EQ: next_binary_op<Eq>(lhs, &Parser::cmp_op); break;
                case Token::CMP_NE: next_binary_op<Ne>(lhs, &Parser::cmp_op); break;
                default: return lhs;
                }
            }
        }

        ExpressionNodePtr Parser::cmp_op()
        {
            auto lhs = add_op();
            while (true)
            {
                switch (current_token.type)
                {
                case Token::CMP_LT: next_binary_op<Lt>(lhs, &Parser::add_op); break;
                case Token::CMP_LE: next_binary_op<Le>(lhs, &Parser::add_op); break;
                case Token::CMP_GT: next_binary_op<Gt>(lhs, &Parser::add_op); break;
                case Token::CMP_GE: next_binary_op<Ge>(lhs, &Parser::add_op); break;
                default: return lhs;
                }
            }
        }

        ExpressionNodePtr Parser::add_op()
        {
            auto lhs = mul_op();
            while (true)
            {
                switch (current_token.type)
                {
                case Token::PLUS: next_binary_op<Add>(lhs, &Parser::mul_op); break;
                case Token::MINUS: next_binary_op<Sub>(lhs, &Parser::mul_op); break;
                default: return lhs;
                }
            }
        }

        ExpressionNodePtr Parser::mul_op()
        {
            auto lhs = unary_op();
            while (true)
            {
                switch (current_token.type)
                {
                case Token::MUL: next_binary_op<Mul>(lhs, &Parser::unary_op); break;
                case Token::DIV: next_binary_op<Div>(lhs, &Parser::unary_op); break;
                case Token::MOD: next_binary_op<Mod>(lhs, &Parser::unary_op); break;
                default: return lhs;
                }
            }
        }

        ExpressionNodePtr Parser::unary_op()
        {
            ExpressionNodePtr rhs;
            while (true)
            {
                switch (current_token.type)
                {
                case Token::PLUS:
                    next();
                    return unary_op();
                case Token::MINUS:
                    next();
                    rhs = unary_op();
                    return std::make_unique<Negative>(std::move(rhs));
                case Token::LOGICAL_NOT:
                    next();
                    rhs = unary_op();
                    return std::make_unique<LogicalNot>(std::move(rhs));
                default: return member_func();
                }
            }
        }

        ExpressionNodePtr Parser::member_func()
        {
            auto lhs = value();
            while (true)
            {
                if (current_token.type == Token::DOT)
                {
                    next();
                    if (current_token.type != Token::SYMBOL) throw SyntaxError("Expected symbol");
                    auto name = current_token.str;

                    next();
                    auto args = func_args();
                    lhs = std::make_unique<MemberFuncCall>(std::move(lhs), std::move(name), std::move(args));
                }
                else if (current_token.type == Token::L_SQ_BRACKET)
                {
                    next();
                    FuncCall::Args args = func_args_inner();
                    if (current_token.type != Token::R_SQ_BRACKET)
                        throw SyntaxError("Expected ']'");

                    next();
                    lhs = std::make_unique<ElementRefOp>(std::move(lhs), std::move(args));
                }
                else break;
            }
            return lhs;
        }
    }
}

