#pragma once
#include "Object.hpp"
#include "Enumerable.hpp"
#include <vector>
namespace slim
{
    class Boolean;
    class Number;
    class String;
    /**Script array type.*/
    class Array : public Object, public Enumerable
    {
    public:
        typedef std::vector<ObjectPtr> List;
        typedef List::iterator iterator;

        explicit Array() : arr() {}
        explicit Array(std::vector<ObjectPtr> &&arr) : arr(std::move(arr)) {}
        explicit Array(const std::vector<ObjectPtr> &arr) : arr(arr) {}

        static const std::string &name()
        {
            static const std::string TYPE_NAME = "Array";
            return TYPE_NAME;
        }
        virtual const std::string& type_name()const override { return name(); }

        virtual ObjectPtr this_obj()override { return shared_from_this(); }

        virtual std::string to_string()const override { return inspect(); }
        virtual std::string inspect()const override;
        virtual bool eq(const Object *rhs)const override { return cmp(rhs) == 0; }
        virtual size_t hash()const;
        virtual int cmp(const Object *rhs)const override;

        virtual ObjectPtr el_ref(const FunctionArgs &args)override { return slice(args); }
        virtual ObjectPtr add(Object *rhs)override;
        virtual ObjectPtr sub(Object *rhs)override;

        iterator begin() { return arr.begin(); }
        iterator end() { return arr.end(); }

        const std::vector<ObjectPtr>& get_value()const { return arr; }

        void push_back(ObjectPtr obj)
        {
            arr.push_back(obj);
        }
        void push_back(Object *obj)
        {
            arr.push_back(obj->shared_from_this());
        }

        //[]=. <<
        //any?
        std::shared_ptr<Object> assoc(const Object *a);
        std::shared_ptr<Object> at(const Number *n);
        //bsearch
        //combination
        std::shared_ptr<Array> compact();
        //concat
        std::shared_ptr<Number> count(const FunctionArgs &args);
        //cycle
        //delete, delete_at, delete_if, drop, drop_while
        virtual ObjectPtr each(const FunctionArgs &args)override;
        //each_index
        std::shared_ptr<Boolean> empty_q();
        std::shared_ptr<Object> fetch(const FunctionArgs &args);
        //fill
        std::shared_ptr<Object> first(const FunctionArgs &args);
        std::shared_ptr<Array> flatten(const FunctionArgs &args);
        void flatten_imp(std::vector<ObjectPtr> &out, int level);
        std::shared_ptr<Boolean> frozen_q();
        //hash
        bool include_q_imp(const Object *obj);
        std::shared_ptr<Boolean> include_q(const Object *obj);
        /** also find_index */
        std::shared_ptr<Object> index(const Object *obj);
        //std::shared_ptr<Object> index(const FunctionArgs &args);
        //initialize_copy
        //insert
        std::shared_ptr<String> join(const String *sep);
        //keep_if
        std::shared_ptr<Object> last(const FunctionArgs &args);
        /**Also length */
        std::shared_ptr<Number> size();
        //pack
        //permutation
        //pop
        //product
        //push
        std::shared_ptr<Object> rassoc(const Object *a);
        //reject
        //repeated_combination
        //repeated_permutation
        //replace
        std::shared_ptr<Array> reverse();
        //reverse_each
        std::shared_ptr<Object> rindex(const Object *obj);
        //std::shared_ptr<Object> rindex(const FunctionArgs &args);
        std::shared_ptr<Array> rotate(const FunctionArgs &args);
        //sample
        //select
        //shift
        //shuffle
        std::shared_ptr<Object> slice(const FunctionArgs &args);
        std::shared_ptr<Array> sort();
        //sort_by
        std::shared_ptr<Array> take(const Number *n);
        //take_while
        //to_ary
        //transpose
        std::shared_ptr<Array> uniq();
        //unshift
        std::shared_ptr<Array> values_at(const FunctionArgs &args);
        //zip
    protected:
        virtual const MethodTable &method_table()const;
    private:
        List arr;
    };

    inline std::shared_ptr<Array> make_value(std::vector<ObjectPtr> &&arr)
    {
        return create_object<Array>(std::move(arr));
    }
    inline std::shared_ptr<Array> make_array(std::vector<ObjectPtr> &&arr)
    {
        return create_object<Array>(std::move(arr));
    }
    inline std::shared_ptr<Array> make_value(const std::vector<ObjectPtr> &arr)
    {
        return create_object<Array>(arr);
    }
    inline std::shared_ptr<Array> make_array(const std::vector<ObjectPtr> &arr)
    {
        return create_object<Array>(arr);
    }
}
