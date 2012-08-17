#include "stdafx.h"
#include "SQRRichToolTip.h"
#include "SQRTipHolder.h"
#include "SQRRootWnd.h"
#include "SQRGUIManager.h"
#include "SQRDialog.h"

SQRRichToolTip::SQRRichToolTip()
: m_pChildToolTip(NULL)
, m_uStyle(17)
, m_bTexInit(false)
, m_pParent(NULL)
, m_pTipHandler(NULL)
{
}

SQRRichToolTip::~SQRRichToolTip()
{
	ClearChildToolTip();
	OnTipHide();
	if ( m_pTipHandler )
		m_pTipHandler->DelRichToolTip(this);
}

void SQRRichToolTip::SetParentWnd(SQRWnd* wnd)
{
	m_pParent = wnd;
}

bool SQRRichToolTip::AddChildToolTip(void)
{
	if(m_pChildToolTip	==	NULL)
	{
		m_pChildToolTip = new SQRRichToolTip();
		if ( m_pChildToolTip )
		{
			m_pChildToolTip->SetParentWnd( m_pParent );
			return true;
		}
	}
		
	return false;
}

void SQRRichToolTip::SetTipHandler( TipHandler* handler )
{
	if ( m_pTipHandler != handler )
	{
		if ( m_pTipHandler )
			m_pTipHandler->DelRichToolTip(this);

		m_pTipHandler = handler;

		if (m_pTipHandler)
			m_pTipHandler->ActiveRichToolTip(this);
	}
}

void SQRRichToolTip::ClearTipHandler()
{
	m_pTipHandler = NULL;
}

void SQRRichToolTip::ClearChildToolTip(void)
{
	// GfkSafeDelete(m_pTipWnd);  // maybe delete by the root wnd, see later
	GfkSafeDelete(m_pChildToolTip);
}

SQRRichToolTip* SQRRichToolTip::GetChildToolTip(void)
{
	return m_pChildToolTip;
}

void SQRRichToolTip::SetToolTipStyle(uint32 uStyle)	
{
	m_uStyle = uStyle;
}

void sqr::SQRRichToolTip::DrawToolTip( SQRWnd* pWnd )
{
	if ( !pWnd->GetRichToolTip()->CanBeShow() )
		return;
	
	WndHandle* pHandle = pWnd->GetWndHandle();
	SQRRichToolTip* pToolTip = pWnd->GetRichToolTip();
	// does here need root wnd? May be yes
	SQRRootWnd* pRootWnd = pWnd->GetRootWnd();

	SQRRichToolTip* pParentToolTip = NULL;
	uint32 temp = pRootWnd->GetStyleIndex();

	int32 nFontSize = int32(pWnd->GetFontSize() * pWnd->GetWndZoom());
	while(pToolTip != NULL)
	{
#ifdef TEXTURE_STINT_MODE
		if (!pToolTip->m_bTexInit)
		{
			pToolTip->m_bTexInit = true;
			pToolTip->m_Image.CreatTex( pWnd->GetGraphic() );
		}
#endif
		// 底图绘制 // 画底图的方式不变
		if (pToolTip->m_Image.GetImageCount() > 0)
		{
			pRootWnd->SetStyleIndex(pToolTip->m_uStyle);
			IMAGE& image = pToolTip->m_Image.GetImage(0);
			float imagePosX = float(pToolTip->m_DrawPos.x);
			float imagePosY = float(pToolTip->m_DrawPos.y) ;
			CFRect rtImage(imagePosX, imagePosY , imagePosX + image.rtTexture.Width(), imagePosY + image.rtTexture.Height());
			pRootWnd->DrawRect(pToolTip->m_Image, &rtImage, 0.0f, 0.0f);
		}
		
		// 文字绘制 // 主要是窗体的绘制
		UIList<SQRWnd*>::iterator it = pToolTip->m_ChildList.begin();
		UIList<SQRWnd*>::iterator itend = pToolTip->m_ChildList.end();
		while ( it != itend )
		{
			pToolTip->Draw( (*it) );
			++it;
		}
		pParentToolTip = pToolTip;
		pToolTip = pToolTip->GetChildToolTip();
	}
	pRootWnd->SetStyleIndex(temp);
}

void SQRRichToolTip::Draw( SQRWnd* Wnd )
{
	WndHandle* phand = Wnd->GetWndHandle();
	if (!(phand->m_bShow))
		return;

#ifdef TEXTURE_STINT_MODE
	if (!(phand->m_bTexInit))
	{
		phand->m_bTexInit = true;
		Wnd->CreatWndTex();
	}
#endif

	Wnd->DrawWndBackground();
	Wnd->DrawWndText();
	// draw ime fx
	uint32 tProTime = uint32(GetProcessTime());
	Wnd->DrawImeFx(tProTime, Wnd->GetWndHandle()->m_pGUI->GetZoomSize());

	SQRWnd* ChildWnd = phand->m_LastChild;
	while ( ChildWnd )
	{
		if ( ( ChildWnd->GetStyle()&WS_MODULE ) == 0 )
		{
			Draw( ChildWnd );
		}
		ChildWnd = ChildWnd->GetWndHandle()->m_PreBrother;
	}

	Wnd->DrawAfterChildren();
}

float SQRRichToolTip::MaxHeight()
{
	float n =  32.0f;
	float h = .0f;
	WndHandle* phand;
	UIList<SQRWnd*>::iterator it		= m_ChildList.begin();
	UIList<SQRWnd*>::iterator itend	= m_ChildList.end();
	while( it != itend )
	{
		phand = (*it)->GetWndHandle();
		h += phand->m_Height;
		it++;
	}
	n = max( n, h );
	return n;
}

float SQRRichToolTip::MaxWidth()
{
	float n = 64.0f;
	WndHandle* phand;
	UIList<SQRWnd*>::iterator it		= m_ChildList.begin();
	UIList<SQRWnd*>::iterator itend	= m_ChildList.end();
	while( it != itend )
	{
		phand = (*it)->GetWndHandle();
		n = max( n, phand->m_Width );
		it++;
	}
	return n;
}

void SQRRichToolTip::PushData()
{
	if ( m_pTipHandler )
		m_pTipHandler->OnToolTipShow( this );
}

void sqr::SQRRichToolTip::PrepareToolTip( SQRWnd* pWnd )
{
	WndHandle* pHandle = pWnd->GetWndHandle();
	SQRRootWnd* pRootWnd = pWnd->GetRootWnd();
	float fontSize = pWnd->GetFontSize() * pWnd->GetWndZoom();

	SQRRichToolTip* pToolTip = pWnd->GetRichToolTip() ;
	SQRRichToolTip* pParentToolTip = NULL;
	while (pToolTip != NULL)
	{
		pToolTip->m_Image = pHandle->m_MouseDesc;
#ifdef TEXTURE_STINT_MODE
		if (!pToolTip->m_bTexInit)
		{
			pToolTip->m_bTexInit = true;
			pToolTip->m_Image.CreatTex( pWnd->GetGraphic() );
		}
#endif
		CFRect rtPreImage(32767, 32767, -32767, -32767);
		for( int32 i = 0; i < pToolTip->m_Image.GetImageCount(); i++ )
		{
			IMAGE& Image = pToolTip->m_Image.GetImage(i);
			rtPreImage.left		= min( Image.posWnd.x, rtPreImage.left );
			rtPreImage.right	= max( Image.posWnd.x + (int32)abs( Image.rtTexture.Width() ), rtPreImage.right );
			rtPreImage.top		= min( Image.posWnd.y, rtPreImage.top );
			rtPreImage.bottom	= max( Image.posWnd.y + (int32)abs( Image.rtTexture.Height() ), rtPreImage.bottom );
		}

		CFRect rtImage;

		// 这种添加窗体的tooltip需要背景图片适应窗体
		rtImage.left = 0;
		rtImage.top  = 0;
		// 计算相关窗体位置信息，确定最终背景图片的大小
		float temp_width = pToolTip->MaxWidth();
		float temp_height = pToolTip->MaxHeight();
		rtImage.right  = temp_width + fontSize ;
		rtImage.bottom = temp_height + fontSize;
		pToolTip->m_Image.Scale( rtImage.Width() - rtPreImage.Width(), rtImage.Height() - rtPreImage.Height() );

		CFPos pos;
		bool bIsDown = pWnd->IsDownTooltips();
		if ( bIsDown )
		{
			CFRect rt ;
			pWnd->GetWndRect(rt);
			pos = CFPos(rt.left,rt.bottom);	
		}
		else
		{
			pos = pWnd->GetCursorPos();
		}	

		CFPos& posDraw = pHandle->m_posMouseOver;
		if( pParentToolTip == NULL )
		{
			if( bIsDown )
				posDraw.SetValue( pos.x, pos.y);
			else
				posDraw.SetValue( pos.x, pos.y - rtImage.Height() );

			if( posDraw.x > pHandle->m_pGUI->GetSreenWidth() - rtImage.Width() )
				posDraw.x = pHandle->m_pGUI->GetSreenWidth() - rtImage.Width();
			if( posDraw.y < pRootWnd->GetWndTop() )
				posDraw.y = pRootWnd->GetWndTop();

			pToolTip->m_DrawPos.SetValue(posDraw.x - pRootWnd->GetWndLeft(),posDraw.y - pRootWnd->GetWndTop()) ;

			// 布局list窗体
			posDraw.x = posDraw.x + fontSize/2;
			posDraw.y = posDraw.y + fontSize/2;

			pToolTip->LayoutTips( posDraw );
		}
		if (pParentToolTip != NULL)
		{	
			posDraw =  pParentToolTip->m_DrawPos;
			IMAGE& image = pToolTip->m_Image.GetImage(0);
			posDraw.x = posDraw.x - image.rtTexture.Width();
			pToolTip->m_DrawPos.SetValue(posDraw.x, posDraw.y); 
			// 刷新根tip
			posDraw.x = posDraw.x + fontSize/2 + pRootWnd->GetWndLeft();
			posDraw.y = posDraw.y + fontSize/2 + pRootWnd->GetWndTop() ;
			pToolTip->LayoutTips( posDraw );
		}
		pParentToolTip = pToolTip;
		pToolTip = pToolTip->GetChildToolTip();
	}

	SQRRichToolTip* pLastToolTip = pParentToolTip;
	if ( pLastToolTip != NULL)
	{
		// 重新布局
		if( pLastToolTip->m_DrawPos.x < 0  )
		{
			float fDistance = fontSize;
			SQRRichToolTip* pToolTip = pWnd->GetRichToolTip();
			float fOverX = abs(pLastToolTip ->m_DrawPos.x) + fDistance + pToolTip->MaxWidth();
			fOverX += abs(pToolTip->m_DrawPos.x);
			float fMaxX = .0f;

			pToolTip = pToolTip->GetChildToolTip();
			while (pToolTip != NULL)
			{
				CFPos& DrawPos =  pToolTip->m_DrawPos;
				pToolTip->m_DrawPos.SetValue(DrawPos.x + fOverX,DrawPos.y); 
				fMaxX = max(fMaxX, pToolTip->m_DrawPos.x + pToolTip->MaxWidth());
				// 校正X是否超过游戏窗口的左边
				pToolTip->RejustPos(fOverX, 0.0f);
				pToolTip = pToolTip->GetChildToolTip();
			}
			
			if( fMaxX > pHandle->m_pGUI->GetSreenWidth() )
			{
				float fDisX = .0f;
				fDisX = pHandle->m_pGUI->GetSreenWidth() - fMaxX - fDistance;
				// 校正x是否超过游戏窗口的右边
				pToolTip = pWnd->GetRichToolTip();
				while (pToolTip != NULL)
				{
					CFPos& DrawPos =  pToolTip->m_DrawPos;
					pToolTip->m_DrawPos.SetValue(DrawPos.x + fDisX,DrawPos.y); 
					pToolTip->RejustPos(fDisX, 0.0f);
					pToolTip = pToolTip->GetChildToolTip();
				}
			}
		}
	}
	pWnd->GetRootWnd()->SetFont( pWnd->GetFontID(), pWnd->IsFontBold(), pWnd->GetFontSize() );
}

void SQRRichToolTip::LayoutTips( CFPos& pos )
{
	UIList<SQRWnd*>::iterator it = m_ChildList.begin();
	UIList<SQRWnd*>::iterator itend = m_ChildList.end();

	UIList<PosRect*>::iterator pos_it = m_RectList.begin();
	UIList<PosRect*>::iterator pos_itend = m_RectList.end();

	float height = 0.0f;
	CFRect rect;
	WndHandle* pHandle;	// = pWnd->GetWndHandle();

	while ( it != itend )
	{
		pHandle = (*it)->GetWndHandle();

		rect.left = pos.x;
		rect.top = pos.y + height;
		rect.right = rect.left + pHandle->m_Width;
		rect.bottom = rect.top + pHandle->m_Height;

		height += pHandle->m_Height;
		if ( pos_it == pos_itend || CheckNeedRepos(rect, (*pos_it)->OrgRect) )
		{
			(*it)->SetWndRect( rect );
			(*pos_it)->RePos = true;
			(*pos_it)->OrgRect = rect;
			m_TempPos = m_DrawPos;
		}
		else
		{
			m_DrawPos = m_TempPos;
			(*pos_it)->RePos = false;
		}
		++it;
		++pos_it;
	}
}

void SQRRichToolTip::RejustPos(float x, float y)
{
	UIList<SQRWnd*>::iterator it = m_ChildList.begin();
	UIList<SQRWnd*>::iterator itend = m_ChildList.end();

	UIList<PosRect*>::iterator pos_it = m_RectList.begin();
	UIList<PosRect*>::iterator pos_itend = m_RectList.end();

	CFRect rect;
	while ( it != itend )
	{
		(*it)->GetWndRect( rect );

		rect.left	+= x;
		rect.right	+= x;
		rect.top	+= y;
		rect.bottom += y;

		if ( pos_it == pos_itend || (*pos_it)->RePos || CheckNeedRepos(rect, (*pos_it)->RejRect) )
		{
			(*it)->SetWndRect( rect );
			(*pos_it)->RejRect = rect;
			(*pos_it)->RePos = false;
			m_TempPos = m_DrawPos;
		}
		++it;
		++pos_it;
	}
}

bool SQRRichToolTip::CheckNeedRepos(const CFRect& r1, const CFRect& r2)
{
	float dis = (r1.left - r2.left) * (r1.left - r2.left) + 
				(r1.right - r2.right) * (r1.right - r2.right) + 
				(r1.top - r2.top) * (r1.top - r2.top) + 
				(r1.bottom - r2.bottom) * (r1.bottom - r2.bottom);
	if (dis > 2.0f)
		return true;
	return false;
}

SQRWnd* SQRRichToolTip::PushTipsWnd(const char* szWindowNameInResFile)
{
	SQRWnd* parent = SQRTipHolder::GetInst()->GetHolder();
	if ( !parent )
		return NULL;
	SQRDialog* tips = new SQRDialog();
	PosRect* pos = new PosRect();
	tips->CreateFromRes(szWindowNameInResFile, parent);
	m_ChildList.push_back( tips );
	m_RectList.push_back( pos );
	tips->ShowWnd(true);
	return tips;
}

// 显示tooltip
void SQRRichToolTip::OnTipShow()
{
	if (m_pTipHandler)
		m_pTipHandler->OnToolTipShow( this );
}

// 隐藏tooltip
void SQRRichToolTip::OnTipHide()
{
	// 完成 挂接窗体的删除，脱离挂接关系的操做
	UIList<SQRWnd*>::iterator it = m_ChildList.begin();
	UIList<SQRWnd*>::iterator itend = m_ChildList.end();

	while ( it != itend )
	{
		GfkSafeDelete(*it);
		++it;
	}
	m_ChildList.clear();

	UIList<PosRect*>::iterator pos_it = m_RectList.begin();
	UIList<PosRect*>::iterator pos_itend = m_RectList.end();
	while ( pos_it != pos_itend )
	{
		GfkSafeDelete(*pos_it);
		++pos_it;
	}
	m_RectList.clear();
	

	if ( m_pChildToolTip )
		m_pChildToolTip->OnTipHide();
}

bool SQRRichToolTip::CanBeShow()
{
	if (m_ChildList.size() != 0)
		return true;
	return false;
}

SQRWnd* SQRRichToolTip::GetParentWnd()
{
	return m_pParent;
}


