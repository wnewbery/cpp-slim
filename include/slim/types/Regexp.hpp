#pragma once
#include "Object.hpp"
#include "Type.hpp"
#include <regex>
#include <unordered_map>
#include <vector>
#include <cassert>
namespace slim
{
    class Array;
    class Number;
    class String;
    class Regexp;
    /**MatchData for Regexp.*/
    class MatchData : public Object
    {
    public:
        static const std::string &name()
        {
            static const std::string TYPE_NAME = "MatchData";
            return TYPE_NAME;
        }
        virtual const std::string& type_name()const override { return name(); }

        virtual std::string to_string()const override;
        virtual bool eq(const Object *rhs)const override;
        virtual size_t hash()const override;
        virtual ObjectPtr el_ref(const FunctionArgs &args)override;

        Ptr<Object> begin(Number *n);
        Ptr<Array> captures();
        Ptr<Object> end(Number *n);
        //names: Not supported because do not support named captures
        Ptr<Object> offset(Number *n);
        Ptr<String> post_match();
        Ptr<String> pre_match();
        Ptr<Regexp> regexp() { return regex; }
        Ptr<Number> size();
        Ptr<String> string();
        Ptr<Array> to_a();
        Ptr<Array> values_at(const FunctionArgs &args);
    protected:
        virtual const MethodTable &method_table()const;
    private:
        friend class Regexp;
        Ptr<Regexp> regex;
        std::string str;
        std::smatch match;

        MatchData(Ptr<Regexp> regex, const std::string &str)
            : regex(regex), str(str), match()
        {}
        std::ssub_match get_sub(Number *n)const;
        Ptr<Object> sub_str(int n)const;
    };

    /**Script Regexp using std::regex.*/
    class Regexp : public Object
    {
    public:
        static const int IGNORECASE = 1;
        static const int EXTENDED = 2;
        static const int MULTILINE = 4;

        static Ptr<String> escape(String *str);
        //static Ptr<Object> last_match(const FunctionArgs &args);
        static Ptr<Regexp> new_instance(const FunctionArgs &args);
        //static Ptr<Regexp> try_convert(Ptr<Object> obj);
        //static Ptr<Regexp> union(const FunctionArgs &args);

        static const std::string &name()
        {
            static const std::string TYPE_NAME = "Regexp";
            return TYPE_NAME;
        }
        virtual const std::string& type_name()const override { return name(); }

        Regexp(const Regexp &cp) = default;
        Regexp(const std::string &str, int opts);

        virtual std::string to_string()const override;
        virtual std::string inspect()const override;
        virtual bool eq(const Object *rhs)const override;
        virtual size_t hash()const override;
        //unary ~

        /**Returns nullptr rather than NIL_VALUE*/
        virtual Ptr<MatchData> do_match(const std::string &str, int pos);
        virtual Ptr<Object> match(const FunctionArgs &args);

        Ptr<Boolean> casefold_q();
        //encoding
        //fixed_encoding?
        //named_capatures
        //names
        Ptr<Number> options();
        Ptr<String> source();

    protected:
        virtual const MethodTable &method_table()const;
    private:
        std::string src;
        int opts;
        std::regex regex;
    };

    class RegexpType : public SimpleClass<Regexp>
    {
    public:
        RegexpType();
    protected:
        virtual const slim::MethodTable &method_table()const override;
    };
}
