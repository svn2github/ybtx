#pragma once

#include "CCtrlBase.h"

namespace sqr_tools
{
	class CColorCtrlImp;
	enum FogModeType
	{
		FMT_NONE	= 0,
		FMT_EXP		= 1,
		FMT_EXP2	= 2,
		FMT_LINEAR	= 3,
		FMT_FORCE_DWORD = 0x7fffffff,
	};

	#define	MGT_UPDATE_FOGCOLOR	0x01
	#define	MGT_UPDATE_FOGPARA	0x02

	#define FOGDENSITY	0.0002f
	#define FOGSTART	1020
	#define FOGEND		8000
	#define FOGMAXSTART 6000
	#define FOGMAXEND	30000

	class CFogParameterCtrlImp :public CCtrlBase
	{
		REG_UI_IMP(CFogParameterCtrlImp);
	public:
		CFogParameterCtrlImp(void* param);
		~CFogParameterCtrlImp(void);

	private:
		CColorCtrlImp*	m_pFogColorCtrlImp;
		FogModeType		m_eFogType;
		bool			m_bEnableFog;
		int				m_nFogStart;
		int				m_nFogEnd;
		float			m_fFogDesnity;

	private:
		virtual void UpdateFog();
	public:

		virtual void SetFogType(const FogModeType eType);
		virtual FogModeType GetFogType()
		{
			return m_eFogType;
		}

		virtual void SetFogStart(const int start);
		virtual int GetFogStart()
		{
			return m_nFogStart;
		}

		virtual void SetFogEnd(const int end);
		virtual int GetFogEnd()
		{
			return m_nFogEnd;
		}

		virtual void SetFogDesnity(const float desnity);
		virtual float GetFogDesnity()
		{
			return m_fFogDesnity;
		}

		virtual void SetFogIsEnable(const bool b);
		virtual bool GetFogIsEnable()
		{
			return m_bEnableFog;
		}

		virtual void SetFogColorImp(CColorCtrlImp* pImp);
		virtual void Update();
		virtual bool _ProcMsg(const CMsg& msg );
	};

}
