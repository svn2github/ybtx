// FormatSelectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TextureEditor.h"
#include "FormatSelectDlg.h"
#include ".\formatselectdlg.h"


// CFormatSelectDlg dialog

IMPLEMENT_DYNAMIC(CFormatSelectDlg, CDialog)
CFormatSelectDlg::CFormatSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFormatSelectDlg::IDD, pParent)
	, m_FrameCircle(0)
{
}

CFormatSelectDlg::~CFormatSelectDlg()
{
}

void CFormatSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	int RadioID = IDC_ARGB32;

	if(m_Format==TF_DXT1) 
		RadioID = IDC_DXT1;
	else if(m_Format==TF_DXT5) 
		RadioID = IDC_DXT5;
	else if(m_Format==TF_RGB24) 
		RadioID = IDC_RGB24;
	else if(m_Format==TF_R5G6B5) 
		RadioID = IDC_R5G6B5;
	else if(m_Format==TF_A4R4G4B4) 
		RadioID = IDC_A4R4G4B4;
	else if(m_Format==TF_A8)
		RadioID = IDC_A8;

	CDialog::CheckRadioButton(IDC_DXT1,IDC_A4R4G4B4,RadioID);
	CDialog::CheckDlgButton( IDC_ENABLEMIP, m_bMipMap );
	DDX_Text(pDX, IDC_FRAMETIME, m_FrameCircle);
}


BEGIN_MESSAGE_MAP(CFormatSelectDlg, CDialog)
	ON_BN_CLICKED(IDC_DXT1, OnBnClickedDxt1)
	ON_BN_CLICKED(IDC_DXT5, OnBnClickedDxt5)
	ON_BN_CLICKED(IDC_RGB24, OnBnClickedRgb24)
	ON_BN_CLICKED(IDC_ARGB32, OnBnClickedArgb32)
	ON_BN_CLICKED(IDC_ENABLEMIP, OnBnClickedEnablemip)
	ON_BN_CLICKED(IDC_R5G6B5, OnBnClickedR5g6b5)
	ON_BN_CLICKED(IDC_A4R4G4B4, OnBnClickedA4r4g4b4)
	ON_BN_CLICKED(IDC_A8,OnBnClickedA8)
END_MESSAGE_MAP()



INT_PTR CFormatSelectDlg::DoModal( eTextureFormat& Format, BOOL& bMipMap, int& nFrameTime )
{
	// TODO: Add your specialized code here and/or call the base class
	m_Format = Format;
	m_bMipMap = bMipMap;
	m_FrameCircle = nFrameTime;
	INT_PTR hr=CDialog::DoModal();
	Format = m_Format;
	bMipMap = m_bMipMap;
	nFrameTime = m_FrameCircle;
	return hr;
}

void CFormatSelectDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateData();
	CDialog::OnOK();
}
