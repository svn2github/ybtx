#pragma once


// CNewMapDialog 对话框

class CNewMapDialog : public CDialog
{
	DECLARE_DYNAMIC(CNewMapDialog)

public:
	CNewMapDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNewMapDialog();
	
// 对话框数据
	enum { IDD = IDD_NEWMAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	DWORD m_dwWidth;
	DWORD m_dwDepth;
	afx_msg void OnBnClickedOk2();
};
