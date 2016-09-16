#include "template/Parser.hpp"
#include "template/Attributes.hpp"
#include "template/Lexer.hpp"
#include "template/Template.hpp"
#include "template/TemplatePart.hpp"
#include "template/TemplateParts.hpp"
#include "template/TemplateBlock.hpp"
#include "template/Token.hpp"
#include "expression/Lexer.hpp"
#include "expression/Parser.hpp"
#include "expression/AstOp.hpp"
#include "types/Boolean.hpp"
#include "types/Nil.hpp"
#include "Error.hpp"
#include "Util.hpp"
#include <cassert>
#include <unordered_set>
#include <regex>

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

        class Parser::OutputFrame
        {
        public:
            OutputFrame() : contents(), text_content(), _in_tag(false) {}
            OutputFrame& operator << (const std::string &text_content)
            {
                handle_in_tag();
                this->text_content += text_content;
                return *this;
            }
            OutputFrame& operator << (char txt_chr)
            {
                handle_in_tag();
                this->text_content += txt_chr;
                return *this;
            }
            OutputFrame& operator << (std::unique_ptr<expr::ExpressionNode> &&expr)
            {
                return *this << slim::make_unique<TemplateOutputExpr>(std::move(expr));
            }
            OutputFrame& operator << (std::unique_ptr<TemplatePart> &&part)
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

            std::unique_ptr<TemplatePart> make_tpl()
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

            bool in_tag()const { return _in_tag; }
            void set_in_tag(bool b = true) { _in_tag = b; }
        private:
            /**The sequence of completed template parts reading to add to the template or
             * parent frame control block.
             */
            std::vector<std::unique_ptr<TemplatePart>> contents;
            /**Text content to go after contents. When adding a non-text part, any text in
             * this buffer is first converted to a TemplateText part.
             */
            std::string text_content;
            /**Currently in a start tag. This will need top be closed with a '<' before other
             * output.
             */
            bool _in_tag;

            void handle_in_tag()
            {
                if (in_tag())
                {
                    set_in_tag(false);
                    this->text_content += '>';
                }
            }
        }; 

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
                    add_interpolated_text(parse_text_line(my_indent), output);
                    break;
                case Token::TEXT_LINE_WITH_TRAILING_SPACE:
                    add_interpolated_text(parse_text_line(my_indent), output);
                    output << ' ';
                    break;
                case Token::HTML_LINE:
                    output << '<';
                    add_interpolated_text(parse_text_line(my_indent), output);
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
                    parse_code_line(my_indent, output);
                    break;
                case Token::CONTROL_LINE:
                    parse_control_code(my_indent, output);
                    break;
                default: error("Unexpected symbol");
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
                default: error("Unexpected token");
                }
            }
        }

        void Parser::parse_tag(int base_indent, OutputFrame & output)
        {
            //name, id and class
            struct Attr
            {
                std::string name;
                std::vector<std::string> static_values;
                std::vector<expr::ExpressionNodePtr> dynamic_values;
            };
            std::vector<Attr> attributes;
            auto get_attr = [&attributes](const std::string &name) -> Attr&
            {
                for (auto &attr : attributes) if (attr.name == name) return attr;
                attributes.push_back({ name,{},{} });
                return attributes.back();
            };

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
                get_attr("id").static_values.push_back(current_token.str);
                current_token = lexer.next_tag_content();
            }
            while (current_token.type == Token::TAG_CLASS)
            {
                current_token = lexer.next_name();
                get_attr("class").static_values.push_back(current_token.str);
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
                expr_lexer.file_name(lexer.file_name());
                expr_lexer.set_reported_pos(lexer.line(), lexer.line_offset());
                expr::Parser expr_parser(local_vars, expr_lexer); //TODO: Allow custom functions


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
                        get_attr(attr).static_values.push_back(lit->value->to_string());
                    }
                }
                else
                {
                    get_attr(attr).dynamic_values.push_back(std::move(expr));
                }

                current_token = lexer.next_tag_content();
            }

            for (auto &attr : attributes)
            {
                if (attr.dynamic_values.empty())
                {
                    assert(attr.static_values.size() > 0);
                    output << attr_str(attr.name, attr.static_values);
                }
            }
            for (auto &attr : attributes)
            {
                if (!attr.dynamic_values.empty())
                {
                    output << slim::make_unique<TemplateTagAttr>(
                        attr.name,
                        std::move(attr.static_values),
                        std::move(attr.dynamic_values));
                }
            }



            //Contents
            output.set_in_tag();
            if (current_token.type == Token::TEXT_CONTENT)
            {
                add_interpolated_text(current_token.str, output);
                current_token = lexer.next_indent();
            }
            else if (current_token.type == Token::EOL)
            {   //no inline content, look for indented lines after this
                current_token = lexer.next_indent();
                parse_lines(base_indent, output);
            }
            else if (current_token.type == Token::OUTPUT_LINE)
            {
                auto ws = parse_ws_control();
                if (ws.leading_space) output << ' ';
                output << parse_code_expr();
                if (ws.trailing_space) output << ' ';
            }
            else if (current_token.type != Token::END)
            {
                error("Unexpected token after tag line");
            }

            bool void_el = VOID_ELEMENTS.count(tag_name) > 0;
            if (output.in_tag() && void_el)
            {
                output.set_in_tag(false);
                output << "/>";
            }
            else if (!void_el) output << "</" + tag_name + ">";
            else error("HTML void elements can not have content");
            if (trailing_space) output << ' ';
        }

        void Parser::parse_code_line(int base_indent, OutputFrame &output)
        {
            auto ws = parse_ws_control();
            auto line = current_token.line;
            auto offset = current_token.offset;
            auto src = parse_code_src();

            std::vector<SymPtr> params;
            //See if there is a do block, but avoid breaking constructs that contain those letters in a word
            auto do_pos = src.rfind(" do");
            bool had_do = false;
            if(do_pos != std::string::npos)
            {
                //Check do is either at end, or has a |params| only after. Otherwise might have found
                //somthing inside the expression
                auto after = do_pos + 3;
                auto p = src.find(' ', after);
                if (p >= src.size() - 1)
                {   //" do *"
                    had_do = true;
                }
                else if(src[p + 1] == '|')
                {   //Expect a second '|' after this, and then only whitespace
                    auto p2 = src.find('|', p + 2);
                    if (p2 != std::string::npos && src.find_first_not_of(" \t\r\n", p2 + 1) == std::string::npos)
                    { 
                        had_do = true;
                    }
                }

                if (had_do)
                {
                    had_do = true;
                    auto left = src.size() - do_pos;
                    auto start = src.data() + do_pos + 3;
                    auto end = src.data() + src.find_last_not_of("\n\r") + 1;

                    auto do_line = line;
                    auto do_offset = offset;
                    auto line_count = (int)std::count(src.data(), start, '\n');
                    if (line_count)
                    {
                        do_line += line_count;
                        auto last_line_start = src.find_last_of("\n\r", do_pos);
                        assert(last_line_start != std::string::npos);
                        ++last_line_start;
                        do_offset = (int)(do_pos - last_line_start + 3 + 1); //3 for " do", +1 because 1-based
                    }

                    expr::Lexer expr_lexer(start, end);
                    expr_lexer.file_name(lexer.file_name());
                    expr_lexer.set_reported_pos(do_line, do_offset);
                    expr::Parser expr_parser(local_vars, expr_lexer);
                    params = expr_parser.param_list();

                    if (expr_parser.get_last_token().pos != end)
                    {
                        error("Unexpected contents after block param list");
                    }

                    src.resize(src.size() - left);
                }
            }

            //parse code
            expr::Lexer expr_lexer(src);
            expr_lexer.file_name(lexer.file_name());
            expr_lexer.set_reported_pos(line, offset);
            expr::Parser expr_parser(local_vars, expr_lexer);
            auto expr = expr_parser.full_expression();

            // See if there is further content to turn into a block
            if (current_token.type == Token::INDENT && current_indent() > base_indent)
            {
                auto func_call = dynamic_cast<expr::FuncCall*>(expr.get());
                if (!func_call) error("Indented block after code line, but no method call to pass block to");
                //add new local variables for block call
                auto old_vars = local_vars;
                for (auto &param : params)
                    local_vars.add(param->str());
                //Pass the indented contents into a new block
                OutputFrame block_frame;
                parse_lines(base_indent, block_frame);
                //Create the executable template
                auto block_tpl = block_frame.make_tpl();
                //Turn it into a expr::Block and TemplateBlock AST node
                auto expr = create_tpl_block(std::move(params), std::move(block_tpl));
                //Put variables back
                local_vars = old_vars;
                //Add it as a block param to the function call
                func_call->args.push_back(std::move(expr));
            }
            else if (had_do) error("Previous code line started 'do' block, but has no content");

            //Output
            if (ws.leading_space) output << ' ';
            output << std::move(expr);
            if (ws.trailing_space) output << ' ';
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
                    auto if_expr = parse_code_expr();
                    OutputFrame if_body;
                    std::vector<TemplateCondExpr> elsif;
                    std::unique_ptr<TemplatePart> else_body;

                    parse_lines(base_indent, if_body);

                    while (current_indent() == base_indent && lexer.try_control_line())
                    {
                        current_token = lexer.control_code_start();
                        if (current_token.type == Token::ELSIF)
                        {
                            auto expr = parse_code_expr();

                            OutputFrame frame;
                            parse_lines(base_indent, frame);

                            elsif.emplace_back(std::move(expr), frame.make_tpl());
                        }
                        else if (current_token.type == Token::ELSE)
                        {
                            current_token = lexer.next_text_content();
                            if (!current_token.str.empty())
                                error("Unexpected content after 'else'");

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
                    //In case of "each do |a, b, c|" block, update local_vars within this scope only
                    error("Unexpected control code start");
                }
            }
        }

        std::unique_ptr<expr::ExpressionNode> Parser::parse_code_expr()
        {
            auto line = lexer.line();
            auto offset = lexer.line_offset();
            auto script_src = parse_code_src();
            expr::Lexer expr_lexer(script_src);
            expr_lexer.set_reported_pos(line, offset);
            expr_lexer.file_name(lexer.file_name());
            expr::Parser expr_parser(local_vars, expr_lexer);
            return expr_parser.full_expression();
        }

        std::string Parser::parse_code_src()
        {
            std::string script_src;
            while (true)
            {
                current_token = lexer.next_text_line();
                if (current_token.str.find_first_not_of(" \t\r\n") == std::string::npos) error("Expected expression");
                script_src += current_token.str;

                auto line_end = script_src.find_last_not_of("\n\r");
                if (line_end != std::string::npos && script_src[line_end] == ',')
                {
                    continue;
                }
                else if (line_end != std::string::npos && script_src[line_end] == '\\')
                {
                    //TODO: Backslashed lines will be part of the script syntax when it has multiple statement support
                    //Replace with space for now rather than strip, so keep position correct
                    script_src[line_end] = ' ';
                    continue;
                }
                else break;
            }
            current_token = lexer.next_indent();
            return script_src;
        }

        Parser::WhiteSpaceControl Parser::parse_ws_control()
        {
            WhiteSpaceControl out;
            current_token = lexer.next_whitespace_control();
            switch (current_token.type)
            {
            case Token::ADD_LEADING_WHITESPACE:
                out.leading_space = true; break;
            case Token::ADD_TRAILING_WHITESPACE:
                out.trailing_space = true; break;
            case Token::ADD_LEADING_AND_TRAILING_WHITESPACE:
                out.leading_space = out.trailing_space = true; break;
            default: break;
            }
            return out;
        }

        int Parser::current_indent()
        {
            if (current_token.type == Token::END) return -1;
            assert(current_token.type == Token::INDENT);
            return (int)current_token.str.size();
        }

        void Parser::add_interpolated_text(const std::string &text, OutputFrame &output)
        {
            auto end = text.data() + text.size();
            size_t p = 0, p2;
            while ((p2 = text.find("#{", p)) != std::string::npos)
            {
                auto start = p;
                p = p2 + 2; //skip past the #{
                if (p2 > 0 && text[p2 - 1] == '\\')
                {   //Escaped #{
                    output << text.substr(start, p2 - start - 1);
                    output << "#{";
                }
                else
                {   //Parse interpolation
                    output << text.substr(start, p2 - start);
                    expr::Lexer expr_lexer(text.data() + p, end);
                    expr_lexer.file_name(lexer.file_name());
                    expr::Parser expr_parser(local_vars, expr_lexer);
                    auto expr = expr_parser.expression();
                    if (expr_parser.get_last_token().type != expr::Token::R_CURLY_BRACKET)
                        error("Expected '}' to end interpolated text");

                    output << std::move(expr);

                    p = expr_lexer.get_pos() - text.data();
                    assert(text[p - 1] == '}');
                }
            }
            if (p < text.size()) output << text.substr(p);
        }

        void Parser::error(const std::string &msg)
        {
            throw TemplateSyntaxError(lexer.file_name(), current_token.line, current_token.offset, msg);
        }
    }
}
