#include "Logger.h"

#include <chrono>
#include <format>
#include <iostream>

void Canavar::Engine::Logger::Log(LogLevel level, const std::string& message)
{
    thread_local const auto TL_THREAD_ID = mLastThreadId.fetch_add(1);

    const auto log = std::format("{:.12} [{:}] [ {:5} ] {:}", GetTimeString(), TL_THREAD_ID, GetLogLevelString(level), message);

    switch (level)
    {
    case LogLevel::TRACE:
    case LogLevel::DEBUG:

    case LogLevel::INFO:
        std::cout << log << std::endl;
        break;
    case LogLevel::WARNING:
    case LogLevel::FATAL:
        std::cerr << log << std::endl;
        break;
    default:
        break;
    }
}

void Canavar::Engine::Logger::SetLogLevel(LogLevel logLevel)
{
    mLogLevel = logLevel;
}

Canavar::Engine::LogLevel Canavar::Engine::Logger::GetLogLevel()
{
    return mLogLevel;
}

std::string Canavar::Engine::Logger::GetTimeString()
{
    const auto zone = std::chrono::current_zone();
    const auto now = std::chrono::system_clock::now();
    const auto local = std::chrono::zoned_time(zone, now);

    return std::format("{:%T}", local);
}

void Canavar::Engine::Logger::QtMessageOutputCallback(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    switch (type)
    {
    case QtDebugMsg:
        LOG_DEBUG("{}", msg.toStdString());
        break;
    case QtInfoMsg:
        LOG_INFO("{}", msg.toStdString());
        break;
    case QtWarningMsg:
        LOG_WARN("{}", msg.toStdString());
        break;
    case QtCriticalMsg:
        LOG_FATAL("{}", msg.toStdString());
        break;
    case QtFatalMsg:
        LOG_FATAL("{}", msg.toStdString());
        break;
    }
}

bool Canavar::Engine::Logger::isLogEnabledFor(LogLevel level)
{
    return mLogLevel <= level;
}

Canavar::Engine::LogLevel Canavar::Engine::Logger::mLogLevel = Canavar::Engine::LogLevel::ALL;

std::atomic_uint32_t Canavar::Engine::Logger::mLastThreadId = 0;

Canavar::Engine::EnterExitLogger::EnterExitLogger(const char* functionName, const char* fileName, int lineNumber)
    : mFunctionName(functionName)
    , mFileName(fileName)
    , mLineNumber(lineNumber)
{
    LOG_DEBUG("{} in '{}' at line {}: BEGIN", mFunctionName, mFileName, mLineNumber);
}

Canavar::Engine::EnterExitLogger::~EnterExitLogger()
{
    LOG_DEBUG("{} in '{}' at line {}: END", mFunctionName, mFileName, mLineNumber);
}
