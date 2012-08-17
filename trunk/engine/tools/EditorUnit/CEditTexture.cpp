#include "stdafx.h"
#include "CEditTexture.h"

CEditTexFormat CEditTexture::Format;

CEditTexture::CEditTexture( const string& szName, IResTexture* pTexture )
	: IResTexture(szName,pTexture?pTexture->GetAlias():L"")
	, CEditInfo(&Format)
	, m_pTexture( pTexture )
{
	if(m_pTexture)
		m_pTexture->AddRef();
}

CEditTexture::~CEditTexture()
{
	if(m_pTexture)
		m_pTexture->Release(); 
}

uint32			CEditTexture::FrameCount()
{ return m_pTexture->FrameCount(); }

eTextureFormat	CEditTexture::GetTexFormat() const
{ return m_pTexture->GetTexFormat(); }

uint32			CEditTexture::GetWidth(void) const
{ return m_pTexture->GetWidth(); }

uint32			CEditTexture::GetHeight(void) const
{ return m_pTexture->GetHeight(); }

void*			CEditTexture::GetBuffer() const
{ return m_pTexture->GetBuffer(); }

bool			CEditTexture::IsTransparent(float x,float y) const
{ return m_pTexture->IsTransparent(x,y); }

int				CEditTexture::FillFromHBitmap( HBITMAP hBitmap, HBITMAP hMask )
{ return m_pTexture->FillFromHBitmap(hBitmap,hMask); }

int				CEditTexture::FillFromMemory( const void* pMemory, eTextureFormat Format, uint32 nWidth, const CIRect& rtSrc, const CIRect& rtDes )
{ return m_pTexture->FillFromMemory(pMemory,Format,nWidth, rtSrc, rtDes); }

bool			CEditTexture::FillFromTexture( const ITexture* pText, const CIRect* rtSrc, const CIRect* rtDes )
{ return m_pTexture->FillFromTexture(pText,rtSrc,rtDes); }

HRESULT			CEditTexture::Lock( uint32 level, _TEXLOCKED_RECT* pTexLocked, CIRect* Rect, uint32 Num, uint32 Flags )
{ return m_pTexture->Lock(level,pTexLocked,Rect,Num,Flags); }

HRESULT			CEditTexture::Unlock( uint32 level,  uint32 Num )
{ return m_pTexture->Unlock(level,Num); }

HRESULT			CEditTexture::SaveToFile(const string& filename, IMAGE_FILEFORMAT Format )
{ return m_pTexture->SaveToFile(filename,Format); }

int				CEditTexture::ReLoadPic( void )
{ 
	return m_pTexture->ReLoadPic(); 
}

int				CEditTexture::CreateEmpty( int32 nWidth, int32 nHeight, uint32 nLevel, eTextureFormat eFormat, EmptyTexType type )
{ 
	return m_pTexture->CreateEmpty(nWidth,nHeight,nLevel,eFormat, type); 
}

int				CEditTexture::LoadPic( uint Width, uint Height,uint MinMap, eTextureFormat eFormat , DWORD Filter,	DWORD MipFilter )
{ 
	return m_pTexture->LoadPic(Width,Height,MinMap,eFormat,Filter,MipFilter); 
}

int				CEditTexture::LoadPic( void )
{ 
	return m_pTexture->LoadPic(); 
}

bool			CEditTexture::IsValid(void)
{
	return m_pTexture->IsValid(); 
}

void			CEditTexture::AutoGenMip()
{
	return m_pTexture->AutoGenMip();
}