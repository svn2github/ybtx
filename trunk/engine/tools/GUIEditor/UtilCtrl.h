#pragma once
//--------------------------------------------------------------------------------
// 文件：UtilCtrl.h
// 功能：小控件
// 说明：因为在程序中经常要用到某些自定义的小控件，所以把他们集中到一起，形成一个集合
//------------------------------------------------------------------------------------

#define NM_CHILDETOPARENT		0x780107
#define NM_CHILDSTATECHANGED	0x780108

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
	inline void CheckRadio( CWnd* pWnd, int First, int Last, int Check )
	{
		m_Container.push_back( CRadioGroup( First, Last, Check ) );
		pWnd->CheckRadioButton( First, Last, Check );
	}

	inline void CheckRadio( CWnd* pWnd, int Check )
	{
		for( int i = 0; i< (int)m_Container.size(); i++ )
		{
			if( Check >= m_Container[i].m_FirstRadio && Check <= m_Container[i].m_LastRadio )
				m_Container[i].m_CheckeRadio = Check;
			pWnd->CheckRadioButton( m_Container[i].m_FirstRadio, m_Container[i].m_LastRadio, m_Container[i].m_CheckeRadio );
		}
	}
};

class CCtrlWnd : public CStatic
{
	BOOL m_IsNeedMsg;
public:
	CCtrlWnd( BOOL IsNeedMsg = FALSE ):m_IsNeedMsg(IsNeedMsg){};

protected:
	virtual LRESULT SendCtrlNotifyMsg( DWORD Notify, UINT message, WPARAM wParam, LPARAM lParam );
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};

struct NotifyMessage
{
	NMHDR hrd;
	CCtrlWnd* pWnd;
	UINT  Message;
	WPARAM wParam;
	LPARAM lParam;
};


// 播放控件
// 用于绘制播放的帧格
enum PLAY_STATE
{
	enumPS_PLAY,
	enumPS_STOP,
};

class CFrameCtrl : public CCtrlWnd
{
	DECLARE_DYNAMIC(CFrameCtrl)
	int m_FrameStart;               //起始帧
	int m_FrameEnd;					//结束帧
	float m_FrameCur;				//当前帧
	int m_FramesPerGrid;			//每格的帧数
	int m_PixPerGrid;				//每格的象素
	CFont m_Font;
	map< int, int > m_KeyFrame;

	BOOL m_IsInDrag;
	BOOL m_BeginDrag;

	PLAY_STATE m_State;

public:
	CFrameCtrl();
	void  CalFramePerGrid();												//计算每格帧数
	void  SetRange( int FrameEnd, int FrameStart = 0 );						//设置帧的范围
	float SetFrameNum( float Frame );										//设置当前帧
	float GetFrameNum()							{ return m_FrameCur; }		//得到当前帧
	void  SetPlayState( PLAY_STATE PlayState )	{ m_State = PlayState; };	//设置播放状态*
	void  ClearKeyFrame()						{ m_KeyFrame.clear(); }
	void  SetKeyFrame( int Num )				{ m_KeyFrame[Num] = Num; }

protected:
	virtual void PreSubclassWindow();
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnDraw( CDC* pDC );
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
};


// 颜色块控件
// 用于绘制一个颜色随着客户区X轴连续变化的控件
class CColorBlock : public CCtrlWnd
{
	DECLARE_DYNAMIC(CColorBlock)

	CFont				m_Font;

	map<float,DWORD>	m_Color;           //颜色变化的列表，float:X轴百分比，DWORD颜色
	CDC					m_MemDC;
	CBitmap				m_Bitmap;
	DWORD*				m_pBit;
	float				m_CurColorPercent;

	BOOL				m_ColorValid;
	map<float,DWORD>*	m_EditColor;

protected:
	DECLARE_MESSAGE_MAP()
public:
	CColorBlock():CCtrlWnd( TRUE ),m_pBit(0),m_CurColorPercent(0),
		m_ColorValid(0),m_EditColor(0)
	{
		m_Font.CreateFont( 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
	}

	void  ResetPic();
	void  SetColorList( map<float,DWORD>& Color );
	void  GetColorList( map<float,DWORD>& Color );
	DWORD GetColor( float Percent );
	DWORD GetAlpha( float Percent );
	DWORD GetGDIColor( float Percent );
	void  SetCurPos( CPoint& point );
	void  OnDraw( CDC* pDC );
	void  Update(){	if( m_EditColor )GetColorList( *m_EditColor ); };

protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	virtual void PreSubclassWindow();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};


// CBlockCombo
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


// CCompBlock
class CCompBlock : public CCtrlWnd
{
	DECLARE_DYNAMIC(CCompBlock)

	CFont				m_Font;

	UINT				m_Range;
	map<float,DWORD>	m_State;
	float				m_CurPercent;
	HCURSOR				m_BlockCursor;
	BOOL				m_SetBlockCur;
	float				m_PreMoveBlock;

	CBlockCombo*		m_BlockOption;
	map<float,DWORD>*	m_EditState;

public:
	CCompBlock():CCtrlWnd( TRUE ),m_CurPercent(0),m_SetBlockCur(0),m_PreMoveBlock(-1),
		m_BlockOption(0),m_EditState(0){}
	void  OnDraw( CDC* pDC );
	void  SetStateList( map<float,DWORD>& State, CBlockCombo* StateOption );
	void  SetRange( UINT Range ){ m_Range = Range; Invalidate(); }
	UINT  GetRange(){ return m_Range; }
	void  SetCurState( DWORD State ){ m_State[m_CurPercent] = State; }
	void  Update(){ if( m_EditState )*m_EditState = m_State; };

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


// CMsgWnd

class CMsgWnd : public CWnd
{
	DECLARE_DYNAMIC(CMsgWnd)

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
};

// CFileDialog

class CFileDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CFileDialog)
	CString			m_FloderName;

public:
	CFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
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


