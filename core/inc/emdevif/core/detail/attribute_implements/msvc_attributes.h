/**
 * @file msvc_attributes.h
 * @brief MSCV 属性
 */

#pragma once
#ifndef EMDEVIF_CORE_DETAIL_MSVC_ATTRIBUTES_H
#define EMDEVIF_CORE_DETAIL_MSVC_ATTRIBUTES_H

#if (defined(_MSC_VER))

/* clang-format off */

#ifdef __cplusplus
    #define EMDEVIF_NO_RETURN [[noreturn]]
#else
    #define EMDEVIF_NO_RETURN __declspec(noreturn)
#endif

#ifdef __cplusplus
    #define EMDEVIF_MAYBE_UNUSED [[maybe_unused]]
#else
    #define EMDEVIF_MAYBE_UNUSED
#endif

#ifdef __cplusplus
    #define EMDEVIF_ALWAYS_INLINE __forceinline
#else
    #define EMDEVIF_ALWAYS_INLINE __forceinline
#endif

#ifdef __cplusplus
    #define EMDEVIF_FALL_THROUGH [[fall_through]]
#else
    #define EMDEVIF_FALL_THROUGH
#endif

#define EMDEVIF_DATA_SECTION(section_name) __declspec(allocate(section_name))

#define EMDEVIF_FUNC_SECTION(section_name) __declspec(code_seg(section_name))

#define EMDEVIF_FORMAT_CHECK(function, string_index, first_to_check)

/* clang-format on */

#endif  // (defined(_MSC_VER))

#endif  // ！EMDEVIF_CORE_DETAIL_MSVC_ATTRIBUTES_H
