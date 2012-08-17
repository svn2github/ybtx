#pragma once
#include "afxwin.h"
#include "GUIBase.h"
#include "CRectangle.h"
#include "WndStyleRender.h"
#include "GUIXmlResSaver.h"
// CWndStyleEditor form view

class CWndStyleEditor 
	: public CFormView
	, public GUIXmlResSaverClass
{
	DECLARE_DYNCREATE(CWndStyleEditor)

public:
	CWndStyleEditor();           // protected constructor used by dynamic creation
	virtual ~CWndStyleEditor();

	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, 
		const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
public:
	enum { IDD = IDD_WND_DEFAULT };
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
	::CComboBox			m_StylesList;
	::CComboBox			m_ListType;
	::CComboBox			m_NineBlock;
	::CComboBox			m_FontName;

	StyleStruct*	m_pStyleStruct;
	WndStyleRender	m_StyleRender;
	GUI_BLOCK*		m_pBlock;
	BLOCK_POOL*		m_pBlockPool;
	CFRect			m_Rect;
	string			m_strWnd;
	::CEdit			m_TextureEdit;
	EventStateMask	m_Mask;
	UINT			m_BlockIndex;

	::CEdit			m_rectLeft;
	::CEdit			m_rectTop;
	::CEdit			m_rectRight;
	::CEdit			m_rectBottom;
	
	::CEdit			m_FontSize;
	::CEdit			m_FontColor;
	::CEdit			m_FontColor2;
	::CEdit			m_FontBKC;

	DWORD			m_FontEffectMask;
public:
	void	RefreshList(void);
	void	RefreshView(void);
	void	RefreshMaskList(void);
	void	RefreshBlockPoolIndex(void);
	
	afx_msg void OnBnClickedWndMove();
	afx_msg void OnBnClickedClock();
	afx_msg void OnBnClickedButton();
	afx_msg void OnBnClickedCheckbutton();
	afx_msg void OnBnClickedCombobox();
	afx_msg void OnBnClickedEdit();
	afx_msg void OnBnClickedRichwnd();
	afx_msg void OnBnClickedListctrl();
	afx_msg void OnBnClickedProgress();
	afx_msg void OnBnClickedScrollbar();
	afx_msg void OnBnClickedSpin();
	afx_msg void OnBnClickedStatic();
	afx_msg void OnBnClickedTreectrl();
	afx_msg void OnBnClickedChart();
	afx_msg void OnBnClickedCWnd();

	afx_msg void OnBnClickedStyleOk();
	afx_msg void OnBnClickedStyleAdd();
	afx_msg void OnBnClickedStyleDel();
	afx_msg void OnBnClickedStyleSave();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedStyleSok();
	afx_msg void OnBnClickedSelectPic();
	afx_msg void OnCbnSelchangeWndBktype2();

	afx_msg void OnBnClickedReplaceAll();
	afx_msg void OnBnClickedCheckAlphablend();
	afx_msg void OnBnClickedCheckAlphatest();

	afx_msg void OnBnClickedFontOk();
	afx_msg void OnBnClickedBtnClr1();
	afx_msg void OnBnClickedBtnClr2();
	afx_msg void OnBnClickedBtnBkclr();

public:
	void StyleProc(UINT uMessage, WPARAM wParam, LPARAM lParam); //先放这里吧

protected:
	float		m_MouseButtonX;
	float		m_MouseButtonY;
	bool		m_bIsButtonDown;

	void ProcEditRect(UINT uMessage, WPARAM wParam, LPARAM lParam);
	void ProcEditOffSet(UINT uMessage, WPARAM wParam, LPARAM lParam);
	void OnSelectColor(int id);
public:
	afx_msg void OnEnChangeEditTop();
	afx_msg void OnEnChangeEditLeft();
	afx_msg void OnEnChangeEditRight();
	afx_msg void OnEnChangeEditBottom();

	afx_msg void OnCbnSelchangeWndNineblock();
	afx_msg void OnBnClickedBlockLeftTop();
	afx_msg void OnBnClickedBlockTop();
	afx_msg void OnBnClickedBlockRightTop();
	afx_msg void OnBnClickedBlockLeft();
	afx_msg void OnBnClickedBlockCenter();
	afx_msg void OnBnClickedBlockRight();
	afx_msg void OnBnClickedBlockLeftBottom();
	afx_msg void OnBnClickedBlockBottom();
	afx_msg void OnBnClickedBlockRightBottom();
	afx_msg void OnBnClickedAutotool();
	::CEdit m_TopHeight;
	::CEdit m_BottomHeight;
	::CEdit m_LeftWidth;
	::CEdit m_RightWidth;
	::CComboBox m_BlockPoolIndex;
	afx_msg void OnNMThemeChangedBlockIndex(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedAddBlock();
	afx_msg void OnCbnSelchangeBlockIndex();
	afx_msg void OnBnClickedAicon();
};


