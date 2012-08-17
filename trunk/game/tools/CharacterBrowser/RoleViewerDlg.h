// RoleViewerDlg.h : header file
//
#pragma once
#include "afxcmn.h"
#include "CBrowserView.h"
#include "PathBrowser\PathBrowser.h"
#include "MiniScene\CMiniScene.h"
#include "ViewTabCtrl\CViewTabCtrl.h"
#include "NpcSelectView.h"
#include "AvatarView.h"
#include "CMatrix.h"
#include "VideoSetting\VideoSettingDlg.h"
#include "CPkgFile.h"


class CDataCache;
// CRoleViewerDlg dialog
class CRoleViewerDlg : public ::CDialog
{
// Construction
friend class CDataCache;
public:
	CRoleViewerDlg(CWnd* pParent = NULL);	// standard constructor
	~CRoleViewerDlg();
	void OnDraw();

// Dialog Data
	enum { IDD = IDD_ROLEVIEWER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

public:
	CBrowserView		m_Show;
	CNpcSelectView		m_NpcView;
	CAvatarView			m_AvaView;
	CViewTabCtrl		m_EditorTools;
	CVideoSettingDlg    m_VideoSetting;
	class CLoaclPathBrowser : public CPathBrowser
	{
	public:
		void SetResPkgFindPath()
		{
			char* aliasArray[] = {"cfg","gui","res","lan","shd"};
			int num = sizeof(aliasArray)/sizeof(char*);

			for (PathList::iterator iter = st_lstPath.begin();iter != st_lstPath.end();++iter)
			{
				for (int i=0;i<num;++i)
				{
					string path = *iter;
					string pathLan = *iter + "/lang";
					string pathRes = *iter + "/res";
					string strTemp = aliasArray[i];
					wstring wstrTemp = gbk_to_utf16(strTemp);
					CPkgFile::AddLoadPath(wstrTemp.c_str(), gbk_to_utf16(path).c_str());
					CPkgFile::AddLoadPath(wstrTemp.c_str(), gbk_to_utf16(pathLan).c_str());
					CPkgFile::AddLoadPath(wstrTemp.c_str(), gbk_to_utf16(pathRes).c_str());
				}
			}
		}
	};
	CLoaclPathBrowser		m_PahtBrowser;
	LONG				m_lSprit;

	sqr::CMatrix		m_matSize;
	sqr::CMatrix		m_matWorld;

// Implementation
protected:
	HICON m_hIcon;
	TCHAR rootPath[1024];

	// Generated message map functions
	virtual BOOL OnInitDialog();
	//virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnClose();
	afx_msg void OnSelfdefine();
	afx_msg void OnOnepointfive();
	afx_msg void OnTwo();
	afx_msg void OnSceneSetting();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	afx_msg void OnPathSetting();
	afx_msg void OnVideoSetting();
	afx_msg void OnHideaperture();
};
