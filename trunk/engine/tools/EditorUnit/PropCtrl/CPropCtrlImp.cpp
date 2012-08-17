#include "StdAfx.h"
#include "PropCtrl\CPropCtrlImp.h"
#include "PropCtrl\CAudioPropCtrlImp.h"
#include "PropCtrl\CModelPropCtrlImp.h"
#include "PropCtrl\CLightPropCtrlImp.h"
#include "PropCtrl\CScenePropCtrlImp.h"

DEF_UI_IMP(CPropCtrlImp)

// PropType  0 场景 1物件 2灯光 3音效 

CPropCtrlImp::CPropCtrlImp(void* _handle)
{
	m_CurPropType=PROP_SCENE;

	m_AudioPropImp=new CAudioPropCtrlImp();
	m_ScenePropImp=new CScenePropCtrlImp();
	m_ModelPropImp=new CModelPropCtrlImp();
	m_LightPropImp=new CLightPropCtrlImp();
}

CPropCtrlImp::~CPropCtrlImp(void)
{
}

DWORD CPropCtrlImp::Filter(void)
{
	return 0;
}

void CPropCtrlImp::ActiveNode( CRenderNode* pNode )
{
	 CCtrlBase::Update();
}

int CPropCtrlImp::GetCurPropType()
{
	return m_CurPropType;
}

void CPropCtrlImp::SetContext(CEditContext* pContext)
{
	CCtrlBaseObserver::SetContext(pContext);
	m_AudioPropImp->SetContext(pContext);
	m_ScenePropImp->SetContext(pContext);
	m_ModelPropImp->SetContext(pContext);
	m_LightPropImp->SetContext(pContext);
	CCtrlBase::Update();
}

CAudioPropCtrlImp*	CPropCtrlImp::GetAudioPropImp()
{
	return m_AudioPropImp;
}

CScenePropCtrlImp*	CPropCtrlImp::GetScenePropImp()
{
	return m_ScenePropImp;
}

CModelPropCtrlImp*	CPropCtrlImp::GetModelPropImp()
{
	return m_ModelPropImp;
}

CLightPropCtrlImp*	CPropCtrlImp::GetLightPropImp()
{
	return m_LightPropImp;
}

//获取选中物体的属性
int CPropCtrlImp::GetSelObjectType()
{
	return 1;
}
