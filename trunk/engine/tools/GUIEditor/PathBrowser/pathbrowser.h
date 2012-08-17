#pragma once

// CEditorView dialog
#include "afxcmn.h"
#include "PathBrowserRes.h"
#include "CEditorConfig.h"

class CPathBrowser;
class CPathOpen : public ::CButton
{
	DECLARE_DYNAMIC(CPathOpen)

public:
	CPathOpen();
	virtual ~CPathOpen();
	void	 Init(CPathBrowser* pParent);
protected:
	CPathBrowser* m_pParent;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};

// CPathBrowser dialog
class CPathBrowser : public ::CDialog
{
	friend class CPathOpen;
	DECLARE_DYNAMIC(CPathBrowser)

public:
	CPathBrowser(::CWnd* pParent = NULL);   // standard constructor
	virtual ~CPathBrowser();
	bool	Initialize(void);
// Dialog Data
	enum { IDD = IDC_PathBrowser_DIALOGBAR };

protected:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	DECLARE_MESSAGE_MAP()
public:
	::CToolTipCtrl	m_ToolTips;
	::CListBox		m_DirList;
	::CPathOpen		m_SelPathButton;
	::CEdit			m_PathEdit;
	int				m_Index;
	void			SelectDir(void);

	afx_msg void OnBnClickedDown();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedNew();
	afx_msg void OnBnClickedUp();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnLbnDblclkDirList();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedSelDir();
	afx_msg void OnLbnSelchangeDirList();
	afx_msg void OnLbnSetfocusDirList();
	static	bool GetResFileName(wstring& str);
	static  bool GetFirstPath(wstring& str);
	static	bool GetPathByIndex(wstring& str,size_t i);
protected:
	typedef list<wstring> PathList;
	static PathList		st_lstPath;	
	static string		st_szCtrlType;
	//static TiXmlNode*	st_config_xml; 
};
