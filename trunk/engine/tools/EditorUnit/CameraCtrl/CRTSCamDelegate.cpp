#include "stdafx.h"
#include "CameraCtrl\CRTSCamDelegate.h"
#include "CEventState.h"
#include "CMsg.h"
#include "CEditContext.h"
#include "CMath.h"
#include "CCameraConfig.h"
#include "CDataScene.h"
#include "CLogicDataMgr.h"
#include "BaseMath.h"

CRTSCamDelegate::CRTSCamDelegate(void)
{
	m_fMouseRadian	= 0.0f;

	m_fMoveXDist	= 0.0f;
	m_fMoveZDist	= 0.0f;

	m_fRadianSpeed	= 3.0f * 0.001f;
	m_fDistSpeed	= 1.0f * eGridSpan;
	m_fMoveSpeed	= 64.0f * 0.001f * 4.0f;

	m_bAutoHight	= true;

	SetMinDist(7*eGridSpan);
	SetMaxDist(22*eGridSpan);
	SetDist(22*eGridSpan);

	SetMinRadian(CMath::ToRadian(5));
	SetMaxRadian(CMath::ToRadian(45));
	SetYRadian(CMath::ToRadian(45));
	SetMaxYDegree( CMath::two_pi );
	SetMinNearPlan(0.1f);
	SetMaxNearPlan(0.1f);
	SetMinYOffset( 1.0f * eGridSpan );
	SetMaxYOffset(-1.65f * eGridSpan );
}

CRTSCamDelegate::~CRTSCamDelegate(void)
{

}

bool CRTSCamDelegate::MsgProc(const sqr::CMsg& msg)
{
	switch( msg.msgSource )
	{
	case MGS_SYSMSG:
		{
			switch(msg.msgType.sys_type)
			{
			case MGT_SYS_RESIZE:
				_InitCameraInfo();
				break;
			}
		}
		break;
	case MGS_MOUSEMSG:
		{
			switch(msg.msgType.mouse_type)
			{
			case MGT_MOUSE_RBNDOWN:
				{
					m_fMouseRadian = msg.msgInfo.mouse_info.X;
				}
			case MGT_MOUSE_MOVE:
				if(CEventState::GetInst()->GetState(MGT_MOUSE_RBUTTON) == MGT_MOUSE_RBNDOWN)
				{
					float yRad =  GetYRotateDegree();
					SetYRotateDegree( yRad + (msg.msgInfo.mouse_info.X - m_fMouseRadian) * m_fRadianSpeed );
					m_fMouseRadian = msg.msgInfo.mouse_info.X;
				}
				break;
			case MGT_MOUSE_WHEEL:
				{
					SetDist( GetDist() - msg.msgInfo._iinfo * m_fDistSpeed );
					if(m_pContext)
						m_pContext->SetCameraOffset(GetDist());
				}
				break;
			default:
				break;
			}
			return true;
		}
		break;
	case MGS_KEYMSG:
		{
			switch(msg.msgType.key_type)
			{
			case MGT_KEY_DOWN:
				{
					switch(msg.msgInfo.key_info)
					{
					case MGI_KEY_A:
						{
							m_fMoveXDist = -m_fMoveSpeed;	
						}
						break;
					case MGI_KEY_D:
						{
							m_fMoveXDist = m_fMoveSpeed;
						}
						break;
					case MGI_KEY_W:
						{
							m_fMoveZDist = m_fMoveSpeed;
						}
						break;
					case MGI_KEY_S:
						{
							m_fMoveZDist = -m_fMoveSpeed;
						}	
						break;
					default:
						break;
					}
				}
				break;
			case MGT_KEY_UP:
				{
					switch(msg.msgInfo.key_info)
					{
					case MGI_KEY_A:
						{
							if( m_fMoveXDist == -m_fMoveSpeed )
							{
								m_fMoveXDist = 0.0f;
							}
						}
						break;
					case MGI_KEY_D:
						{
							if( m_fMoveXDist == m_fMoveSpeed )
							{
								m_fMoveXDist = 0.0f;
							}
						}
						break;
					case MGI_KEY_W:
						{
							if( m_fMoveZDist == m_fMoveSpeed )
							{
								m_fMoveZDist = 0.0f;
							}
						}
						break;
					case MGI_KEY_S:
						{
							if( m_fMoveZDist == -m_fMoveSpeed )
							{
								m_fMoveZDist = 0.0f;
							}
						}	
						break;
					default:
						break;
					}
					break;	
				}
				break;
			default:
				break;
			}
			return false;
		}
	default:
		break;
	}
	return CEventDelegate::MsgProc(msg);
}

CtrlState CRTSCamDelegate::_DoCameraCtrl( uint32 uCurTime,CCamera* pCamera, CSceneQuery* pQuery )
{
	if(!gIsZero(m_fMoveXDist) || !gIsZero(m_fMoveZDist))
	{
		float X = (uCurTime - m_uCurTime) * m_fMoveXDist;
		float Z = (uCurTime - m_uCurTime) * m_fMoveZDist;
		CVector3f tmpMove( X , 0.0f, Z );
		CMatrix mat;
		mat.SetRotateY( GetYRotateDegree() + GetYRadian() );
		tmpMove.Rotate(mat);
		CVector3f tarPos = GetTargetPos() + tmpMove;
	
		if( m_pContext && m_pContext->GetDataScene() )
		{
			tarPos.x = limit2(tarPos.x, 0.0f, float(m_pContext->GetDataScene()->GetWidthInPixel()));
			tarPos.z = limit2(tarPos.z, 0.0f, float(m_pContext->GetDataScene()->GetHeightInPixel()));
			if(m_bAutoHight)
				tarPos.y = m_pContext->GetDataScene()->GetOffsetHeightInPixel( tarPos.x , tarPos.z);
			else
				tarPos.y = m_fCameraHight;
			if(m_pContext->GetDataScene()->GetWidthInGrid() 
				&& m_pContext->GetDataScene()->GetHeightInGrid())
				m_pContext->SetCameraDest(tarPos.x,tarPos.z);
		}		
		SetTargetPos( tarPos );

	}
	return CRTSCameraCtrller::_DoCameraCtrl( uCurTime, pCamera, pQuery );
}

void CRTSCamDelegate::_Init( CEventOwner* pOwner )
{
	m_pContext = dynamic_cast<CEditContext*>(pOwner);
	if(m_pContext)
	{
		_InitCameraInfo();
		m_pContext->SetCameraController(this);
		Active();
	}
}

void CRTSCamDelegate::_InitCameraInfo( void )
{
	if(m_pContext)
	{
		CIRect rtClient;
		CWindowTarget* pTarget = m_pContext->GetWindowTarget();
		if(pTarget==NULL)
			return;

		pTarget->GetRect(rtClient);
		if (rtClient.Width() == 0 || rtClient.Height() == 0)
			return;

		int	idx	= (int)(ECam_Mode16_10);
		if (rtClient.Width() != 0.0f)
		{
			float ratio[4] = { 9.0f/16, 10.0f/16, 3.0f/4, 4.0f/5 };
			float dis	= 100.0f;
			float cur_ratio =  ratio[0];
			cur_ratio = float(rtClient.Height()) / float(rtClient.Width());
			for (int i = 0; i < 4; i++)
			{
				if ( dis > abs(cur_ratio - ratio[i]) )
				{
					dis = abs(cur_ratio - ratio[i]);
					idx = i;
				}
			}
		}
		ECamMode cm = (ECamMode)idx;

		this->SetMaxXZ_Dist( CCameraConfig::GetInst()->GetCamMaxHOffset(cm) * eGridSpan );
		this->SetMinDist( CCameraConfig::GetInst()->GetCamMinDist(cm) * eGridSpan );
		this->SetMaxDist( CCameraConfig::GetInst()->GetCamMaxDist(cm) * eGridSpan );
		this->SetDist(	CCameraConfig::GetInst()->GetCamDefDist(cm)* eGridSpan );
		this->SetMaxRadian( CCameraConfig::GetInst()->GetCamMaxXRadian(cm) );
		this->SetMinRadian( CCameraConfig::GetInst()->GetCamMinXRadian(cm) );
		this->SetMinFov( CCameraConfig::GetInst()->GetCamMinFovRadian(cm) );
		this->SetMaxFov( CCameraConfig::GetInst()->GetCamMaxFovRadian(cm) );
		this->SetMinYOffset( CCameraConfig::GetInst()->GetCamMinYOffset(cm) * eGridSpan );
		this->SetMaxYOffset( CCameraConfig::GetInst()->GetCamMaxYOffset(cm) * eGridSpan );
		this->SetYRadian( CCameraConfig::GetInst()->GetCamYRadian(cm) );
		this->SetMaxNearPlan( CCameraConfig::GetInst()->GetCamMaxNearPlan(cm) * eGridSpan );
		this->SetMinNearPlan( CCameraConfig::GetInst()->GetCamMinNearPlan(cm) * eGridSpan );
		this->SetDistSpeed( CCameraConfig::GetInst()->GetCamSpeed(cm) * eGridSpan );
		this->ReSetPosNow();
	}
}