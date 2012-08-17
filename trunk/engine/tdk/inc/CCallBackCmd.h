#pragma once
#include "Cmd.h"
#include "CCtrlBase.h"

namespace sqr_tools
{
	typedef void CmdCallBack(void* param);
	class CCallBackCmd 
		: public CCmd
		, public CCtrlBase
	{
		REG_UI_IMP(CCallBackCmd);
	public:
		CCallBackCmd(void* pObject = NULL);
		~CCallBackCmd(void);
		virtual void SetCallBack(CmdCallBack* pExeFun,CmdCallBack* pUnExeFun,void* pObject = NULL);

		virtual void MgrUnDo(void);
		virtual void MgrReDo(void);
		virtual void Execute(void);
	protected:
		void _Execute(void);
		void _UnExecute(void);

		CmdCallBack*	m_pExeCallBack;
		CmdCallBack*	m_pUnExeCallBack;
		void*			m_pObject;
	};
}