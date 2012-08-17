#include "stdafx.h"
#include "CDirectionInfo.h"
#include "SQRRootWnd.h"
#include "CCamera.h"
#include "MathAlgorithm.h"
#include "CEntityClientManager.h"
#include "CCharacterFollower.h"

CDirectionInfo::CDirectionInfo()
:m_uCharacterEntityID(0)
{
}

CDirectionInfo::~CDirectionInfo()
{
}

void CDirectionInfo::SetCharacterEntityID(uint32 uEntityID)
{
	m_uCharacterEntityID = uEntityID;
}

void CDirectionInfo::DrawWndBackground()
{
	CRenderDialog::DrawWndBackground();
}

void CDirectionInfo::OnPreRender( uint32 uCurTime,CCamera* pCamera  )
{
	CRenderDialog::OnPreRender(uCurTime,pCamera);
	
	if( !IsCreated() )
		return;
	
	if( m_bShow && m_pCamera)
	{
		uint32 uDirector = CEntityClientManager::GetInst()->GetDirectorID();
		CCharacterFollower* pDirector = CCharacterFollower::GetCharacterByID(uDirector);
		Ast (pDirector);
		CCharacterFollower* m_pCharacter = CCharacterFollower::GetCharacterByID(m_uCharacterEntityID);

		if (!m_pCharacter) 
			return;

		float fDistance = pDirector->GetEntityDist(m_pCharacter);


		CVector3f	vecPos;
		CVector2f	vecHW;
		CVector2f	vecTmp;
		CFRect		rt_tmp;

		GetRootWnd()->GetWndRect( rt_tmp );
		CIRect	rt((RECT)rt_tmp);
		CMatrix	matParent;
		m_pParentSpace->GetAttachMatrix( matParent, -1 );
		vecPos = SceneToScreen( rt, matParent.GetAxis(3), m_pCamera->getViewProjectMatrix() );
		float fScaleRate = (1.0f - vecPos.z)*SCALE_BY_Z;
		vecHW.x = GetWndHandle()->m_WidthInitial * fScaleRate;
		vecHW.y = GetWndHandle()->m_HeightInitial * fScaleRate;	
		vecTmp.x = rt.left + vecPos.x - vecHW.x * 0.5f - m_xPosition * fScaleRate; 
		vecTmp.y = rt.top  + vecPos.y - vecHW.y - m_yPosition * fScaleRate;


		if ( vecTmp.x < rt_tmp.left || vecTmp.x > rt_tmp.right || vecTmp.y < rt_tmp.bottom
			|| !( m_pCharacter->BeDangerForDirector() )
			|| fDistance > 768.f ) //768是12个格子的距离 12*64
		{
			m_bInit = false;
			GetWndHandle()->m_bShow = false;
			return;
		}
		else
		{
			GetWndHandle()->m_bShow = true;
			m_bInit = true;
		}

		float x = rt.left + vecPos.x - GetWndHandle()->m_WidthInitial * GetRootWnd()->GetZoomSize() * 0.5f - m_xPosition; 
		float y = rt.bottom - GetWndHandle()->m_HeightInitial * GetRootWnd()->GetZoomSize() - m_yPosition;

		CFRect rtWnd;
		GetWndRect( rtWnd );
		if( rtWnd.left != x || rtWnd.top != y )
		{
			rtWnd.left		= x;
			rtWnd.top		= y;
			rtWnd.right		= x + GetWndHandle()->m_WidthInitial;
			rtWnd.bottom	= y + GetWndHandle()->m_HeightInitial;
			ResizeRect( rtWnd );
		}
	}
}