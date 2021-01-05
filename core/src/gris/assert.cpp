#include "gris/assert.h"

#include <cstdarg>
#include <cstdio>
#include <memory>

namespace
{

void FileLoggigCallback(FILE* outputFile, const char* file, uint32_t line, const char* format, va_list args);

}  // namespace <anonymous>

namespace Gris
{
namespace Assert
{

//----------------------------------------------------------------------
void StdoutLoggigCallback(const char* file, uint32_t line, const char* format, va_list args)
{
  FileLoggigCallback(stdout, file, line, format, args);
}

//----------------------------------------------------------------------
void StderrLoggigCallback(const char* file, uint32_t line, const char* format, va_list args)
{
  FileLoggigCallback(stderr, file, line, format, args);
}

//----------------------------------------------------------------------
void NullLoggigCallback(const char* file, uint32_t line, const char* format, va_list args)
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
static AssertLoggingCallback sg_loggingCallback = &StderrLoggigCallback;
static AssertHandler sg_failureHandler = &AbortHandler;

//----------------------------------------------------------------------
void AssertFired(const char* file, uint32_t line, const char* format, ...)
{
  va_list args;
  va_start(args, format);
  (*sg_loggingCallback)(file, line, format, args);
  va_end(args);

  (*sg_failureHandler)();
}

}  // namespace Detail

AssertLoggingCallback SetLoggingCallback(AssertLoggingCallback callback)
{
  if(callback == nullptr)
  {
    abort();
  }

  auto prev = Detail::sg_loggingCallback;
  Detail::sg_loggingCallback = callback;
  return prev;
}

AssertHandler SetFailureHandler(AssertHandler handler)
{
  if(handler == nullptr)
  {
    abort();
  }

  auto prev = Detail::sg_failureHandler;
  Detail::sg_failureHandler = handler;
  return prev;
}

}  // namespace Assert
}  // namespace Gris

namespace
{

//----------------------------------------------------------------------
void FileLoggigCallback(FILE* outputFile, const char* file, uint32_t line, const char* format, va_list args)
{
  auto const sizeWithNul = vsnprintf(nullptr, 0, format, args) + 1;
  auto buffer = std::make_unique<char[]>(sizeWithNul);
  vsprintf_s(buffer.get(), sizeWithNul, format, args);

  fprintf(outputFile, "Assert failed %s(%ud): %s", file, line, buffer.get());
}

}  // namespace <anonymous>
