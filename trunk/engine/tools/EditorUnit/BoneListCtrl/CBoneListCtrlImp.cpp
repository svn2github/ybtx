#include "stdafx.h"
#include "CCtrlBase.h"
#include "CEditObserver.h"
#include "CEditContext.h"
#include "BoneListCtrl/CBoneListCtrlImp.h"
#include "ContextMsg.h"

DEF_UI_IMP(CBoneListCtrlImp);

#define MODEL ((CEditModel*)m_pOwnContext->GetActiveObj()) 

CBoneListCtrlImp::CBoneListCtrlImp( void* param )
{
	m_strLastSkeName = "";
}

CBoneListCtrlImp::~CBoneListCtrlImp( void )
{

}

void CBoneListCtrlImp::SetContext( CEditContext* pContext )
{
	CCtrlBaseObserver::SetContext(pContext);
}

void CBoneListCtrlImp::ActiveNode( CRenderNode* pNode )
{
	if (MODEL && MODEL->GetSketetalFrame())
	{
		if (m_strLastSkeName != MODEL->GetSketetalFrame()->GetName())
		{
			m_strLastSkeName = MODEL->GetSketetalFrame()->GetName();
			CCtrlBase::Update();
		}
	}
	else
	{
		m_strLastSkeName = "";
		CCtrlBase::Update();
	}
}

std::string sqr_tools::CBoneListCtrlImp::GetRootSkeletalName( void )
{
	if (MODEL && MODEL->GetSketetalFrame())
		return MODEL->GetSketetalFrame()->GetRootSkeletal().GetName();
	else
		return "";
}

void sqr_tools::CBoneListCtrlImp::FindSkeletal( const CSkeletal* ske, const CSkeletal** resultSke, std::string strName )
{
	if ( 0 == strName.compare(ske->GetName()) )
	{
		*resultSke = ske;
		return;
	}

	for (unsigned int i = 0; i < ske->GetChildNum(); i++)
	{
		FindSkeletal(ske->GetChild(i), resultSke, strName);

		if ( 0 == strName.compare(ske->GetChild(i)->GetName()) )
		{
			*resultSke = ske->GetChild(i);
			return;
		} 
	}
}

int sqr_tools::CBoneListCtrlImp::GetChildSkeletalNum( std::string strName )
{
	if (MODEL && MODEL->GetSketetalFrame())
	{
		const CSkeletal& rootSke = MODEL->GetSketetalFrame()->GetRootSkeletal();
		const CSkeletal* result = NULL;
		FindSkeletal(&rootSke, &result, strName);
		if (result)
			return result->GetChildNum();
		else
			return 0;
	} 
	else
	{
		return 0;
	}
}

std::string sqr_tools::CBoneListCtrlImp::GetChildSkeletalNameByIndex( std::string strName, int index )
{
	if ( MODEL && MODEL->GetSketetalFrame() )
	{
		const CSkeletal& rootSke = MODEL->GetSketetalFrame()->GetRootSkeletal();
		const CSkeletal* result = NULL;
		FindSkeletal(&rootSke, &result, strName);
		if (result)
			return result->GetChild(index)->GetName();
		else
			return "";
	} 
	else
	{
		return "";
	}
}

void sqr_tools::CBoneListCtrlImp::Reset( void )
{
	m_vtBoneNames.clear();
}

void sqr_tools::CBoneListCtrlImp::AddNode( std::string name )
{
	m_vtBoneNames.push_back(name);
}

//void sqr_tools::CBoneListCtrlImp::AddNodes( std::vector<std::string> str_vector )
//{
//
//}

void sqr_tools::CBoneListCtrlImp::SendMsg( void )
{
	MSG_INFO info;
	info._handle = (void*)&m_vtBoneNames;
	MSG_SYS_ADDBONE msg(info);
	m_pOwnContext->EventCollect(msg);
}