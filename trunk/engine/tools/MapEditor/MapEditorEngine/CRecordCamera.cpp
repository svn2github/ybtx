#include "stdafx.h"
#include "CRecordCamera.inl"
#include "CRecordCameraNode.h"
#include "CMapEditApp.h"
#include "Operator.h"
#include "Operator.inl"
#include "CCameraBase.inl"

namespace sqr
{
	static const float MOVE_VIEW_Y_SPEED	= 3.0f;
	static const float MOVE_SPEED_RATIO		= 64.0f;
	static const float ZOOM_SPEED_RATIO		= 10.0f;


	CRecordCamera::CRecordCamera(CCameraManager* camMgr)
		: CCameraBase(camMgr)
		, type(eTP_Position)
	{
		m_TargetPos = CVector3f(0.0f, 0.0f, 1.0f);
		
		SetCamType(eCT_FirstView);
		EventProc=new CRecordCameraNode(this);
	}

	CRecordCamera::~CRecordCamera()
	{
	}

	void CRecordCamera::Move( CVector3f v )
	{
		if ( type == eTP_Position )
			v.y = 0.0f;

		v.normalize();
		v *= m_fMovSpeed;

		m_NewPosition += v;
		m_NewTarget   += v;

		CMapEditApp::GetInst()->GetOperator()->MoveHitPoint(v);
	}

	void CRecordCamera::Rotate( float yaw, float pitch )
	{
		if ( !m_pCtrlNode || !m_isInCtrl )
			return;

		if ( type == eTP_Position )
			_RotatePosition(yaw, pitch);
		else
			_RotateTarget(yaw, pitch);
	}

	void CRecordCamera::Zoom( float fValue )
	{
		if ( !m_pCtrlNode || !m_isInCtrl )
			return;

		CCamera *pCamera = dynamic_cast<CCamera*>(m_pCtrlNode);
		CVector3f vDir = pCamera->getDIRECTION();
		vDir.normalize();
		vDir *= fValue*ZOOM_SPEED_RATIO;
		m_NewPosition += vDir;

		// 只有操作目标点时才移动目标点
		if ( type == eTP_Target )
			m_NewTarget   += vDir;

		CMapEditApp::GetInst()->GetOperator()->MoveHitPoint(pCamera->getDIRECTION().x, pCamera->getDIRECTION().y, 0);
	}

	void CRecordCamera::_RotatePosition( float yaw, float pitch )
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

	void CRecordCamera::_RotateTarget( float yaw, float pitch )
	{
		CCamera *pCamera = (CCamera*)m_pCtrlNode;

		m_fNewPitch   += pitch;
		m_fNewYaw	  += yaw;

		pCamera->RotateY(yaw);
		pCamera->Pitch(pitch);

		// 旋转观察点
		CVector3f vecLen	= GetNewTarget() - GetNewPosition();
		float	  fLen		= vecLen.Mag();
		CVector3f vecDir	= pCamera->getDIRECTION() * fLen;
		SetNewTarget(GetNewPosition() + vecDir);
	}

	void CRecordCamera::SetPitchAndYaw( float fPitch, float fYaw )
	{
		if ( !m_pCtrlNode || !m_isInCtrl )
			return;

		m_fNewPitch   = CMath::ToRadian(fPitch);
		m_fNewYaw	  = CMath::ToRadian(-fYaw);

		CMatrix matRotX, matRotY;
		CVector3f v(0.0f,0.0f,1.0f);
		matRotX.SetRotateX(m_fNewPitch);
		matRotY.SetRotateY(m_fNewYaw);

		v *= matRotX;
		v *= matRotY;

		CVector3f vecLen = m_NewTarget - m_NewPosition;
		float fViewLen = vecLen.Mag();
		v *= fViewLen;

		SetTarget(m_NewPosition + v);
	}

	void CRecordCamera::SwitchToStandard( const CVector3f& vPos )
	{
		SetPosition(vPos);
		SetTarget(vPos+CVector3f(0.0f, 0.0f, 1.0f));
		m_fNewFov = CMath::ToRadian(STDFOV);
		SetPitchAndYaw(STDPITCH,STDYAW);
	}

	sqr::CtrlState CRecordCamera::_DoCameraCtrl( uint32 uCurTime,CCamera* pCamera, CSceneQuery* pQuery )
	{
		float		fDeltaTime = float(uCurTime - m_uCurTime) * 0.001f;

		CVector3f camPosPre = pCamera->GetPosition();

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

	void CRecordCamera::DoCtrlUpdate(uint32 uCurTime)
	{
		this->DoCtrl(uCurTime);
	}
}