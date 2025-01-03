#pragma once

#include <assert.h>
#include <atomic>
#include <format>
#include <map>
#include <string>

#include <QDebug>

namespace Canavar::Engine
{
    enum class LogLevel
    {
        ALL = -1,
        TRACE = 0,
        DEBUG = 1,
        INFO = 2,
        WARNING = 3,
        FATAL = 4,
        NONE = 5
    };

    class Logger
    {
      public:
        Logger() = delete;

        static void Log(LogLevel level, const std::string& message);
        static void SetLogLevel(LogLevel logLevel);
        static bool isLogEnabledFor(LogLevel level);
        static LogLevel GetLogLevel();
        static std::string GetTimeString();

        static void QtMessageOutputCallback(QtMsgType type, const QMessageLogContext& context, const QString& msg);

        inline static std::string GetLogLevelString(LogLevel level)
        {
            switch (level)
            {
            case LogLevel::ALL:
                return "ALL";
            case LogLevel::NONE:
                return "NONE";
            case LogLevel::TRACE:
                return "TRACE";
            case LogLevel::DEBUG:
                return "DEBUG";
            case LogLevel::INFO:
                return "INFO";
            case LogLevel::WARNING:
                return "WARN";
            case LogLevel::FATAL:
                return "FATAL";
            default:
                return "N/A";
            }
        }

      private:
        static LogLevel mLogLevel;
        static std::atomic_uint32_t mLastThreadId;
    };

    class EnterExitLogger
    {
      public:
        explicit EnterExitLogger(const char* functionName, const char* fileName, int lineNumber);
        ~EnterExitLogger();

      private:
        const char* mFunctionName;
        const char* mFileName;
        int mLineNumber;
    };
}

#define LOG_PRIVATE(LEVEL, FORMAT, ...) \
    do \
    { \
        if (Canavar::Engine::Logger::isLogEnabledFor(LEVEL)) \
        { \
            Canavar::Engine::Logger::Log(LEVEL, std::format(FORMAT, __VA_ARGS__)); \
        } \
    } while (false)

#define LOG_TRACE(FORMAT, ...) LOG_PRIVATE(Canavar::Engine::LogLevel::TRACE, FORMAT, __VA_ARGS__)
#define LOG_DEBUG(FORMAT, ...) LOG_PRIVATE(Canavar::Engine::LogLevel::DEBUG, FORMAT, __VA_ARGS__)
#define LOG_INFO(FORMAT, ...) LOG_PRIVATE(Canavar::Engine::LogLevel::INFO, FORMAT, __VA_ARGS__)
#define LOG_WARN(FORMAT, ...) LOG_PRIVATE(Canavar::Engine::LogLevel::WARNING, FORMAT, __VA_ARGS__)
#define LOG_FATAL(FORMAT, ...) LOG_PRIVATE(Canavar::Engine::LogLevel::FATAL, FORMAT, __VA_ARGS__)

#define CGE_ASSERT(EXPRESSION) assert(EXPRESSION)

#define CGE_EXIT_FAILURE(FORMAT, ...) \
    do \
    { \
        LOG_FATAL(FORMAT, __VA_ARGS__); \
        std::exit(EXIT_FAILURE); \
    } while (false)

#define PRINT_ADDRESS(PTR) std::format("{:#010x}", reinterpret_cast<intptr_t>(PTR))

#define CGE_FILE_NAME (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#define CGE_LINE_NUMBER __LINE__

#define CGE_LOG_ENTER_EXIT(FUNCTION_NAME_PARAMETER) \
    static const std::string STATIC_STRING_CGE_FUNCTION_NAME{ FUNCTION_NAME_PARAMETER }; \
    static const std::string STATIC_STRING_CGE_FILE_NAME{ CGE_FILE_NAME }; \
    Canavar::Engine::EnterExitLogger CGE_ENTER_EXIT_LOGGER(STATIC_STRING_CGE_FUNCTION_NAME.data(), STATIC_STRING_CGE_FILE_NAME.data(), CGE_LINE_NUMBER)
