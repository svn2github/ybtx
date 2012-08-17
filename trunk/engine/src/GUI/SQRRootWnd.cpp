#include "stdafx.h"
#include "SQRRootWnd.h"
#include "SQRGUIManager.h"
#include "CGraphic.h"
#include "CodeCvs.h"
#include "SQRTipHolder.h"

namespace sqr
{
	
	SQRDrawHandler::SQRDrawHandler(void)
	{
	
	}
	SQRDrawHandler::~SQRDrawHandler(void)
	{
	}

	DYNAMIC_CREATE_WND( SQRRootWnd, SQRWnd )


	SQRRootWnd::SQRRootWnd(void)
	: m_fDefinition(1.0f, 1.0f)
	, m_fZoom(1.0f)
	, m_fZoomBak(1.0f)
	, m_pDrawHandler(NULL)
	, m_bZmLimit(false)
	{
	}

	SQRRootWnd::~SQRRootWnd(void)
	{
		SQRTipHolder::Destroy();
	}

	// 功能：创建根窗口
	void SQRRootWnd::CreateRootWnd( IGUIHandler* pIGUIHandler, CFRect& rt, IGraphic* pIGraphic, HWND hMainWnd, IEffectManager* pEffectMgr,
		const char* szResFileName, const char* szPkgName, bool beUITool, bool binit )
	{
		m_rtOrg = rt;
		WndHandle* pWndH = GetWndHandle();
		pWndH->m_pGUI = new SQRGUIManager( pIGUIHandler, rt, *this, pIGraphic, hMainWnd, pEffectMgr, szResFileName, szPkgName, beUITool, binit );
		pWndH->m_pGUI->RegistWnd( this );
		pWndH->m_xInitial		= m_rtOrg.left;                 //窗口初始的水平坐标
		pWndH->m_yInitial		= m_rtOrg.top;					//窗口初始的的垂直坐标
		pWndH->m_WidthInitial	= m_rtOrg.right - m_rtOrg.left; //窗口初始的的宽度
		pWndH->m_HeightInitial	= m_rtOrg.bottom - m_rtOrg.top; //窗口初始的的高度
		pWndH->m_bShow			= true;
		OnCreated();

		// the tip holder alive or die with the root wnd
		SQRTipHolder::Create();
		SQRTipHolder::GetInst()->CreatHolderWnd(this);
	}

	void SQRRootWnd::LoadStaticText()
	{
		GetWndHandle()->m_pGUI->LoadStaticText();
	}

	// 功能：根窗口接受要处理的消息，其他窗口调用无效
	LRESULT SQRRootWnd::RecieveMsg( uint32 message, WPARAM wParam, LPARAM lParam )
	{
		SQR_TRY
		{
			if ( m_hWnd->m_ParentWnd || !m_hWnd->m_pGUI )
				return 0;
			if ( message <= WM_MOUSELAST && message >= WM_MOUSEFIRST && message != WM_SETCURSOR )
			{
				int x = (int)(LOWORD(lParam) * m_fDefinition.x);
				int y = (int)(HIWORD(lParam) * m_fDefinition.y);
				lParam = MAKELONG(x,y);
			}

			return m_hWnd->m_pGUI->WndProc( message, wParam, lParam );
		}
		SQR_CATCH(exp)
		{
			LogExp(exp);
		}
		SQR_TRY_END;
		return NULL;
	}

	void SQRRootWnd::_ResizeRect( const CFRect& rt ,bool refreshbak )
	{
		CFRect RootRt = rt;
		RootRt.bottom -= RootRt.top;
		RootRt.right -= RootRt.left;
		RootRt.top = 0.0f;
		RootRt.left = 0.0f;
		SQRWnd::_ResizeRect(RootRt,refreshbak);	

		m_hWnd->m_xScreen = rt.left;
		m_hWnd->m_yScreen = rt.top;
		if(refreshbak)
		{
			m_hWnd->m_xScreenbak = m_hWnd->m_xScreen;
			m_hWnd->m_yScreenbak = m_hWnd->m_yScreen;
		}
		OnScreenPosChanged();
	}

	void SQRRootWnd::_SetWndRect( const CFRect& rt )
	{
		CFRect RootRt = rt;
		RootRt.bottom -= RootRt.top;
		RootRt.right -= RootRt.left;
		RootRt.top = 0.0f;
		RootRt.left = 0.0f;

		SQRWnd::_SetWndRect(rt);
		m_hWnd->m_xScreen = m_hWnd->m_xScreenbak = rt.left;
		m_hWnd->m_yScreen = m_hWnd->m_yScreenbak = rt.top;
		OnScreenPosChanged();
	}

	void SQRRootWnd::ResizeRectOneChild( const CFRect& rt, SQRWnd* pChild )
	{
		float x = m_hWnd->m_xScreen;
		float y = m_hWnd->m_yScreen;
		SQRWnd::ResizeRectOneChild(rt,pChild);
		m_hWnd->m_xScreen = m_hWnd->m_xScreenbak = x;
		m_hWnd->m_yScreen = m_hWnd->m_yScreenbak = y;
		//OnScreenPosChanged();
	}

	void SQRRootWnd::SetWndSize(float x, float y, float width, float height)
	{
		float zoom = 1.0f;
		if ( (width/height) > (m_rtOrg.Width()/m_rtOrg.Height()) )
			zoom = height / m_rtOrg.Height();
		else
			zoom = width / m_rtOrg.Width();

		if( zoom > 1.0f && m_bZmLimit )
			zoom = sqrt(zoom);

		zoom *= m_fZoom;
		
		STATIC_RS& RS = CGraphic::GetInst()->GetStaticRS(RS_GUI);
		if(zoom>1.05f || zoom < 0.95)
		{
			RS.m_Sample_S0 		= TEXF_LINEAR;
			RS.m_Sample_S1 		= TEXF_LINEAR;
			RS.m_Sample_S2 		= TEXF_LINEAR;
			RS.m_Sample_S3 		= TEXF_LINEAR;
		}
		else
		{
			RS.m_Sample_S0 		= TEXF_POINT;
			RS.m_Sample_S1 		= TEXF_POINT;
			RS.m_Sample_S2 		= TEXF_POINT;
			RS.m_Sample_S3 		= TEXF_POINT;
		}
		CFRect  rtRootWnd(x, y, x + width, y + height);
		m_hWnd->m_pGUI->SetZoomSize(zoom);
		m_hWnd->m_pGUI->GetGUIHandler()->OnWndSized(rtRootWnd);
		ResizeRect(rtRootWnd);
	}

	float SQRRootWnd::GetZoomSize()  //可视化范围 zoom：摄像机移动
	{
		return m_hWnd->m_pGUI->GetZoomSize();
	}

	float SQRRootWnd::GetOrgZoom()  //可视化范围 zoom：摄像机移动
	{
		return m_fZoom;
	}

	void SQRRootWnd::SetOrgZoom(float size, bool bdo)  //可视化范围 zoom：摄像机移动
	{
		if(m_fZoom != size || bdo)
		{
			float zoom = 1.0f;
			WndHandle* pWndH = GetWndHandle();
			RECT  rtRootWnd = {static_cast<LONG>(pWndH->m_xScreen),
				static_cast<LONG>(pWndH->m_yScreen),
				static_cast<LONG>((pWndH->m_xScreen + pWndH->m_Width)),
				static_cast<LONG>((pWndH->m_yScreen + pWndH->m_Height))
			};

			if (pWndH->m_Width / static_cast<float>(pWndH->m_Height) > m_rtOrg.Width() / static_cast<float>(m_rtOrg.Height()))
				zoom = pWndH->m_Height / (float)(m_rtOrg.Height());
			else
				zoom = pWndH->m_Width / (float)(m_rtOrg.Width());


			if( zoom > 1.0f && m_bZmLimit )
				zoom = sqrt(zoom);

			m_fZoom = size;
			zoom *= m_fZoom;
			STATIC_RS& RS = CGraphic::GetInst()->GetStaticRS(RS_GUI);
			if(zoom>1.05f || zoom < 0.95)
			{
				RS.m_Sample_S0 		= TEXF_LINEAR;
				RS.m_Sample_S1 		= TEXF_LINEAR;
				RS.m_Sample_S2 		= TEXF_LINEAR;
				RS.m_Sample_S3 		= TEXF_LINEAR;
			}
			else
			{
				RS.m_Sample_S0 		= TEXF_POINT;
				RS.m_Sample_S1 		= TEXF_POINT;
				RS.m_Sample_S2 		= TEXF_POINT;
				RS.m_Sample_S3 		= TEXF_POINT;
			}
			m_hWnd->m_pGUI->SetZoomSize(zoom);

			ResizeRect(rtRootWnd);
		}
	}

	void SQRRootWnd::SetLimitedState(bool st)
	{
		if ( m_bZmLimit != st )
		{
			// 执行一些设置操做
			SetBeZoomLimited(st);
			if (st)			// 非LOADING  true
				SetOrgZoom(m_fZoomBak, true);
			else
				SetOrgZoom(1.0f, true);		// LOADING模式
		}
	}

	void SQRRootWnd::SetLimitedStateWithZoom(bool st, float zm)
	{
		// 执行一些设置操做
		SetBeZoomLimited(st);
		SetOrgZoom(zm, true);
		if (st == true)
			m_fZoomBak = zm;

	}

	void SQRRootWnd::ShowWndArea(bool isShow)
	{
		m_hWnd->m_pGUI->ShowWndArea(isShow);
	}

	bool SQRRootWnd::isShowWndArea()
	{
		return m_hWnd->m_pGUI->isShowWndArea();
	}

	void SQRRootWnd::DrawAfterChildren()
	{
		SQRWnd::DrawAfterChildren();
		if ( m_pDrawHandler != NULL )
		{
			m_pDrawHandler->DrawAfterChildren();
		}
	}

	void SQRRootWnd::SetDrawHandler( SQRDrawHandler*  DrawHandler)
	{
		if ( DrawHandler != NULL)
		{
			m_pDrawHandler = DrawHandler;
		}
	}

	void SQRRootWnd::GetSysCursorPos(CFPos& pos)
	{
		POINT CursorPoint;
		::GetCursorPos( &CursorPoint );
		pos.x = (float)CursorPoint.x;
		pos.y = (float)CursorPoint.y;
	}

	void SQRRootWnd::SetSysCursorPos(int x, int y)
	{
		::SetCursorPos(x,y);
	}
	
	void SQRRootWnd::ShowCursor(bool bShow)
	{
		::ShowCursor(bShow);
	}

	void SQRRootWnd::SetLockCursorMessage(bool bLock)
	{
		m_hWnd->m_pGUI->SetLockCursorMessage(bLock);
	}

	void SQRRootWnd::GetClientRect(CIRect& rect)
	{
		m_hWnd->m_pGUI->GetClientRect(rect);
	}

	void SQRRootWnd::ClipCursor(CIRect& rect)
	{
		m_hWnd->m_pGUI->ClipCursor(rect);
	}

	void SQRRootWnd::SetCapture()
	{
		m_hWnd->m_pGUI->SetCapture();
	}

	void SQRRootWnd::ReleaseCapture()
	{
		m_hWnd->m_pGUI->ReleaseCapture();
	}

	void SQRRootWnd::SwitchHidenMode()
	{
		m_hWnd->m_pGUI->SwitchHidenMode();
	}
}
