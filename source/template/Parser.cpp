#include "template/Parser.hpp"
#include "template/Lexer.hpp"
#include "template/Token.hpp"
#include "template/TemplateParts.hpp"
#include "Error.hpp"
#include "Util.hpp"
#include <cassert>

namespace slim
{
    namespace tpl
    {
        Parser::OutputFrame& Parser::OutputFrame::operator << (const std::string &text_content)
        {
            this->text_content += text_content;
            return *this;
        }
        Parser::OutputFrame& Parser::OutputFrame::operator << (char txt_chr)
        {
            this->text_content += txt_chr;
            return *this;
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
                    parse_tag(my_indent, output);
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
            assert(current_token.type == Token::NAME);
            auto tagname = current_token.str;


            current_token = lexer.next_tag_content();

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
            }

            //Create opening tag
            if (leading_space) output << ' ';
            output << '<' << tagname << '>'; //TODO: attributes, empty tag
            
            //Contents
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
            else if (current_token.type != Token::END)
            {
                throw TemplateSyntaxError("Unexpected token after tag line");
            }
            output << "</" + tagname + ">";
            if (trailing_space) output << ' ';
        }

        int Parser::current_indent()
        {
            if (current_token.type == Token::END) return -1;
            assert(current_token.type == Token::INDENT);
            return (int)current_token.str.size();
        }

    }
}
