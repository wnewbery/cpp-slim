#pragma once
#include "Object.hpp"
#include <vector>
#include <cassert>
namespace slim
{
    class Array;
    class Boolean;
    class Number;
    class String;
    class Hash : public Object
    {
    public:
        static const std::string TYPE_NAME;
        Hash();
        Hash(ObjectPtr def_value)
            : def_value(), map(), list()
        {}

        virtual const std::string& type_name()const override { return TYPE_NAME; }
        virtual std::string to_string()const override { return inspect(); }
        virtual std::string inspect()const override;
        virtual size_t hash()const;
        virtual bool eq(const Object *rhs)const override;

        virtual ObjectPtr el_ref(const FunctionArgs &args)override;

        void set(ObjectPtr key, ObjectPtr val);

        //any?
        //assoc
        //clear
        //compare_by_identity, compare_by_identity?
        //default
        //delete, delete_if
        //each, each_pair, each_key, each_value
        ObjectPtr empty_q();
        //eql?
        ObjectPtr fetch(const FunctionArgs &args);
        std::shared_ptr<Array> flatten(const FunctionArgs &args);
        /**Alias key?, include?, member? */
        ObjectPtr has_key_q(Object *obj);
        /**Alias value? */
        ObjectPtr has_value_q(const Object *obj);
        std::shared_ptr<Hash> invert();
        //keep_if
        ObjectPtr key(const Object *val);
        std::shared_ptr<Array> keys();
        std::shared_ptr<Array> values();
        /**Alias length*/
        ObjectPtr size();
        std::shared_ptr<Hash> merge(Hash *other_hash);
        //merge!
        //rassoc
        //rehash
        //reject, reject!
        //replace
        //select, select!
        //shift
        //store
        std::shared_ptr<Array> to_a();
        std::shared_ptr<Hash> to_h();
        //update
        //values_at
    protected:
        virtual const MethodTable &method_table()const;
    private:
        typedef std::unordered_map<ObjectPtr, size_t, ObjHash, ObjEquals> Map;
        typedef std::vector<std::pair<ObjectPtr, ObjectPtr>> List;

        ObjectPtr def_value;
        Map map;
        List list;
    };

    inline std::shared_ptr<Hash> make_hash(const std::vector<ObjectPtr> &arr)
    {
        assert(arr.size() % 2 == 0);
        auto out = create_object<Hash>();
        for (auto i = arr.begin(); i != arr.end();)
        {
            auto key = *i++;
            auto val = *i++;
            out->set(key, val);
        }
        return out;
    }
}
