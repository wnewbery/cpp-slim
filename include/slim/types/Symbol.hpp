#pragma once
#include "Object.hpp"
namespace slim
{
    class String;
    /**Symbol instances as in Ruby are globally unique instances for any given string value.
     *
     * Instances should not be created directly, instead use slim::symbol(str).
     *
     * If there is no existing instance for a given string a new one is created, else the existing
     * instance is used.
     *
     * Fetching the symbol from a string is very slow compared to just using the all ready known
     * string, and once a symbol has been allocated, it will never be freed, but as a single
     * instance for any given string, its identity / memory address may be used for equality
     * comparisons and therefore also hash values.
     *
     * This makes symbols better suited in situations such as hash maps, assuming the symbol
     * instance is known in advance (since slim::symbol must use some sort of thread-safe string
     * map, calling it for every key lookup would eliminate any advantage).
     *
     * Because once allocate symbols will never be freed from memory, symbols should not be created
     * for very large strings, and symbols should not be created for user-defined strings
     * (e.g. URL or form parameters).
     *
     * slim::symbol thread safe, and is safe to use during static / global initialisation,
     * e.g. for global constants holding important symbols.
     */
    class Symbol : public Object
    {
    public:
        static const std::string TYPE_NAME;
        ~Symbol();

        virtual const std::string& type_name()const override { return TYPE_NAME; }
        virtual std::string to_string()const override;
        virtual std::string inspect()const override;
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
