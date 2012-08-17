#pragma once
#include "afxwin.h"
#include "GUIBase.h"
#include "CRectangle.h"
#include "CTextureFontMgr.h"
#include "GUIEditorHelper.h"
#include "CEmoticonRender.h"
#include "GUIXmlResSaver.h"
#define EMT_IDX_STR "#0000"
#define EMT_IDX_LTH 4

class CTextureFontEditor 
	: public CFormView
	, public GUIXmlResSaverClass
{
	DECLARE_DYNCREATE(CTextureFontEditor)

public:
	CTextureFontEditor();           // protected constructor used by dynamic creation
	virtual ~CTextureFontEditor();

	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, 
		const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

public:
	enum { IDD = IDD_WND_FACE };
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
	// 自行使用区域
	static const int32 EMT_FONT_BEGIN = 0xE000;
	static const int32 EMT_FONT_END = 0xF8FF;
public:
	::CComboBox			m_EmtSel;
	::CComboBox			m_EmtAllSel;

	::CEdit m_Texture;
	::CEdit m_Emtx;
	::CEdit m_Emty;
	::CEdit m_Emtw;
	::CEdit m_Emth;
	::CEdit m_EmtNew;
	

	::CEdit m_IdFrom;
	::CEdit m_IdTo;
	::CEdit m_WAll;
	::CEdit m_HAll;
	::CEdit m_EmtCount;
	wstring	m_emtPath;
	CTextureFontRender m_EmtRender;
public:
	afx_msg void OnBnClickedChooseTex();
	afx_msg void OnBnClickedDelEmt();
	afx_msg void OnBnClickedAddEmt();
	afx_msg void OnBnClickedDelAllEmt();
	afx_msg void OnBnClickedAddAllEmt();
	afx_msg void OnBnClickedSaveALL();
	afx_msg void OnBnClickedAddNew();

	afx_msg void OnCbnSelchangeEmtCur();
	afx_msg void OnCbnSelchangeEmtAll();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	void		 RefreshView(void);
private:
	void	RefreshEmtShow();
	void	RefreshEmtAll();
	void	ReBuiltU32StrToIdx(string& val);

private:
	ITexture*		m_pTex;
	CFRect			m_Rect;
	CTextureFontMgr*	m_EmtMgr;
	string			m_strTex;
	
};

