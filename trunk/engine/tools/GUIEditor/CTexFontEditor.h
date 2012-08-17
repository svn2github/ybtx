#pragma once
#include "afxwin.h"
#include "GUIBase.h"
#include "CRectangle.h"
#include "CTextureFontMgr.h"
#include "GUIEditorHelper.h"
#include "CTexFontRender.h"
#include "GUIXmlResSaver.h"

class CTexFontEditor 
	: public CFormView
	, public GUIXmlResSaverClass
{
	DECLARE_DYNCREATE(CTexFontEditor)

public:
	CTexFontEditor();           // protected constructor used by dynamic creation
	virtual ~CTexFontEditor();

	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, 
		const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

public:
	enum { IDD = IDD_WND_TEXFONT };
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

protected:
	float		m_MouseButtonX;
	float		m_MouseButtonY;
	bool		m_bIsButtonDown;

public:
	::CComboBox			m_TexKeySel;	// 添加

	::CComboBox			m_TexKeySelCur;	// 当前编辑添加
	::CComboBox			m_TexFontSel;	// 当前字

	::CEdit m_Texture;
	::CEdit m_FontName;

	::CEdit m_Fontx;
	::CEdit m_Fonty;
	::CEdit m_Fontw;
	::CEdit m_Fonth;

	wstring	m_TexFontPath;
	CTexFontRender m_FontRender;
public:
	void ProcEditRect(UINT uMessage, WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedChooseTex();
	afx_msg void OnBnClickedAddFont();
	afx_msg void OnBnClickedDelFont();
	afx_msg void OnBnClickedEditOk();
	afx_msg void OnBnClickedSaveALL();

	afx_msg void OnCbnSelchangeFontCur();
	afx_msg void OnCbnSelchangeFontKeyCur();

	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	void		 RefreshView(void);
private:
	void	RefreshFontShow();
	void	RefreshFontAll();

private:
	string				m_strTex;
	ITexture*			m_pTex;
	CFRect				m_Rect;
	CTextureFontMgr*	m_FontMgr;
};