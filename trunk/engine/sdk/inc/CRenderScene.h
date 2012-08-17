#pragma once
#include "TPart.h"
#include "CPos.h"
#include "CVector3.h"
#include "CDynamicObject.h"
#include "CLapseDeleteQueue.h"
#include "CRenderObject.h"
#include "CQuadScene.h"
#include "CRTSCameraCtrller.h"
#include "CTPSCameraCtrller.h"
#include "CSceneQuery.h"
#include "CCoordinateContainer.h"
#include "IRenderTime.h"
#include "CSceneDebugHelper.h"
namespace sqr
{
	struct DeltaPos;
	struct CGridObjInfo;
	struct GridColor;
	struct RegionsInfo;

	class CMetaRegion;
	class CRenderRegion;
	class CMetaSceneClient;
	class CRenderLight;
	class CCoreObjectFollower;
	class CCoreObjectDirector;
	class CGraphic;
	class CSkyMgr;
	class CDataScene;
	class CRenderLogicModel;
	class CRegionAmbientEffectMgr;
	class CSceneDebugMgr;
	
#define MAX_WATER_REGION	32
#define MAX_RENDER_LIGHT	4

	struct CGridTextIndex : public CSceneMallocObject
	{
		union
		{
			uint32		m_uMultiIndex;
			uint8		m_uSingleIndex[4];
		};
		int8			m_xDelGrid;
		int8			m_yDelGrid;
	};

	class	CRenderSceneQueue 
		: public CRenderSpace
		, public CSceneMallocObject
	{
	public:
		CRenderSceneQueue();
		~CRenderSceneQueue();
		void	RenderEx( CCamera* pCamera, uint32 uCurTime );
		void	UnRenderEx(uint32 uCurTime);
		void	UpdateTime(uint32 uCurTime);
		void	DestroyRenderChild(CRenderSpaceNode* pObj);
		void	SelectRendObj(const CVector3f& rayPos, const CVector3f& rayDir, const CVector3f CameraDir);
		CLapseDeleteQueue		m_LapseQueue;
		SpaceNodeList			m_RenderDelete;
	private:
		void (__thiscall CRenderSceneQueue:: *m_fpDestoryFn)(CRenderSpaceNode* pObj);
		void			_DestoryRenderChildtInRender(CRenderSpaceNode* pObj);
		void			LockRenderQueue(void);
		void			UnLockRenderQueue(void);
		void			_DestoryRenderChild(CRenderSpaceNode* pObj);
	};

	class CTestSuiteMetaSceneClient;
	class CRenderObject;
	class CRenderSystem;

	class CRenderScene
		: public virtual CDynamicObject
		, public CSceneDebugMgr
		, public CSceneMallocObject
		, public ILogicHandler
	{
	public:
		float GetTerrainHeight( float xGround, float yGround );
		bool IsBackGrid(const CVector3f& pos) const;

	public:

		//渲染
		void					Render();
		void					Update();

		CDataScene*				GetDataScene()const;
		CRenderObject*			GetSelectedObj()const;
		void SetSelectedObj(CRenderObject* pRenderObj);
		const CVector3f&		GetSelectedPos()const;
		void					GetSelectedPos(CVector3f& vecPos);
		CVector3f				GetCameraDest()const;
		void					GetCameraDest(CVector3f& vecDest);
		float					GetCameraOffset();

		bool					IsLoadEnd(void);
		void					PlayCameraPath(uint32 Index);
		void					SetCameraController(CCameraController* pCtrl);
		void					SetCameraMaxSpeed( float speed );
		void					InitSceneRes();
		bool					SetCameraDest( float fPixelX,float fPixelZ );

		CRTSCameraCtrller*		GetRTSCamCtrller();
		CTPSCameraCtrller*		GetTPSCamCtrller();
		void					OnRTSConfigChanged(float DefDist);
		//设置镜头相对于观察点的偏移
		void					SetCameraOffset( float fDist );
		void					SetDist(float minDist ,float maxDist);
		void					SetFov(float minfov, float maxfov );
		void					SetRadian(float mindegree, float maxdegree );
		void					SetYOffset(float minyoffset ,float maxyoffset);
		void					SetXZDist(float xzdist);
		void					SetDistSpeed(float distSpeed);

		//对旋转设想机有效		
		void					SetCameraXRotate( float dgree );
		void					SetCameraYRotate( float dgree );
		float					GetCameraXRotate( void );
		float					GetCameraYRotate( void );
		float					GetCameraYDegree( void );

		//fTimeOfDay单位：小时
		void					SetTimeOfDay( float fTimeOfDay );;
		//改为GetLastFrameRenderInfo
		bool					IsShowDebugInfo()const;
		void					ShowDebugInfo( bool bShow );

		void					AttachRenderObject( CRenderObject& RenderObject, const CVector3f& Pos );
		void					AttachRenderObjectAbs( CRenderObject& RenderObject, const CVector3f& Pos );
		void					SetRenderObjectPosition( CRenderObject& RenderObject, const CVector3f& Pos, bool bTerrain );
		void					DetachRenderObject( CRenderObject& RenderObject );

		//产生动态水波
		void					DoDimple( CRenderObject& RenderObject, const CVector2f& vDelta, bool bMove );

		///实时计算区域光参
		void					CalRegionLightParam();

		virtual CRenderObject*	CreateRenderObject(IRenderTime* pRenderTime);
		void					DestroyRenderObject(CRenderObject* pObj);

		//bool					RegisterTerrainObject(CRenderTerrainObj* pObj);

		CVector3f               GetPickCameraPos();
		CVector3f               GetPickCameraOffset();
		bool					IsSelectRenderObj(float xpos, float ypos, CRenderObject* renderObj);
		bool					SelectRenderObjDir(float xCursorPercent, float yCursorPercent, CVector3f& vecOrig, CVector3f& vDir);
		//确定当前选中的格子和物体
		void					SelectPosByCursor( float xCursorPercent, float yCursorPercent );

		//得到区域
		//CRenderRegion*			GetRegion( uint32 uGridX, uint32 uGridY );
		//得到区域
		//CRenderRegion*			GetRegion( uint32 uId );
		//得到当前帧
		uint32					GetCurFrame()const;

		//得到格子的索引
		//uint32				GetGridIndex( uint32 j, uint32 i );

		///register login model
		bool SetLogicModelHandler( const char* szLogicName, IModelHandler* pHandler );
		void RegisterLogicModel( const SString& strLogicName, CRenderObject *pLoginObj );
		void UnRegisterLogicModel( const SString& strLogicName, CRenderObject *pLoginObj );
		bool SetLogicModelNextAni( const char* szLogicName, const char* szAniFileName, bool bLoop = true, int32 uDelayTime = 200, float AniSpeed = -1.0f );
		bool SetLogicModelEffect( const char* szLogicName, const char* szFxFile, const char* szFx );

		CRenderSceneQueue& GetRenderSceneQuene();


	protected:
		const GridColor*		GetBakedGridColor(index_t gridIndex);

		//typedef CCoordinateVector<CRenderRegion*> CVecRenderRegion;
		typedef TPart<CRenderScene,CRenderSystem> ParentPart_t;

		friend class CTestSuiteMetaSceneClient;
		friend class CTestSuiteCoreObjectClient;
		friend class CRenderSystem;
		friend class TPart<CRenderScene,CRenderSystem>;
		friend struct CGridObjInfo;
		friend class CCoreObjectDirector;

		enum ELoad { 
			eLoaded_Null				= 0x0, 
			eLoaded_LightAndModeInfo	= 0x1, 
			eLoaded_TerranStart			= 0x2, 
			eLoaded_Model				= 0x4,
			eLoaded_Camera				= 0x8,
			eLoaded_Finished			= 0xF };

			//CVecRenderRegion		m_vecRegion;
			CDataScene*				m_pDataScene;
			CRenderObject*			m_pSelectedObj;

			CPos					m_posSelectedGrid;
			CVector3f				m_vecSelectedLogic;
			CPos					m_posCurCenterGrid;
			CVector3f				m_vecCameraDest;
			float					m_CamMinSpeed;

			bool					m_IsDynamicShadowEnable;
			bool					m_bShowDebugInfo;
			uint8					m_eLoadState;
			float					m_fTimeofDay;
			float					m_fPercent;

			bool					m_bFogEnable;
			uint32					m_uIndexUseNum;
			SVector<CGridTextIndex>	m_GridTextIndex;
			CRenderSceneQueue		m_RenderQueue;
			CTreeScene*					m_pTreeScene;
			CRegionAmbientEffectMgr*	m_pAmbientMgr;
			//CSkyMgr*				m_pSkyBox;
			//CRenderLight*	m_pTerrainLight[MAX_RENDER_LIGHT];

			//SString					m_sSceneColorFilePath;///当前场景的顶点烘焙文件所在的路径
			SString					m_sSceneName;
			uint32					m_uPreTime;
			uint32					m_uCurTime;
			uint32					m_uUpdateFrame;

			ITexture*				m_pSkyTexture;
			CCamera*				m_pCamera;
			CRenderGroup*			m_pRenderGroup;
			CRTSCameraCtrller		m_RTSCamerCtrl;// 即时战略摄像机视角
			CTPSCameraCtrller		m_TPSCamerCtrl; //
			CCameraController*		m_pActiveCamCtrl;
			CSceneQuery*			m_pSceneQuery;
			CLightSystem*			m_pLightSys;

			CVector3f				m_vecStart;
			CVector3f				m_vecEnd;

			//uint8					m_TerranVerParam[256];
			uint32					m_SceneWidth;
			uint8					m_eGotoState;
			/*
			因为场景太阳光和环境光要与人物分离需要
			保存中间变量
			*/
			CColor					m_dwSunColor;
			CColor					m_dwShadowColor;
			CColor					m_dwAmbientColor;
			CColor					m_dwPlayerSunColor;
			CColor					m_dwPlayerAmbientColor;
			CColor					m_dwFogColor;
			float					m_FogStart;
			float					m_FogEnd;
			float					m_fCamDist;
			float					m_fDefCamDist;
			CVector3f				m_vSunDir;

			float					m_fCameraFogStart;///用于计算实时雾的起点距离
			float					m_fCameraFogEnd;///用于计算实时雾的终点距离

			SString					m_sCurMusic;
			uint32					m_stGX;
			uint32					m_stGZ;

			SVector<IEffect*>	    m_pEffectRenderObjList;

			///register login model
			typedef	SMap<SString, CRenderObject*>	LocalModelMap;
			LocalModelMap			m_LogicRenderObjMap;
			//-----------------------------

//			void					UpdateTerranParam( bool bvalue );
// 
// 			uint8					GetTerranParam( uint8 n )
// 			{
// 				return m_TerranVerParam[n];
// 			}
// 			void					SetTerranParam( uint8 n, uint8 v )
// 			{
// 				m_TerranVerParam[n] = v;
// 			}

			virtual void			PreDraw();
			void					MoveSKyBox();
			void					DrawObject(void);// bool bAlpha, bool bMain, bool bLink );
			void					DrawSelect();
			void					DrawSelectObj(CRenderObject* pRenderObj);
			void					DrawWater();
			void					DrawAmbientEffect();///render 全局特效
			void					CreateAmbientEffect(); /// create 全局特效
			//void					CreateLight( CMetaRegion* pRegion );
			void					DoProcess( float fPercent );

			//得到当前时间
			uint32					GetCurTime()const;
			//得到帧间隔时间
			uint32					GetDeltaTime()const;
			

			void					UpdateCameraOffset();
			void					UpdateCameraYOffset();

	protected:
			void					Initialize(void);
			void					InitSceneParams(void);
			CRenderScene( CDataScene* pDataScene );
			virtual ~CRenderScene(void);
	};

	inline CRTSCameraCtrller* CRenderScene::GetRTSCamCtrller()
	{
		return &m_RTSCamerCtrl;
	}

	inline CTPSCameraCtrller* CRenderScene::GetTPSCamCtrller()
	{
		return &m_TPSCamerCtrl;
	}
// 	//------------------------------------------------------------------------------
// 	inline CRenderRegion* CRenderScene::GetRegion( uint32 uGridX, uint32 uGridY )
// 	{
// 		return m_vecRegion.GetElem( uGridX >> 4, uGridY >> 4);
// 	}
// 
// 	//------------------------------------------------------------------------------
// 	inline CRenderRegion* CRenderScene::GetRegion( uint32 uId )
// 	{
// 		return m_vecRegion.GetElem( uId );
// 	}

	//------------------------------------------------------------------------------
	inline uint32
		CRenderScene::GetCurTime() const
	{
		return m_uCurTime;
	}

	//------------------------------------------------------------------------------
	inline uint32
		CRenderScene::GetDeltaTime() const
	{
		return m_uCurTime - m_uPreTime;
	}

	//------------------------------------------------------------------------------
	inline void
		CRenderScene::SetTimeOfDay( float fTimeOfDay )
	{
		m_fTimeofDay = fTimeOfDay;
	}

	//------------------------------------------------------------------------------
	inline bool
		CRenderScene::IsShowDebugInfo() const
	{
		return m_bShowDebugInfo;
	}

	//------------------------------------------------------------------------------
	inline void
		CRenderScene::ShowDebugInfo( bool bShow )
	{
		m_bShowDebugInfo = bShow;
	}

	//------------------------------------------------------------------------------
	inline uint32
		CRenderScene::GetCurFrame() const
	{
		return m_uUpdateFrame;
	}

	inline bool
		CRenderScene::IsLoadEnd(void)
	{
		return m_eLoadState == eLoaded_Finished;
	}

}
