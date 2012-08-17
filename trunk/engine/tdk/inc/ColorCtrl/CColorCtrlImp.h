#pragma once

#include "CCtrlBase.h"

namespace sqr_tools
{
	#define	MGT_UPDATE_COLOR	0x01

	class CColorCtrlImp :public CCtrlBase
	{
		REG_UI_IMP(CColorCtrlImp);

	public:
		CColorCtrlImp(void* param);
		~CColorCtrlImp(void);

	private:
		DWORD m_dwColor;

	public:
 		virtual void SetColor(const DWORD& color);
 
 		virtual DWORD GetColor()
 		{
 			return m_dwColor;
 		}
 
 		virtual void Update();
 		virtual bool _ProcMsg(const CMsg& msg );
	};

}
