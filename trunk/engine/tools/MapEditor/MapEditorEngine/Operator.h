#pragma once
#include "Scene.h"
#include "CRefObject.h"
#include "CEventOwner.h"
#include "TerrainMeshObject.h"

namespace sqr_tools
{
	class CEditModel;
}

namespace sqr
{
	class IEffect;
	class CEditModelGroup;;
	class ICamPathHandler;

	class  COperatorPropertyCtrl;
	struct SceneLight;
	struct MoldeSaveProperty;

	class COperator 
	{
	public:

		struct SProcessParams
		{
			SProcessParams()
			{
				dwGridIndexHot = 0;
				dwVertexIndexHot = 0;
			}

			DWORD dwGridIndexHot;
			CTObjectPtr pObjectHot;
			DWORD dwVertexIndexHot;
		};

	private:

		IEffect *				m_pTitleEffect;
		CTObjectPtr				m_pTitleObject;

		vector< pair<CTObject *,bool> > m_vecAmbientObjectsPairs;
		vector< CTObject *>				m_vecIndividualEffectObjects;

		// ----------------------------------------------------------------------------
		// 逻辑操作
		SProcessParams	m_processParams;

		COperatorPropertyCtrl	*pOperatorPropertyCtrl;
		bool			m_bShowEffectBase;
		bool			m_bRgnAmbientFxBase;
		bool			m_bReceiveShadow;
		// 是否是悬崖贴图
		bool			m_bCliffLayerOne;

		bool			m_bCreateDynamicLight;
		bool			m_bRebirthEffectSuc;
		bool			m_bRebirthResult;		// rebirth物件的结果,true-成功,false-失败

		bool			m_bSceneLightEffectTerrain;
		bool			m_bShowSpecialGridCueMat;//显示特殊的地表音效材质
		bool			m_bShowWireBox;

		CVector3f		m_vHitPoint;		// pick point

		// MFC操作对应的各个逻辑类
		bool					m_bCurSetSpotLight;			// 标志当前设置的是否是聚光灯
		bool					m_bMoveSpotLightTargetPos;	// 标志是否改变聚光灯目标点位置

		// ----------------------------------------------------------------------------
		float					m_fModelLogicHeight;

		bool					m_bReSizeMap;

		DWORD					m_dwLastAutoSaveTime;

		//////////////////////////////////////////////////////////////////////////

		bool					m_bUseSpecular;
		bool					m_bSetVertexDiffuseColor;  ///是否刷顶点色
		bool					m_bPreviewTerrainBaking;   ///是否预览地表烘培色

	public:
		COperator(void);
		~COperator();

		void	Create( HWND hMainFrame, HWND hResSetView, HWND hParamsView, 
						HWND hParamsExtraView, HWND hTextureView, HWND hMeshView, 
						HWND hFXSetView, HWND hVertexView, HWND hLoginView, 
						HWND hRecordView, HWND hAudioView, HWND hRegionView, HWND hTileRegionView);
		void	InitProcessParams(bool bFirst, DWORD dwVersion);

		void	SetRebirthResult(bool bRebirthResult);
		bool	GetRebirthResult();

		// ----------------------------------------------------------------------------
		/// 文字信息
		void	DrawTextInfo();

		// ----------------------------------------------------------------------------
		// 事件响应
		void	OnEvent(const CMsg& msg);
		void	PickHotGrid();

		bool	GetSnapMode();
		void	SwitchSnapMode();

		int		GetGridIndexHot();
		void	SetGridIndexHot(DWORD dw);

		DWORD	GetVertexIndexHot();
		void	SetVertexIndexHot(DWORD dw);

		void	SetHitPoint(CVector3f v);
		void	MoveHitPoint(CVector3f v);
		void	MoveHitPoint(float x, float y, float z);
		CVector3f GetHitPoint();

		// 是否线框模式
		void	SetWireFrame(bool b);
		bool	GetWireFrame();

		//是否显示包围盒
		void	SetShowWireBox(bool b);
		bool	GetShowWireBox(void);

		// 新加入的灯是否影响地表标志
		void	SetLightEffectTerrain(const bool b);
		bool	GetLightEffectTerrain() const;

		// 新加入的灯是否影响地表标志
		void	SetShowSpecialGridCueMat(const bool b);
		bool	GetShowSpecialGridCueMat() const;

		// Facade for ToolSetModel
		//模型
		void	ToolSetTitleModel_LinkEffect(string str, void * p);

		void	SetReceiveShadow(const bool bReceiveShadow);
		bool	IsReceiveShadow() const;

		// 模型物件
		bool	m_bPreViewModel;

		void	SetIsPreViewModel(bool b);
		bool	IsPreViewModel();

		void	ShowAll();
		void	HideAll();

		bool	GetDoesShowEffectBase() const;
		void	SetDoesShowEffectBase(bool b);

		bool	GetDoesRgnAmbientFxBase() const;
		void	SetDoesRgnAmbientFxBase(bool b);

		void	RefreshEffectViewSceneLightSliderState( float vRang, float vAttenuatuin0, float vAttenuatuin1, float vAttenuatuin2, byte vBrightTime, byte vDarkTime, byte vChangeTime, byte vChangeProbability, bool bDynamic);
		void	RefreshAmbientAudioPro();
		void	RefreshEffectViewSpotLightSliderState( float fRang, float fAttenuatuin0, float fAttenuatuin1, float fAttenuatuin2, float fFallOff, float fTheta, float fPhi);

		// 标志当前设置的是否是聚光灯
		void	SetCurIsSetSpotLight(const bool bSetSpotLight );
		bool	GetCurIsSetSpotLight() const;

		// 标志是否改变聚光灯目标点位置
		void	SetIsMoveSpotLightTargetPos(const bool bMove);
		bool	GetIsMoveSpotLightTargetPos() const;

		// 标志载入模型是否成功
		bool	m_bLoadModel;

		void	SetCreateDyanmicLight(bool b);
		bool	GetCreateDynamicLight();

		void	SetRebirthEffectIsSuc(bool b);
		bool	GetRebirthEffectIsSuc();

		void	CreateActiveObject(BYTE bResGroupMark, bool bVisibleByConfigure, bool bShadow, bool bTranslucent, string strItemName, vector<string> & veCEditModelFileNames, bool bSubUnit, sqr::ECenter_Type eCenterType, float fLift, float fShiftX, float fShiftZ, float fLiftEx, float fShiftXEx, float fShiftZEx, float fRotation = 0.0f, int nRotationCnt = 0, float fPitch = 0.0f, int nPitchCnt = 0, float fYaw = 0.0f, int nYawCnt = 0, float xScale = 1.0f, float yScale = 1.0f, float zScale = 1.0f ,
								   CVector3f vGroupOffset = CVector3f(0.0f,0.0f,0.0f), bool bCameraOriented = false, bool bEffectBase = false, bool bAudio = false, bool bLogin = false);

		// 用于模型
		void	CreateTitleActiveObject(vector<string> & veCTileEditorModelFileNames, vector< vector<sqr::SBlockInfo> > & vvBlock, vector< vector<sqr::SLogicHeightInfo> > & vvHeight, bool bSubUnit, sqr::ECenter_Type eBlockType, float fLift, float fShiftX, float fShiftZ);

		void	AddIndividualEffectObject(CTObject * p);
		void	RemoveIndividualEffectObject(CTObject * p);

		void	AddAmbientObject(CTObject * p);
		void    ReMoveAmbientObject(CTObject * p);
		CTObject* GetLastAmbientObject();

		void	HideObject(void * p);
		void	ShowObject(void * p);

		// 用于模型
		void	SetTitleActiveObject(CTObject *p);
		CTObject * GetTitleActiveObject();

		void	MoveTo(CTObject * p);
		int		MoveTo(int x, int z);

		void	GetMapCoordinate(const int nGridIndex, int& nX, int& nZ);

		bool	Rebirth(string strItemName, int nGridIndex, float fRotation, int nRotationCnt, float fPitch, int nPitchCnt, float fYaw, int nYawCnt, float scaleX, float scaleY, float scaleZ,
						CVector3f * pV, BYTE byBindingFlag, const TCHAR * pchClassName,int nOrdinal, int nAniStart = 0, float fPlus = 1.0f, sqr::EAUTO_GENERATION_TYPE e = sqr::EAGT_NONE, int nRenderStyle = 0,int nAtten = 0, bool bVisibleByConfigure=false,bool bShadow=true,bool bTrans=true,
						BYTE bTransState = 0, int nMirrorType = 0, int nAffectRatio = 100, string strResSetName = "", string strCurItemName = "", string strCurName = "");
		
		bool	Rebirth(string strItemName, int nGridIndex, float fRotation, int nRotationCnt, float fPitch, int nPitchCnt, float fYaw, int nYawCnt, float scaleX, float scaleY, float scaleZ,
						CVector3f * pV, BYTE byBindingFlag, const TCHAR * pchClassName,int nOrdinal, int nAniStart = 0, float fPlus = 1.0f, sqr::EAUTO_GENERATION_TYPE e = sqr::EAGT_NONE, int nRenderStyle = 0,int nAtten = 0, bool bVisibleByConfigure=false,bool bShadow=true,bool bTrans=true,bool bZTest=false, bool bSelfZTest=true,bool bExport=true,string strResSetName = "");

		bool    Rebirth(const MoldeSaveProperty* pModelSavePro, const string& strItemName, const string strResSetName = "", int nAffectRatio = 100, const string strCurItemName = "", const string strCurName = "" );

		bool    RebirthLogin(string strItemName, int nGridIndex, float fRotation, int nRotationCnt, float fPitch, int nPitchCnt, float fYaw, int nYawCnt, float scaleX, float scaleY, float scaleZ, 
			sqr::EAUTO_GENERATION_TYPE e, int nRenderStyle,int nAtten, bool bVisible,bool bShadow,bool bTrans, BYTE bTransState, int nMirrorType, int nAffectRatio);

		void	RebirthIndividualEffect(string strItemName, int nGridIndex, float fRotation, int nRotationCnt, float fPitch, int nPitchCnt, float fYaw, int nYawCnt, float scaleX, float scaleY, float scaleZ,
										CVector3f * pV, const CVector3f& pos, BYTE byBindingFlag, const TCHAR * pchClassName,int nOrdinal, int nAniStart = 0, float fPlus = 1.0f, sqr::EAUTO_GENERATION_TYPE e = sqr::EAGT_NONE, int nRenderStyle = 0,int nAtten = 0, bool bVisibleByConfigure=false,bool bShadow=true,bool bTrans=true,bool bZTest=false, bool bSelfZTest=true,bool bExport=true,string strResSetName = "");

		void	CopyRebirth(int nGridIndex, CEditModelGroup *pModelGroup, sqr::EAUTO_GENERATION_TYPE e, CVector3f vGroupOffset);

		CTObject *GetObjectHot();
		void	SetObjectHot(CTObject * p);


		void	MapBackPicMoveInfo();

		DWORD	GetLastAutoSaveTime () const;
		void	SetLastAutoSaveTime( DWORD time);

		// ----------------------------------------------------------------------------
		// 开启强制保存地图的标志
		void	OpenSaveMapMark();

		void	SetIsResizeMap(bool b);
		bool	GetIsResizeMap() const;

		///扩展后旧地图格子索引在新地图中的新索引映射 first old grid   index second new grid index
		map<int, int> m_OldMapGridIndexMap;
		///扩展后旧地图细化区域索引在新地图中的新索引映射 first old region index second new region index
		map<int, int> m_OldMapRegionIndexMap;
		///分割后旧地图格子索引在新地图中的新索引映射 first old grid   index second new grid index
		map<int, int> m_SplitOldMapGridIndexMap;
		//////////////////////////////////////////////////////////////////////////

		set<string>		 m_LoginLoginNameList;
		map<string, int> m_LoginLoginItemNameMap; //first model item name, second num
		map<int, string> m_LoginModelGridIndexs;

		DWORD	m_dwLoadSceneModelCnt;

		void	SetIsUseSpecular(bool b);
		bool	GetIsUseSpecular() const;

		void	SetIsSetVertexDiffuseColor(bool b);
		bool	GetIsSetVertexDiffuseColor() const;

		void	SetIsPreviewTerrainBaking(bool b);
		bool	GetIsPreviewTerrainBaking() const;
	};
}
