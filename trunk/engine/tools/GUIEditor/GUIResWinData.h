#pragma once
#include "afxcmn.h"
#include "CRendererBase.h"
#include "CTexture.h"
#include "CRectangle.h"

// GUIResWinData dialog
class CGUIResWinDataDlg : 
	 public ::CFormView 
	,public CRendererBase
{
	DECLARE_DYNAMIC(CGUIResWinDataDlg)
	bool m_bRes2Win;
public:
	CGUIResWinDataDlg();   // standard constructor
	virtual ~CGUIResWinDataDlg();

	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, 
		const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	// Dialog Data
	enum { IDD = IDD_DLG_GUIRES };

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void PostNcDestroy(){};
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl	m_ResWinTree;

	wstring		m_strCurImageName;	// 当前贴图的名字
	ITexture*	m_pText;			// 当前贴图，贴图画整个的
	CFRect		m_fRect;			// 区域大小
	bool		m_bStateInRender;
	float		m_fOffX;
	float		m_fOffY;

	uint32		m_Width;
	uint32		m_Height;

	bool		m_bAlphaTest;
	bool		m_bAlphaBlend;

	typedef set<wstring> vec_wstr;
	std::map<wstring, vec_wstr>	m_Win2Res;
	std::map<wstring, vec_wstr>	m_Res2Win;

	afx_msg void	OnTvnSelchangedTreeRes(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnRes2WinChanged();
	afx_msg void	OnBnClickedBtnRefresh();
	afx_msg void	OnShowWindow(BOOL bShow, UINT nStatus);

	wstring			GetSelectImgName() const;
	void			PushResData(const wstring& win, const wstring& res);


	void			Render();
	void			BeginRenderer(void);
	void			EndRenderer(void);
	inline bool		IsStateInRender(void)		{ return m_bStateInRender; }
};
