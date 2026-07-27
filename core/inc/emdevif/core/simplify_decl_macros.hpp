/**
 * @file simplify_decl_macros.hpp
 * @brief 简化定义宏
 */

#pragma once
#ifndef EMDEVIF_CORE_SIMPLIFY_DECL_MACROS_HPP
#define EMDEVIF_CORE_SIMPLIFY_DECL_MACROS_HPP

/**
 * 删除拷贝构造函数和拷贝赋值运算符
 * @param ClassName 待删除的类名
 */
#define EMDEVIF_DELETE_COPY_CONSTRUCTOR(ClassName) \
    ClassName(const ClassName&) = delete;          \
    ClassName& operator=(const ClassName&) = delete

/**
 * 删除移动构造函数和移动赋值运算符
 * @param ClassName 待删除的类名
 */
#define EMDEVIF_DELETE_MOVE_CONSTRUCTOR(ClassName) \
    ClassName(ClassName&&) = delete;               \
    ClassName& operator=(const ClassName&&) = delete

/**
 * 删除拷贝与移动构造函数及相关运算符
 * @param ClassName 待删除的类名
 */
#define EMDEVIF_DELETE_COPY_AND_MOVE(ClassName) \
    EMDEVIF_DELETE_COPY_CONSTRUCTOR(ClassName); \
    EMDEVIF_DELETE_MOVE_CONSTRUCTOR(ClassName)

/**
 * 声明为零例模式
 * @param ClassName 类名
 */
#define EMDEVIF_DECLARE_ZERO_INSTANCE(ClassName) \
    ClassName() = delete;                        \
    ~ClassName() = delete;                       \
    EMDEVIF_DELETE_COPY_AND_MOVE(ClassName)

/**
 * @def EMDEVIF_REASON_DELETE(reason)
 *
 * @brief 根据编译器支持情况删除函数，并提供删除原因（如果支持的话）
 * @param reason 删除原因字符串
 */
#ifdef __cpp_deleted_function
#define EMDEVIF_REASON_DELETE(reason) delete (reason)
#else
#define EMDEVIF_REASON_DELETE(reason) delete
#endif

#endif  // !EMDEVIF_CORE_SIMPLIFY_DECL_MACROS_HPP
