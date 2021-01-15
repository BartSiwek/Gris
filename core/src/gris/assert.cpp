#include <gris/assert.h>

#include <gris/macros.h>
#include <gris/log.h>

#include <iostream>
#include <memory>

namespace Gris::Assert
{

// -------------------------------------------------------------------------------------------------
void StdoutLoggingCallback(std::string message)
{
    std::cout << message;
}

// -------------------------------------------------------------------------------------------------
void StderrLoggingCallback(std::string message)
{
    std::cerr << message;
}

// -------------------------------------------------------------------------------------------------
void LogCriticalCallback(std::string message)
{
    Log::Critical(message);
}

// -------------------------------------------------------------------------------------------------
void NullLoggingCallback(std::string message)
{
    // NO-OP
    (void)message;
}

// -------------------------------------------------------------------------------------------------
void AbortHandler()
{
    GRIS_DEBUGBREAK();
    abort();
}

// -------------------------------------------------------------------------------------------------
void ThrowHandler()
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
    static AssertLoggingCallback sg_loggingCallback = &StderrLoggingCallback;
    static AssertHandler sg_failureHandler = &AbortHandler;

    // -------------------------------------------------------------------------------------------------
    void InvokeLoggingCallback(std::string message)
    {
        (*sg_loggingCallback)(std::move(message));
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
        abort();

    auto prev = Detail::sg_loggingCallback;
    Detail::sg_loggingCallback = callback;
    return prev;
}

// -------------------------------------------------------------------------------------------------
AssertHandler SetFailureHandler(AssertHandler handler)
{
    if (handler == nullptr)
        abort();

    auto prev = Detail::sg_failureHandler;
    Detail::sg_failureHandler = handler;
    return prev;
}

}  // namespace Gris::Assert
