#include "stdafx.h"
#include "CBaseComponent.h"
#include "CUIBase.h"
#include "CCtrlBase.h"
#include "ContextCtrl/CContextCtrl.h"

void ComponentCallBack(void* pUI)
{
	IntPtr ptr = (IntPtr)pUI;
	CBaseComponent^ pUIBase = (CBaseComponent^)System::Runtime::InteropServices::Marshal::GetObjectForIUnknown(ptr);	
	if(pUIBase)
		pUIBase->Update();
}



CContextComponent^ CBaseComponent::ContextComponent::get()			
{
	return m_pContextCom;	
}

void CBaseComponent::ContextComponent::set(CContextComponent^ val)
{ 
	m_pContextCom = val;		
	if(m_pContextCom!=nullptr && GetCoreCtrl())
	{
		CContextCtrlImp* pContextImp = (CContextCtrlImp*)m_pContextCom->GetCoreCtrl();
		if(pContextImp)
			m_pCtrl->SetContext(pContextImp->GetContext());
	}
}


CBaseComponent::CBaseComponent(void)
{
	m_pCtrl=NULL;	
	m_pContextCom = nullptr;
}

void* CBaseComponent::GetImpParam(void)
{
	return NULL;
}

System::Void CBaseComponent::_InitCtrl(void)
{}

CBaseComponent::~CBaseComponent(void)
{
	if(m_pCtrl)
	{
		m_pCtrl->RegisterUICallBack( System::Runtime::InteropServices::Marshal::GetIUnknownForObject(this).ToPointer() ,NULL);
		m_pCtrl->Destroy();
	}
	m_pCtrl=NULL;
}

void* CBaseComponent::GetCoreCtrl()
{
	if(m_pCtrl == NULL)
	{
		char* ImpName = GetImpTypeName();
		if( ImpName == NULL )
			return NULL;

		m_pCtrl=(CCtrlBase*)CUIBase::CreateCtrlImp( ImpName, GetImpParam() );
		if(m_pCtrl)
		{
			m_pCtrl->RegisterUICallBack( System::Runtime::InteropServices::Marshal::GetIUnknownForObject(this).ToPointer() , ComponentCallBack);
			if(m_pContextCom!=nullptr)
			{
				CContextCtrlImp* pContextImp = (CContextCtrlImp*)m_pContextCom->GetCoreCtrl();	
				if(pContextImp)
					m_pCtrl->SetContext(pContextImp->GetContext());
			}
			_InitCtrl();
		}
	}
	return m_pCtrl;
}

void CBaseComponent::Update(void)
{
}

CContextComponent^ CBaseComponent::GetContextComponet( void )
{
	return m_pContextCom;
}