#include "stdafx.h"
#include "TraceHelper.h"
#include "StringHelper.h"
#include <stdarg.h>
#include <iostream>
#include "CCpuUsage.h"
#ifndef _WIN32
#include <unistd.h>
#include <climits>
#else
#include <WinBase.h>
#endif

namespace sqr
{

	void AddWatchCpuThreadID(const char* szThreadName, HTHREADID tid)
	{
		CWatchCpuThreadID::GetInst()->AddWatchCpuThreadID(szThreadName,tid);
	}

	const std::string GetComputerName()
	{
#ifdef _WIN32
		DWORD MaxComputerLength=MAX_COMPUTERNAME_LENGTH; 
		CHAR ComputerName[MAX_COMPUTERNAME_LENGTH + 1]; 
		if(::GetComputerName(ComputerName, &MaxComputerLength))
			return ComputerName;
		else
			return "";
#else
		char ComputerName[HOST_NAME_MAX + 1];
		if(!gethostname(ComputerName, sizeof(ComputerName)))
			return ComputerName;
		else
			return "";
#endif
	}

	void vDebugOut(bool endLine, const char *format, va_list argptr)
	{
		std::string str;
		vformat(str, format, argptr);
		if(endLine)
			str.append("\n");
		sqr::OutputDebugLog(str);
	}
	void DebugOut(const char *format,...)
	{
		va_list argptr;
		va_start(argptr, format);
		sqr::vDebugOut(false, format, argptr);
		va_end(argptr);
	}
	void DebugOutLine(const char *format,...)
	{
		va_list argptr;
		va_start(argptr, format);
		sqr::vDebugOut(true, format, argptr);
		va_end(argptr);
	}

	bool vCompputerDebugOut(bool endLine, const std::string& user, const char* format, va_list argptr)
	{
		static std::string computerName = sqr::GetComputerName();
		if(icompare(user, computerName) != 0)
			return false;

		std::string str;
		vformat(str, format, argptr);

		std::string cp_user(user);
		str.insert(0, toupper(cp_user).append(" "));
		if(endLine)
			str.append("\n");

		sqr::OutputDebugLog(str);
		return true;
	}
	bool CompputerDebugOut(const std::string& user, const char* format, ...)
	{
		va_list argptr;
		va_start(argptr, format);
		bool ret = sqr::vCompputerDebugOut(false, user, format, argptr);
		va_end(argptr);
		return ret;
	}
	bool CompputerDebugOutLine(const std::string& user, const char* format, ...)
	{
		va_list argptr;
		va_start(argptr, format);
		bool ret = sqr::vCompputerDebugOut(true, user, format, argptr);
		va_end(argptr);
		return ret;
	}

#ifdef _WIN32

	extern "C"
	{
		/// @brief 该函数的声明居然在 WinBase.h : 3205 中没有被正确打开？
		///		VS2008中该bug已经消除，是因为正确的定义_WIN32_WINNT宏
		WINBASEAPI BOOL WINAPI IsDebuggerPresent(VOID);
	}
	void OutputDebugLog(const std::string& log)
	{
#if 0
		if(IsDebuggerPresent())
			::OutputDebugString(log.c_str());
		else
			std::cerr << log;
#else
		::OutputDebugString(log.c_str());
#endif
	}

#else

	void OutputDebugLog(const std::string& log)
	{
		std::cerr << log;
	}

#endif
}
