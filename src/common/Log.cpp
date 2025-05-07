#include "Log.h"

#if defined(ANDROID)
#include <android/log.h>
#endif

namespace rhi
{
    const char* SeverityName(LogSeverity severity)
    {
        switch (severity) {
        case LogSeverity::Debug:
            return "Debug";
        case LogSeverity::Info:
            return "Info";
        case LogSeverity::Warning:
            return "Warning";
        case LogSeverity::Error:
            return "Error";
        default:
            return "";
        }
    }

#if defined (ANDROID)
    android_LogPriority AndroidLogPriority(LogSeverity severity)
    {
        switch (severity) {
        case LogSeverity::Debug:
            return ANDROID_LOG_INFO;
        case LogSeverity::Info:
            return ANDROID_LOG_INFO;
        case LogSeverity::Warning:
            return ANDROID_LOG_WARN;
        case LogSeverity::Error:
            return ANDROID_LOG_ERROR;
        default:
            return ANDROID_LOG_ERROR;
        }
    }
#endif

	LogMessage::LogMessage(LogSeverity severity) : mSeverity(severity) {}

	LogMessage::LogMessage(LogMessage&& other) = default;

	LogMessage& LogMessage::operator=(LogMessage&& other) = default;

	LogMessage::~LogMessage()
	{
        std::string fullMessage = mStream.str();

        // If this message has been moved, its stream is empty.
        if (fullMessage.empty()) {
            return;
        }

        const char* severityName = SeverityName(mSeverity);

#if defined(ANDROID)
        android_LogPriority androidPriority = AndroidLogPriority(mSeverity);
        __android_log_print(androidPriority, "Dawn", "%s: %s\n", severityName, fullMessage.c_str());
#else
        FILE* outputStream = stdout;
        if (mSeverity == LogSeverity::Warning || mSeverity == LogSeverity::Error) 
        {
            outputStream = stderr;
        }
#endif // ANDROID

        // Note: we use fprintf because <iostream> includes static initializers.
        fprintf(outputStream, "%s: %s\n", severityName, fullMessage.c_str());
        fflush(outputStream);
	}

    LogMessage DebugLog()
    {
        return LogMessage(LogSeverity::Debug);
    }

    LogMessage InfoLog()
    {
        return LogMessage(LogSeverity::Info);
    }

    LogMessage WarningLog()
    {
        return LogMessage(LogSeverity::Warning);
    }

    LogMessage ErrorLog() 
    {
        return LogMessage(LogSeverity::Error);
    }

    LogMessage DebugLog(const char* file, const char* function, int line)
    {
        LogMessage message = DebugLog();
        message << file << ":" << line << "(" << function << ")";
        return message;
    }
}