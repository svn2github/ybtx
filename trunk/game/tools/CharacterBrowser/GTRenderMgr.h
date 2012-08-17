#pragma once
#include "CRenderSystem.h"
#include "CRenderFont.h"
#include "CRenderObject.h"
#include "TSingleton.h"
#include "CTPSCameraCtrller.h"

namespace sqr
{
	class CCamera;
};

namespace sqr_tools
{
	class CEditModel;
}

enum EModelType;

class CGTRenderMgr : public Singleton<CGTRenderMgr>
{
public:
	CGTRenderMgr(CRenderGroup* pRenderGroup);
	~CGTRenderMgr(void);

	//场景名、模型名、动画名、模型个数
	CRenderObject* CreateCharacter(string name,EModelType type);
	void DisplayCharacterAperture(float fScale);
	void DoAnimate(string name);
	void CreateScene(const string& strScenePath);
	void OnRender();
	void SetCameraPos(DWORD direction,int plength);
	void DestroyObj(void);
	void HideApertureAndRefer();


	bool m_IsShow;
	bool IsShow() {return m_IsShow;}
	CCamera* m_pCamera;
	string	GetModelFileName() {return m_ModelFileName;}
	string	GetFxFileName();
	CRenderObject* GetDummyCharacter();

	//CRenderScene* GetRenderScene() { return pRenderScene; }
	CMiniScene			m_MiniScene;
	CTPSCameraCtrller	m_TPSCtrller;
private:
	CEditModel*	m_pRenderObj;
	CRenderObject*	m_pDummyRenderObj;
	CRenderObject*	m_pReferenceRenderObj;
	CRenderFont *	m_pFont;
	string		m_ModelFileName;
	string		m_LastNpcFxName;

	TCHAR szBuf[256];

	int m_CModelFaceNum;
	int m_CModelVertexNum;

	int					nFPS;
	DWORD				tStart;
	DWORD               tFrameCnt;
	CVector3f			m_CameraPos;
	bool						m_bDisplayAperture;
};
