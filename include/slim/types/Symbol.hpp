#pragma once
#include "Object.hpp"
namespace slim
{
    class String;
    class Symbol : public Object
    {
    public:
        static const std::string TYPE_NAME;
        ~Symbol();

        virtual const std::string& type_name()const override { return TYPE_NAME; }
        virtual std::string to_string()const override;
        virtual std::string inspect()const override;
        virtual ObjectPtr to_string_obj()const override;
        virtual bool is_true()const override { return true; }
        virtual bool eq(const Object *rhs)const override { return this == rhs; }
        virtual size_t hash()const override;
        virtual int cmp(const Object *rhs)const override;

        const std::shared_ptr<String> &str_obj()const { return _str; }
        const std::string &str()const;
        const char *c_str()const;
    protected:
        virtual const MethodTable &method_table()const;
    private:
        explicit Symbol(std::shared_ptr<String> str);
        std::shared_ptr<String> _str;

        friend std::shared_ptr<Symbol> symbol(const std::string &str);
    };

    SymPtr symbol(const std::string &str);
    SymPtr symbol(std::shared_ptr<String> str);
}
