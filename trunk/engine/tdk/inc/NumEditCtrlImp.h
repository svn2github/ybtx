#pragma once
#include "CCtrlPropBase.h"

namespace sqr_tools
{
	class CNumIntCmd;
	class CNumByteCmd;
	class CNumFloatCmd;
	class NumEditCtrlImp : public CCtrlPropBase
	{
		REG_UI_IMP(NumEditCtrlImp);
		friend class CNumIntCmd;
		friend class CNumByteCmd;
		friend class CNumFloatCmd;
	public:
		NumEditCtrlImp(void* param);
		~NumEditCtrlImp();

	public:
		virtual void SetImpIntValue(int i);
		virtual void SetImpByteValue(int i);
		virtual void SetImpFloatValue(float f);

		virtual int GetImpIntValue();
		virtual int GetImpByteValue();
		virtual float GetImpFloatValue();

	protected:
		virtual void _SetImpIntValue(int i);
		virtual void _SetImpByteValue(int i);
		virtual void _SetImpFloatValue(float f);
	};
}