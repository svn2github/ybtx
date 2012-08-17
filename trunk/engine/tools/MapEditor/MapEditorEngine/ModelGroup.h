#pragma once
#include "SceneLight.h"
#include "ExsertStructDB.h"
#include "CRefObject.h"

#define ATTENTRUM 8
#define MAXAFFECTRATIO 100
#define MINAFFECTRATIO 0

namespace sqr
{
	class IEffect;
	class IEffectGroup;
	class CTObject;
	class CMapEditObject;
	class CTileEditorModel;
	struct SceneAudio;
	struct MoldeSaveProperty;
	struct ModelCommonProperty;

	SmartPointer(CEditModelGroup);
	class CEditModelGroup : public CRefObject
	{
	public:

		typedef vector<CMapEditObject*>	MapEditObjPool;
		MapEditObjPool	m_vecSelfPresentation;
		vector<CTileEditorModel*>	m_vecTileSelfPresentation;

		vector<string>		m_vecCMapEditModelFileNames;
		sqr::ECenter_Type	m_eCenterType;

		CVector3f		m_vUserOffset;
		string			m_strAgpFileName;
		string			m_strItemName;
		string			m_strEffectNameForGroup;		// 用于群组复制粘贴时的特效
		vector<int>		m_vecGridIndicesImageStandsOn;	// grids my image stands on

		bool			m_bSubUnit;
		int				m_bFirstSee;
		int				m_nGridIndexImageCenter;
		int				m_nAtten;
		int				m_nAffectRatio;
		bool			m_bShadowWithZ;
		CVector3f		m_vGroupCenter;					// 物件群组的中心
		BOOL			m_bNotGroup;					// 是否是群组
		bool			m_bHide;				// 是否隐藏模型
		bool			m_bModelInfoIsValid;

	public:
		void	SetEffectNameForGroup(string str);
		string	GetEffectNameForGroup();
		// ---- 设置物件群组中心
		void	SetGroupCenter(CVector3f m_vGC);
		// ---- 设置是否是群组
		void	SetNotGroup(BOOL bNotGroup);
		
		int		GetCenterGridIndexCurrent();
		int		GetCenterGridIndex();

		void	SetRotation(float fR);
		void	SetYaw(float fR);
		void	SetPitch(float fR);
		void	SetScaleX(float x);
		void	SetScaleY(float y);
		void	SetScaleZ(float z);

		void	SwitchShadowSecondPass();
		bool	GetShadowSecondPass();

		void	IncAtten();
		void	DecAtten();

		void	IncAffectRatio();
		void	DecAffectRatio();
		int		GetAffectRatio() const;
		void	SetAffectRatio( int nRatio);

		vector<int>& GetGridIndicesImageStandsOn();
		void	SetGridIndexImageCenter(int n);
		int		GetGridIndexImageCenter();
		void	AddGridIndexImageStandsOn(int n);
		void	ClearImages();

		void	GetFinalMatrix(CMatrix & mat);

	public:
		CEditModelGroup();
		~CEditModelGroup();
		void    ReleaseSelfModels();

		void	GetOverallAmbientRatio(byte & uR, byte & uG, byte & uB);
		void	GetOverallAmbientRatio(float &fR, float &fG, float &fB);

		void	AddContainer(vector<CMapEditObject *> & vec,bool bSubUnit, sqr::ECenter_Type eBlockType, int nXDelta, int nZDelta);
		void	AddTitleContainer(vector<CTileEditorModel *> & vec, bool bSubUnit, sqr::ECenter_Type eBlockType, int nXDelta, int nZDelta);
		void	SetCenterType(sqr::ECenter_Type e);

		CMapEditObject* GetSelfPresentation(int n);
		int		GetSelfPresentationCount();

		CTileEditorModel* GetTitleSelfPresentation(int n);
		int		GetTitleSelfPresentationCount();

		void SetModelFileNames(vector<string> &vec);
		vector<string>& GetModelFileNames();
		size_t	GetModelFileNamesSize();
		string& GetModelFileNameByIndex(int n);

		void	SetEditState(sqr::EEdit_State e);
		void	SetCenterGridIndex(int n);
		void	SetLocked(bool);

		void	InitPlaced();
		void	SetPlacedEditState(sqr::EEdit_State_Placed);

		void	UpdateLiftEx(int n);
		void	UpdateSuperLiftEx(int n);

		void	UpdateShiftXEx(int n);
		void	UpdateShiftZEx(int n);
		void	SetShiftXEx(float x);
		void	SetShiftZEx(float z);

		float	GetShiftXEx();
		float	GetShiftZEx();
		float	GetLiftEx();

		float	GetShiftX();
		float	GetShiftZ();
		float	GetLiftx();

		CVector3f GetGroupOffset();

		void	UpdateRotation(int n);
		void	UpdateScale(int n, const EEdit_Scale_Type eScaleType);

		float	GetRotation();
		float	GetYaw();
		float	GetPitch();
		float	GetScaleX();
		float	GetScaleY();
		float	GetScaleZ();
		float	GetAdjustedRotation();

		void	  SetUserOffsetEx(CVector3f vec);
		CVector3f GetUserOffsetEx();
		CVector3f GetModelOffsetOverall();

		void	Render();

		bool	Intersected(CVector3f vDir,CVector3f vPos);
		bool	GetIsSubUnit();
		sqr::ECenter_Type GetCenterType();

		void	SetSelected(bool);
		bool	GetSelected();
		bool	GetLocked();
		bool	m_bNext;

		// ---- 地形
		void	SetAnimationByAgpFileName(string strAgpFileName);
		void	SetLoginAniByAgpFileName(string strAgpFileName);
		// ----  模型
		bool	SetTitleAnimationByAgpFileName(string strAgpFileName);
		string	GetAgpFileName();

		void	SetIsChanged(bool b);
		void	SetUserOffset(CVector3f vec);

		CVector3f GetUserOffset();

		string& GetItemName();
		void	SetItemName(string & str);

		void	GetImageOffsetToAdjustedCenterGrid(float & x, float & y, float & z, float &fRotationX, float &fRotationY, float &fRotationZ, float& fScaleX, float& fScaleY, float& fScaleZ );

		void	SetMirrorType(int n);
		int		GetMirrorType() const;

		int		GetRotationCount();
		int		GetYawCount();
		int		GetPitchCount();
		int		GetAdjustedGridIndexBlockCenter(int nGridIndex);
		BYTE	GetDir();

		void	ResetOverallAmbientRatio();
		void	SetIsTranslucent(bool b);
		void	SetShadowSecondPass(bool b);
		int		GetAniFrameCount();

		void	SetIsUseCameraCollison(bool b);
		bool	GetIsUseCameraCollison();

		// ---- effect
		void	LinkEffect(IEffect * pEffect, IEffectGroup * pGroup);
		void	ReLinkEffect();
		void	DeleteLinkEffect();

		// ---- model
		void	TitleLinkEffect(IEffect * pEffect, IEffectGroup * pGroup);
		void	TitleUnlinkEffect();

		// ---- animation
		void	StopModelAnimation();
		string  GetAnimationGroupName() const;
		string  GetAnimationSkeName() const;

		void	StopSceneCue( const bool bStop );
		bool	GetIsStopSound();

		void	SetIsPlayAnimation(const bool b);
		bool	GetIsPlayAnimation() const;

		void	HideSelf();
		void	ShowSelf();

		void	SetIsEffectBased(bool b);
		bool	GetIsEffectBased();
	
		void	SetSceneCueName(const string& name);
		string	GetSceneCueName() const;

		void	SetResGroupMark(BYTE bResGroupMark);
		BYTE	GetResGroupMark();

		void	SetIsCameraOriented(bool b);
		bool	GetIsCameraOriented();

		bool	GetEffectName(string & strEffectName);
		bool	GetEffectFileName(string & strEffectFileName);

		float	GetHeight();
		float	GetHeight(const size_t index);

		// ---- 模型消隐状态 自动计算、强制消隐，强制可见
		void	SetModelTransState( const byte transState );
		byte	GetModelTransState() const;
		byte	GetModelTransState(const size_t index) const;

		IEffectGroup* GetEffectGroup();
		IEffect* GetEffect();

		void	SetGroupOffeset(CVector3f & v);
		int		GetRenderStyle();
		void	SetRenderStyle(int nRenderStyle);

		void	UpdateOverallAmbientRatio();
		void	SetOverallAmbientRatio(float fRed, float fGreen, float fBlue);

		int		GetAtten();
		void	SetAtten(int nAtten);

		// ---- 更新聚光灯的方向
		void	UpdateSpotLightDir();

		// ---- 更新聚光灯的目标位置
		void	UpdateSpotTargetPos(const int nType);

		// ---- 模型是否隐藏
		void	SetHide(bool b);
		bool	GetHide();

		// ---- 隐藏模型顶点色
		void	SetHideModelVertexColor(bool b);

		CVector3f GetBlockCenter();
		void	SetBlockCenter(CVector3f center);

		void	SetPreCenterGridIndex(int i);
		int		GetPreCenterGridIndex();

		bool	GetIsTranslucent();

		void	SetCastShadow(bool b);
		bool	IsCastShadow();

		void	SetReceiveShadow(const bool bReceiveShadow);
		bool	IsReceiveShadow() const;

		void SetIsVisibleByConfigure(const bool b);
		bool GetIsVisibleByConfigure() const;

		vector<string> GetAniNameList() const;
		void SetNextAnimation( const TCHAR* szAniFileName, bool bLoop, int32 DelayTime, float AniSpeed );
		void SetAnimationFrame(const string& str, const int8 nFrame);
		int16 GetAnimationFrame(const string& str) const;
		vector<int16> GetAnimationFrames(const string& str);
		void ChangeAnimation();
		void CleanAniFrames(const string& str);
		void DeleteAniFrame(const string& str, const int nCurrKey);
		void   SetLoginModelLogicName(const string& name);
		string GetLoginModelLogicName() const;
		void   SetLoginModelPalyAniFromStart(const bool b);

		void	SetIsLightEffect(const bool b);
		bool	GetIsLightEffect() const;

		void	SetMoldeSaveProperty(const MoldeSaveProperty *pModeSavePro);
		void	SetWorldPosition(const CVector3f& pos);

		void	SetPlayTimeLenMin( const DWORD dwTimeMin );
		void	SetPlayTimeLenMax( const DWORD dwTimeMax );
		void	SetPlayIntervalMin( const DWORD dwTimeMin );
		void	SetPlayIntervalMax( const DWORD dwTimeMax );
		DWORD	GetPlayTimeLenMin();
		DWORD	GetPlayTimeLenMax();
		DWORD	GetPlayIntervalMin();
		DWORD	GetPlayIntervalMax();
		DWORD	GetPlayTimeLen();
		DWORD	GetCurPlayIntervalTime();

		void	SetBakingColorIsNotGreaterShadow( const bool b );
		bool	GetBakingColorIsNotGreaterShadow();

		//void	UpdateBoundingBox(bool bAudioUpdate);

		///gui
		void	AddObjectRenderDialog(CTObject *pObject);

		void SetVisible(bool b);

	public:
		SceneLight		m_SL;			// 场景点光源
		SceneSpotLight	m_SpotLight;	// 场景聚光灯
		CVector3f		m_SpotLightPos; // 聚光灯位置
		bool			m_bMoveDelete;
		SceneAudio		*m_pSceneAudio; //场景音源
		bool			m_bLoginModel;  //区分登录界面与场景model
		bool			m_bReginEffect; //区域特效标志
	};
}
