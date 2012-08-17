#include "stdafx.h"
#include "SQRItemWnd.h"
#include "SQRGUIManager.h"
#include "SQRListCtrl.h"

DYNAMIC_CREATE_WND( SQRItemWnd, SQRButton )
SQRItemWnd::SQRItemWnd(void)
: m_bSel( false )
, m_bkeyboard( 0 )
{
}

SQRItemWnd::~SQRItemWnd(void)
{
	DestroyWnd();
}

int32 SQRItemWnd::Create( const WndCreateParam& param )
{
	WndCreateParam newParam = param;
	newParam.uStyle |= WS_CUTBYPARENT|WS_ITEM;
	return SQRButton::Create( newParam );
}

//WND_RENDER_OBJ*	SQRItemWnd::GetRenderObject()
//{
//	return &m_RenderObject;
//}

const char* SQRItemWnd::GetRenderObjAni( const IMAGE_PARAM& ImageFlag )
{
	//#define IP_ENABLE					0x0001		//背景贴图
	//#define IP_DISABLE				0x0002		//背景贴图
	//#define IP_MOUSEOVER		        0x0003		//BUTTON控件MOUSEOVER贴图
	//#define IP_CLICKDOWN		        0x0004		//BUTTON控件CLICKDOWN贴图
	//#define IP_UNCHECK_ENABLE			0x0005		//BUTTON控件Uncheck Normal贴图
	//#define IP_UNCHECK_ENABLE			0x0006		//BUTTON控件Uncheck Disable贴图
	//#define IP_UNCHECK_MOUSEOVER	    0x0007		//BUTTON控件Uncheck MOUSEOVER贴图
	//#define IP_UNCHECK_CLICKDOWN	    0x0008		//BUTTON控件Uncheck CLICKDOWN贴图

	IMAGE_PARAM Flag = ImageFlag;
	if( Flag.CtrlMask != SM_BS_BK )
		return NULL;
	else if( Flag.StateMask == IP_ENABLE )
		return m_EnableAni;
	else if( Flag.StateMask == IP_DISABLE )
		return m_DisableAni;
	else if( Flag.StateMask == IP_MOUSEOVER )
		return m_MouseOverAni;
	else if( Flag.StateMask == IP_CLICKDOWN )
		return m_ClickDownAni;
	else
		return NULL;
}

void SQRItemWnd::SetRenderObjAni( const IMAGE_PARAM& ImageFlag, const char* szAni )
{
	IMAGE_PARAM Flag = ImageFlag;
	if( Flag.CtrlMask != SM_BS_BK )
		return;
	else if( Flag.StateMask == IP_ENABLE )
		m_EnableAni = szAni;
	else if( Flag.StateMask == IP_DISABLE )
		m_DisableAni = szAni;
	else if( Flag.StateMask == IP_MOUSEOVER )
		m_MouseOverAni = szAni;
	else if( Flag.StateMask == IP_CLICKDOWN )
		m_ClickDownAni = szAni;
}

void SQRItemWnd::OnLButtonUp( uint32 nFlags, int32 x, int32 y )
{
	SetSelected( true );
}

void SQRItemWnd::OnLButtonClick( uint32 nFlags, int32 x, int32 y )
{
	SetSelectedByClick( true );
}

void SQRItemWnd::DrawRect( WND_IMAGE_LIST& ImageList, const CFRect* rt, float offsetX, float offsetY )
{
	SQRWnd::OnProcess();
	if ( !(m_hWnd->m_bShow) )
	{
		return;
	}

	SQRWnd* pParentWnd = GetParent();

	if ( pParentWnd && pParentWnd->GetClassName() == string( "SQRListCtrl" ) )
	{
		SQRListCtrl* pListCtrl = (SQRListCtrl*)pParentWnd;
		// 通过pListCtrl获取材质

		CFRect rtWnd = rt ? *rt : CFRect( 0, 0, (float)GetWndWidth(), (float)GetWndHeight() );

		/*if ( pListCtrl->GetStyle()&WS_CUTBYPARENT )
		{*/
			CFRect rtRootCutWnd = GetParentCutRect();
			ScreenToWnd( rtRootCutWnd );
			rtWnd = rtWnd.Intersection(rtRootCutWnd);
		/*}*/
		if ( m_pCutWnd )
		{
			CFRect rtCutWnd;
			m_pCutWnd->GetCutAreaRect(rtCutWnd);
			ScreenToWnd( rtCutWnd );
			rtWnd = rtWnd.Intersection(rtCutWnd);
		}

		if(rtWnd.IsEmpty())
		{
			return;
		}

		// 父窗体信息
		StyleStruct* pStyle = pListCtrl->GetStyleStruct();
		uint32 Style = pListCtrl->GetWndHandle()->m_StyleIndex;

		// 该窗体信息
		CFRect	TempWnd;
		CFRect	TempTexture;
		float Zoom = m_hWnd->m_pGUI->GetZoomSize();

		GUI_BLOCK* pBlock = NULL;
		if ( Style > 0 && NULL != pStyle )
		{
			// 需要映射回去
			EventStateMask t_EState;
			if (GetEventStateMask() == IP_ENABLE || GetEventStateMask() == IP_DISABLE)
			{
				t_EState = IP_LS_UNSEL;
			}
			else if (GetEventStateMask() == IP_MOUSEOVER)
			{
				t_EState = IP_MOUSEOVER;
			}
			else if (GetEventStateMask() == IP_CLICKDOWN)
			{
				t_EState = IP_LS_SEL;
			}
			else
			{
				t_EState = IP_LS_UNSEL;
			}

			pBlock = pStyle->m_vecPool[t_EState][Style];
			if(pBlock == NULL)
				pBlock = pStyle->m_vecPool[t_EState][1];
		}

		if( Style == 0 || pBlock==NULL)
		{
			SQRWnd::DrawRect( ImageList, &rtWnd, offsetX, offsetY );
			return;
		}
		else
		{
			// 用风格的方式画
			//---------------------九宫格---------------------------------
			GUI_BLOCK& Block = *pBlock;
			float TopLeftH		= Block[GBM_TOP_LEFT].Height()* Zoom;
			float TopLeftW		= Block[GBM_TOP_LEFT].Width() * Zoom;
			float BottomRightW	= Block[GBM_BOTTOM_RIGHT].Width() * Zoom;
			float BottomRightH	= Block[GBM_BOTTOM_RIGHT].Height()* Zoom;
			float TopRightW		= Block[GBM_TOP_RIGHT].Width() * Zoom;
			float BottomLeftW	= Block[GBM_BOTTOM_LEFT].Width() * Zoom;

			TempTexture = Block[GBM_CENTER];
			TempWnd.top		= rtWnd.top		+ TopLeftH;
			TempWnd.left	= rtWnd.left	+ TopLeftW;
			TempWnd.right	= rtWnd.right	- BottomRightW;
			TempWnd.bottom	= rtWnd.bottom	- BottomRightH;
			TempWnd = TempWnd.Intersection(rtWnd);
			if ( !(TempWnd.IsEmpty()) )
				SQRWnd::DrawRect(pStyle->pTexture, TempWnd, pStyle->m_dwColor, &TempTexture	, false);

			TempTexture = Block[GBM_TOP];
			TempWnd.top		= rtWnd.top;
			TempWnd.bottom	= rtWnd.top		+ TempTexture.Height() * Zoom;
			TempWnd.left	= rtWnd.left	+ TopLeftW;
			TempWnd.right	= rtWnd.right	- TopRightW;
			TempWnd = TempWnd.Intersection(rtWnd);
			if ( !(TempWnd.IsEmpty()) )
				SQRWnd::DrawRect(pStyle->pTexture, TempWnd, pStyle->m_dwColor, &TempTexture	, false);


			TempTexture = Block[GBM_BOTTOM];
			TempWnd.top		= rtWnd.bottom	- TempTexture.Height() * Zoom;;
			TempWnd.bottom	= rtWnd.bottom;
			TempWnd.left	= rtWnd.left	+ BottomLeftW;
			TempWnd.right	= rtWnd.right	- BottomRightW;
			TempWnd = TempWnd.Intersection(rtWnd);
			if ( !(TempWnd.IsEmpty()) )
				SQRWnd::DrawRect(pStyle->pTexture, TempWnd, pStyle->m_dwColor, &TempTexture	, false);		

			TempTexture = Block[GBM_LEFT];
			TempWnd.top		= rtWnd.top		+ TopLeftH;
			TempWnd.left	= rtWnd.left;
			TempWnd.right	= rtWnd.left	+ TempTexture.Width() * Zoom;;
			TempWnd.bottom	= rtWnd.bottom	- BottomRightH;
			SQRWnd::DrawRect(pStyle->pTexture, TempWnd, pStyle->m_dwColor, &TempTexture	, false);

			TempTexture = Block[GBM_RIGHT];
			TempWnd.top		= rtWnd.top		+ TopLeftH;
			TempWnd.left	= rtWnd.right	- TempTexture.Width() * Zoom;;
			TempWnd.right	= rtWnd.right;
			TempWnd.bottom	= rtWnd.bottom	- BottomRightH;
			TempWnd = TempWnd.Intersection(rtWnd);
			if ( !(TempWnd.IsEmpty()) )
				SQRWnd::DrawRect(pStyle->pTexture, TempWnd, pStyle->m_dwColor, &TempTexture	, false);

			TempTexture = Block[GBM_TOP_LEFT];
			TempWnd.top		= rtWnd.top;
			TempWnd.bottom	= rtWnd.top		+ TopLeftH;//TempTexture.Height();
			TempWnd.left	= rtWnd.left;
			TempWnd.right	= rtWnd.left	+ TopLeftW;//TempTexture.Width();
			SQRWnd::DrawRect(pStyle->pTexture, TempWnd, pStyle->m_dwColor, &TempTexture	, false);
			m_itX = TempTexture.Width();
			m_itY = TempTexture.Height();

			TempTexture = Block[GBM_TOP_RIGHT];
			TempWnd.top		= rtWnd.top;
			TempWnd.bottom	= rtWnd.top		+ TempTexture.Height() * Zoom;
			TempWnd.left	= rtWnd.right	- TopRightW;//TempTexture.Width();
			TempWnd.right	= rtWnd.right;	
			TempWnd = TempWnd.Intersection(rtWnd);
			if ( !(TempWnd.IsEmpty()) )
				SQRWnd::DrawRect(pStyle->pTexture, TempWnd, pStyle->m_dwColor, &TempTexture	, false);

			TempTexture = Block[GBM_BOTTOM_LEFT];
			TempWnd.top		= rtWnd.bottom	- TempTexture.Height() * Zoom;;
			TempWnd.bottom	= rtWnd.bottom;
			TempWnd.left	= rtWnd.left;
			TempWnd.right	= rtWnd.left	+ BottomLeftW;//TempTexture.Width();
			TempWnd = TempWnd.Intersection(rtWnd);
			if ( !(TempWnd.IsEmpty()) )
				SQRWnd::DrawRect(pStyle->pTexture, TempWnd, pStyle->m_dwColor, &TempTexture	, false);

			TempTexture = Block[GBM_BOTTOM_RIGHT];
			TempWnd.top		= rtWnd.bottom	- BottomRightH;//TempTexture.Height();
			TempWnd.bottom	= rtWnd.bottom;
			TempWnd.left	= rtWnd.right	- BottomRightW;//TempTexture.Width();
			TempWnd.right	= rtWnd.right;
			TempWnd = TempWnd.Intersection(rtWnd);
			if ( !(TempWnd.IsEmpty()) )
				SQRWnd::DrawRect(pStyle->pTexture, TempWnd, pStyle->m_dwColor, &TempTexture	, false);
		}
	}
	else
	{
		SQRWnd::DrawRect( ImageList, rt, offsetX, offsetY );
	}
}

void SQRItemWnd::SetSelected( bool bSel )
{
	m_bSel = bSel;
	SendMsg( ITEM_LBUTTONUP, m_bSel, 0 );
	SendCtrlMsg( ITEM_LBUTTONUP, m_bSel, 0 );
}

void SQRItemWnd::SetSelectedByClick( bool bSel )
{
	m_bSel = bSel;
	SendMsg( ITEM_LBUTTONCLICK, m_bSel, 0 );
	SendCtrlMsg( ITEM_LBUTTONCLICK, m_bSel, 0 );
}

bool SQRItemWnd::IsSelected() const
{
	return m_bSel;
}

void SQRItemWnd::DrawWndBackground()
{
	m_beBGDrawed = true;
	WndHandle* handle = GetWndHandle();
	IDNAME CurAni;
	//如果已经已经接收了键盘事件
	if( 3 == m_bkeyboard ) // CancelMouseMove called
	{
		SetEventStateMask(IP_ENABLE);
		SQRWnd::DrawRect( handle->m_Enable );
		CurAni = m_EnableAni;
	}
	else if( 1 == m_bkeyboard || 2 == m_bkeyboard) // SetHighLight called or 2 ?
	{
		SetEventStateMask(IP_MOUSEOVER);
		SQRWnd::DrawRect( m_MouseOverImage );
		CurAni = m_MouseOverAni;
	}
	else if( 0 == m_bkeyboard ) // CancelHighLight called
	{	
		CFPos pt = GetCursorPos();
		if(IsEnable() == false)
		{
			SetEventStateMask(IP_DISABLE);
			SQRWnd::DrawRect( handle->m_Disable );
			CurAni = m_DisableAni;
		}
		else if(IsInWndArea( pt.x, pt.y ) && IsLastMouseMsgWnd())
		{
			SetEventStateMask(IP_MOUSEOVER);
			SQRWnd::DrawRect( m_MouseOverImage );
			CurAni = m_MouseOverAni;
			m_bkeyboard = 2;	// 2 ?
		}
		else if(IsSelected())
		{
			SetEventStateMask(IP_CLICKDOWN);
			SQRWnd::DrawRect( m_ClickDownImage );
			CurAni = m_ClickDownAni;
		}
		else
		{
			SetEventStateMask(IP_ENABLE);
			SQRWnd::DrawRect( handle->m_Enable );
			CurAni = m_EnableAni;
		}
	}
	CFPos pt = GetCursorPos();
	if (!IsInWndArea( pt.x, pt.y ))
	{
		m_bkeyboard = 0;
	}
	/*if( CurAni != m_CurAni)
		m_RenderObject.SetNextAnimation( CurAni, FramePair( 0, -1 ) );*/

	SQRGUIManager* guiManager = handle->m_pGUI;
	//if (!m_RenderObject.IsEmpty())
	//	guiManager->FlushDraw();

	CMatrix matOff, matScale;
	matOff.SetTranslate(handle->m_xScreen + handle->m_Width/2.0f - guiManager->GetSreenWidth()/2.0f,
		guiManager->GetSreenHeight()/2.0f - handle->m_yScreen - handle->m_Height/2.0f, 0);
	matScale.SetScale( guiManager->GetZoomSize(), guiManager->GetZoomSize(), guiManager->GetZoomSize() );

	//m_RenderObject.Render(matOff * matScale, (uint32)GetProcessTime());
}
