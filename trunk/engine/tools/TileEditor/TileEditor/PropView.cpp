// PropView.cpp : 实现文件
//

#include "stdafx.h"
#include "MapEditor.h"
#include "PropView.h"
#include "Brain.h"
#include "Props.h"
#include ".\propview.h"
#include "MainFrm.h"
#include "ids.h"

#define HORZ_PAD 5
#define VERT_HEIGHT 26

// CPropView

CPropView::CPropView()
	: CFormView(CPropView::IDD)
{
	m_nControlCnt = INVALID;
	m_nCurPos = -1;
	m_dwControlHeight = m_dwControlWidth = 0; 
	m_pBrain = NULL;
}

CPropView::~CPropView()
{
}

void CPropView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPropView, CFormView)
	ON_WM_SHOWWINDOW()
	//ON_EN_CHANGE(IDC_EDITBOOL1, OnEnChangeEditBool)
	//ON_EN_CHANGE(IDC_EDITBOOL2, OnEnChangeEditBool)
	//ON_EN_CHANGE(IDC_EDITBOOL3, OnEnChangeEditBool)
	//ON_EN_CHANGE(IDC_EDITBOOL4, OnEnChangeEditBool)
	//ON_EN_CHANGE(IDC_EDITBOOL5, OnEnChangeEditBool)

	//ON_EN_CHANGE(IDC_EDITINT1, OnEnChangeEditInt)
	//ON_EN_CHANGE(IDC_EDITINT2, OnEnChangeEditInt)
	//ON_EN_CHANGE(IDC_EDITINT3, OnEnChangeEditInt)
	//ON_EN_CHANGE(IDC_EDITINT4, OnEnChangeEditInt)
	//ON_EN_CHANGE(IDC_EDITINT5, OnEnChangeEditInt)

	//ON_EN_CHANGE(IDC_EDITREAL1, OnEnChangeEditReal)
	//ON_EN_CHANGE(IDC_EDITREAL2, OnEnChangeEditReal)
	//ON_EN_CHANGE(IDC_EDITREAL3, OnEnChangeEditReal)
	//ON_EN_CHANGE(IDC_EDITREAL4, OnEnChangeEditReal)
	//ON_EN_CHANGE(IDC_EDITREAL5, OnEnChangeEditReal)

	//ON_EN_CHANGE(IDC_EDITTEXT1, OnEnChangeEditText)
	//ON_EN_CHANGE(IDC_EDITTEXT2, OnEnChangeEditText)
	//ON_EN_CHANGE(IDC_EDITTEXT3, OnEnChangeEditText)
	//ON_EN_CHANGE(IDC_EDITTEXT4, OnEnChangeEditText)
	//ON_EN_CHANGE(IDC_EDITTEXT5, OnEnChangeEditText)
END_MESSAGE_MAP()


// CPropView 诊断

#ifdef _DEBUG
void CPropView::AssertValid() const
{
	CFormView::AssertValid();
}

void CPropView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

void CPropView::Init()
{
	CRect rcClient;
	this->GetClientRect( &rcClient );
	m_dwControlWidth = (rcClient.Width() - (HORZ_PAD*4))/3;
	m_dwControlHeight = VERT_HEIGHT;
	
	CMainFrame * pMainFrame = ((CMainFrame*)AfxGetMainWnd());
	
	if ( !pMainFrame ) return;
	
//	if ( m_pBrain == pMainFrame->GetActiveBrain() ) return;
	
	if ( m_pBrain )
	{
		for ( int i = 0; i < m_pBrain->GetPropCnt(); i++ )
		{
			CBaseProp * pProp = m_pBrain->GetProp(i);
			pProp->Term();
		}
	}
	
	m_pBrain = pMainFrame->GetActiveBrain();
	
	if ( !m_pBrain ) return;
	
	for ( int i = 0; i < m_pBrain->GetPropCnt(); i++ )
	{
		CBaseProp * pProp = m_pBrain->GetProp(i);
		CRect rcLabel,rcControl;
		this->GetLabelRect(i,rcLabel);
		this->GetControlRect(i,rcControl);
		pProp->ShowMe(rcLabel,rcControl,this,i);
	}
}

void CPropView::GetLabelRect(int nLabelIndex, CRect & rect)
{
	rect.left = HORZ_PAD;
	rect.top = ( nLabelIndex - m_nCurPos ) * m_dwControlHeight;
	rect.right = (rect.left + m_dwControlWidth) - HORZ_PAD;
	rect.bottom = rect.top + m_dwControlHeight;
}

void CPropView::GetControlRect(int nControlIndex, CRect &rect)
{
	rect.left = (HORZ_PAD)+m_dwControlWidth;
	rect.top = ( nControlIndex - m_nCurPos ) * m_dwControlHeight;
	rect.right = (rect.left + m_dwControlWidth) - HORZ_PAD;
	rect.bottom = rect.top + m_dwControlHeight;
}

void CPropView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CFormView::OnShowWindow(bShow, nStatus);

	if ( bShow )
	{
		this->Init();
	}
}

BOOL CPropView::OnCommand(WPARAM wParam, LPARAM lParam)
{	
	if ( m_pBrain )
	{
		CBaseProp * pProp = m_pBrain->GetProp(LOWORD(wParam)-IDC_PROPBASE);
		
		if ( HIWORD(wParam) == EN_CHANGE )
			pProp->UpdateValue();
	}
	return CFormView::OnCommand(wParam, lParam);
}