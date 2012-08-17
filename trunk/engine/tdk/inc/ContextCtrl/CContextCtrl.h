#pragma once
#include "CCtrlBase.h"

namespace sqr
{
	class CSceneGridDebugNode;
	class CSceneBlockDebugNode;
}

namespace sqr_tools
{
	class CEditContext;
	class CContextCtrlImp : public CCtrlBase
	{
		REG_UI_IMP(CContextCtrlImp);
	public:
		CContextCtrlImp(void* param);
		~CContextCtrlImp(void);

	public:
		virtual CEditContext*	GetContext(void);

		virtual bool			LoadScene(const char* path);
		virtual const	char*	GetSceneName(void);

		virtual unsigned __int32			GetAmbientColor(void);
		virtual unsigned __int32			GetSunLightColor(void);
		virtual void						GetSunLightDir(float& X, float& Y, float& Z);

		virtual void			SetAmbientColor( uint32 color );
		virtual void			SetSunLightColor( uint32 color );
		virtual void			SetSunLightDir( float X, float Y, float Z );

		virtual uint32			GetSkyLightColor(void);
		virtual void			SetSkyLightColor( uint32 color );
	public: //Debug
		virtual void			ShowGrid(bool isShow);
		virtual void			ShowBlock(bool isShow);

		virtual bool			IsShowGrid(void);
		virtual bool			IsShowBlock(void);
		virtual void			SetContext(CEditContext* pContext);
	protected:
		CSceneGridDebugNode*	m_pGridNode;
		CSceneBlockDebugNode*	m_pBlockNode;
	};
}