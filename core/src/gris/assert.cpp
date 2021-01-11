#include "gris/assert.h"

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <memory>

namespace
{

GRIS_PRINTF_FORMAT_ATTRIBUTE(1, 0)
int CalculateRequiredBufferSize(const char * format, va_list args);
GRIS_PRINTF_FORMAT_ATTRIBUTE(4, 0)
void FileLoggingCallback(FILE * outputFile, const char * file, uint32_t line, const char * format, va_list args);

}  // namespace

namespace Gris::Assert
{

//----------------------------------------------------------------------
GRIS_PRINTF_FORMAT_ATTRIBUTE(3, 0)
void StdoutLoggingCallback(const char * file, uint32_t line, const char * format, va_list args)
{
    FileLoggingCallback(stdout, file, line, format, args);
}

//----------------------------------------------------------------------
GRIS_PRINTF_FORMAT_ATTRIBUTE(3, 0)
void StderrLoggingCallback(const char * file, uint32_t line, const char * format, va_list args)
{
    FileLoggingCallback(stderr, file, line, format, args);
}

//----------------------------------------------------------------------
GRIS_PRINTF_FORMAT_ATTRIBUTE(3, 0)
void NullLoggingCallback(const char * file, uint32_t line, const char * format, va_list args)
{
    // NO-OP
    (void)file;
    (void)line;
    (void)format;
    (void)args;
}

//----------------------------------------------------------------------
void AbortHandler()
{
    abort();
}

//----------------------------------------------------------------------
void ThrowHandler()
{
    throw AssertionException();
}

//----------------------------------------------------------------------
void NullHandler()
{
    // NO-OP
}

namespace Detail
{

    //----------------------------------------------------------------------
    static AssertLoggingCallback sg_loggingCallback = &StderrLoggingCallback;
    static AssertHandler sg_failureHandler = &AbortHandler;

    //----------------------------------------------------------------------
    void AssertFired(const char * file, uint32_t line, const char * format, ...)
    {
        va_list args;
        va_start(args, format);
        (void)va_arg(args, const char *);  // Ignore the first argument which is format
        (*sg_loggingCallback)(file, line, format, args);
        va_end(args);
        (*sg_failureHandler)();
    }

}  // namespace Detail

AssertLoggingCallback SetLoggingCallback(AssertLoggingCallback callback)
{
    if (callback == nullptr)
        abort();

    auto prev = Detail::sg_loggingCallback;
    Detail::sg_loggingCallback = callback;
    return prev;
}

AssertHandler SetFailureHandler(AssertHandler handler)
{
    if (handler == nullptr)
        abort();

    auto prev = Detail::sg_failureHandler;
    Detail::sg_failureHandler = handler;
    return prev;
}

}  // namespace Gris::Assert

namespace
{

//----------------------------------------------------------------------
GRIS_PRINTF_FORMAT_ATTRIBUTE(1, 0)
int CalculateRequiredBufferSize(const char * format, va_list args)
{
    va_list argsCopy;
    va_copy(argsCopy, args);
    auto const sizeWithNul = vsnprintf(nullptr, 0, format, argsCopy) + 1;
    va_end(argsCopy);
    return sizeWithNul;
}

//----------------------------------------------------------------------
GRIS_PRINTF_FORMAT_ATTRIBUTE(4, 0)
void FileLoggingCallback(FILE * outputFile, const char * file, uint32_t line, const char * format, va_list args)
{
    auto const sizeWithNul = CalculateRequiredBufferSize(format, args);
    assert(sizeWithNul >= 0);
    auto buffer = std::make_unique<char[]>(static_cast<size_t>(sizeWithNul));
#ifdef _MSC_VER
    vsprintf_s(buffer.get(), sizeWithNul, format, args);
#else
    vsprintf(buffer.get(), format, args);
#endif

    fprintf(outputFile, "Assert failed %s(%ud): %s", file, line, buffer.get());
}

}  // namespace
