#pragma once
#include "myedit.h"
#include "afxwin.h"
#include "Texture.h"
#include "D3dd.h"
// CDDSConvertDlg dialog

class CDDSConvertDlg : public CDialog
{
	DECLARE_DYNAMIC(CDDSConvertDlg)

public:
	enum	DDSTRANSFLAG	{ DDSTF_UNCHOOSE = -1, DDSTF_COMPRESS = 0, DDSTF_KEEPORIGIN = 1 };
	typedef vector<CString> CStringVec;

public:
	CDDSConvertDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDDSConvertDlg();

public:
	int		Run();
	void	ConvertAllPicToDDS( CString strPicDir );
	void	ConvertAndCompress( CString strPicDir, CTexture& tex );
	virtual INT_PTR DoModal(CD3d* d3d);

	virtual BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_DDSCONVERT };
	CStringVec			m_vecSupportExt;
	CString				m_strTransFailedDir;
	CD3d*				m_d3d;
	UINT				m_nTransFailedNum;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOldPicBrown();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CMyEdit m_PicPath;
	CStatic m_ddsTransInfo;
	DDSTRANSFLAG m_DDSTransFlag;
	afx_msg void OnBnClickedRadiocompress();
	afx_msg void OnBnClickedRadioKeepOrigin();
};
