#pragma once
#include "CEventOwner.h"
#include "CRenderScene.h"
#include "CEditModel.h"

namespace sqr
{
	class CRenderGroup;
	class CWindowTarget;
}

namespace sqr_tools
{	
	class CEditDataScene;
	class CEditContext 
		: public CEventOwner
		, public CRenderScene 
	{
	public:
		CEditContext(void);
		~CEditContext(void);

		virtual void Bump( uint32 uTime );
		virtual void SetCycSpeed( uint32 cyc );
		virtual void SetRenderTarget(CWindowTarget* target);
		//virtual void SetRenderGroup(CRenderGroup* group);
	
		virtual CRenderObject*	CreateRenderObject(IRenderTime* pRenderTime);
		virtual CEditModel*		CreateEditModel(IRenderTime* pRenderTime);

		virtual void			AddToTreeScene( CEditModel* pChild );
		virtual void			AddToRenderScene( CEditModel* pChild );

		virtual CRenderNode*	SelectModelByCursor(const float xCursorPercent, const float yCursorPercent);
		virtual bool			LoadScene(const string& sName);
		virtual bool			ClearScene(void);
		virtual CEditDataScene*	GetEditDataScene(void);
		virtual CWindowTarget*	GetWindowTarget(void);
		virtual void			SetFogParams( float fStart, float fEnd );
	public:
		virtual CRenderNode*	GetActiveObj(void);
		virtual bool			SetActiveObj(CRenderNode* pObj);

	public: 
		//property
		virtual CColor			GetAmbientColor(void);
		virtual CColor			GetSunLightColor(void);
		virtual CVector3f		GetSunLightDir();
		virtual CColor			GetPlayerSunColor();
		virtual CColor			GetPlayerAmbientColor();
		virtual CColor			GetFogColor();
		virtual float			GetFogStart();
		virtual float			GetFogEnd();

		virtual void			SetAmbientColor( CColor color );
		virtual void			SetSunLightColor( CColor color );
		virtual void			SetSunLightDir( const CVector3f& dir );
		virtual void			SetPlayerSunColor(CColor);
		virtual void			SetPlayerAmbientColor(CColor);
		virtual void			SetFogColor(CColor);
		virtual void			SetFogStart(float);
		virtual void			SetFogEnd(float);

		virtual CVector3f		GetSelectedLogicPoint() const;
	public:

		CCameraController*		GetCurCamCtrller();

	protected:
		virtual void _Update( uint32 uTime );
		virtual void _Render( uint32 uTime );

		bool _ProcMsg( const CMsg& msg );

	protected:
		CWindowTarget*	m_pWindowTarget;
		
	protected:
		uint64			m_u64LastTime;
		uint64			m_uUseTime;
		uint64			m_uLimitCyc;
		CRenderNode*	m_pActiveModel;
	};
	
	inline CCameraController* CEditContext::GetCurCamCtrller()
	{
		return m_pActiveCamCtrl;
	}
}