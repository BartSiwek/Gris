#include <gris/assert.h>

#include <gris/log.h>
#include <gris/macros.h>

#include <iostream>

namespace Gris::Assert
{

// -------------------------------------------------------------------------------------------------
void StdoutLoggingCallback(const std::string & message)
{
    std::cout << message;
}

// -------------------------------------------------------------------------------------------------
void StderrLoggingCallback(const std::string & message)
{
    std::cerr << message;
}

// -------------------------------------------------------------------------------------------------
void LogCriticalCallback(const std::string & message)
{
    Log::Critical(message);
}

// -------------------------------------------------------------------------------------------------
void NullLoggingCallback(const std::string & message)
{
    // NO-OP
    (void)message;
}

// -------------------------------------------------------------------------------------------------
[[noreturn]] void AbortHandler()
{
    GRIS_DEBUGBREAK();
    abort();
}

// -------------------------------------------------------------------------------------------------
[[noreturn]] void ThrowHandler()
{
    throw AssertionException();
}

// -------------------------------------------------------------------------------------------------
void NullHandler()
{
    // NO-OP
}

namespace Detail
{

// -------------------------------------------------------------------------------------------------
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
static AssertLoggingCallback sg_loggingCallback = &LogCriticalCallback;

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
static AssertHandler sg_failureHandler = &AbortHandler;

// -------------------------------------------------------------------------------------------------
void InvokeLoggingCallback(const std::string & message)
{
    (*sg_loggingCallback)(message);
}

// -------------------------------------------------------------------------------------------------
void InvokeFailureHandler()
{
    (*sg_failureHandler)();
}

}  // namespace Detail

// -------------------------------------------------------------------------------------------------
AssertLoggingCallback SetLoggingCallback(AssertLoggingCallback callback)
{
    if (callback == nullptr)
    {
        abort();
    }

    auto const prev = Detail::sg_loggingCallback;
    Detail::sg_loggingCallback = callback;
    return prev;
}

// -------------------------------------------------------------------------------------------------
AssertHandler SetFailureHandler(AssertHandler handler)
{
    if (handler == nullptr)
    {
        abort();
    }

    auto const prev = Detail::sg_failureHandler;
    Detail::sg_failureHandler = handler;
    return prev;
}

}  // namespace Gris::Assert
