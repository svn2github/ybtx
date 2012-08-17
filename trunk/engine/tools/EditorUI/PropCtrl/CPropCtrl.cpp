#include "stdafx.h"
#include "CPropCtrl.h"
#include "PropCtrl\CPropCtrlImp.h"

#define PropCtrlImp ((CPropCtrlImp*)GetCoreCtrl())

char* CPropCtrl::GetImpTypeName(void)
{
	return GET_IMP_NAME(CPropCtrlImp);
}

System::Void CPropCtrl::_InitCtrl(void)
{	
	if(!sceneProp)
		sceneProp=gcnew sqr_tools::CSceneProp(this->m_pCtrl);
	if(!modelProp)
		modelProp=gcnew sqr_tools::CModelProp(this->m_pCtrl);
	if(!lightProp)
		lightProp=gcnew sqr_tools::CLightProp(this->m_pCtrl);
	if(!audioProp)
		audioProp=gcnew sqr_tools::CAudioProp(this->m_pCtrl);
	this->m_PropView->SelectedObject=sceneProp;
}

//定时器更新
void CPropCtrl::UpdateTimer(void)
{
	if(!m_PropView || !PropCtrlImp)
		return ;
	int pType=PropCtrlImp->GetCurPropType();
	switch(pType)
	{
	case 0:
		{
			if(sceneProp)
				sceneProp->UpdateTimerProp();
			break;
		}
	case 1:
		{
			if(modelProp)
				modelProp->UpdateTimerProp();
			break;
		}
	case 2:
		{
			if(lightProp)
				lightProp->UpdateTimerProp();
			break;
		}
	case 3:
		{
			if(audioProp)
				audioProp->UpdateTimerProp();
			break;
		}
	case PROP_BASE:
		{
			this->m_PropView->SelectedObject = nullptr;
			break;
		}
	}
	if(this->m_IsEdit->Checked==false)
		this->m_PropView->Refresh();
}

//一次更新
void CPropCtrl::Update(void)
{
	if(!m_PropView || !PropCtrlImp)
		return ;
	int pType=PropCtrlImp->GetCurPropType();
	switch(pType)
	{
	case 0:
		{
			if(sceneProp)
			{
				this->m_PropView->SelectedObject = sceneProp;
				sceneProp->UpdateProp();
			}
			break;
		}
	case 1:
		{
			if(modelProp)
			{
				this->m_PropView->SelectedObject = modelProp;
				modelProp->UpdateProp();
			}
			break;
		}
	case 2:
		{
			if(lightProp)
			{
				this->m_PropView->SelectedObject = lightProp;
				lightProp->UpdateProp();
			}
			break;
		}
	case 3:
		{
			if(audioProp)
			{
				this->m_PropView->SelectedObject = audioProp;
				audioProp->UpdateProp();
			}
			break;
		}
	case PROP_BASE:
		{
			this->m_PropView->SelectedObject = nullptr;
			break;
		}
	}
}