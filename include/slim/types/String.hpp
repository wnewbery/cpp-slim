#pragma once
#include "Object.hpp"
namespace slim
{
    class String : public Object
    {
    public:
        static const std::string TYPE_NAME;
        explicit String(std::string v) : v(v) {}

        virtual const std::string& type_name()const override { return TYPE_NAME; }
        virtual std::string to_string()const override { return v; }
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
        std::shared_ptr<Number> to_f();
        std::shared_ptr<Number> to_i();
    protected:
        virtual const MethodTable &method_table()const;
    private:
        std::string v;
    };

    inline std::shared_ptr<String> make_value(const std::string &v)
    {
        return create_object<String>(v);
    }
    inline std::shared_ptr<String> make_value(const char *v)
    {
        return create_object<String>(v);
    }
}
