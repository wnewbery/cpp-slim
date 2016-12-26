#pragma once
#include "Function.hpp"
#include <intrin.h>
#include <typeinfo>
#include <cassert>

namespace slim
{
    /**Caches the result of find_method for the most recent type to avoid method lookups where possible.
     * Thread safe.
     */
    class CachedMethod
    {
    public:
        const Method *get(Object *obj, const SymPtr &name)
        {
            typedef long long Value;
            assert(((unsigned long long)(&data) % 16) == 0);
            Data cached;

            // Atomic read
            while (true)
            {
                auto high = (Value)(cached.method = data.method);
                auto low = (Value)(cached.type = data.type);
                if (_InterlockedCompareExchange128((Value*)&data, high, low, (Value*)&cached))
                    break;
            }
            // Check
            auto obj_type = &typeid(*obj);
            if (obj_type == cached.type)
                return cached.method;
            // Update
            auto method = obj->get_method(name);
            // Atomic write
            while (true)
            {
                cached = data;
                auto high = (Value)obj_type;
                auto low = (Value)method;
                if (_InterlockedCompareExchange128((Value*)&data, high, low, (Value*)&cached))
                    break;
            }
            return method;
        }
    private:
        #if defined(_WIN32)
            __declspec(align(sizeof(void*) * 2))
        #else
        #error Not supported
        #endif
        struct Data
        {
            Method *method = nullptr;
            const std::type_info *type = nullptr;
        };
        Data data;
    };
}
