#include <gris/log.h>
#include <iostream>

template<typename TLoggerImpl>
concept LoggerLike = requires(TLoggerImpl log)
{
    log.LogDebug(std::string_view{});
    log.LogInfo(std::string_view{});
    log.LogError(std::string_view{});
};

template<LoggerLike TLoggerImpl>
struct Logger : public TLoggerImpl
{};

struct MyLoggerImpl
{
    void LogDebug(std::string_view message) const
    {
        std::cout << "[My Debug] " << message << '\n';
    }
    void LogInfo(std::string_view message) const
    {
        std::cout << "[My Info] " << message << '\n';
    }
    void LogError(std::string_view message) const
    {
        std::cout << "[My Error] " << message << '\n';
    }
};

static_assert(LoggerLike<MyLoggerImpl>);  // This works too

using MyLogger = Logger<MyLoggerImpl>;

int main()
{
    LoggerLike auto logger = MyLogger{};
    logger.LogDebug("Hello demo!");
    return EXIT_SUCCESS;
}
