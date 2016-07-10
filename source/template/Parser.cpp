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
        Parser::Parser(Lexer &lexer)
            : lexer(lexer)
        {}
        Parser::~Parser()
        {}


        Template Parser::parse()
        {
            output_stack.push({-1});
            parse_lines();
            if (current_token.type != Token::END) throw TemplateSyntaxError("Expected end");
            assert(input_stack.empty());
            assert(output_stack.size() == 1);
            
            auto root = finish_output_frame();

            return Template(std::move(root));
        }

        void Parser::parse_lines()
        {
            assert(!output_stack.empty());
            assert(input_stack.empty() || input_stack.top().indent == output_stack.top().indent);
            int base_indent = output_stack.top().indent;

            current_token = lexer.next_indent();
            while (current_token.type != Token::END && (int)current_token.str.size() > base_indent)
            {
                close_tags((int)current_token.str.size());
                parse_line();
                if (current_token.type == Token::END) break;
                else current_token = lexer.next_indent();
            }
            close_tags(base_indent);
        }
        void Parser::parse_line()
        {
            assert(current_token.type == Token::INDENT);
            int my_indent = (int)current_token.str.size();

            //Only support simple elements right now!
            current_token = lexer.next_line_start();
            switch (current_token.type)
            {
            case Token::NAME: return parse_tag(my_indent);
            case Token::TEXT_LINE:
            case Token::TEXT_LINE_WITH_TRAILING_SPACE:
                return parse_text_line();
            default: throw TemplateSyntaxError("Unknown line start token");
            }
        }

        void Parser::parse_tag(int indent)
        {
            assert(current_token.type == Token::NAME);
            auto tagname = current_token.str;
            auto &buf = txt_output_buf();

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
            if (leading_space) buf += " ";
            buf += "<" + tagname + ">"; //TODO: attributes

            //Contents
            if (current_token.type == Token::TEXT_CONTENT)
            {
                buf += current_token.str;
                buf += "</" + tagname + ">";
                if (trailing_space) buf += " ";
            }
            else //no inline content, look for indented lines after this
            {
                input_stack.emplace(indent, trailing_space, tagname);
            }
        }

        void Parser::parse_text_line()
        {
            bool trailing_space = current_token.type == Token::TEXT_LINE_WITH_TRAILING_SPACE;
            auto &buf = txt_output_buf();
            current_token = lexer.next_text_content();
            buf += current_token.str;
            if (trailing_space) buf += " ";
        }

        std::string& Parser::txt_output_buf()
        {
            //Only support text content right now
            assert(!output_stack.empty());
            assert(output_stack.top().contents.empty());
            return output_stack.top().text_content;
        }
        
        void Parser::close_tags(int indent)
        {
            auto &buf = txt_output_buf();
            while (!input_stack.empty() && input_stack.top().indent >= indent)
            {
                auto &frame = input_stack.top();
                buf += "</" + frame.tagname + ">";
                if (frame.trailing_space) buf += " ";
                input_stack.pop();
            }
        }

        std::unique_ptr<TemplatePart> Parser::finish_output_frame()
        {
            assert(!output_stack.empty());
            auto &frame = output_stack.top();
            assert(frame.contents.empty()); //Not yet supported
            auto ret = slim::make_unique<TemplateText>(std::move(frame.text_content));
            output_stack.pop();
            return std::move(ret);
        }
    }
}
