#pragma once
#include "TSingleton.h"
#include "CVector3.h"
namespace sqr
{
	class CCamera;
	class CRenderObject;
	class CRenderGroup;
	class CRenderFont;
};

namespace sqr_tools
{
	class CEditModel;
	class CEditContext;
}

enum EModelType;

class CGTRenderMgr : public Singleton<CGTRenderMgr>
{
public:
	CGTRenderMgr();
	~CGTRenderMgr(void);

	void SetRenderSceneContext(CEditContext* context);

	//场景名、模型名、动画名、模型个数
	CRenderObject*	CreateCharacter(string name,EModelType type);
	void			DisplayCharacterAperture(float fScale);
	void			DoAnimate(string name);
	void			CreateScene(const string& strScenePath);
	void			OnRender();
	void			SetCameraPos(DWORD direction,int plength);
	void			DestroyMainPlayer(void);
	void			DestroyObj(void);
	void			HideApertureAndRefer();

	bool			IsShow() {return m_IsShow;}
	
	string			GetModelFileName() {return m_ModelFileName;}
	string			GetFxFileName();
	CRenderObject*	GetDummyCharacter();

	void			CreateMainPlayer();
	void			SetMainPlayerIsShow(const bool show);
	void			SetMainPlayerAni(const string& aniname, const float fSpeed);
	void			MainPlayerAddPiece(const string& modelname, const string& piecename, const string& rsname);
	void			HideMainPlayerPiece(const vector<string> hidePieceNames);
	void			UpdatePlayerAnimationGroup(const string& name);
	bool			GetAnimationGroupAniNames(const string& agpName, vector<string>& aninames);
	void			ReleaseMainPlayer();

	void			GetPlayerEffectNames(const string strEffectPath, vector<string>& effectNames);
	void			AttachPlayerEffect(const string& strEffectPath, const string& strEffectName);


private:
	CEditContext*	m_pRenderSceneContext;
	CEditModel*     m_pMainPlayer;
	CEditModel*		m_pRenderObj;
	CRenderObject*	m_pDummyRenderObj;
	CRenderObject*	m_pReferenceRenderObj;
	CRenderFont *	m_pFont;
	string			m_ModelFileName;
	string			m_LastNpcFxName;
	bool			m_IsShow;

	TCHAR szBuf[256];

	int m_CModelFaceNum;
	int m_CModelVertexNum;

	int					nFPS;
	DWORD				tStart;
	DWORD               tFrameCnt;
	CVector3f			m_CameraPos;
	bool				m_bDisplayAperture;
};
