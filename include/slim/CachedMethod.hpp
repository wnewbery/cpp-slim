#pragma once
#include "Function.hpp"
#include <typeinfo>
#include <cassert>
#ifdef _WIN32
    #include <intrin.h>
#endif

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
            assert(((size_t)(&data) % (sizeof(void*)*2)) == 0);
            auto cached = atomic_read();
            // Check
            auto obj_type = &typeid(*obj);
            if (obj_type == cached.type)
                return cached.method;
            // Update
            auto method = obj->get_method(name);
            atomic_write({ method, obj_type });

            return method;
        }
    private:
        #if defined(_WIN32)
            __declspec(align(sizeof(void*) * 2))
        #else
            __attribute__((aligned(sizeof(void*) * 2)))
        #endif
        struct Data
        {
            const Method *method;
            const std::type_info *type;
        };
        Data data = { nullptr, nullptr };
        #if defined _WIN64
        Data atomic_read()
        {
            // Out always contains the value of data (note, docs for InterlockedCompareExchange128 appear to be wrong)
            Data out;
            _InterlockedCompareExchange128((intptr_t*)&data, 0, 0, (intptr_t*)&out);
            return out;
        }
        void atomic_write(const Data &value)
        {
            // There is a chance the compare fails and the result is not written, just let the other thread win
            auto cached = data;
            _InterlockedCompareExchange128((intptr_t*)&data, (intptr_t)value.method, (intptr_t)value.type, (intptr_t*)&cached);
        }
        #elif defined _WIN32
        Data atomic_read()
        {
            auto ret = _InterlockedCompareExchange64((long long*)&data, 0, 0);
            return *(Data*)&ret;
        }
        void atomic_write(const Data &value)
        {
            _InterlockedCompareExchange64((long long*)&data, *(const long long*)&value, *(long long*)&data);
        }
        #else
            #if __x86_64__
                typedef __int128_t AtomicValue;
            #else
                typedef long long AtomicValue;
            #endif
        Data atomic_read()
        {
            auto ret = __sync_val_compare_and_swap((AtomicValue*)&data, 0, 0);
            return *(Data*)&ret;
        }
        void atomic_write(const Data &value)
        {
            __sync_val_compare_and_swap((AtomicValue*)&data, *(AtomicValue*)&data, *(const AtomicValue*)&value);
        }
        #endif
    };
}
