#pragma once


// CMyEdit
#define WM_DROP_MESSAGE (WM_USER+100)
#define WM_DROP_MESSAGE1 (WM_USER+101)
class CMyEdit : public CEdit
{
	DECLARE_DYNAMIC(CMyEdit)

public:
	CMyEdit();
	virtual ~CMyEdit();
	HWND *m_hwnd;
	void GetMsgNum(int msg);
	int m_msg;

	CString m_Path;
protected:
	afx_msg void OnDropFiles(HDROP hDrop);
	DECLARE_MESSAGE_MAP()
};


