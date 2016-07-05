#pragma once
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "Error.hpp"
#include "Operators.hpp"
namespace slim
{
    class Object;
    class Boolean;
    class Number;
    typedef std::shared_ptr<Object> ObjectPtr;
    typedef std::shared_ptr<const Object> CObjectPtr;
    class Symbol;
    typedef std::shared_ptr<Symbol> SymPtr;

    typedef std::vector<ObjectPtr> FunctionArgs;
    class MethodTable;

    SymPtr symbol(const std::string &str);

    /**Base abstract object for the expression interpreter.*/
    class Object : public std::enable_shared_from_this<Object>
    {
    public:
        /**Create an instance of this object.
         * 
         * Used by slim::create_object<T> via T::create.
         * 
         * The default implementation simply uses std::make_shared and forwards to the constructor,
         * but types may provide an alternative implementation. For example Null and Boolean are
         * immutable types, and always return a reference to singleton null, true and false
         * instances.
         */
        template<class T, class... Args>
        static std::shared_ptr<T> create(Args && ... args)
        {
            return std::make_shared<T>(std::forward<Args>(args)...);
        }

        virtual ~Object() {}
        /**Returns the type name of this object.
         * Should vary by class only, not instances such that a comparison of type_name strings
         * is equivalent to comparing the typeid of the instances.
         */
        virtual const std::string& type_name()const = 0;
        /**Convert this instance to a displayable string.*/
        virtual std::string to_string()const { return inspect(); }
        /**Convert this instance to a displayable representation of this object.*/
        virtual std::string inspect()const;
        /**Convert this instance to a displayable string object. The default uses to_string. */
        virtual ObjectPtr to_string_obj()const;
        /**Convert this instance to a displayable representation of this object.
         * The default uses inspect.
         */
        virtual ObjectPtr inspect_obj()const;
        /**Returns if this object instance should be considered true in a boolean context.*/
        virtual bool is_true()const { return true; }
        /**Compare with another object of the same type.
         * Default is identity equality.
         */
        virtual bool eq(const Object *rhs)const;
        /**Computes a hash value to optimise equality checks.*/
        virtual size_t hash()const;
        /**Compare with another object of the same type.
         * Default throws UnorderableTypeError.
         */
        virtual int cmp(const Object *rhs)const;

        //operators
        virtual ObjectPtr el_ref(const FunctionArgs &args);
        virtual ObjectPtr mul(Object *rhs);
        virtual ObjectPtr div(Object *rhs);
        virtual ObjectPtr mod(Object *rhs);
        virtual ObjectPtr pow(Object *rhs);
        virtual ObjectPtr add(Object *rhs);
        virtual ObjectPtr sub(Object *rhs);
        virtual ObjectPtr negate();
        virtual ObjectPtr bit_lshift(Object *rhs);
        virtual ObjectPtr bit_rshift(Object *rhs);
        virtual ObjectPtr bit_and(Object *rhs);
        virtual ObjectPtr bit_or(Object *rhs);
        virtual ObjectPtr bit_xor(Object *rhs);
        virtual ObjectPtr bit_not();

        virtual ObjectPtr call_method(SymPtr name, const FunctionArgs &args);
    protected:
        /**Get a function table for the default implementation of call_method.*/
        virtual const MethodTable &method_table()const;
    };

    /**Create an instance of an object of type T.
     * 
     * T should derive from Object.
     * 
     * See Object::create for details on providing custom implementations.
     */
    template<class T, class... Args>
    std::shared_ptr<T> create_object(Args && ... args)
    {
        return T::template create<T>(std::forward<Args>(args)...);
    }

    double as_number(const Object *obj);
    inline double as_number(const ObjectPtr &obj)
    {
        return as_number(obj.get());
    }


    template<class T> T *coerce(Object *obj)
    {
        auto obj2 = dynamic_cast<T*>(obj);
        if (obj2) return obj2;
        else throw TypeError(obj, T::TYPE_NAME);
    }
    template<class T> const T *coerce(const Object *obj)
    {
        auto obj2 = dynamic_cast<const T*>(obj);
        if (obj2) return obj2;
        else throw TypeError(obj, T::TYPE_NAME);
    }
    template<class T> std::shared_ptr<T>coerce(const std::shared_ptr<Object> &obj)
    {
        auto obj2 = std::dynamic_pointer_cast<T>(obj);
        if (obj2) return obj2;
        else throw TypeError(obj.get(), T::TYPE_NAME);
    }
    template<class T> std::shared_ptr<const T> coerce(const std::shared_ptr<const Object> &obj)
    {
        auto obj2 = std::dynamic_pointer_cast<const T>(obj);
        if (obj2) return obj2;
        else throw TypeError(obj.get(), T::TYPE_NAME);
    }

    namespace detail
    {
        template<class T> size_t hash(const T &x)
        {
            return std::hash<T>()(x);
        }
        /**http://stackoverflow.com/a/2595226/6266 */
        template<class T>
        void hash_combine(size_t &seed, const T &v)
        {
            seed ^= hash(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
    }
    struct ObjLess
    {
        bool operator()(const Object *lhs, const Object *rhs)const
        {
            return cmp(lhs, rhs) < 0;
        }
        bool operator()(const ObjectPtr &lhs, const ObjectPtr &rhs)const
        {
            return cmp(lhs.get(), rhs.get()) < 0;
        }
    };
    struct ObjEquals
    {
        bool operator()(const Object *lhs, const Object *rhs)const
        {
            return eq(lhs, rhs);
        }
        bool operator()(const ObjectPtr &lhs, const ObjectPtr &rhs)const
        {
            return eq(lhs.get(), rhs.get());
        }
    };
    struct ObjHash
    {
        size_t operator()(const Object &obj)const { return obj.hash(); }
        size_t operator()(const Object *obj)const { return obj->hash(); }
        size_t operator()(const ObjectPtr &obj)const { return obj->hash(); }
    };
    typedef std::unordered_map<ObjectPtr, ObjectPtr, ObjHash, ObjEquals> ObjectMap;
}
namespace std
{
    template<> struct hash<slim::Object>
    {
        size_t operator()(const slim::Object &obj)const
        {
            return obj.hash();
        }
    };
}
