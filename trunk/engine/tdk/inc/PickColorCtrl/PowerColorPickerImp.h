#pragma once

#include "CCtrlBase.h"
#include "CColor.h"

namespace sqr_tools
{
#define	MGT_UPDATE_KEY	0x01

	class CPowerColorPickerImp :public CCtrlBase
	{
		REG_UI_IMP(CPowerColorPickerImp);
	public:
		CPowerColorPickerImp(void* param);
		~CPowerColorPickerImp(void);

	private:
		CColor m_curPickColor;
	public:
		virtual void SetCurPickColor(const uint8 r, const uint8 g, const uint8 b);
		virtual void Update();
		virtual bool _ProcMsg( const CMsg& msg );
	};

}
