//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "BluelyG.h"
#include "Scene.h"
#include "Render.h"

//#define MOVE_TOLERANCE 1 // in pixel

#define MOVE_SPEED_RATIO 32
#define ZOOM_SPEED_RATIO 1.0f

namespace bg
{
namespace sc
{

//////////////////////////////////////////////////////////////////////////

class CCamera : public ISceneNode
{
	D3DXVECTOR3 m_vecPos; 
	D3DXVECTOR3 m_vecView; 

	D3DXMATRIX  m_matView;
	D3DXMATRIX  m_matBillboard;
	D3DXMATRIX  m_matProjection;
	
	float m_fFOVY;
	float m_fAspect;
	float m_fNearPlane;
	float m_fFarPlane;
	
	rd::CFrustum m_frustum;
	
	bool m_bResetVS;
	
	bool m_bRotate;

private:
	void OnEvent(ip::CInputDevice::SEvent & event);
	
public:
	void CalculateAxis();
	void MoveRTS(const D3DXVECTOR3 & v);
	void MoveToRTSByView(D3DXVECTOR3 v);
	void RotatePosition(const D3DXVECTOR3 & v);
	void Zoom(const D3DXVECTOR3 & v);

	bool GetResetVS() { return m_bResetVS; }
	void SetResetVS(bool b) { m_bResetVS = b; }

	rd::CFrustum * GetFrustum();

	float GetNearPlane() { return m_fNearPlane;}
	float GetFarPlane()  { return m_fFarPlane;}
	
	D3DXVECTOR3 m_vecXaxis,m_vecYaxis,m_vecZaxis;
	
	D3DXVECTOR3 GetView() { return m_vecView; }
	
	D3DXVECTOR3 GetPosition() { return m_vecPos; }
	
	void SetFOVY(float fFOVY);
	
	float GetFOVY() { return D3DXToDegree(m_fFOVY); }
	
	float GetPitch() 
	{
		D3DXVECTOR3 vecVector = m_vecPos - m_vecView;
		return D3DXToDegree(atanf(abs(vecVector.y)/abs(sqrt(vecVector.x*vecVector.x+vecVector.z*vecVector.z))));
	}

	void SetPitchAndYaw(float fPitch, float fYaw)
	{
		D3DXVECTOR3 v(0.0f,0.0f,-1.0f);

		D3DXMATRIX matRotX;
		D3DXMatrixRotationX(&matRotX,D3DXToRadian(fPitch));
		D3DXVec3TransformCoord(&v,&v,&matRotX);
		
		D3DXMATRIX matRotY;
		D3DXMatrixRotationY(&matRotY,D3DXToRadian(-fYaw));
		D3DXVec3TransformCoord(&v,&v,&matRotY);
		
		v *= STDDISTANCE;
		m_vecPos = m_vecView + v;
		CalculateAxis();
	}

	void CCamera::SwitchToStandard(const D3DXVECTOR3 & vView)
	{
		m_vecView = vView;
		m_fFOVY = D3DXToRadian(STDFOV);
		this->SetPitchAndYaw(STDPITCH,STDYAW);

		//// adjust vpos to center
		//m_vecPos.x = (int)(m_vecPos.x/GRID_SPACE)*GRID_SPACE + 0.5*GRID_SPACE*m_vecPos.x/fabs(m_vecPos.x);
		//m_vecPos.z = (int)(m_vecPos.z/GRID_SPACE)*GRID_SPACE + 0.5*GRID_SPACE*m_vecPos.z/fabs(m_vecPos.z);
	}

	//void SetPitchAndYaw(float fPitch, float fYaw)
	//{
	//	D3DXVECTOR3 vecVector = m_vecPos - m_vecView;
	//	D3DXVECTOR3 v(0.0f,0.0f,0.0f);
	//	
	//	float f =  D3DXToDegree(atanf(abs(vecVector.y)/abs(sqrt(vecVector.x*vecVector.x+vecVector.z*vecVector.z))));
	//	v.x = fPitch - f;
	//	v.x = D3DXToRadian(v.x);

	//	v.y = fYaw - D3DXToDegree(atanf(abs(m_vecPos.x - m_vecView.x)/abs(m_vecPos.z - m_vecView.z)));
	//	v.y = D3DXToRadian(v.y);
	//	RotatePosition(v);
	//}
	
	void SetPitch(float f)
	{
		D3DXVECTOR3 v(0.0f,0.0f,0.0f);
		v.x = f - D3DXToDegree(atanf(abs(m_vecPos.y - m_vecView.y)/abs(m_vecPos.z - m_vecView.z)));
		v.x = D3DXToRadian(v.x);
		RotatePosition(v);
	}
	
	void SetYaw(float f)
	{
		D3DXVECTOR3 v(0.0f,0.0f,0.0f);
		v.y = f - D3DXToDegree(atanf(abs(m_vecPos.z - m_vecView.z)/abs(m_vecPos.x - m_vecView.x)));
		v.y = D3DXToRadian(v.y);
		RotatePosition(v);
	}
	
	void SetAspect(float fAspect);
	void SetNearClipPlane(float fNear);
	void SetFarClipPlane(float fFar);
	
	void SetPosition(const D3DXVECTOR3 & vecPos);
	void SetView(const D3DXVECTOR3 & vecView);
	
	void Render();
	
	const D3DXMATRIX * GetProjectionMatrix();
	const D3DXMATRIX * GetViewMatrix();
	const D3DXMATRIX * GetBillboard(void);

	CCamera(ISceneNode* pParent);
	virtual ~CCamera();

	float GetDistance() 
	{
		return D3DXVec3Length(&D3DXVECTOR3(m_vecView - m_vecPos));
	}
};

}
}