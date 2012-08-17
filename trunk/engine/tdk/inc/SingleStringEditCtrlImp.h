#pragma once
#include "CCtrlPropBase.h"

namespace sqr_tools
{
	class CSingleStrCmd;
	class SingleStringEditCtrlImp : public CCtrlPropBase
	{
		REG_UI_IMP(SingleStringEditCtrlImp);
		friend class CSingleStrCmd;
	public:
		SingleStringEditCtrlImp(void* param);
		~SingleStringEditCtrlImp();

	public:
		virtual void SetImpSingleStrValue(const char* c);
		virtual void SetImpStrWithFullPath(const char* c);
		virtual const char* GetImpSingleStrValue();
	private:
		virtual		void _SetImpSingleStrValue(const char* c);
		string		m_sTempStr;

	};
}