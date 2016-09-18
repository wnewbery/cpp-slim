#pragma once
#include "Error.hpp"
#include "types/Boolean.hpp"
#include "types/Number.hpp"
#include "types/String.hpp"
#include <cassert>

namespace slim
{
    /**Sets out if arg is a String instance.*/
    inline bool try_unpack_arg(const ObjectPtr &arg, std::string *out)
    {
        auto str = dynamic_cast<String*>(arg.get());
        if (str)
        {
            *out = str->get_value();
            return true;
        }
        else return false;
    }
    /**Sets out if arg is a Number instance.*/
    inline bool try_unpack_arg(const ObjectPtr &arg, double *out)
    {
        auto n = dynamic_cast<Number*>(arg.get());
        if (n)
        {
            *out = n->get_value();
            return true;
        }
        else return false;
    }
    /**Sets out if arg is a Number instance with a truncation conversion.*/
    inline bool try_unpack_arg(const ObjectPtr &arg, int *out)
    {
        auto n = dynamic_cast<Number*>(arg.get());
        if (n)
        {
            *out = (int)n->get_value();
            return true;
        }
        else return false;
    }
    /**Sets out if arg is a Boolean instance.*/
    inline bool try_unpack_arg(const ObjectPtr &arg, bool *out)
    {
        auto n = dynamic_cast<Boolean*>(arg.get());
        if (n)
        {
            *out = n->is_true();
            return true;
        }
        else return false;
    }

    /**Sets out if arg is an instance of the same type.*/
    template<class T>
    bool try_unpack_arg(const ObjectPtr &arg, T **out)
    {
        auto ptr = dynamic_cast<T*>(arg.get());
        if (ptr)
        {
            *out = ptr;
            return true;
        }
        else return false;
    }

    /**Sets out if arg is an instance of the same type.*/
    template<class T>
    bool try_unpack_arg(const ObjectPtr &arg, std::shared_ptr<T> *out)
    {
        auto ptr = std::dynamic_pointer_cast<T>(arg);
        if (ptr)
        {
            *out = std::move(ptr);
            return true;
        }
        else return false;
    }

    /**Uses try_unpack_arg to try and convert arg to an instance of T.
     * @throw ArgumentError if try_unpack_arg fails. 
     */
    template<class T>
    T unpack_arg(const ObjectPtr &arg)
    {
        T out;
        if (try_unpack_arg(arg, &out))
            return out;
        throw ArgumentError();
    }

    namespace detail
    {
        /**Base case for recursive try_unpack_inner template.
         * @return true.
         */
        inline bool try_unpack_inner(FunctionArgs::const_iterator begin, FunctionArgs::const_iterator end)
        {
            assert(begin == end);
            return true;
        }

        /**Implementation of try_unpack.
         * Given an iterator range that is no large than the template ARGS parameter, unpacks each
         * argument from the iterator range into args in turn using try_unpack_arg.
         *
         * If unpacking any arg fails, then args is left untouched.
         * @tparam NEXT The type of the next argument.
         * @tparam ARGS The types of the remaining arguments for recursion.
         * @param begin The start of the input arguments iterator range.
         * @param end The end of the input arguments iterator range.
         * @param out The next output argument to unpack (from *begin).
         * @param args The remaining arguments to unpack.
         * @return true If unpacking every element in the iterator range suceeds.
         */
        template<class NEXT, class... ARGS>
        bool try_unpack_inner(
            FunctionArgs::const_iterator begin, FunctionArgs::const_iterator end,
            NEXT *out, ARGS && ...args)
        {
            if (begin == end) return true;
            NEXT tmp; //dont overwrite *out until sure of success
            if (!try_unpack_arg(*begin, &tmp)) return false;
            if (!try_unpack_inner(begin + 1, end, std::forward<ARGS>(args)...)) return false;
            *out = std::move(tmp);
            return true;
        }
    }

    /**Tries to get all the specified arguments with the correct data types.
     *
     * Optional arguments are those with a high index than the "required" parameter. If they are
     * not provided by "args" the it is not considered an error, and the initial value is left
     * unaltered.
     * 
     * The actual conversions are done by try_unpack_arg which is called in a manner where ADL will
     * apply, so additional native type conversions can be added in the types own namespace.
     * 
     * @tparam required The number of required arguments. Required arguments are always at the start.
     * @tparam ARGS The types of the arguments. The compiler can generally determine this template
     * parameter implicitly.
     * @param args The input arguments array to unpack.
     * @param out The output argument object pointers to unpack input.
     * @return True if the number of arguments is acceptable and unpacking all arguments succeeded.
     * Otherwise false and the "out" parameters are left untouched.
     */
    template<size_t required, class... ARGS>
    bool try_unpack(const FunctionArgs &args, ARGS && ...out)
    {
        if (required > args.size() || args.size() > sizeof...(ARGS)) return false;
        return detail::try_unpack_inner(args.begin(), args.end(), std::forward<ARGS>(out)...);
    }

    /**try_unpack with no optional arguments.*/
    template<class... ARGS>
    bool try_unpack(const FunctionArgs &args, ARGS && ...out)
    {
        return try_unpack<sizeof...(ARGS)>(args, std::forward<ARGS>(out)...);
    }

    /**try_unpack but throws an exception on failure rather than returning false.*/
    template<size_t required, class... ARGS>
    void unpack(const FunctionArgs &args, ARGS && ...out)
    {
        if (required > args.size() || args.size() > sizeof...(ARGS))
            throw ArgumentCountError(args.size(), required, sizeof...(ARGS));
        if (!detail::try_unpack_inner(args.begin(), args.end(), std::forward<ARGS>(out)...))
            throw ArgumentError();
    }

    /**unpack with no optional arguments.*/
    template<class... ARGS>
    void unpack(const FunctionArgs &args, ARGS && ...out)
    {
        return unpack<sizeof...(ARGS)>(args, std::forward<ARGS>(out)...);
    }
}
