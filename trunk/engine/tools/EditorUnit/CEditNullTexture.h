#pragma once
#include "CEditTexture.h"

namespace sqr_tools
{
	class CEditNullTexture:public CEditTexture
	{
	public:
		CEditNullTexture(const TCHAR* szTexName);
		virtual ~CEditNullTexture();

		string m_TexName;
		const char*	GetName() const{return m_TexName.c_str();}
		bool IsValid(){ return true;}

		uint32		FrameCount()							{ return 0; }
		uint32		GetWidth(void)							{ return 0; }
		uint32		GetHeight(void)							{ return 0; }
		void*		GetBuffer() const						{ return NULL;}
		bool		IsTransparent(float x,float y)const					{ return false;}
		int			FillFromHBitmap( HBITMAP hBitmap, HBITMAP hMask = NULL ) { return 0; };
		int			FillFromMemory( const void* pMemory, eTextureFormat Format, uint32 nWidth, const CIRect& rtSrc, const CIRect& rtDes ) { return 0; };
		HRESULT		SaveToFile(const string& filename,IMAGE_FILEFORMAT Format = IFF_DDS ){return true;};
		HRESULT		Lock( uint32 level, _TEXLOCKED_RECT* pTexLocked, CIRect* Rect, uint32 Num = 0, uint32 Flags = 0 ) { return GERROR; };
		HRESULT		Unlock( uint32 level, uint32 Num = 0 ) { return GERROR; };
	};
}