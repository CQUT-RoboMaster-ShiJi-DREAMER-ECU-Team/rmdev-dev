/**
 * @file clang_attributes.h
 * @brief clang 的属性宏实现
 */

#pragma once
#ifndef EMDEVIF_CORE_DETAIL_CLANG_ATTRIBUTES_H
#define EMDEVIF_CORE_DETAIL_CLANG_ATTRIBUTES_H

#if (defined(__clang__))

/* clang-format off */

#ifdef __cplusplus
    #define EMDEVIF_NO_RETURN [[noreturn]]
#else
    #define EMDEVIF_NO_RETURN __attribute__((noreturn))
#endif

#ifdef __cplusplus
    #define EMDEVIF_MAYBE_UNUSED [[maybe_unused]]
#else
    #define EMDEVIF_MAYBE_UNUSED __attribute__((unused))
#endif

#ifdef __cplusplus
    #define EMDEVIF_USED [[clang::used]]
#else
    #define EMDEVIF_USED __attribute__((used))
#endif

#ifdef __cplusplus
    #define EMDEVIF_WEAK [[clang::weak]]
#else
    #define EMDEVIF_WEAK __attribute__((weak))
#endif

#ifdef __cplusplus
    #define EMDEVIF_ALWAYS_INLINE [[clang::always_inline]]
#else
    #define EMDEVIF_ALWAYS_INLINE __attribute__((always_inline))
#endif

#ifdef __cplusplus
    #define EMDEVIF_FALL_THROUGH [[fall_through]]
#else
    #define EMDEVIF_FALL_THROUGH __attribute__((fallthrough))
#endif

#define EMDEVIF_DATA_SECTION(section_name) __attribute__((section(section_name)))

#define EMDEVIF_FUNC_SECTION(section_name) __attribute__((section(section_name)))

#define EMDEVIF_FORMAT_CHECK(function, string_index, first_to_check) __attribute__((format(function, string_index, first_to_check)))

/* clang-format on */

#endif  // (defined(__clang__))

#endif  // !EMDEVIF_CORE_DETAIL_CLANG_ATTRIBUTES_H
