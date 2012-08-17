#include "stdafx.h"
#include "CContextCom.h"
#include "ContextCtrl/CContextCtrl.h"
#include "CUIBase.h"
#include "PathManager/CPathManager.h"
#include "CtrlImpHelper.h"


#define ScenePropImp ((CContextCtrlImp*)GetCoreCtrl())
CContextComponent::CContextComponent(void)
{
	m_pCtrl = NULL;
}

CContextComponent::~CContextComponent(void)
{
	if(m_pCtrl)
		m_pCtrl->Destroy();
}

void*	CContextComponent::GetCoreCtrl(void)
{
	if(m_pCtrl == NULL)
	{
		char* ImpName = GET_IMP_NAME(CContextCtrlImp);
		if( ImpName == NULL )
			return NULL;

		m_pCtrl=(CCtrlBase*)CUIBase::CreateCtrlImp( ImpName, NULL );

		ScenePropImp->ShowGrid(m_bShowGrid);
		ScenePropImp->ShowBlock(m_bShowBlock);
		ScenePropImp->SetAmbientColor(m_cAmbinetColor.ToArgb());
		ScenePropImp->SetSunLightColor(m_cSunColor.ToArgb());
		ScenePropImp->LoadScene(MagStringToUnmagString(m_strSceneName));
	}
	return m_pCtrl;
}

System::String^ CContextComponent::SceneName::get()
{
	if(m_pCtrl)
	{
		m_strSceneName = UnmagStringToMagString(ScenePropImp->GetSceneName());
	}
	return m_strSceneName;
}

void CContextComponent::SceneName::set(System::String^ value)
{
	m_strSceneName = value;
	if(m_pCtrl)
		ScenePropImp->LoadScene(MagStringToUnmagString(m_strSceneName));
}

System::Drawing::Color CContextComponent::AmbientColor::get()
{
	if(m_pCtrl)
	{
		m_cAmbinetColor.FromArgb(ScenePropImp->GetAmbientColor());
	}
	return m_cAmbinetColor;
}

void CContextComponent::AmbientColor::set(System::Drawing::Color value)
{
	m_cAmbinetColor = value;
	if(m_pCtrl)
	{
		ScenePropImp->SetAmbientColor(m_cAmbinetColor.ToArgb());
	}
}

System::Drawing::Color CContextComponent::SunColor::get()
{
	if(m_pCtrl)
	{
		m_cSunColor.FromArgb(ScenePropImp->GetSunLightColor());
	}
	return m_cSunColor;
}

void CContextComponent::SunColor::set(System::Drawing::Color value)
{
	m_cSunColor = value;
	if(m_pCtrl)
	{
		ScenePropImp->SetSunLightColor(m_cSunColor.ToArgb());
	}
}

bool CContextComponent::ShowGrid::get()
{
	if(m_pCtrl)
		m_bShowGrid = ScenePropImp->IsShowGrid();
	return m_bShowGrid;
}

void CContextComponent::ShowGrid::set(bool value)
{
	m_bShowGrid = value;
	if(m_pCtrl)
		ScenePropImp->ShowGrid(m_bShowGrid);
}

bool CContextComponent::ShowBlock::get()
{
	if(m_pCtrl)
		m_bShowBlock = ScenePropImp->IsShowBlock();
	return m_bShowBlock;
}

void CContextComponent::ShowBlock::set(bool value)
{
	m_bShowBlock = value;
	if(m_pCtrl)
		ScenePropImp->ShowBlock(m_bShowBlock);
}

void SceneSeleteEditor::InitializeDialog( System::Windows::Forms::OpenFileDialog^ openFileDialog )
{
	CPathSetImp* pPathImp=static_cast<CPathSetImp*>(CUIBase::CreateCtrlImp(GET_IMP_NAME(CPathSetImp),NULL));
	openFileDialog->Multiselect = false;
	openFileDialog->Filter = "场景文件(*.Arg)|*.Arg";
	openFileDialog->Title = "打开新场景";

	if(pPathImp)
	{
		if( openFileDialog->InitialDirectory->Empty )
		{
			openFileDialog->InitialDirectory = UnmagStringToMagString(pPathImp->GetPath(0).c_str());
		}
		pPathImp->Destroy();
	}
}