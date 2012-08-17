#pragma once

#include "GraphicBase.h"
// CFormatSelectDlg dialog

class CFormatSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(CFormatSelectDlg)

	eTextureFormat m_Format;
	BOOL		  m_bMipMap;

public:
	CFormatSelectDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFormatSelectDlg();

// Dialog Data
	enum { IDD = IDD_SAVEFORMAT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedDxt1()		{ m_Format = TF_DXT1; };
	afx_msg void OnBnClickedDxt5()		{ m_Format = TF_DXT5; };
	afx_msg void OnBnClickedRgb24()		{ m_Format = TF_RGB24; };
	afx_msg void OnBnClickedArgb32()	{ m_Format = TF_ARGB32; };
	afx_msg void OnBnClickedR5g6b5()	{ m_Format = TF_R5G6B5; };
	afx_msg void OnBnClickedA4r4g4b4()	{ m_Format = TF_A4R4G4B4; };
	afx_msg void OnBnClickedA8()		{ m_Format = TF_A8; };
	afx_msg void OnBnClickedEnablemip()	{ m_bMipMap = !m_bMipMap; };

	virtual INT_PTR DoModal(eTextureFormat& Format, BOOL& bMipMap, int& nFrameTime );
	int m_FrameCircle;
protected:
	virtual void OnOK();
public:
};
