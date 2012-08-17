#pragma once
#include "SQRWnd.h"
#include "SQRTipHolder.h"
#include "TSqrAllocator.inl"

namespace sqr
{
	class SQRDrawHandler : public SQRGuiMallocObject
	{
		public:
			SQRDrawHandler(void);
			~SQRDrawHandler(void);
			virtual void DrawAfterChildren(){};
	};

	class GUI_API SQRRootWnd : public SQRWnd
	{
		DYNAMIC_DECLARE_WND( SQRRootWnd )
		CFRect		m_rtOrg;
		CVector2f	m_fDefinition;	///< 清晰度
		float		m_fZoom;
		float		m_fZoomBak;
		bool		m_bZmLimit;

		SQRDrawHandler* m_pDrawHandler;

	public:
		SQRRootWnd(void);
		~SQRRootWnd(void);

		void	CreateRootWnd( IGUIHandler* pIGUIHandler, CFRect& rt, IGraphic* pIGraphic, HWND hMainWnd, IEffectManager* pEffectMgr = NULL,
			const char* szResFileName = NULL, const char* szPkgName = NULL, bool beUITool = FALSE, bool binit = true );
		void	LoadStaticText();
		LRESULT	RecieveMsg( uint32 message, WPARAM wParam, LPARAM lParam );
		void	SetWndSize(float x, float y, float width, float height);
		float	GetZoomSize();
		void	SwitchHidenMode();

		float	GetOrgZoom();
		void	SetOrgZoom(float size, bool bdo = false);

		virtual void DrawAfterChildren();
		const CVector2f& GetDefinition() const;
		void	SetDefinition(const CVector2f& f);
		CFRect& GetOrgRect();
		void	SetOrgRect(CFRect rect);
		void	ShowWndArea(bool isShow);
		bool	isShowWndArea();
		void	SetDrawHandler( SQRDrawHandler*  DrawHandler);
		void	_ResizeRect( const CFRect& rt ,bool refreshbak = true);
		void	_SetWndRect( const CFRect& rt );
		void	ResizeRectOneChild( const CFRect& rt, SQRWnd* pChild );
		void	ShowCursor(bool bShow);
		void	GetSysCursorPos(CFPos& pos);
		void	SetSysCursorPos(int x, int y);
		void	SetLockCursorMessage(bool bLockCursorMessage);
		void	GetClientRect(CIRect& rect);
		void	ClipCursor(CIRect& rect);
		void	SetCapture();
		void	ReleaseCapture();
		float	GetWndLeft()	{ return 0.0f; };
		float	GetWndTop()		{ return 0.0f; };
		void	SetBeZoomLimited(bool b);
		void	SetLimitedState(bool st);	
		void	SetLimitedStateWithZoom(bool st, float zm);
	private:
	};
	
	//------------------------------------------------------------------------------
	inline void SQRRootWnd::SetBeZoomLimited(bool b)
	{
		m_bZmLimit = b;
	}

	//------------------------------------------------------------------------------
	inline const CVector2f&	SQRRootWnd::GetDefinition() const
	{
		return m_fDefinition;
	}

	//------------------------------------------------------------------------------
	inline void	SQRRootWnd::SetDefinition( const CVector2f& f )
	{
		m_fDefinition = f;
	}
	//------------------------------------------------------------------------------
	inline CFRect&	SQRRootWnd::GetOrgRect()
	{
		return m_rtOrg;
	}

	//------------------------------------------------------------------------------
	inline void	SQRRootWnd::SetOrgRect( CFRect rect )
	{
		WndHandle* pWndH = GetWndHandle();
		m_rtOrg = rect;
		pWndH->m_xInitial		= m_rtOrg.left;                 //窗口初始的水平坐标
		pWndH->m_yInitial		= m_rtOrg.top;					//窗口初始的的垂直坐标
		pWndH->m_WidthInitial	= m_rtOrg.right - m_rtOrg.left; //窗口初始的的宽度
		pWndH->m_HeightInitial	= m_rtOrg.bottom - m_rtOrg.top; //窗口初始的的高度
	}

}

;
