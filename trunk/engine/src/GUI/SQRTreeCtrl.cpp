#include "stdafx.h"
#include "SQRTreeCtrl.h"
#include "TreeNode.h"
#include "SQRDialog.h"
#include "CodeCvs.h"
#include "CColor.inl"

#include "TSqrAllocator.inl"


#define TREE_NODE_EXPAND	0x00000001
#define TREE_NODE_GROUP		0x00000002
#define TREE_NODE_SELECT	0x00000004

extern const float FLOAT_MAX_VALUE;
extern const float FLOAT_MIN_VALUE;
extern const float FLOAT_EPSILON;

inline bool NodeContain::operator< ( const NodeContain& n ) const
{
	if( pNode->m_uAscendData < n.pNode->m_uAscendData )
		return true;
	else if( pNode->m_uAscendData == n.pNode->m_uAscendData )
		return pNode->m_NodeName < n.pNode->m_NodeName;
	else
		return false;
}

inline UIList<NodeContain>* _TC( HANDLE p ){ return (UIList<NodeContain>*)p; }

DYNAMIC_CREATE_WND( SQRTreeCtrl, SQRWnd )

bool TreeNode::MsgWalk( SQRTreeCtrl* pTree, CFRect Btn, float xPos, float& yPos, uint32 uMsg, CFPos CursorPos )
{
	float yCurPos = yPos;
	yPos += Btn.bottom;
	if( ( m_uState&TREE_NODE_GROUP ) || m_Children.size() != 0 )
	{
		if( uMsg == WM_LBUTTONDOWN &&
			CursorPos.x > xPos + Btn.left && CursorPos.x < xPos + Btn.right &&
			CursorPos.y > yCurPos + Btn.top && CursorPos.y < yCurPos + Btn.bottom )
		{
			pTree->ExpandNode( this, ( m_uState&TREE_NODE_EXPAND ) == 0 );
			return true;
		}
		xPos += Btn.right;
	}

	CFRect rt[2] = { CFRect( FLOAT_MAX_VALUE, FLOAT_MAX_VALUE, 0, 0 ), 
					 CFRect( FLOAT_MAX_VALUE, FLOAT_MAX_VALUE, 0, 0 ) };
	WND_IMAGE_LIST* List[2] = { &m_ImageHead, &m_ImageTail };
	for( int32 i = 0; i < 2; i++ )
	{
		for( int32 j = 0; j < List[i]->GetImageCount(); j++ )
		{
			rt[i].left   = min( rt[i].left,   List[i]->GetImage(j).posWnd.x );
			rt[i].top    = min( rt[i].top,    List[i]->GetImage(j).posWnd.y );
			rt[i].right  = max( rt[i].right,  List[i]->GetImage(j).posWnd.x + abs( List[i]->GetImage(j).rtTexture.Width() ) );
			rt[i].bottom = max( rt[i].bottom, List[i]->GetImage(j).posWnd.y + abs( List[i]->GetImage(j).rtTexture.Height() ) );
		}
	}
	
	float fTextLen = pTree->GetFont()->CalculateSize(m_NodeName.c_str()).x + max( 0, rt[0].Width() ) + max( 0, rt[1].Width() );
	if( CursorPos.x > xPos + Btn.left && CursorPos.x < xPos + Btn.right + fTextLen &&
		CursorPos.y > yCurPos + Btn.top && CursorPos.y < yCurPos + Btn.bottom )
	{
		if( uMsg == WM_LBUTTONDOWN )
			pTree->OnLClickNode( this );
		if( uMsg == WM_LBUTTONDBLCLK )
			pTree->OnLDBClickNode( this );
		if( uMsg == WM_RBUTTONDOWN )
			pTree->OnRClickNode( this );
		if( uMsg == WM_RBUTTONDBLCLK )
			pTree->OnRDBClickNode( this );

		pTree->SelectNode( this, true );
		return true;
	}

	if( m_uState&TREE_NODE_EXPAND )
	{
		for( UIList<NodeContain>::iterator it = m_Children.begin(); it != m_Children.end(); it++ )
			if( (*it).pNode->MsgWalk( pTree, Btn, xPos, yPos, uMsg, CursorPos ) )
				return true;
	}

	return false;
}


void TreeNode::DrawNode( SQRTreeCtrl* pTree, CFRect Btn, float xPos, float& yPos )
{
	struct DRAWINFO
	{
		WND_IMAGE_LIST* pImageList;
		uint32          uColor;
		uint32			uBack;
		EventStateMask	uMask;
	};

	if( yPos + Btn.bottom > 0 )
	{
		DRAWINFO ImageList[2][4] = 
		{	
			{ 
				{ &pTree->m_CheckMouseOverImage,	pTree->m_CheckMouseOverColor,	pTree->m_CheckMouseOverBcColor,	IP_MOUSEOVER	},	
				{ &pTree->m_CheckClickDownImage,	pTree->m_CheckClickDownColor,	pTree->m_CheckClickDownBcColor,	IP_CLICKDOWN	},		
				{ &pTree->m_CheckEnableImage,		pTree->m_CheckEnableColor,		pTree->m_CheckEnableBcColor,	IP_ENABLE		},	
				{ &pTree->m_CheckDisableImage,		pTree->m_CheckDisableColor,		pTree->m_CheckDisableBcColor,	IP_DISABLE		}	
			},
			{ 
				{ &pTree->m_UncheckMouseOverImage,	pTree->m_UncheckMouseOverColor,	pTree->m_UncheckMouseOverBcColor,	IP_UNCHECK_MOUSEOVER	},	
				{ &pTree->m_UncheckClickDownImage,	pTree->m_UncheckClickDownColor,	pTree->m_UncheckClickDownBcColor,	IP_UNCHECK_CLICKDOWN	},	
				{ &pTree->m_UncheckEnableImage,		pTree->m_UncheckEnableColor,	pTree->m_UncheckEnableBcColor,		IP_UNCHECK_ENABLE		},
				{ &pTree->m_UncheckDisableImage,	pTree->m_UncheckDisableColor,	pTree->m_UncheckDisableBcColor,		IP_UNCHECK_DISABLE		}
			}
		};

		CFRect rt;
		pTree->GetWndRect( rt );
		CFPos pt( -1, -1 );
		if( pTree->IsLastMouseMsgWnd() )
		{
			pt = pTree->GetCursorPos();
			pt.x -= rt.left;
			pt.y -= rt.top;
		}
		
		if( ( m_uState&TREE_NODE_GROUP ) || m_Children.size() != 0 )
		{
			float Zoom = pTree->GetRootZoomSize();
			DRAWINFO* pStateList = ( m_uState&TREE_NODE_EXPAND ) ? ImageList[0] : ImageList[1];
			DRAWINFO CurState = pStateList[2];
			if( pTree->IsEnable() == false )
				CurState = pStateList[3];
			else if( pt.x > xPos + Btn.left && pt.x < xPos + Btn.right && pt.y > yPos +  + Btn.top && pt.y < yPos + Btn.bottom )
				CurState = pTree->IsHeld() ? pStateList[1] : pStateList[0];

			CFRect Rect(0.0f,0.0f,0.0f,0.0f);
			for( int32 i = 0; i < CurState.pImageList->GetImageCount(); i++ )
			{
				IMAGE& image = CurState.pImageList->GetImage(i);
				image.posWnd += CFPos( xPos, yPos );
				CFRect rtImage( (float)image.posWnd.x, (float)image.posWnd.y, image.posWnd.x + abs(image.rtTexture.Width()) * Zoom, image.posWnd.y + abs(image.rtTexture.Height()) * Zoom );
				if (rtImage.right > pTree->GetWndWidth())
					rtImage.right = (float)pTree->GetWndWidth() ;
				
				if (rtImage.bottom > pTree->GetWndHeight())
					rtImage.bottom = (float)pTree->GetWndHeight() ;	

				Rect = Rect.Combination(rtImage);
			}

			pTree->SetEventStateMask(CurState.uMask);
			pTree->DrawRect( *CurState.pImageList ,&Rect );
			for ( int32 i = 0; i < (*CurState.pImageList).GetImageCount(); i++ )
			{
				IMAGE& image = (*CurState.pImageList).GetImage(i);
				CFRect rtText(image.rtTexture.left,image.rtTexture.top,image.rtTexture.right,image.rtTexture.bottom);
				pTree->DrawRect(image.pTexture, Rect, image.dwColor, &rtText, false);
			}

			for( int32 i = 0; i < CurState.pImageList->GetImageCount(); i++ )
				CurState.pImageList->GetImage(i).posWnd -= CFPos( xPos, yPos );

			xPos += Btn.right;
		}

		float fTextLen = pTree->GetFont()->CalculateSize(m_NodeName.c_str()).x;
		if( m_ImageHead.GetImageCount() || m_ImageTail.GetImageCount() )
		{
			CFRect rt[2] = { CFRect( FLOAT_MAX_VALUE, FLOAT_MAX_VALUE, 0, 0 ), 
							 CFRect( FLOAT_MAX_VALUE, FLOAT_MAX_VALUE, 0, 0 ) };
			WND_IMAGE_LIST* List[2] = { &m_ImageHead, &m_ImageTail };
			for( int32 i = 0; i < 2; i++ )
			{
				for( int32 j = 0; j < List[i]->GetImageCount(); j++ )
				{
					rt[i].left   = min( rt[i].left,   List[i]->GetImage(j).posWnd.x );
					rt[i].top    = min( rt[i].top,    List[i]->GetImage(j).posWnd.y );
					rt[i].right  = max( rt[i].right,  List[i]->GetImage(j).posWnd.x + abs( List[i]->GetImage(j).rtTexture.Width() ) );
					rt[i].bottom = max( rt[i].bottom, List[i]->GetImage(j).posWnd.y + abs( List[i]->GetImage(j).rtTexture.Height() ) );
				}
			}
			fTextLen += max( 0, rt[0].Width() ) + max( 0, rt[1].Width() );
		}

		DRAWINFO* pStateList = ( m_uState&TREE_NODE_SELECT ) ? ImageList[0] : ImageList[1];
		DRAWINFO CurState = pStateList[2];
		if( pTree->IsEnable() == false )
			CurState = pStateList[3];
		else if( pt.x > xPos + Btn.left && pt.x < xPos + Btn.right + fTextLen && pt.y > yPos + Btn.top && pt.y < yPos + Btn.bottom )
			CurState = pTree->IsHeld() ? pStateList[1] : pStateList[0];

		pTree->DrawNode( this, CFRect( xPos, yPos + Btn.top, xPos + rt.right - rt.left, yPos + Btn.bottom ), CurState.uColor, CurState.uBack );
	}
	else
	{
		if( ( m_uState&TREE_NODE_GROUP ) || m_Children.size() != 0 )
			xPos += Btn.right;
	}

	yPos += Btn.bottom;
	if( m_uState&TREE_NODE_EXPAND && m_Children.size() )
	{
		float yPrePos = yPos;

		IMAGE_PARAM Flag;

		Flag.CtrlMask = SM_BS_BK;
		pTree->SetEventStateMask(pTree->IsEnable() ? IP_ENABLE : IP_DISABLE);
		Flag.StateMask = (uint16)( pTree->GetEventStateMask() );
		
		uint32 uColor = pTree->GetWndTextColor( Flag );

		for( UIList<NodeContain>::iterator it = m_Children.begin(); it != m_Children.end(); it++ )
		{
			if( pTree->GetStyle()&TREE_HASLINE )
			{
				PointGroup PG[2];
				PG[0].uColor = PG[1].uColor = uColor;

				PG[0].vPoint = CVector3f( xPos - Btn.Width()/2.0f, yPrePos, 0 );
				PG[1].vPoint = CVector3f( xPos - Btn.Width()/2.0f, yPos + Btn.bottom - Btn.Height()/2.0f, 0 );
				pTree->DrawLine( PG, 2 );

				PG[0].vPoint = CVector3f( xPos - Btn.Width()/2.0f, yPos + ( Btn.top + Btn.bottom )/2.0f, 0 );
				PG[1].vPoint = CVector3f( xPos, yPos + ( Btn.top + Btn.bottom )/2.0f, 0 );
				pTree->DrawLine( PG, 2 );

				yPrePos = yPos + Btn.bottom - Btn.Height()/2;
			}

			(*it).pNode->DrawNode( pTree, Btn, xPos, yPos );
		}
	}
}

int32 TreeNode::GetShowNodeCount()
{
	int32 n = 1;
	if( m_uState&TREE_NODE_EXPAND )
	{
		for( UIList<NodeContain>::iterator it = m_Children.begin(); it != m_Children.end(); it++ )
			n +=(*it).pNode->GetShowNodeCount();
	}

	return n;
}

bool TreeNode::GetNodeShowPos( TreeNode* pNode, int32& nPos )
{
	if( pNode == this )
		return true;
	nPos++;

	if( m_uState&TREE_NODE_EXPAND )
	{
		for( UIList<NodeContain>::iterator it = m_Children.begin(); it != m_Children.end(); it++ )
		{
			if( (*it).pNode->GetNodeShowPos( pNode, nPos ) )
				return true;
		}
	}

	return false;
}

void TreeNode::SetSelectNode( TreeNode* pNode )
{
	if( pNode == this )
		m_uState |= TREE_NODE_SELECT;
	else
		m_uState &= ~TREE_NODE_SELECT;

	for( UIList<NodeContain>::iterator it = m_Children.begin(); it != m_Children.end(); it++ )
		(*it).pNode->SetSelectNode( pNode );
}

//////////////////////////////////////////////////////////////////////////

SQRTreeCtrl::SQRTreeCtrl(void)
{
	m_pCurrentNode = NULL;
	m_hTree = new UIList<NodeContain>;
}

SQRTreeCtrl::~SQRTreeCtrl(void)
{
	for( UIList<NodeContain>::iterator it = _TC(m_hTree)->begin(); it !=  _TC(m_hTree)->end(); it++ )
		delete (*it).pNode;
	_TC(m_hTree)->clear();
	delete  _TC(m_hTree);
}

void SQRTreeCtrl::ReleaseWndTex()
{
	// 释放可以释放的UI贴图资源
	m_CheckMouseOverImage.ReleaseTex();
	m_CheckClickDownImage.ReleaseTex();
	m_CheckEnableImage.ReleaseTex();
	m_CheckDisableImage.ReleaseTex();
	m_UncheckMouseOverImage.ReleaseTex();
	m_UncheckClickDownImage.ReleaseTex();
	m_UncheckEnableImage.ReleaseTex();
	m_UncheckDisableImage.ReleaseTex();
	SQRControl::ReleaseWndTex();
}
void SQRTreeCtrl::CreatWndTex()
{
	// 显示之前申请资源
	if ( m_FlushDlg == NULL )
	{
		m_FlushDlg = GetFlushDialog();
		if ( m_FlushDlg == NULL ) return;
	}
	m_CheckMouseOverImage.CreatTex( GetGraphic(), m_FlushDlg );
	m_CheckClickDownImage.CreatTex( GetGraphic(), m_FlushDlg );
	m_CheckEnableImage.CreatTex( GetGraphic(), m_FlushDlg );
	m_CheckDisableImage.CreatTex( GetGraphic(), m_FlushDlg );
	m_UncheckMouseOverImage.CreatTex( GetGraphic(), m_FlushDlg );
	m_UncheckClickDownImage.CreatTex( GetGraphic(), m_FlushDlg );
	m_UncheckEnableImage.CreatTex( GetGraphic(), m_FlushDlg );
	m_UncheckDisableImage.CreatTex( GetGraphic(), m_FlushDlg );
	SQRControl::CreatWndTex();
}

int32 SQRTreeCtrl::Create( const WndCreateParam& param )
{
	WndCreateParam newParam = param;
	newParam.uStyle |= WS_CHILD;
	if( FAILED( SQRWnd::Create( newParam ) ) )
		return GERROR;

	CFRect rt( 0.0f, 0.0f, 16.0f, 16.0f );
	m_CheckMouseOverImage.  AddImage( GetGraphic(), 0, "", &rt, CFPos(0.0f, 0.0f), CColor::RandDark() );
	m_CheckClickDownImage.  AddImage( GetGraphic(), 0, "", &rt, CFPos(0.0f, 0.0f), CColor::RandDark() );
	m_CheckEnableImage.     AddImage( GetGraphic(), 0, "", &rt, CFPos(0.0f, 0.0f), CColor::RandDark() );
	m_CheckDisableImage.    AddImage( GetGraphic(), 0, "", &rt, CFPos(0.0f, 0.0f), CColor::RandDark() );
	m_UncheckMouseOverImage.AddImage( GetGraphic(), 0, "", &rt, CFPos(0.0f, 0.0f), CColor::RandDark() );
	m_UncheckClickDownImage.AddImage( GetGraphic(), 0, "", &rt, CFPos(0.0f, 0.0f), CColor::RandDark() );
	m_UncheckEnableImage.   AddImage( GetGraphic(), 0, "", &rt, CFPos(0.0f, 0.0f), CColor::RandDark() );
	m_UncheckDisableImage.  AddImage( GetGraphic(), 0, "", &rt, CFPos(0.0f, 0.0f), CColor::RandDark() );

	m_CheckMouseOverColor	= 0xffffffff;
	m_CheckClickDownColor	= 0xffffffff;
	m_CheckEnableColor		= 0xffffffff;
	m_CheckDisableColor		= 0xffffffff;
	m_UncheckMouseOverColor = 0xffffffff;
	m_UncheckClickDownColor = 0xffffffff;
	m_UncheckEnableColor	= 0xffffffff;
	m_UncheckDisableColor	= 0xffffffff;

	m_CheckMouseOverBcColor = 0;
	m_CheckClickDownBcColor = 0;
	m_CheckEnableBcColor = 0;
	m_CheckDisableBcColor = 0;
	m_UncheckMouseOverBcColor = 0;
	m_UncheckClickDownBcColor = 0;
	m_UncheckEnableBcColor = 0;
	m_UncheckDisableBcColor = 0;

	IMAGE_PARAM IP;
	IP.CtrlMask = SM_BS_BK;
	IP.StateMask = IP_ENABLE;
	SQRWnd::SetWndTextColor( IP, 0x50000000 );
	IP.StateMask = IP_DISABLE;
	SQRWnd::SetWndTextColor( IP, 0x50000000 );

	WndCreateParam sbParam;
	sbParam.x = GetWndWidth() - 16;
	sbParam.y = 0;
	sbParam.width = 16;
	sbParam.height = GetWndHeight();
	sbParam.pParentWnd = this;
	sbParam.nFontID = param.nFontID;
	sbParam.font_size = param.font_size;
	m_ScrollBar.Create( sbParam );
	m_ScrollBar.SetLockFlag( LOCK_RIGHT|LOCK_TOP|LOCK_BOTTOM );

	ResetScrollBar();

	return 0;
}


WND_IMAGE_LIST* SQRTreeCtrl::GetWndBkImage( const IMAGE_PARAM& ImageFlag )
{
	IMAGE_PARAM Flag = ImageFlag;
	if( Flag.CtrlMask == SM_TC_BK )
	{
		if( Flag.StateMask == IP_MASK )
			return SQRWnd::GetWndBkImage( Flag );
		if( Flag.StateMask == IP_TR_ENABLE )
		{
			Flag.StateMask = IP_ENABLE;
			return SQRWnd::GetWndBkImage( Flag );
		}
		if( Flag.StateMask == IP_TR_DISABLE )
		{
			Flag.StateMask = IP_DISABLE;
			return SQRWnd::GetWndBkImage( Flag );
		}
		if( Flag.StateMask == IP_ENABLE )
			return &m_CheckEnableImage;
		if( Flag.StateMask == IP_DISABLE )
			return &m_CheckDisableImage;
		if( Flag.StateMask == IP_MOUSEOVER )
			return &m_CheckMouseOverImage;
		if( Flag.StateMask == IP_CLICKDOWN )
			return &m_CheckClickDownImage;
		if( Flag.StateMask == IP_UNCHECK_ENABLE )
			return &m_UncheckEnableImage;
		if( Flag.StateMask == IP_UNCHECK_DISABLE )
			return &m_UncheckDisableImage;
		if( Flag.StateMask == IP_UNCHECK_MOUSEOVER )
			return &m_UncheckMouseOverImage;
		if( Flag.StateMask == IP_UNCHECK_CLICKDOWN )
			return &m_UncheckClickDownImage;
	}
	else if( Flag.CtrlMask <= SM_TC_SCV_BAR )
	{
		Flag.CtrlMask -= SM_TC_SCV_BK - SM_BS_BK;
		return m_ScrollBar.GetWndBkImage( Flag );
	} 

	return NULL;
}

bool SQRTreeCtrl::ObjHasEventState(EventStateMask Mask)
{
	return SQRTreeCtrl::IsHasEventState(Mask);
}

bool SQRTreeCtrl::IsHasEventState(EventStateMask Mask)
{
	if(	IP_MASK			== Mask ||
		IP_MOUSEDESC	== Mask ||
		IP_ENABLE		== Mask ||
		IP_DISABLE		== Mask ||
		IP_MOUSEOVER		== Mask ||
		IP_CLICKDOWN		== Mask ||
		IP_UNCHECK_ENABLE	== Mask ||
		IP_UNCHECK_DISABLE	== Mask ||
		IP_UNCHECK_MOUSEOVER== Mask ||
		IP_UNCHECK_CLICKDOWN== Mask ||
		IP_FILL				== Mask ||
		IP_TR_ENABLE		== Mask ||
		IP_TR_DISABLE		== Mask ||
		IP_LS_SEL			== Mask ||
		IP_LS_UNSEL			== Mask )
		return true;
	return false;
}

uint32 SQRTreeCtrl::GetWndTextColor( const IMAGE_PARAM& ImageFlag )
{
	IMAGE_PARAM Flag = ImageFlag;
	if( Flag.CtrlMask == SM_TC_BK )
	{
		if( Flag.StateMask == IP_MASK )
			return SQRWnd::GetWndTextColor( Flag );
		if( Flag.StateMask == IP_TR_ENABLE )
		{
			Flag.StateMask = IP_ENABLE;
			return SQRWnd::GetWndTextColor( Flag );
		}
		if( Flag.StateMask == IP_TR_DISABLE )
		{
			Flag.StateMask = IP_DISABLE;
			return SQRWnd::GetWndTextColor( Flag );
		}
		if( Flag.StateMask == IP_ENABLE )
			return m_CheckEnableColor;
		if( Flag.StateMask == IP_DISABLE )
			return m_CheckDisableColor;
		if( Flag.StateMask == IP_MOUSEOVER )
			return m_CheckMouseOverColor;
		if( Flag.StateMask == IP_CLICKDOWN )
			return m_CheckClickDownColor;
		if( Flag.StateMask == IP_UNCHECK_ENABLE )
			return m_UncheckEnableColor;
		if( Flag.StateMask == IP_UNCHECK_DISABLE )
			return m_UncheckDisableColor;
		if( Flag.StateMask == IP_UNCHECK_MOUSEOVER )
			return m_UncheckMouseOverColor;
		if( Flag.StateMask == IP_UNCHECK_CLICKDOWN )
			return m_UncheckClickDownColor;
	}
	else if( Flag.CtrlMask <= SM_TC_SCV_BAR )
	{
		Flag.CtrlMask -= SM_TC_SCV_BK - SM_BS_BK;
		return m_ScrollBar.GetWndTextColor( Flag );
	} 

	return 0;
}


uint32 SQRTreeCtrl::GetWndTextBcColor( const IMAGE_PARAM& ImageFlag )
{
	IMAGE_PARAM Flag = ImageFlag;
	if( Flag.CtrlMask == SM_TC_BK )
	{
		if( Flag.StateMask == IP_MASK )
			return SQRWnd::GetWndTextBcColor( Flag );
		if( Flag.StateMask == IP_TR_ENABLE )
		{
			Flag.StateMask = IP_ENABLE;
			return SQRWnd::GetWndTextBcColor( Flag );
		}
		if( Flag.StateMask == IP_TR_DISABLE )
		{
			Flag.StateMask = IP_DISABLE;
			return SQRWnd::GetWndTextBcColor( Flag );
		}
		if( Flag.StateMask == IP_ENABLE )
			return m_CheckEnableBcColor;
		if( Flag.StateMask == IP_DISABLE )
			return m_CheckDisableBcColor;
		if( Flag.StateMask == IP_MOUSEOVER )
			return m_CheckMouseOverBcColor;
		if( Flag.StateMask == IP_CLICKDOWN )
			return m_CheckClickDownBcColor;
		if( Flag.StateMask == IP_UNCHECK_ENABLE )
			return m_UncheckEnableBcColor;
		if( Flag.StateMask == IP_UNCHECK_DISABLE )
			return m_UncheckDisableBcColor;
		if( Flag.StateMask == IP_UNCHECK_MOUSEOVER )
			return m_UncheckMouseOverBcColor;
		if( Flag.StateMask == IP_UNCHECK_CLICKDOWN )
			return m_UncheckClickDownBcColor;
	}
	else if( Flag.CtrlMask <= SM_TC_SCV_BAR )
	{
		Flag.CtrlMask -= SM_TC_SCV_BK - SM_BS_BK;
		return m_ScrollBar.GetWndTextBcColor( Flag );
	} 

	return 0;
}

void SQRTreeCtrl::SetWndTextColor( const IMAGE_PARAM& ImageFlag, uint32 uColor )
{
	IMAGE_PARAM Flag = ImageFlag;
	if( Flag.CtrlMask == SM_TC_BK )
	{
		if( Flag.StateMask == IP_MASK )
			SQRWnd::SetWndTextColor( Flag, uColor );
		else if( Flag.StateMask == IP_TR_ENABLE )
		{
			Flag.StateMask = IP_ENABLE;
			SQRWnd::SetWndTextColor( Flag, uColor );
		}
		else if( Flag.StateMask == IP_TR_DISABLE )
		{
			Flag.StateMask = IP_DISABLE;
			SQRWnd::SetWndTextColor( Flag, uColor );
		}
		else if( Flag.StateMask == IP_ENABLE )
			m_CheckEnableColor = uColor;
		else if( Flag.StateMask == IP_DISABLE )
			m_CheckDisableColor = uColor;
		else if( Flag.StateMask == IP_MOUSEOVER )
			m_CheckMouseOverColor = uColor;
		else if( Flag.StateMask == IP_CLICKDOWN )
			m_CheckClickDownColor = uColor;
		else if( Flag.StateMask == IP_UNCHECK_ENABLE )
			m_UncheckEnableColor = uColor;
		else if( Flag.StateMask == IP_UNCHECK_DISABLE )
			m_UncheckDisableColor = uColor;
		else if( Flag.StateMask == IP_UNCHECK_MOUSEOVER )
			m_UncheckMouseOverColor = uColor;
		else if( Flag.StateMask == IP_UNCHECK_CLICKDOWN )
			m_UncheckClickDownColor = uColor;
	}
	else if( Flag.CtrlMask <= SM_TC_SCV_BAR )
	{
		Flag.CtrlMask -= SM_TC_SCV_BK - SM_BS_BK;
		m_ScrollBar.SetWndTextColor( Flag, uColor );
	} 
}

void SQRTreeCtrl::SetWndTextBcColor( const IMAGE_PARAM& ImageFlag, uint32 uColor )
{
	IMAGE_PARAM Flag = ImageFlag;
	if( Flag.CtrlMask == SM_TC_BK )
	{
		if( Flag.StateMask == IP_MASK )
			SQRWnd::SetWndTextBcColor( Flag, uColor );
		else if( Flag.StateMask == IP_TR_ENABLE )
		{
			Flag.StateMask = IP_ENABLE;
			SQRWnd::SetWndTextBcColor( Flag, uColor );
		}
		else if( Flag.StateMask == IP_TR_DISABLE )
		{
			Flag.StateMask = IP_DISABLE;
			SQRWnd::SetWndTextBcColor( Flag, uColor );
		}
		else if( Flag.StateMask == IP_ENABLE )
			m_CheckEnableBcColor = uColor;
		else if( Flag.StateMask == IP_DISABLE )
			m_CheckDisableBcColor = uColor;
		else if( Flag.StateMask == IP_MOUSEOVER )
			m_CheckMouseOverBcColor = uColor;
		else if( Flag.StateMask == IP_CLICKDOWN )
			m_CheckClickDownBcColor = uColor;
		else if( Flag.StateMask == IP_UNCHECK_ENABLE )
			m_UncheckEnableBcColor = uColor;
		else if( Flag.StateMask == IP_UNCHECK_DISABLE )
			m_UncheckDisableBcColor = uColor;
		else if( Flag.StateMask == IP_UNCHECK_MOUSEOVER )
			m_UncheckMouseOverBcColor = uColor;
		else if( Flag.StateMask == IP_UNCHECK_CLICKDOWN )
			m_UncheckClickDownBcColor = uColor;
	}
	else if( Flag.CtrlMask <= SM_TC_SCV_BAR )
	{
		Flag.CtrlMask -= SM_TC_SCV_BK - SM_BS_BK;
		m_ScrollBar.SetWndTextBcColor( Flag, uColor );
	} 
}

SQRWnd* SQRTreeCtrl::GetResWnd( int32 nCtrlMask )
{
	if( nCtrlMask == SM_TC_BK )
	{
		nCtrlMask -= SM_TC_BK - SM_BS_BK;
		return SQRWnd::GetResWnd( nCtrlMask );
	}
	else if( nCtrlMask <= SM_TC_SCV_BAR )
	{
		nCtrlMask -= SM_TC_SCV_BK - SM_BS_BK;
		return m_ScrollBar.GetResWnd( nCtrlMask );
	}

	return NULL;
}

int32 SQRTreeCtrl::GetShowNodeCount()
{
	int32 n = 0;
	for( UIList<NodeContain>::iterator it =  _TC(m_hTree)->begin(); it != _TC(m_hTree)->end(); it++ )
		n +=(*it).pNode->GetShowNodeCount();

	return n;
}

HTREENODE SQRTreeCtrl::InsertNode( HTREENODE pParent, const char* szText, 
								const WND_IMAGE_LIST* pHeadImage, const WND_IMAGE_LIST* pTailImage, 
								uint32 uAscendData, uint32 uParam)
{
	NodeContain Node;
	Node.pNode = new TreeNode;
	Node.pNode->m_ImageHead		= pHeadImage ? *pHeadImage : WND_IMAGE_LIST();
	Node.pNode->m_ImageTail		= pTailImage ? *pTailImage : WND_IMAGE_LIST();
	Node.pNode->m_pParent		= pParent;
	Node.pNode->m_uAscendData	= uAscendData;
	Node.pNode->m_uParam		= uParam;
	Node.pNode->m_NodeName		= utf8_to_utf16(szText).c_str();

	UIList<NodeContain>& NodeList = Node.pNode->m_pParent ? Node.pNode->m_pParent->m_Children : *_TC(m_hTree);

	UIList<NodeContain>::iterator it = NodeList.insert( NodeList.end(), Node );
	(*it).pNode->m_it = it;
	
	//默认不排序
	//NodeList.sort();

	ResetScrollBar();
	return Node.pNode;
}

void SQRTreeCtrl::Sort(HTREENODE pParent)
{
	if (NULL == pParent)
	{
		UIList<NodeContain>& NodeList = *_TC(m_hTree);
		NodeList.sort();	
	}
	else
	{
		UIList<NodeContain>& NodeList = pParent->m_Children;
		NodeList.sort();
	}
}

void SQRTreeCtrl::DeleteNode( HTREENODE pNode )
{
	UIList<NodeContain>& NodeList = pNode->m_pParent ? pNode->m_pParent->m_Children : *_TC(m_hTree);
	UIList<NodeContain>::iterator it = NodeList.begin();
	for( ; it != NodeList.end(); it++ )
		if( (*it).pNode == pNode )
			break;
	if( it == NodeList.end() )
		return;

	if ( m_pCurrentNode == (*it).pNode )
		m_pCurrentNode  = NULL;
	delete (*it).pNode;


	NodeList.erase( it );

	ResetScrollBar();
}

void SQRTreeCtrl::DeleteAll()
{
	for( UIList<NodeContain>::iterator it = _TC(m_hTree)->begin(); it != _TC(m_hTree)->end(); it++ )
		delete (*it).pNode;
	_TC(m_hTree)->clear();
	m_pCurrentNode = NULL;
}

void SQRTreeCtrl::ExpandNode( HTREENODE pNode, bool bExpand )
{
	if( bExpand )
		pNode->m_uState |= TREE_NODE_EXPAND;
	else
		pNode->m_uState &= ~TREE_NODE_EXPAND;

	ResetScrollBar();
}

bool SQRTreeCtrl::IsExpand( HTREENODE pNode )
{
	return ( pNode->m_uState & TREE_NODE_EXPAND ) != 0;
}

void SQRTreeCtrl::GroupNode( HTREENODE pNode, bool bGroup )
{
	if( bGroup )
		pNode->m_uState |= TREE_NODE_GROUP;
	else
		pNode->m_uState &= ~TREE_NODE_GROUP;
}

bool SQRTreeCtrl::IsGroup( HTREENODE pNode )
{
	return ( pNode->m_uState & TREE_NODE_GROUP ) != 0;
}

void SQRTreeCtrl::SelectNode( HTREENODE pNode, bool bSelect )
{
	m_pCurrentNode = pNode;

	if( bSelect )
	{
		for( UIList<NodeContain>::iterator it = _TC(m_hTree)->begin(); it != _TC(m_hTree)->end(); it++ )
			(*it).pNode->SetSelectNode( pNode );		
	}
	else
	{
		pNode->m_uState &= ~TREE_NODE_SELECT;
		m_pCurrentNode = NULL;
	}
}

void SQRTreeCtrl::SelectNodeNone()
{
	if(m_pCurrentNode)
	{
		m_pCurrentNode->SetSelectNode(NULL);
		m_pCurrentNode = NULL;
	}
}

void SQRTreeCtrl::ScrollToNode( HTREENODE pNode )
{
	if( pNode->m_pParent && !IsExpand( pNode->m_pParent ) )
		ExpandNode( pNode->m_pParent, true );

	int32 nPos = 0;
	for( UIList<NodeContain>::iterator it = _TC(m_hTree)->begin(); it != _TC(m_hTree)->end(); it++ )
		if( (*it).pNode->GetNodeShowPos( pNode, nPos ) )
			break;
	m_ScrollBar.SetPos( nPos );
}

bool SQRTreeCtrl::IsSelect( HTREENODE pNode )
{
	return ( pNode->m_uState & TREE_NODE_SELECT ) != 0;
}

void SQRTreeCtrl::SetNodeText( HTREENODE pNode, const char* szText )
{
	pNode->m_NodeName = utf8_to_utf16(szText).c_str();
}

void SQRTreeCtrl::SetNodeImage( HTREENODE pNode, const WND_IMAGE_LIST* pHeadImage, const WND_IMAGE_LIST* pTailImage )
{
	pNode->m_ImageHead = pHeadImage ? *pHeadImage : WND_IMAGE_LIST();
	pNode->m_ImageHead.CreatTex( GetGraphic() );
	pNode->m_ImageTail = pTailImage ? *pTailImage : WND_IMAGE_LIST();
	pNode->m_ImageTail.CreatTex( GetGraphic() );
}

void SQRTreeCtrl::SetNodeAscendData( HTREENODE pNode, uint32 uAscendData )
{
	pNode->m_uAscendData = uAscendData;
	UIList<NodeContain>& NodeList = pNode->m_pParent ? pNode->m_pParent->m_Children : *_TC(m_hTree);
	NodeList.sort();
}

void SQRTreeCtrl::SetNodeParam( HTREENODE pNode, uint32 uParam )
{
	pNode->m_uParam = uParam;
}

const char* SQRTreeCtrl::GetNodeText( HTREENODE pNode )
{
	m_utf8Str = utf16_to_utf8(pNode->m_NodeName.c_str()).c_str();
	return m_utf8Str.c_str();
}

void SQRTreeCtrl::GetNodeImage( HTREENODE pNode, WND_IMAGE_LIST& HeadImage, WND_IMAGE_LIST& TailImage )
{
	HeadImage = pNode->m_ImageHead;
	TailImage = pNode->m_ImageTail;
}

uint32 SQRTreeCtrl::GetNodeAscendData( HTREENODE pNode )
{
	return pNode->m_uAscendData;
}

uint32 SQRTreeCtrl::GetNodeParam( HTREENODE pNode )
{
	return pNode->m_uParam;
}

HTREENODE  SQRTreeCtrl::GetNodeParent( HTREENODE pNode )
{
	return pNode->m_pParent;
}

uint32  SQRTreeCtrl::GetChildNodeCount( HTREENODE pNode )
{
	return pNode ? pNode->m_Children.size() : _TC(m_hTree)->size();
}

HTREENODE  SQRTreeCtrl::GetFirstChildNode( HTREENODE pNode )
{
	UIList<NodeContain>& NodeList = pNode ? pNode->m_Children : *_TC(m_hTree);
	return NodeList.empty() ? NULL : NodeList.begin()->pNode;
}

HTREENODE  SQRTreeCtrl::GetNextChildNode( HTREENODE pNode )
{
	UIList<NodeContain>::iterator it = pNode->m_it;
	it++;
	return it == ( pNode->m_pParent ? pNode->m_pParent->m_Children : *_TC(m_hTree) ).end() ? NULL : it->pNode;
}

//////////////////////////////////////////////////////////////////////////
void SQRTreeCtrl::OnLButtonDblClk( uint32 nFlags, int32 x, int32 y )
{
	NodeWalk( WM_LBUTTONDBLCLK, CFPos( (float)x, (float)y ) );
}

void SQRTreeCtrl::OnLButtonDown( uint32 nFlags, int32 x, int32 y )
{
	NodeWalk( WM_LBUTTONDOWN, CFPos( (float)x, (float)y ) );
	SendCtrlMsg(BUTTON_CLICKDOWN, x, y);
}

void SQRTreeCtrl::OnRButtonDblClk( uint32 nFlags, int32 x, int32 y )
{
	NodeWalk( WM_RBUTTONDBLCLK, CFPos( (float)x, (float)y ) );
}

void SQRTreeCtrl::OnRButtonDown( uint32 nFlags, int32 x, int32 y )
{
	NodeWalk( WM_RBUTTONDOWN, CFPos( (float)x, (float)y ) );
	//xyt
	/*
	@可以使用脚本来访问到鼠标右键按下的消息,否则没有反应
	*/
	SendCtrlMsg(TREECTRL_RIGHTCLICK, x, y);
}

void SQRTreeCtrl::OnMouseWheel( uint32 nFlags, short zDelta, int32 x, int32 y )
{
	m_ScrollBar.SendMsg( WM_MOUSEWHEEL, MAKELONG( nFlags, zDelta ), MAKELONG( x, y ) );
}

int32 SQRTreeCtrl::DestroyWnd()
{
	this->DeleteAll();

	return SQRWnd::DestroyWnd();
}

//////////////////////////////////////////////////////////////////////////
bool SQRTreeCtrl::NodeWalk( uint32 uMsg, CFPos Point )
{
	WND_IMAGE_LIST* ImageList[8] = 
	{
		&m_CheckMouseOverImage,		&m_CheckClickDownImage,		&m_CheckEnableImage,	&m_CheckDisableImage,
		&m_UncheckMouseOverImage,	&m_UncheckClickDownImage,	&m_UncheckEnableImage,	&m_UncheckDisableImage
	};

	float Zoom = GetRootZoomSize();
	CFRect rt( FLOAT_MAX_VALUE, FLOAT_MAX_VALUE, 0, 0 );
	for( int32 i = 0; i < 8; i++ )
	{
		for( int32 j = 0; j < ImageList[i]->GetImageCount(); j++ )
		{
			rt.left   = min( rt.left,   ImageList[i]->GetImage(j).posWnd.x * Zoom );
			rt.top    = min( rt.top,    ImageList[i]->GetImage(j).posWnd.y * Zoom );
			rt.right  = max( rt.right,  ( ImageList[i]->GetImage(j).posWnd.x + abs( ImageList[i]->GetImage(j).rtTexture.Width() ) ) * Zoom );
			rt.bottom = max( rt.bottom, ( ImageList[i]->GetImage(j).posWnd.y + abs( ImageList[i]->GetImage(j).rtTexture.Height() ) ) * Zoom );
		}
	}

	float y = -(float)m_ScrollBar.GetPos()*rt.bottom;
	for( UIList<NodeContain>::iterator it = _TC(m_hTree)->begin(); it != _TC(m_hTree)->end(); it++ )
		if( (*it).pNode->MsgWalk( this, rt, 0, y, uMsg, Point ) )
			return true;
	this->SelectNodeNone();
	return false;
}

void SQRTreeCtrl::ResetScrollBar()
{
	WND_IMAGE_LIST* ImageList[8] = 
	{
		&m_CheckMouseOverImage,		&m_CheckClickDownImage,		&m_CheckEnableImage,	&m_CheckDisableImage,
		&m_UncheckMouseOverImage,	&m_UncheckClickDownImage,	&m_UncheckEnableImage,	&m_UncheckDisableImage
	};

	float h = 0;
	for( int32 i = 0; i < 8; i++ )
		for( int32 j = 0; j < ImageList[i]->GetImageCount(); j++ )
			h = max( h, ImageList[i]->GetImage(j).posWnd.y + abs( ImageList[i]->GetImage(j).rtTexture.Height() ) );

	float Zoom = GetRootZoomSize();
	int32 nPage  = (int32)GetWndHeight()/(int32)(h*Zoom);
	int32 nRange = GetShowNodeCount();
	m_ScrollBar.SetPage( nPage );
	m_ScrollBar.SetRange( nRange );
	m_ScrollBar.ProcessMove(true, true);

	m_ScrollBar.GetResWnd( SM_SC_BAR )->ShowWnd( nRange > nPage );
	m_ScrollBar.EnableWnd( nRange > nPage );
}

void SQRTreeCtrl::DrawWndBackground()
{
	m_beBGDrawed = true;
	if (m_hWnd->m_bIsShowBackground)
	{
		bool isEnable = IsEnable();
		SetEventStateMask(isEnable ? IP_TR_ENABLE : IP_TR_DISABLE);
		DrawRect( isEnable ? m_hWnd->m_Enable : m_hWnd->m_Disable );
	}

	WND_IMAGE_LIST* ImageList[8] = 
	{
		&m_CheckMouseOverImage,		&m_CheckClickDownImage,		&m_CheckEnableImage,	&m_CheckDisableImage,
		&m_UncheckMouseOverImage,	&m_UncheckClickDownImage,	&m_UncheckEnableImage,	&m_UncheckDisableImage
	};

	float Zoom = GetRootZoomSize();
	CFRect rt( FLOAT_MAX_VALUE, FLOAT_MAX_VALUE, 0, 0 );
	for( int32 i = 0; i < 8; i++ )
	{
		for( int32 j = 0; j < ImageList[i]->GetImageCount(); j++ )
		{
			rt.left   = min( rt.left,   ImageList[i]->GetImage(j).posWnd.x * Zoom);
			rt.top    = min( rt.top,    ImageList[i]->GetImage(j).posWnd.y * Zoom);
			rt.right  = max( rt.right,  ( ImageList[i]->GetImage(j).posWnd.x + abs( ImageList[i]->GetImage(j).rtTexture.Width() ) ) * Zoom );
			rt.bottom = max( rt.bottom, ( ImageList[i]->GetImage(j).posWnd.y + abs( ImageList[i]->GetImage(j).rtTexture.Height() ) ) * Zoom );
		}
	}

	float y = -(float)m_ScrollBar.GetPos()*rt.bottom;
	for( UIList<NodeContain>::iterator it = _TC(m_hTree)->begin(); it != _TC(m_hTree)->end(); it++ )
		(*it).pNode->DrawNode( this, rt, 0, y );
}

void SQRTreeCtrl::DrawNode( HTREENODE pNode, CFRect rtNode, uint32 uTextColor, uint32 uBackColor )
{
	float xEnd = rtNode.right;
	uint32 StyleIndex = GetStyleIndex();
	SetStyleIndex(0);
	DrawNodeHeadImage( pNode, rtNode );
	rtNode.left = rtNode.right;
	rtNode.right = xEnd;
	SetStyleIndex(StyleIndex);
	DrawNodeText( pNode, rtNode, uTextColor, uBackColor );
	rtNode.left = rtNode.right;
	rtNode.right = xEnd;
	SetStyleIndex(0);	
	DrawNodeTailImage( pNode, rtNode );
	rtNode.left = rtNode.right;
	rtNode.right = xEnd;
	SetStyleIndex(StyleIndex);
}

void SQRTreeCtrl::DrawNodeHeadImage( HTREENODE pNode, CFRect& rtImage )
{
	float Zoom = GetRootZoomSize();
	CFRect rt( FLOAT_MAX_VALUE, FLOAT_MAX_VALUE, 0, 0 );
	for( int32 j = 0; j < pNode->m_ImageHead.GetImageCount(); j++ )
	{
		rt.left   = min( rt.left,   pNode->m_ImageHead.GetImage(j).posWnd.x * Zoom );
		rt.top    = min( rt.top,    pNode->m_ImageHead.GetImage(j).posWnd.y * Zoom );
		rt.right  = max( rt.right,  ( pNode->m_ImageHead.GetImage(j).posWnd.x + abs( pNode->m_ImageHead.GetImage(j).rtTexture.Width() ) ) * Zoom );
		rt.bottom = max( rt.bottom, ( pNode->m_ImageHead.GetImage(j).posWnd.y + abs( pNode->m_ImageHead.GetImage(j).rtTexture.Height() ) ) * Zoom );
	}
	rtImage.right = rtImage.left + max( 0, rt.Width() );

	for( int32 i = 0; i < pNode->m_ImageHead.GetImageCount(); i++ )
		pNode->m_ImageHead.GetImage(i).posWnd += CFPos( rtImage.left - rt.left, rtImage.top - rt.top );
	CFRect rtfImage = rtImage.Intersection( CFRect( 0, 0, GetWndWidth(), GetWndHeight() ) );
	
	DrawRect( pNode->m_ImageHead, &rtfImage );
	for( int32 i = 0; i < pNode->m_ImageHead.GetImageCount(); i++ )
		pNode->m_ImageHead.GetImage(i).posWnd -= CFPos( rtImage.left - rt.left, rtImage.top - rt.top );

}

void SQRTreeCtrl::DrawNodeTailImage( HTREENODE pNode, CFRect& rtImage )
{
	float Zoom = GetRootZoomSize();
	CFRect rt( FLOAT_MAX_VALUE, FLOAT_MAX_VALUE, 0, 0 );
	for( int32 j = 0; j < pNode->m_ImageTail.GetImageCount(); j++ )
	{
		rt.left   = min( rt.left,   pNode->m_ImageTail.GetImage(j).posWnd.x * Zoom );
		rt.top    = min( rt.top,    pNode->m_ImageTail.GetImage(j).posWnd.y * Zoom );
		rt.right  = max( rt.right,  ( pNode->m_ImageTail.GetImage(j).posWnd.x + abs( pNode->m_ImageTail.GetImage(j).rtTexture.Width() ) ) * Zoom );
		rt.bottom = max( rt.bottom, ( pNode->m_ImageTail.GetImage(j).posWnd.y + abs( pNode->m_ImageTail.GetImage(j).rtTexture.Height() ) ) * Zoom );
	}
	rtImage.right = rtImage.left + max( 0, rt.Width() );

	for( int32 i = 0; i < pNode->m_ImageTail.GetImageCount(); i++ )
		pNode->m_ImageTail.GetImage(i).posWnd += CFPos( rtImage.left - rt.left, rtImage.top - rt.top );
	CFRect rtfImage = rtImage.Intersection(CFRect( 0, 0, GetWndWidth(), GetWndHeight()));


	DrawRect( pNode->m_ImageTail, &rtfImage );
	for( int32 i = 0; i < pNode->m_ImageTail.GetImageCount(); i++ )
		pNode->m_ImageTail.GetImage(i).posWnd -= CFPos( rtImage.left - rt.left, rtImage.top - rt.top );
}

void SQRTreeCtrl::DrawNodeText( HTREENODE pNode, CFRect& rtText, uint32 uTextColor, uint32 uBackColor )
{
	CVector2f txtSize = SQRWnd::GetFont()->CalculateSize(pNode->m_NodeName.c_str());
	rtText.right = rtText.left + txtSize.x;
	float dy = ( rtText.Height() - SQRWnd::GetFont()->GetLineSpacing() )/2;
	if( IsSelect( pNode ) )
	{
		IMAGE_PARAM IP;
		IP.CtrlMask = SM_BS_BK;
		SetEventStateMask(IsEnable()? IP_ENABLE : IP_DISABLE);
		IP.StateMask = (WORD)GetEventStateMask();
		CFRect rt( rtText );
		if ( GetStyle()&TREE_FULLSELECT )
			rt.right = (float)GetWndWidth() - m_ScrollBar.GetWndWidth();

		DrawRect( NULL, rt, SQRWnd::GetWndTextColor( IP ), NULL, true );

		DrawText( pNode->m_NodeName.c_str(), rtText.left, rtText.top + dy, uTextColor, uBackColor, false, NULL );
	}
	else
		DrawText( pNode->m_NodeName.c_str(), rtText.left, rtText.top + dy, uTextColor, uBackColor, false, NULL );
}

HTREENODE SQRTreeCtrl::GetNodeByText(const char* text, HTREENODE pNode /*= NULL*/) const
{
	if(text == NULL)
		return NULL;
	wstring utf16_str;
	utf8_to_utf16(utf16_str, text);
	const UIList<NodeContain>* nodes = NULL;
	if(pNode)
		nodes = &(pNode->m_Children);
	else
		nodes = _TC(m_hTree);
	
	for(UIList<NodeContain>::const_iterator it = nodes->begin(); 
		it != nodes->end(); ++it)
	{
		if(it->pNode->m_NodeName == utf16_str.c_str())
			return it->pNode;
		HTREENODE hn = GetNodeByText(text, it->pNode);
		if(hn)
			return hn;
	}
	return NULL;
}
