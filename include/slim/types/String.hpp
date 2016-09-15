#pragma once
#include "Object.hpp"
namespace slim
{
    class Array;
    class Boolean;
    class Number;
    class String : public Object
    {
    public:
        static const std::string TYPE_NAME;
        explicit String(std::string &&v) : v(std::move(v)) {}
        explicit String(const std::string &v) : v(v) {}
        explicit String() : v() {}

        virtual const std::string& type_name()const override { return TYPE_NAME; }
        virtual std::string to_string()const override { return v; }
        virtual std::string inspect()const override;
        virtual std::shared_ptr<String> to_string_obj()override
        {
            return std::static_pointer_cast<String>(shared_from_this());
        }
        virtual bool is_true()const override { return !v.empty(); }
        virtual bool eq(const Object *rhs)const override
        {
            return v == ((const String*)rhs)->v;
        }
        virtual size_t hash()const { return detail::hash(v); }
        virtual int cmp(const Object *rhs)const override
        {
            return v.compare(((const String*)rhs)->v);
        }
        const std::string& get_value()const { return v; }

        virtual ObjectPtr add(Object *rhs);
        //% * << =~
        virtual ObjectPtr el_ref(const FunctionArgs &args)override;

        std::shared_ptr<Number> to_f();
        std::shared_ptr<Number> to_i();
        /** Also intern */
        std::shared_ptr<Symbol> to_sym();
        /** Return a copy as a HtmlSafeString. */
        std::shared_ptr<String> html_safe();

        //Encoding functions not supported. String is just UTF-8.
        //b, bytes, bytesize, byteslice
        //chars
        //codepoints
        //each_byte, each_char, cache_codepoint
        //force_encoding
        //getbyte, setbyte

        //try_convert
        std::shared_ptr<Boolean> ascii_only_q();
        std::shared_ptr<String> capitalize();
        //capitalize!
        ObjectPtr casecmp(String *rhs);
        std::shared_ptr<String> center(const FunctionArgs &args);
        std::shared_ptr<String> chomp(const FunctionArgs &args);
        //chomp!
        //chop, chop!
        //chr
        //clear
        //concat
        //std::shared_ptr<Number> count(const FunctionArgs &args);
        //crypt
        //delete, delete!
        std::shared_ptr<String> downcase();
        //downcase!
        //dump
        ObjectPtr each_line(const FunctionArgs &args);
        std::shared_ptr<Boolean> empty_q();
        //encode, encode!
        std::shared_ptr<Boolean> end_with_q(const FunctionArgs &args);
        //eql?
        //gsub, gsub!
        std::shared_ptr<Number> hex();
        std::shared_ptr<Boolean> include_q(const String *rhs);
        ObjectPtr index(const FunctionArgs &args);
        //insert
        std::shared_ptr<Array> lines(const FunctionArgs &args);
        std::shared_ptr<String> ljust(const FunctionArgs &args);
        std::shared_ptr<String> lstrip();
        //lstrip!
        //match
        //next, next!
        //oct
        std::shared_ptr<Number> ord();
        std::shared_ptr<Array> partition(String *obj);
        //prepend
        //replace
        std::shared_ptr<String> reverse();
        std::shared_ptr<String> rjust(const FunctionArgs &args);
        std::shared_ptr<Array> rpartition(String *obj);
        std::shared_ptr<String> rstrip();
        //rstrip!
        //scan
        //scrub, scrub!

        //reverse!
        ObjectPtr rindex(const FunctionArgs &args);
        /** Also length */
        std::shared_ptr<Number> size();
        //slice!
        std::shared_ptr<Array> split(const FunctionArgs &args);
        //std::shared_ptr<String> squeeze(const FunctionArgs &args);
        std::shared_ptr<Boolean> start_with_q(const FunctionArgs &args);
        std::shared_ptr<String> strip();
        //strib!
        //sub, sub!
        //succ, succ!
        //sum
        //swapcase, sawpcase!
        //to_c, to_r
        //std::shared_ptr<String> tr(const String *from_str, const String *to_str);
        //tr!
        //std::shared_ptr<String> tr_s(const String *from_str, const String *to_str);
        //tr_s!
        //unpack
        std::shared_ptr<String> upcase();
        //upcase!
        //upto
        //valid_encoding?

    protected:
        virtual const MethodTable &method_table()const;
    private:
        std::string v;

        std::shared_ptr<Array> do_partition(bool reverse, String * sep);
        ObjectPtr do_slice(int start, int length);
        std::vector<std::string> split_lines()const;
        std::vector<std::string> split_lines(const std::string &sep)const;
    };
    typedef std::shared_ptr<String> StringPtr;

    inline StringPtr make_value(std::string &&v)
    {
        return create_object<String>(std::move(v));
    }
    inline StringPtr make_value(const std::string &v)
    {
        return create_object<String>(v);
    }
    inline StringPtr make_value(const char *v)
    {
        return create_object<String>(v);
    }
}
