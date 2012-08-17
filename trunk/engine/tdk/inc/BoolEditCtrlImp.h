#pragma once
#include "CCtrlPropBase.h"

namespace sqr_tools
{
	class CBoolEditCmd;
	class BoolEditCtrlImp : public CCtrlPropBase
	{
		REG_UI_IMP(BoolEditCtrlImp);
		friend class CBoolEditCmd;
	public:
		BoolEditCtrlImp(void* param);
		~BoolEditCtrlImp();
	
	public:
		virtual void SetImpBoolValue(bool b);
		virtual bool GetImpBoolValue();

	protected:
		virtual void _SetBoolValue(bool b);
	};
}