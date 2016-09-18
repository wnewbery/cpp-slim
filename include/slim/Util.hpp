#pragma once
#include <memory>
#include <string>
#include <utility>

namespace slim
{
    class Object;
    /** std::make_unique was not added until c++14 and is not in GCC 4.8. */
    template<class T, class... ARGS>
    std::unique_ptr<T> make_unique(ARGS&&... args)
    {
        return std::unique_ptr<T>(new T(std::forward<ARGS>(args)...));
    }

    /**Encodes '&', '<', '>', '"' and '\'' to entities.*/
    std::string html_escape(const std::string &str);
    /**Encodes if not a HtmlSafeString.*/
    std::string html_escape(const Object *obj);
    inline std::string html_escape(const std::shared_ptr<Object> &obj)
    {
        return html_escape(obj.get());
    }
}

