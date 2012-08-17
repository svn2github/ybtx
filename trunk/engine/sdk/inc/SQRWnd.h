#pragma once

#include "IGraphic.h"
#include "CRenderFont.h"
#include "IEffectManager.h"
#include "GUIBase.h"
#include "SQRWndClass.h"
#include "CRectangle.h"
#include "CDynamicObject.h"
#include "TimeHelper.h"
#include "CColor.h"
#include "SQRWndDef.h"
#include "CallBackX.h"
#include "CodeCvs.h"
#include "SQRGUITransparentEffect.h"
#include "SQRGUIShapeEffect.h"

namespace sqr
{
	class ITexture;
	class SQRDialog;
	class SQRRootWnd;
	class SQRGUIEffectHandler;

	class GUI_API SQRWnd 
		: public virtual CDynamicObject
		, public SQRGuiMallocObject
	{
	protected:
		WndHandle*				m_hWnd;
		SQRGUIEffectHandler*	m_effHld;		// 效果
		friend		class   SQRDialog;
		DYNAMIC_DECLARE_WND( SQRWnd )
	private:
		void			Relock(CFRect& rect, WndHandle* pWndH, SQRWnd* pChildWnd, const CFRect& rt, float Zoom,bool bTestIsGUITool);

	public:
		WndHandle*		GetWndHandle(void) const;
		static bool		IsHasEventState(EventStateMask Mask);

		virtual void	OnShapeEffectEnded();
	protected:

		//////////////////////////////////////////////////////////////////////////
		//消息函数

		//本窗口处理消息的函数
		virtual LRESULT WndProc( uint32 message, WPARAM wParam, LPARAM lParam );

		//窗口被点住不放
		virtual void	OnHeld( float /*xScreen*/, float /*yScreenCToolTip*/ ) {}
		//左键点住拖拉
		virtual void	OnDrag( const CFPos& HeldPoint, const CFPos& CurPoint )
		{
			OnDrag(HeldPoint.x, HeldPoint.y, CurPoint.x, CurPoint.y);
		}
		virtual void	OnDrag(float iPosX1, float iPosY1, float iPosX2, float iPosY2) {
			CALL_CLASS_CALLBACK_4(iPosX1, iPosY1, iPosX2, iPosY2)
		}
		//活动状态改变
		virtual void    OnActive( bool bActive, SQRWnd* Wnd ) ;
		//窗口创建
		virtual void    OnCreated();
		//窗口创建
		virtual void    OnDestroy();
		//窗口创建
		virtual void    OnResCreated( const char* szResInfo );
		//子窗口创建
		virtual void    OnChildCreated();
		//父窗口移动
		virtual void	OnParentWndMoved( float dx, float dy );
		//窗口移动前
		virtual bool	OnWndPreMoved( float& /*dx*/, float& /*dy*/ )
		{
			return true;
		}
		//窗口移动
		virtual void	OnWndMoved( float dx, float dy );
		//窗口显示
		virtual void	OnWndShow( bool bShow );
		//播放窗口特效
		virtual void	OnPlayWndFx( EMsgFx eFx );
		//播放窗口音效
		virtual void	OnPlayWndSound( ESoundMsg eSound );
		//显示鼠标信息之前，返回延时时间
		virtual uint32	OnPreShowMouseDesc();
		//鼠标Tooltip提示的RichText有效，返回false使用默认背景
		virtual bool	OnMouseDescBegin()
		{
			CALL_CLASS_CALLBACK_RET(bool) 
			return false;
		}
		//鼠标Tooltip提示的RichText有效
		virtual void	OnMouseDescEnd();

		//--------------------标准Windows消息--------------------
		//字符消息
		virtual void	OnChar( uint32 nChar, uint32 nFlags ) ;
		//Ime字符消息
		virtual void	OnImeChar( uint32 nChar, uint32 nFlags );
		//键盘按下消息
		virtual void	OnKeyDown( uint32 nChar, uint32 nFlags );
		//键盘弹起消息
		virtual void	OnKeyUp( uint32 nChar, uint32 nFlags );
		//左键双击消息
		virtual void	OnLButtonDblClk( uint32 nFlags, int32 x, int32 y );
		//左键点击消息
		virtual void	OnLButtonDown( uint32 nFlags, int32 x, int32 y ); 
		//左键释放消息
		virtual void	OnLButtonUp( uint32 nFlags, int32 x, int32 y ); 
		//一次鼠标左键按下抬起操作
		virtual void	OnLButtonClick( uint32 nFlags, int32 x, int32 y ); 
		//按住shift时候左键释放消息
		virtual void	OnShiftLButtonUp( uint32 nFlags, int32 x, int32 y );
		//按住shift时候左键释放消息
		virtual void	OnShiftLButtonDown( uint32 nFlags, int32 x, int32 y );
		//中键双击消息
		virtual void	OnMButtonDblClk( uint32 nFlags, int32 x, int32 y );
		//中键点击消息
		virtual void	OnMButtonDown( uint32 nFlags, int32 x, int32 y );
		//中键弹起消息
		virtual void	OnMButtonUp( uint32 nFlags, int32 x, int32 y );
		//鼠标移动消息
		virtual void	OnMouseMove( uint32 nFlags, int32 x, int32 y );
		//鼠标滚轮消息
		virtual void	OnMouseWheel( uint32 nFlags, short zDelta, int32 x, int32 y );
		//右键双击消息
		virtual void	OnRButtonDblClk( uint32 nFlags, int32 x, int32 y );
		//右键点击消息
		virtual void	OnRButtonDown( uint32 nFlags, int32 x, int32 y );
		//右键释放消息
		virtual void	OnRButtonUp( uint32 nFlags, int32 x, int32 y ); 
		//一次鼠标右键按下抬起操作
		virtual void	OnRButtonClick( uint32 nFlags, int32 x, int32 y );
		//设置光标消息
		virtual bool	OnSetCursor()
		{
			return true;
		}
		//输入焦点激活消息
		virtual void	OnEditActive() {}
		//逻辑处理
		virtual	void	OnProcess();

		friend class SQRGUIManager;

	public:

		SQRWnd(void);
		virtual ~SQRWnd(void);

		void SetTransparentObj(int32 runtime, bool bvalue);
		void EndTransparent( bool bvalue);

		//响应子窗口
		virtual void			OnCtrlmsg( SQRWnd* pChild, uint32 uMsgID, uint32 uParam1, uint32 uParam2 );
		//窗口创建和销毁
		void					RegisterAccelKeyCommand( bool bCtrl, bool bShift, bool bAlt, uint8 VirKey, uint32 wParam, uint32 lParam );
		void					UnRegisterAccelKeyCommand( bool bCtrl, bool bShift, bool bAlt, uint8 VirKey );///////////add by lily
		virtual int32			DestroyWnd();		// 销毁数据
		virtual int32			Create( const WndCreateParam& param );
		int32					ReCreateRes( const char* szChildName );
		GUI_FONT*				GetFontInfo(uint32 idx);
		//屏幕做标位置改变
		virtual void			OnScreenPosChanged();

		//制定旋转多少弧度
		void					SetRotation( FLOAT angle );

		//制定饶那个点旋转
		void					SetRotationOrigin( FLOAT offsetx, FLOAT offsety);
		//////////////////////////////////////////////////////////////////////////
		//功能性函数，可以被调用实现指定功能

		//命令消息
		virtual LRESULT         OnCommand( WPARAM wParam, LPARAM lParam );
		//发送消息到本窗口
		LRESULT					SendMsg( uint32 message, WPARAM wParam, LPARAM lParam );
		//重画所有子窗口
		void					ReDrawChildren();
		//画特效（特效分层处理，感觉不好，使UI变的复杂，且这种效果是可以通过直接制作特效实现的，先加这个接口吧）
		virtual void			DrawImeFx( uint curTime, float zoom );
		virtual void			FlashImeFx( uint curTime, float zoom );

		//画矩形
		virtual void			DrawRect( ITexture* pTexture, const CFRect& rt, uint32 Color, const CFRect* pTextRect = NULL, bool bClip = false );
		virtual void			DrawRect( ITexture* pTexture, const CFRect& rt, uint32 Color, const CPosRect& TextRect );
		virtual void			DrawRect( const CVector2f* rt, uint32 uColor, ITexture* pTexture,const CVector2f* rtText, bool bClip = false );
		virtual void			DrawRect( const CVector2f* rt, uint32 uColor, ITexture* pTexture0, ITexture* pTexture1, const CFRect* rtText0,const CVector2f* rtText1, ERenderStyle rs, float z );
		virtual void			DrawRect( const CVector2f* rt, uint32 uColor, ITexture* pTexture0, ITexture* pTexture1, ITexture* pTexture2,ERenderStyle rs, float z );
		virtual void			DrawRect( WND_IMAGE_LIST& ImageList, const CFRect* rt = NULL );
		virtual void			DrawRect( WND_IMAGE_LIST& ImageList, const CFRect* rt, float offsetX, float offsetY);
		virtual void			DrawRect( const CFRect& rt, uint32 color, bool screenPos = false );
		/// @brief 绘制SQRWnd的边框
		virtual void			DrawRect(CColor color = CColor::Red);

		//画线
		virtual void			DrawLine( const PointGroup* pPointGroup, int32 nNum, bool bScreenPos = false );
		//写字
		virtual void			DrawText( const wstring& text, uint32 textColor, uint32 textBcColor );
		virtual void			DrawText(const wstring& text, float x, float y, CColor color, CColor bkColor, bool multiline, CFRect* rect, bool vertical);
		virtual void			DrawText(const wstring& text, float x, float y, CColor color, CColor bkColor, bool multiline, CFRect* rect);

		//强制画
		void                    FlushRect();
		void					FlushDraw();
		//画接口
		virtual void			Draw();

		//属性函数
		IGraphic*				GetGraphic();
		IEffectManager*			GetEffectManager();
		CRenderFont*			GetFont()							const;
		SQRRootWnd*				GetRootWnd();
		float					GetRootZoomSize();
		SQRWnd*					GetModuleWnd();
		uint32					GetStyle() const;
		virtual void			SetStyle( uint32 Style );
		SQRWnd*					GetParent();
		SQRToolTip*				GetToolTipRichText();
		// rich tooltip
		SQRRichToolTip*			GetRichToolTip();
		SQRRichToolTip*			SetNewRichToolTip();
		void					DelRichToolTip();
		// end of rich tooltip
		float					GetFontSize()const;
		void					SetFontSize(float size);
		bool					IsFontBold();
		int32					GetFontID();
		int32					GetGroupID();
		void					SetGroupID( int32 nGroupID );
		const char*				GetszClassName();
		void					SetszClassName( const char* szDesc );
		const char*				GetszCreateInfo();
		void					SetszCreateInfo( const char* szDesc );
		const char*				GetszWindowName();
		void					SetszWindowName( const char* szDesc );
		const char*				GetMouseOverDesc();
		void					SetMouseOverDesc( const char* szDesc );
		void					SetMouseOverDescAfter( const char* szDesc );
		void					SetMouseOverRichTextImageForAllWnd( int32 nIndex, WND_IMAGE_LIST& ImageList );
		const char*				GetMsgFx( EMsgFx eFxType );
		void					SetMsgFx( EMsgFx eFxType, const char* szDesc );
		const char*				GetMsgSound( ESoundMsg eSoundType );
		void					SetMsgSound( ESoundMsg eSoundType, const UIString& strSoundName );
		uint32					GetLockFlag();
		void					SetLockFlag( uint32 nLockFlag );
		ERenderStyle			GetRenderStyle();
		void					SetRenderStyle( ERenderStyle eRS );
		float					GetDepth();
		virtual void			SetToAutoFormat(bool b){};
		bool					GetBGIsDrawed();
		void					SetBGIsDrawed(bool bd);
		// 播放特效，可以选择特效包的接口
		void					PlayScreenPathFx(const char* fxFile, const char* fxName, EEffectRun eEffrun = eEffRun_Normal);
		void					PlayWndPathFx(const char* fxFile, const char* fxName, EEffectRun eEffrun = eEffRun_Normal);
		void					PlayWndImePathFx(const char* fxFile, const char* fxName, EEffectRun eEffrun = eEffRun_Normal);
		//播放特效的函数
		void					PlayWndFx(const char* fxName);
		void					PlayScreenFx(const char* fxName);
		void					PlayWndFxEx(const char* fxName, EEffectRun eEffrun = eEffRun_Normal);
		void					PlayScreenFxEx( const char* fxName, EEffectRun eEffrun = eEffRun_Normal);
		//删除特效，那写不能自动删除的效果需要手动删除
		void					DelWndFx(const char* fxName);
		void					DelWndFxEx();
		void					DelScreenFx(const char* fxName);
		// 播放即时特效
		void					PlayWndImeFx(const char* fxName, EEffectRun eEffrun);
		void					DelWndImeFx(const char* fxName);

		/// @brief 设置窗口的ZOrder，0最近，1最远
		void					SetDepth( float fDepth );
		const CFRect&			WndToScreen( CFRect& rt );
		const CFRect&			ScreenToWnd( CFRect& rt );
		void					WndToScreen(CFRect& rt, CFRect& out);
		void					ScreenToWnd(CFRect& rt, CFRect& out);
		void					SetWndZoom( float zoom ); // 这个函数不会马上产生变化，需要手动调用ResizeRect
		float					GetWndZoom( void );
		const CFPos&			GetCursorPos();
		void					GetCursorPos(CFPos& pos);
		const CFPos&			GetLogicCursorPos();
		void					GetLogicCursorPos(CFPos& pos);
		uint32					GetCurMessage();
		bool					FindParent( SQRWnd* pParent );
		SQRWnd*					FindNextWnd();
		SQRWnd*					FindPreWnd();
		SQRWnd*					FindFirstChild();
		SQRWnd*					FindLastChild();
		SQRWnd*					InserBefore( SQRWnd* pWnd );
		SQRWnd*					GetTopWnd();
		const char*				GetZOrder();
		SQRWnd*					GetDlgChild( const char* szChildName );
		bool					AssignDlgChildToWnd( SQRWnd& Wnd, const wchar_t* szChildName );
		void					SetWndData( uint32 dwData );
		uint32					GetWndData();
		static void				RegisterAccelerator( IAccelerator* pAccelerator, uint32 uWndMsg );
		static void				UnRegisterAccelerator( IAccelerator* pAccelerator );
		static void				RegisterHook( IHook* pHook, uint32 uWndMsg );
		static void				UnRegisterHook( IHook* pHook );
		void					AddHookMsg( IHook* pHook, uint32 uWndMsg );
		virtual bool			TestHook( IHook* pHook );
		virtual void			PrepareDefaultDesc();
		void					PushTipData();
		void					CopyToClipboard( const char* str );

		virtual void			EnableWnd( bool bEnable );
		virtual void			ShowWnd( bool bShow );
		virtual void			ShowWndIgnTex( bool bShow );	// 忽略材质处理
		virtual void			SetLogicRect( const CFRect& rt );
		virtual void			GetLogicRect( CFRect& rt );
		virtual float			RefreshWndDis( float val );
		virtual void			SetWndRect( const CFRect& rt );
		virtual void			ResizeRect( const CFRect& rt ,bool refreshbak = true);
		virtual void			ResetWndPos(float x, float y);
		virtual void			_ResetWndPos(float x, float y);
		virtual void			OnSized();
		virtual void			ResizeRectOneChild( const CFRect& rt, SQRWnd* pChild );

		void					SetShapeEffect(int32 runtime, CFRect& src, CFRect& des);
		void					DelShapeEffect( SQRWnd* Wnd );
		void					SetInShape();

		void					GetWndRect( CFRect& rt ); //得到全屏坐标
		void					GetWndRectBak( CFRect& rt );	// 获取形变窗体实际大小
		virtual void			_ResizeRect( const CFRect& rt ,bool refreshbak = true);
		virtual void			_SetWndRect( const CFRect& rt );
		virtual	float			GetWndLeft();
		virtual float			GetWndTop();
		float					GetWndRight();
		float					GetWndBottom();
		float					GetWndWidth()						const;
		float					GetWndHeight()						const;
		float					GetWndOrgWidth();
		float					GetWndOrgHeight();
		void					SetWndWidth(float width);
		void					SetWndHeight(float height);
		void					SetActive();
		void					SetFocus();
		void					SetTransparent( float fTrans ); //设置窗体透明度
		float					GetTransparent();
		bool					SetFont( int32 nFontID, bool bBold, float nSize );
		SQRWnd*					SetParent( SQRWnd* pParent );
		void					AddChildWnd( SQRWnd *pWnd );		//为当前窗口加入一个子窗口
		virtual	CFRect			GetParentCutRect();//得到根裁切窗口
		virtual void			GetCutAreaRect( CFRect& rt );

		virtual void			OnCreatedAfter(){};
		virtual void			ReleaseWndTex();		// 节省资源模式下，窗口关闭使用
		virtual void			CreatWndTex();			// 节省资源模式下，窗口打开使用
		bool					IsModleState();

		const char*				GetWndName();
		virtual const char*		GetWndText();
		const wchar_t*			GetWndNameW();
		virtual const wchar_t*	GetWndTextW();
		virtual const bool		GetIsBottom();
		const bool				GetHaveEditActive();
		virtual void			SetWndText( const char* str );
		/// @brief 窗口字符串追加
		/// @remark 比采用SQRWnd::SetWndText((SQRWnd::GetWndText() + str).c_str());有很大的速度优势
		virtual const char*		AppendWndText(const string& str);
		virtual void			SetIsBottom(bool bIsBottom);

		void					SetWndBkImage( const IMAGE_PARAM& ImageFlag, const WND_IMAGE_LIST& Image );
		virtual WND_IMAGE_LIST*	GetWndBkImage( const IMAGE_PARAM& ImageFlag );

		//----------------------------------------------------------------------------
		virtual void			SetWndTextColor( const IMAGE_PARAM& ImageFlag, uint32 uColor );
		virtual uint32			GetWndTextColor( const IMAGE_PARAM& ImageFlag );

		virtual void			SetWndTextBcColor( const IMAGE_PARAM& ImageFlag, uint32 uColor );
		virtual uint32			GetWndTextBcColor( const IMAGE_PARAM& ImageFlag );

		virtual void			SetFontGradualColor( uint color );
		virtual void			SetFontEffectMask( uint mask );

		virtual uint			GetFontGradualColor(void);
		virtual uint			GetFontEffectMask(void);

		virtual uint32			GetStyleIndex();
		virtual void			SetStyleIndex( uint32 index );

		virtual const char*		GetszSourceName();
		virtual void			SetszSourceName( const char* szDesc );
		//----------------------------------------------------------------------------

		virtual SQRWnd*			GetResWnd( int32 nCtrlMask );
		// 光标函数
		void					SetWndCursor( const char* szCursorFile );
		const char*				GetWndCursor();
		void					SetGlobCursor( const char* szCursorFile );
		const char*				GetGlobCursor();
		void					UpdateGlobCursor(SQRWnd &Wnd);			//更新光标

		void                    SetDetectMouseOverWnd(bool bFlag);
		bool                    IsDetectMouseOverWnd();

		bool					IsChild( SQRWnd* pWnd );
		bool					IsHeld();					// 窗口是否被点住不放
		bool					IsActive();
		bool					IsFocus();
		bool					IsEnable();
		bool					IsEnableTest();
		bool					IsShow();;
		bool					IsCreated();
		bool					IsSysCreated();
		bool					IsLastMouseMsgWnd();
		bool					IsDownTooltips();
		void					SetDownTooltips( bool bIsDown );
		void					RefreshToolTip();
		SQRWnd*					GetLastMouseMsgWnd();
		virtual bool			IsInWndArea( float x, float y  );
		virtual bool			IsInTitleArea( float x, float y );
		virtual bool			IsInFrameArea( float x, float y );
		bool					GetMouseWheel();
		void					SetMouseWheel(bool bGetMouseWheel);
		void					ClearImeFx();
		void					SetTipFontSize(float size);
		float					GetTipFontSize();
		
		//////////////////////////////////////////////////////////////////////////
		//被系统调用来画窗口的函数，通常需要被重载

		//画背景
		virtual void			DrawWndBackground();
		//画区域
		virtual void			DrawWndArea();

		//画窗口文本
		virtual void			DrawWndText();
		//在子窗口被画完后调用
		virtual void			DrawAfterChildren();
		//显示鼠标信息
		virtual void			DrawMouseOver();

		//
		virtual void			DrawAfterAllEyeCandy();

		bool					IsDrawBackground();
		void					SetIsDrawBackground(bool bShaow);
		void					SetAlignToPixel(bool bAlignToPixel);

		void					DestroyFlashAtt();
		virtual SQRDialog*		GetFlushDialog();
		virtual bool			IsFlushDialog();

		void					AddFlashInfoShow();		// 添加到显示队列
		void					AddFlashInfoShow(FlashWndInfo* info, uint32 fs);		// 添加到显示队列
		void					AddFlashAttentionTex( char* flashTex, const CFRect& rt, SQRWnd* pChildWnd, uint32 ct, float v, 
												   float lx, float ty ,float rx, float by);
		void					AddFlashInfoByName(const char* info);
		void					AddFlashInfoByNameImp(const char* info);
		void					AddFlashInfoByNameEx(const char* info, bool cut);
		void					DelFlashAttention();
		void					DelFlashAttentionSpe( SQRWnd* pChildWnd, const char* sTex );

		void					SetFlashInfo(EventStateMask fs, const char* name);
		const char*				GetFlashInfo(EventStateMask fs);
		void					DelFlashInfo(EventStateMask fs);
		UIMap<EventStateMask, FlashWndInfo*>&	GetFlashInfoMap();

		virtual void			SetEventStateMask(EventStateMask es);
		EventStateMask			GetEventStateMask();
		virtual bool			ObjHasEventState(EventStateMask Mask);
		
		void					SetCutWnd(SQRWnd* wnd);
		SQRWnd*					GetCutWnd();

		bool					GetCutFlash();
		void					SetCutFlash(bool cut);
		void					RejustRect(CFRect& rt);

		void					SetWndDebugInfo(const char* info);
		void					GetWndDebugInfo(string& str);

		UIVector<SQRDialog*>	m_pDlgVec;			// 窗体的父Dlg,flash相关
		SQRDialog*				m_FlushDlg;			// flush相关的DLG
		bool					m_bInShape;
		bool					m_bCutFlash;

		bool					bIsBottom; //是否是最底层窗口,ture则会显示在其他窗口之下
		bool					bDetectMouseOverWnd; //是否检测鼠标所在位置的窗口
		bool					bNotModule;

		float					m_WndZoom;
		float					m_itX;			//文字的偏移
		float					m_itY;			//文字的偏移
		FLOAT					offset[7];		//四个顶点的偏移量 0 偏移量  1 偏移量 ...   4 移动的是哪个点    5
		UIString				debugWndInfo;	//调试使用

		bool					m_bAlignToPixel;
		typedef UIVector<UIString> EffectVec;
		EffectVec				m_Effect;		// GUI特效
		bool					m_beBGDrawed;	// 背景该帧是否画了

		SQRWnd*					m_pCutWnd;		//可以设定被某个窗体裁切
	private:
		EventStateMask			m_ESMask;
		bool					m_bIgnTex;
		
	};

	inline bool
		SQRWnd::GetCutFlash()
	{
		return m_bCutFlash;
	}

	inline void
		SQRWnd::SetCutFlash(bool cut)
	{
		m_bCutFlash = cut;
	}

	//------------------------------------------------------------------------------
	inline void
		SQRWnd::SetDetectMouseOverWnd( bool bFlag )
	{
		bDetectMouseOverWnd = bFlag;    //设定是否检测鼠标所在位置的窗口
	}

	//------------------------------------------------------------------------------
	inline bool
		SQRWnd::IsDetectMouseOverWnd()
	{
		return bDetectMouseOverWnd;
	}
	//------------------------------------------------------------------------------
	inline bool
		SQRWnd::IsShow()
	{
		return !GetParent()|| (m_hWnd->m_bShow && GetParent()->IsShow());
	}

	inline bool
		SQRWnd::GetBGIsDrawed()
	{
		return m_beBGDrawed;
	}

	inline void
		SQRWnd::SetBGIsDrawed(bool bd)
	{
		m_beBGDrawed = bd;
	}


	//------------------------------------------------------------------------------
	inline bool
		SQRWnd::IsDrawBackground()
	{
		return m_hWnd->m_bIsShowBackground;
	}

	//------------------------------------------------------------------------------
	inline void
		SQRWnd::SetIsDrawBackground( bool bShaow )
	{
		m_hWnd->m_bIsShowBackground = bShaow;
	}

	//------------------------------------------------------------------------------
	inline SQRWnd*
		SQRWnd::FindFirstChild()
	{
		return m_hWnd->m_FirstChild;
	}

	//------------------------------------------------------------------------------
	inline WndHandle*
		SQRWnd::GetWndHandle( void ) const
	{
		return m_hWnd;
	}

	//------------------------------------------------------------------------------
	inline uint32 SQRWnd::GetStyle() const
	{
		return m_hWnd->m_WndStyle;
	}

	inline SQRWnd* SQRWnd::GetResWnd( int32 nCtrlMask )
	{
		return nCtrlMask == SM_BS_BK ? this : NULL;
	}

	inline float SQRWnd::GetFontSize() const
	{
		return m_hWnd->m_WndFont ? m_hWnd->m_FontSize : 0.0f;
	}

	inline void SQRWnd::SetFontSize(float size)
	{
		WndHandle* wnd = GetWndHandle();
		this->SetFont(wnd->m_WndFont->GetFontID(), wnd->m_WndFont->IsBold(), size);
	}
	
	inline void SQRWnd::SetTipFontSize(float size)
	{
		m_hWnd->m_TipFontSize = size;
	}

	inline float SQRWnd::GetTipFontSize()
	{
		return m_hWnd->m_TipFontSize; 
	}
	inline CRenderFont* SQRWnd::GetFont() const
	{
		return m_hWnd->m_WndFont;
	}

	inline bool SQRWnd::IsHeld()
	{
		return m_hWnd->m_bIsHeld ;
	}

	inline bool SQRWnd::IsDownTooltips()
	{
		return m_hWnd->m_bIsDownTooltips;
	}

	inline void SQRWnd::SetDownTooltips( bool bIsDown )
	{
		m_hWnd->m_bIsDownTooltips = bIsDown;	
	}
	
	inline bool SQRWnd::IsActive()
	{
		return m_hWnd->m_IsActive && IsEnable();
	}

	inline bool SQRWnd::IsCreated()
	{
		return ( m_hWnd->m_pGUI != NULL );
	}

	inline bool SQRWnd::IsSysCreated()
	{
		return IsCreated() && m_hWnd->m_bNewByGUISys;
	}

	inline bool SQRWnd::GetMouseWheel()
	{
		return m_hWnd->m_bGetMouseWheel;
	}

	inline void SQRWnd::SetMouseWheel(bool bGetMouseWheel)
	{
		m_hWnd->m_bGetMouseWheel = bGetMouseWheel;
	}

	inline SQRWnd* SQRWnd::GetParent()
	{
		return m_hWnd->m_ParentWnd;
	}

	inline SQRToolTip*
		SQRWnd::GetToolTipRichText()
	{
		return m_hWnd->m_DescRichText; 
	}

	inline SQRRichToolTip*
		SQRWnd::GetRichToolTip()
	{
		return m_hWnd->m_pRichToolTip;
	}

	inline bool SQRWnd::IsEnable()
	{
		if ( GetParent() && GetParent()->IsEnable() == false )
			return false;
		return m_hWnd->m_bEnable && IsShow();
	}

	//专门测试自己是否为Enable,而不需要考虑父窗口的
	inline bool SQRWnd::IsEnableTest()
	{
		return m_hWnd->m_bEnable;
	}

	inline void  SQRWnd::SetWndData( uint32 dwData )
	{
		m_hWnd->m_dwOtherData = dwData;
	}

	inline uint32 SQRWnd::GetWndData()
	{
		return m_hWnd->m_dwOtherData;
	}

	inline SQRWnd* SQRWnd::FindNextWnd()
	{
		return m_hWnd->m_NextBrother;
	}

	inline SQRWnd* SQRWnd::FindPreWnd()
	{
		return m_hWnd->m_PreBrother;
	}

	inline SQRWnd* SQRWnd::FindLastChild()
	{
		return m_hWnd->m_LastChild;
	}

	inline const char* SQRWnd::GetMsgFx( EMsgFx eFxType )
	{
		return m_hWnd->m_MsgFx[eFxType].c_str();
	}

	inline void SQRWnd::SetMsgFx( EMsgFx eFxType, const char* szDesc )
	{
		m_hWnd->m_MsgFx[eFxType] = szDesc ? szDesc : "";
	}

	inline const char* SQRWnd::GetMsgSound( ESoundMsg eSoundType )
	{
		return m_hWnd->m_MsgSound[eSoundType].c_str();
	}

	inline void SQRWnd::SetMsgSound( ESoundMsg eSoundType, const UIString& strSoundName )
	{
		m_hWnd->m_MsgSound[eSoundType] = strSoundName;
	}

	inline uint32 SQRWnd::GetLockFlag()
	{
		return m_hWnd->m_uLockFlage;
	}

	inline void SQRWnd::SetLockFlag( uint32 nLockFlag )
	{
		m_hWnd->m_uLockFlage = nLockFlag;
	}

	inline ERenderStyle SQRWnd::GetRenderStyle()
	{
		return m_hWnd->m_eRS;
	}

	inline void SQRWnd::SetRenderStyle( ERenderStyle eRS )
	{
		m_hWnd->m_eRS = eRS;
	}

	inline float SQRWnd::GetDepth()
	{
		return m_hWnd->m_fZ;
	}

	inline void SQRWnd::SetDepth( float fDepth )
	{
		m_hWnd->m_fZ = fDepth;
	}

	inline int32 SQRWnd::GetGroupID()
	{
		return m_hWnd->m_GroupID;
	}

	inline void SQRWnd::SetGroupID( int32 nGroupID )
	{
		m_hWnd->m_GroupID = nGroupID;
	}

	inline void SQRWnd::GetCursorPos(CFPos& pos)
	{
		pos = GetCursorPos();
	}

	inline void SQRWnd::GetLogicCursorPos(CFPos& pos)
	{
		pos = GetLogicCursorPos();
	}

	inline void SQRWnd::WndToScreen(CFRect& rt, CFRect& out)
	{
		out = WndToScreen(rt);
	}

	inline void SQRWnd::ScreenToWnd(CFRect& rt, CFRect& out)
	{
		out = ScreenToWnd(rt);
	}

	inline bool SQRWnd::IsFontBold()
	{
		return m_hWnd->m_WndFont ? m_hWnd->m_WndFont->IsBold() : false;
	}

	inline float SQRWnd::GetWndRight()
	{
		return GetWndLeft() + m_hWnd->m_Width;
	}

	inline float SQRWnd::GetWndBottom()
	{
		return GetWndTop() + m_hWnd->m_Height;
	}

	inline float SQRWnd::GetWndWidth() const
	{
		return m_hWnd->m_Width;
	}

	inline float SQRWnd::GetWndHeight() const
	{
		return m_hWnd->m_Height;
	}

	inline float SQRWnd::GetWndOrgWidth()
	{
		return m_hWnd->m_WidthInitial;
	}

	inline float SQRWnd::GetWndOrgHeight()
	{
		return m_hWnd->m_HeightInitial;
	}

	inline void SQRWnd::SetWndWidth(float width)
	{
		m_hWnd->m_Width = width;
	}

	inline void SQRWnd::SetWndHeight(float height)
	{
		m_hWnd->m_Height = height;
	}

	inline float SQRWnd::GetTransparent()
	{
		return m_hWnd->m_fTransparent;
	}

	inline const char* SQRWnd::GetWndName()
	{
		return m_hWnd->m_WindowName.c_str();
	}

	inline const wchar_t* SQRWnd::GetWndNameW()
	{
		return m_hWnd->m_WindowNameW.c_str();
	}

	inline const wchar_t* SQRWnd::GetWndTextW()
	{
		return m_hWnd->m_WindowTitleW.c_str();
	}

	inline const bool SQRWnd::GetIsBottom()
	{
		return this->bIsBottom;
	}

	inline void SQRWnd::EnableWnd( bool bEnable )
	{
		m_hWnd->m_bEnable = bEnable;
	}

	inline void SQRWnd::SetRotation( FLOAT angle )
	{
		offset[5] = angle;
	}

	//------------------------------------------------------------------------------
	inline void
		SQRWnd::SetFontGradualColor( uint color )
	{
		m_hWnd->m_FontGradualColor = color;
	}

	//------------------------------------------------------------------------------
	inline void
		SQRWnd::SetFontEffectMask( uint mask )
	{
		m_hWnd->m_FontEffectMask = mask;
	}

	//------------------------------------------------------------------------------
	inline uint
		SQRWnd::GetFontGradualColor( void )
	{
		return m_hWnd->m_FontGradualColor;	
	}

	//------------------------------------------------------------------------------
	inline uint
		SQRWnd::GetFontEffectMask( void )
	{
		return m_hWnd->m_FontEffectMask;
	}

	//------------------------------------------------------------------------------
	inline EventStateMask			
		SQRWnd::GetEventStateMask()
	{
		return m_ESMask;
	}

	//------------------------------------------------------------------------------
	inline void		
		SQRWnd::SetCutWnd(SQRWnd* wnd)
	{
		m_pCutWnd = wnd;
	}

	//------------------------------------------------------------------------------
	inline SQRWnd*			
		SQRWnd::GetCutWnd()
	{
		return m_pCutWnd;
	}

}// namespace sqr
