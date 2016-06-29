#pragma once
#include <memory>
#include <string>
namespace slim
{
    class Object;
    typedef std::shared_ptr<Object> ObjectPtr;
    typedef std::shared_ptr<const Object> CObjectPtr;
    class Object
    {
    public:
        template<class T, class... Args>
        static std::shared_ptr<T> create(Args && ... args)
        {
            return std::make_shared<T>(std::forward<Args>(args)...);
        }

        virtual ~Object() {}

        virtual const std::string& type_name()const = 0;
        virtual std::string to_string()const = 0;
        virtual bool is_true()const { return true; }
        /**Compare with another object of the same type.
         * Default is identity equality.
         */
        virtual bool eq(const Object *rhs)const;
        /**Compare with another object of the same type.
         * Default throws UnorderableTypeError.
         */
        virtual int cmp(const Object *rhs)const;
    };

    template<class T, class... Args>
    std::shared_ptr<T> create_object(Args && ... args)
    {
        return T::create<T>(std::forward<Args>(args)...);
    }
}
