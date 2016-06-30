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
        Parser::Parser(Lexer & lexer)
            : lexer(lexer), current_token(Token::END)
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
            return op1();
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
                    if (current_token.type == Token::LPAREN) throw std::runtime_error("Function calls not implemented");
                    else return std::make_unique<Variable>(name);
                }
            case Token::LPAREN: return sub_expression();
            default: throw SyntaxError("Expected value");
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

        ExpressionNodePtr Parser::op1()
        {
            auto lhs = op2();
            while (true)
            {
                switch(current_token.type)
                {
                case Token::LOGICAL_AND: next_binary_op<LogicalAnd>(lhs, &Parser::op2); break;
                case Token::LOGICAL_OR: next_binary_op<LogicalOr>(lhs, &Parser::op2); break;
                default: return lhs;
                }
            }
        }

        ExpressionNodePtr Parser::op2()
        {
            auto lhs = op3();
            while (true)
            {
                switch (current_token.type)
                {
                case Token::CMP_EQ: next_binary_op<Eq>(lhs, &Parser::op3); break;
                case Token::CMP_NE: next_binary_op<Ne>(lhs, &Parser::op3); break;
                default: return lhs;
                }
            }
        }

        ExpressionNodePtr Parser::op3()
        {
            auto lhs = op4();
            while (true)
            {
                switch (current_token.type)
                {
                case Token::CMP_LT: next_binary_op<Lt>(lhs, &Parser::op4); break;
                case Token::CMP_LE: next_binary_op<Le>(lhs, &Parser::op4); break;
                case Token::CMP_GT: next_binary_op<Gt>(lhs, &Parser::op4); break;
                case Token::CMP_GE: next_binary_op<Ge>(lhs, &Parser::op4); break;
                default: return lhs;
                }
            }
        }

        ExpressionNodePtr Parser::op4()
        {
            auto lhs = op5();
            while (true)
            {
                switch (current_token.type)
                {
                case Token::PLUS: next_binary_op<Add>(lhs, &Parser::op5); break;
                case Token::MINUS: next_binary_op<Sub>(lhs, &Parser::op5); break;
                default: return lhs;
                }
            }
        }

        ExpressionNodePtr Parser::op5()
        {
            auto lhs = op6();
            while (true)
            {
                switch (current_token.type)
                {
                case Token::MUL: next_binary_op<Mul>(lhs, &Parser::op6); break;
                case Token::DIV: next_binary_op<Div>(lhs, &Parser::op6); break;
                case Token::MOD: next_binary_op<Mod>(lhs, &Parser::op6); break;
                default: return lhs;
                }
            }
        }

        ExpressionNodePtr Parser::op6()
        {
            ExpressionNodePtr rhs;
            while (true)
            {
                switch (current_token.type)
                {
                case Token::PLUS:
                    next();
                    return op6();
                case Token::MINUS:
                    next();
                    rhs = op6();
                    return std::make_unique<Negative>(std::move(rhs));
                case Token::LOGICAL_NOT:
                    next();
                    rhs = op6();
                    return std::make_unique<LogicalNot>(std::move(rhs));
                default: return value();
                }
            }
        }
    }
}

