#pragma once
#include "Object.hpp"
#include "Enumerable.hpp"
namespace slim
{
    class Boolean;
    class Number;
    /**Immutable number range.*/
    class Range : public Object, public Enumerable
    {
    public:
        Range(Ptr<Number> begin, Ptr<Number> end, Boolean *exclude_end);
        Range(Ptr<Number> begin, Ptr<Number> end, bool exclude_end);
        Range(Ptr<Object> begin, Ptr<Object> end, bool exclude_end);

        static const std::string &name()
        {
            static const std::string TYPE_NAME = "Range";
            return TYPE_NAME;
        }
        virtual const std::string& type_name()const override { return name(); }
        virtual ObjectPtr this_obj()override { return shared_from_this(); }
        virtual std::string to_string()const override;
        virtual std::string inspect()const override { return to_string(); }
        virtual bool eq(const Object *rhs)const override;
        virtual size_t hash()const override;

        //bsearch
        Ptr<Object> begin();
        Ptr<Boolean> cover_q(Object *obj);
        virtual ObjectPtr each(const FunctionArgs &args)override;
        Ptr<Object> end();
        Ptr<Boolean> exclude_end_q();
        Ptr<Object> first(const FunctionArgs &args);
        Ptr<Boolean> include_q(Object *obj);
        Ptr<Object> last(const FunctionArgs &args);
        Ptr<Object> size();
        Ptr<Object> step(const FunctionArgs &args);
    protected:
        virtual const MethodTable &method_table()const;
    private:
        double _begin, _end;
        bool exclude_end;
    };
}
