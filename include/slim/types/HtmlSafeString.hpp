#pragma once
#include "String.hpp"
namespace slim
{
    class HtmlSafeString : public String
    {
    public:
        static const std::string TYPE_NAME;
        using String::String;

        virtual const std::string& type_name()const override { return TYPE_NAME; }
    };
}
