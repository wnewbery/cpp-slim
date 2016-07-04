#pragma once
#include <memory>
#include <utility>

namespace slim
{
    /** std::make_unique was not added until c++14 and is not in GCC 4.8. */
    template<class T, class... ARGS>
    std::unique_ptr<T> make_unique(ARGS&&... args)
    {
        return std::unique_ptr<T>(new T(std::forward<ARGS>(args)...));
    }
}

