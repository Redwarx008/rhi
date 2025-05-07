#pragma once


#include "../RHIStruct.h"
#include "Utils.h"
#include <iostream>
#include <absl/strings/str_format.h>

namespace rhi::impl
{
	extern LoggingCallback gDebugMessageCallback;
	extern void* gDebugMessageCallbackUserData;

	template<typename... ArgsType>
	void LogMsg(LoggingSeverity severity, const char* file, const char* functionName, int line, const ArgsType&... args)
	{
		std::string prefix;
		if (severity == LoggingSeverity::Error)
		{
			prefix = "Error";
#if defined(_WIN32)
			prefix = "\033[31m" + prefix + "\033[0m";
#endif
		}
		else if (severity == LoggingSeverity::Warning)
		{
			prefix = "Warning";
#if defined(_WIN32)
			prefix = "\033[33m" + prefix + "\033[0m";
#endif
		}
		std::stringstream ss;
		auto msg = combineString(args...);
		ss << prefix << " in " << file << ":" << functionName << " at line " << line << " : " << msg << '\n';
		if (gDebugMessageCallback != nullptr)
		{
			gDebugMessageCallback(severity, ss.str().c_str(), gDebugMessageCallbackUserData);
		}
		else
		{
			std::cerr << ss.str();
		}
		if (severity >= LoggingSeverity::Error)
		{
			abort();
		}
	}

#define ASSERT_CALLSITE_HELPER(file, func, line, condition) \
	do                                    \
	{										\
		if(!(condition))					\
		{									\
			LogMsg(LoggingSeverity::Error, file, func, line, #condition);	\
			abort();							\
		}									\
	} while(false)							\

#define LOG_ERROR(...)																			 \
	do																							 \
	{																						     \
         LogMsg(LoggingSeverity::Error, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);                \
	} while (false)																				 \



#define LOG_WARNING(...)																			 \
	do																							 \
	{																						     \
         LogMsg(LoggingSeverity::Warning, __FILE__, __FUNCTION__, __LINE__, absl::StrFormat(##__VA_ARGS__));                \
	} while (false)																				 \




#ifdef NDEBUG
#define ASSERT_MSG(EXPR, ...) ((void)0)
#else
#define ASSERT_MSG(EXPR, ...)   \
			do					\
			{					\
				if(!(EXPR))		\
				{				\
					LogMsg(LoggingSeverity::Error, __FILE__,  __FUNCTION__, __LINE__, absl::StrFormat(__VA_ARGS__));	\
                    abort();\
				}	        \
			} while (false)		
#endif // NDEBUG


#ifdef NDEBUG
#define ASSERT(condition) ((void)0)
#else
#define ASSERT(condition) ASSERT_CALLSITE_HELPER(__FILE__, __func__, __LINE__, condition)
#endif


#ifdef NDEBUG
#define INVALID_IF(EXPR, ...) ((void)0)
#else
#define INVALID_IF(EXPR, ...)   \
			do					\
			{					\
				if(EXPR)		\
				{				\
					LogMsg(LoggingSeverity::Error, __FILE__, __FUNCTION__, __LINE__, absl::StrFormat(__VA_ARGS__));	\
                    abort();\
				}	        \
			} while (false)		
#endif // NDEBUG


}