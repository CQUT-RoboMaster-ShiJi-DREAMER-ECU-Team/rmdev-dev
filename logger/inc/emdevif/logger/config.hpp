/**
 * @file config.hpp
 * @brief
 */

#pragma once
#ifndef EMDEVIF_LOGGER_CONFIG_HPP
#define EMDEVIF_LOGGER_CONFIG_HPP

#ifdef EMDEVIF_LOGGER_CONFIG_FILE
#include EMDEVIF_LOGGER_CONFIG_FILE
#endif

#if (!((EMDEVIF_LOGGER_MODE) >= 0 && (EMDEVIF_LOGGER_MODE) <= 2))
#error "Invalid EMDEVIF_LOGGER_MODE value! It should be 0 (Sync), 1 (Async) or 2 (ExternalImpl)."
#endif

#ifndef EMDEVIF_LOGGER_IGNORE_LEVEL
#define EMDEVIF_LOGGER_IGNORE_LEVEL 2
#endif

#if (EMDEVIF_LOGGER_MODE == 0)  // Sync
#ifndef EMDEVIF_LOGGER_BUFFER_SIZE
#define EMDEVIF_LOGGER_BUFFER_SIZE 256U
#endif

#ifndef EMDEVIF_LOGGER_BUFFER_COUNT
#define EMDEVIF_LOGGER_BUFFER_COUNT 4
#endif

#ifndef EMDEVIF_LOGGER_SYNC_USE_LOCK
#define EMDEVIF_LOGGER_SYNC_USE_LOCK true
#endif

#ifdef EMDEVIF_LOGGER_ASYNC_THREAD_STACK_SIZE
#warning "EMDEVIF_LOGGER_ASYNC_THREAD_STACK_SIZE is defined but will be ignored because the logger mode is Sync."
#endif

#ifndef EMDEVIF_LOGGER_DYNAMIC_CREATE
#define EMDEVIF_LOGGER_DYNAMIC_CREATE false
#endif
#elif (EMDEVIF_LOGGER_MODE == 1)  // Async
#ifndef EMDEVIF_LOGGER_BUFFER_SIZE
#define EMDEVIF_LOGGER_BUFFER_SIZE 256U
#endif

#ifndef EMDEVIF_LOGGER_BUFFER_COUNT
#define EMDEVIF_LOGGER_BUFFER_COUNT 4
#endif

#ifdef EMDEVIF_LOGGER_SYNC_USE_LOCK
#warning "EMDEVIF_LOGGER_SYNC_USE_LOCK is defined but will be ignored because the logger mode is Async."
#endif

#ifndef EMDEVIF_LOGGER_ASYNC_THREAD_STACK_SIZE
#define EMDEVIF_LOGGER_ASYNC_THREAD_STACK_SIZE 128
#endif

#ifndef EMDEVIF_LOGGER_DYNAMIC_CREATE
#define EMDEVIF_LOGGER_DYNAMIC_CREATE false
#endif
#else  // ExternalImpl
#ifdef EMDEVIF_LOGGER_BUFFER_SIZE
#warning "EMDEVIF_LOGGER_BUFFER_SIZE is defined but will be ignored because the logger mode is ExternalImpl."
#endif

#ifdef EMDEVIF_LOGGER_BUFFER_COUNT
#warning "EMDEVIF_LOGGER_BUFFER_COUNT is defined but will be ignored because the logger mode is ExternalImpl."
#endif

#ifdef EMDEVIF_LOGGER_ASYNC_THREAD_STACK_SIZE
#warning \
    "EMDEVIF_LOGGER_ASYNC_THREAD_STACK_SIZE is defined but will be ignored because the logger mode is ExternalImpl."
#endif

#ifdef EMDEVIF_LOGGER_SYNC_USE_LOCK
#warning "EMDEVIF_LOGGER_SYNC_USE_LOCK is defined but will be ignored because the logger mode is ExternalImpl."
#endif

#ifdef EMDEVIF_LOGGER_DYNAMIC_CREATE
#warning "EMDEVIF_LOGGER_DYNAMIC_CREATE is defined but will be ignored because the logger mode is ExternalImpl."
#endif
#endif

#endif  // !EMDEVIF_LOGGER_CONFIG_HPP
