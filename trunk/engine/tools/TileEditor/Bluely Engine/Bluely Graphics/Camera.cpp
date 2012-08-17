//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#include "extradefine.h"
#include "Camera.h"
#include "../Bluely Common/Console.h"
#include "Render.h"
#include "Input.h"
#include "BluelyGUI.h"
#include "CTileEditApp.h"
#include "Operator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace bg
{
namespace sc
{

//////////////////////////////////////////////////////////////////////////

CCamera::CCamera(ISceneNode * pParent) : ISceneNode(pParent)
{
	m_vecPos = D3DXVECTOR3(0.0f,0.0f,-5.0f);
	m_vecView  = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_fFOVY = 90.0;
	
	D3DVIEWPORT9 viewPort;
	rd::CRenderDevice::GetInstance()->GetRawDevice()->GetViewport(&viewPort);
	
	m_fAspect = float(viewPort.Width)/float(viewPort.Height);
	m_fNearPlane = 0.0f;
	m_fFarPlane  = 100.0f;
	m_bResetVS = true;
}

CCamera::~CCamera() 
{ 
}

const D3DXMATRIX * CCamera::GetViewMatrix()  
{
	return D3DXMatrixLookAtLH(&m_matView,&m_vecPos,&m_vecView,&D3DXVECTOR3(0.0f,1.0f,0.0f));
}

const D3DXMATRIX * CCamera::GetProjectionMatrix() 
{
	return D3DXMatrixPerspectiveFovLH(&m_matProjection,m_fFOVY,m_fAspect,m_fNearPlane,m_fFarPlane);
}

void CCamera::SetPosition(const D3DXVECTOR3 & vecPos)
{
	m_vecPos = vecPos;
	CalculateAxis();
}

void CCamera::SetView(const D3DXVECTOR3 & vecView)
{
	m_vecView = vecView;
	CalculateAxis();
}

void CCamera::SetNearClipPlane(float fNear)
{
	m_fNearPlane = fNear;
}

void CCamera::SetFarClipPlane(float fFar)
{
	m_fFarPlane = fFar;
}

void CCamera::SetAspect(float fAspect)
{
	m_fAspect = fAspect;
}

void CCamera::SetFOVY(float fFOVY)
{
	m_fFOVY = D3DXToRadian(fFOVY);
}

void CCamera::RotatePosition(const D3DXVECTOR3 & v)
{	
	D3DXVECTOR3 vecVector = m_vecPos - m_vecView;

	// get distance
	float d = sqrtf(vecVector.x*vecVector.x + vecVector.y*vecVector.y + vecVector.z*vecVector.z);
	float reta = acosf(vecVector.y/d);

	if ( ( reta < D3DXToRadian(5)  && v.x > 0 ) ||( reta > D3DXToRadian(175) && v.x < 0))  return;

	// rot by y
	D3DXMATRIX matRotY;
	D3DXMatrixRotationY(&matRotY,v.y);
	//D3DXMatrixRotationAxis(&matRotY,&m_vecYaxis,v.y);
	D3DXVec3TransformCoord(&vecVector,&vecVector,&matRotY);
	m_vecPos = m_vecView + vecVector;
	CalculateAxis();

	// rot by x
	D3DXMATRIX matRotX;
	// D3DXMatrixRotationX(&matRotX,v.x);
	D3DXMatrixRotationAxis(&matRotX,&m_vecXaxis,v.x);
	D3DXVec3TransformCoord(&vecVector,&vecVector,&matRotX);
	m_vecPos = m_vecView + vecVector;
	CalculateAxis();
}


//void CCamera::Rotate(D3DXVECTOR3 * pVec)
//{
//	D3DXVECTOR3 vecVector = m_vecView - m_vecPos;
//
//	CalculateAxis();
//
//	// rot by y
//	D3DXMATRIX matRotY;
//	D3DXMatrixRotationAxis(&matRotY,&m_vecYaxis,pVec->y);
//	D3DXVec3TransformCoord(&vecVector,&vecVector,&matRotY);
//
//	// rot by x
//	D3DXMATRIX matRotX;
//	D3DXMatrixRotationAxis(&matRotX,&m_vecXaxis,pVec->x);
//	D3DXVec3TransformCoord(&vecVector,&vecVector,&matRotX);
//
//	m_vecView = m_vecPos + vecVector;
//
//	CalculateAxis();
//}
//
//void CCamera::MovePositionCamera(D3DXVECTOR3 * pVec)
//{
//	CalculateAxis();
//	m_vecPos += m_vecXaxis * pVec->x;
//	m_vecPos += m_vecYaxis * pVec->y;
//	m_vecPos += m_vecZaxis * pVec->z;
//}

void CCamera::MoveRTS(const D3DXVECTOR3 & v)
{
	m_vecPos += v;
	m_vecView += v;
//	m_vecView.y = 0; // locked to xz plane
	CalculateAxis();
}

void CCamera::MoveToRTSByView(D3DXVECTOR3 v)
{
	v = v - m_vecView; 
	m_vecPos += v;
	m_vecView += v;
	//	m_vecView.y = 0; // locked to xz plane
	CalculateAxis();
	m_bResetVS = true;
}

void CCamera::Zoom(const D3DXVECTOR3 & v)
{
	m_vecPos += v;
	CalculateAxis();
}

//void CCamera::MoveView(D3DXVECTOR3 * pVec)
//{
//	CalculateAxis();
//	m_vecView += m_vecXaxis * pVec->x;
//	m_vecView += m_vecYaxis * pVec->y;
//	m_vecView += m_vecZaxis * pVec->z;
//}

void CCamera::CalculateAxis()
{
	m_vecZaxis = m_vecView - m_vecPos;
	D3DXVec3Normalize(&m_vecZaxis,&m_vecZaxis);
	D3DXVec3Cross(&m_vecXaxis,&D3DXVECTOR3(0,1,0),&m_vecZaxis);
	D3DXVec3Normalize(&m_vecXaxis,&m_vecXaxis);
	D3DXVec3Cross(&m_vecYaxis,&m_vecZaxis,&m_vecXaxis);
}

const D3DXMATRIX * CCamera::GetBillboard(void)
{
	D3DXMatrixLookAtLH(&m_matView,&m_vecPos,&m_vecView,&D3DXVECTOR3(0.0f,1.0f,0.0f));
	D3DXMatrixInverse(&m_matBillboard, NULL, &m_matView );
	m_matBillboard._41 = m_matBillboard._42 = m_matBillboard._43 = 0.0f;
	return &m_matBillboard;
}

//void CCamera::FlatMoveFB(float c)
//{
//	CalculateAxis();
//	m_vecZaxis.y = 0;
//	m_vecZaxis *= c;
//	m_vecPos += m_vecZaxis ;
//	m_vecView += m_vecZaxis ;
//}
//
//void CCamera::FlatMoveLR(float c)
//{
//	CalculateAxis();
//	m_vecXaxis.y = 0;
//	m_vecXaxis *=c;
//	m_vecPos += m_vecXaxis ;
//	m_vecView += m_vecXaxis ;
//}

void CCamera::OnEvent(ip::CInputDevice::SEvent & event)
{
	// handle mouse
	switch ( event.eType )
	{
	case ip::EIET_KB_KEYDOWN :
		{

			//if ( event.dwData == DIK_COMMA )
			//{
			//	D3DXVECTOR3 vecVector = m_vecPos - m_vecView;
			//	D3DXVec3Normalize(&vecVector,&vecVector);
			//	vecVector *= -ZOOM_SPEED_RATIO;
			//	this->Zoom(vecVector);
			//	m_bResetVS = true;
			//}

			//if ( event.dwData == DIK_PERIOD )
			//{
			//	D3DXVECTOR3 vecVector = m_vecPos - m_vecView;
			//	D3DXVec3Normalize(&vecVector,&vecVector);
			//	vecVector *= ZOOM_SPEED_RATIO;
			//	this->Zoom(vecVector);
			//	m_bResetVS = true;
			//}

			// handle keyboard
			D3DXVECTOR3 vecVector;

			// left
			if ( event.dwData == DIK_A )
			{
				vecVector = -m_vecXaxis;
				vecVector.y = 0;
				D3DXVec3Normalize(&vecVector,&vecVector);
				vecVector *= MOVE_SPEED_RATIO;
				this->MoveRTS(vecVector);
				m_bResetVS = true;
			}

			// right
			if ( event.dwData == DIK_D )
			{
				vecVector = m_vecXaxis;
				vecVector.y = 0;
				D3DXVec3Normalize(&vecVector,&vecVector);
				vecVector *= MOVE_SPEED_RATIO;
				this->MoveRTS(vecVector);
				m_bResetVS = true;
			}

			// up
			if ( event.dwData == DIK_W )
			{
				vecVector = m_vecView - m_vecPos;
				vecVector.y = 0;
				D3DXVec3Normalize(&vecVector,&vecVector);
				vecVector *= MOVE_SPEED_RATIO;
				this->MoveRTS(vecVector);
				m_bResetVS = true;
			}

			// down
			if ( event.dwData == DIK_S )
			{
				vecVector = m_vecPos - m_vecView;
				vecVector.y = 0;
				D3DXVec3Normalize(&vecVector,&vecVector);
				vecVector *= MOVE_SPEED_RATIO;
				this->MoveRTS(vecVector);
				m_bResetVS = true;
			}

			// zoom fov
			if ( event.dwData == DIK_N )
			{
				m_fFOVY += 0.025;
				if ( m_fFOVY > 3.14f ) m_fFOVY = 3.14f;
			}
			
			if ( event.dwData == DIK_M )
			{
				m_fFOVY -= 0.025;
				if ( m_fFOVY < 0.0f ) m_fFOVY = 0.0f;
			}

			if ( event.dwData == DIK_G )
			{
				m_fFOVY = D3DXToRadian(STDFOV);
				this->SetPitchAndYaw(STDPITCH,STDYAW);
			}

			if ( event.dwData == DIK_H )
			{
				m_fFOVY = D3DXToRadian(STDFOV);
				this->SetPitchAndYaw(STDPITCH+15,STDYAW);
			}
		}
	break;

	case ip::EIET_MS_WHEEL :
		{
			D3DXVECTOR3 vecVector = m_vecPos - m_vecView;
			D3DXVec3Normalize(&vecVector,&vecVector);
			vecVector *= -event.fData*ZOOM_SPEED_RATIO;
			this->Zoom(vecVector);
			m_bResetVS = true;
		}
	break;

	case ip::EIET_MS_MOVE :
		{
			if ( ip::CInputDevice::GetInstance()->GetButtonState(MK_RBUTTON) ) // right button down rotate
			{
				int x,y;
				ip::CInputDevice::GetInstance()->GetMouseDelta(x,y);
				float fx = 0.02f * y;
				float fy = 0.02f * x;
				RotatePosition(D3DXVECTOR3(fx,fy,0));
				m_bResetVS = true;
			}

			if ( ip::CInputDevice::GetInstance()->GetButtonState(MK_MBUTTON) )
			{
				int x,y;
				ip::CInputDevice::GetInstance()->GetMouseDelta(x,y);
				D3DXVECTOR3 vecVector = m_vecPos - m_vecView;
				D3DXVec3Normalize(&vecVector,&vecVector);
				vecVector *= y*ZOOM_SPEED_RATIO;
				this->Zoom(vecVector);
				m_bResetVS = true;
			}
		}			
	break;
	}
	
	//// handle keyboard
	//D3DXVECTOR3 vecVector;


	//// left
	//if ( ip::CInputDevice::GetInstance()->GetKeyState(DIK_LEFT) & 0x80 )
	//{
	//	vecVector = -m_vecXaxis;
	//	vecVector.y = 0;
	//	D3DXVec3Normalize(&vecVector,&vecVector);
	//	vecVector *= MOVE_SPEED_RATIO;
	//	this->MoveRTS(vecVector);
	//	m_bResetVS = true;
	//}
	//
	//// right
	//if ( ip::CInputDevice::GetInstance()->GetKeyState(DIK_RIGHT) & 0x80 )
	//{
	//	vecVector = m_vecXaxis;
	//	vecVector.y = 0;
	//	D3DXVec3Normalize(&vecVector,&vecVector);
	//	vecVector *= MOVE_SPEED_RATIO;
	//	this->MoveRTS(vecVector);
	//	m_bResetVS = true;
	//}

	//// up
	//if ( ip::CInputDevice::GetInstance()->GetKeyState(DIK_UP) & 0x80 )
	//{
	//	vecVector = m_vecView - m_vecPos;
	//	vecVector.y = 0;
	//	D3DXVec3Normalize(&vecVector,&vecVector);
	//	vecVector *= MOVE_SPEED_RATIO;
	//	this->MoveRTS(vecVector);
	//	m_bResetVS = true;
	//}

	//// down
	//if ( ip::CInputDevice::GetInstance()->GetKeyState(DIK_DOWN) & 0x80 )
	//{
	//	vecVector = m_vecPos - m_vecView;
	//	vecVector.y = 0;
	//	D3DXVec3Normalize(&vecVector,&vecVector);
	//	vecVector *= MOVE_SPEED_RATIO;
	//	this->MoveRTS(vecVector);
	//	m_bResetVS = true;
	//}

	if ( m_bResetVS )
	{
	}
}

rd::CFrustum * CCamera::GetFrustum() 
{
	m_frustum.CalculateFrustum(this->GetViewMatrix(),this->GetProjectionMatrix());
	return &m_frustum;
}

void CCamera::Render()
{
	rd::CRenderDevice * pRender = rd::CRenderDevice::GetInstance();
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	pRender->GetRawDevice()->SetTransform(D3DTS_WORLD,&matWorld);
	pRender->GetRawDevice()->SetTransform(D3DTS_VIEW,this->GetViewMatrix());
	pRender->GetRawDevice()->SetTransform(D3DTS_PROJECTION,this->GetProjectionMatrix());
}

}
}