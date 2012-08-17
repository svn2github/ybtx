#include "StdAfx.h"
#include "GroundSpeedCtrl/CGroundSettingCtrlImp.h"
#include "CGraphic.h"
#include "CRenderScene.h"
#include "CDataScene.h"
#include "CEditContext.h"
#include "GroundSpeedCtrl/CGridNode.h"
#include "CEditAnimate.h"
#include "ContextMsg.h"

DEF_UI_IMP(CGroundSettingCtrlImp);

#define MODEL ((CEditModel*)m_pOwnContext->GetActiveObj()) 

CGroundSettingCtrlImp::CGroundSettingCtrlImp(void* param)
: m_pGridNode(NULL)
, m_pAnimate(NULL)
, m_bPlayState(false)
{
}

CGroundSettingCtrlImp::~CGroundSettingCtrlImp(void)
{
}

void CGroundSettingCtrlImp::SetContext( CEditContext* pContext )
{
	CCtrlBaseObserver::SetContext(pContext);
	ShowGrid(true);
}

void CGroundSettingCtrlImp::NotifyObserver( void )
{
	MSG_SYS_SELCHANGE msg(NULL);
	m_pOwnContext->EventCollect(msg);
}

void CGroundSettingCtrlImp::Update()
{
	CCtrlBase::Update();
}

void CGroundSettingCtrlImp::SetGroundSpeed( const float fSpeed )
{
	if (m_pAnimate)
		m_pAnimate->SetGroundSpeed(fSpeed);
	else
		return;

	if (m_pGridNode && MODEL->IsAnimate() && MODEL->GetAnimateGroup())
	{
		m_pGridNode->SetGroundSpeed(fSpeed);
	}
	else
	{
		m_pGridNode->SetGroundSpeed(0.0f);
	}

	Update();
}

float CGroundSettingCtrlImp::GetGroundSpeed() const
{
	if (m_pAnimate)
		return m_pAnimate->GetGroundSpeed();
	else
		return 0.0f;
}

void CGroundSettingCtrlImp::ShowGrid( bool isShow )
{
	if(isShow)
	{
		if(m_pGridNode)
			return;
		m_pGridNode = new CGridNode(ROS_LIGHTMAT1,(CRenderScene*)m_pOwnContext);
		m_pOwnContext->RegistDebugNode(m_pGridNode);
	}
	else
	{
		if(m_pGridNode)
		{
			m_pOwnContext->DelelteDebugNode(m_pGridNode);
			m_pGridNode = NULL;
		}
	}
}

void sqr_tools::CGroundSettingCtrlImp::ActiveNode( CRenderNode* pNode )
{
	if (MODEL)
	{
		IDNAME name = MODEL->GetCurAnimateName();
		if (MODEL->GetAnimateGroup())
		{
			uint32 AniNo = MODEL->GetAnimateGroup()->GetAniIDbyName( name );

			if ( FAILED( AniNo ) )
			{
				SetGroundSpeed(0.0f);
				return;
			}

			CAnimate* Ani = MODEL->GetAnimateGroup()->GetAniById(AniNo);
			if (m_pAnimate != (CEditAnimate*)Ani)
			{
				m_pAnimate = (CEditAnimate*)Ani;
				SetGroundSpeed(m_pAnimate->GetGroundSpeed());
				return;
			}

			if (!m_pAnimate)
			{
				m_pGridNode->SetGroundSpeed(0.0f);
				return;
			}

			if (m_bPlayState != MODEL->IsAnimate())
			{
				m_bPlayState = MODEL->IsAnimate();
				if (m_bPlayState)
				{
					m_pGridNode->SetGroundSpeed(m_pAnimate->GetGroundSpeed());
					return;
				}
				else
				{
					m_pGridNode->SetGroundSpeed(0.0f);
					return;					
				}
			}
		}
	}
}