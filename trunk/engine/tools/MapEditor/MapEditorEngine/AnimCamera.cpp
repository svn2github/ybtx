#include "stdafx.h"
#include "AnimCamera.h"
#include "Render.h"
#include "Input.h"
#include "BluelyGUI.h"
#include "CMapEditApp.h"
#include "Operator.h"
#include "TerrainAlgorithm.h"
#include "Graphic_dx9\CDX9RenderPipeline.h"

#include "CTrace.h"

namespace sqr
{
	CAnimCamera::CAnimCamera(ISceneNode* pParent) 
		: CBGCamera(pParent)
	{

	}

	void CAnimCamera::OnEvent(InputEvent & event)
	{
		COperator *pOperator = NULL;
		pOperator = CMapEditApp::GetInst()->GetOperator();
		if ( pOperator == NULL || !m_bActive )
		{
			return;
		}

		CVector3f vecVector;
		CVector3f hitpoint = pOperator->GetHitPoint();
		switch ( event.eType )
		{
		case EIET_KB_KEYDOWN :
			{
				// handle keyboard
				if ( m_bMovable )
				{
					// left
					if ( event.dwData == DIK_A )
					{
						vecVector	= -m_pCamEx->getRIGHT();
						vecVector.y = 0;
						vecVector.normalize();
						vecVector   *= MOVE_SPEED_RATIO;
						MoveRTS(vecVector) ;
						hitpoint.x	+= vecVector.x;
						hitpoint.z	+= vecVector.z;
						pOperator->SetHitPoint(hitpoint);
						m_bResetVS  = true;
					}

					// right
					if ( event.dwData == DIK_D )
					{
						vecVector	= m_pCamEx->getRIGHT();
						vecVector.y = 0;
						vecVector.normalize();
						vecVector   *= MOVE_SPEED_RATIO;
						MoveRTS(vecVector) ;
						hitpoint.x	+= vecVector.x;
						hitpoint.z	+= vecVector.z;
						pOperator->SetHitPoint(hitpoint);
						m_bResetVS  = true;
					}

					// up
					if ( event.dwData == DIK_W )
					{
						vecVector	= m_pCamEx->getDIRECTION();
						vecVector.y	= 0;
						vecVector.normalize();
						vecVector	*= MOVE_SPEED_RATIO;
						MoveRTS(vecVector);
						hitpoint.x	+= vecVector.x;
						hitpoint.z	+= vecVector.z;
						pOperator->SetHitPoint(hitpoint);
						m_bResetVS  = true;
					}

					// down
					if ( event.dwData == DIK_S )
					{
						vecVector	= -m_pCamEx->getDIRECTION();
						vecVector.y	= 0;
						vecVector.normalize();
						vecVector	*= MOVE_SPEED_RATIO;
						MoveRTS(vecVector);
						hitpoint.x	+= vecVector.x;
						hitpoint.z	+= vecVector.z;
						pOperator->SetHitPoint(hitpoint);
						m_bResetVS = true;
					}


					// up another type
					if ( event.dwData == DIK_Q )
					{
						vecVector	= m_pCamEx->getDIRECTION();
						vecVector   *= MOVE_SPEED_RATIO;
						MoveRTS(vecVector);
						hitpoint.x	+= m_pCamEx->getDIRECTION().x;
						hitpoint.y	+= m_pCamEx->getDIRECTION().y;
						pOperator->SetHitPoint(hitpoint);
						m_bResetVS = true;
						CMapEditApp::GetInst()->GetOperator()->SetGPushed(false);
					}

					// down another type
					if ( event.dwData == DIK_E )
					{
						vecVector	= -m_pCamEx->getDIRECTION();
						vecVector   *= MOVE_SPEED_RATIO;
						MoveRTS(vecVector);
						hitpoint.x	-= m_pCamEx->getDIRECTION().x;
						hitpoint.y	-= m_pCamEx->getDIRECTION().y;
						pOperator->SetHitPoint(hitpoint);
						m_bResetVS = true;
						CMapEditApp::GetInst()->GetOperator()->SetGPushed(false);
					}
				}
			}
			break;

		case EIET_MS_WHEEL :
			{
				vecVector	= -m_pCamEx->getDIRECTION();
				vecVector.normalize();
				vecVector	*= -event.fData*ZOOM_SPEED_RATIO;
				Zoom(vecVector);
				hitpoint.x	+= vecVector.x;
				hitpoint.z	+= vecVector.z;
				pOperator->SetHitPoint(hitpoint);
				m_bResetVS = true;
			}
			break;

		case EIET_MS_MOVE :
			{
				if ( CInputDevice::GetInst()->GetButtonState(MK_RBUTTON) ) // right button down rotate
				{
					if( pOperator->GetEditState() == COperator::EES_SET_TILE_BLOCK )
						return;

					if ( m_bRotate )
					{
						int x,y;
						CInputDevice::GetInst()->GetMouseDelta(x,y);
						float pitch = 0.004f * y;
						float yaw	= 0.004f * x;

						// ÈÆ×ÔÉíÐý×ª
						Rotate(yaw, pitch);

						m_bResetVS = true;
						CMapEditApp::GetInst()->GetOperator()->SetGPushed(false);
					}
				}

				if ( CInputDevice::GetInst()->GetButtonState(MK_MBUTTON) )
				{
					int x,y;
					CInputDevice::GetInst()->GetMouseDelta(x,y);
					vecVector  = -m_pCamEx->getDIRECTION();
					vecVector *= y*ZOOM_SPEED_RATIO;
					Zoom(vecVector);
					m_bResetVS = true;
					CMapEditApp::GetInst()->GetOperator()->SetGPushed(false);
				}
			}
			break;

		default:
			break;
		}

		if ( m_bResetVS )
		{
			CMapEditApp::GetInst()->GetOperator()->UpdateParamsView();
		}
	}

	void CAnimCamera::Rotate( float yaw, float pitch )
	{
		m_pCamEx->rotateY(yaw);
		m_pCamEx->pitch(pitch);

		CVector3f vecPos	= m_pCamEx->getPosition();
		CVector3f vecTarget = m_pCamEx->getTargetPosition();
		CVector3f vecLen	= vecTarget - vecPos;
		float	  fLen		= vecLen.Mag();
		CVector3f vecDir	= m_pCamEx->getDIRECTION() * fLen;
		vecDir = vecPos + vecDir;

		DebugOut("%f, %f, %f\n", vecDir.x, vecDir.y, vecDir.z );
		
		SetTarget( vecDir );
	}
}