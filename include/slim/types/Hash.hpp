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
    /**Script Hash type.*/
    class Hash : public Object
    {
    public:
        typedef std::vector<std::pair<ObjectPtr, ObjectPtr>> List;
        typedef List::iterator const_iterator;

        Hash();
        Hash(ObjectPtr def_value)
            : def_value(), map(), list()
        {}

        static const std::string &name()
        {
            static const std::string TYPE_NAME = "Hash";
            return TYPE_NAME;
        }
        virtual const std::string& type_name()const override { return name(); }

        virtual std::string to_string()const override { return inspect(); }
        virtual std::string inspect()const override;
        virtual size_t hash()const;
        virtual bool eq(const Object *rhs)const override;

        virtual ObjectPtr el_ref(const FunctionArgs &args)override;

        const_iterator begin() { return list.begin(); }
        const_iterator end() { return list.end(); }

        void set(ObjectPtr key, ObjectPtr val);
        /**Gets a value by key if it exists, else creates a new blank one with
         * create_object.
         * Functions in a similar manner to std::map::operator[].
         */
        template<class T>
        ObjectPtr get_or_create(ObjectPtr key)
        {
            auto x = map.emplace(key, list.size());
            if (x.second)
            {
                auto created = create_object<T>();
                list.emplace_back(key, created);
                return created;
            }
            else return list[x.first->second].second;
        }

        //TODO: Needs some more thought (e.g. string vs symbol keys)
        /**C++ convinence function for "self.fetch(key, default).to_s" expression.*/
        std::string get_str(const std::string &key, const std::string &def);

        /**Shallow copy this hash.*/
        std::shared_ptr<Hash> dup();

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
