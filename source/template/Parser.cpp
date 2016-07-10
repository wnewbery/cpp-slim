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
            current_token = lexer.next_name();
            assert(current_token.type == Token::NAME);
            parse_tag(my_indent);
        }

        void Parser::parse_tag(int indent)
        {
            assert(current_token.type == Token::NAME);
            auto tagname = current_token.str;
            auto &buf = txt_output_buf();
            bool trailing_space = false;
            current_token = lexer.next_tag_content();
            if (current_token.type == Token::ADD_LEADING_WHITESPACE || current_token.type == Token::ADD_LEADING_AND_TRAILING_WHITESPACE)
                buf += " ";
            buf += "<" + tagname + ">";
            if (current_token.type == Token::ADD_TRAILING_WHITESPACE || current_token.type == Token::ADD_LEADING_AND_TRAILING_WHITESPACE)
                trailing_space = " ";

            if (current_token.type != Token::END && current_token.type != Token::EOL)
                current_token = lexer.next_line();

            input_stack.emplace(indent, trailing_space, tagname);
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
