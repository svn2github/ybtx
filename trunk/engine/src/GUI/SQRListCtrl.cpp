#include "stdafx.h"
#include "SQRListCtrl.h"
#include "SQRWnd.h"
#include "SQRGUIManager.h"
#include "BaseHelper.h"
#include "SQRItemWnd.h"
#include "SQRDialog.h"
#include "TSqrAllocator.inl"
#include "CColor.inl"

/// @brief ListCtrl的一个格子
struct ItemWnd 
{
	ItemWnd( SQRWnd* pWnd = NULL, bool bSel = false )
		: m_pWnd( pWnd )
		, m_bSel( bSel )
	{
	}
	SQRWnd*	m_pWnd;
	bool	m_bSel;
};

/// @brief ListCtrl的一列格子
struct ListItem
{
	ListItem() 
		: m_uParam(0)
		, m_uAscendData(0)
		, m_fHeight(0)
		, m_bCItemWnd(false)
	{
	}
	~ListItem()
	{ 
		for( size_t i = 0; i < m_arrItem.size(); i++ )
			GfkSafeDelete( m_arrItem[i].m_pWnd );
		m_arrItem.clear();
	}

	uint32				m_uParam;
	uint32				m_uAscendData;
	float				m_fHeight;		///< 每个格子的高度
	bool				m_bCItemWnd;
	UIVector<ItemWnd>	m_arrItem;
};

/// @brief 按照列优先组织的ListCtrl的格子二维表
struct ListContainer
{
	UIVector<float>		m_ListWidth;	///< 每列的宽度
	UIVector<ListItem*>	m_ListItem;		///< 每列的格子
};

inline ListContainer& __LI( HANDLE p ){ return *((ListContainer*)p); }

/////////////////////////////////////////////////
// class SQRListCtrl
/////////////////////////////////////////////////

DYNAMIC_CREATE_WND( SQRListCtrl, SQRWnd )

SQRListCtrl::SQRListCtrl()
{
	m_hList					= new ListContainer;
	m_ItemSelColor			= 0xffffffff;
	m_ItemUnselColor		= 0xffffffff;
	m_ItemSelBkColor		= 0xff000000;
	m_ItemUnselBkColor		= 0x00000000;
	m_keyboardCount			= -1;
	m_uLastTimeHighLightX	= -1;
	m_uLastTimeHighLightY	= -1;
}

SQRListCtrl::~SQRListCtrl()
{
	delete (ListContainer*)m_hList;
}

int32	SQRListCtrl::Create( const WndCreateParam& param )
{
	WndCreateParam newParam = param;
	newParam.szTitle = "";
	newParam.uStyle |= WS_CHILD|WS_NOTIFY;
   	if( FAILED( SQRWnd::Create( newParam ) ) )
		return GERROR;
	
	WndCreateParam hParam = param;
	hParam.szTitle = "";
	hParam.uStyle = SS_HSCROLL;
	hParam.x = 0;
	hParam.y = GetWndHeight();
	hParam.width = GetWndWidth();
	hParam.height = 16;
	hParam.pParentWnd = this;
	m_sbHorizon.Create( hParam );
	m_sbHorizon.SetLockFlag( LOCK_BOTTOM|LOCK_LEFT|LOCK_RIGHT );

	WndCreateParam vParam = param;
	vParam.szTitle = "";
	vParam.uStyle = 0;
	vParam.x = GetWndWidth();
	vParam.y = 0;
	vParam.width = 16;
	vParam.height = GetWndHeight();
	vParam.pParentWnd = this;
	m_sbVertical.Create( vParam );
	m_sbVertical.SetLockFlag( LOCK_RIGHT|LOCK_TOP|LOCK_BOTTOM );

	m_sbHorizon.SetRange( 1 );
	m_sbHorizon.SetPage( 1 );
	m_sbHorizon.SetPos( 0 );
	
	m_sbVertical.SetRange( 1 );
	m_sbVertical.SetPage( 1 );
	m_sbVertical.SetPos( 0 );
	
	m_sbHorizon.ShowWnd( (GetStyle()&WS_HORSCROLL) != 0 );
	m_sbVertical.ShowWnd( (GetStyle()&WS_VERSCROLL) != 0 );

	CFRect rt( 0, 0, (float)GetWndWidth(), GetWndHeight()/2.0f );
	m_ItemSelImage.AddImage( GetGraphic(), 0, "", &rt, CFPos(0.0f, 0.0f), CColor::RandDark() );
	m_ItemUnselImage.AddImage( GetGraphic(), 0, "", &rt, CFPos(0.0f, 0.0f), CColor::RandDark() );
	m_ItemMouseOverImage.AddImage( GetGraphic(), 0, "", &rt, CFPos(0.0f, 0.0f), CColor::RandDark() );

	m_ItemSelColor			= 0xffffffff;
	m_ItemUnselColor		= 0xffffffff;
	m_ItemMouseOverColor	= 0xffffffff;
	m_ItemSelBkColor		= 0xff000000;
	m_ItemUnselBkColor		= 0xff000000;
	m_ItemMouseOverBkColor	= 0xff000000;

	return 0;
}

int32 SQRListCtrl::DestroyWnd()
{
	m_ItemSelImage.Release();
	m_ItemUnselImage.Release();
	DeleteAllItem();
	return SQRWnd::DestroyWnd();
}

void SQRListCtrl::SetStyle( uint32 Style )
{
	SQRWnd::SetStyle( Style );
	ResetItemRect(true, true);
}

void SQRListCtrl::SetWndRect( const CFRect& rt )
{
	SQRWnd::SetWndRect(rt);
	// ResetItemRect 与 SetWndRect在既改变窗体大小
	// 又改变滚动条位置的情况下，设计的有冲突，考虑新的设计方法
	ResetItemRect(true, false);
}

void SQRListCtrl::ResizeRect( const CFRect& rt, bool refreshbak )
{
	SQRWnd::ResizeRect( rt,refreshbak );
	ResetItemRect(true, refreshbak);
}

void SQRListCtrl::_SetWndRect( const CFRect& rt )
{
	SQRWnd::_SetWndRect(rt);
	// ResetItemRect 与 SetWndRect在既改变窗体大小
	// 又改变滚动条位置的情况下，设计的有冲突，考虑新的设计方法
	ResetItemRect(true, false);
}

void SQRListCtrl::_ResizeRect( const CFRect& rt, bool refreshbak )
{
	SQRWnd::_ResizeRect( rt,refreshbak );
	ResetItemRect(true, refreshbak);
}

/// @brief 绘制ListCtrl item间的格子
void SQRListCtrl::DrawAfterChildren()
{
	if((GetStyle()&LCTL_SHOWGRID) == 0)
		return;

#ifdef UI_HIDE_MODE
	if (m_hWnd->m_pGUI->GetHidenMode() && !IsModleState() )
	{
		return;
	}
#endif

	float Zoom = m_hWnd->m_pGUI->GetZoomSize();
	ListContainer& ListC = __LI( m_hList );
	
	IMAGE_PARAM Flag;
	Flag.CtrlMask = SM_BS_BK;
	Flag.StateMask = (uint16)( IsEnable() ? IP_ENABLE : IP_DISABLE );
	uint32 uColor = GetWndTextColor( Flag );

	if( ListC.m_ListItem.empty() || ListC.m_ListWidth.empty() )
		return;
	float fTotalWidth = 1.0f, fTotalHeight = 0.0f;
	for( int32 i = (int32)m_sbVertical.GetPos(); i < (int32)ListC.m_ListItem.size(); i++ )
		fTotalHeight += ListC.m_ListItem[i]->m_fHeight;
	for( int32 i = (int32)m_sbHorizon.GetPos(); i < (int32)ListC.m_ListWidth.size(); i++ )
		fTotalWidth += ListC.m_ListWidth[i];
	
	fTotalHeight *= Zoom;
	fTotalWidth  *= Zoom;

	float fCurHeight = 0.0f;
	for( int32 i = (int32)m_sbVertical.GetPos(); i < (int32)ListC.m_ListItem.size() + 1; i++ )
	{
		PointGroup P[2];
		P[0].uColor = uColor;
		P[0].vPoint = CVector3f( 0, fCurHeight, 0 );
		P[1].uColor = uColor;
		P[1].vPoint = CVector3f(fTotalWidth, fCurHeight, 0 );
		DrawLine( P, 2 );
		if( i < (int32)ListC.m_ListItem.size() )
			fCurHeight += int32(ListC.m_ListItem[i]->m_fHeight * Zoom);
	}
	 

	float fCurWidth = 0.0f;
	for( int32 i = (int32)m_sbHorizon.GetPos(); i < (int32)ListC.m_ListWidth.size() + 1; i++ )
	{
		PointGroup P[2];
		P[0].uColor = uColor;
		P[0].vPoint = CVector3f( (float)fCurWidth, 0, 0 );
		P[1].uColor = uColor;
		P[1].vPoint = CVector3f( (float)fCurWidth, (float)fTotalHeight, 0 );
		DrawLine( P, 2 );
		if( i < (int32)ListC.m_ListWidth.size() )
			fCurWidth += int32(ListC.m_ListWidth[i] * Zoom);
	}
}

void SQRListCtrl::DrawRect( WND_IMAGE_LIST& ImageList, const CFRect* rt, float offsetX, float offsetY )
{
	if ( GetEventStateMask() == IP_MOUSEOVER || GetEventStateMask() == IP_LS_SEL || GetEventStateMask() == IP_LS_UNSEL )
	{
		SetEventStateMask(IP_ENABLE);
	}

	SQRWnd::DrawRect( ImageList, rt, offsetX, offsetY );
}

float SQRListCtrl::GetWorkAreaHeight()
{
	StyleStruct* pStyle = GetStyleStruct();
	uint32 Style = m_hWnd->m_StyleIndex;
	GUI_BLOCK* pBlock = NULL;

	float heigh = GetWndHeight();
	if ( Style>0 && NULL != pStyle )
	{
		pBlock = pStyle->m_vecPool[IP_ENABLE][Style];
		if(pBlock == NULL)
			pBlock = pStyle->m_vecPool[IP_ENABLE][1];
		if( pBlock )
		{
			GUI_BLOCK& Block = *pBlock;
			float t_Zoom = m_hWnd->m_pGUI->GetZoomSize();
			float TopLeftH		= Block[GBM_TOP_LEFT].Height()* t_Zoom;
			float BottomRightH	= Block[GBM_BOTTOM_RIGHT].Height()* t_Zoom;
			heigh = heigh - TopLeftH - BottomRightH;
		}
	}
	return heigh;
}

float SQRListCtrl::GetWorkAreaWidth()
{
	StyleStruct* pStyle = GetStyleStruct();
	uint32 Style = m_hWnd->m_StyleIndex;
	GUI_BLOCK* pBlock = NULL;

	float width = GetWndWidth();

	if ( Style>0 && NULL != pStyle )
	{
		pBlock = pStyle->m_vecPool[IP_ENABLE][Style];
		if(pBlock == NULL)
			pBlock = pStyle->m_vecPool[IP_ENABLE][1];
		if( pBlock )
		{
			GUI_BLOCK& Block = *pBlock;
			float t_Zoom = m_hWnd->m_pGUI->GetZoomSize();
			float TopLeftW		= Block[GBM_TOP_LEFT].Width() * t_Zoom;
			float BottomRightW	= Block[GBM_BOTTOM_RIGHT].Width() * t_Zoom;
			width = width - TopLeftW - BottomRightW;
		}
	}
	return width;
}

CFRect SQRListCtrl::GetWorkAreaRect()
{
	StyleStruct* pStyle = GetStyleStruct();
	uint32 Style = m_hWnd->m_StyleIndex;
	GUI_BLOCK* pBlock = NULL;

	CFRect wndRect = CFRect(0, 0, GetWndWidth(), GetWndHeight());
	if ( Style>0 && NULL != pStyle )
	{
		pBlock = pStyle->m_vecPool[IP_ENABLE][Style];
		if(pBlock == NULL)
			pBlock = pStyle->m_vecPool[IP_ENABLE][1];
		if( pBlock )
		{
			GUI_BLOCK& Block = *pBlock;
			float t_Zoom = m_hWnd->m_pGUI->GetZoomSize();
			float TopLeftH		= Block[GBM_TOP_LEFT].Height()* t_Zoom;
			float TopLeftW		= Block[GBM_TOP_LEFT].Width() * t_Zoom;
			float BottomRightW	= Block[GBM_BOTTOM_RIGHT].Width() * t_Zoom;
			float BottomRightH	= Block[GBM_BOTTOM_RIGHT].Height()* t_Zoom;

			wndRect.left += TopLeftW;
			wndRect.top += TopLeftH;
			wndRect.bottom -= BottomRightH;
			wndRect.right -= BottomRightW;
		}
	}
	return wndRect;
}

void SQRListCtrl::GetWorkAreaRectEx(CFRect& rect)
{
	rect = GetWorkAreaRect();
}

float SQRListCtrl::GetWorkAreaOrgHeight()
{
	StyleStruct* pStyle = GetStyleStruct();
	uint32 Style = m_hWnd->m_StyleIndex;
	GUI_BLOCK* pBlock = NULL;

	float heigh = GetWndOrgHeight();
	if ( Style>0 && NULL != pStyle )
	{
		pBlock = pStyle->m_vecPool[IP_ENABLE][Style];
		if(pBlock == NULL)
			pBlock = pStyle->m_vecPool[IP_ENABLE][1];
		if( pBlock )
		{
			GUI_BLOCK& Block = *pBlock;
			float TopLeftH		= Block[GBM_TOP_LEFT].Height();
			float BottomRightH	= Block[GBM_BOTTOM_RIGHT].Height();
			heigh = heigh - TopLeftH - BottomRightH;
		}
	}
	return heigh;
}

float SQRListCtrl::GetWorkAreaOrgWidth()
{
	StyleStruct* pStyle = GetStyleStruct();
	uint32 Style = m_hWnd->m_StyleIndex;
	GUI_BLOCK* pBlock = NULL;

	float width = GetWndOrgWidth();

	if ( Style>0 && NULL != pStyle )
	{
		pBlock = pStyle->m_vecPool[IP_ENABLE][Style];
		if(pBlock == NULL)
			pBlock = pStyle->m_vecPool[IP_ENABLE][1];
		if( pBlock )
		{
			GUI_BLOCK& Block = *pBlock;
			float TopLeftW		= Block[GBM_TOP_LEFT].Width();
			float BottomRightW	= Block[GBM_BOTTOM_RIGHT].Width();
			width = width - TopLeftW - BottomRightW;
		}
	}
	return width;
}

CFRect SQRListCtrl::GetWorkAreaOrgRect()
{
	StyleStruct* pStyle = GetStyleStruct();
	uint32 Style = m_hWnd->m_StyleIndex;
	GUI_BLOCK* pBlock = NULL;

	CFRect wndRect = CFRect(0, 0, GetWndOrgWidth(), GetWndOrgHeight());
	if ( Style>0 && NULL != pStyle )
	{
		pBlock = pStyle->m_vecPool[IP_ENABLE][Style];
		if(pBlock == NULL)
			pBlock = pStyle->m_vecPool[IP_ENABLE][1];
		if( pBlock )
		{
			GUI_BLOCK& Block = *pBlock;
			float TopLeftH		= Block[GBM_TOP_LEFT].Height();
			float TopLeftW		= Block[GBM_TOP_LEFT].Width();
			float BottomRightW	= Block[GBM_BOTTOM_RIGHT].Width();
			float BottomRightH	= Block[GBM_BOTTOM_RIGHT].Height();

			wndRect.left += TopLeftW;
			wndRect.top += TopLeftH;
			wndRect.bottom -= BottomRightH;
			wndRect.right -= BottomRightW;
		}
	}
	return wndRect;
}

void SQRListCtrl::GetWorkAreaOrgRectEx(CFRect& rect)
{
	rect = GetWorkAreaOrgRect();
}

void SQRListCtrl::GetCutAreaRect( CFRect& rt )
{
	StyleStruct* pStyle = GetStyleStruct();
	uint32 Style = m_hWnd->m_StyleIndex;
	GUI_BLOCK* pBlock = NULL;

	GetWndRect(rt);
	if ( Style>0 && NULL != pStyle )
	{
		pBlock = pStyle->m_vecPool[IP_ENABLE][Style];
		if(pBlock == NULL)
			pBlock = pStyle->m_vecPool[IP_ENABLE][1];
		if( pBlock )
		{
			GUI_BLOCK& Block = *pBlock;
			float t_Zoom = m_hWnd->m_pGUI->GetZoomSize();
			float TopLeftH		= Block[GBM_TOP_LEFT].Height()* t_Zoom;
			float TopLeftW		= Block[GBM_TOP_LEFT].Width() * t_Zoom;
			float BottomRightW	= Block[GBM_BOTTOM_RIGHT].Width() * t_Zoom;
			float BottomRightH	= Block[GBM_BOTTOM_RIGHT].Height()* t_Zoom;

			rt.left += TopLeftW;
			rt.top += TopLeftH;
			rt.bottom -= BottomRightH;
			rt.right -= BottomRightW;
		}
	}
}

CFPos SQRListCtrl::GetWorkAreaOffset()
{
	StyleStruct* pStyle = GetStyleStruct();
	uint32 Style = m_hWnd->m_StyleIndex;
	GUI_BLOCK* pBlock = NULL;

	CFPos posOffset = CFPos( 0.0f, 0.0f );
	if ( Style>0 && NULL != pStyle )
	{
		pBlock = pStyle->m_vecPool[IP_ENABLE][Style];
		if(pBlock == NULL)
			pBlock = pStyle->m_vecPool[IP_ENABLE][1];
		if( pBlock )
		{
			GUI_BLOCK& Block = *pBlock;
			float t_Zoom = m_hWnd->m_pGUI->GetZoomSize();
			float TopLeftH		= Block[GBM_TOP_LEFT].Height()* t_Zoom;
			float TopLeftW		= Block[GBM_TOP_LEFT].Width() * t_Zoom;
			float BottomRightW	= Block[GBM_BOTTOM_RIGHT].Width() * t_Zoom;
			float BottomRightH	= Block[GBM_BOTTOM_RIGHT].Height()* t_Zoom;

			posOffset.x += TopLeftW;
			posOffset.y += TopLeftH;
		}
	}
	return posOffset;
}


void SQRListCtrl::ResetItemRect( bool bSetScroll,bool refreshbak )
{
	ListContainer& listControl = __LI( m_hList );
	float Zoom = m_hWnd->m_pGUI->GetZoomSize();
	// 统计所有item的总显示宽度和总显示高度
	float fTotalWidth = 0.0f, fTotalHeight = 0.0f;
	for( size_t i = 0; i < listControl.m_ListWidth.size(); i++ )
		fTotalWidth += listControl.m_ListWidth[i];
	for( size_t i = 0; i < listControl.m_ListItem.size(); i++ )	
		fTotalHeight += listControl.m_ListItem[i]->m_fHeight; 

	fTotalHeight *= Zoom;
	fTotalWidth  *= Zoom;
	float fRangeRow = 0.0f, fRangeLine = 0.0f, fPageRow = 0.0f, fPageLine = 0.0f;
	if( GetStyle() & LCTL_SCLSMOOTH )
	{
		fPageRow	= GetWorkAreaWidth();
		fPageLine	= GetWorkAreaHeight();
		fRangeRow	= fTotalWidth;
		fRangeLine	= fTotalHeight;
	}
	else
	{
		int32 nEndRow = 0, nEndLine = 0;
		float fCurWith = 0.0f, fCurHeight = 0.0f;
		for(nEndRow = (int32)listControl.m_ListWidth.size(); nEndRow > 0; nEndRow--)
		{
			fCurWith += listControl.m_ListWidth[nEndRow - 1] * Zoom;
			if(fCurWith > GetWorkAreaWidth())
				break;
		}
		for(nEndLine = (int32)listControl.m_ListItem.size(); nEndLine > 0; nEndLine--)
		{
			fCurHeight += listControl.m_ListItem[nEndLine - 1]->m_fHeight * Zoom;
			if(fCurHeight > GetWorkAreaHeight())
				break;
		}
		fRangeRow	= (float)listControl.m_ListWidth.size();
		fRangeLine	= (float)listControl.m_ListItem.size();
		fPageRow	= fRangeRow - nEndRow;
		fPageLine	= fRangeLine - nEndLine;
	}

	if( bSetScroll )
	{
		m_sbHorizon.SetRange( max( (int32)fRangeRow, 1 ) );
		m_sbVertical.SetRange( max( (int32)fRangeLine, 1 ) );
		m_sbHorizon.SetPage( max( (int32)fPageRow, 1 ) );
		m_sbVertical.SetPage( max( (int32)fPageLine, 1 ) );
		int32 hWide = m_sbHorizon.GetRange() - m_sbHorizon.GetPage();
		int32 vWide = m_sbVertical.GetRange() - m_sbVertical.GetPage();
		if (m_sbHorizon.GetPos() > hWide)
			m_sbHorizon.SetPos(hWide);
		if (m_sbVertical.GetPos() > vWide)
			m_sbVertical.SetPos(vWide);
	}
	SetMouseWheel(true);
	bool bShowNotAuto = !( GetStyle()&LCTL_SHOWSCLAUTO );
	m_sbHorizon.ShowWnd( ( GetStyle()&WS_HORSCROLL ) && ( fTotalWidth > GetWorkAreaWidth() || bShowNotAuto ) );
	m_sbVertical.ShowWnd( ( GetStyle()&WS_VERSCROLL) && ( fTotalHeight > GetWorkAreaHeight() || bShowNotAuto ) );

	CFRect RtTemp;
	m_sbVertical.GetWndRect(RtTemp);
	RtTemp.bottom = RtTemp.top + GetWndHeight();
	m_sbVertical.ResizeRect(RtTemp,refreshbak);

	CFRect wndRect = GetWorkAreaRect();
	CFPos posOffset = GetWorkAreaOffset();
	float posOffsetx = posOffset.x;
	float posOffsety = posOffset.y;

	for( size_t i = 0; i < listControl.m_ListItem.size(); i++ )
	{
		posOffset.x = posOffsetx;
		for( size_t j = 0; j < listControl.m_ListItem[i]->m_arrItem.size(); j++ )
		{
			CFRect rt;
			if( GetStyle()&LCTL_SCLSMOOTH )
			{
				rt.left = posOffset.x - m_sbHorizon.GetPos();
				rt.top  = posOffset.y - m_sbVertical.GetPos();
			}
			else
			{
				if(static_cast<int32>(j) < m_sbHorizon.GetPos() )
					posOffset.x -= listControl.m_ListWidth[j] * Zoom;
				rt.left = posOffset.x;
				if(static_cast<int32>(i) < m_sbVertical.GetPos())
					posOffset.y -= listControl.m_ListItem[i]->m_fHeight * Zoom;
				rt.top = posOffset.y;
			}
			rt.right	= rt.left + static_cast<int32>(listControl.m_ListWidth[j] * Zoom + 0.5f);
			rt.bottom	= rt.top + static_cast<int32>(listControl.m_ListItem[i]->m_fHeight * Zoom + 0.5f);

			CFRect cliprt = wndRect.Intersection(rt);
			if(cliprt.IsEmpty()||cliprt.Height() < 1.0f)
			{
				listControl.m_ListItem[i]->m_arrItem[j].m_pWnd->ShowWnd(false);
			}
			else
			{
				listControl.m_ListItem[i]->m_arrItem[j].m_pWnd->ShowWnd(true);
				WndToScreen(rt);
				//listControl.m_ListItem[i]->m_arrItem[j].m_pWnd->ResizeRect(rt, refreshbak);
				listControl.m_ListItem[i]->m_arrItem[j].m_pWnd->SetWndRect(rt);
			}
			posOffset.x += listControl.m_ListWidth[j] * Zoom ; 
		}
		posOffset.y += listControl.m_ListItem[i]->m_fHeight * Zoom;
	}
}

int32 SQRListCtrl::InsertColumn( int32 nCol, float fWidth )
{
	ListContainer& ListC = __LI( m_hList );
	
	nCol = (int32)max( (uint32)nCol, ListC.m_ListWidth.size() );
	ListC.m_ListWidth.insert( ListC.m_ListWidth.begin() + nCol, fWidth );

	float nTotalHeight	= 0;
	float nTotalWidth	= 0;
	for( size_t i = 0; i < ListC.m_ListWidth.size(); i++ )
		nTotalWidth += ListC.m_ListWidth[i];

	for( size_t i = 0; i < ListC.m_ListItem.size(); i++ )
	{
		IMAGE_PARAM IP = IMAGE_PARAM( SM_BS_BK, IP_ENABLE );
		SQRItemWnd* pItem = new SQRItemWnd;

		nTotalHeight += i == 0 ? 0 : ListC.m_ListItem[i-1]->m_fHeight;
		float x = nTotalWidth;
		float y = nTotalHeight;
		float fHeight = ListC.m_ListItem[i]->m_fHeight;

		WndCreateParam param;
		param.szTitle = "test_";
		param.uStyle = GetStyle() & WS_MULLINETEXT;
		param.x = x;
		param.y = y;
		param.width = fWidth;
		param.height= fHeight;
		param.pParentWnd = this;
		param.nFontID = GetFontID();
		param.font_size = GetFontSize();
		pItem->Create( param );

		pItem->SetWndBkImage( IP, m_ItemUnselImage );
		pItem->SetWndTextColor( IP, m_ItemUnselColor );
		pItem->SetWndTextBcColor( IP, m_ItemUnselBkColor );
		IP.StateMask = IP_DISABLE;
		pItem->SetWndBkImage( IP, m_ItemUnselImage );
		pItem->SetWndTextColor( IP, m_ItemUnselColor );
		pItem->SetWndTextBcColor( IP, m_ItemUnselBkColor );
		IP.StateMask = IP_MOUSEOVER;
		pItem->SetWndBkImage( IP, m_ItemMouseOverImage );
		pItem->SetWndTextColor( IP, m_ItemMouseOverColor );
		pItem->SetWndTextBcColor( IP, m_ItemMouseOverBkColor );
		IP.StateMask = IP_CLICKDOWN;
		pItem->SetWndBkImage( IP, m_ItemSelImage );
		pItem->SetWndTextColor( IP, m_ItemSelColor );
		pItem->SetWndTextBcColor( IP, m_ItemSelBkColor );

		ListC.m_ListItem[i]->m_arrItem.insert( ListC.m_ListItem[i]->m_arrItem.begin() + nCol, pItem );
	}

    ResetItemRect(true,true);
    return nCol;
}

void SQRListCtrl::SetItemHeight( float fHeight, int32 nItem )
{
	if( fHeight < 0.0f )
		return;

	ListContainer& ListC = __LI( m_hList );
	if( nItem < 0 || nItem >= (int32)ListC.m_ListItem.size() )
	{
		for( size_t i = 0; i < ListC.m_ListItem.size(); i++ )
			ListC.m_ListItem[i]->m_fHeight = fHeight;
	}
	else
	{
		ListC.m_ListItem[nItem]->m_fHeight = fHeight;
	}

	ResetItemRect(true,true);
}

int32 SQRListCtrl::InsertItem( int32 nItem, float fHeight )
{
	ListContainer& ListC = __LI( m_hList );
	if( !ListC.m_ListWidth.size() )
		return -1;

	nItem = (int32)min( (uint32)nItem, ListC.m_ListItem.size() );

	if ( fHeight < 0.0f )
		fHeight = 20.0f;//给个默认值

	ListItem* pItem = new ListItem;
	pItem->m_arrItem.resize( ListC.m_ListWidth.size() );
	pItem->m_fHeight = fHeight;

	float fTotalWidth	= 0.0f;	
	float fTotalHeight	= 0.0f;
	for( size_t i = 0; i < ListC.m_ListItem.size(); i++ )
		fTotalHeight += ListC.m_ListItem[i]->m_fHeight; 
	
	for( size_t i = 0; i < pItem->m_arrItem.size(); i++ )
	{
		pItem->m_arrItem[i] = ItemWnd( new SQRItemWnd );
		pItem->m_bCItemWnd = true;
		IMAGE_PARAM IP = IMAGE_PARAM( SM_BS_BK, IP_ENABLE );

		fTotalWidth += i == 0 ? 0 : ListC.m_ListWidth[i-1];
		float x = fTotalWidth;
		float y = fTotalHeight;
		float fWidth = ListC.m_ListWidth[i];

		WndCreateParam param;
		param.uStyle = GetStyle() & WS_MULLINETEXT;
		param.x = x;
		param.y = y;
		param.width = fWidth;
		param.height = fHeight;
		param.pParentWnd = this;
		param.nFontID = GetFontID();
		param.font_size = GetFontSize();
		pItem->m_arrItem[i].m_pWnd->Create(param);
		pItem->m_arrItem[i].m_pWnd->SetIsBottom( this->GetIsBottom() );
		pItem->m_arrItem[i].m_pWnd->SetMouseWheel(true);
		pItem->m_arrItem[i].m_pWnd->SetWndBkImage( IP, m_ItemUnselImage );
		pItem->m_arrItem[i].m_pWnd->SetWndTextColor( IP, m_ItemUnselColor );
		pItem->m_arrItem[i].m_pWnd->SetWndTextBcColor( IP, m_ItemUnselBkColor );
		IP.StateMask = IP_DISABLE;
		pItem->m_arrItem[i].m_pWnd->SetWndBkImage( IP, m_ItemUnselImage );
		pItem->m_arrItem[i].m_pWnd->SetWndTextColor( IP, m_ItemUnselColor );
		pItem->m_arrItem[i].m_pWnd->SetWndTextBcColor( IP, m_ItemUnselBkColor );
		IP.StateMask = IP_MOUSEOVER;
		pItem->m_arrItem[i].m_pWnd->SetWndBkImage( IP, m_ItemMouseOverImage );
		pItem->m_arrItem[i].m_pWnd->SetWndTextColor( IP, m_ItemMouseOverColor );
		pItem->m_arrItem[i].m_pWnd->SetWndTextBcColor( IP, m_ItemMouseOverBkColor );
		IP.StateMask = IP_CLICKDOWN;
		pItem->m_arrItem[i].m_pWnd->SetWndBkImage( IP, m_ItemSelImage );
		pItem->m_arrItem[i].m_pWnd->SetWndTextColor( IP, m_ItemSelColor );
		pItem->m_arrItem[i].m_pWnd->SetWndTextBcColor( IP, m_ItemSelBkColor );
	}

	ListC.m_ListItem.insert( ListC.m_ListItem.begin() + nItem, pItem );
    ResetItemRect(true,true);
    return nItem;
}

int32 SQRListCtrl::InsertItemFromRes( int32 nItem, const char* szRes )
{
	ListContainer& ListC = __LI( m_hList );
	if( !ListC.m_ListWidth.size() )
		return -1;

	nItem = (int32)min( (uint32)nItem, ListC.m_ListItem.size() );

	ListItem* pItem = new ListItem;
	pItem->m_arrItem.resize( ListC.m_ListWidth.size() );

	for( size_t i = 0; i < pItem->m_arrItem.size(); i++ )
	{
		if( SQRItemWnd::classSQRItemWnd.FindParentClass( m_hWnd->m_pGUI->GetWndRes( szRes )->m_ClassName ) )
			pItem->m_arrItem[i] = ItemWnd( new SQRItemWnd );
		else
			pItem->m_arrItem[i] = ItemWnd( SQRWndClass::CreateWndObject( m_hWnd->m_pGUI->GetWndRes( szRes )->m_ClassName ) );
		pItem->m_bCItemWnd = pItem->m_arrItem[i].m_pWnd->FindParentClass( "SQRItemWnd" );
	}

	ListC.m_ListItem.insert( ListC.m_ListItem.begin() + nItem, pItem );

	ResetItemRect(true,true);
	return nItem;
}

void SQRListCtrl::UpdateItemImage()
{
	ListContainer& ListC = __LI( m_hList );

	for( size_t i = 0; i < ListC.m_ListItem.size(); i++ )
	{
		IMAGE_PARAM IP = IMAGE_PARAM( SM_BS_BK, IP_ENABLE );
		for ( size_t j = 0; j < ListC.m_ListItem[i]->m_arrItem.size(); j++ )
		{
			SQRItemWnd* pItem = (SQRItemWnd*)( ListC.m_ListItem[i]->m_arrItem[j].m_pWnd );

			pItem->SetWndBkImage( IP, m_ItemUnselImage );
			pItem->SetWndTextColor( IP, m_ItemUnselColor );
			pItem->SetWndTextBcColor( IP, m_ItemUnselBkColor );

			IP.StateMask = IP_DISABLE;
			pItem->SetWndBkImage( IP, m_ItemUnselImage );
			pItem->SetWndTextColor( IP, m_ItemUnselColor );
			pItem->SetWndTextBcColor( IP, m_ItemUnselBkColor );

			IP.StateMask = IP_MOUSEOVER;
			pItem->SetWndBkImage( IP, m_ItemMouseOverImage );
			pItem->SetWndTextColor( IP, m_ItemMouseOverColor );
			pItem->SetWndTextBcColor( IP, m_ItemMouseOverBkColor );

			IP.StateMask = IP_CLICKDOWN;
			pItem->SetWndBkImage( IP, m_ItemSelImage );
			pItem->SetWndTextColor( IP, m_ItemSelColor );
			pItem->SetWndTextBcColor( IP, m_ItemSelBkColor );
		}
	}
}

void SQRListCtrl::SetItemHighLight(int32 index)
{
	ListContainer& ListC = __LI( m_hList );
	if( -1 != m_keyboardCount)
	{
		SQRItemWnd* pValue = (SQRItemWnd*)(ListC.m_ListItem[m_keyboardCount]->m_arrItem[0].m_pWnd);
		if( m_keyboardCount == m_mouseCount )
		{
			pValue->CancelMouseMove();
		}
		else
		{
			pValue->CancelHighLight();
		}
		m_keyboardCount += static_cast<int8>(index);	
	}
	else 
	{
		m_keyboardCount = static_cast<int8>(-1 == index ? ListC.m_ListItem.size()-1 : 0);  
	}	
	if( m_keyboardCount < 0 )
		m_keyboardCount = 0;
	else if( m_keyboardCount > static_cast<uint8>(ListC.m_ListItem.size() - 1) )
		m_keyboardCount = static_cast<uint8>(ListC.m_ListItem.size() - 1);
	
	SQRItemWnd* pValue = (SQRItemWnd*)(ListC.m_ListItem[m_keyboardCount]->m_arrItem[0].m_pWnd);
	pValue->SetHighLight();
}

void SQRListCtrl::SelectItem( int32 nItem )
{
	ListContainer& ListC = __LI( m_hList );
	if( nItem >= 0 && nItem < (int32)ListC.m_ListItem.size() )
	{
		bool bSelected = true;
		for( size_t i = 0; i < ListC.m_ListItem[nItem]->m_arrItem.size(); i++ )
			if( !ListC.m_ListItem[nItem]->m_arrItem[i].m_bSel )
				bSelected = false;

		if( !bSelected )
		{
			if( !(GetStyle()&LCTL_MULTSEL) )
			{
				for( size_t i = 0; i < ListC.m_ListItem.size(); i++ )
				{
					if( ListC.m_ListItem[i]->m_bCItemWnd )
					{
						for( size_t j = 0; j < ListC.m_ListItem[i]->m_arrItem.size(); j++ )
						{
							ListC.m_ListItem[i]->m_arrItem[j].m_bSel = false;
							if( ListC.m_ListItem[i]->m_bCItemWnd )
                                ( (SQRItemWnd*)ListC.m_ListItem[i]->m_arrItem[j].m_pWnd )->SetSelected( false );
						}
					}
				}
			}

			for( size_t i = 0; i < ListC.m_ListItem[nItem]->m_arrItem.size(); i++ )
			{
				ListC.m_ListItem[nItem]->m_arrItem[i].m_bSel = true;
				if( ListC.m_ListItem[nItem]->m_bCItemWnd )
					( (SQRItemWnd*)ListC.m_ListItem[nItem]->m_arrItem[i].m_pWnd )->SetSelected( true );
			}
				
			SendCtrlMsg( ITEM_LBUTTONUP, nItem, 0 );
		}
	}
}


void SQRListCtrl::DeleteAllItem()
{
	ListContainer& ListC = __LI( m_hList );
	for( size_t i = 0; i < ListC.m_ListItem.size(); i++ )
	{
		GfkSafeDelete( ListC.m_ListItem[i] );
	}
	ListC.m_ListItem.clear();
	ListC.m_ListWidth.clear();
}


void SQRListCtrl::DeleteItem( int32 nItem )
{
	--nItem;
	ListContainer& ListC = __LI( m_hList );
	if( nItem < 0 || nItem >= (int32)ListC.m_ListItem.size() )
		return;

	//这里移动位置
	CFRect oldrt;
	CFRect rt;
	oldrt.left = -1.0f;
	if( nItem < static_cast<int32>(ListC.m_ListItem.size() - 1) )
	{
		ListC.m_ListItem[nItem]->m_arrItem[0].m_pWnd->GetWndRect(oldrt);
		for( size_t i = nItem+1; i < ListC.m_ListItem.size(); ++i )
		{	
			ListC.m_ListItem[i]->m_arrItem[0].m_pWnd->GetWndRect(rt);
			ListC.m_ListItem[i]->m_arrItem[0].m_pWnd->SetWndRect(oldrt);
			oldrt = rt;
		}
	}
	GfkSafeDelete( ListC.m_ListItem[nItem] );
	ListC.m_ListItem.erase( ListC.m_ListItem.begin() + nItem);
	ResetItemRect(true,true);
}

void SQRListCtrl::DeleteSubItem(int row, int col)
{
	ListContainer& ListC = __LI( m_hList );
	if( row < 0 || row >= (int32)ListC.m_ListItem.size() )
		return;
	GfkSafeDelete( ListC.m_ListItem[row]->m_arrItem[col].m_pWnd );
	ListC.m_ListItem[row]->m_arrItem.erase(ListC.m_ListItem[row]->m_arrItem.begin() + col);
	ResetItemRect(true,true);
}

int32 SQRListCtrl::GetSelectItem( int32 nPreItem )
{
	ListContainer& ListC = __LI( m_hList );
	int32 i = nPreItem + 1;
	for( ; i < (int32)ListC.m_ListItem.size(); i++ )
	{
		for( size_t j = 0; j < ListC.m_ListItem[i]->m_arrItem.size(); j++ )
			if( ListC.m_ListItem[i]->m_arrItem[j].m_bSel )
				return i;
	}

	return -1;
}

int32 SQRListCtrl::GetSelectSubItem( int32 nItem, int32 nPreSubItem )
{
	ListContainer& ListC = __LI( m_hList );
	if( nItem < 0 || nItem >= (int32)ListC.m_ListItem.size() || !ListC.m_ListItem[nItem]->m_bCItemWnd )
		return -1;
	for(int32 i = nPreSubItem + 1; i < (int32)ListC.m_ListItem[nItem]->m_arrItem.size(); i++ )
	{
		if( ListC.m_ListItem[nItem]->m_arrItem[i].m_bSel )
			return i;
	}

	return -1;
}

int32 SQRListCtrl::GetItemCount()
{
	return (int32)__LI( m_hList ).m_ListItem.size();
}

int32 SQRListCtrl::GetSubItemCount()
{
	return (int32)__LI( m_hList ).m_ListWidth.size();
}

SQRWnd* SQRListCtrl::GetSubItem( int32 nItem, int32 nSubItem )
{
	ListContainer& ListC = __LI( m_hList );
	if (!(uint32(nItem) < ListC.m_ListItem.size() && uint32(nSubItem) < ListC.m_ListWidth.size()))
	{
		stringstream ExpStr;
		ExpStr << "nItem:" << nItem << "; Item Size:" << ListC.m_ListItem.size() 
			<< "; nSubItem:" << nSubItem << "; Width:" << ListC.m_ListWidth.size() << "\n";
		GenErr("SQRListCtrl GetItem Error", ExpStr.str());
	}
	//Ast( uint32(nItem) < ListC.m_ListItem.size() && uint32(nSubItem) < ListC.m_ListWidth.size() );
	return ListC.m_ListItem[nItem]->m_arrItem[nSubItem].m_pWnd;
}
	
void SQRListCtrl::OnCtrlmsg( SQRWnd* pChild, uint32 uMsgID, uint32 wParam, uint32 lParam )
{
	if (uMsgID ==SCROLL_MOVE)
	{
		//if( pChild == &m_sbHorizon || pChild == &m_sbVertical )
		ResetItemRect( false ,true);
	}
	else if (uMsgID == BUTTON_DRAG)
	{
		if (GetKeyState(VK_SHIFT) & 0x8000)
			return;
		ListContainer& ListC = __LI( m_hList );
		ItemWnd* pSel = NULL;
		uint32 uParam1 = 0;
		uint32 uParam2 = 0;
		for( uParam1 = 0; uParam1 < ListC.m_ListItem.size() && !pSel; pSel ? uParam1 : uParam1++ )
			for( uParam2 = 0; uParam2 < (int32)ListC.m_ListItem[uParam1]->m_arrItem.size() && !pSel; pSel ? uParam2 : uParam2++ )
				if( ListC.m_ListItem[uParam1]->m_arrItem[uParam2].m_pWnd == pChild )
					pSel = &( ListC.m_ListItem[uParam1]->m_arrItem[uParam2] );
		SendCtrlMsg( BUTTON_DRAG, uParam1, uParam2 );
	}
	else if( uMsgID == WND_NOTIFY )
	{
		if( (WM_LBUTTONUP == wParam || WM_RBUTTONUP == wParam || WM_LBUTTONDBLCLK == wParam) && (pChild != &m_sbHorizon && pChild != &m_sbVertical) ) 
		{
			ListContainer& ListC = __LI( m_hList );
			ItemWnd* pSel = NULL;
			uint32 uParam1 = 0;
			uint32 uParam2 = 0;
			for( uParam1 = 0; uParam1 < ListC.m_ListItem.size() && !pSel; pSel ? uParam1 : uParam1++ )
				for( uParam2 = 0; uParam2 < (int32)ListC.m_ListItem[uParam1]->m_arrItem.size() && !pSel; pSel ? uParam2 : uParam2++ )
					if( ListC.m_ListItem[uParam1]->m_arrItem[uParam2].m_pWnd == pChild )
						pSel = &( ListC.m_ListItem[uParam1]->m_arrItem[uParam2] );

			if( !(GetStyle()&LCTL_MULTSEL) || !pSel )
			{
				for( size_t i = 0; i < ListC.m_ListItem.size(); i++ )
				{
					for( size_t j = 0; j < ListC.m_ListItem[i]->m_arrItem.size(); j++ )
					{
						ListC.m_ListItem[i]->m_arrItem[j].m_bSel = false;
						if( ListC.m_ListItem[i]->m_bCItemWnd )
							( (SQRItemWnd*)ListC.m_ListItem[i]->m_arrItem[j].m_pWnd )->SetSelected( false );
					}
				}
			}

			if( pSel )
			{
				pSel->m_bSel = true;
				if( ListC.m_ListItem[uParam1]->m_bCItemWnd )
					((SQRItemWnd*)pSel->m_pWnd)->SetSelected( true );

				if( GetStyle()&LCTL_SELFULLLINE )
				{
					for( size_t i = 0; i < ListC.m_ListItem[uParam1]->m_arrItem.size(); i++ )
					{
						ListC.m_ListItem[uParam1]->m_arrItem[i].m_bSel = true;
						if( ListC.m_ListItem[uParam1]->m_bCItemWnd )
							( (SQRItemWnd*)ListC.m_ListItem[uParam1]->m_arrItem[i].m_pWnd )->SetSelected( true );
					}
				}
				if (GetKeyState(VK_SHIFT) & 0x8000)
				{
					pSel->m_bSel = true;
					if( ListC.m_ListItem[uParam1]->m_bCItemWnd )
						((SQRItemWnd*)pSel->m_pWnd)->SetSelected( true );
					if ( WM_LBUTTONUP == wParam)
					{
						SendCtrlMsg( ITEM_SHIFTDOWN, uParam1, uParam2 );
					}
					else if(WM_RBUTTONUP == wParam)
					{
						SendCtrlMsg( ITEM_SHIFTRDOWN, uParam1, uParam2 );
					}

				}
				else if(GetKeyState(VK_CONTROL) & 0x8000)
				{
					if ( WM_LBUTTONUP == wParam)
					{
						SendCtrlMsg(ITEM_CTRLDOWN,uParam1,uParam2);
					}
				}
				else if( WM_LBUTTONUP == wParam )
				{
					if (pChild == m_hWnd->m_pGUI->GetLastLButtonDownWnd())
						SendCtrlMsg( ITEM_LBUTTONCLICK, uParam1, uParam2 );
					else
						SendCtrlMsg( ITEM_LBUTTONUP, uParam1, uParam2 );
				}
				else if( WM_LBUTTONDBLCLK == wParam )
				{
					SendCtrlMsg( ITEM_LBUTTONDBLCLICK, uParam1, uParam2 );
				}
				else if ( WM_RBUTTONUP == wParam )
				{
					if (pChild == m_hWnd->m_pGUI->GetLastRButtonDownWnd())
						SendCtrlMsg( ITEM_RBUTTONCLICK, uParam1, uParam2 );
					else
						SendCtrlMsg( ITEM_RBUTTONUP, uParam1, uParam2 );
				}
			}
		}
		else if (WM_MOUSEMOVE == wParam)
		{
			ListContainer& ListC = __LI( m_hList );
			for( size_t i = 0; i < ListC.m_ListItem.size(); ++i )
			{	
				((SQRItemWnd*)(ListC.m_ListItem[i]->m_arrItem[0].m_pWnd))->CancelHighLight();
				m_keyboardCount = -1;
			}

			ItemWnd* pSel = NULL;
			uint32 uParam1 = 0;
			uint32 uParam2 = 0;
			for( uParam1 = 0; uParam1 < ListC.m_ListItem.size() && !pSel; pSel ? uParam1 : uParam1++ )
			{
				for( uParam2 = 0; uParam2 < (int32)ListC.m_ListItem[uParam1]->m_arrItem.size() && !pSel; pSel ? uParam2 : uParam2++ )
				{
					if( ListC.m_ListItem[uParam1]->m_arrItem[uParam2].m_pWnd == pChild )
					{
						pSel = &( ListC.m_ListItem[uParam1]->m_arrItem[uParam2] );
						if ( pSel )
						{
							if ( !(m_uLastTimeHighLightX == uParam1 && m_uLastTimeHighLightY == uParam2) )
							{
								SendCtrlMsg( ITEM_SELECT_CHANGED, uParam1, uParam2 );
								m_uLastTimeHighLightX = uParam1;
								m_uLastTimeHighLightY = uParam2;
							}
						}
					}
				}
			}
		}
		else
		{
			ListContainer& ListC = __LI( m_hList );
			for( size_t i = 0; i < ListC.m_ListItem.size(); ++i )
			{	
				((SQRItemWnd*)(ListC.m_ListItem[i]->m_arrItem[0].m_pWnd))->CancelHighLight();
				m_keyboardCount = -1;
			}
		}
	}
}

void SQRListCtrl::OnMouseWheel( uint32 nFlags, short zDelta, int32 x, int32 y )
{
	if ( GetStyle() & WS_IGNOREWHEEL )
		SQRWnd::OnMouseWheel( nFlags, zDelta, x, y );
	else if ( GetStyle()&WS_VERSCROLL )
		m_sbVertical.SendMsg( WM_MOUSEWHEEL, MAKELONG( nFlags, zDelta ), MAKELONG( x, y ) );
}

void SQRListCtrl::OnLButtonUp(uint32 nFlags, int32 x, int32 y)
{
	uint32 uMsgID = LISTCTRL_LEFTCLICKUP;
	SendCtrlMsg( uMsgID, uint32(GetLogicCursorPos().x), uint32(GetLogicCursorPos().y) );	
}

void SQRListCtrl::OnLButtonClick(uint32 nFlags, int32 x, int32 y)
{
	uint32 uMsgID = LISTCTRL_LEFTCLICK;
	SendCtrlMsg( uMsgID, uint32(GetLogicCursorPos().x), uint32(GetLogicCursorPos().y) );	
}

void SQRListCtrl::OnRButtonUp(uint32 nFlags, int32 x, int32 y)
{
	uint32 uMsgID = LISTCTRL_RIGHTCLICKUP;
	SendCtrlMsg( uMsgID, uint32(GetLogicCursorPos().x), uint32(GetLogicCursorPos().y) );	
}

void SQRListCtrl::OnRButtonClick(uint32 nFlags, int32 x, int32 y)
{
	uint32 uMsgID = LISTCTRL_RIGHTCLICK;
	SendCtrlMsg( uMsgID, uint32(GetLogicCursorPos().x), uint32(GetLogicCursorPos().y) );	
}

int32 SQRListCtrl::GetHighLightIndex()
{
	ListContainer& ListC = __LI( m_hList );
	if( -1 != m_keyboardCount )
		return (int32)m_keyboardCount;
	else
	{
		for( size_t i = 0; i < ListC.m_ListItem.size(); ++i )
		{
			if ( 2 == ((SQRItemWnd*)(ListC.m_ListItem[i]->m_arrItem[0].m_pWnd))->GetHighLightState() )
			{
				((SQRItemWnd*)(ListC.m_ListItem[i]->m_arrItem[0].m_pWnd))->CancelMouseMove();
				m_keyboardCount = static_cast<int8>(i);
				m_mouseCount    = static_cast<int8>(i);
				return (int32)m_keyboardCount;
			}
		}
	}
	return -1;
}

bool SQRListCtrl::Isbottom() 
{
	if ( (GetStyle() & WS_VERSCROLL) && m_sbVertical.IsShow())
		return m_sbVertical.Isbottom();
	else if( (GetStyle() & WS_HORSCROLL) && m_sbHorizon.IsShow())
		return m_sbHorizon.Isbottom();
	else
		return false;
}

bool SQRListCtrl::Istop()
{
	if ( (GetStyle() & WS_VERSCROLL) && m_sbVertical.IsShow())
		return m_sbVertical.Istop();
	else if( (GetStyle() & WS_HORSCROLL) && m_sbHorizon.IsShow())
		return m_sbHorizon.Istop();
	else
		return true;
}

void SQRListCtrl::SetVerticalPos(int32 nPos)
{
	m_sbVertical.SetPos(nPos);
}

int32 SQRListCtrl::GetVerticalPos()
{
	return m_sbVertical.GetPos();
}

void SQRListCtrl::ReleaseWndTex()
{
	// 释放可以释放的UI贴图资源
	m_ItemSelImage.ReleaseTex();
	m_ItemUnselImage.ReleaseTex();
	m_ItemMouseOverImage.ReleaseTex();
	SQRControl::ReleaseWndTex();
}
void SQRListCtrl::CreatWndTex()
{
	// 显示之前申请资源
	if ( m_FlushDlg == NULL )
	{
		m_FlushDlg = GetFlushDialog();
		if ( m_FlushDlg == NULL ) return;
	}
	m_ItemSelImage.CreatTex( GetGraphic(), m_FlushDlg );
	m_ItemUnselImage.CreatTex( GetGraphic(), m_FlushDlg );
	m_ItemMouseOverImage.CreatTex( GetGraphic(), m_FlushDlg );
	SQRControl::CreatWndTex();
}


WND_IMAGE_LIST* SQRListCtrl::GetWndBkImage( const IMAGE_PARAM& ImageFlag )
{
	IMAGE_PARAM Flag = ImageFlag;
	if( 0 == Flag.CtrlMask )
		return NULL;
	else if( SM_LB_BK == Flag.CtrlMask )
	{
		if( IP_LS_SEL == Flag.StateMask )
			return &m_ItemSelImage;
		if( IP_LS_UNSEL == Flag.StateMask )
			return &m_ItemUnselImage;
		if( IP_MOUSEOVER == Flag.StateMask )
			return &m_ItemMouseOverImage;

		Flag.CtrlMask -= SM_LB_BK - SM_BS_BK;
		return SQRWnd::GetWndBkImage( Flag );
	}
	else if( Flag.CtrlMask < SM_LB_SCV_BK )
	{
		Flag.CtrlMask -= SM_LB_SCH_BK - SM_SC_BK;
		return m_sbHorizon.GetWndBkImage( Flag );
	}
	else
	{
		Flag.CtrlMask -= SM_LB_SCV_BK - SM_SC_BK;
		return m_sbVertical.GetWndBkImage( Flag );
	}
}


bool SQRListCtrl::IsHasEventState(EventStateMask Mask)
{
	if(	IP_LS_SEL		== Mask ||
		IP_MOUSEOVER	== Mask ||
		IP_LS_UNSEL		== Mask )
		return true;
	return SQRWnd::IsHasEventState(Mask);
}

bool SQRListCtrl::ObjHasEventState(EventStateMask Mask)
{
	return SQRListCtrl::IsHasEventState(Mask);
}

uint32 SQRListCtrl::GetWndTextColor( const IMAGE_PARAM& ImageFlag )
{
	IMAGE_PARAM Flag = ImageFlag;
	if( 0 == Flag.CtrlMask )
		return NULL;
	else if( SM_LB_BK == Flag.CtrlMask )
	{
		if( IP_LS_SEL == Flag.StateMask )
			return m_ItemSelColor;
		if( IP_LS_UNSEL == Flag.StateMask )
			return m_ItemUnselColor;
		if( IP_MOUSEOVER == Flag.StateMask )
			return m_ItemMouseOverColor;
		Flag.CtrlMask -= SM_LB_BK - SM_BS_BK;
		return SQRWnd::GetWndTextColor( Flag );
	}
	else if( Flag.CtrlMask < SM_LB_SCV_BK )
	{
		Flag.CtrlMask -= SM_LB_SCH_BK - SM_SC_BK;
		return m_sbHorizon.GetWndTextColor( Flag );
	}
	else
	{
		Flag.CtrlMask -= SM_LB_SCV_BK - SM_SC_BK;
		return m_sbVertical.GetWndTextColor( Flag );
	}
}

uint32 SQRListCtrl::GetWndTextBcColor( const IMAGE_PARAM& ImageFlag )
{
	IMAGE_PARAM Flag = ImageFlag;
	if( 0 == Flag.CtrlMask )
		return NULL;
	else if( SM_LB_BK == Flag.CtrlMask )
	{
		if( IP_LS_SEL == Flag.StateMask )
			return m_ItemSelBkColor;
		if( IP_LS_UNSEL == Flag.StateMask )
			return m_ItemUnselBkColor;
		if( IP_MOUSEOVER == Flag.StateMask )
			return m_ItemMouseOverBkColor;

		Flag.CtrlMask -= SM_LB_BK - SM_BS_BK;
		return SQRWnd::GetWndTextBcColor( Flag );
	}
	else if( Flag.CtrlMask < SM_LB_SCV_BK )
	{
		Flag.CtrlMask -= SM_LB_SCH_BK - SM_SC_BK;
		return m_sbHorizon.GetWndTextBcColor( Flag );
	}
	else
	{
		Flag.CtrlMask -= SM_LB_SCV_BK - SM_SC_BK;
		return m_sbVertical.GetWndTextBcColor( Flag );
	}
}

void SQRListCtrl::SetWndTextColor( const IMAGE_PARAM& ImageFlag, uint32 uColor )
{
	IMAGE_PARAM Flag = ImageFlag;
	if( SM_LB_BK == Flag.CtrlMask )
	{
		if( IP_LS_SEL == Flag.StateMask )
			m_ItemSelColor = uColor;
		else if( IP_LS_UNSEL == Flag.StateMask )
			m_ItemUnselColor = uColor;
		else if( IP_MOUSEOVER == Flag.StateMask )
			m_ItemMouseOverColor = uColor;
		else
		{
			Flag.CtrlMask -= SM_LB_BK - SM_BS_BK;
			return SQRWnd::SetWndTextColor( Flag, uColor );
		}
	}
	else if( Flag.CtrlMask < SM_LB_SCV_BK )
	{
		Flag.CtrlMask -= SM_LB_SCH_BK - SM_SC_BK;
		return m_sbHorizon.SetWndTextColor( Flag, uColor );
	}
	else
	{
		Flag.CtrlMask -= SM_LB_SCV_BK - SM_SC_BK;
		return m_sbVertical.SetWndTextColor( Flag, uColor );
	}
}

void SQRListCtrl::SetWndTextBcColor( const IMAGE_PARAM& ImageFlag, uint32 uColor )
{
	IMAGE_PARAM Flag = ImageFlag;
	if( SM_LB_BK == Flag.CtrlMask )
	{
		if( IP_LS_SEL == Flag.StateMask )
			m_ItemSelBkColor = uColor;
		else if( IP_LS_UNSEL == Flag.StateMask )
			m_ItemUnselBkColor = uColor;
		else if( IP_MOUSEOVER == Flag.StateMask )
			m_ItemMouseOverBkColor = uColor;
		else
		{
			Flag.CtrlMask -= SM_LB_BK - SM_BS_BK;
			return SQRWnd::SetWndTextBcColor( Flag, uColor );
		}
	}
	else if( Flag.CtrlMask < SM_LB_SCV_BK )
	{
		Flag.CtrlMask -= SM_LB_SCH_BK - SM_SC_BK;
		return m_sbHorizon.SetWndTextBcColor( Flag, uColor );
	}
	else
	{
		Flag.CtrlMask -= SM_LB_SCV_BK - SM_SC_BK;
		return m_sbVertical.SetWndTextBcColor( Flag, uColor );
	}
}

SQRWnd* SQRListCtrl::GetResWnd( int32 nCtrlMask )
{
	if( 0 == nCtrlMask )
		return NULL;
	else if( SM_LB_BK == nCtrlMask )
	{
		nCtrlMask -= SM_LB_BK - SM_BS_BK;
		return SQRWnd::GetResWnd( nCtrlMask );
	}
	else if( nCtrlMask < SM_LB_SCV_BK )
	{
		nCtrlMask -= SM_LB_SCH_BK - SM_SC_BK;
		return m_sbHorizon.GetResWnd( nCtrlMask );
	}
	else
	{
		nCtrlMask -= SM_LB_SCV_BK - SM_SC_BK;
		return m_sbVertical.GetResWnd( nCtrlMask );
	}
}


SQRScrollBar* SQRListCtrl::GetVerticalScroll()
{
	return &m_sbVertical;
}
