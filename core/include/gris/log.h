/*
 * Copyright (c) 2020 Bartlomiej Siwek All rights reserved.
 */

#pragma once

#define GRIS_LEVEL_TRACE 0
#define GRIS_LEVEL_DEBUG 1
#define GRIS_LEVEL_INFO 2
#define GRIS_LEVEL_WARN 3
#define GRIS_LEVEL_ERROR 4
#define GRIS_LEVEL_CRITICAL 5
#define GRIS_LEVEL_OFF 6

#ifdef GRIS_ACTIVE_LOG_LEVEL
#define SPDLOG_ACTIVE_LEVEL GRIS_ACTIVE_LOG_LEVEL
#endif

#include <spdlog/spdlog.h>

#if GRIS_ACTIVE_LOG_LEVEL <= GRIS_LEVEL_TRACE
#define GRIS_TRACE(...) SPDLOG_TRACE(__VA_ARGS__)
#else
#define GRIS_TRACE(...) (void)(__VA_ARGS__)
#endif

#if GRIS_ACTIVE_LOG_LEVEL <= GRIS_LEVEL_DEBUG
#define GRIS_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
#else
#define GRIS_DEBUG(...) (void)(__VA_ARGS__)
#endif

#if GRIS_ACTIVE_LOG_LEVEL <= GRIS_LEVEL_INFO
#define GRIS_INFO(...) SPDLOG_INFO(__VA_ARGS__)
#else
#define GRIS_INFO(...) (void)(__VA_ARGS__)
#endif

#if GRIS_ACTIVE_LOG_LEVEL <= GRIS_LEVEL_WARN
#define GRIS_WARNING(...) SPDLOG_WARN(__VA_ARGS__)
#else
#define GRIS_WARNING(...) (void)(__VA_ARGS__)
#endif

#if GRIS_ACTIVE_LOG_LEVEL <= GRIS_LEVEL_ERROR
#define GRIS_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)
#else
#define GRIS_ERROR(...) (void)(__VA_ARGS__)
#endif

#if GRIS_ACTIVE_LOG_LEVEL <= GRIS_LEVEL_CRITICAL
#define GRIS_CRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__)
#else
#define GRIS_CRITICAL(...) (void)(__VA_ARGS__)
#endif

namespace Gris::Log
{

enum class Level
{
    Trace = spdlog::level::trace,
    Debug = spdlog::level::debug,
    Info = spdlog::level::info,
    Warning = spdlog::level::warn,
    Error = spdlog::level::err,
    Critical = spdlog::level::critical,
    Off = spdlog::level::off,
};

inline void SetLogLevel(Level logLevel)
{
    spdlog::set_level(static_cast<spdlog::level::level_enum>(logLevel));
}

template<typename FormatString, typename... Args>
void Trace(const FormatString & format, Args &&... args)
{
    spdlog::trace(format, std::forward<Args>(args)...);
}

template<typename FormatString, typename... Args>
void Debug(const FormatString & format, Args &&... args)
{
    spdlog::debug(format, std::forward<Args>(args)...);
}

template<typename FormatString, typename... Args>
void Info(const FormatString & format, Args &&... args)
{
    spdlog::info(format, std::forward<Args>(args)...);
}

template<typename FormatString, typename... Args>
void Warning(const FormatString & format, Args &&... args)
{
    spdlog::warn(format, std::forward<Args>(args)...);
}

template<typename FormatString, typename... Args>
void Error(const FormatString & format, Args &&... args)
{
    spdlog::error(format, std::forward<Args>(args)...);
}

template<typename FormatString, typename... Args>
void Critical(const FormatString & format, Args &&... args)
{
    spdlog::critical(format, std::forward<Args>(args)...);
}

}