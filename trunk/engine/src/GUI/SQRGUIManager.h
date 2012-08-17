#pragma once

#include "IGUIManager.h"
#include "SQRToolTip.h"
#include "GUIStuffParser.h"
#include "GUILayoutHandler.h"

#define		ACCEKEY_CTRL		0x0100
#define		ACCEKEY_SHIFT		0x0200
#define		ACCEKEY_ALT			0x0400
#define     ACCEKEY_TOPLEVEL	0x0600

namespace sqr
{
	extern const wstring GUI_PATH_ALIAS_GUITEX;
	extern const wstring GUI_PATH_ALIAS_GUI;
	extern const wstring GUI_PATH_ALIAS_LAN;

	typedef UIMap< uint32, UIVector<UIWString> >     MsgResMap;

	class SQRGUIManager : public IGUIManager
	{
	public:
		typedef UIMap<const UIString, ResInfo> MapResStaticInfo;
		struct TextBuf
		{
			RenderTextParam Pa;
			CRenderFont*	Rf;
		};

	private:
		struct DtMsg
		{
			DtMsg() : pWnd(NULL), wParam(0), lParam(0) {}
			SQRWnd*		pWnd;
			uint32		wParam;
			uint32		lParam;
		};
		
		typedef UIVector<TextBuf> TextBufVec;
		typedef UIMap<IHook*,UIVector<uint32> > HookMap;
		typedef UIMap<IAccelerator*,UIVector<uint32> > AcceleratorMap;
		typedef UIMap<UIString, WndResParam*> ResMap;
		typedef UIMap< WORD, DtMsg > MapAcceKeyToCommWnd;

		
		TextBufVec			m_TextBuf;
		CRenderFont*		m_curRdFont; 
		UIString			m_sFileName;
		SQRRootWnd*			m_pRootWnd;
		SQRWnd*				m_pModule;				// 当前模态窗口
		SQRWnd*				m_pHeldWnd;
		SQRWnd*				m_pFocusWnd;
		SQRWnd*				m_pTipHoldWnd;
		IGraphic*			m_pGraphic;
		IEffectManager*		m_pEffectMgr;
		IGUIHandler*		m_pIGUIHandler;
		SQRWnd*				m_PreCurSet;
		CFPos				m_CurCursor;
		CFPos				m_LogicCurCursor;
		uint32				m_CurMessage;
		uint32				m_nPreUpdateTime;
		SQRWnd*				m_pMoreLastMouseMsgWnd; // 记录最后一个接受鼠标消息的窗口
		SQRWnd*				m_pLastMouseMsgWnd;		// 最后一个接受鼠标消息的窗口
		SQRWnd*				m_pLastLButtonDownWnd;	// 最后一个接受鼠标左键按下消息的窗口
		SQRWnd*				m_pLastRButtonDownWnd;	// 最后一个接受鼠标右键按下消息的窗口
		uint32				m_uPreMouseMsgTime;		// 上一条消息的时间
		bool				m_bUserDefineDesc;		// 使用自定义MouseDesc显示
		bool				m_bIsEditActive;		// 是否有SQREdit焦点
		SQRWnd*				m_UserShowWnd;			// 显示的窗口
		bool				m_bUserShowWnd;			// 使用显示的窗口
		static HookMap		m_Hook;					// GUI钩子
		static AcceleratorMap	m_Accelerator;		// GUI快捷键
		UISet<SQRWnd*>				m_AllWnds;		// 所有创建的窗口
		UIMap<UIString, HCURSOR>	m_CursorRes;
		UIMap<HCURSOR, UIString>	m_Cursor2Name;
		HCURSOR					m_hGlobCursor;
		MapAcceKeyToCommWnd		m_AcceKey;			// 快捷键注册表
		MsgResMap				m_ResContainer;		// MessageBox
		bool					m_isShowWndArea;
		MapResStaticInfo		m_ResStaticInfoContainer;// GUI静态文本容器
		HWND					m_hMainWnd;
		bool					m_bLockCursorMessage;		// 锁定鼠标位置
		float					m_fDragResponseDis;			//拖拽容错距离
		bool					m_bRefreshToolTip;

		//----------------渲染------------------
		ITexture*				m_pTextue;			// ForTexturBuf
		ITexture*				m_pAlpha;
		ERenderStyle			m_RS;
		UIVector<RectGroup>		m_TextureBuf;
		uint32					m_nCurTextureBufPos;

		uint32					m_nTotalRect;
		uint32					m_nFlush;
		float					m_fZoomSize;
		bool					m_bHidenMode;	///< 主界面隐藏模式
		UISet<CRenderFont*>		m_FontVec;

		//----------------资源解析--------------------
		XMLParser*				m_pXMLParser;		// XML解析器
		GUILayoutHandler*		m_pLayoutHandler;	// layout handler
		bool					m_beAucRes;			// 是否为AUC资源
		bool					m_beGUITool;		// 是否为guitool使用
		bool					m_beInitedRes;		// GUI资源是否预初始化

	public:
		static UIString			m_strPkgName;		// 为使pkg包知道GUI资源所在包名而记录的GUI pkg包名
		ResMap					m_ResTemplate;		// 资源模板
		inline const char*	GetGuiPkgPath() { return m_strPkgName.c_str(); }

	private:
		void				DragWnd(SQRWnd& Wnd);
	
	public:
		SQRGUIManager( IGUIHandler* pIGUIHandler, CFRect& rt, SQRRootWnd& Root, IGraphic* pGraphic, HWND hMainWnd,
			IEffectManager* pEffectMgr, const char* szResFileName,  const char* szPkgName = NULL, bool beUITool = FALSE, bool beInit = true );
		~SQRGUIManager();

		void				LoadStaticText();
		HRESULT				WndProc( uint32 message, WPARAM wParam, LPARAM lParam );
		void				Render();


		void				SetRefreshToolTip();
		SQRRootWnd*			GetRootWnd() const;
		SQRWnd*				GetModuleWnd() const;
		IGraphic*			GetGraphic() const;
		IEffectManager*		GetEffectManager() const;
		IGUIHandler*		GetGUIHandler() const;
		void				SetXMLParser();
		void				SetLayoutHandler();
		XMLParser*			GetXMLParser() const;	
		void				SetBeGUITool( bool be_tool );
		bool				GetBeGUITool();
		GUILayoutHandler*	GetLayoutHandler() const;
		float				GetSreenWidth() const;
		float				GetSreenHeight() const;
		const CFPos&		GetCursorPos() const;
		CFPos&				GetLogicCursorPos();
		uint32				GetCurMessage() const;
		void				SetGlobalCursor( HCURSOR hCursor );
		HCURSOR				GetGlobalCursor() const;
		BOOL				FindWnd( SQRWnd* pWnd );
		bool				IsFocus( SQRWnd* pWnd );
		bool				GetIsEditActive();
		void				SetIsEditActive(bool bIsEditActive);
		SQRWnd*				GetLastMouseMsgWnd();
		SQRWnd*				GetLastLButtonDownWnd();
		void				SetLastLButtonDownWnd( SQRWnd* pWnd );
		SQRWnd*				GetLastRButtonDownWnd();
		void				SetLastRButtonDownWnd( SQRWnd* pWnd );
		MsgResMap&			GetMsgResMap();
		void				SetZoomSize( float fSize );
		float				GetZoomSize();
		//GUIRenderEvn		SetRenderEvn();
		//void				SetRenderEvn(const GUIRenderEvn &evn);
		//GUIRenderEvn		GetRenderEvn();
		static void			RegisterAccelerator( IAccelerator* pAccelerator, uint32 uWndMsg );
		static void			UnRegisterAccelerator( IAccelerator* pAccelerator );
		static void			RegisterHook( IHook* pHook, uint32 uWndMsg );
		static void			UnRegisterHook( IHook* pHook );
		void				AddHookMsg( IHook* pHook, uint32 uWndMsg );
		void				CreateTipHoldWnd();
		void				OnTipHandlerDel(SQRWnd* wnd);

		bool				CheckHook( SQRWnd& Wnd, uint32 message, WPARAM wParam, LPARAM lParam );
		bool				CheckAccelerator( SQRWnd& Wnd, uint32 message, WPARAM wParam, LPARAM lParam );

		//以下函数进行消息的筛选和分配，是整个GUI系统的核心代码

		//得到某个鼠标位置落在那个窗口上
		SQRWnd* DetectWnd( SQRWnd& Wnd, CFPos posCursor );
		//设置活动窗口
		void SetActive( SQRWnd& Wnd, bool bFoucus );
		//消息筛选
		LRESULT WndProc( SQRWnd& Wnd, SQRWnd& WndOnDrag, uint32 message, WPARAM wParam, LPARAM lParam );
		//父窗口移动，子窗口相应调整
		void OnParentMoved( SQRWnd& Wnd, float dx, float dy );
		//设置x,y位置窗口的点击拖拽状态
		void SetHeldWindow( SQRWnd* pWnd );
		//画窗口
		/*void Draw( SQRWnd& Wnd );*/
		//画窗口
		void DrawModulate( SQRWnd& Wnd );

		//画窗口区域
		void DrawArea( SQRWnd& Wnd );

		//得到窗口管理器
		static SQRGUIManager* GetGUI( SQRWnd* pWnd );
		//设置当前模态窗口
		void SetModuleWnd( SQRWnd* pWnd );
		//删除当前模态窗口
		void DeleteModuleWnd( SQRWnd* pWnd );
		//登记窗口
		void RegistWnd( SQRWnd* pWnd );
		//注销窗口
		void UnregistWnd( SQRWnd* pWnd );
		//得到最顶窗口
		SQRWnd* GetTopWnd( SQRWnd* pWnd = NULL );
		//登记加速键
		void RegisterAccelKeyCommand( SQRWnd* pWnd, bool bCtrl, bool bShift, bool bAlt, uint8 VirKey, uint32 wParam, uint32 lParam );
		//翻译加速键
		SQRWnd* TranslateAccelKey( uint32& message, WPARAM& wParam, LPARAM& lParam );
		//取消加速键
		void UnRegisterAccelKeyCommand( SQRWnd* pWnd );
		//取消加速键
		void UnRegisterAccelKeyCommand( bool bCtrl, bool bShift, bool bAlt, uint8 VirKey );
		//初始化资源窗口
		void LoadResMap();
		//从AUC读资源
		bool LoadResFromAuc();
		//从XML读资源
		bool LoadResFromXml();
		//从coder读资源
		bool LoadResFromCoder();
		//得到窗口资源
		WndResParam* GetWndRes( const char* szWindowNameInResFile );
		//从资源创建窗口背景
		WND_IMAGE_LIST LoadBKRes( UIVector<WindowImage>& Res );
		//创建光标资源
		HCURSOR LoadCursorRes( const char* sCurFile );
		//得到光标字符串
		const char* GetCursorFileName( HCURSOR hCursor );
		//更新光标
		void UpdateCursor(SQRWnd &Wnd);
		//播放特效
		void PlayFx( SQRWnd* pWnd, const char* szFx, EEffectRun eEffrun = eEffRun_Normal );
		//立即播放特效
		void* PlayFxIme( SQRWnd* pWnd, const char* szFx, EEffectRun eEffrun = eEffRun_Normal );
		//播放特效
		void PlayFxEx( SQRWnd* pWnd, const char* szFxFile, const char* szFx, EEffectRun eEffrun = eEffRun_Normal );
		//立即播放特效
		void* PlayFxImeEx( SQRWnd* pWnd, const char* szFxFile, const char* szFx, EEffectRun eEffrun = eEffRun_Normal );
		//清理GUI特效
		void ClearFx( SQRWnd* pWnd, const char* szFx );
		//清理GUI特效
		void ClearFx( SQRWnd* pWnd );
		//清理GUI立即特效
		void ClearImeFx( SQRWnd* pWnd, const char* szFx );
		//刷新立即特效
		bool DrawImeFx( SQRWnd* pWnd, const char* szFx, uint curTime, float zoom );
		//关闭、启用输入法
		void EnableIme( bool bEnable );
		
		//加载GUI静态文本
		void LoadResStaticText();
		void LoadResStaticTxt();
		bool ProcessStaticText(UIVector<UIString>& vec, string& txt);		// true title; false desc
		void LoadResTxt(UIMap<UIString,UIString>& txtmap, const wchar_t* pkg, const char* file);
		UIMap<const UIString, ResInfo>* GetResStaticInfoContainer();
		ResInfo*	GetResInfo(const UIString& str);

		//画矩形
		void DrawRect( const CFRect& rtr, uint32 uColor, ITexture* pTexture, const CFRect* rtText, ERenderStyle rs, float z, FLOAT *pOffset = NULL);
		//画矩形
		void DrawRect( const CFRect& rtr, uint32 uColor, ITexture* pTexture, const CPosRect& rtText, ERenderStyle rs, float z );
		//画矩形
		void DrawRect( const CVector2f* rt, uint32 uColor, ITexture* pTexture, const CVector2f* rtText, ERenderStyle rs, float z );
		//画矩形
		void DrawRect( const CVector2f* rt, uint32 uColor, ITexture* pTexture0, ITexture* pTexture1, const CFRect* rtText0, const CVector2f* rtText1, ERenderStyle rs, float z );
		//画矩形
		void DrawRect( const CVector2f* rt, uint32 uColor, ITexture* pTexture0, ITexture* pTexture1, ITexture* pTexture2, ERenderStyle rs, float z );
		//画矩形
		void FlushRect();
		//画文字
		void FlushText();
		//画矩形和文字
		void FlushDraw();
		//写字
		void DrawText( CRenderFont* font, float size, const wstring& text,
			uint32 color, uint32 bkColor, uint32 gradualColor,
			CFRect rect, float x, float y, float z,
			FontEffect::Mask mask, float offset );
		//字体清理处理
		void ClearFontCatch(CRenderFont* font);
		//当前帧文字绘制结束
		void EndFontFrame();

		virtual void SwitchHidenMode(){ m_bHidenMode = !m_bHidenMode; }
		bool GetHidenMode();

		//需要显示的窗口
		void LoadWnd(SQRWnd *pWnd);

		//卸载需要显示的窗口
		void UnloadWnd();

		//显示窗体轮廓
		void ShowWndArea(bool isShow)				{ m_isShowWndArea = isShow;	};
		bool isShowWndArea()						{ return m_isShowWndArea;	};
		void SetLockCursorMessage(bool bLock)		{ m_bLockCursorMessage = bLock; };
		bool GetLockCursorMessage()					{ return m_bLockCursorMessage; };
		void RenderArea();
		void ShowCursor(bool bShow);
		void GetSysCursorPos(CFPos& pos);
		void SetSysCursorPos(int x, int y);
		void GetClientRect(CIRect& rect);
		void ClipCursor(CIRect& rect);
		void SetCapture();
		void ReleaseCapture();
	};

	inline int16 GetWndZOrder( SQRWnd* pWnd )
	{
		SQRWnd* pParent = pWnd->GetParent();
		int16 nOrder = 1;
		if ( !pParent )
			return nOrder;
		SQRWnd* pChild = pParent->FindLastChild();
		while ( pChild )
		{
			if ( pChild == pWnd )
				return nOrder;
			nOrder++;
			pChild = pChild->FindPreWnd();
		}

		return nOrder;
	}

	//-----------------------------------------------------------------------------
	inline void	
		SQRGUIManager::SetRefreshToolTip()
	{
		m_bRefreshToolTip = true;
	}

	//------------------------------------------------------------------------------
	inline SQRRootWnd*
		SQRGUIManager::GetRootWnd() const
	{
		return m_pRootWnd;
	}

	//------------------------------------------------------------------------------
	inline SQRWnd*
		SQRGUIManager::GetModuleWnd() const
	{
		return m_pModule;
	}

	//------------------------------------------------------------------------------
	inline bool 
		SQRGUIManager::GetHidenMode()
	{
		return m_bHidenMode;
	}

	//------------------------------------------------------------------------------
	inline IGraphic*
		SQRGUIManager::GetGraphic() const
	{
		return m_pGraphic;
	}

	//------------------------------------------------------------------------------
	inline IEffectManager*
		SQRGUIManager::GetEffectManager() const
	{
		return m_pEffectMgr;
	}

	//------------------------------------------------------------------------------
	inline IGUIHandler*
	SQRGUIManager::GetGUIHandler() const
	{
		return m_pIGUIHandler;
	}

	//------------------------------------------------------------------------------
	inline void
		SQRGUIManager::SetXMLParser()
	{
		if (!m_pXMLParser)
		{
			// 目前只有这一个解析器
			m_pXMLParser = new StuffParser();
		}
	}

	//------------------------------------------------------------------------------
	inline void
		SQRGUIManager::SetLayoutHandler()
	{
		if (!m_pLayoutHandler)
		{
			m_pLayoutHandler = new GUILayoutHandler();
		}
	}

	//------------------------------------------------------------------------------
	inline XMLParser*			
		SQRGUIManager::GetXMLParser() const
	{
		return m_pXMLParser;
	}

	inline void
		SQRGUIManager::SetBeGUITool(bool be_tool)
	{
		m_beGUITool = be_tool;
	}

	inline bool
		SQRGUIManager::GetBeGUITool()
	{
		return m_beGUITool;
	}

	//------------------------------------------------------------------------------
	inline GUILayoutHandler*	
		SQRGUIManager::GetLayoutHandler() const
	{
		return m_pLayoutHandler;
	}

	//------------------------------------------------------------------------------
	inline float
		SQRGUIManager::GetSreenWidth() const
	{
		return m_pRootWnd->m_hWnd->m_Width;
	}

	//------------------------------------------------------------------------------
	inline float
		SQRGUIManager::GetSreenHeight() const
	{
		return m_pRootWnd->m_hWnd->m_Height;
	}

	//------------------------------------------------------------------------------
	inline const CFPos&
		SQRGUIManager::GetCursorPos() const
	{
		return m_CurCursor;
	}

	//------------------------------------------------------------------------------
	inline uint32
		SQRGUIManager::GetCurMessage() const
	{
		return m_CurMessage;
	}

	//------------------------------------------------------------------------------
	inline void
		SQRGUIManager::SetGlobalCursor( HCURSOR hCursor )
	{
		m_hGlobCursor = hCursor;
	}

	//------------------------------------------------------------------------------
	inline HCURSOR
		SQRGUIManager::GetGlobalCursor() const
	{
		return m_hGlobCursor;
	}

	//------------------------------------------------------------------------------
	inline BOOL
		SQRGUIManager::FindWnd( SQRWnd* pWnd )
	{
		return m_AllWnds.find( pWnd ) != m_AllWnds.end();
	}

	//------------------------------------------------------------------------------
	inline bool
		SQRGUIManager::IsFocus( SQRWnd* pWnd )
	{
		return m_pFocusWnd == pWnd;
	}

	//------------------------------------------------------------------------------
	inline bool
		SQRGUIManager::GetIsEditActive()
	{
		return m_bIsEditActive;
	}

	//------------------------------------------------------------------------------
	inline void
		SQRGUIManager::SetIsEditActive( bool bIsEditActive )
	{
		m_bIsEditActive = bIsEditActive;
	}

	//------------------------------------------------------------------------------
	inline SQRWnd*
		SQRGUIManager::GetLastMouseMsgWnd()
	{
		return m_pLastMouseMsgWnd;
	}

	//------------------------------------------------------------------------------
	inline SQRWnd*
		SQRGUIManager::GetLastLButtonDownWnd()
	{
		return m_pLastLButtonDownWnd;
	}

	//------------------------------------------------------------------------------
	inline void
		SQRGUIManager::SetLastLButtonDownWnd(SQRWnd* pWnd)
	{
		m_pLastLButtonDownWnd = pWnd;
	}

	//------------------------------------------------------------------------------
	inline SQRWnd*
		SQRGUIManager::GetLastRButtonDownWnd()
	{
		return m_pLastRButtonDownWnd;
	}

	//------------------------------------------------------------------------------
	inline void
		SQRGUIManager::SetLastRButtonDownWnd(SQRWnd* pWnd)
	{
		m_pLastRButtonDownWnd = pWnd;
	}

	//------------------------------------------------------------------------------
	inline MsgResMap&
		SQRGUIManager::GetMsgResMap()
	{
		return m_ResContainer;
	}

	//------------------------------------------------------------------------------
	inline void
		SQRGUIManager::SetZoomSize( float fSize )
	{
		m_fZoomSize = fSize;
	}

	//------------------------------------------------------------------------------
	inline float
		SQRGUIManager::GetZoomSize()
	{
		return m_fZoomSize;
	}

	//------------------------------------------------------------------------------
	inline void
		SQRGUIManager::LoadWnd( SQRWnd *pWnd )
	{
		m_UserShowWnd = pWnd;
	}

	//------------------------------------------------------------------------------
	inline void
		SQRGUIManager::UnloadWnd()
	{
		m_UserShowWnd = NULL;
	}

	//------------------------------------------------------------------------------
	inline UIMap<const UIString, ResInfo>*
		SQRGUIManager::GetResStaticInfoContainer()
	{
		return &m_ResStaticInfoContainer;
	}
	
}// namespace sqr
