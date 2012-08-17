#pragma once

#if defined(_WIN32)
typedef DWORD			HTHREADID;
#else
typedef pid_t				HTHREADID;
#endif

namespace sqr
{
	///// @brief 得到本地主机名称
	const std::string GetComputerName();

	void AddWatchCpuThreadID(const char* szThreadName, HTHREADID tid);

	void vDebugOut(bool endLine, const char *format, va_list argptr);
	///// @brief 输出调试信息
	void DebugOut(const char *format,...);
	///// @brief 输出调试信息并自动换行
	void DebugOutLine(const char *format,...);

	bool vCompputerDebugOut(bool endLine, const std::string& user, const char* format, va_list argptr);
	///// @brief 只在给定的主机上输出调试信息
	bool CompputerDebugOut(const std::string& user, const char* format, ...);
	///// @brief 只在给定的主机上输出调试信息并自动换行
	bool CompputerDebugOutLine(const std::string& user, const char* format, ...);


	void OutputDebugLog(const std::string& log);
}

////////////////////////////////////////////////////////////////////////
/// @brief 所有TR*功能均为Debug下输出调试信息，Win下输出到Output窗口，
/// 	linux下输出到cerr。
/// @remark Release下这些宏将不会被编译，请注意TR*不编译可能导致的副作用
////////////////////////////////////////////////////////////////////////

/// @brief Trace
#if defined(_DEBUG) || defined(DEBUG)
#  ifdef _WIN32
#    define TR(...) sqr::DebugOut(__VA_ARGS__)
#  else
#    define TR(fmt, arg...) sqr::DebugOut(fmt, ##arg)
#  endif
#else
#  ifdef _WIN32
#    define TR(...) __noop
#  else
#    define TR(fmt, arg...)
#  endif
#endif

/// @brief Trace line
#if defined(_DEBUG) || defined(DEBUG)
#  ifdef _WIN32
#    define TRLINE(...) sqr::DebugOutLine(__VA_ARGS__)
#  else
#    define TRLINE(fmt, arg...) sqr::DebugOutLine(fmt, ##arg)
#  endif
#else
#  ifdef _WIN32
#    define TRLINE(...) __noop
#  else
#    define TRLINE(fmt, arg...)
#  endif
#endif

/// @brief Trace user
#if defined(_DEBUG) || defined(DEBUG)
#  ifdef _WIN32
#    define TRUSER(user, ...) sqr::CompputerDebugOut(user, __VA_ARGS__)
#  else
#    define TRUSER(user, fmt, arg...) sqr::CompputerDebugOut(user, fmt, ##arg)
#  endif
#else
#  ifdef _WIN32
#    define TRUSER(user, ...) __noop
#  else
#    define TRUSER(user, fmt, arg...)
#  endif
#endif

/// @brief Trace user line
#if defined(_DEBUG) || defined(DEBUG)
#  ifdef _WIN32
#    define TRUSERLINE(user, ...) sqr::CompputerDebugOutLine(user, __VA_ARGS__)
#  else
#    define TRUSERLINE(user, fmt, arg...) sqr::CompputerDebugOutLine(user, fmt, ##arg)
#  endif
#else
#  ifdef _WIN32
#    define TRUSERLINE(user, ...) __noop
#  else
#    define TRUSERLINE(user, fmt, arg...)
#  endif
#endif

/// @brief Trace if
#if defined(_DEBUG) || defined(DEBUG)
#  ifdef _WIN32
#    define TRIF(x, ...) if(x) sqr::DebugOut(__VA_ARGS__)
#  else
#    define TRIF(x, fmt, arg...) if(x) sqr::DebugOut(fmt, ##arg)
#  endif
#else
#  ifdef _WIN32
#    define TRIF(x, ...) __noop
#  else
#    define TRIF(x, fmt, arg...)
#  endif
#endif

/// @brief Trace line if
#if defined(_DEBUG) || defined(DEBUG)
#  ifdef _WIN32
#    define TRLINEIF(x, ...) if(x) sqr::DebugOutLine(__VA_ARGS__)
#  else
#    define TRLINEIF(x, fmt, arg...) if(x) sqr::DebugOutLine(fmt, ##arg)
#  endif
#else
#  ifdef _WIN32
#    define TRLINEIF(x, ...) __noop
#  else
#    define TRLINEIF(x, fmt, arg...)
#  endif
#endif

/// @brief Trace user if
#if defined(_DEBUG) || defined(DEBUG)
#  ifdef _WIN32
#    define TRUSERIF(x, user, ...) if(x) sqr::CompputerDebugOut(user, __VA_ARGS__)
#  else
#    define TRUSERIF(x, user, fmt, arg...) if(x) sqr::CompputerDebugOut(user, fmt, ##arg)
#  endif
#else
#  ifdef _WIN32
#    define TRUSERIF(x, user, ...) __noop
#  else
#    define TRUSERIF(x, user, fmt, arg...)
#  endif
#endif

/// @brief Trace user line if
#if defined(_DEBUG) || defined(DEBUG)
#  ifdef _WIN32
#    define TRUSERLINEIF(x, user, ...) if(x) sqr::CompputerDebugOutLine(user, __VA_ARGS__)
#  else
#    define TRUSERLINEIF(x, user, fmt, arg...) if(x) sqr::CompputerDebugOutLine(user, fmt, ##arg)
#  endif
#else
#  ifdef _WIN32
#    define TRUSERLINEIF(x, user, ...) __noop
#  else
#    define TRUSERLINEIF(x, user, fmt, arg...)
#  endif
#endif
