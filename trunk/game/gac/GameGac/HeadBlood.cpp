#include "stdafx.h"
#include "HeadBlood.h"

HeadBlood::HeadBlood()
{
}

HeadBlood::~HeadBlood()
{
}

void HeadBlood::CreateBloodRendlerDialog(SQRWnd* ParentWnd, bool bShow, EBloodDialogType eBDType)
{
	switch(eBDType)
	{
	case eBDT_Normal:
		{
			CreateFromRes("BloodWnd",ParentWnd);
		}
		break;
	case eBDT_GAttack:
		{
			CreateFromRes("BloodWndGAttack",ParentWnd);
		}
		break;
	case eBDT_SuperNpc:
		{
			CreateFromRes("BloodWndSuperNpc",ParentWnd);
		}
		break;
	case eBDT_RingLeader:
		{
			CreateFromRes("BloodWndRingLeader",ParentWnd);
		}
		break;
	case eBDT_Leader:
		{
			CreateFromRes("BloodWndLeader",ParentWnd);
		}
		break;
	case eBDT_Boss:
		{
			CreateFromRes("BloodWndBoss",ParentWnd);
		}
		break;
	default:
		GenErr("HeadBlood::CreateBloodRendlerDialog异常状况!");
		return;
	}
	
	m_BloodProgress = class_cast<SQRProgress*>(GetDlgChild("BloodProgress"));
	SetIsBottom(true);
	SetRenderWndHeight(60);
	m_BloodProgress->SetProgressMode(0);
	ShowWnd( bShow );
}

void HeadBlood::UpdateInfo(uint32 HealthPoint, uint32 HealthPointAgile, bool bIsFeignDeath)
{
	if(!IsShow())
	{
		ShowWnd(true);
	}
	if(HealthPointAgile == 0)
	{
		ShowWnd(false);
		return;
	}
	Ast(m_BloodProgress);
	m_BloodProgress->SetRange(HealthPoint);
	if(bIsFeignDeath)
	{
		m_BloodProgress->SetPos(0);
	}
	else
	{
		m_BloodProgress->SetPos(HealthPointAgile);
	}
}
void HeadBlood::SetHeadBloodTex(EBloodDialogType eBDType,bool bIsEnemy)
{
	IMAGE_PARAM IP = IMAGE_PARAM(SM_BS_BK,IP_FILL);
	WND_IMAGE_LIST *pList = m_BloodProgress->GetWndBkImage(IP);
	pList->DeleteImage(-1);

	CFRect rt;
	switch(eBDType)
	{
	case eBDT_GAttack:
		rt = CFRect( 0, 0, 37, 7);
		break;
	case eBDT_Normal:
		rt = CFRect( 0, 8, 73, 15);
		break;
	case  eBDT_RingLeader:
		rt = CFRect( 0, 16, 98, 23);
		break;
	case eBDT_SuperNpc:
		rt = CFRect( 0, 16, 98, 23);
		break;
	case eBDT_Leader:
		rt = CFRect( 0, 32, 153, 39);
		break;
	case eBDT_Boss:
		rt = CFRect( 0, 32, 153, 39);
		break;
	default:
		GenErr("HeadBlood::CreateBloodRendlerDialog异常状况!");
		rt = CFRect( 0, 32, 153, 39);
	}

	string strtex;
	if (bIsEnemy)
		strtex = "guitex/tex/ybtx/gui/2ndEdition/Misc/bloodbar_yellow.tex";
	else
		strtex = "guitex/tex/ybtx/gui/2ndEdition/Misc/bloodbar_green.tex";

	pList->AddImage(m_BloodProgress->GetGraphic(),-1,strtex.c_str(),&rt, CFPos(2.0f, 2.0f),0xffffffff);
}