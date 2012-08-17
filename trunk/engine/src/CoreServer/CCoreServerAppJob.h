#pragma once
#include "CCoreServerJob.h"

namespace sqr
{

	class CCoreServerAppQuitJob : public CCoreServerJob
	{
	public:
		CCoreServerAppQuitJob();
		~CCoreServerAppQuitJob();
	};

	class CCoreServerAppCleanJob : public CCoreServerJob
	{
	public:
		CCoreServerAppCleanJob();
		~CCoreServerAppCleanJob();
	};

	class CCorePrintRegisterTickNumJob : public CCoreServerJob
	{
	public:
		CCorePrintRegisterTickNumJob(size_t stCppTickNum, size_t stLuaTickNum);
		~CCorePrintRegisterTickNumJob();

	private:
		size_t	m_stCppTickNum;
		size_t	m_stLuaTickNum;
	};

	class CCorePrintRegisterTickInfoJob : public CCoreServerJob
	{
	public:
		CCorePrintRegisterTickInfoJob(std::string* strInfo);
		~CCorePrintRegisterTickInfoJob();

	private:
		std::string*		m_strInfo;;
	};
}
