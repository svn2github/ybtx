#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "CBasePropList.h"
#include "CWndImageList.h"
#include "CWndImageProp.h"
#include "CMsgEffectList.h"

class CWnd;

// CWndProp form view

class CWndProp : public CFormView
{
	CWndGroup*			m_pWndGroup;
	SQRWnd*			m_pCurEditWnd;

	DECLARE_DYNCREATE(CWndProp)

public:
	enum { IDD = IDD_WND_PROP };

	CBasePropList		m_PropList;
	::CComboBox			m_ImageListType;
	CWndImageList		m_ImageList;
	CWndImageProp		m_ImageProp;

	::CEdit				m_TextColor1;
	::CEdit				m_TextColor2;
	::CEdit             m_TextBcColor;
	::CEdit				m_TextAlphaEdit;

	DWORD				m_FontEffectMask;

	CWndProp();           // protected constructor used by dynamic creation
	virtual ~CWndProp();
	void Init( CWndGroup* pWndGroup );
	void Update( uint32 nFlag );
	void MoveText( UINT uMessage, WPARAM wParam, LPARAM lParam );
	void DrawEditLine( CGraphic* pGraphic, int n );
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, 
		const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

	DECLARE_MESSAGE_MAP()

protected:
	void OnSelectColor(int id);
	virtual void PostNcDestroy(){};
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support    
public:
	afx_msg void OnCbnSelendokWndBktype();
	afx_msg void OnBnClickedFliph();
	afx_msg void OnBnClickedFlipv();
	afx_msg void OnEnKillfocusEditTextcolor1();
	afx_msg void OnEnChangeEditTextcolor1();
	afx_msg void OnEnChangeAeditTextcolor2();
	afx_msg void OnEnKillfocusAeditTextcolor2();
	afx_msg void OnEnChangeEditTextbccolor();
	afx_msg void OnEnKillfocusEditTextbccolor();
	afx_msg void OnBnClickedIsShowBack();
	::CButton m_isShowBack;
	afx_msg void OnBnClickedBtnClr1();
	afx_msg void OnBnClickedBtnClr2();
	afx_msg void OnBnClickedBtnBkclr();
	afx_msg void OnBnClickedCheckItalic();
	afx_msg void OnBnClickedCheckShadow();
	afx_msg void OnBnClickedCheckOutline();
	afx_msg void OnBnClickedCheckGradual();
	afx_msg void OnBnClickedChooseTexture();
	afx_msg void OnBnClickedChooseAudio();
	::CComboBox m_AudioTypeComBox;
	::CComboBox m_FlashStateComBox;
	::CComboBox m_FlashNameComBox;
	afx_msg void OnCbnSelchangeAudioType();
	afx_msg void OnCbnSelchangeFlashState();
	afx_msg void OnCbnSelchangeFlashName();
	CString m_strAudioCueName;
	int		m_nAudioType;
	afx_msg void OnBnClickedClearAudio();
};
