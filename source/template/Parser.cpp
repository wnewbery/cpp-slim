#include "template/Parser.hpp"
#include "template/Lexer.hpp"
#include "template/Token.hpp"
#include "template/TemplateParts.hpp"
#include "template/Attributes.hpp"
#include "expression/Lexer.hpp"
#include "expression/Parser.hpp"
#include "expression/AstOp.hpp"
#include "types/Boolean.hpp"
#include "types/Nil.hpp"
#include "BuiltinFunctions.hpp"
#include "Error.hpp"
#include "Util.hpp"
#include <cassert>
#include <unordered_set>

namespace slim
{
    namespace tpl
    {
        namespace
        {
            const std::unordered_set<std::string> VOID_ELEMENTS =
            {
                "area", "base", "br", "col", "command", "embed", "hr", "img", "input", "keygen",
                "link", "meta", "param", "source", "track", "wbr"
            };
            const std::string DEF_TAG = "div";
        }

        Parser::OutputFrame& Parser::OutputFrame::operator << (const std::string &text_content)
        {
            handle_in_tag();
            this->text_content += text_content;
            return *this;
        }
        Parser::OutputFrame& Parser::OutputFrame::operator << (char txt_chr)
        {
            handle_in_tag();
            this->text_content += txt_chr;
            return *this;
        }

        Parser::OutputFrame& Parser::OutputFrame::operator << (std::unique_ptr<expr::ExpressionNode> &&expr)
        {
            return *this << slim::make_unique<TemplateOutputExpr>(std::move(expr));
        }

        Parser::OutputFrame& Parser::OutputFrame::operator << (std::unique_ptr<TemplatePart> &&part)
        {
            handle_in_tag();
            if (text_content.size())
            {
                contents.push_back(slim::make_unique<TemplateText>(std::move(text_content)));
                text_content.clear();
            }
            contents.push_back(std::move(part));
            return *this;
        }
        
        void Parser::OutputFrame::handle_in_tag()
        {
            if (in_tag())
            {
                set_in_tag(false);
                this->text_content += '>';
            }
        }

        std::unique_ptr<TemplatePart> Parser::OutputFrame::make_tpl()
        {
            if (!text_content.empty())
            {
                contents.push_back(slim::make_unique<TemplateText>(std::move(text_content)));
            }
            if (contents.size() > 1)
            {
                return slim::make_unique<TemplatePartsList>(std::move(contents));
            }
            else if (contents.size() == 1)
            {
                return std::move(contents[0]);
            }
            else return slim::make_unique<TemplateText>(std::string());
        }

        Parser::Parser(Lexer &lexer)
            : lexer(lexer)
        {}
        Parser::~Parser()
        {}

        Template Parser::parse()
        {
            OutputFrame root;
            current_token = lexer.next_indent();
            parse_lines(-1, root);
            auto root_tpl = root.make_tpl();
            return Template(std::move(root_tpl));
        }

        void Parser::parse_lines(int base_indent, OutputFrame &output)
        {
            while (current_token.type != Token::END)
            {
                assert(current_token.type == Token::INDENT);
                int my_indent = (int)current_token.str.size();
                if (my_indent <= base_indent) return;

                current_token = lexer.next_line_start();
                switch (current_token.type)
                {
                case Token::TEXT_LINE:
                    output << parse_text_line(my_indent);
                    break;
                case Token::TEXT_LINE_WITH_TRAILING_SPACE:
                    output << parse_text_line(my_indent);
                    output << ' ';
                    break;
                case Token::HTML_LINE:
                    output << '<' << parse_text_line(my_indent);
                    break;
                case Token::COMMENT_LINE:
                    parse_text_line(my_indent);
                    break;
                case Token::HTML_COMMENT_LINE:
                    output << "<!--" << parse_text_line(my_indent) << "-->";
                    break;
                case Token::NAME:
                case Token::TAG_ID:
                case Token::TAG_CLASS:
                    parse_tag(my_indent, output);
                    break;
                case Token::OUTPUT_LINE:
                    parse_code_output(output);
                    break;
                case Token::CONTROL_LINE:
                    parse_control_code(my_indent, output);
                    break;
                default: throw TemplateSyntaxError("Unexpected symbol");
                }
            }
        }

        std::string Parser::parse_text_line(int base_indent)
        {
            std::string buf;
            int leading_spaces = 0;
            while (true)
            {
                current_token = lexer.next_text_content();
                switch (current_token.type)
                {
                case Token::END: return buf;
                case Token::TEXT_CONTENT:
                    for (int i = 0; i < leading_spaces; ++i) buf += ' ';
                    buf += current_token.str;
                    current_token = lexer.next_indent();
                    if (current_indent() <= base_indent) return buf;
                    leading_spaces = current_indent() - base_indent - 2;
                    break;
                default: throw TemplateSyntaxError("Unexpected token");
                }
            }
        }

        void Parser::parse_tag(int base_indent, OutputFrame & output)
        {
            //name, id and class
            struct Attr
            {
                std::vector<std::string> static_values;
                std::vector<expr::ExpressionNodePtr> dynamic_values;
            };
            std::unordered_map<std::string, Attr> attributes;

            std::string tag_name;
            if (current_token.type == Token::NAME)
            {
                tag_name = current_token.str;
                current_token = lexer.next_tag_content();
            }
            else tag_name = DEF_TAG;
            if (current_token.type == Token::TAG_ID)
            {
                current_token = lexer.next_name();
                attributes["id"].static_values.push_back(current_token.str);
                current_token = lexer.next_tag_content();
            }
            while (current_token.type == Token::TAG_CLASS)
            {
                current_token = lexer.next_name();
                attributes["class"].static_values.push_back(current_token.str);
                current_token = lexer.next_tag_content();
            }

            //Whitespace control
            bool leading_space = false, trailing_space = false;
            switch (current_token.type)
            {
            case Token::ADD_LEADING_WHITESPACE:
                leading_space = true;
                current_token = lexer.next_tag_content();
                break;
            case Token::ADD_TRAILING_WHITESPACE:
                trailing_space = true;
                current_token = lexer.next_tag_content();
                break;
            case Token::ADD_LEADING_AND_TRAILING_WHITESPACE:
                leading_space = trailing_space = true;
                current_token = lexer.next_tag_content();
                break;
            default: break;
            }

            //Create opening tag
            if (leading_space) output << ' ';
            output << '<' << tag_name; //TODO: attributes, empty tag

            //attributes
            while (current_token.type == Token::ATTR_NAME)
            {

                expr::Lexer expr_lexer(lexer.get_pos(), lexer.get_end());
                expr::Parser expr_parser(BUILTIN_FUNCTIONS, expr_lexer); //TODO: Allow custom functions


                auto attr = current_token.str;
                auto expr = expr_parser.expression();

                lexer.set_pos(expr_parser.get_last_token().pos);

                if (auto lit = dynamic_cast<expr::Literal*>(expr.get()))
                {
                    if (lit->value == TRUE_VALUE)
                    {
                        output << ' ' << attr;
                    }
                    else if (lit->value == FALSE_VALUE || lit->value == NIL_VALUE)
                    {
                        //skip attribute
                    }
                    else
                    {
                        attributes[attr].static_values.push_back(lit->value->to_string());
                    }
                }
                else
                {
                    attributes[attr].dynamic_values.push_back(std::move(expr));
                }

                current_token = lexer.next_tag_content();
            }

            for (auto &attr : attributes)
            {
                if (attr.second.dynamic_values.empty())
                {
                    assert(attr.second.static_values.size() > 0);
                    output << attr_str(attr.first, attr.second.static_values);
                }
            }
            for (auto &attr : attributes)
            {
                if (!attr.second.dynamic_values.empty())
                {
                    output << slim::make_unique<TemplateTagAttr>(
                        attr.first,
                        std::move(attr.second.static_values),
                        std::move(attr.second.dynamic_values));
                }
            }



            //Contents
            output.set_in_tag();
            if (current_token.type == Token::TEXT_CONTENT)
            {
                output << current_token.str;
                current_token = lexer.next_indent();
            }
            else if (current_token.type == Token::EOL)
            {   //no inline content, look for indented lines after this
                current_token = lexer.next_indent();
                parse_lines(base_indent, output);
            }
            else if (current_token.type == Token::OUTPUT_LINE)
            {
                parse_code_output(output);
            }
            else if (current_token.type != Token::END)
            {
                throw TemplateSyntaxError("Unexpected token after tag line");
            }

            bool void_el = VOID_ELEMENTS.count(tag_name) > 0;
            if (output.in_tag() && void_el)
            {
                output.set_in_tag(false);
                output << "/>";
            }
            else if (!void_el) output << "</" + tag_name + ">";
            else throw TemplateSyntaxError("HTML void elements can not have content");
            if (trailing_space) output << ' ';
        }

        void Parser::parse_code_output(OutputFrame &output)
        {
            assert(current_token.type == Token::OUTPUT_LINE);
            current_token = lexer.next_whitespace_control();

            bool leading_space = false, trailing_space = false;
            switch (current_token.type)
            {
            case Token::ADD_LEADING_WHITESPACE:
                leading_space = true; break;
            case Token::ADD_TRAILING_WHITESPACE:
                trailing_space = true;  break;
            case Token::ADD_LEADING_AND_TRAILING_WHITESPACE:
                leading_space = trailing_space = true; break;
            default: break;
            }

            auto expr = parse_code_lines();

            if (leading_space) output << ' ';
            output << std::move(expr);
            if (trailing_space) output << ' ';

            current_token = lexer.next_indent();
        }

        void Parser::parse_control_code(int base_indent, OutputFrame &output)
        {
            assert(current_token.type == Token::CONTROL_LINE);

            current_token = lexer.control_code_start();
            bool have_control_line = true;
            while (have_control_line)
            {
                have_control_line = false;
                if (current_token.type == Token::IF)
                {
                    auto if_expr = parse_code_lines();
                    OutputFrame if_body;
                    std::vector<TemplateCondExpr> elsif;
                    std::unique_ptr<TemplatePart> else_body;

                    current_token = lexer.next_indent();
                    parse_lines(base_indent, if_body);

                    while (current_indent() == base_indent && lexer.try_control_line())
                    {
                        current_token = lexer.control_code_start();
                        if (current_token.type == Token::ELSIF)
                        {
                            auto expr = parse_code_lines();
                            OutputFrame frame;

                            current_token = lexer.next_indent();
                            parse_lines(base_indent, frame);

                            elsif.emplace_back(std::move(expr), frame.make_tpl());
                        }
                        else if (current_token.type == Token::ELSE)
                        {
                            current_token = lexer.next_text_content();
                            if (!current_token.str.empty())
                                throw TemplateSyntaxError("Unexpected content after 'else'");

                            OutputFrame frame;

                            current_token = lexer.next_indent();
                            parse_lines(base_indent, frame);
                            else_body = frame.make_tpl();

                            break; //'else' is last in chain
                        }
                        else //if, each, unless, etc. start a new control block in outer loop
                        {
                            have_control_line = true;
                            break;
                        }
                    }

                    output << slim::make_unique<TemplateIfExpr>(
                        TemplateCondExpr{std::move(if_expr), if_body.make_tpl()},
                        std::move(elsif),
                        std::move(else_body)
                        );
                }
                else
                {
                    throw TemplateSyntaxError("Unexpected control code start");
                }
            }
        }

        std::unique_ptr<expr::ExpressionNode> Parser::parse_code_lines()
        {
            std::string script_src;
            while (true)
            {
                current_token = lexer.next_text_content();
                if (current_token.str.empty()) throw TemplateSyntaxError("Expected expression");
                script_src += current_token.str;

                if (script_src.back() == ',')
                {
                    continue;
                }
                else if (script_src.back() == '\\')
                {
                    script_src.pop_back();
                    continue;
                }
                else break;
            }

            expr::Lexer expr_lexer(script_src);
            expr::Parser expr_parser(BUILTIN_FUNCTIONS, expr_lexer); //TODO: Allow custom functions
            auto expr = expr_parser.full_expression();

            return expr;
        }

        int Parser::current_indent()
        {
            if (current_token.type == Token::END) return -1;
            assert(current_token.type == Token::INDENT);
            return (int)current_token.str.size();
        }

    }
}
