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
        using String::String;

        static const std::string &name()
        {
            static const std::string TYPE_NAME = "HtmlSafeString";
            return TYPE_NAME;
        }
        virtual const std::string& type_name()const override { return name(); }
    };
}
