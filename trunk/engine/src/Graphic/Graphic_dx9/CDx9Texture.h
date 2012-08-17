#pragma once
#include "Dx9Base.h"
#include "CTextureImp.h"

namespace sqr
{
class DX9Graphic;
class CDx9TexCoder;
class CDx9Texture : public CTextureImp
{
	friend class CDx9TexCoder;
	friend class CDx9DefaultTexCoder;
	friend class CDx9DDSCoder;
	friend class CDx9DDSTexture;
	friend class CDx9DDSVolumeTexture;
	friend class CDx9DDSCubeTexture;
	friend class CDx9DataSources;
	friend class DX9DepthTexture;

private:
	CDx9TexCoder* m_pActiveCoder;

	bool		m_IsLoaded;
	bool		m_bExa;

	DWORD		m_Filter;
	DWORD		m_MinMapFilter;

	sTextureInfo	m_Head;					//文件头
	LPDIRTEX*	m_pTextures;				//贴图

	HDC			m_pTexDC;


	void ReadDataFromMem( CRefBufFile& GraphicFile );

	friend class DX9Graphic;

public:
	CDx9Texture( IResTexture* pParent );
	~CDx9Texture();

	const char* GetType() const;
	static const char*
		Type();

	int  LoadCOM( CRefBufFile* pGraphicFile );                         //载入标准文件，bmp，tga，jpg
	int  LoadCOMEx( CRefBufFile* pGraphicFile );                       //载入标准文件，bmp，tga，jpg
	int  LoadTEX( CRefBufFile* pGraphicFile );                         //载入tex（暂定）文件
	
	bool IsValid(){ return m_IsLoaded; }
	
	void	SetCustomParams( uint Width, uint Height,uint MinMap, eTextureFormat eFormat, uint32 Filter, uint32 MipFilter );

	int  CreateEmpty( int32 nWidth, int32 nHeight, uint32 nLevel, eTextureFormat eFormat );

	HDC				GetHDC(void);

	uint32				GetWidth() const
	{
		return m_Head.uWidth;
	}
	uint32				GetHeight() const
	{
		return m_Head.uHeight;
	}
	eTextureFormat		GetFormat()		const
	{
		return m_Head.eFormat;
	}

	eTextureFormat		GetTexFormat()		const
	{
		return m_Head.eFormat;
	}

	HRESULT				SaveToFile(const string& filename, IMAGE_FILEFORMAT Format = IFF_DDS );
	bool				FillFromTexture(  const  ITexture* pText, const CIRect* rtSrc, const CIRect* rtDes );

	bool				IsTransparent(float x,float y) const;
	void*				GetBuffer() const;
	void				SetBufSize( uint32 uSize );

	uint32		FrameCount()
	{
		return m_Head.uFrameCount;
	}

	int			FillFromHBitmap( HBITMAP hBitmap, HBITMAP hMask = NULL );
	int			FillFromMemory( const void* pMemory, eTextureFormat Format, uint32 nWidth, const CIRect& rtSrc, const CIRect& rtDes );
	HRESULT		Lock( uint32 level, _TEXLOCKED_RECT* pTexLocked, CIRect* Rect, uint32 Num = 0, uint32 Flags = 0 );
	HRESULT		Unlock( uint32 level, uint32 Num = 0 );

	const char*	GetName() const;
	void		AddRef();
	void		Release();
	void		_Reclaim();
	void		Reclaim();
	uint32		GetRefCount();

	virtual void
			AutoGenMip();

	virtual void
			SetLod(uint32 lod);

private:
	void	ReleaseD3D();
	//void	_Reclaim(void);
	//void	_FreeRes(void);
};

}// namespace sqr