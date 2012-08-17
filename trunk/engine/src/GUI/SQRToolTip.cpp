#include "stdafx.h"
#include "SQRToolTip.h"
#include "SQRGUIManager.h"
#include "BaseHelper.h"
#include "CodeCvs.h"

SQRToolTip::SQRToolTip()
: m_pChildToolTip(NULL)
, m_uStyle(29)
, m_szDesc("")
, m_szDescW(L"")
, m_bTexInit(false)
{

}

SQRToolTip::~SQRToolTip()
{
	ClearChildToolTip();
}

bool sqr::SQRToolTip::AddChildToolTip(void)
{
	if(m_pChildToolTip	==	NULL)
		return NULL != (m_pChildToolTip = new SQRToolTip);
	return false;
}

void sqr::SQRToolTip::ClearChildToolTip(void)
{
	GfkSafeDelete(m_pChildToolTip);
}

void sqr::SQRToolTip::SetToolTipDesc( const char* szDesc )
{
	this->m_szDesc = szDesc ? szDesc : "";
	this->m_szDescW = utf8_to_utf16(this->m_szDesc.c_str()).c_str();
}

SQRToolTip* sqr::SQRToolTip::GetChildToolTip(void)
{
	return m_pChildToolTip;
}

void sqr::SQRToolTip::SetToolTipStyle(uint32 uStyle)	
{
	m_uStyle = uStyle;
}

void sqr::SQRToolTip::DrawToolTip( SQRWnd* pWnd )
{
	WndHandle* pHandle = pWnd->GetWndHandle();
	SQRToolTip* pToolTip = pWnd->GetToolTipRichText();
	SQRRootWnd* pRootWnd = pWnd->GetRootWnd();
	
	SQRToolTip* pParentToolTip = NULL;
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
		// µ×Í¼»æÖÆ
		float imagePosX = float(pToolTip->m_DrawPos.x);
		float imagePosY = float(pToolTip->m_DrawPos.y) ;
		if (pToolTip->m_Image.GetImageCount() > 0)
		{
			pRootWnd->SetStyleIndex(pToolTip->m_uStyle);
			IMAGE& image = pToolTip->m_Image.GetImage(0);
			CFRect rtImage(imagePosX, imagePosY , imagePosX + image.rtTexture.Width(), imagePosY + image.rtTexture.Height());
			pRootWnd->DrawRect(pToolTip->m_Image, &rtImage, 0.0f, 0.0f);
		}
		
		// ÎÄ×Ö»æÖÆ
		pToolTip->Draw(pRootWnd, imagePosX + nFontSize/2, imagePosY + nFontSize/2);
		pParentToolTip = pToolTip;
		pToolTip = pToolTip->GetChildToolTip();
	}
	pRootWnd->SetStyleIndex(temp);
}

void sqr::SQRToolTip::PrepareToolTip( SQRWnd* pWnd )
{
	WndHandle* pHandle = pWnd->GetWndHandle();
	SQRRootWnd* pRootWnd = pWnd->GetRootWnd();
	float fontZoomSize = pWnd->GetFontSize() * pWnd->GetWndZoom();
	float fontSize = pWnd->GetFontSize(); 
	float tipFontSize = pWnd->GetTipFontSize() ? pWnd->GetTipFontSize() : fontSize;

	SQRToolTip* pToolTip = pWnd->GetToolTipRichText();
	SQRToolTip* pParentToolTip = NULL;
	pWnd->SetFont(pWnd->GetFontID(),pWnd->IsFontBold(),tipFontSize);
	while (pToolTip != NULL)
	{
		pToolTip->CreateWndTex();
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
		if( pToolTip->m_Image.GetImageCount() == 1 && 
			/*pToolTip.m_Image.GetImage(0).dwColor == 0xCC000000 &&*/ 
			pToolTip->m_Image.GetImage(0).dwLockFlag == (LOCK_LEFT|LOCK_RIGHT|LOCK_TOP|LOCK_BOTTOM) )
		{
			pToolTip->SetWidth( 245 );
			pToolTip->SetText( pWnd, ( L"#W" + pToolTip->m_szDescW ).c_str() );
			pToolTip->SetWidth( pToolTip->GetMaxWidth() );
			rtImage.left = 0;
			rtImage.top  = 0;
			rtImage.right  = pToolTip->GetMaxWidth() + fontZoomSize ;
			rtImage.bottom = pToolTip->GetCurHeight() + fontZoomSize;
			pToolTip->m_Image.Scale( rtImage.Width() - rtPreImage.Width(), rtImage.Height() - rtPreImage.Height() );
		}
		else
		{
			pToolTip->SetWidth( rtPreImage.Width() - 5 );
			pToolTip->SetText( pWnd, ( L"#c0" + pToolTip->m_szDescW ).c_str() );
			rtImage = rtPreImage;
			rtImage.bottom = rtImage.top + pToolTip->GetCurHeight() + 5;
			pToolTip->m_Image.Scale( 0, rtImage.Height() - rtPreImage.Height() );
		}
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

			posDraw.x += 2;
			posDraw.y += 2;
			pToolTip->m_DrawPos.SetValue(posDraw.x - pRootWnd->GetWndLeft(),posDraw.y - pRootWnd->GetWndTop()) ;
		}
		if (pParentToolTip != NULL)
		{	
			CFPos& parentPos =  pParentToolTip->m_DrawPos;
			IMAGE& image = pToolTip->m_Image.GetImage(0);
			pToolTip->m_DrawPos.SetValue(parentPos.x - image.rtTexture.Width(), parentPos.y); 
		}
		pParentToolTip = pToolTip;
		pToolTip = pToolTip->GetChildToolTip();
	}
	pWnd->SetFont(pWnd->GetFontID(),pWnd->IsFontBold(),fontSize);

	SQRToolTip* pLastToolTip = pParentToolTip;
	if ( pLastToolTip != NULL)
	{
		if( pLastToolTip->m_DrawPos.x < 0  )
		{
			float fOverX = abs(pLastToolTip ->m_DrawPos.x) + 1;
			SQRToolTip* pToolTip = pWnd->GetToolTipRichText();
			while (pToolTip != NULL)
			{
				CFPos& DrawPos =  pToolTip->m_DrawPos;
				pToolTip->m_DrawPos.SetValue(DrawPos.x + fOverX,DrawPos.y); 
				pToolTip = pToolTip->GetChildToolTip();
			}
		}
	}
	pWnd->GetRootWnd()->SetFont( pWnd->GetFontID(), pWnd->IsFontBold(),tipFontSize );
}

