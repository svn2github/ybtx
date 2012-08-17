#pragma once
#include "CColor4.h"
#include "CGraphicMallocObject.h"

#define NM_CHILDETOPARENT		0x780107
#define NM_CHILDSTATECHANGED	0x780108


class CCtrlWnd;
struct NotifyMessage
{
	NMHDR hrd;
	CCtrlWnd* pWnd;
	UINT  Message;
	WPARAM wParam;
	LPARAM lParam;
};


class CRadioGroup
{
public:
	CRadioGroup(){};
	CRadioGroup( int First, int Last, int Check )
	{ m_FirstRadio = First;  m_LastRadio = Last;  m_CheckeRadio = Check; };

	int m_FirstRadio;
	int m_LastRadio;
	int m_CheckeRadio;
};


class CRadios
{
	vector<CRadioGroup> m_Container;
public:
	void CheckRadio( CWnd* pWnd, int First, int Last, int Check );
	void CheckRadio( CWnd* pWnd, int Check );
};

class CCtrlWnd : public ::CStatic
{
	BOOL m_IsNeedMsg;
public:
	CCtrlWnd( BOOL IsNeedMsg = FALSE ):m_IsNeedMsg(IsNeedMsg){};

protected:
	virtual LRESULT SendCtrlNotifyMsg( DWORD Notify, UINT message, WPARAM wParam, LPARAM lParam );
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};


/**
* 用于绘制一个颜色随着客户区X轴连续变化的控件
*/
class CColorBlock : public CCtrlWnd
{
	DECLARE_DYNAMIC(CColorBlock)

	enum CtrlType
	{
		CT_FLOAT,
		CT_UINT32,
		CT_COLOR,
		CT_COLOR4,
	};
	CFont				m_Font;

	GMap<float,CColor>	m_Color;           //颜色变化的列表，float:X轴百分比，DWORD颜色
	CDC					m_MemDC;
	CBitmap				m_Bitmap;
	DWORD*				m_pBit;
	float				m_CurColorPercent;

	BOOL				m_ColorValid;
	void*				m_EditColor;
	CtrlType			m_eType;

protected:
	DECLARE_MESSAGE_MAP()
public:
	CColorBlock():CCtrlWnd( TRUE ),m_pBit(0),m_CurColorPercent(0),
		m_ColorValid(0),m_EditColor(NULL),m_eType(CT_COLOR)
	{
		m_Font.CreateFont( 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
	}

	void  ResetPic();
	void  SetColorList( GMap<float,CColor>& Color );
	void  GetColorList( GMap<float,CColor>& Color );
	void  SetColorList( GMap<float,CColor4>& Color );
	void  GetColorList( GMap<float,CColor4>& Color );
	void  SetColorList( GMap<float,float>& Color );
	void  GetColorList( GMap<float,float>& Color );
	void  SetColorList( GMap<float,uint32>& Color );
	void  GetColorList( GMap<float,uint32>& Color );


	CColor GetColor( float Percent );
	DWORD GetAlpha( float Percent );
	DWORD GetGDIColor( float Percent );
	void  SetCurPos( CPoint& point );
	void  OnDraw( CDC* pDC );
	void  UpdateColorList(GMap<float,CColor>& Color);	
	void  Update();
	void  ClearEditColor();

protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	virtual void PreSubclassWindow();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};


class CCompBlock;
class CBlockCombo : public CComboBox
{
	DECLARE_DYNAMIC(CBlockCombo)
	CCompBlock* m_BlockEdit;
public:
	CBlockCombo():m_BlockEdit(0){}
	void SetCompBlock( CCompBlock* Block ){ m_BlockEdit = Block; }
	void DeleteAll(){ while( DeleteString( 0 ) >=0 ); };
	void SetCurSelByDataValue( DWORD Data );

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelendok();
protected:
	virtual void PreSubclassWindow();
};

class CCompBlock : public CCtrlWnd
{
	DECLARE_DYNAMIC(CCompBlock)

	CFont				m_Font;

	UINT				m_Range;
	GMap<float,uint32>	m_State;
	float				m_CurPercent;
	HCURSOR				m_BlockCursor;
	BOOL				m_SetBlockCur;
	float				m_PreMoveBlock;

	CBlockCombo*		m_BlockOption;
	GMap<float,uint32>*	m_EditState;

public:
	CCompBlock():CCtrlWnd( TRUE ),m_CurPercent(0),m_SetBlockCur(0),m_PreMoveBlock(-1),
		m_BlockOption(0),m_EditState(0){}
	void  OnDraw( CDC* pDC );
	void  SetStateList( GMap<float,uint32>& State, CBlockCombo* StateOption );
	void  SetRange( UINT Range );
	UINT  GetRange();
	void  SetCurState( DWORD State );
	void  Update();

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();

	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};


class CMsgWnd : public CWnd
{
	DECLARE_DYNAMIC(CMsgWnd)

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
};


class CModelFileDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CModelFileDialog)
	CString			m_FloderName;

public:
	CModelFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);
	virtual INT_PTR DoModal( CString& RootPath );

protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnFolderChange();
};


class CAutoDlg : public ::CDialog
{
public:
	CAutoDlg(UINT nID,CWnd* pParent):CDialog(nID, pParent){}
	virtual void  AddFile( CString sFile ){};
	static void FindFile( TCHAR* Path, CAutoDlg* pDlg );
};


class CColorSimpleBlock : public CCtrlWnd
{
	DECLARE_DYNAMIC(CColorSimpleBlock)

	map<float,CColor4>	m_Color;           //颜色变化的列表，float:X轴百分比，DWORD颜色
	CDC					m_MemDC;
	CBitmap				m_Bitmap;
	DWORD*				m_pBit;
	float				m_CurColorPercent;

	BOOL				m_ColorValid;
	map<float,CColor4>*	m_EditColor;

protected:
	DECLARE_MESSAGE_MAP()
public:
	CColorSimpleBlock()
		:CCtrlWnd( TRUE )
		,m_pBit(0)
		,m_CurColorPercent(0)
		,m_ColorValid(0)
		,m_EditColor(NULL){}

	void  ResetPic();
	void  SetColorList( map<float,CColor4>& Color );
	void  GetColorList( map<float,CColor4>& Color );
	CColor4 GetColor( float Percent );
	DWORD GetAlpha( float Percent );
	CColor4 GetGDIColor( float Percent );
	void  SetCurPos( CPoint& point );
	void  OnDraw( CDC* pDC );
	void  Update(){	if( m_EditColor )GetColorList( *m_EditColor ); };

protected:
	afx_msg void OnPaint();
	virtual void PreSubclassWindow();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};

