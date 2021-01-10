/*
 * Copyright (c) 2018 Bartlomiej Siwek All rights reserved.
 */

#pragma once

#include <gris/macros.h>

#include <cstdint>
#include <stdexcept>

#if defined(GRIS_TU_MODE_NONE) || defined(GRIS_TU_MODE_ALWAYS) || defined(GRIS_TU_MODE_DISABLE_ALWAYS)
#if !defined(GRIS_TU_MODE_NONE) && defined(GRIS_TU_MODE_ALWAYS) && !defined(GRIS_TU_MODE_DISABLE_ALWAYS)
#define GRIS_ALWAYS_ASSERT_IS_ACTIVE
#else
#undef GRIS_ALWAYS_ASSERT_IS_ACTIVE
#endif
#else
#if !defined(GRIS_TARGET_MODE_NONE)
#define GRIS_ALWAYS_ASSERT_IS_ACTIVE
#else
#undef GRIS_ALWAYS_ASSERT_IS_ACTIVE
#endif
#endif


#if defined(GRIS_TU_MODE_NONE) || defined(GRIS_TU_MODE_FAST) || defined(GRIS_TU_MODE_DISABLE_FAST)
#if !defined(GRIS_TU_MODE_NONE) && defined(GRIS_TU_MODE_FAST) && !defined(GRIS_TU_MODE_DISABLE_FAST)
#define GRIS_FAST_ASSERT_IS_ACTIVE
#else
#undef GRIS_FAST_ASSERT_IS_ACTIVE
#endif
#else
#if !defined(GRIS_TARGET_MODE_NONE) && (defined(GRIS_TARGET_MODE_DEBUG) || defined(GRIS_TARGET_MODE_SAFE))
#define GRIS_FAST_ASSERT_IS_ACTIVE
#else
#undef GRIS_FAST_ASSERT_IS_ACTIVE
#endif
#endif

#if defined(GRIS_TU_MODE_NONE) || defined(GRIS_TU_MODE_SLOW) || defined(GRIS_TU_MODE_DISABLE_SLOW)
#if !defined(GRIS_TU_MODE_NONE) && defined(GRIS_TU_MODE_SLOW) && !defined(GRIS_TU_MODE_DISABLE_SLOW)
#define GRIS_SLOW_ASSERT_IS_ACTIVE
#else
#undef GRIS_SLOW_ASSERT_IS_ACTIVE
#endif
#else
#if !defined(GRIS_TARGET_MODE_NONE) && defined(GRIS_TARGET_MODE_SAFE)
#define GRIS_SLOW_ASSERT_IS_ACTIVE
#else
#undef GRIS_SLOW_ASSERT_IS_ACTIVE
#endif
#endif

namespace Gris::Assert
{

class AssertionException : public std::logic_error
{
public:
    AssertionException()
        : logic_error("Assertion failed")
    {
    }
};

using AssertLoggingCallback = void (*)(const char * file, uint32_t line, const char * format, va_list args);
using AssertHandler = void (*)();

AssertLoggingCallback SetLoggingCallback(AssertLoggingCallback callback);
AssertHandler SetFailureHandler(AssertHandler handler);

GRIS_PRINTF_FORMAT_ATTRIBUTE(3, 0)
void StdoutLoggingCallback(const char * file, uint32_t line, const char * format, va_list args);
GRIS_PRINTF_FORMAT_ATTRIBUTE(3, 0)
void StderrLoggingCallback(const char * file, uint32_t line, const char * format, va_list args);
GRIS_PRINTF_FORMAT_ATTRIBUTE(3, 0)
void NullLoggingCallback(const char * file, uint32_t line, const char * format, va_list args);

void AbortHandler();
void ThrowHandler();
void NullHandler();

namespace Detail
{

    void AssertFired(const char * file, uint32_t line, const char * format, ...);

    template<typename... T>
    void Ignore(T const &...) noexcept
    {
    }

    template<typename T>
    bool MaybeCastToBool(T&& value)
    {
        if constexpr (std::is_same_v<bool, T>)
            return value;
        else
            return static_cast<bool>(std::forward<T>(value));
    }

}  // namespace Detail
}  // namespace Gris::Assert

#ifdef _MSC_VER
#define GRIS_DEBUGBREAK __debugbreak()
#else
#include <signal.h>
#define GRIS_DEBUGBREAK raise(SIGTRAP)
#endif

#define GRIS_ASSERT_EXPAND(arg) arg
#define GRIS_ASSERT_FIRST(first, ...) first

#define GRIS_ASSERT_IMPL(condition, ...)                                                                                                   \
    do                                                                                                                                     \
    {                                                                                                                                      \
        if (!Gris::Assert::Detail::MaybeCastToBool(condition))                                                                             \
        {                                                                                                                                  \
            Gris::Assert::Detail::AssertFired(                                                                                             \
                __FILE__,                                                                                                                  \
                __LINE__,                                                                                                                  \
                "Assertion [" #condition "] failed. " GRIS_ASSERT_EXPAND(GRIS_ASSERT_FIRST(__VA_ARGS__, GRIS_ASSERT_DUMMY_ARG)),           \
                __VA_ARGS__                                                                                                                \
            );                                                                                                                              \
            GRIS_DEBUGBREAK;                                                                                                               \
        }                                                                                                                                  \
    } while (false)

#define GRIS_ASSERT_IGNORE_IMPL(condition, ...) Gris::Assert::Detail::Ignore(condition, __VA_ARGS__);

#ifdef GRIS_ALWAYS_ASSERT_IS_ACTIVE
#define GRIS_ALAWYS_ASSERT(condition, ...) GRIS_ASSERT_IMPL(condition, __VA_ARGS__)
#else
#define GRIS_ALAWYS_ASSERT(condition, ...) GRIS_ASSERT_IGNORE_IMPL(condition, __VA_ARGS__);
#endif

#ifdef GRIS_FAST_ASSERT_IS_ACTIVE
#define GRIS_FAST_ASSERT(condition, ...) GRIS_ASSERT_IMPL(condition, __VA_ARGS__)
#else
#define GRIS_FAST_ASSERT(condition, ...) GRIS_ASSERT_IGNORE_IMPL(condition, __VA_ARGS__);
#endif

#ifdef GRIS_SLOW_ASSERT_IS_ACTIVE
#define GRIS_SLOW_ASSERT(condition, ...) GRIS_ASSERT_IMPL(condition, __VA_ARGS__)
#else
#define GRIS_SLOW_ASSERT(condition, ...) GRIS_ASSERT_IGNORE_IMPL(condition, __VA_ARGS__);
#endif

#if defined(GRIS_TARGET_MODE_NONE) && defined(GRIS_TARGET_MODE_RELEASE)
#error "GRIS_TARGET_MODE_NONE and GRIS_TARGET_MODE_RELEASE cannot be defined at the same time."
#endif

#if defined(GRIS_TARGET_MODE_NONE) && defined(GRIS_TARGET_MODE_DEBUG)
#error "GRIS_TARGET_MODE_NONE and GRIS_TARGET_MODE_DEBUG cannot be defined at the same time."
#endif

#if defined(GRIS_TARGET_MODE_NONE) && defined(GRIS_TARGET_MODE_SAFE)
#error "GRIS_TARGET_MODE_NONE and GRIS_TARGET_MODE_SAFE cannot be defined at the same time."
#endif

#if defined(GRIS_TARGET_MODE_RELEASE) && defined(GRIS_TARGET_MODE_DEBUG)
#error "GRIS_TARGET_MODE_RELEASE and GRIS_TARGET_MODE_DEBUG cannot be defined at the same time."
#endif

#if defined(GRIS_TARGET_MODE_RELEASE) && defined(GRIS_TARGET_MODE_SAFE)
#error "GRIS_TARGET_MODE_RELEASE and GRIS_TARGET_MODE_SAFE cannot be defined at the same time."
#endif

#if defined(GRIS_TARGET_MODE_DEBUG) && defined(GRIS_TARGET_MODE_SAFE)
#error "GRIS_TARGET_MODE_DEBUG and GRIS_TARGET_MODE_SAFE cannot be defined at the same time."
#endif

#if defined(GRIS_TU_MODE_NONE) && (defined(GRIS_TU_MODE_ALWAYS) || defined(GRIS_TU_MODE_DISABLE_ALWAYS) || defined(GRIS_TU_MODE_FAST) || defined(GRIS_TU_MODE_DISABLE_FAST) || defined(GRIS_TU_MODE_SLOW) || defined(GRIS_TU_MODE_DISABLE_SLOW))
#error "GRIS_TU_MODE_NONE cannot be mixed with any other GRIS_TU_MODE_* defines."
#endif

#if defined(GRIS_TU_MODE_ALWAYS) && defined(GRIS_TU_MODE_DISABLE_ALWAYS)
#error "GRIS_TU_MODE_ALWAYS and GRIS_TU_MODE_DISABLE_ALWAYS cannot be defined at the same time."
#endif

#if defined(GRIS_TU_MODE_FAST) && defined(GRIS_TU_MODE_DISABLE_FAST)
#error "GRIS_TU_MODE_FAST and GRIS_TU_MODE_DISABLE_FAST cannot be defined at the same time."
#endif

#if defined(GRIS_TU_MODE_SLOW) && defined(GRIS_TU_MODE_DISABLE_SLOW)
#error "GRIS_TU_MODE_SLOW and GRIS_TU_MODE_DISABLE_SLOW cannot be defined at the same time."
#endif
