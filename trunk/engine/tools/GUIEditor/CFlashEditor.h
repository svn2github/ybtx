#pragma once
#include "afxwin.h"
#include "GUIBase.h"
#include "CRectangle.h"
#include "FlashDataManager.h"
#include "GUIEditorHelper.h"
#include "CFlashRender.h"
#include "GUIXmlResSaver.h"

class CFlashEditor 
	: public CFormView
	, public GUIXmlResSaverClass
{
	DECLARE_DYNCREATE(CFlashEditor)

public:
	CFlashEditor();           // protected constructor used by dynamic creation
	virtual ~CFlashEditor();

	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, 
		const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

public:
	enum { IDD = IDD_FLASH_EDIT };
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
	::CComboBox			m_FlashSel;

	::CEdit m_Texture;
	::CEdit m_FlashName;

	::CEdit m_Flashx;
	::CEdit m_Flashy;
	::CEdit m_Flashw;
	::CEdit m_Flashh;

	::CEdit m_FlashLx;
	::CEdit m_FlashRx;
	::CEdit m_FlashTy;
	::CEdit m_FlashBy;

	::CEdit m_AMax;
	::CEdit m_AMin;
	::CEdit m_VTime;
	::CEdit m_Count;

	wstring	m_FlashPath;
	CFlashRender m_FlashRender;
public:
	afx_msg void OnBnClickedChooseTex();
	afx_msg void OnBnClickedAddFlash();
	afx_msg void OnBnClickedDelFlash();
	afx_msg void OnBnClickedEditOk();
	afx_msg void OnBnClickedSaveALL();

	afx_msg void OnCbnSelchangeFlashCur();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	void		 RefreshView(void);
private:
	void	RefreshFlashShow();
	void	RefreshFlashAll();

private:
	string				m_strTex;
	ITexture*			m_pTex;
	CFRect				m_Rect;
	CFRect				m_RePose;
	FlashDataManager*	m_FlashMgr;
};