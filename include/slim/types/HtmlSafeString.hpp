#pragma once
#include "String.hpp"
namespace slim
{
    /**Script HtmlSafeString type.
     * This is a String but is detected by RTTI by html_escape and assumed to already be escaped.
     */
    class HtmlSafeString : public String
    {
    public:
        static const std::string TYPE_NAME;
        using String::String;

        virtual const std::string& type_name()const override { return TYPE_NAME; }
    };
}
