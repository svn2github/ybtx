#pragma once
#include "CRenderBatch.h"
#include "CFilterNode.h"
#include "CLightSystem.h"
#include "CCamera.h"
#include "CRenderBatch.h"
#include "CModel.h"
#include "CGeometryInstance.h"
#define MaxRenderID		4096

typedef DWORD RS_HANDLE;

namespace sqr
{
	class CRenderGroup : public CRefObject
	{
	protected:

		bool	m_isOpt;
	public:
		FilterNode* InsertRender(FilterNode* fn);
		void InsertGeometry( CGeometryInst* pGI, ERenderObjStyle eObjStyle );
		void SetGeomeotryInstParams(const CGeometryParams& params);
		
		void Render(void);
		void PostRender(void);

		void Reset(void);
		void SetShadowType( EShadowType fShadow );
		void SetShadowFilter( CShadowFilter* pSFilter );
		void Destroy();
		void RegisterBatch(RENDER_BATCH_ID fID,CRenderBatch* pFilter);
		void UnRegisterBatch(RENDER_BATCH_ID fID,CRenderBatch* pFilter);
		CRenderBatch* 	GetBatch(RENDER_BATCH_ID fID);
		EShadowType GetShadowType();
		void SetLightIndex(uint index) {};

	public:
		void	SetFogColor( const DWORD Color );
		void	EnableFog( bool bEnable );
		bool	IsFogEnabled();;
		void	SetFog( FogModeType fMode , DWORD fColor , ... );
		void	SetFogParams( FogModeType fMode, ... );

		DWORD	GetFogColor(void);
		FogModeType	GetFogType(void);

	public:
		CRenderGroup(void);
		~CRenderGroup();

		void	SetRenderQueueFlag( const RenderQueueFlag& QueueFlag) ;
		const	RenderQueueFlag& GetRenderQueueFlag();
		void	Begin( CModel* pModel );
		void	Begin( NodePool* pRenderList );
		void	End(void);

		void	SetImmediate(void);
		void	SetOptimize(void);
		bool	isRenderOptimize(void);
		void	 SetCamera(CCamera* pCam = NULL);

	public:
		CLightSystem*	GetLightSystem(void);
		CCamera*		GetCamera(void);
		CShadowFilter*	GetShadowFilter(void);
	protected:
		//FilterNode* (__thiscall CRenderGroup:: *m_fpInertFn)(FilterNode* fn);

		//FilterNode* _InsertI(FilterNode* fn);
		//FilterNode* _InsertC(FilterNode* fn);

		NodePool*		m_pActiveList;
		NodePool		m_ltRenderNode;
		CRenderBatch*	m_Filters[ID_MAX_FILTER_NUM];
		EShadowType		m_ShadowType;
		PARAM_ID		m_BaseId;
		RenderQueueFlag	m_FilterQueue;

	protected:
		bool					m_isEnableFog;
		CGeometryParams			m_GeometryParams;
		FogModeType				m_tFogMod;
		DWORD					m_dwFogColor;
		float					m_fFogParam1;
		float					m_fFogParam2;
		CLightSystem*			m_LightSystem;

		CShadowFilter*			m_pShadowFilter;		//ÉãÏñ»ú
		CCamera*				m_pActiveCamera;
		CCamera					m_Camera;


		void					Init(void);
	};
#pragma region InlineMethod
	//------------------------------------------------------------------------------
	inline bool
	CRenderGroup::IsFogEnabled()
	{
		return m_isEnableFog;
	}

	//------------------------------------------------------------------------------
	inline DWORD
	CRenderGroup::GetFogColor( void )
	{
		return m_dwFogColor;
	}
	//------------------------------------------------------------------------------
	inline const	RenderQueueFlag&
	CRenderGroup::GetRenderQueueFlag()
	{
		return m_FilterQueue;
	}

	//------------------------------------------------------------------------------
	inline bool
	CRenderGroup::isRenderOptimize( void )
	{
		return m_isOpt;
	}

	//------------------------------------------------------------------------------
	inline CLightSystem* CRenderGroup::GetLightSystem( void )
	{
		return  m_LightSystem;
	}

	//------------------------------------------------------------------------------
	inline CCamera* CRenderGroup::GetCamera( void )
	{
		return m_pActiveCamera;
	}
	//------------------------------------------------------------------------------
	inline void CRenderGroup::SetCamera(CCamera* pCam)
	{
		if( pCam == NULL )
			m_pActiveCamera = &m_Camera;
		else
			m_pActiveCamera = pCam;
	}
	//------------------------------------------------------------------------------
	inline CShadowFilter* CRenderGroup::GetShadowFilter( void )
	{
		return m_pShadowFilter;
	}

	//------------------------------------------------------------------------------

	//------------------------------------------------------------------------------
	inline FogModeType CRenderGroup::GetFogType( void )
	{
		return m_tFogMod;
	}
#pragma endregion InlineMethod
}