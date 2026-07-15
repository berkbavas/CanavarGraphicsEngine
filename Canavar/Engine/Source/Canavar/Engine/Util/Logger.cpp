#include "Logger.h"

#include <chrono>
#include <format>
#include <iostream>

void Canavar::Engine::Logger::Log(LogLevel LogLevel, const std::string& LogMessage)
{
    thread_local const auto TL_THREAD_ID = mLastThreadId.fetch_add(1);

    const auto Log = std::format("{:.12} [{:}] [ {:5} ] {:}", GetTimeString(), TL_THREAD_ID, GetLogLevelString(LogLevel), LogMessage);

    switch (LogLevel)
    {
    case LogLevel::TRACE:
    case LogLevel::DEBUG:

    case LogLevel::INFO:
        std::cout << Log << std::endl;
        break;
    case LogLevel::WARNING:
    case LogLevel::FATAL:
        std::cerr << Log << std::endl;
        break;
    default:
        break;
    }
}

void Canavar::Engine::Logger::SetLogLevel(LogLevel LogLevel)
{
    mLogLevel = LogLevel;
}

Canavar::Engine::LogLevel Canavar::Engine::Logger::GetLogLevel()
{
    return mLogLevel;
}

std::string Canavar::Engine::Logger::GetTimeString()
{
    const auto Zone = std::chrono::current_zone();
    const auto Now = std::chrono::system_clock::now();
    const auto Local = std::chrono::zoned_time(Zone, Now);

    return std::format("{:%T}", Local);
}

void Canavar::Engine::Logger::QtMessageOutputCallback(QtMsgType Type, const QMessageLogContext& Context, const QString& LogMessage)
{
    switch (Type)
    {
    case QtDebugMsg:
        LOG_DEBUG("{}", LogMessage.toStdString());
        break;
    case QtInfoMsg:
        LOG_INFO("{}", LogMessage.toStdString());
        break;
    case QtWarningMsg:
        LOG_WARN("{}", LogMessage.toStdString());
        break;
    case QtCriticalMsg:
        LOG_FATAL("{}", LogMessage.toStdString());
        break;
    case QtFatalMsg:
        LOG_FATAL("{}", LogMessage.toStdString());
        break;
    }
}

bool Canavar::Engine::Logger::isLogEnabledFor(LogLevel LogLevel)
{
    return mLogLevel <= LogLevel;
}

Canavar::Engine::LogLevel Canavar::Engine::Logger::mLogLevel = Canavar::Engine::LogLevel::ALL;

std::atomic_uint32_t Canavar::Engine::Logger::mLastThreadId = 0;
