#pragma once
#include "SQRScrollBar.h"
#include "CodeCvs.h"
#include "CRectangle.h"

#define CTRL_A		( 'A' - 'A' + 1 )
#define CTRL_C		( 'C' - 'A' + 1 )
#define CTRL_V		( 'V' - 'A' + 1 )
#define CTRL_X		( 'X' - 'A' + 1 )


namespace sqr
{
	class GUI_API SQREdit : public SQRControl
	{
		DYNAMIC_DECLARE_WND( SQREdit )

	public:
		// ----------------------------------------------------------------------------------------
		// Accessor functions
		// ----------------------------------------------------------------------------------------
		void			SetSelection(size_t start_pos, size_t end_pos);

		/**
		\brief
			取得滚动条
		*/
		SQRScrollBar*		GetHorzScrollbar();
		SQRScrollBar*		GetVertScrollbar();

		/**
		\brief
		设置光标位置
		*/
		void			setCaratIndex(size_t carat_index);						

		// ----------------------------------------------------------------------------------------
		// Overridden functions
		// ----------------------------------------------------------------------------------------
		virtual int32   Create( const WndCreateParam& param );
		virtual int32	DestroyWnd();
		virtual WND_IMAGE_LIST* GetWndBkImage( const IMAGE_PARAM& ImageFlag );
		virtual void	SetIsSendMsgToParent( bool IsSendMsgToParent );
		virtual void	SetWndText( const char* str );
		virtual void	SetWndRect( const CFRect& rt );
		virtual void	ResizeRect( const CFRect& rt  ,bool refreshbak = true);
		virtual void	_ResizeRect( const CFRect& rt ,bool refreshbak = true);
		virtual void	_SetWndRect( const CFRect& rt );
	public:
		virtual void	EnableWnd( bool bEnable );
		virtual void	ShowWnd( bool bShow );
		virtual void	SetMaxTextLenght( int32 nLen );
		virtual int32	GetMaxTextLenght();
		virtual void	InsertStr( const char* str );
		virtual void	DisableIme( bool bDisable );
		virtual int32	GetLineCount( int32* pMaxLineLength );
		virtual int32	GetLineCount() { return GetLineCount(NULL); }
		virtual const char*		GetWndText();

		static bool		IsHasEventState(EventStateMask Mask);
		virtual bool	ObjHasEventState(EventStateMask Mask);
		virtual SQRWnd*	GetResWnd( int32 nCtrlMask );
		void			SetIsOnCommand(bool bFlag);
		bool			GetIsOnCommand();
		virtual void	GetCutAreaRect( CFRect& rt );
		void			SetSendType(bool bType);
		bool			GetSendType();

		// ----------------------------------------------------------------------------------------
		// Construction and Destruction
		// ----------------------------------------------------------------------------------------
		SQREdit();
		virtual		   ~SQREdit();

		// ----------------------------------------------------------------------------------------
		// Implementation struct
		// ----------------------------------------------------------------------------------------
		/**
		\brief
			struct used to store information about a formatted line within the
			control text.
		*/
		struct LineInfo
		{
			size_t  startIdx;     // Starting index for this line.
			size_t  length;       // Code point length of this line.
			float   extent;       // Rendered extent of this line.
		};
		typedef UIVector<LineInfo>   LineList;   // Type for collection of LineInfos.

	protected:
		void LoseFocusByShowFalse();

	protected:
		// ----------------------------------------------------------------------------------------
		// Overridden event handlers
		// ----------------------------------------------------------------------------------------
		virtual void	OnChar( uint32 nChar, uint32 nFlags );
		virtual void	OnImeChar( uint32 nChar, uint32 nFlags );
		virtual void	OnKeyDown( uint32 nChar, uint32 nFlags );
		virtual void	OnLButtonDblClk( uint32 nFlags, int32 x, int32 y );
		virtual void	OnLButtonDown( uint32 nFlags, int32 x, int32 y );
		virtual void	OnMouseWheel( uint32 nFlags, short zDelta, int32 x, int32 y );
		virtual void	OnDrag( const CFPos& HeldPoint, const CFPos& CurPoint );
		virtual void    OnActive( bool bActive, SQRWnd* Wnd );
		virtual void	DrawWndText();

		// ----------------------------------------------------------------------------------------
		// Implementation methods
		// ----------------------------------------------------------------------------------------
		void			CtrlC();
		void			CtrlV();
		void			SetCurShow();
		UIWString&		GetEditString();
		const UIWString& GetEditString()const;
		bool			GetIsSendMsgToParent();
		void			OnCtrlmsg( SQRWnd* pChild, uint32 uMsgID, uint32 uParam1, uint32 uParam2 );

		/**
		\brief
			更新光标时间，用于实现光标闪烁
		*/
		void			UpdateShowTime();

		/**
		\brief
			根据字体大小计算缩放值	
		*/ 
		float			GetFontZoomSize();

		size_t			getNextTokenLength(const wstring &text, size_t start_idx) const;
		float			getTextPixelLen(const wstring &text, size_t start_idx, size_t len);
		float			getTextPixelLen(const wstring &text);
		void			formatText();
		
		void			clearSelection();

		/**
		\brief
			用字符索引计算字符所在的行
		*/
		size_t			getLineNumberFromIndex(size_t index)	const;

		/**
		\brief
			计算坐标所在位置的字符的索引(窗口坐标系下)
		*/
		size_t			getTextIndexFromPosition(CFPos pt)		const;

		/**
		\brief
			获取字符渲染区域(窗口坐标系下)
		*/
		CFRect			getTextDrawArea()						const;

		/**
		\brief
			取得文字选取相关信息(选取起始位置、结束位置和宽度)
		*/
		size_t			getSelectionStartIndex()				const;
		size_t			getSelectionEndIndex()					const;
		size_t			getSelectionLength()					const; 

		/**
		\brief
			删除被选择的文字
		*/
		void			eraseSelectedText();

		/**
		\brief
			取得光标位置
		*/
		size_t			getCaratIndex()							const;

		/**
		\brief
			确保光标移动后可见
		*/
		void			ensureCaratIsVisible();

		/**
		\brief
			绘制文字或光标
		*/
		void			drawTextLines(const CFRect &dest_area);
		void			drawCarat(const CFRect &textArea);

		/**
		\brief
			当文本被改变后需要调用此函数，此函数确保文本以'\n'结束，并且进行文本
			格式化，调整滚动条确保光标可见
		*/
		void			onTextChanged();

		/**
		\brief
			取得格式化好的文本行
		*/
		const LineList& getFormattedLines(void)				const;

		/**
		\brief
			处理滚动条
		*/
		void			configureScrollbars();

		/**
		\brief
			按键处理函数
		*/
		void			handleNewLine();
		void			handleBackspace();
		void			handleDelete();
		void			handleCharLeft();
		void			handleCharRight();
		void			handleWordLeft();
		void			handleWordRight();
		void			handleLineUp();
		void			handleLineDown();
		void			handleLineHome();
		void			handleLineEnd();
		void			handleDocHome();		
		void			handleDocEnd();

		// ----------------------------------------------------------------------------------------
		// Implementation data
		// ----------------------------------------------------------------------------------------
		SQRScrollBar		m_sbHorizon;		//水平滚动条
		SQRScrollBar		m_sbVertical;		//竖直滚动条

		uint32			m_uMaxTextLen;
		uint64			m_ShowTime;			// 用于实现光标闪烁的
		bool			m_IsSendMsgToParent;

		LineList		m_lines;			// 保存文本各行信息
		size_t			m_caratIndex;		// 光标在字符串中的位置
		size_t			m_selectStart;		// 选择区域起始位置
		size_t			m_selectEnd;		// 选择区域结束位置
		size_t			m_dragAnchorIdx;	// 鼠标拖拽锚点
		float			m_widestExtent;		// 最宽行宽度
		UIWString		m_szPassWord;

		const static UIWString	m_lineBreakChars;
		const static float		m_fCaratWidth;		// 光标宽度
		
	private:
		bool			m_bOnCommand;		//是否运行了组合快捷键
		bool			m_bEnterSend;		//是否使用Enter键发送(默认是Ctrl+Enter)
	};

	// ----------------------------------------------------------------------------------------
	// Inline methods
	// ----------------------------------------------------------------------------------------
	inline SQRScrollBar* SQREdit::GetHorzScrollbar()
	{
		return &m_sbHorizon;
	}

	inline SQRScrollBar* SQREdit::GetVertScrollbar()
	{
		return &m_sbVertical;
	}

	inline void SQREdit::SetMaxTextLenght( int32 nLen )
	{
		m_uMaxTextLen = uint32( nLen );
	}

	inline int32 SQREdit::GetMaxTextLenght()
	{
		return int32( m_uMaxTextLen );
	}

	inline bool SQREdit::GetIsSendMsgToParent()
	{
		return m_IsSendMsgToParent;
	}

	inline void SQREdit::UpdateShowTime()
	{
		m_ShowTime = GetProcessTime();
	}

	inline UIWString& SQREdit::GetEditString()
	{
		return m_hWnd->m_WindowTitleW;
	}

	inline const UIWString& SQREdit::GetEditString()const
	{
		return m_hWnd->m_WindowTitleW;
	}

	inline void SQREdit::SetIsSendMsgToParent( bool IsSendMsgToParent )
	{
		m_IsSendMsgToParent = IsSendMsgToParent;
	}

	inline size_t SQREdit::getSelectionLength() const
	{
		return m_selectEnd - m_selectStart;
	}

	inline size_t SQREdit::getCaratIndex() const 
	{ 
		return m_caratIndex; 
	}

	inline const SQREdit::LineList& SQREdit::getFormattedLines(void) const   
	{
		return m_lines;
	}

	inline void SQREdit::SetIsOnCommand(bool bFlag)
	{
		m_bOnCommand = bFlag;
	}

	inline bool SQREdit::GetIsOnCommand()
	{
		return m_bOnCommand;
	}

	inline void SQREdit::SetSendType(bool bType)
	{
		m_bEnterSend = bType;
	}

	inline bool SQREdit::GetSendType()
	{
		return m_bEnterSend;
	}

}
