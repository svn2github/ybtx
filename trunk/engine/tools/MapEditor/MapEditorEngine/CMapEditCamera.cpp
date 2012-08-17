#include "stdafx.h"
#include "CMapEditCamera.inl"
#include "CCameraBase.inl"
#include "CMapEditApp.h"
#include "Operator.h"
#include "Operator.inl"
#include "CWindowTarget.h"

namespace sqr
{
	static const float MOVE_VIEW_Y_SPEED	= 3.0f;
	static const float MOVE_SPEED_RATIO		= 64.0f;
	static const float ZOOM_SPEED_RATIO		= 20.0f;
	static const float INIT_VALUE			= -100000000.0f;

	CMapEditCamera::CMapEditCamera(CCameraManager* camMgr)
		: CCameraBase(camMgr)
		, EventProc(0)
	{
		this->m_fFarClip = 10000.0f;
		m_TargetPos = CVector3f(INIT_VALUE, INIT_VALUE, INIT_VALUE);
		m_bOrthogonal=false;
		m_bYBTXCamera=true;
		CIRect viewPort;
		CMainWindowTarget::GetInst()->GetRect(viewPort);
		m_fPatchWidth  = float(viewPort.Width());
		m_fPatchHeight = float(viewPort.Height());
		SetCamType(eCT_MapEdit);
		EventProc=new CMapEditCameraNode(this);
	}

	CMapEditCamera::~CMapEditCamera()
	{
		delete EventProc;
	}

	void CMapEditCamera::RotatePosition(float yaw, float pitch)
	{
		if ( !m_pCtrlNode || !m_isInCtrl )
			return;

		CCamera *pCamera = (CCamera*)m_pCtrlNode;
		
		m_fNewPitch  += pitch;
		m_fNewYaw	 += yaw;

		pCamera->centerRotate( m_NewTarget, CVector3f::UNIT_Y, yaw );
		pCamera->centerPitch( m_NewTarget, pitch );
		m_NewPosition = m_pCtrlNode->GetPosition();
	}

	void CMapEditCamera::Zoom(float fValue)
	{
		if ( !m_pCtrlNode || !m_isInCtrl )
			return;
		
		CCamera *pCamera = dynamic_cast<CCamera*>(m_pCtrlNode);
		CVector3f vDir = pCamera->getDIRECTION();
		float dis = vDir.Mag();
		vDir.normalize();
		vDir *= fValue*ZOOM_SPEED_RATIO;
		dis = vDir.Mag();

		CVector3f vecPos = m_NewPosition + vDir;
		if ( (m_NewTarget - vecPos).MagSqr() <= 5000.0f )
		{
			return;
		}
		m_NewPosition = vecPos;

		CMapEditApp::GetInst()->GetOperator()->MoveHitPoint(pCamera->getDIRECTION().x, pCamera->getDIRECTION().y, 0);
	}

	void CMapEditCamera::SetIsOrthogonal(bool b)
	{
		if ( !m_pCtrlNode || !m_isInCtrl )
			return;

		m_bOrthogonal = b;
	
		CCamera *pCamera = dynamic_cast<CCamera*>(m_pCtrlNode);
		if(m_bOrthogonal)
		{
			pCamera->SetOrthoProject(true);
			pCamera->SetOrthoSize(m_fPatchWidth,m_fPatchHeight);
		}
		else
		{
			pCamera->SetOrthoProject(false);
		}
	}

	void CMapEditCamera::AdjustViewY(bool bUp)
	{
		if ( !m_pCtrlNode || !m_isInCtrl )
			return;

		if ( bUp )
		{
			m_NewTarget += CVector3f( 0.0f,MOVE_VIEW_Y_SPEED,0.0f );
		}
		else
		{
			m_NewTarget -= CVector3f( 0.0f,MOVE_VIEW_Y_SPEED,0.0f );
		}
	}

	void CMapEditCamera::AdjustBothY(bool bUp)
	{
		if ( !m_pCtrlNode || !m_isInCtrl )
			return;

		if ( bUp )
		{
			m_NewTarget   += CVector3f( 0.0f,MOVE_VIEW_Y_SPEED,0.0f );
			m_NewPosition += CVector3f(0.0f,MOVE_VIEW_Y_SPEED,0.0f);
		}
		else
		{
			m_NewTarget   -= CVector3f( 0.0f,MOVE_VIEW_Y_SPEED,0.0f );
			m_NewPosition -= CVector3f(0.0f,MOVE_VIEW_Y_SPEED,0.0f);
		}
	}

	void CMapEditCamera::SetPitchAndYaw( float fPitch, float fYaw, bool bLogic /*= false*/ )
	{
		if ( !m_pCtrlNode || !m_isInCtrl )
			return;

		m_fNewPitch  = CMath::ToRadian(fPitch);
		m_fNewYaw	 = CMath::ToRadian(fYaw);

		CMatrix matRotX, matRotY;
		CVector3f v(0.0f,0.0f,-1.0f);
		matRotX.SetRotateX( m_fNewPitch);
		matRotY.SetRotateY( m_fNewYaw );

		v *= matRotX;
		v *= matRotY;

		if ( bLogic )
		{
			v *= STDLOGICDISTANCE;
		}
		else if (m_bOrthogonal )
		{
			v *= STDMINIDISTANCE;
		}
		else
		{
			v *= STDDISTANCE;
		}
		CCamera *pCamera		 = (CCamera*)m_pCtrlNode;
		SetPosition(m_NewTarget + v);
		SetTarget(m_NewTarget);
	}

	CVector3f CMapEditCamera::GetDirection()
	{
		CVector3f dir = GetTarget() - GetPosition();
		dir.normalize();
		return dir;
	}

	void CMapEditCamera::MoveRTS(CVector3f v)
	{
		v.y = 0;
		v.normalize();
		v *= m_fMovSpeed;

		m_NewPosition += v;
		m_NewTarget   += v;

		CMapEditApp::GetInst()->GetOperator()->MoveHitPoint(v);
	}
	
	//void CMapEditCamera::AddToScene( ISceneNode *pParent )
	//{
	//	ASSERT(pParent);
	//	ASSERT(!m_pImpl->m_pCamNode);

	//	m_pImpl->m_pCamNode = new CMapEditCameraNode(this, pParent);
	//}

	//void CMapEditCamera::RemoveFromScene()
	//{
	//	ASSERT(m_pImpl->m_pCamNode);

	//	m_pImpl->m_pCamNode->Remove();
	//	m_pImpl->m_pCamNode = NULL;
	//}

	sqr::CtrlState CMapEditCamera::_DoCameraCtrl( uint32 uCurTime,CCamera* pCamera, CSceneQuery* pQuery )
	{
		float		fDeltaTime = float(uCurTime - m_uCurTime) * 0.001f;

		CVector3f camPosPre = pCamera->GetPosition();
		CVector3f vecPosDir = m_NewPosition - camPosPre;
		CVector3f vecTgtDir = m_NewTarget - m_TargetPos;

		if ( m_NewPosition != camPosPre )
		{
			pCamera->SetPosition(m_NewPosition);
		}

		float fovPre = pCamera->getFOVy();
		if ( m_fNewFov != fovPre  )
		{
			pCamera->setFOVy(m_fNewFov);
		}

		if ( m_NewTarget != m_TargetPos || m_bInitBind )
		{
			pCamera->lookAt(m_NewTarget);
			m_TargetPos = m_NewTarget;
			m_bInitBind = false;
		}

		if ( m_fFarClip != pCamera->getFarDist() )
		{
			if ( m_fFarClip < m_fNearClip )
				pCamera->setFarDist(m_fNearClip);
			else
				pCamera->setFarDist(m_fFarClip);
		}		

		return CS_CTRL;	
	}

	void CMapEditCamera::DoCtrlUpdate(uint32 uCurTime)
	{
		this->DoCtrl(uCurTime);
		CCamera *pCamera = (CCamera*)m_pCtrlNode;
		if(pCamera)
		{
			pCamera->getUP();
		}
	}
}