// ParamsView.cpp : 实现文件
//

#include "stdafx.h"
#include "MapEditor.h"
#include "ParamsView.h"
#include ".\paramsview.h"
#include <Engine.h>
#include <Camera.h>
#include <UserMsg.h>

// CParamsView

CParamsView::CParamsView()
	: CFormView(CParamsView::IDD)
	, m_fCameraPitch(0)
	, m_fCameraFOV(0)
	, m_fCameraX(0)
	, m_fCameraY(0)
	, m_fCameraZ(0)
{
}

CParamsView::~CParamsView()
{
}

void CParamsView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDITROLL, m_fCameraPitch);
	DDX_Text(pDX, IDC_EDITFOV, m_fCameraFOV);
	DDX_Text(pDX, IDC_EDITX, m_fCameraX);
	DDX_Text(pDX, IDC_EDITY, m_fCameraY);
	DDX_Text(pDX, IDC_EDITZ, m_fCameraZ);
}

BEGIN_MESSAGE_MAP(CParamsView, CFormView)
	ON_EN_CHANGE(IDC_EDITROLL, OnEnChangeEditPitch)
	ON_EN_CHANGE(IDC_EDITFOV, OnEnChangeEditfov)
END_MESSAGE_MAP()


// CParamsView 诊断

#ifdef _DEBUG
void CParamsView::AssertValid() const
{
	CFormView::AssertValid();
}

void CParamsView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

void CParamsView::OnEnChangeEditPitch()
{
	UpdateData();
	eg::CEngine::GetInstance()->GetActiveCamera()->SetPitch(m_fCameraPitch);
}

void CParamsView::OnEnChangeEditfov()
{
	UpdateData();
	eg::CEngine::GetInstance()->GetActiveCamera()->SetFOVY(m_fCameraFOV);
}

LRESULT CParamsView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch ( message ) 
	{	
	case WM_UPDATECAMERA:
		{
			sc::CCamera * pCamera = eg::CEngine::GetInstance()->GetActiveCamera();
			m_fCameraPitch = pCamera->GetPitch();
			m_fCameraFOV = pCamera->GetFOVY();
			m_fCameraX = pCamera->GetPosition().x;
			m_fCameraY = pCamera->GetPosition().y,
			m_fCameraZ = pCamera->GetPosition().z;
			UpdateData(false);
		}
	break;
	}
	
	return CFormView::WindowProc(message, wParam, lParam);
}
