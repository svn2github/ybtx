#include "stdafx.h"
#include "SkeletonEditCtrl/CSkeletonEditImp.h"
#include "SkeletonEditCtrl/CSkeletonDelegate.h"
#include "CEditContext.h"
#include "BaseHelper.h"
#include "CEditModel.h"
#include "CEditRenderStyle.h"
#include "CEditPieceGroup.h"
#include "CEditAnimationsRes.h"
#include "CRenderDebugFun.h"
#include "PathManager/CPathManager.h"

DEF_UI_IMP(CSkeletonEditImp);

#define MODEL ((CEditModel*)m_pOwnContext->GetActiveObj()) 

CSkeletonEditImp::CSkeletonEditImp( void* param )
:m_pSkeletalFrame(NULL)
,m_pModel(NULL)
,m_pDelegate(NULL)
{
	m_strCurSkeName = "";
	m_strCurSelSkeBoneName = "";
	m_bShowSkeleton = false;
}

CSkeletonEditImp::~CSkeletonEditImp( void )
{
	if (m_pDelegate)
	{
		delete m_pDelegate;
		m_pDelegate = NULL;
	}
}

void CSkeletonEditImp::SetContext( CEditContext* pContext )
{
	CCtrlBaseObserver::SetContext(pContext);

	if (m_pDelegate)
		return;
	m_pDelegate = new CSkeletonDelegate(this);
	m_pOwnContext->RegisterDelegate(m_pDelegate);
	m_pOwnContext->ActiveDelegate(m_pDelegate);
}

void CSkeletonEditImp::SetModel( CEditModel* model )
{
	m_pModel = model;
}

void CSkeletonEditImp::ActiveNode( CRenderNode* pNode )
{
	if (MODEL)
	{
		if (m_pSkeletalFrame != MODEL->GetSketetalFrame())
		{
			m_pSkeletalFrame = MODEL->GetSketetalFrame();
			if (m_pSkeletalFrame)
				m_strCurSkeName = m_pSkeletalFrame->GetName();
			else
			{
				m_strCurSkeName =  m_strCurSelSkeBoneName = "";
			}

			CCtrlBase::Update();
		}
	}
}

std::string CSkeletonEditImp::GetCurSkeName( void )
{
	return m_strCurSkeName;
}

void CSkeletonEditImp::SetCurSelSkeBoneName( std::string str )
{
	m_strCurSelSkeBoneName = str;
	CCtrlBase::Update();
}

std::string CSkeletonEditImp::GetCurSelSkeBoneName( void )
{
	return m_strCurSelSkeBoneName;
}

void CSkeletonEditImp::SetShowSkeleton( bool value )
{
	m_bShowSkeleton = value;
}

void CSkeletonEditImp::SetSkeBoneSize( int delta )
{
	if(MODEL && MODEL->GetSketetalFrame())
		MODEL->CalSkeletalSize(delta);
}

void CSkeletonEditImp::ChangeCurSelSkeBoneName( std::string str )
{
	if(MODEL && MODEL->GetSketetalFrame())
	{
		int sel = MODEL->GetCurSelSkelatal();
		if(!str.empty() && sel != -1)
			(*MODEL->GetSketetalFrame())[MODEL->GetCurSelSkelatal()]->m_pSkeletal->SetName(str.c_str());
	}
}

std::string CSkeletonEditImp::SaveSke(void)
{
	if(MODEL && MODEL->GetSketetalFrame())
	{
		string skename = MODEL->GetSketetalFrame()->GetName();
		string tempRootPath;
		tempRootPath = CPathSetImp::GetInst()->GetPath(0);
		tempRootPath += "/";
		tempRootPath +=	skename;
		replace(tempRootPath.begin(), tempRootPath.end(), '\\', '/');
		if(((CEditSkeletalFrame*)MODEL->GetSketetalFrame())->BinSave(tempRootPath))
			return tempRootPath+"保存成功";
		else
			return tempRootPath+"保存失败";
	}
	else
		return "不存在骨架";
}

void CSkeletonEditImp::PickBone( int mouseX, int mouseY )
{
	PickSkeletalParam pick( mouseX, mouseY, 
		m_pOwnContext->GetWindowTarget()->GetWidth()/2,m_pOwnContext->GetWindowTarget()->GetHeight()/2 );
	if (MODEL)
	{
		MODEL->PickSkeletal( pick );
		SetCurSelSkeBoneName(pick.m_SkeletalName);
	}
}

void CSkeletonEditImp::Render( void )
{
	if (MODEL)
	{
		if (m_bShowSkeleton)
		{
			MODEL->RenderBone();
		}
	}
}

