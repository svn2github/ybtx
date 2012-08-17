#pragma once
#include "CCtrlPropBase.h"

namespace sqr_tools
{
	class CCombDataHelper;
	class ComBoCmd;
	class ComBoEditCtrlImp : public CCtrlPropBase
	{
		REG_UI_IMP(ComBoEditCtrlImp);
		friend class ComBoCmd;
	public:
		ComBoEditCtrlImp(void* param);
		~ComBoEditCtrlImp();

	public:
		virtual void				SetImpDataValue(int value);
		virtual int					GetImpDataValue();
		virtual CCombDataHelper*	GetImpDataItems();

	protected:
		virtual void				_SetImpDataValue(int value);
	};
}