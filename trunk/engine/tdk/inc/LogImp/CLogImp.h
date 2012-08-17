#pragma once
#include "CCtrlBase.h"
#include "CTick.h"
#include "TSingleton.h"

namespace sqr_tools
{
	class CLogCtrlImp : public CCtrlBase
	{
		REG_UI_IMP(CLogCtrlImp);
	public:
		CLogCtrlImp(void* param);
		~CLogCtrlImp(void);
	
		virtual void ShowLog(const string& log);	
		virtual const string& GetLog(void);
		virtual void Clear(void);

	protected:
		string	m_Log;
	};

	class CSystemLogImp 
		: public CLogCtrlImp
		, public CTick
		, public TSingleton<CSystemLogImp>
	{
		REG_UI_IMP(CSystemLogImp);
	public:
		CSystemLogImp(void);
		~CSystemLogImp(void);

		virtual void	OnTick(void);
	protected:
		string			m_logFile;
	};
}