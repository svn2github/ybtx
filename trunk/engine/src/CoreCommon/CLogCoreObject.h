#pragma once

namespace sqr
{
	class CLog;
	class CLogCoreObject
	{
	public:
		static CLogCoreObject& Inst();
		
		void LogMsg(const string& strMsg);

	private:
		CLogCoreObject();
		~CLogCoreObject();

	private:
		CLog*	m_pLog;
	};

//#define LOG_COREOBJ_MOVE
}

