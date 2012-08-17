// GUIEditorDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "SQRRootWnd.h"

#include "SQRButton.h"
#include "SQRCheckButton.h"
#include "SQRComboBox.h"
#include "SQREdit.h"
#include "SQRRichText.h"
#include "SQRProgress.h"
#include "SQRStatic.h"
#include "SQRScrollBar.h"
#include "SQRSpin.h"
#include "SQRWnd.h"
#include "SQRClock.h"
#include "SQRWndOffSet.h"
#include "SQRTreeCtrl.h"
#include "SQRListCtrl.h"
#include "SQRChart.h"
#include "SQRIcon.h"
#include "SQRVideo.h"

#include "SQRFlashWnd.h"
#include "SQRWebBrowser.h"

#include "CRenderFont.h"
#include "CWndGroup.h"
#include "CWndProp.h"
#include "WndStyle.h"
#include "WndStyleEditor.h"
#include "WndStyleRender.h"
#include "CEmoticonEditor.h"
#include "CFlashEditor.h"
#include "CTexFontEditor.h"
#include "GUIResWinData.h"
#include "GUIPicSaver.h"

#include "CVector2.h"
#include "CRenderFont.h"
#include "PathBrowser\PathBrowser.h"
#define NM_CHILDETOPARENT		0x780107
#define NM_CHILDSTATECHANGED	0x780108

namespace sqr
{
	class CGuiFxPlayer;
	class IAudioSystem;
	class IEffectManager;
}


#pragma region Helper
struct NotifyMessage
{
	NMHDR hrd;
	::CWnd* pWnd;
	UINT  Message;
	WPARAM wParam;
	LPARAM lParam;
};


//////////////////////////////////////////////////////////////////////////
// 显示控件
class CShow : public ::CStatic
{
protected:

	LRESULT SendCtrlNotifyMsg( DWORD Notify, UINT uMessage, WPARAM wParam, LPARAM lParam );

	virtual LRESULT WindowProc(UINT uMessage, WPARAM wParam, LPARAM lParam);
};

//////////////////////////////////////////////////////////////////////////
// 面板控件
class CEditPan : public CTabCtrl
{
	CFont m_Font;
	// Construction
public:
	//加入面板
	UINT AddToList( CView* pView, int Image, LPWSTR ItemName, int id );

	//显示面板
	void ShowTab( int Num );

public:
	virtual BOOL Create( DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID );

	DECLARE_MESSAGE_MAP()
protected:
	afx_msg void OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnSize( UINT nType, int cx, int cy );
};

#pragma endregion Helper

//////////////////////////////////////////////////////////////////////////
// CGUIEditorDlg 对话框
class CGUIEditorDlg : public ::CDialog , public SQRDrawHandler
{
	// 构造
public:
	CGUIEditorDlg(::CWnd* pParent = NULL);	// 标准构造函数
	~CGUIEditorDlg();

	BOOL			Create( UINT ID, ::CWnd* pParent ){ return CDialog::Create( ID,  pParent ); }
	virtual void	DrawAfterChildren();
	void			EditWndRect( CPoint p );
	void			UpdateRect(void);

	// 对话框数据
	static unsigned GetDialogID();
	CRect			RootWndRect();
	CEditPan		m_EditTab;

	void			SetGUIPath(const wstring& GUIPath);
	void			SetFxPath(const wstring& path);
	void			SetEmtPath(const wstring& path);
	void			SetFlashPath(const wstring& path);
	void			SetTexFontPath( const wstring& path );
	void			SetPicResPath( const wstring& path );
	void			SetTexPath();

	LRESULT			RecieveMsg( uint32 message, WPARAM wParam, LPARAM lParam );
	IGraphic*		GetGraphic();
	SQRWnd*	GetGUIRootWnd() const { return m_pGUIRootWnd; }

	void			SetN( float n ){ m_n = limit2( n, 0.2f, 4.0f ); if( fabs( m_n - 1 ) < 0.05 )m_n = 1;};
	float			GetN(){ return m_n;	}

	void			SetLanguageID(int nLanID);

	void*	operator new(size_t nSize);
	void*	operator new(size_t nSize, void* p);
	void	operator delete(void* p);
	void	operator delete(void* p, void* pPlace);

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	void			DataExchage();

private:
	wstring				m_strGUIPath;
	wstring				m_fxPath;
	wstring				m_emtPath;
	wstring				m_flashPath;
	wstring				m_PicResPath;

	CShow				m_Show;
	CPoint				m_HitPos;
	CPoint				m_CurPos;
	BOOL				m_bHit;
	CString				m_strFile;
	CSliderCtrl			m_zoomSlider;
	CSliderCtrl			m_winSlider;
	::CStatic			m_zoom;
	::CStatic			m_win;
	

	CGraphic*			m_pGraphic;
	ITexture*           m_pBkTex;
	IAudioSystem*		m_pAudioSystem;
	CGuiFxPlayer*		m_pFxPlayer;
	IEffectManager*		m_pEffectManger;

	WORD				m_SelTextureType;
	BOOL				m_bSizeFollow;
	CWndGroup			m_WndGroup;
	CWndProp			m_WndProp;
	CWndStyleEditor		m_WndStyleEditor;
	CTextureFontEditor	m_EmtEditor;
	CFlashEditor		m_FlashEditor;
	CTexFontEditor		m_TexFontEditor;
	CWndStyle			m_WndStyle;
	CGUIResWinDataDlg	m_GUIResDlg;

	SQRRootWnd*			m_pGUIRootWnd;
	float				m_n;
	int					m_nCurLan;

	CPathBrowser		m_PathBrowser;
	GUIPicSaver			m_PicSaver;
	// 实现
protected:
	HICON m_hIcon;

	DECLARE_MESSAGE_MAP()

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	virtual void OnOK(){};
	virtual void OnCancel();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg HCURSOR OnQueryDragIcon()		{ return m_hIcon; }

	void createAudioSystem();

public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedRadioPos();		
	afx_msg void OnBnClickedRadioTex();	
	afx_msg void OnBnClickedRadioPreview();
	afx_msg void OnBnClickedShowAlpha();
	afx_msg void OnBnClickedRedo();
	afx_msg void OnBnClickedUndo();
	afx_msg void OnBnClickedSetWorkpath();
	afx_msg void OnBnClickedBtnBktex();
	afx_msg void OnBnClickedSetBkColor();
	afx_msg void OnSliderZoomChange(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSliderWinChange(NMHDR *pNMHDR, LRESULT *pResult);

	void StartPreView();
	void EndPreView();
	afx_msg void OnCbnSelchangeLanguageList();
	void	PushResData(const wstring& win, const wstring& res);
};

inline void* CGUIEditorDlg::operator new(size_t nSize)
{
	return ::CDialog::operator new(nSize);
}

inline void* CGUIEditorDlg::operator new(size_t nSize, void* p)
{
	return ::CDialog::operator new(nSize, p);
}

inline void	 CGUIEditorDlg::operator delete(void* p)
{
	::CDialog::operator delete(p);
}

inline void	 CGUIEditorDlg::operator delete(void* p, void* pPlace)
{
	::CDialog::operator delete(p,pPlace);
}
