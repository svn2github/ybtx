#pragma once
#include "SQRScrollBar.h"
#include "SQRWndEx.h"


namespace sqr
{
	class SQRRichText;


	class GUI_API SQRRichString 
		: public virtual CDynamicObject
		, public SQRGuiMallocObject
	{
		HANDLE	m_hRichText;

	public:
		SQRRichString();
		SQRRichString(SQRRichText *pCRichText);
		virtual ~SQRRichString(void);

		void	Free();
		void	InsertImage( int32 nIndex, WND_IMAGE_LIST& ImageAni );//#000~#999
		float	GetCurHeight();
		float   GetMaxWidth();
		void	SetMaxWidth( float fMaxWidth );
		void	SetWidth( float nWidth );
		int32	SetText( SQRWnd* pWnd, const wchar_t *str );
		int32	AddText( SQRWnd* pWnd, const wchar_t *str );
		void	Draw( SQRWnd* pWnd, float x, float y );
		int32	Click( float x, float y , uint32 sty, float wndwidth,float wndheight, uint32 uMsgID = RICHWND_MOUSEOVER);
		void	SetBeAutoFormat(bool b);
		void    SetWndTextXR(bool b);
		void	ReleaseWndTex();
		void	CreateWndTex();

		const wchar_t*	GetChooseStr()
		{
			return m_ChooseStr.c_str();
		}
		const wchar_t*	GetChooseStrAttr()
		{
			return m_sChooseStrAttr.c_str();
		}
		void SetRowDist(float rowDist);
		void OnHighLightBegin(int32 x, int32 y);
		void OnHighLightEnd(int32 x, int32 y);
		const wchar_t* GetHighLightText()const;
	private:
		UIWString		 m_ChooseStr;
		UIWString		 m_sChooseStrAttr;
	};

	/// @brief 辅助显示的结构,用来显示高亮文本
	struct SQRRichWndItem
	{
		SQRWndEx  mWnd;
		UIString  str;
	};

	class GUI_API SQRRichText : public SQRControl
	{
		DYNAMIC_DECLARE_WND( SQRRichText )
	public:
		// ----------------------------------------------------------------------------------------
		// Accessor functions
		// ----------------------------------------------------------------------------------------
		void			DrawText( const wstring& text, float x, float y, CColor color, CColor bkColor, 
								  bool multiline, CFRect* rect, float curLineLenght);
		void			Updata();
		const			char* GetChooseStr();
		const			char* GetChooseStrAttr();
		SQRWnd*			GetNameWnd(char *name);
		void			SetShowScrollBar(bool bvalue);
		void			SetRowDist(float rowDist);
		void			SetAutoWidth (float fMinWidth, float fMaxWidth);
		float			GetMinWidth ();
		float			GetMaxWidth ();
		void			SetMinHeight(float fMinHeight);
		float			GetMinHeight();
		void			SetPageUp();
		void			SetPageDown();
		void			SetPageBottom();
		SQRRichString*	GetRichStringInfo();
		void			SetToAutoFormat(bool b);
		void			SetWndTextXR( bool b);

		bool			Isbottom()							const;
		bool			CheckDis(float wb, float hb, float w, float h);
		
		static bool		IsHasEventState(EventStateMask Mask);
		virtual bool	ObjHasEventState(EventStateMask Mask);

		
		// ----------------------------------------------------------------------------------------
		// Overridden functions
		// ----------------------------------------------------------------------------------------
		virtual void	DrawWndText();
		virtual WND_IMAGE_LIST* GetWndBkImage( const IMAGE_PARAM& ImageFlag );
		virtual uint32	GetWndTextColor( const IMAGE_PARAM& ImageFlag );
		virtual void	SetWndTextColor( const IMAGE_PARAM& ImageFlag, uint32 uColor );
		virtual uint32	GetWndTextBcColor( const IMAGE_PARAM& ImageFlag );
		virtual void	SetWndTextBcColor( const IMAGE_PARAM& ImageFlag, uint32 uColor );
		virtual SQRWnd*	GetResWnd( int32 nCtrlMask );
		virtual void	OnMouseWheel( uint32 nFlags, short zDelta, int32 x, int32 y );
		virtual void	OnDrag( const CPos& HeldPoint, const CPos& CurPoint );
		virtual void	OnLButtonClick( uint32 nFlags, int32 x, int32 y );
		//virtual void OnLButtonUp( uint32 nFlags, int32 x, int32 y );
		virtual void	OnLButtonDown( uint32 nFlags, int32 x, int32 y );
		virtual void	OnRButtonUp( uint32 nFlags, int32 x, int32 y );
		virtual void	OnRButtonDown( uint32 nFlags, int32 x, int32 y );
		virtual void	OnRButtonClick(uint32 nFlags,int32 x, int32 y);
		virtual void	OnMouseMove( uint32 nFlags, int32 x, int32 y );
		virtual void	OnCtrlmsg( SQRWnd* pChild, uint32 uMsgID, uint32 uParam1, uint32 uParam2 );
		//virtual void  DrawText( const UIString& text, int32 x, int32 y, CColor color, CColor bkColor, bool multiline, RECT* rect);
		virtual int32	Create( const WndCreateParam& param );
		virtual void	InsertImage( int32 nIndex, WND_IMAGE_LIST& ImageAni );//#000~#999
		virtual void	SetWndText( const char* str );
		virtual void	AddWndText( const char* str );
		virtual void	ResizeRect( const CFRect& rt  ,bool refreshbak = true);
		virtual void	SetWndRect( const CFRect& rt );
		virtual void	_ResizeRect( const CFRect& rt ,bool refreshbak = true);
		virtual void	_SetWndRect( const CFRect& rt );
	public:
		virtual void	AddNewLineText( const char* str);
		virtual void	SetStyle( uint32 Style );
		
		
		// ----------------------------------------------------------------------------------------
		// Construction and destruction
		// ----------------------------------------------------------------------------------------
		SQRRichText(void);
		~SQRRichText(void);

	protected:
		// ----------------------------------------------------------------------------------------
		// Implementation methods
		// ----------------------------------------------------------------------------------------
		void			CtrlC();
		CFRect			getTextDrawArea();

		virtual void	OnChar( uint32 nChar, uint32 nFlags );

		// ----------------------------------------------------------------------------------------
		// Implementation data
		// ----------------------------------------------------------------------------------------
		SQRRichString			   *m_pRichString;
		SQRScrollBar				m_sbVertical;		///< 竖直滚动条
		UIVector<SQRRichWndItem*>	mClrWnd;			///< 带有颜色的字体
		float					m_fMinWidth;
		float					m_fMaxWidth;
		float					m_fInitMinWidth;
		float					m_fInitMaxWidth;
		float					m_fLastCurHeight;
		float					m_fMinHeight;

		// 供返回时转换使用
		UIString					m_szChooseStr;
		UIString					m_szChooseStrAttr;
		
	private:
		// ----------------------------------------------------------------------------------------
		// Implementation methods
		// ----------------------------------------------------------------------------------------
		void AdjustAfterContentChanged();
	};
}
