/**
 * @file heap.inl
 * @brief
 */

#pragma once
#ifndef EMDEVIF_FREERTOS_SYSTEM_IMPL_SYSTEM_HEAP_INL
    #define EMDEVIF_FREERTOS_SYSTEM_IMPL_SYSTEM_HEAP_INL

    #if (defined(EMDEVIF_THREAD_USE_ESPIDF_FREERTOS) && EMDEVIF_THREAD_USE_ESPIDF_FREERTOS)
        #include "freertos/FreeRTOS.h"
    #else
        #include "FreeRTOS.h"
    #endif

    #include "emdevif/core/concepts.hpp"

    #include <memory>
    #include <utility>
    #include <type_traits>

namespace emdevif::heap::detail {

inline void* mallocByte(const std::size_t size_in_bytes) noexcept
{
    return pvPortMalloc(size_in_bytes);
}

inline void free(void* block) noexcept
{
    vPortFree(block);
}

}  // namespace emdevif::heap::detail

namespace emdevif::heap {

    #ifdef __cpp_exceptions
template<typename T, typename... Args>
T* construct(Args&&... args)

{
    static_assert(!std::is_void_v<T>, "can't create pointer to incomplete type");
    static_assert(sizeof(T) > 0, "can't create pointer to incomplete type");

    auto ret = static_cast<T*>(detail::mallocByte(sizeof(T)));
    if (ret == nullptr) {
        throw std::bad_alloc();
    }

    try {
        std::construct_at(ret, std::forward<Args>(args)...);
    }
    catch (...) {
        detail::free(ret);
        throw;
    }

    return ret;
}
    #endif

template<typename T, typename... Args>
T* construct(std::nothrow_t, Args&&... args) noexcept
{
    static_assert(!std::is_void_v<T>, "can't create pointer to incomplete type");
    static_assert(sizeof(T) > 0, "can't create pointer to incomplete type");

    auto ret = static_cast<T*>(detail::mallocByte(sizeof(T)));
    if (ret == nullptr) {
        return nullptr;
    }

    std::construct_at(ret, std::forward<Args>(args)...);

    return ret;
}

template<PointerType T>
void destruct(T& p) noexcept
{
    static_assert(!std::is_void_v<T> && !std::is_void_v<std::remove_pointer_t<T>>,
                  "can't delete pointer to incomplete type");
    static_assert(sizeof(T) > 0, "can't delete pointer to incomplete type");

    if (p == nullptr) {
        return;
    }

    std::destroy_at(p);

    detail::free(p);

    p = nullptr;
}

template<typename T>
class Deleter
{
public:
    void operator()(T* p) const noexcept
    {
        static_assert(!std::is_void_v<T>, "can't delete pointer to incomplete type");
        static_assert(sizeof(T) > 0, "can't delete pointer to incomplete type");

        destruct(p);
    }
};

    #ifdef __cpp_exceptions
template<typename T, typename... Args>
unique_ptr<T> make_unique(Args&&... args)
{
    try {
        return unique_ptr<T>{construct<T>(std::forward<Args>(args)...)};
    }
    catch (...) {
        throw;
    }
}
    #endif

template<typename T, typename... Args>
unique_ptr<T> make_unique(std::nothrow_t, Args&&... args) noexcept
{
    return unique_ptr<T>{construct<T>(std::nothrow, std::forward<Args>(args)...)};
}

// 使用时除了导入模块，还需要引入头文件 <memory>
template<typename T>
class Allocator
{
public:
    using pointer = T*;
    using const_pointer = const T*;

    using void_pointer = void*;
    using const_void_pointer = const void*;

    using value_type = T;

    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

public:
    Allocator() noexcept = default;

    // ReSharper disable once CppNonExplicitConvertingConstructor
    template<typename U>
    Allocator(const Allocator<U>&) noexcept  // NOLINT(*-explicit-constructor)
    {
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    pointer allocate(const size_type n) noexcept
    {
        return static_cast<pointer>(detail::mallocByte(n * sizeof(value_type)));
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    // ReSharper disable once CppParameterMayBeConst
    void deallocate(pointer p, const size_type) noexcept
    {
        detail::free(p);
    }
};

}  // namespace emdevif::heap

#endif  // !EMDEVIF_FREERTOS_SYSTEM_IMPL_SYSTEM_HEAP_INL
