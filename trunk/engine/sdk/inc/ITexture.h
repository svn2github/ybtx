#pragma once

#include "GraphicRes.h"
#include "GraphicBase.h"
#include "CResourceContainer.h"

namespace sqr
{
template<class T> struct TRect;
typedef TRect<int>		CIRect;


/// 纹理
class ITextureOld
	: public GraphicRes
	, public CGraphicMallocObject
{
public:
	virtual uint32		FrameCount()		= 0;
	virtual eTextureFormat	GetTexFormat() const	{ return TF_ARGB32; };
	virtual uint32			GetWidth(void)	= 0;
	virtual uint32			GetHeight(void)	= 0;
	virtual const char*	GetName()	const = 0;
	virtual void*		GetBuffer() const = 0;
	virtual bool		IsTransparent(float x,float y) const = 0;
	virtual int			FillFromHBitmap( HBITMAP hBitmap, HBITMAP hMask = NULL ) = 0;
	virtual int			FillFromMemory( const void* pMemory, eTextureFormat Format, uint32 nWidth, const CIRect& rtSrc, const CIRect& rtDes ) = 0;
	virtual bool		FillFromTexture( const ITexture* pText, const CIRect* rtSrc, const CIRect* rtDes ) { return 0; };
	virtual HRESULT		Lock( uint32 level, _TEXLOCKED_RECT* pTexLocked, CIRect* Rect, uint32 Num = 0, uint32 Flags = 0 ) = 0;
	virtual HRESULT		Unlock( uint32 level,  uint32 Num = 0 ) = 0;
	virtual HRESULT		SaveToFile(const string& filename, IMAGE_FILEFORMAT Format = IFF_DDS ) = 0;
};

class IResTextureOld
	: public ITextureOld
	, public CResUnit
{
public:
	IResTextureOld(CResourceContainer* pContainer, CResourceRecycle* pRecycle, const string& szName, const wstring& szAlias):CResUnit(pContainer,pRecycle,szName,szAlias, eRT_eTex){}
	virtual int			ReLoadPic( void ) { return 0; };						//载入文件
	virtual int			CreateEmpty( int32 nWidth, int32 nHeight, eTextureFormat eFormat )											= 0;
	virtual int			LoadPic( uint Width, uint Height,uint MinMap, eTextureFormat eFormat , DWORD Filter,	DWORD MipFilter )	= 0;
	virtual int			LoadPic( void )	{ return 0; }
			int32		AddRef()			{ return ITextureOld::AddRef(); }
			const char*	GetName() const		{ return CResUnit::GetName(); }
			bool		IsValid()			{ return CResUnit::IsValid(); }
};

}