#include "stdafx.h"
#include "SQRGUIManager.h"
#include "CLightSystem.h"
#include <algorithm>
#include "PkgStream.h"
#include <d3dx9math.h>
#include "CCamera.h"
#include "CGraphic.h"
#include "CRenderGroup.h"
#include "CLightSystem.h"
#include "CBufFile.h"
#include <iostream>
#include <fstream>
#include "tinyxml.h"
#include "GUIBase.h"
#include "TSqrAllocator.inl"
#include "CSqrAllocPool.h"
#include "CFontMgr.h"
#include "CPkgDirectory.h"
#include "StringHelper.h"
#include "CMath.h"
#include "FlashDataManager.h"
#include "CGraphicExp.h"
#include "DebugHelper.h"
#include "CGuiCoder.h"
#include "CFontCfg.h"

#pragma warning(disable:4101)

namespace sqr
{
	const wstring GUI_PATH_ALIAS_GUITEX = L"guitex";
	const wstring GUI_PATH_ALIAS_GUI	= L"gui";
	const wstring GUI_PATH_ALIAS_LAN	= L"lan";
}

UIMap<IHook*,UIVector<uint32> > SQRGUIManager::m_Hook;
UIMap<IAccelerator*,UIVector<uint32> >SQRGUIManager::m_Accelerator;
const CFPos posInValid = CFPos( FLOAT_MAX_VALUE, FLOAT_MAX_VALUE );
DWORD dwAUFVersion = 0;


WndHandle::WndHandle()
		: m_WndStyle(0)
		, m_xScreen(0)
		, m_yScreen(0)
		, m_Width(0)
		, m_Height(0)
		, m_xScreenbak(0)
		, m_yScreenbak(0)
		, m_Widthbak(0)
		, m_Heightbak(0)
		, m_GroupID(-1)
		, m_uLockFlage(LOCK_LEFT|LOCK_TOP)
		, m_WndFont(NULL)
		, m_EnableColor(0xffffffff)
		, m_DisableColor(0xffffffff)

		, m_EnableBcColor(0)
		, m_DisableBcColor(0)

		, m_FontEffectMask(0)
		, m_FontGradualColor(0)

		, m_bIsShowBackground(true)
		, m_fTransparent(1.0f)
		, m_bShow(true)
		, m_bEnable(true)

		, m_dwHeldTime(0)
		, m_bIsHeld(false)
		, m_IsActive(false)
		, m_bNewByGUISys(false)
		, m_bGetMouseWheel(false)
		, m_bIsDownTooltips(false)

		, m_ParentWnd(NULL)
		, m_FirstChild(NULL)
		, m_LastChild(NULL)
		, m_PreBrother(NULL)
		, m_NextBrother(NULL)

		, m_pNextModule(NULL)

		, m_pGUI(NULL)

		, m_dwOtherData(0)
		, m_pResParam(NULL)
		, m_eRS(RS_GUI)
		, m_fZ(MAX_DEPTH)
		, m_hCursor(NULL)
		, m_pTransparenTex(NULL)
		, m_StyleIndex(0)
		, m_DescRichText(NULL)

		, m_bTexInit(false)
		, m_TipFontSize(0)
		, m_pRichToolTip(NULL)
{
		m_FlashInfo.clear();
}

// 静态成员声明
UIString SQRGUIManager::m_strPkgName;

SQRGUIManager::SQRGUIManager( IGUIHandler* pIGUIHandler, CFRect& rt, SQRRootWnd& Root, IGraphic* pGraphic, HWND hMainWnd,
                          IEffectManager* pEffectMgr, const char* szResFileName, const char* szPkgName, bool beUITool, bool beInit )
		: m_pRootWnd( &Root )
		, m_pGraphic( pGraphic )
		, m_pEffectMgr( pEffectMgr )
		, m_pIGUIHandler( pIGUIHandler )
		, m_beAucRes(TRUE)
		, m_pXMLParser(NULL)
		, m_pLayoutHandler(NULL)
		, m_pLastMouseMsgWnd( NULL )
		, m_pMoreLastMouseMsgWnd( NULL )
		, m_pHeldWnd(NULL)
		, m_hGlobCursor(NULL)
		, m_nPreUpdateTime(0)
		, m_nCurTextureBufPos(0)
		, m_pTextue(0)
		, m_RS(RS_GUI)
		, m_CurMessage(0)
		, m_sFileName( szResFileName ? szResFileName : "" )
		, m_pFocusWnd( &Root )
		, m_fZoomSize(1.0f)
		, m_UserShowWnd(NULL)
		, m_bIsEditActive(false)
		, m_isShowWndArea(false)
		, m_hMainWnd(hMainWnd)
		, m_bLockCursorMessage(false)
		, m_bHidenMode(false)
		, m_pTipHoldWnd(NULL)
		, m_fDragResponseDis(10.f)
		, m_curRdFont(NULL)
		, m_bRefreshToolTip(false)
{
	m_beGUITool = beUITool;
	m_pRootWnd->m_hWnd->m_Width  = rt.right - rt.left;
	m_pRootWnd->m_hWnd->m_Height = rt.bottom - rt.top;
	m_PreCurSet = NULL;
	m_pModule = NULL;
	beUITool ? m_beInitedRes = true :  m_beInitedRes = beInit;

	SetLayoutHandler();
	SetXMLParser();
	LoadResMap();
	SetGlobalCursor( LoadCursorRes( IDC_ARROW ) );
	SQRGUIStyleManager::Create();
	if (szPkgName == NULL)
		m_strPkgName.assign("");
	else
		m_strPkgName.assign( szPkgName );
	SQRGUIStyleManager::GetInst()->Load((m_strPkgName + "style/GuiStyle.xml").c_str());
	if(CMainWindowTarget::HasInst())
		CMainWindowTarget::GetInst()->InsertRenderer(this);
	
	FlashDataManager::Create();

	if ( !CFontCfg::HasInst() )
	{
		CFontCfg::Create();
		CFontCfg::GetInst()->UseDefault();
	}
}

SQRGUIManager::~SQRGUIManager()
{
	SafeRelease( m_pTextue );
	for ( UIMap<UIString, WndResParam*>::iterator it  = m_ResTemplate.begin(); it != m_ResTemplate.end(); it++ )
		delete it->second;
	if(CMainWindowTarget::HasInst())
		CMainWindowTarget::GetInst()->DeleteRenderer(this);

	SQRGUIStyleManager::Destroy();

	if ( m_pXMLParser )
		delete m_pXMLParser;
	if ( m_pLayoutHandler )
		delete m_pLayoutHandler;
	
	if (FlashDataManager::HasInst())
		FlashDataManager::Destroy();

	if ( CFontCfg::HasInst() )
	{
		CFontCfg::Destroy();
	}
}


/**
\brief
	这个方法要在语言系统初始化完毕之后调用
*/
void SQRGUIManager::LoadStaticText()
{
	// 读取GUI静态文本，其中根据当前语言配置加载对应语言文本
#ifdef READFROMXML
	LoadResStaticText();
#else
	LoadResStaticTxt();
#endif
}

////////////////////////////////////////////////////////////////////////
//得到某个鼠标位置落在哪个窗口上，并返回该窗口
////////////////////////////////////////////////////////////////////////
SQRWnd* SQRGUIManager::DetectWnd( SQRWnd& Wnd, CFPos posCursor )
{
	if (Wnd.m_hWnd->m_bShow)
	{
		for ( SQRWnd* pChildWnd = Wnd.FindFirstChild(); pChildWnd; pChildWnd = pChildWnd->FindNextWnd() )
		{
#ifdef UI_HIDE_MODE
			if ( GetHidenMode() )
			{
				if (Wnd.GetParent() == m_pRootWnd)
				{
					return Wnd.IsInWndArea( posCursor.x, posCursor.y )? &Wnd : NULL;
				}
			}
#endif
			SQR_TRY{
				SQRWnd* pWnd = DetectWnd( *pChildWnd, posCursor );
				if ( pWnd )
					return pWnd;
			}
			SQR_CATCH(exp) {
				Wnd.DestroyWnd();
				GfkLogExp(exp);
				return NULL;
			}
			SQR_TRY_END;
		}
		return Wnd.IsInWndArea( posCursor.x, posCursor.y )? &Wnd : NULL;
	}
	return NULL;

}


SQRWnd* SQRGUIManager::TranslateAccelKey( uint32& message, WPARAM& wParam, LPARAM& lParam )
{
	if ( ( ( message == WM_KEYDOWN || message == WM_SYSKEYDOWN ) && ( lParam&0x40000000 ) == 0 )
	        || ( message == WM_KEYUP && wParam == VK_SNAPSHOT ) )
	{
		WORD AccKey = static_cast<WORD>(wParam);   //typedef unsigned short      WORD; WPARAM也是一种数据类型
		if ( GetKeyState( VK_CONTROL )&0x8000 )
			AccKey |= ACCEKEY_CTRL;
		if ( GetKeyState( VK_SHIFT )&0x8000 )
			AccKey |= ACCEKEY_SHIFT;
		if ( GetKeyState( VK_MENU )&0x8000 )
			AccKey |= ACCEKEY_ALT;

		MapAcceKeyToCommWnd::iterator it = m_AcceKey.find( AccKey );
		if ( it != m_AcceKey.end() )
		{
			message		= WM_COMMAND;
			wParam		= it->second.wParam;    //命令
			lParam		= it->second.lParam;
			return it->second.pWnd;
		}
	}

	return NULL;
}


////////////////////////////////////////////////////////////////////////
// 功能：检查hook是否能通过
// 参数：uint32 message, WPARAM wParam, LPARAM lParam 消息内容
// 返回：是否通过
////////////////////////////////////////////////////////////////////////
bool SQRGUIManager::CheckHook( SQRWnd& Wnd, uint32 message, WPARAM wParam, LPARAM lParam )
{
	bool bPass = true;
	struct __
	{
		IHook* pHook;
		uint32 uMsg;
	};
	__ CurHook[1024];
	int32 n = 0;
	for ( HookMap::iterator it = m_Hook.begin(); it != m_Hook.end(); it++ )
	{
		for ( size_t i = 0; i < it->second.size(); i++, n++ )
		{
			if ( n >= 1024 )
				GenErr( "UI Manager error","Hook注册超过1024个" );
			CurHook[n].pHook = it->first;
			CurHook[n].uMsg  = it->second[i];
		}
	}

	for ( int32 i = 0; i < n; i++ )
	{
		if ( m_Hook.find( CurHook[i].pHook ) != m_Hook.end() &&
		        message == CurHook[i].uMsg &&
		        !CurHook[i].pHook->OnHook( &Wnd, message, wParam, lParam ) )
			bPass = false;
	}
	return bPass;
}

////////////////////////////////////////////////////////////////////////
// 功能：检查Accelerator是否能通过
// 参数：uint32 message, WPARAM wParam, LPARAM lParam 消息内容
// 返回：是否通过
////////////////////////////////////////////////////////////////////////
bool SQRGUIManager::CheckAccelerator( SQRWnd& Wnd, uint32 message, WPARAM wParam, LPARAM lParam )
{
	bool bPass = true;
	struct __
	{
		IAccelerator* pAccelerator;
		uint32 uMsg;
	};
	__ CurAccelerator[1024];
	int32 n = 0;
	for ( AcceleratorMap::iterator it = m_Accelerator.begin(); it != m_Accelerator.end(); it++ )
	{
		for ( size_t i = 0; i < it->second.size(); i++, n++ )
		{
			if ( n >= 1024 )
				GenErr( "UI Manager error", "pAccelerator注册超过1024个" );
			CurAccelerator[n].pAccelerator = it->first;
			CurAccelerator[n].uMsg  = it->second[i];
		}
	}
	if ( m_pLastMouseMsgWnd && FindWnd( m_pLastMouseMsgWnd ))
	{
		if (m_pLastMouseMsgWnd->GetMouseWheel() && message == WM_MOUSEWHEEL )
		{
			return false;
		}
	}
	if (this->GetIsEditActive() || GetModuleWnd())
	{
		if ( ( message >= WM_KEYFIRST && message <= WM_KEYLAST && wParam != VK_ESCAPE ) || ( message <= WM_KEYFIRST && message >= WM_KEYLAST ) )
		{
			for ( int32 i = 0; i < n; i++ )
			{
				if ( m_Accelerator.find( CurAccelerator[i].pAccelerator ) != m_Accelerator.end() &&
					message == CurAccelerator[i].uMsg )
						if (this->GetIsEditActive())
							CurAccelerator[i].pAccelerator->OnAcceleratorInEdit();
						else
							CurAccelerator[i].pAccelerator->OnAcceleratorIsModule(message, wParam, lParam);
			}
			return bPass;
		}
	}
	else
		this->EnableIme( false );

	for ( int32 i = 0; i < n; i++ )
	{
		if ( m_Accelerator.find( CurAccelerator[i].pAccelerator ) != m_Accelerator.end() &&
		        message == CurAccelerator[i].uMsg &&
		        !CurAccelerator[i].pAccelerator->OnAccelerator( message, wParam, lParam ) )
			bPass = false;
	}
	return bPass;
}

////////////////////////////////////////////////////////////////////////
// 功能：内部调用处理窗口的消息
// 参数：uint32 message, WPARAM wParam, LPARAM lParam 消息内容
// 返回：略
////////////////////////////////////////////////////////////////////////
HRESULT SQRGUIManager::WndProc( uint32 message, WPARAM wParam, LPARAM lParam )
{
	uint32 errstate = 0;
	SQR_TRY
	{
		SQRWnd* pWnd = m_pModule ? m_pModule : m_pRootWnd;

		uint32 uMsg = message;
		WPARAM wPrm = wParam;
		LPARAM lPrm = lParam;
		SQRWnd* pAcceKeyWnd = TranslateAccelKey( uMsg, wPrm, lPrm );
		SQRWnd* pAcceKeyWndTree = pAcceKeyWnd;
		while ( pAcceKeyWndTree )
		{
			if ( pWnd == pAcceKeyWndTree || lPrm )
				return pAcceKeyWnd->OnCommand( wPrm, 0 );
			pAcceKeyWndTree = pAcceKeyWndTree->GetParent();
		}
		errstate = 1;
		if ( message == WM_KEYDOWN && pWnd->GetClassName() == UIString( "SQRMessageWnd" ) )
			return pWnd->SendMsg( message, wParam, lParam );

		SQRWnd* pPreHeldWnd = m_pHeldWnd;
		if ( message == WM_LBUTTONUP || message == WM_MOUSEMOVE && !( wParam&MK_LBUTTON ) )
		{
			SetHeldWindow( NULL );
		}

		m_CurMessage = message;
		SQRWnd* pDetectWnd = NULL;

		if ( message <= WM_MOUSELAST && message >= WM_MOUSEFIRST || message == WM_SETCURSOR )
		{
			if ( message != WM_SETCURSOR )
			{
				m_CurCursor.x = ((float)LOWORD(lParam) + 0.5f) - m_pRootWnd->m_hWnd->m_xScreen;
				m_CurCursor.y = ((float)HIWORD(lParam) + 0.5f) - m_pRootWnd->m_hWnd->m_yScreen;
				lParam = MAKELONG(m_CurCursor.x, m_CurCursor.y);
				if (!m_pRootWnd->IsInWndArea(m_CurCursor.x ,m_CurCursor.y))
					return 0;
				if (GetLockCursorMessage())
				{
					if (message == WM_MOUSEMOVE || message == WM_MOUSEWHEEL)
					{
						CheckAccelerator( *m_pRootWnd, message, wParam, lParam );
					}
					if (message == WM_RBUTTONUP)
					{
						CheckHook( *m_pRootWnd, message, wParam, lParam );
					}
					errstate = 2;
					return 0;
				}
			}
			
			pDetectWnd = (m_pHeldWnd && !m_pHeldWnd->IsDetectMouseOverWnd()) ? m_pHeldWnd : DetectWnd( *pWnd, m_CurCursor);

			if (pDetectWnd)
			{
				if ( m_pLastMouseMsgWnd != pDetectWnd )
				{
					if ( m_pLastMouseMsgWnd && FindWnd( m_pLastMouseMsgWnd ) && m_pLastMouseMsgWnd->m_hWnd->m_posMouseOver != posInValid )
						m_pLastMouseMsgWnd->OnMouseDescEnd();
					m_uPreMouseMsgTime = uint32(GetProcessTime());
				}
				m_pLastMouseMsgWnd = pDetectWnd;
			}
			errstate = 3;

			if (pDetectWnd && pDetectWnd->IsEnable())
			{
				if ( ( message == WM_LBUTTONDOWN || message == WM_LBUTTONDBLCLK ) && pDetectWnd)
				{
					SetLastLButtonDownWnd( pDetectWnd );
				}
				else if ( message == WM_LBUTTONUP && pDetectWnd )
				{
					LRESULT lRes = WndProc( *pDetectWnd, *m_pHeldWnd, message, wParam, lParam );
					SetLastLButtonDownWnd( NULL );
					return lRes;
				}
				else if ( ( message == WM_RBUTTONDOWN  || message == WM_RBUTTONDBLCLK ) && pDetectWnd)
				{
					SetLastRButtonDownWnd( pDetectWnd );
				}
				else if ( message == WM_RBUTTONUP && pDetectWnd )
				{
					LRESULT lRes = WndProc( *pDetectWnd, *m_pHeldWnd, message, wParam, lParam );
					SetLastRButtonDownWnd( NULL );
					return lRes;
				}
			}
			errstate = 4;
			if ( !m_pHeldWnd && pPreHeldWnd && pDetectWnd != pPreHeldWnd )
				return 0;
		}

		pDetectWnd = pDetectWnd ? pDetectWnd : m_pFocusWnd;

		if ( !pDetectWnd->IsEnable() )
		{
			CheckHook( *m_pRootWnd, message, wParam,lParam);
			CheckAccelerator( *m_pRootWnd, message, wParam, lParam );
			return 0;
		}

		if ( message == WM_LBUTTONDOWN ||
				message == WM_LBUTTONDBLCLK||
				message == WM_RBUTTONDOWN ||
				message == WM_RBUTTONDBLCLK||
				message == WM_MBUTTONDOWN ||
				message == WM_MBUTTONDBLCLK )
		{
			SetActive( *pDetectWnd, true );
			if ( message == WM_LBUTTONDOWN )
				SetHeldWindow( pDetectWnd );
		}
		errstate = 5;
		if ( message == WM_DESTROY )
			PostQuitMessage(0);
		else if ( FindWnd( pWnd ) )
			return WndProc( *pDetectWnd, *m_pHeldWnd, message, wParam, lParam );
		return 0;
	}
	SQR_CATCH(exp)
	{
		ostringstream strm;	
		strm << "SQRGUIManager::WndProc 499:"
			<< errstate
			<<endl;
		exp.AppendMsg(strm.str().c_str());
		LogExp(exp);
	}
	SQR_TRY_END;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// 功能：内部调用处理窗口的消息
// 参数：uint32 message, WPARAM wParam, LPARAM lParam 消息内容
// 返回：略
////////////////////////////////////////////////////////////////////////
LRESULT SQRGUIManager::WndProc( SQRWnd& Wnd, SQRWnd& WndOnDrag, uint32 message, WPARAM wParam, LPARAM lParam )
{
	uint32 errstate = 0;
	string errstring;
	Wnd.GetWndDebugInfo(errstring);
	SQR_TRY
	{
		if ( &WndOnDrag != NULL && message == WM_MOUSEMOVE && ( wParam&MK_LBUTTON ) )
		{
			errstate = 1;
			if ( WndOnDrag.m_hWnd->m_ParentWnd )
			{
				short x = (short)LOWORD(lParam);
				short y = (short)HIWORD(lParam);
				x = x - (short)( WndOnDrag.GetWndLeft() );
				y = y - (short)( WndOnDrag.GetWndTop() );

				if ( !CheckHook( WndOnDrag, message, wParam, MAKELONG( x, y ) ) )
					return 0;
				CheckAccelerator( WndOnDrag, message, wParam, MAKELONG( x, y ) );
				if (!FindWnd(&WndOnDrag))
					return 0;
				errstate = 2;
				DragWnd(WndOnDrag);
			}
			return 0;
		}
		else
		{
			if ( message >= WM_MOUSEFIRST && message <= WM_MOUSELAST  || message == WM_SETCURSOR )
			{
				SQRGUIManager* pGUI = Wnd.m_hWnd->m_pGUI;
				errstate = 4;
				if ( pGUI && pGUI->m_PreCurSet != &Wnd || message == WM_SETCURSOR )
				{
					pGUI->m_PreCurSet = &Wnd;
					if ( pGUI->m_PreCurSet->m_hWnd->m_hCursor )
						SetCursor( pGUI->m_PreCurSet->m_hWnd->m_hCursor );
					else
						SetCursor( m_hGlobCursor );
				}
				errstate = 5;
			}

			if ( message <= WM_MOUSELAST && message >= WM_MOUSEFIRST )
			{
				short x = (short)LOWORD(lParam);
				short y = (short)HIWORD(lParam);
				x = x - (short)( Wnd.GetWndLeft() );
				y = y - (short)( Wnd.GetWndTop() );

				lParam = MAKELONG( x, y );
			}
			errstate = 6;
			if ( !CheckHook( Wnd, message, wParam, lParam ) )
				if (!this->GetIsEditActive())
				{
					CheckAccelerator( Wnd, message, wParam, lParam );
					return 0;
				}
			CheckAccelerator( Wnd, message, wParam, lParam );
			errstate = 7;
			///click effect and sound lbutton down
			if ( message == WM_LBUTTONDOWN && FindWnd(&Wnd) )
			{
				Wnd.OnPlayWndFx( eMF_Click );
				Wnd.OnPlayWndSound( eMS_Click );
			}
			errstate = 8;
			if (FindWnd(&Wnd))
				return Wnd.WndProc( message, wParam, lParam );
			
		}
	}
	SQR_CATCH(exp)
	{
		ostringstream strm;	
		strm << "SQRGUIManager::WndProc 608:"
			<< errstate
			<< errstring
			<< endl;
		exp.AppendMsg(strm.str().c_str());
		LogExp(exp);
	}
	SQR_TRY_END;
	
	return NULL;
}

void SQRGUIManager::DrawModulate( SQRWnd& Wnd )
{
	FlushDraw();
	if ( Wnd.m_hWnd->m_pNextModule )
		DrawModulate( *Wnd.m_hWnd->m_pNextModule );
	Wnd.Draw();
	//Draw( Wnd );
}

//void SQRGUIManager::Draw( SQRWnd& Wnd )
//{
//	if (Wnd.m_hWnd->m_bShow)
//	{
//#ifdef TEXTURE_STINT_MODE
//		if (!(Wnd.m_hWnd->m_bTexInit))
//		{
//			Wnd.m_hWnd->m_bTexInit = true;
//			Wnd.CreatWndTex();
//		}
//#endif
//
//		Wnd.DrawWndBackground();
//		Wnd.DrawWndText();
//
//		if (Wnd.m_hWnd->m_bShow)
//		{
//			uint32 tProTime = uint32(GetProcessTime());
//			Wnd.DrawImeFx(tProTime, m_fZoomSize);
//
//			SQRWnd* ChildWnd = Wnd.m_hWnd->m_LastChild;
//			while ( ChildWnd )
//			{
//				if ( ChildWnd->bNotModule )
//					Draw( *ChildWnd );
//				ChildWnd = ChildWnd->m_hWnd->m_PreBrother;
//			}
//			Wnd.DrawAfterChildren();
//		}
//	}
//}

void SQRGUIManager::DrawArea( SQRWnd& Wnd )
{
	if (!(Wnd.m_hWnd->m_bShow))
		return;

#ifdef TEXTURE_STINT_MODE
	if (!(Wnd.m_hWnd->m_bTexInit))
	{
		Wnd.m_hWnd->m_bTexInit = true;
		Wnd.CreatWndTex();
	}
#endif

	Wnd.DrawWndArea();
	Wnd.DrawWndText();
	// draw ime fx
	uint32 tProTime = uint32(GetProcessTime());
	Wnd.DrawImeFx(tProTime, this->GetZoomSize());

	SQRWnd* ChildWnd = Wnd.m_hWnd->m_LastChild;
	while ( ChildWnd )
	{
		if ( ChildWnd->bNotModule )
			DrawArea( *ChildWnd );
		ChildWnd = ChildWnd->m_hWnd->m_PreBrother;
	}
}

void SQRGUIManager::OnParentMoved( SQRWnd& Wnd, float dx, float dy )
{
	if ( Wnd.m_hWnd->m_WndStyle & WS_CHILD )
	{
		Wnd.m_hWnd->m_xScreen += dx;
		Wnd.m_hWnd->m_yScreen += dy;

		Wnd.m_hWnd->m_xScreenbak += dx;
		Wnd.m_hWnd->m_yScreenbak += dy;

		SQRWnd* pChildWnd = Wnd.m_hWnd->m_FirstChild;
		while ( pChildWnd )
		{
			OnParentMoved( *pChildWnd, dx, dy );
			pChildWnd->OnParentWndMoved( dx, dy );
			if ( !FindWnd( pChildWnd ) )
				break;
			pChildWnd = pChildWnd->m_hWnd->m_NextBrother;
		}
	}
}


////////////////////////////////////////////////////////////////////////
// 功能：设置x,y位置窗口的点击拖拽状态
// 参数：BOOL bHeld    是否点击拖拽状态,
//		 int32 x, int32 y  点击拖拽位置
// 返回：无
////////////////////////////////////////////////////////////////////////
void SQRGUIManager::SetHeldWindow( SQRWnd* pWnd )
{
	if ( m_pHeldWnd )
	{
		if ( FindWnd( m_pHeldWnd ) )
			m_pHeldWnd->m_hWnd->m_bIsHeld = FALSE;
		m_pHeldWnd = NULL;
	}

	if ( pWnd )
	{
		m_pHeldWnd = pWnd;
		m_pHeldWnd->m_hWnd->m_bIsHeld = true;
		m_pHeldWnd->m_hWnd->m_CurHeldPos = m_CurCursor - CFPos( m_pHeldWnd->GetWndLeft(), m_pHeldWnd->GetWndTop() );
	}
}

SQRWnd* SQRGUIManager::GetTopWnd( SQRWnd* pWnd )
{
	SQRWnd* pChildWnd = pWnd ? pWnd->FindFirstChild() : ( m_pModule ? m_pModule : m_pRootWnd );
	while ( pChildWnd )
	{
		if ( pChildWnd->IsActive() )
			return GetTopWnd( pChildWnd );
		pChildWnd = pChildWnd->FindNextWnd();
	}
	return pWnd ? pWnd : ( m_pModule ? m_pModule : m_pRootWnd );
}

//得到窗口管理器
SQRGUIManager* SQRGUIManager::GetGUI( SQRWnd* pWnd )
{
	return pWnd ? pWnd->m_hWnd->m_pGUI : NULL;
}

//设置当前模态窗口
void SQRGUIManager::SetModuleWnd( SQRWnd* pWnd )
{
	DeleteModuleWnd( pWnd );
	SetHeldWindow( NULL );
	pWnd->m_hWnd->m_pNextModule = m_pModule;
	m_pModule = pWnd;
}

//删除当前模态窗口
void SQRGUIManager::DeleteModuleWnd( SQRWnd* pWnd )
{
	SQRWnd** ppWnd = &m_pModule;
	while ( *ppWnd )
	{
		if ( *ppWnd == pWnd )
		{
			*ppWnd = pWnd->m_hWnd->m_pNextModule;
			pWnd->m_hWnd->m_pNextModule = NULL;
			break;
		}
		ppWnd = &((*ppWnd)->m_hWnd->m_pNextModule);
	}
}

CFPos& SQRGUIManager::GetLogicCursorPos()
{
	//float x = m_pRootWnd->GetWndLeft();
	//float y = m_pRootWnd->GetWndTop();
	float zoomSize = m_pRootWnd->GetRootZoomSize();
	m_LogicCurCursor.x = (m_CurCursor.x)/zoomSize;
	m_LogicCurCursor.y = (m_CurCursor.y)/zoomSize;
	return m_LogicCurCursor;
}

//登记窗口
void SQRGUIManager::RegistWnd( SQRWnd* pWnd )
{
	m_AllWnds.insert( pWnd );
};

//注销窗口
void SQRGUIManager::UnregistWnd( SQRWnd* pWnd )
{
	UnRegisterAccelKeyCommand( pWnd );
	m_AllWnds.erase( pWnd );
	if ( m_PreCurSet == pWnd )
		m_PreCurSet = 0;
}

//设置活动窗口
void SQRGUIManager::SetActive( SQRWnd& Wnd, bool bFoucus )
{
	//if( !Wnd.IsActive() )
	if (bFoucus)
	{
		SQRWnd* vecWnd[256];
		vecWnd[0] = &Wnd;
		int32 nCur = 1;

		SQRWnd* pChildWnd = &Wnd;
		SQRWnd* pParent = Wnd.GetParent();
		//------
		while ( pParent && !Wnd.GetIsBottom() && nCur<256/*&& !(0x00310000L & Wnd.m_hWnd->m_WndStyle) && !(0x00310000L & _W( pParent )->m_WndStyle)*/)
		{
			if ( pChildWnd != pParent->m_hWnd->m_FirstChild )
				pChildWnd->InserBefore( pParent->m_hWnd->m_FirstChild );
			vecWnd[nCur++] = pChildWnd = pParent;
			pParent = pChildWnd->GetParent();
		}

		SQRWnd* pShareParent = NULL;
		for ( pShareParent = m_pFocusWnd; pShareParent && pShareParent->GetParent(); pShareParent = pShareParent->GetParent() )
		{
			if ( find( vecWnd, vecWnd + nCur, pShareParent ) != vecWnd + nCur )
				break;
			pShareParent->m_hWnd->m_IsActive = false;
			pShareParent->OnActive( false, pChildWnd );
		}

		if ( !pShareParent )
			GenErr( "UI Manager error","pShareParent为空" );

		m_pFocusWnd = &Wnd;
		bool bNeedActive = false;
		for ( int32 i = nCur - 1; i >= 0; i-- )
		{
			if ( vecWnd[i] == pShareParent )
				bNeedActive = true;
			else if ( !bNeedActive )
				continue;
			else
			{
				vecWnd[i]->m_hWnd->m_IsActive = true;
				vecWnd[i]->OnActive( true, pChildWnd );
			}
		}
	}
	else
	{
		m_pFocusWnd = &Wnd;
		m_pFocusWnd->m_hWnd->m_IsActive = false;
		m_pFocusWnd->OnActive( false, &Wnd );
		for ( SQRWnd* pChild = m_pFocusWnd->FindFirstChild(); pChild; pChild = pChild->FindFirstChild() )
		{
			pChild->m_hWnd->m_IsActive = false;
			pChild->OnActive( false, &Wnd );
		}
		while ( m_pFocusWnd && !m_pFocusWnd->GetIsBottom() )
		{
			if (m_pFocusWnd->GetParent())
				m_pFocusWnd = m_pFocusWnd->GetParent();
			else
				break;
		}
	}

}

//////////////////////////////////////////////////////////////////////////

void SQRGUIManager::RegisterAccelKeyCommand( SQRWnd* pWnd, bool bCtrl, bool bShift, bool bAlt, uint8 VirKey, uint32 wParam, uint32 lParam )
{
	// map组成< AccKey, pair<command, pWnd> >
	// 认为整个Gui系统使用一份map
	WORD AccKey = 0;
	if ( bCtrl )
		AccKey |= ACCEKEY_CTRL;
	if ( bShift )
		AccKey |= ACCEKEY_SHIFT;
	if ( bAlt )
		AccKey |= ACCEKEY_ALT;

	AccKey |= VirKey;

	DtMsg& msg = m_AcceKey[AccKey];
	msg.wParam = wParam;
	msg.lParam = lParam;
	msg.pWnd   = pWnd;
}

void SQRGUIManager::UnRegisterAccelKeyCommand( SQRWnd* pWnd )
{
	MapAcceKeyToCommWnd::iterator itmap = m_AcceKey.begin();

	while ( itmap != m_AcceKey.end() )
	{
		if ( itmap->second.pWnd == pWnd )
			m_AcceKey.erase( itmap++ );
		else
			itmap++;
	}
}

void SQRGUIManager::UnRegisterAccelKeyCommand( bool bCtrl, bool bShift, bool bAlt, uint8 VirKey )
{
	WORD AccKey = 0;
	if ( bCtrl )
		AccKey |= ACCEKEY_CTRL;
	if ( bShift )
		AccKey |= ACCEKEY_SHIFT;
	if ( bAlt )
		AccKey |= ACCEKEY_ALT;

	AccKey |= VirKey;
	m_AcceKey.erase( AccKey );
}

//GUIRenderEvn SQRGUIManager::SetRenderEvn()
//{
//	GUIRenderEvn Evn;
//
//	CLightSystem* pLight;
//	pLight = CMainWindowTarget::GetInst()->GetGroup()->GetLightSystem();
//	pLight->SetAmbient( 0xffffffff,1 );
//
//	CCamera* pCamera = CMainWindowTarget::GetInst()->GetCamera();
//	pCamera->SetPosition(CVector3f( 0.0f, 100.0f, -400.0f ));
//	pCamera->lookAt( CVector3f( 0, 0, 1 ));
//	pCamera->setFOVy(CMath::qtr_pi);
//	((CGraphic*)m_pGraphic)->UpdateCamera();
//	return Evn;
//}
//void SQRGUIManager::SetRenderEvn(const GUIRenderEvn &evn)
//{
//	CCamera* pCamera = CMainWindowTarget::GetInst()->GetCamera();
//	CLightSystem* pLight;
//	pLight = CMainWindowTarget::GetInst()->GetGroup()->GetLightSystem();
//	pLight->SetAmbient( evn.uAmbient,1 );
//	pCamera->SetPosition(evn.vecCameraPos);
//	pCamera->lookTo(evn.vecCameraDir);
//	pCamera->setFOVy(evn.fFovy);
//	((CGraphic*)m_pGraphic)->UpdateCamera();
//}
//GUIRenderEvn SQRGUIManager::GetRenderEvn()
//{
//	GUIRenderEvn Evn;
//	CLightSystem* pLight;
//	CCamera* pCamera = CMainWindowTarget::GetInst()->GetCamera();
//	pLight = CMainWindowTarget::GetInst()->GetGroup()->GetLightSystem();
//	Evn.uAmbient = pLight->GetAmbient(1);
//	Evn.vecCameraPos = pCamera->GetPosition();
//	Evn.vecCameraDir = pCamera->getDIRECTION();
//	Evn.fFovy = pCamera->getFOVy();
//	return Evn;
//}

void SQRGUIManager::Render()
{
	m_nTotalRect	= 0;
	m_nFlush		= 0;
	m_FontVec.clear();
	if( m_isShowWndArea ) 
		DrawArea( *m_pRootWnd );
	else 
		m_pRootWnd->Draw();

	if ( m_pModule ) 
		DrawModulate( *m_pModule );

	FlushDraw();

	if ( m_pLastMouseMsgWnd && !FindWnd( m_pLastMouseMsgWnd ) )
		m_pLastMouseMsgWnd = NULL;

	if ( (m_pMoreLastMouseMsgWnd != m_pLastMouseMsgWnd && m_pLastMouseMsgWnd)
		|| (m_bRefreshToolTip && m_pLastMouseMsgWnd) )
	{
		     // 清理
		if ( m_pMoreLastMouseMsgWnd )
		{
			if (m_pMoreLastMouseMsgWnd->GetRichToolTip())
				m_pMoreLastMouseMsgWnd->GetRichToolTip()->OnTipHide();
			else if (m_pMoreLastMouseMsgWnd->GetToolTipRichText())
				m_pMoreLastMouseMsgWnd->GetToolTipRichText()->ReleaseWndTex();
		}
		// 回调
		m_pMoreLastMouseMsgWnd = m_pLastMouseMsgWnd;
		m_bRefreshToolTip = false;
		m_pMoreLastMouseMsgWnd->PushTipData();
	}
	else if ( m_pMoreLastMouseMsgWnd != m_pLastMouseMsgWnd && m_pLastMouseMsgWnd == NULL )
	{
		if ( m_pMoreLastMouseMsgWnd )
		{
			if (m_pMoreLastMouseMsgWnd->GetRichToolTip())
				m_pMoreLastMouseMsgWnd->GetRichToolTip()->OnTipHide();
			else if (m_pMoreLastMouseMsgWnd->GetToolTipRichText())
				m_pMoreLastMouseMsgWnd->GetToolTipRichText()->ReleaseWndTex();
		}
		m_pMoreLastMouseMsgWnd = NULL;
	}
	
	uint32 tProTime = uint32(GetProcessTime());
	if ( m_pHeldWnd && tProTime - m_nPreUpdateTime > 100)
	{
		m_nPreUpdateTime = tProTime;
		if ( m_pLastMouseMsgWnd && m_pLastMouseMsgWnd->IsHeld() )
			m_pLastMouseMsgWnd->OnHeld( GetCursorPos().x, GetCursorPos().y );	
	}

	if ( m_pLastMouseMsgWnd && m_pLastMouseMsgWnd->IsShow() &&
	        tProTime - m_uPreMouseMsgTime > m_pLastMouseMsgWnd->OnPreShowMouseDesc() )
	{
		if ( !GetLockCursorMessage() 
			  &&( ( m_pLastMouseMsgWnd->m_hWnd->m_DescRichText 
			        && !m_pLastMouseMsgWnd->m_hWnd->m_DescRichText->m_szDescW.empty()
				  ) ||  m_pLastMouseMsgWnd->m_hWnd->m_pRichToolTip
				)
			)
		{
			m_pLastMouseMsgWnd->PrepareDefaultDesc();
			m_pLastMouseMsgWnd->DrawMouseOver();
		}
	}
	else if ( m_pLastMouseMsgWnd )
		m_pLastMouseMsgWnd->m_hWnd->m_posMouseOver = posInValid;

	FlushDraw();

	// render effects
	if(m_pIGUIHandler)
		m_pIGUIHandler->OnUpdateFx(tProTime, this->GetZoomSize());
	m_pRootWnd->DrawAfterAllEyeCandy();
	FlushDraw();
	EndFontFrame();
}

void SQRGUIManager::EndFontFrame()
{
	UISet<CRenderFont*>::iterator itr = m_FontVec.begin();
	UISet<CRenderFont*>::iterator eitr = m_FontVec.end();
	while(itr != eitr)
	{
		(*itr)->SetFrameFirst();
		++itr;
	}
}

void LoadItemDefaultPro( CBufFile& BufFile, WndResParam& WndParam )
{
	char	szBuf[4096];
	char*   szCurBuf = szBuf;
	int32	ClassLen;
	int32	WndTextLen;
	int32   TranTexNameLen;
	int32   nDescLen;
	int32   nUseClassLen;
	int32   nUseInfLen;
	int32   nClickFxLen;
	int32   nOpenFxLen;
	int32   nCloseFxLen;
	uint32	wsStyle;
	int32	x;
	int32	y;
	int32	w;
	int32	h;
	uint32	GroupID;
	int32	nFontID;
	bool	bBoldFont = false;
	int32	nFontSize;
	uint32  uLockFlag;
	uint32	fontGradualColor;
	uint32	fontEffectMask;
	bool	bisShowBack;
	uint32	StyleIndex;

	memset( szBuf, 0, 4096 );
	BufFile.read( (char*)&ClassLen, sizeof(int32) );
	BufFile.read( (char*)szCurBuf, ClassLen );
	szCurBuf += ClassLen + 1;
	BufFile.read( (char*)&WndTextLen, sizeof(int32) );
	BufFile.read( (char*)szCurBuf, WndTextLen );
	szCurBuf += WndTextLen + 1;
	BufFile.read( (char*)&wsStyle, sizeof(uint32) );
	BufFile.read( (char*)&x, sizeof(int32) );
	BufFile.read( (char*)&y, sizeof(int32) );
	BufFile.read( (char*)&w, sizeof(int32) );
	BufFile.read( (char*)&h, sizeof(int32) );
	BufFile.read( (char*)&GroupID, sizeof(uint32) );

	BufFile.read( (char*)&nFontID, sizeof(int32) );
	BufFile.read( (char*)&bBoldFont, sizeof(bool) );

	BufFile.read( (char*)&nFontSize, sizeof(int32) );

	BufFile.read( (char*)&fontGradualColor, sizeof(uint32)); 
	BufFile.read( (char*)&fontEffectMask, sizeof(uint32));
	BufFile.read( (char*)&bisShowBack, sizeof(bool));
	BufFile.read( (char*)&StyleIndex, sizeof(uint32));
	//读取透明纹理
	BufFile.read( (char*)&TranTexNameLen, sizeof(int32) );
	BufFile.read( (char*)szCurBuf, TranTexNameLen );
	szCurBuf += TranTexNameLen + 1;

	BufFile.read( (char*)&uLockFlag, sizeof(uint32) );

	BufFile.read( (char*)&nDescLen, sizeof(int32) );
	BufFile.read( (char*)szCurBuf, nDescLen );
	szCurBuf += nDescLen + 1;
	BufFile.read( (char*)&nUseClassLen, sizeof(int32) );
	BufFile.read( (char*)szCurBuf, nUseClassLen );
	szCurBuf += nUseClassLen + 1;
	BufFile.read( (char*)&nUseInfLen, sizeof(int32) );
	BufFile.read( (char*)szCurBuf, nUseInfLen );
	szCurBuf += nUseInfLen + 1;
	BufFile.read( (char*)&nClickFxLen, sizeof(int32) );
	BufFile.read( (char*)szCurBuf, nClickFxLen );
	szCurBuf += nClickFxLen + 1;
	BufFile.read( (char*)&nOpenFxLen, sizeof(int32) );
	BufFile.read( (char*)szCurBuf, nOpenFxLen );
	szCurBuf += nOpenFxLen + 1;
	BufFile.read( (char*)&nCloseFxLen, sizeof(int32) );
	BufFile.read( (char*)szCurBuf, nCloseFxLen );
	szCurBuf += nCloseFxLen + 1;

	int32 nMaxLen				= szCurBuf - szBuf;
	szCurBuf					= (char*)SqrNew(nMaxLen, SQRGuiMallocObject::GetPool());
	memcpy( szCurBuf, szBuf, nMaxLen );

	WndParam.m_uFontGradualColor	= fontGradualColor;
	WndParam.m_uFontEffectMask = fontEffectMask;

	WndParam.m_ClassName		= szCurBuf;
	szCurBuf += ClassLen + 1;
	WndParam.m_WindowTitle		= szCurBuf;
	szCurBuf += WndTextLen + 1;
	WndParam.m_WndStyle			= wsStyle;
	WndParam.m_x				= x;
	WndParam.m_y				= y;
	WndParam.m_Width			= w;
	WndParam.m_Height			= h;
	WndParam.m_GroupID			= GroupID;

	WndParam.m_nFontID			= nFontID;
	WndParam.m_bBold			= bBoldFont;
	WndParam.m_FontSize			= nFontSize;
	
	WndParam.m_uLockFlage		= uLockFlag;
	WndParam.m_isShowBackground = bisShowBack;
	WndParam.m_uStyleIndex		= StyleIndex;
	WndParam.m_TransTexture		= szCurBuf;
	szCurBuf += TranTexNameLen + 1;


	WndParam.m_szDesc			= szCurBuf;
	szCurBuf += nDescLen + 1;
	WndParam.m_UseClassName		= szCurBuf;
	szCurBuf += nUseClassLen + 1;
	WndParam.m_UseInfo			= szCurBuf;
	szCurBuf += nUseInfLen + 1;
	WndParam.m_MsgFx[eMF_Click]	= szCurBuf;
	szCurBuf += nClickFxLen + 1;
	WndParam.m_MsgFx[eMF_Open]	= szCurBuf;
	szCurBuf += nOpenFxLen + 1;
	WndParam.m_MsgFx[eMF_Close]	= szCurBuf;
	szCurBuf += nCloseFxLen + 1;

	int32 nChildCount = 0;
	BufFile.read( (char*)&nChildCount, sizeof(int32) );
	WndParam.m_ImageList.resize( nChildCount );
	for ( int32 nCtrlMask = 1; nCtrlMask < nChildCount + 1; nCtrlMask++ )
	{
		ChildWndInfo& Info = WndParam.m_ImageList[ nCtrlMask - 1 ];
		if ( nCtrlMask > 1 )
		{
			int32 x,y,w,h;
			uint32 ChildStyle;
			uint32 LockFlag;
			BufFile.read( (char*)&x, sizeof(int32) );
			BufFile.read( (char*)&y, sizeof(int32) );
			BufFile.read( (char*)&w, sizeof(int32) );
			BufFile.read( (char*)&h, sizeof(int32) );
			BufFile.read( (char*)&ChildStyle, sizeof(uint32) );
			BufFile.read( (char*)&LockFlag, sizeof(uint32));
			Info.rtWnd = CFRect( float(x), float(y), float(x + w), float(y + h) );
			Info.m_uStyleIndex = ChildStyle;
			Info.m_uLockFlag = LockFlag;
		}

		int32 nImageListCount = 0;
		BufFile.read( (char*)&nImageListCount, sizeof(int32) );
		Info.ChildBkList.resize( nImageListCount );
		for ( int32 nState = 0; nState < nImageListCount; nState++ )
		{
			int32 nImageCount;
			uint32 uStateColor;
			int32 nStateMask;

			uint32 uRimColor;

			BufFile.read( (char*)&nStateMask, sizeof(int32) );
			BufFile.read( (char*)&uStateColor, sizeof(uint32) );
			BufFile.read( (char*)&uRimColor, sizeof(uint32) );
			BufFile.read( (char*)&nImageCount, sizeof(int32) );

			Info.ChildBkList[ nState ].nStateMask = nStateMask;
			Info.ChildBkList[ nState ].uTextColor = uStateColor;
			Info.ChildBkList[ nState ].uRimColor = uRimColor;
			Info.ChildBkList[ nState ].ImageList.resize( nImageCount );
			for ( int32 i = 0; i < nImageCount; i++ )
			{
				char TextName[256];
				memset( TextName, 0, 256 );
				int32  Len;
				CFRect rt;
				CFPos Pos;
				uint32 dwColor;
				uint32 dwLockFlag;
				BufFile.read( (char*)&Len, sizeof(int32) );
				BufFile.read( TextName, Len );
				BufFile.read( (char*)&rt, sizeof( CFRect ) );
				BufFile.read( (char*)&Pos, sizeof( CFPos ) );
				BufFile.read( (char*)&dwColor, sizeof( uint32 ) );
				BufFile.read( (char*)&dwLockFlag, sizeof( uint32 ) );

				UIString strTexName;
				strTexName += TextName;

				Info.ChildBkList[ nState ].ImageList[i].szTexture = strTexName; // TextName;
				Info.ChildBkList[ nState ].ImageList[i].rtScale = rt;
				Info.ChildBkList[ nState ].ImageList[i].posWnd = Pos;
				Info.ChildBkList[ nState ].ImageList[i].dwColor = dwColor;
				Info.ChildBkList[ nState ].ImageList[i].dwLockFlage = dwLockFlag;
			}
		}
	}
}

void LoadItemSoundPro( CBufFile& BufFile, WndResParam& WndParam )
{
	for( int i = 0; i < eMS_Count; ++i )
	{
		int32 nLen;
		char szSoundName[512];
		memset( szSoundName, 0, 512 );
		BufFile.read( (char*)&nLen, sizeof(int32) );
		BufFile.read( szSoundName, nLen );

		WndParam.m_MsgSound[i] = szSoundName;
	}
}

void LoadItemFlashInfo( CBufFile& BufFile, WndResParam& WndParam )
{
	/*for ( int i = 0; i < 4; ++i )
	{
		int32 nLen;
		char szFlsName[512];
		memset( szFlsName, 0, 512 );
		BufFile.read( (char*)&nLen, sizeof(int32) );
		BufFile.read( szFlsName, nLen );
	}*/
	///load flash property
	// 数量
	int32 fls_ct;
	BufFile.read( (char*)&fls_ct, sizeof(int32) );
	for ( int32 i = 0; i < fls_ct; ++i)
	{
		// 状态
		int32 fls_st;
		BufFile.read( (char*)&fls_st, sizeof(int32) );
		// 存储内容
		int32 nLen;
		char szFlsName[512];
		memset( szFlsName, 0, 512 );
		BufFile.read( (char*)&nLen, sizeof(int32) );
		BufFile.read( szFlsName, nLen ); 

		UIString flsname = szFlsName;
		EventStateMask eStFls = (EventStateMask)fls_st;
		WndParam.m_FlashWndStr.insert(pair<EventStateMask, UIString>(eStFls, flsname));
	}
}

void LoadItem( CBufFile& BufFile, WndResParam& WndParam )
{
	LoadItemSoundPro(BufFile, WndParam);
	LoadItemFlashInfo( BufFile, WndParam );
	LoadItemDefaultPro(BufFile, WndParam);
}

static void OnReadAsyn( void* pBaseRead, CRefBufFile& BufFile )
{
	WndResParam& MainWndParam = *(WndResParam*)pBaseRead;
	if ( MainWndParam.m_ClassName )
		return;

	SQR_TRY
	{
		LoadItem( BufFile, MainWndParam );

		int32 nChildCount;
		BufFile.read( (void*)&nChildCount, sizeof(int32) );
		for ( int32 i = 0; i < nChildCount; i++ )
		{
			int32 Len;
			char szChildName[256];
			memset( szChildName, 0, 256 );
			BufFile.read( (char*)&Len, sizeof(int32) );
			BufFile.read( szChildName, Len );
			UIMap<UIString, WndResParam*>::iterator it = MainWndParam.m_Child.find(szChildName);
			if (it!=MainWndParam.m_Child.end())
				delete it->second;
			WndResParam& ChildWndParam = *( MainWndParam.m_Child[szChildName] = new WndResParam );
			LoadItem( BufFile, ChildWndParam );
		}
	}
	SQR_CATCH(exp)
	{
		GfkLogPkgErr(exp, BufFile.GetFileName(), BufFile);
	}
	SQR_TRY_END
}


static void OnReadXml( string& szFileName, void* pBaseRead )
{
	WndResParam& MainWndParam = *(WndResParam*)pBaseRead;
	if ( MainWndParam.m_ClassName )
		return;

	ipkgstream strmLayout;
	if (strmLayout.open(GUI_PATH_ALIAS_GUI.c_str(), szFileName.c_str()))
	{
		GenErr("UI Manager error", "Can't open Layout file");
		return;
	}

	// read XML here
	TiXmlDocument		LayoutDoc;
	strmLayout >> LayoutDoc;
	strmLayout.close();

	SQRGUIManager* pGUI = static_cast<SQRGUIManager*>(SQRGUIManager::GetInst());
	if ( pGUI )
	{
		if ( pGUI->GetXMLParser() && pGUI->GetLayoutHandler() )
		{
			pGUI->GetLayoutHandler()->SetRootWndResParam(&MainWndParam);
			pGUI->GetXMLParser()->ParseXMLFile( *( pGUI->GetLayoutHandler() ), LayoutDoc );
		}
	}
}

//初始化资源窗口
void SQRGUIManager::LoadResMap()
{
	if (m_beInitedRes)
	{
		if ( m_sFileName.empty() )
			return;

		if ( m_beGUITool )
		{
			// 优先读XML
			if ( LoadResFromXml() )
				return;
			else
				LoadResFromCoder();
		}
		else
		{
			// 优先读AUC
#if 1
			if ( LoadResFromCoder() )
				return;
#endif
#if 0
			if ( LoadResFromAuc() )
				return;
#endif
			else
				LoadResFromXml();

		}
	}
}

//从AUC读资源
bool SQRGUIManager::LoadResFromAuc()
{
	printf("读取GUI资源开始...");
	m_beAucRes = true;
	CPkgDir pkg;
	bool bAsyn = CPkgFile::IsEnableAsyncRead();
	CPkgFile::EnableAsyncRead(false);
	pkg.Open(GUI_PATH_ALIAS_GUI.c_str(), m_sFileName.c_str(),"auc");

	size_t co = pkg.GetFileNum();
	if (co == 0)
	{
		return false;
	}

	CTimeCheckPoint tcp;

	for(size_t i = 0; i < pkg.GetFileNum(); i++)
	{
		string szFileName;
		string szChildName;
		pkg.GetFullFileName(i,szFileName);

		szChildName = szFileName;
		size_t pos = szChildName.find(".auc");
		if (pos == string::npos)
		{
			continue;
		}
		szChildName = szChildName.substr(0,pos);


		pos = szChildName.find_last_of('.');
		if (pos == string::npos)
		{
			continue;
		}
		szChildName = szChildName.substr(pos + 1, szChildName.length() - pos - 1);

		tolower(szChildName);
		ResMap::iterator it = m_ResTemplate.find(szChildName.c_str());
		if (it!=m_ResTemplate.end())
			delete it->second;
		WndResParam& MainWndParam = *( m_ResTemplate[szChildName.c_str()] = new WndResParam );
		MainWndParam.m_beAucRes = true;

		// 得到了窗口的名字
		SQR_TRY
		{
			CPkgFile::ReadWholeFile( GUI_PATH_ALIAS_GUI.c_str(), szFileName.c_str(), &MainWndParam, OnReadAsyn );
		}
		SQR_CATCH(exp)
		{
			string errmsg = exp.ErrorMsg();
			errmsg = errmsg + szFileName;
			GenErr(exp.ErrorTitle(),errmsg);
		}
		SQR_TRY_END;
	}

	printf("结束:花费%d毫秒\n",tcp.GetElapse());

	CPkgFile::EnableAsyncRead(bAsyn);

	return true;
}
//从XML读资源
bool SQRGUIManager::LoadResFromXml()
{
	m_beAucRes = FALSE;
	UIString sFileXmlName = m_sFileName;
	sFileXmlName.erase( sFileXmlName.length() - 10, 10 );
	sFileXmlName += "src\\Layout";

	CPkgDir pkg;
	bool bAsyn = CPkgFile::IsEnableAsyncRead();
	CPkgFile::EnableAsyncRead(false);

	pkg.Open(GUI_PATH_ALIAS_GUI.c_str(), sFileXmlName.c_str(),"xml");

	size_t co = pkg.GetFileNum();
	if (co == 0)
	{
		return false;
	}

	for(size_t i = 0; i < co; ++i)
	{
		string szFileName;
		string szChildName;
		pkg.GetFullFileName(i,szFileName);

		szChildName = szFileName;
		size_t pos = szChildName.find(".xml");
		if (pos == string::npos)
		{
			continue;
		}
		szChildName = szChildName.substr(0,pos);

		pos = szChildName.find("ybtx.");
		if (pos == string::npos)
		{
			continue;
		}
		else
		{
			pos = pos + 4;
		}
		szChildName = szChildName.substr(pos + 1, szChildName.length() - pos - 1);

		ResMap::iterator it = m_ResTemplate.find(szChildName.c_str());
		if (it!=m_ResTemplate.end())
			delete it->second;
		WndResParam& MainWndParam = *( m_ResTemplate[szChildName.c_str()] = new WndResParam );
		MainWndParam.m_beAucRes = false;

		// 得到了窗口的名字
		OnReadXml( szFileName, &MainWndParam );
	}
	CPkgFile::EnableAsyncRead(bAsyn);

	return true;
}


//从XML读资源
bool SQRGUIManager::LoadResFromCoder()
{
	printf("读取GUI资源开始...");
	m_beAucRes = true;
	CPkgDir pkg;
	bool bAsyn = CPkgFile::IsEnableAsyncRead();
	CPkgFile::EnableAsyncRead(false);
	pkg.Open(GUI_PATH_ALIAS_GUI.c_str(), m_sFileName.c_str(),"aud");

	size_t co = pkg.GetFileNum();
	if (co == 0)
		return false;

	CTimeCheckPoint tcp;

	for(size_t i = 0; i < co; i++)
	{
		string szFileName;
		string szChildName;
		pkg.GetFullFileName(i,szFileName);

		szChildName = szFileName;
		size_t pos = szChildName.find(".aud");
		if (pos == string::npos)
			continue;
		szChildName = szChildName.substr(0,pos);

		pos = szChildName.find_last_of("/");
		if (pos == string::npos)
			continue;
		szChildName = szChildName.substr(pos + 1, szChildName.length() - pos - 1);

		if (!m_beGUITool)
			tolower(szChildName);
		ResMap::iterator it = m_ResTemplate.find(szChildName.c_str());
		if (it!=m_ResTemplate.end())
			delete it->second;
		WndResParam& MainWndParam = *( m_ResTemplate[szChildName.c_str()] = new WndResParam );
		MainWndParam.m_beAucRes = true;

		SQR_TRY
		{
			CGuiFormat gui_corder;
			gui_corder.RegisterCoder(new CGUIResCoder);
			gui_corder.Load(GUI_PATH_ALIAS_GUI.c_str(), szFileName.c_str(), &MainWndParam);
		}
		SQR_CATCH(exp)
		{
			string errmsg = exp.ErrorMsg();
			errmsg = errmsg + szFileName;
			GenErr(exp.ErrorTitle(),errmsg);
		}
		SQR_TRY_END;
	}

	CPkgFile::EnableAsyncRead(bAsyn);
	printf("结束:花费%d毫秒\n",tcp.GetElapse());
	return true;
}

void SQRGUIManager::LoadResStaticText()
{
	UIString szCurLan = CFontMgr::GetInst()->GetLangFolder();
	UIString szStaticTextFile = "GUIStaticText.xml";
	UIString strConfigPath = szCurLan + "\\gui\\" + szStaticTextFile;
	sqr::ipkgstream strmStaticText(PATH_ALIAS_LAN.c_str(), strConfigPath.c_str());
	TiXmlDocument doc;
	strmStaticText>>doc;
	strmStaticText.close();

	TiXmlNode* pRoot = doc.FirstChild("root");
	m_ResStaticInfoContainer.clear();
	for( TiXmlNode* pNode = pRoot->FirstChild(); pNode; pNode=pNode->NextSibling())
	{
		TiXmlElement* pElement = pNode->ToElement();
		if (!pElement)
			continue;

		const string sName = pElement->ValueStr();
		UIString sTitle	= "";
		UIString sDesc	= "";
		for( TiXmlAttribute* pA = pElement->FirstAttribute(); pA; pA = pA->Next() )
		{
			if ( "title" == string( pA->Name() ) )
				sTitle	= pA->Value();
			else if( "desc" == string( pA->Name() ) )
				sDesc	= pA->Value();
		}
		
		UIMap<const UIString, ChildResInfo> resChildInfo;
		for ( TiXmlNode* pSubNode=pNode->FirstChild(); pSubNode; pSubNode=pSubNode->NextSibling() )
		{
			TiXmlElement* pSubElemet = pSubNode->ToElement();
			if (!pSubElemet)
				continue;

			const string sChildName = pSubElemet->ValueStr();
			UIString sChildTitle = "";
			UIString sChildDesc	= "";
			for( TiXmlAttribute* pA = pSubElemet->FirstAttribute(); pA; pA = pA->Next() )
			{
				if ( "title" == string( pA->Name() ) )
					sChildTitle	= pA->Value();
				else if( "desc" == string( pA->Name() ) )
					sChildDesc	= pA->Value();
			}
			
			ChildResInfo childResInfo(sChildTitle, sChildDesc);
			resChildInfo.insert( pair<const UIString, ChildResInfo>(sChildName.c_str(), childResInfo) );
		}

		ResInfo resInfo( sTitle, sDesc, resChildInfo );
		m_ResStaticInfoContainer.insert( pair<const UIString, ResInfo>(sName.c_str(), resInfo) );
	}
}

bool SQRGUIManager::ProcessStaticText(UIVector<UIString>& vec, string& txt)
{
	string temp_str = txt;
	string temp_head;
	while (temp_str.length() > 0)
	{
		int temp_pos = temp_str.find_first_of("#");
		if(temp_pos == string::npos || temp_pos == 0)
			break;
		temp_head = temp_str.substr(0,temp_pos);
		temp_str = temp_str.substr(temp_pos + 1, temp_str.length() - temp_pos - 1);

		UIString STR = temp_head.c_str();
		vec.push_back(STR);
	}
	if (temp_head == "title")
		return true;
	return false;
}

void SQRGUIManager::LoadResTxt(UIMap<UIString,UIString>& txtmap, const wchar_t* pkg, const char* file)
{
	sqr::ipkgstream strmStaticText;
	if ( strmStaticText.open(pkg, file) )
		return;
	UIString IDX = "", VAL = "";
	char tempchar;
	bool beIdx = true;
	string tempstr = "";

	while (strmStaticText.peek() != EOF)
	{
		strmStaticText.read((char*)(&tempchar), 1);
		if (tempchar == '\n')	// 换行
		{
			if ( beIdx == false )		// 读过tab
				VAL = tempstr.c_str();
			if (IDX.length() != 0 && VAL.length() != 0)
				txtmap.insert(pair<UIString, UIString>(IDX.c_str(), VAL.c_str()));
			IDX = ""; VAL = "";
			tempstr = "";
			beIdx = true;		// 读完一行
		}
		else if (tempchar == '\t')	// tab
		{
			if (beIdx)
			{
				IDX = tempstr.c_str();
				beIdx = false;
				tempstr = "";
			}
		}
		else if (tempchar != 0x0D)
				tempstr.append(1,tempchar);
	}

	strmStaticText.close();
}

void SQRGUIManager::LoadResStaticTxt()
{
	UIString szOriLan = CFontMgr::GetInst()->GetOriLangFolder();

	UIString szCurLan = CFontMgr::GetInst()->GetLangFolder();
	UIString szStaticTextFile = "GUIStaticText.txt";
	UIString strConfigPath = szCurLan + "\\gui\\" + szStaticTextFile;
	m_ResStaticInfoContainer.clear();

	UIMap<UIString,UIString> CurLanTxtMap;
	LoadResTxt( CurLanTxtMap, PATH_ALIAS_LAN.c_str(), strConfigPath.c_str() );

	UIMap<UIString,UIString> OriTxtMap;
	UIString strOriConfigPath = szOriLan + "\\gui\\" + szStaticTextFile;
	LoadResTxt( OriTxtMap, PATH_ALIAS_LAN.c_str(), strOriConfigPath.c_str() );

	//UIMap<UIString,UIString>::iterator itr = CurLanTxtMap.begin();
	//UIMap<UIString,UIString>::iterator itrend = CurLanTxtMap.end();
	UIMap<UIString,UIString>::iterator itr = OriTxtMap.begin();
	UIMap<UIString,UIString>::iterator itrend = OriTxtMap.end();

	while (itr != itrend)
	{
		string strGUIidx, strGUItxt;
		strGUIidx =  itr->first.c_str();
		//strGUItxt = itr->second.c_str();

		if (strGUIidx.empty())
			continue;

		UIMap<UIString,UIString>::iterator itrCurLan = CurLanTxtMap.find(itr->first);
		if(itrCurLan != CurLanTxtMap.end())
		{
			strGUItxt = itrCurLan->second.c_str();
		}
		else
		{
			strGUItxt = itr->second.c_str();
		}

		if (strGUItxt.empty())
			continue;
		
		UIVector<UIString> vec;
		bool beTile = ProcessStaticText(vec, strGUIidx);
		
		uint32 ct = 0;
		if (ct < vec.size() - 1 && vec.size() != 0)
		{
			// have one main win
			UIString sWin = vec[ct];
			UIString sTitle = "" , sDesc	= "";

			MapResStaticInfo::iterator MapItr = m_ResStaticInfoContainer.find(sWin);
			if (MapItr == m_ResStaticInfoContainer.end())
			{
				UIMap<const UIString, ChildResInfo> resChildInfo;
				ResInfo resInfo( sTitle, sDesc, resChildInfo );
				m_ResStaticInfoContainer.insert( pair<const UIString, ResInfo>(sWin.c_str(), resInfo) );
				MapItr = m_ResStaticInfoContainer.find(sWin);
			}
			++ct;
			if (ct < vec.size() - 1)
			{
				UIString sCWin = vec[ct];
				UIMap<const UIString, ChildResInfo>& ChildInfo = MapItr->second.mapChildInfo;
				UIMap<const UIString, ChildResInfo>::iterator CMapItr = ChildInfo.find(sCWin);
				if (CMapItr == ChildInfo.end())
				{
					ChildResInfo childResInfo(sTitle, sDesc);
					ChildInfo.insert( pair<const UIString, ChildResInfo>(sCWin.c_str(), childResInfo) );
					CMapItr = ChildInfo.find(sCWin);
				}
				if (beTile)
					CMapItr->second.szTitle = strGUItxt.c_str();
				else
					CMapItr->second.szDesc = strGUItxt.c_str();
			}
			else		//  是主窗体的文字信息
			{
				if (beTile)
					MapItr->second.szTitle = strGUItxt.c_str();
				else
					MapItr->second.szDesc = strGUItxt.c_str();

			}
		}
		++itr;
	}
}

//得到窗口资源
WndResParam* SQRGUIManager::GetWndRes( const char* szWindowNameInResFile )
{
	std::string WinName = szWindowNameInResFile;
	if ( m_beAucRes )
		tolower(WinName);

	UIMap<UIString, WndResParam*>::iterator it = m_ResTemplate.find( WinName.c_str() );
	if ( it == m_ResTemplate.end() )
	{
		if (!m_beInitedRes)
		{
			WndResParam& MainWndParam = *( m_ResTemplate[WinName.c_str()] = new WndResParam );
			MainWndParam.m_beAucRes = true;
			it = m_ResTemplate.find( WinName.c_str() );
		}
		else
			return NULL;
	}

	if ( it->second->m_ClassName )
		return it->second;

	if (m_beAucRes)
	{
		// 读取AUC资源
#if 0
		string szFileName = m_sFileName.c_str();
		szFileName += "/ybtx.";
		szFileName += szWindowNameInResFile;
		szFileName += ".auc";

		bool bAsyn = CPkgFile::IsEnableAsyncRead();
		CPkgFile::EnableAsyncRead(false);
		// 得到了窗口的名字
		SQR_TRY
		{
			CPkgFile::ReadWholeFile( GUI_PATH_ALIAS_GUI.c_str(), szFileName.c_str(), &(*(it->second)), OnReadAsyn );
		}
		SQR_CATCH(exp)
		{
			string errmsg = exp.ErrorMsg();
			errmsg = errmsg + szFileName;
			GenErr(exp.ErrorTitle(),errmsg);
		}
		SQR_TRY_END;
		CPkgFile::EnableAsyncRead(bAsyn);
#endif
#if 1
		string szFileName = m_sFileName.c_str();
		szFileName += "/";
		szFileName += szWindowNameInResFile;
		szFileName += ".aud";

		bool bAsyn = CPkgFile::IsEnableAsyncRead();
		CPkgFile::EnableAsyncRead(false);
		SQR_TRY
		{
			CGuiFormat gui_corder;
			gui_corder.RegisterCoder(new CGUIResCoder);
			gui_corder.Load(GUI_PATH_ALIAS_GUI.c_str(), szFileName.c_str(), &(*(it->second)));
		}
		SQR_CATCH(exp)
		{
			string errmsg = exp.ErrorMsg();
			errmsg = errmsg + szFileName;
			GenErr(exp.ErrorTitle(),errmsg);
		}
		SQR_TRY_END;
		CPkgFile::EnableAsyncRead(bAsyn);
#endif
	}
	else
	{
		// 读取XML逻辑
		string LayoutFileName = m_sFileName.c_str();
		LayoutFileName.erase( LayoutFileName.length() - 16, 16 );
		LayoutFileName += "src\\Layout\\ybtx.";
		LayoutFileName += szWindowNameInResFile;
		LayoutFileName += ".xml";

		bool bAsyn = CPkgFile::IsEnableAsyncRead();
		CPkgFile::EnableAsyncRead(false);
		OnReadXml( LayoutFileName, it->second );
		CPkgFile::EnableAsyncRead(bAsyn);
	}
	
	return it->second;
}

//从资源创建窗口背景,不创建资源
WND_IMAGE_LIST SQRGUIManager::LoadBKRes( UIVector<WindowImage>& Res )
{
	WND_IMAGE_LIST Image;
	Image.Release();

	for ( int32 i = 0; i < (int32)Res.size(); i++ )
	{
		CFRect rt( Res[i].rtScale );
#ifdef TEXTURE_STINT_MODE
		if ( false && m_beGUITool && Res[i].szTexture.c_str() && (Res[i].szTexture.c_str())[0] != 0)
		{
			string ChopString("..\\resbin\\");
			string strtex = Res[i].szTexture.c_str();
			int i = strtex.find( ChopString );
			if ( i != string::npos )
			{
				strtex = strtex.substr(10,strtex.length()-10);
			}
			Image.AddImageWin( m_pGraphic, -1, strtex.c_str(), &rt, Res[i].posWnd, Res[i].dwColor, Res[i].dwLockFlage );
		}
		else
		{
			Image.AddImageWin( m_pGraphic, -1, (Res[i].szTexture).c_str(), &rt, Res[i].posWnd, Res[i].dwColor, Res[i].dwLockFlage );
		}

#else
		Image.AddImage( m_pGraphic, -1, Res[i].szTexture.c_str(), &rt, Res[i].posWnd, Res[i].dwColor, Res[i].dwLockFlage );
#endif
		
	}

	return Image;
}

HCURSOR SQRGUIManager::LoadCursorRes( const char* sCurFile )
{
	char szBuf[256];
	const char* szCursorFile = sCurFile;
	if ( sCurFile < (char*)65536 )
	{
		sprintf( szBuf, "%d", sCurFile );
		szCursorFile = szBuf;
	}

	UIMap<UIString, HCURSOR>::iterator it = m_CursorRes.find( szCursorFile );
	if ( it == m_CursorRes.end() )
	{
		HCURSOR hCursor = sCurFile < (char*)65536 ? LoadCursor( NULL, sCurFile ) : LoadCursorFromFile( sCurFile );
		if ( hCursor )
		{
			m_CursorRes[szCursorFile] = hCursor;
			m_Cursor2Name[hCursor] = szCursorFile;
		}
		return hCursor;
	}

	return it->second;
}


//得到光标字符串
const char* SQRGUIManager::GetCursorFileName( HCURSOR hCursor )
{
	UIMap<HCURSOR, UIString>::iterator it = m_Cursor2Name.find( hCursor );
	return it == m_Cursor2Name.end() ? NULL : m_Cursor2Name[hCursor].c_str();
}
//更新光标
void SQRGUIManager::UpdateCursor(SQRWnd &Wnd)
{
	SQRGUIManager* pGUI = Wnd.m_hWnd->m_pGUI;
	pGUI->m_PreCurSet = &Wnd;
	if ( pGUI->m_PreCurSet->m_hWnd->m_hCursor )
		SetCursor( pGUI->m_PreCurSet->m_hWnd->m_hCursor );
	else
		SetCursor( m_hGlobCursor );

}

void SQRGUIManager::RegisterHook( IHook* pHook, uint32 uWndMsg )
{
	if ( pHook == NULL )
		return;
	/*HookMap::iterator it = m_Hook.find(pHook);
	if(!m_Hook.empty() && it == m_Hook.end())
	{
		GenErr("RegisterHook错误,Hook只能有一个");
	}*/
	m_Hook[pHook].push_back( uWndMsg );
}

void SQRGUIManager::UnRegisterHook( IHook* pHook )
{
	if ( pHook )
		m_Hook.erase( pHook );
}

void SQRGUIManager::RegisterAccelerator( IAccelerator* pAccelerator, uint32 uWndMsg )
{
	if ( pAccelerator == NULL )
		return;
	AcceleratorMap::iterator it = m_Accelerator.find(pAccelerator);
	if(!m_Accelerator.empty() && it == m_Accelerator.end())
	{
		GenErr("UI Manager error", "Register Accelerator只能有一个");
	}
	m_Accelerator[pAccelerator].push_back( uWndMsg );
}

void SQRGUIManager::UnRegisterAccelerator( IAccelerator* pAccelerator )
{
	if ( pAccelerator )
		m_Accelerator.erase( pAccelerator );
}

void SQRGUIManager::AddHookMsg( IHook* pHook, uint32 uWndMsg )
{
	if ( m_Hook.find( pHook ) != m_Hook.end() )
		m_Hook[pHook].push_back( uWndMsg );
}
//播放特效
void SQRGUIManager::PlayFx( SQRWnd* pWnd, const char* szFx, EEffectRun eEffrun )
{
	if ( m_pIGUIHandler )
		m_pIGUIHandler->OnPlayFx( pWnd, szFx, eEffrun );
}

//播放特效
void* SQRGUIManager::PlayFxIme( SQRWnd* pWnd, const char* szFx, EEffectRun eEffrun )
{
	void* ptr = NULL;
	if ( m_pIGUIHandler )
		ptr = m_pIGUIHandler->OnPlayFxIme( pWnd, szFx, eEffrun );
	return ptr;
}

//播放特效
void SQRGUIManager::PlayFxEx( SQRWnd* pWnd, const char* szFxFile, const char* szFx, EEffectRun eEffrun )
{
	if ( m_pIGUIHandler )
		m_pIGUIHandler->OnPlayFxEx( pWnd, szFxFile, szFx, eEffrun );
}

//立即播放特效
void* SQRGUIManager::PlayFxImeEx( SQRWnd* pWnd, const char* szFxFile, const char* szFx, EEffectRun eEffrun )
{
	void* ptr = NULL;
	if ( m_pIGUIHandler )
		ptr = m_pIGUIHandler->OnPlayFxImeEx( pWnd, szFxFile, szFx, eEffrun );
	return ptr;
}

//删除特效
void SQRGUIManager::ClearFx( SQRWnd* pWnd, const char* szFx )
{
	if ( m_pIGUIHandler )
		m_pIGUIHandler->OnClearFx( pWnd, szFx );
}

//删除特效ALL
void SQRGUIManager::ClearFx( SQRWnd* pWnd)
{
	if ( m_pIGUIHandler )
		m_pIGUIHandler->OnClearFx( pWnd );
}

//删除特效
void SQRGUIManager::ClearImeFx( SQRWnd* pWnd, const char* szFx )
{
	if ( m_pIGUIHandler )
		m_pIGUIHandler->OnClearImeFx( pWnd, szFx );
}

//刷新特效
bool SQRGUIManager::DrawImeFx( SQRWnd* pWnd, const char* szFx, uint curTime, float zoom )
{
	bool succ = false;
	if ( m_pIGUIHandler )
	{
		succ = m_pIGUIHandler->OnRenderFx( make_pair(pWnd, (UIString)(szFx)), curTime, zoom );
	}
	return succ;
}

void SQRGUIManager::OnTipHandlerDel(SQRWnd* wnd)
{
	if (m_pMoreLastMouseMsgWnd == wnd)
	{
		m_pMoreLastMouseMsgWnd = NULL;
	}
}


//关闭、启用输入法
void SQRGUIManager::EnableIme( bool bEnable )
{
	if ( m_pIGUIHandler )
		m_pIGUIHandler->OnEnableIme( bEnable );
}
//////////////////////////////////////////////////////////////////////////

//画矩形
void SQRGUIManager::DrawRect( const CFRect& rtr, uint32 uColor, ITexture* pTexture, const CFRect* rtText, ERenderStyle rs, float z, FLOAT *pOffset )
{
	if ( (uColor& 0xFF000000) == 0 )
		return;

	CFRect rt = rtr;
	rt.top		+= m_pRootWnd->m_hWnd->m_yScreen;
	rt.bottom	+= m_pRootWnd->m_hWnd->m_yScreen;
	rt.left		+= m_pRootWnd->m_hWnd->m_xScreen;
	rt.right	+= m_pRootWnd->m_hWnd->m_xScreen;

	if ( m_nCurTextureBufPos > 0 && ( m_pTextue != pTexture || m_RS != rs ) )
	{
		FlushRect();
	}
	if ( pTexture )
		pTexture->AddRef();
	SafeRelease( m_pTextue );

	if ( m_nCurTextureBufPos >= m_TextureBuf.size() )
		m_TextureBuf.push_back( RectGroup() );

	m_pTextue									= pTexture;
	m_RS										= rs;
	m_TextureBuf[m_nCurTextureBufPos].bFullText	= rtText ? false : true;
	m_TextureBuf[m_nCurTextureBufPos].Color		= uColor;
	m_TextureBuf[m_nCurTextureBufPos].z			= z;
	if ( NULL == pOffset )
	{
		m_TextureBuf[m_nCurTextureBufPos].vec[0].Init( rt.left,  rt.bottom );
		m_TextureBuf[m_nCurTextureBufPos].vec[1].Init( rt.left,  rt.top );
		m_TextureBuf[m_nCurTextureBufPos].vec[2].Init( rt.right, rt.bottom );
		m_TextureBuf[m_nCurTextureBufPos].vec[3].Init( rt.right, rt.top );
	}
	else
	{
		if ( 0.0f != pOffset[5] )
		{
			CMatrix pos0;
			CMatrix pos1;
			CMatrix pos2;
			CMatrix pos3;
			CMatrix angle;
			// D3DXMatrixRotationZ( &angle, pOffset[5] );
			angle.SetRotateZ(pOffset[5]);

			CVector3f vPos;
			vPos.x = rt.left + (rt.right  - rt.left) / 2;
			vPos.y = rt.bottom;//rt.top  + (rt.bottom - rt.top) / 2;
			vPos.z = 0.0f;

			pos0.SetTranslate( rt.left, rt.bottom, 0.0f );
			pos0._41 -= vPos.x;
			pos0._42 -= vPos.y;
			pos0 = pos0 * angle;
			pos0._41 += vPos.x;
			pos0._42 += vPos.y;

			pos1.SetTranslate( rt.left, rt.top, 0.0f );
			pos1._41 -= vPos.x;
			pos1._42 -= vPos.y;
			pos1 = pos1 * angle;
			pos1._41 += vPos.x;
			pos1._42 += vPos.y;

			pos2.SetTranslate( rt.right, rt.bottom, 0.0f );
			pos2._41 -= vPos.x;
			pos2._42 -= vPos.y;
			pos2 = pos2 * angle;
			pos2._41 += vPos.x;
			pos2._42 += vPos.y;

			pos3.SetTranslate( rt.right, rt.top, 0.0f );
			pos3._41 -= vPos.x;
			pos3._42 -= vPos.y;
			pos3 = pos3 * angle;
			pos3._41 += vPos.x;
			pos3._42 += vPos.y;

			m_TextureBuf[m_nCurTextureBufPos].vec[0].Init( (pos0._41), (pos0._42) );
			m_TextureBuf[m_nCurTextureBufPos].vec[1].Init( (pos1._41), (pos1._42) );
			m_TextureBuf[m_nCurTextureBufPos].vec[2].Init( (pos2._41), (pos2._42) );
			m_TextureBuf[m_nCurTextureBufPos].vec[3].Init( (pos3._41), (pos3._42) );
		}
		else if ( -1.0f != pOffset[6] )
		{
			switch ( (int32) pOffset[6] )
			{
			case 0:
				if ( pOffset[1] > 0.0f)
				{
					m_TextureBuf[m_nCurTextureBufPos].vec[2].Init( rt.right, ( rt.top + pOffset[1] ) );
					m_TextureBuf[m_nCurTextureBufPos].vec[3].Init( rt.right, rt.top );
				}
				else
				{
					m_TextureBuf[m_nCurTextureBufPos].vec[2].Init( ( rt.left + pOffset[0] ),  rt.top );
					m_TextureBuf[m_nCurTextureBufPos].vec[2].Init( ( rt.left + pOffset[0] ), rt.top );
				}
				m_TextureBuf[m_nCurTextureBufPos].vec[0].Init( rt.left,  rt.bottom );
				m_TextureBuf[m_nCurTextureBufPos].vec[1].Init( rt.left, rt.top );
				break;
			case 1:
				if ( pOffset[1] > 0.0f)
				{
					m_TextureBuf[m_nCurTextureBufPos].vec[0].Init( ( rt.right - pOffset[1] ), rt.bottom );
					m_TextureBuf[m_nCurTextureBufPos].vec[2].Init( rt.right, rt.bottom );
				}
				else
				{
					m_TextureBuf[m_nCurTextureBufPos].vec[0].Init( rt.right, ( rt.top + pOffset[0] ) );
					m_TextureBuf[m_nCurTextureBufPos].vec[2].Init( rt.right, ( rt.top + pOffset[0] ) );
				}
				m_TextureBuf[m_nCurTextureBufPos].vec[3].Init( rt.right,  rt.top );
				m_TextureBuf[m_nCurTextureBufPos].vec[1].Init( rt.left,  rt.top );
				break;
			case 2:
				if ( pOffset[1] > 0.0f)
				{
					m_TextureBuf[m_nCurTextureBufPos].vec[0].Init( rt.left, rt.bottom );
					m_TextureBuf[m_nCurTextureBufPos].vec[1].Init( rt.left, ( rt.bottom - pOffset[1] ) );
				}
				else
				{
					m_TextureBuf[m_nCurTextureBufPos].vec[0].Init( ( rt.right - pOffset[0] ), rt.bottom );
					m_TextureBuf[m_nCurTextureBufPos].vec[1].Init( ( rt.right - pOffset[0] ), rt.bottom );
				}
				m_TextureBuf[m_nCurTextureBufPos].vec[2].Init( rt.right,  rt.bottom );
				m_TextureBuf[m_nCurTextureBufPos].vec[3].Init( rt.right, rt.top );
				break;
			case 3:
				if ( pOffset[1] > 0.0f)
				{
					m_TextureBuf[m_nCurTextureBufPos].vec[3].Init( ( rt.left + pOffset[1] ),  rt.top );
					m_TextureBuf[m_nCurTextureBufPos].vec[1].Init( rt.left,  rt.top );
				}
				else
				{
					m_TextureBuf[m_nCurTextureBufPos].vec[3].Init( rt.left, ( rt.bottom - pOffset[0] ) );
					m_TextureBuf[m_nCurTextureBufPos].vec[1].Init( rt.left, ( rt.bottom - pOffset[0] ) );
				}
				m_TextureBuf[m_nCurTextureBufPos].vec[0].Init( rt.left, rt.bottom );
				m_TextureBuf[m_nCurTextureBufPos].vec[2].Init( rt.right, rt.bottom );
				break;
			}
		}
		else
		{
			switch ( (int32) pOffset[4] )
			{
			case 0:
				if ( pOffset[1] > 0.0f)
				{
					m_TextureBuf[m_nCurTextureBufPos].vec[1].Init( rt.right, ( rt.top + pOffset[1] ) );
					m_TextureBuf[m_nCurTextureBufPos].vec[3].Init( rt.right, ( rt.top + pOffset[1] ) );
				}
				else
				{
					m_TextureBuf[m_nCurTextureBufPos].vec[1].Init( ( rt.left + pOffset[0] ),  rt.top );
					m_TextureBuf[m_nCurTextureBufPos].vec[3].Init( rt.right, rt.top );
				}
				m_TextureBuf[m_nCurTextureBufPos].vec[0].Init( rt.left,  rt.bottom );
				m_TextureBuf[m_nCurTextureBufPos].vec[2].Init( rt.right, rt.bottom );
				break;
			case 1:
				if ( pOffset[1] > 0.0f)
				{
					m_TextureBuf[m_nCurTextureBufPos].vec[3].Init( ( rt.right - pOffset[1] ), rt.bottom );
					m_TextureBuf[m_nCurTextureBufPos].vec[2].Init( ( rt.right - pOffset[1] ), rt.bottom );
				}
				else
				{
					m_TextureBuf[m_nCurTextureBufPos].vec[3].Init( rt.right, ( rt.top + pOffset[0] ) );
					m_TextureBuf[m_nCurTextureBufPos].vec[2].Init( rt.right, rt.bottom );
				}
				m_TextureBuf[m_nCurTextureBufPos].vec[0].Init( rt.left,  rt.bottom );
				m_TextureBuf[m_nCurTextureBufPos].vec[1].Init( rt.left,  rt.top );
				break;
			case 2:
				if ( pOffset[1] > 0.0f)
				{
					m_TextureBuf[m_nCurTextureBufPos].vec[0].Init( rt.left, ( rt.bottom - pOffset[1] ) );
					m_TextureBuf[m_nCurTextureBufPos].vec[2].Init( rt.left, ( rt.bottom - pOffset[1] ) );
				}
				else
				{
					m_TextureBuf[m_nCurTextureBufPos].vec[0].Init( rt.left, rt.bottom );
					m_TextureBuf[m_nCurTextureBufPos].vec[2].Init( ( rt.right - pOffset[0] ), rt.bottom );
				}
				m_TextureBuf[m_nCurTextureBufPos].vec[1].Init( rt.left,  rt.top );
				m_TextureBuf[m_nCurTextureBufPos].vec[3].Init( rt.right, rt.top );
				break;
			case 3:
				if ( pOffset[1] > 0.0f)
				{
					m_TextureBuf[m_nCurTextureBufPos].vec[0].Init( ( rt.left + pOffset[1] ),  rt.top );
					m_TextureBuf[m_nCurTextureBufPos].vec[1].Init( ( rt.left + pOffset[1] ),  rt.top );
				}
				else
				{
					m_TextureBuf[m_nCurTextureBufPos].vec[0].Init( rt.left, ( rt.bottom - pOffset[0] ) );
					m_TextureBuf[m_nCurTextureBufPos].vec[1].Init( rt.left, rt.top );
				}
				m_TextureBuf[m_nCurTextureBufPos].vec[2].Init( rt.right, rt.bottom );
				m_TextureBuf[m_nCurTextureBufPos].vec[3].Init( rt.right, rt.top );
				break;
			}
		}
	}
	if ( rtText )
	{
		m_TextureBuf[m_nCurTextureBufPos].rtText[0].Init( rtText->left,  rtText->bottom );
		m_TextureBuf[m_nCurTextureBufPos].rtText[1].Init( rtText->left,  rtText->top );
		m_TextureBuf[m_nCurTextureBufPos].rtText[2].Init( rtText->right, rtText->bottom );
		m_TextureBuf[m_nCurTextureBufPos].rtText[3].Init( rtText->right, rtText->top );
	}

	m_nCurTextureBufPos++;
}

//画矩形
void SQRGUIManager::DrawRect( const CFRect& rtr, uint32 uColor, ITexture* pTexture, const CPosRect& rtText, ERenderStyle rs, float z )
{
	if ( (uColor& 0xFF000000) == 0 )
		return;

	CFRect rt = rtr;
	rt.top		+= m_pRootWnd->m_hWnd->m_yScreen;
	rt.bottom	+= m_pRootWnd->m_hWnd->m_yScreen;
	rt.left		+= m_pRootWnd->m_hWnd->m_xScreen;
	rt.right	+= m_pRootWnd->m_hWnd->m_xScreen;

	if ( m_nCurTextureBufPos > 0 && ( m_pTextue != pTexture || m_RS != rs ) )
		FlushRect();
	if ( pTexture )
		pTexture->AddRef();
	SafeRelease( m_pTextue );

	if ( m_nCurTextureBufPos >= m_TextureBuf.size() )
		m_TextureBuf.push_back( RectGroup() );
	m_pTextue									= pTexture;
	m_RS										= rs;
	m_TextureBuf[m_nCurTextureBufPos].bFullText	= false;
	m_TextureBuf[m_nCurTextureBufPos].Color		= uColor;
	m_TextureBuf[m_nCurTextureBufPos].z			= z;
	m_TextureBuf[m_nCurTextureBufPos].vec[0].Init( rt.left,  rt.bottom );
	m_TextureBuf[m_nCurTextureBufPos].vec[1].Init( rt.left,  rt.top );
	m_TextureBuf[m_nCurTextureBufPos].vec[2].Init( rt.right, rt.bottom );
	m_TextureBuf[m_nCurTextureBufPos].vec[3].Init( rt.right, rt.top );
	m_TextureBuf[m_nCurTextureBufPos].rtText[0].Init( float( rtText.ptLB.x ),  float( rtText.ptLB.y ) );
	m_TextureBuf[m_nCurTextureBufPos].rtText[1].Init( float( rtText.ptLT.x ),  float( rtText.ptLT.y ) );
	m_TextureBuf[m_nCurTextureBufPos].rtText[2].Init( float( rtText.ptRB.x ),  float( rtText.ptRB.y ) );
	m_TextureBuf[m_nCurTextureBufPos].rtText[3].Init( float( rtText.ptRT.x ),  float( rtText.ptRT.y ) );

	m_nCurTextureBufPos++;
}

//画矩形
void SQRGUIManager::DrawRect( const CVector2f* rt, uint32 uColor, ITexture* pTexture, const CVector2f* rtText, ERenderStyle rs, float z )
{
	if ( (uColor& 0xFF000000) == 0 )
		return;

	if ( m_nCurTextureBufPos > 0 && ( m_pTextue != pTexture || m_RS != rs ) )
		FlushRect();
	if ( pTexture )
		pTexture->AddRef();
	SafeRelease( m_pTextue );

	if ( m_nCurTextureBufPos >= m_TextureBuf.size() )
		m_TextureBuf.push_back( RectGroup() );
	m_pTextue									= pTexture;
	m_RS										= rs;
	m_TextureBuf[m_nCurTextureBufPos].bFullText	= rtText ? false : true;
	m_TextureBuf[m_nCurTextureBufPos].Color		= uColor;
	m_TextureBuf[m_nCurTextureBufPos].z			= z;
	m_TextureBuf[m_nCurTextureBufPos].vec[0]	= rt[0];
	m_TextureBuf[m_nCurTextureBufPos].vec[1]	= rt[1];
	m_TextureBuf[m_nCurTextureBufPos].vec[2]	= rt[2];
	m_TextureBuf[m_nCurTextureBufPos].vec[3]	= rt[3];
	if ( rtText )
	{
		m_TextureBuf[m_nCurTextureBufPos].rtText[0]	= rtText[0];
		m_TextureBuf[m_nCurTextureBufPos].rtText[1]	= rtText[1];
		m_TextureBuf[m_nCurTextureBufPos].rtText[2]	= rtText[2];
		m_TextureBuf[m_nCurTextureBufPos].rtText[3]	= rtText[3];
	}

	m_nCurTextureBufPos++;
}

void SQRGUIManager::DrawRect( const CVector2f* rt, uint32 uColor, ITexture* pTexture0, ITexture* pTexture1, const CFRect* rtText0, const CVector2f* rtText1, ERenderStyle rs, float z )
{
	if ( (uColor& 0xFF000000) == 0 )
		return;

	FlushRect();
	SafeRelease( m_pTextue );

	m_pTextue									= NULL;//pTexture0;
	m_RS										= rs;

	float u1 = 0.00001f;
	float v1 = 0.00001f;
	float u2 = 1.00001f;
	float v2 = 1.00001f;

	Vertex2DAlpha* pVB;
	m_pGraphic->GetRenderStack( m_RS, pTexture0, pTexture1, PT_TRIANGLESTRIP, 4, 2,
	                            Vertex2DAlpha::Format, sizeof(Vertex2DAlpha), (void**)&pVB, NULL );


	u1 = ( rtText0->left + 0.25f )/pTexture0->GetWidth();
	v1 = ( rtText0->top + 0.25f )/pTexture0->GetHeight();
	u2 = ( rtText0->right + 0.25f )/pTexture0->GetWidth();
	v2 = ( rtText0->bottom + 0.25f )/pTexture0->GetHeight();


	pVB[0] = Vertex2DAlpha( rt[0].x + m_pRootWnd->m_hWnd->m_xScreen, rt[0].y + m_pRootWnd->m_hWnd->m_yScreen, z, uColor, u1, v1 ,  rtText1[0].x/pTexture1->GetWidth(),  rtText1[0].y/pTexture1->GetHeight()  );
	pVB[1] = Vertex2DAlpha( rt[1].x + m_pRootWnd->m_hWnd->m_xScreen, rt[1].y + m_pRootWnd->m_hWnd->m_yScreen, z, uColor, u2, v1 ,  rtText1[1].x/pTexture1->GetWidth(),  rtText1[1].y/pTexture1->GetHeight()  );
	pVB[2] = Vertex2DAlpha( rt[2].x + m_pRootWnd->m_hWnd->m_xScreen, rt[2].y + m_pRootWnd->m_hWnd->m_yScreen, z, uColor, u1, v2 ,  rtText1[2].x/pTexture1->GetWidth(),  rtText1[2].y/pTexture1->GetHeight()  );
	pVB[3] = Vertex2DAlpha( rt[3].x + m_pRootWnd->m_hWnd->m_xScreen, rt[3].y + m_pRootWnd->m_hWnd->m_yScreen, z, uColor, u2, v2 ,  rtText1[3].x/pTexture1->GetWidth(),  rtText1[3].y/pTexture1->GetHeight()  );
}

void SQRGUIManager::DrawRect(const CVector2f* rt, uint32 uColor, ITexture* pTexture0, ITexture* pTexture1, ITexture* pTexture2, ERenderStyle rs, float z )
{
	if ( (uColor& 0xFF000000) == 0 )
		return;

	FlushRect();
	SafeRelease( m_pTextue );

	m_pTextue									= NULL;//pTexture0;
	m_RS										= rs;

	float u1 = 0.00001f;
	float v1 = 0.00001f;
	float u2 = 1.00001f;
	float v2 = 1.00001f;

	Vertex2D* pVB;
	m_pGraphic->GetRenderThreeLayerStack( m_RS, pTexture0, pTexture1,pTexture2, PT_TRIANGLESTRIP, 4, 2,
		Vertex2D::Format, sizeof(Vertex2D), (void**)&pVB, NULL );

	pVB[0] = Vertex2D( rt[0].x + m_pRootWnd->m_hWnd->m_xScreen, rt[0].y + m_pRootWnd->m_hWnd->m_yScreen, z, uColor, u1, v1 );
	pVB[1] = Vertex2D( rt[1].x + m_pRootWnd->m_hWnd->m_xScreen, rt[1].y + m_pRootWnd->m_hWnd->m_yScreen, z, uColor, u2, v1 );
	pVB[2] = Vertex2D( rt[2].x + m_pRootWnd->m_hWnd->m_xScreen, rt[2].y + m_pRootWnd->m_hWnd->m_yScreen, z, uColor, u1, v2 );
	pVB[3] = Vertex2D( rt[3].x + m_pRootWnd->m_hWnd->m_xScreen, rt[3].y + m_pRootWnd->m_hWnd->m_yScreen, z, uColor, u2, v2 );
}

//画矩形
void SQRGUIManager::FlushRect()
{
	if ( m_nCurTextureBufPos )
	{
		m_nTotalRect += m_nCurTextureBufPos;
		m_nFlush++;
		SQR_TRY
		{
			m_pGraphic->DrawRectGroup( m_pTextue, m_RS, &m_TextureBuf[0], m_nCurTextureBufPos );
		}
		SQR_CATCH(exp)
		{
			stringstream  ExpStr;
			ExpStr << exp.ErrorMsg() << "; Image Count:" << m_nCurTextureBufPos;
			GfkLogErr(exp.ErrorTitle(), ExpStr.str());
		}
		SQR_TRY_END;
		m_nCurTextureBufPos = 0;
	}
}

//画文字
void SQRGUIManager::FlushText()
{
	// 注：字体异，入队列
	CRenderFont*	curFont = NULL;
	int32			curFontId = -1;
	float			curSize = 0.0f;
	uint32			bufsize = m_TextBuf.size();
	for (uint32 i = 0; i < bufsize; i++)
	{
		if (curFont == NULL || curFont != m_TextBuf[i].Rf || 
			curSize != m_TextBuf[i].Pa.GetSize() || curFont->GetFontID() != curFontId)
		{
			if (curFont != NULL)
			{
				// end this dp draw
				curFont->EndFlush();
			}
			curFont = m_TextBuf[i].Rf;
			curFontId = curFont->GetFontID();
			curSize = m_TextBuf[i].Pa.GetSize();
			// start the new dp draw
			if ( curFont )
				curFont->BeginFlush(curSize);
			else
				continue;
		}
		// add text to flush
		curFont->AddDrawContent(m_TextBuf[i].Pa);
	}
	if ( curFont )
		curFont->EndFlush();

	for (uint32 i = 0; i < bufsize; i++)
		m_TextBuf[i].Rf->SubRenderRef();

	m_TextBuf.clear();
}

void SQRGUIManager::FlushDraw()
{
	FlushRect();
	FlushText();
}

void SQRGUIManager::ClearFontCatch(CRenderFont* font)
{
	// texture
	m_nCurTextureBufPos = 0;
	// font
	uint32	bufsize = m_TextBuf.size();
	for (uint32 i = 0; i < bufsize; i++)
		m_TextBuf[i].Rf->SubRenderRef();
	m_TextBuf.clear();
}

//------------------------------------------------------------------------------
void
SQRGUIManager::DrawText( CRenderFont* font, float size, const wstring& text,
                       uint32 color, uint32 bkColor, uint32 gradualColor,
                       CFRect rect, float x, float y, float z,
                       FontEffect::Mask mask, float offset )
{
	//FlushRect();
	if (font)
		m_FontVec.insert(font);
	RenderTextParam param(size*= GetZoomSize());
	rect.top	+= m_pRootWnd->m_hWnd->m_yScreen;
	rect.bottom += m_pRootWnd->m_hWnd->m_yScreen;
	rect.left	+= m_pRootWnd->m_hWnd->m_xScreen;
	rect.right	+= m_pRootWnd->m_hWnd->m_xScreen;

	param.SetPosition(x, y, z, offset);
	param.SetColor(color);
	param.SetGradualColor(gradualColor);
	param.SetRect(rect);
	param.SetFontEffect(mask);
	param.SetBackColor(bkColor);
	param.SetText(text);
	
	TextBuf t_buf;

	font->AddRenderRef();
	t_buf.Rf = font;
	t_buf.Pa = param;
	m_TextBuf.push_back(t_buf);
}

void SQRGUIManager::GetSysCursorPos(CFPos& pos)
{
	::GetCursorPos((LPPOINT)&pos);
}

void SQRGUIManager::SetSysCursorPos(int x, int y)
{
	::SetCursorPos(x,y);
}

void SQRGUIManager::ShowCursor(bool bShow)
{
	::ShowCursor(bShow);
}

void SQRGUIManager::GetClientRect(CIRect& rect)
{
	::GetClientRect(m_hMainWnd, (RECT*)&rect);
	POINT point = {0,0};
	point.x = rect.left;
	point.y = rect.top;
	::ClientToScreen( m_hMainWnd, (LPPOINT)&point );
	rect.left = point.x;
	rect.top  = point.y;
	point.x = rect.right;
	point.y = rect.bottom;
	::ClientToScreen( m_hMainWnd, (LPPOINT)&point );
	rect.right  = point.x;
	rect.bottom = point.y;
}

void SQRGUIManager::ClipCursor(CIRect& rect)
{
	::ClipCursor((RECT*)&rect);
}

void SQRGUIManager::SetCapture()
{
	::SetCapture(m_hMainWnd);
}

void SQRGUIManager::ReleaseCapture()
{
	::ReleaseCapture();
}

void SQRGUIManager::DragWnd(SQRWnd& Wnd)
{
	if ( Wnd.m_hWnd->m_ParentWnd )
	{
		if ( !( WS_CHILD&Wnd.m_hWnd->m_WndStyle ) )
		{
			if ( Wnd.IsInTitleArea( Wnd.m_hWnd->m_CurHeldPos.x + Wnd.GetWndLeft(), Wnd.m_hWnd->m_CurHeldPos.y + Wnd.GetWndTop() ) )
			{
				float dx = GetCursorPos().x - Wnd.m_hWnd->m_CurHeldPos.x - Wnd.GetWndLeft();
				float dy = GetCursorPos().y - Wnd.m_hWnd->m_CurHeldPos.y - Wnd.GetWndTop();

				if ( Wnd.OnWndPreMoved(dx, dy) )
				{
					Wnd.m_hWnd->m_xScreen += dx;
					Wnd.m_hWnd->m_yScreen += dy;

					Wnd.m_hWnd->m_xScreenbak += dx;
					Wnd.m_hWnd->m_yScreenbak += dy;

					Wnd.OnWndMoved( dx, dy );

					SQRWnd* pChildWnd = Wnd.m_hWnd->m_FirstChild;
					while ( pChildWnd )
					{
						OnParentMoved( *pChildWnd, dx, dy );
						if ( !FindWnd( pChildWnd ) )
							break;
						pChildWnd = pChildWnd->m_hWnd->m_NextBrother;
					}
				}
			}
		}
		else
		{
			CFPos pt( GetCursorPos().x - Wnd.GetWndLeft(), GetCursorPos().y - Wnd.GetWndTop() );
			CFPos curHeldPos = Wnd.m_hWnd->m_CurHeldPos;
			float fDisXSqr = CMath::Sqr(curHeldPos.x - pt.x);
			float fDisYSqr = CMath::Sqr(curHeldPos.y - pt.y);
			if( fDisXSqr + fDisYSqr > CMath::Sqr(m_fDragResponseDis) )
				Wnd.OnDrag( curHeldPos, pt );
		}
	}
}

ResInfo* SQRGUIManager::GetResInfo( const UIString& str )
{
	MapResStaticInfo::iterator itr = m_ResStaticInfoContainer.find(str);
	if ( itr != m_ResStaticInfoContainer.end() )
		return &(itr->second);
	return NULL;
}
