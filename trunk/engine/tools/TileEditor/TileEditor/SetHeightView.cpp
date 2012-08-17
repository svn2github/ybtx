// SetHeightView.cpp : 实现文件
//

#include "stdafx.h"
#include "MapEditor.h"
#include "SetHeightView.h"
#include ".\setheightview.h"


// CSetHeightView

CSetHeightView::CSetHeightView()
	: CFormView(CSetHeightView::IDD)
{
}

CSetHeightView::~CSetHeightView()
{
}

void CSetHeightView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSetHeightView, CFormView)
	ON_BN_CLICKED(IDC_GAUSS, OnBnClickedGauss)
	ON_BN_CLICKED(IDC_LINEAR, OnBnClickedLinear)
END_MESSAGE_MAP()

// CSetHeightView 诊断

#ifdef _DEBUG
void CSetHeightView::AssertValid() const
{
	CFormView::AssertValid();
}

void CSetHeightView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// CSetHeightView 消息处理程序

void CSetHeightView::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
//	radios.CheckRadio(this,IDC_LINEAR, IDC_GAUSS, IDC_GAUSS);
    CFormView::PreSubclassWindow();
}

BOOL CSetHeightView::OnCmdMsg(UINT nID, int nCode, void* pExtra,
							  AFX_CMDHANDLERINFO* pHandlerInfo)
{
//	m_radios.CheckRadio( this, nID );
	return CFormView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}		

void CSetHeightView::OnBnClickedGauss()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CSetHeightView::OnBnClickedLinear()
{
	// TODO: 在此添加控件通知处理程序代码
}