#pragma once
#include "scene.h"
#include "ExsertStructDB.h"
#include "CMapEditModelObjPro.h"
#include "IAudioEmitter.h"
#include "CEditModel.h"
namespace sqr_tools
{
	class CEditModel;
}

namespace sqr
{
	//#define USE_GUI_DIALOG

	class CPieceGroup;
	class IEffect;
	class IEffectGroup;
	class CWireBox;
	class CTObject;
	class CMapObjBaseDialog;

	class CMapEditObject : public CEditModel
	{
	protected:
		///只有音效物件才具备的属性
		IAudioEmitterPtr	m_pAEmitter;
		string				m_strSceneCueName;

		CWireBox			*m_pWireBox;
		CWireBox			*m_pTempWireBox;

		CMapObjBaseDialog	*m_pObjectDialog;
		IEffectGroup		*m_pGroup;
		
	public:
		MoldeSaveProperty	sModelSavePro;
		ModelCommonProperty sModelCommonPro;
	public:

		CMapEditObject();

		virtual ~CMapEditObject();
		virtual void Release();

		virtual void SetVisible(bool b);

		void SetIsEffectBased(bool b);
		bool GetIsEffectBased();
		bool GetEffectName(string & strEffectName);
		bool GetEffectFileName(string & strEffectFileName);
		IEffectGroup * GetEffectGroup();

		// 设置物件群组中心
		void SetGroupCenter(CVector3f vGC);
		CVector3f GetGroupCenter() const;
		// 设置是否是群组
		void SetNotGroup(BOOL bNotGroup);

		int GetCenterGridIndexCurrent();

		void SetRotation(float fR);
		void SetYaw(float fR);
		void SetPitch(float fR);

		void SetScaleX(float x);
		void SetScaleY(float y);
		void SetScaleZ(float z);

		void UpdateRotation(int n);
		void UpdateScale(int n, const EEdit_Scale_Type eScaleType);

		void RecaculateSelfAmbientRatio(float & fRed, float & fGreen, float & fBlue);

		void InitRotation();
		void InitScale();

		void UpdateRotationDiscrete();
		void UpdateLiftEx(int nWay);
		void UpdateSuperLiftEx(int nWay);

		float GetHeight();

		// 模型消隐状态 自动计算、强制消隐，强制可见
		void SetModelTransState( const byte transState );
		byte GetModelTransState() const;

		float GetRotation();
		float GetYaw();
		float GetPitch();

		float GetScaleX();
		float GetScaleY();
		float GetScaleZ();

		void RestoreLiftEx(float f);

		void SetOffsetOverall(CVector3f vec);
		void CalOffsetOverall();

		void RestoreRotation(float f, int n);
		void RestoreRotation(float yf, int yn, float xf, int xn, float zf, int zn);
		void RestoreScale(float scalex, float scaley, float scalez);

		//void UpdateBoundingBox(bool bAudioUpdate = false);

		// -----------------------------------------------------------------------------

		int GetWidthInGrids();

		int GetDepthInGrids();

		void SetCenterGridIndex(int n);
		int GetCenterGridIndex();
		CVector3f GetGridCenter() const;

		int GetOriginAdjustedCenterGridIndex(int n);

		//////////////////////////////////////////////////////////////////////////

		virtual void SetLocked(bool b);
		bool GetLocked();
		void UpdateBoundingBox(bool bAudioUpdate = false);
		//////////////////////////////////////////////////////////////////////////

		string & GetName();

		CVector3f GetEditModelScale();

		EEdit_State_Placed GetPlacedEditState();
		void SetPlacedEditState(EEdit_State_Placed e);
		void SetEditState(EEdit_State state);
		void SetPlaceEditState();

		void SetCenterType(ECenter_Type eCenterType);
		ECenter_Type GetCenterType();

		bool IsActive();

		void SetSelected(bool b);
		bool GetSelected();

		void SetCenterTypeOrigin(ECenter_Type eBlockType);

		//成功创建返回true，否则返回false
		bool Create(string strFileName, bool bSubUnit, float fLift, float fShiftX, float fShiftZ, float fLiftEx, float fShiftXEx, float fShiftZEx, float fRotation, int nRotationCnt, 
					float fPitch, int nPitchCnt, float fYaw, int nYawCnt, float xScale, float yScale, float zScale);

		bool Create(const string& strFileName, bool bSubUnit, float fLift, float fShiftX, float fShiftZ, const CVector3f& vUserOffsetEx, const CVector3f& vRotate, 
					int nRotationCnt, int nPitchCnt, int nYawCnt, const CVector3f& vScale );

		void IncAtten();
		void DecAtten();
		void SetAttenRadius(int nWidth, int nDepth);
		void GetAttenRadius(int & nWidth, int & nDepth);

		//bool Intersected(CVector3f vDir,CVector3f vPos);

		void SetAnimationByAgpFileName(string strAgpFileName);
		void StopModelAnimation();
		string GetAnimationGroupName() const;
		string GetAnimationSkeName() const;

		int GetUserOffsetAdjustedGridIndex(int nGridIndex);
		void GetImageOffsetToAdjustedCenterGrid(float & x, float & y, float & z , float &fRotationX, float &fRotationY, float &fRotationZ, float& fScaleX, float& fScaleY, float& fScaleZ);
		void GetImageOffsetToAdjustedCenterGrid(CVector3f& offset);

		void SetIsChanged(bool b);
		bool GetIsChanged();

		void UpdateShiftXEx(int nWay);
		void UpdateShiftZEx(int nWay);
		void SetShiftXEx(float x);
		void SetShiftZEx(float z);

		float GetShiftXEx();
		float GetShiftZEx();
		float GetLiftEx();

		float GetShiftX();
		float GetShiftZ();
		float GetLiftx();

		void UpdateOverallAmbientRatio(float fRed, float fGreen, float fBlue);
		void UpdateOverallAmbientRatio(const float fRatio);

		void	  SetUserOffset(CVector3f vec);
		void	  SetUserOffsetEx(CVector3f vec);
		CVector3f GetUserOffset();
		CVector3f GetUserOffsetEx();

		void GetOverallAmbientRatio(byte & uR, byte & uG, byte & uB);
		void GetOverallAmbientRatio(float &fR, float &fG, float &fB);

		int GetRotationCount();
		int GetYawCount();
		int GetPitchCount();

		float GetAdjustedRotation();

		void SetGroupOffset(CVector3f v);
		CVector3f GetGroupOffset();

		int GetAdjustedGridIndexBlockCenter(int nGridIndex);

		void SetAnimationStart(int n, float fPlus);

		void SetIsTranslucent(bool b);
		bool GetIsTranslucent();

		void SetIsUseCameraCollison(bool b);
		bool GetIsUseCameraCollison();

		int GetAniFrameCount();
		void SetIsPlayAnimation(const bool b);
		bool GetIsPlayAnimation() const;

		void SetResGroupMark(BYTE bResGroupMark);
		BYTE GetResGroupMark();

		void LinkEffect(IEffect * pEffect, IEffectGroup * pGroup);
		void ReLinkEffect();
		void DeleteLinkEffect();

		void SetIsCameraOriented(bool b);
		bool GetIsCameraOriented();

		virtual void HideSelf();
		virtual void ShowSelf();

		void GetFinalMatrix(CMatrix & mat);

		CMatrix GetMatrixWorld();
		void	SetWorldPosition(const CVector3f pos);

		void SetRenderStyle(int nRenderStyle);
		int GetRenderStyle();

		CVector3f GetBlockCenter();
		void SetBlockCenter(CVector3f center);

		void SetPreCenterGridIndex(int i);
		int GetPreCenterGridIndex();

		void SetTranslucent(bool b);
		bool GetTranslucent();

		void SetCastShadow(bool b);
		bool IsCastShadow();

		// 是否接收阴影
		void SetReceiveShadow(const bool bReceiveShadow);
		bool IsReceiveShadow() const;

		CVector3f GetModelOffset();
		CVector3f GetModelOffsetOverall() const;
		CVector3f GetModelBlockCenter() const;

		bool IsSubUnit();

		void SetHideModelVertexColor(bool b);
		bool GetHideModelVertexColor();

		virtual void UseNextRenderStyle() {;}
		float GetIntersectedPosHeight(const POINT& point) ;

		void SetMirrorType(int n);
		int  GetMirrorType() const;

		void SetIsVisibleByConfigure(const bool b);
		bool GetIsVisibleByConfigure() const;

		void SetIsLightEffect(const bool b);
		bool GetIsLightEffect() const;

		void SetBakingColorIsNotGreaterShadow( const bool b );
		bool GetBakingColorIsNotGreaterShadow();

		virtual void	SetIsPointLight( const bool b ){;}
		virtual void	SetIsSpotLight( const bool b ){;}

		///audio
		virtual bool	IsNeedPlay(){return false;}
		virtual void	PlaySceneCue(){;}
		virtual void	SetSceneCueName(const string& name){;}
		virtual string	GetSceneCueName() const{return "";}
		virtual void	StopSceneCue( const bool bStop, const bool bAuto = true ){;}
		virtual bool	GetIsStopSound() {return false;}
		virtual void	SetPlayTimeLenMin( const DWORD dwTimeMin ){;}
		virtual void	SetPlayTimeLenMax( const DWORD dwTimeMax ){;}
		virtual void	SetPlayIntervalMin( const DWORD dwTimeMin ){;}
		virtual void	SetPlayIntervalMax( const DWORD dwTimeMax ){;}
		virtual DWORD	GetPlayTimeLenMin(){return 0;}
		virtual DWORD	GetPlayTimeLenMax(){return 0;}
		virtual DWORD	GetPlayIntervalMin(){return 0;}
		virtual DWORD	GetPlayIntervalMax(){return 0;}
		virtual DWORD	GetPlayTimeLen(){return 0;}
		virtual DWORD	GetCurPlayIntervalTime(){return 0;}
		virtual void	SetNeedPlayBeginMark(){;}

		///login
		virtual void SetNextAnimation( const TCHAR* szAniFileName, bool bLoop, int32 DelayTime, float AniSpeed ){;}
		virtual void SetLoginAniByAgpFileName(string strAgpFileName){;}
		virtual vector<string> GetAniNameList() const;
		virtual void SetAnimationFrame(const string& str, const int8 nFrame){;}
		virtual int16 GetAnimationFrame(const string& str) {return -1;}
		virtual vector<int16> GetAnimationFrames(const string& str)
		{ 
			vector<int16> frameList; 
			return frameList;
		}
		virtual void ChangeAnimation(){;}
		virtual void CleanAniFrames(const string& str){;}
		virtual void DeleteAniFrame(const string& str, const int nCurrKey){;}

		virtual void   SetLoginModelLogicName(const string& name){;}
		virtual string GetLoginModelLogicName() const {return "";}

		virtual void   SetLoginModelPalyAniFromStart(const bool b){;} 

		///gui
		virtual void   AddObjectRenderDialog(CTObject *pObject){;}
};
}
