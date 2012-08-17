#include "stdafx.h"
#include "CDx9Texture.h"
#include "TimeHelper.h"
#include "CBufFile.h"
#include "BaseHelper.h"
#include "CGraphic.h"
#include "CDx9DDSTextureCoder.h"
#include "CTextureQualityMgr.h"
#include "CHardwareCapacity.h"
#include "CDx9MainWindow.h"
#include "CDx9TexFmtMapper.h"
#include <DxErr.h>

#define MAX_MIPMAP 3
#define USE_TEXTURE

//#define IS_SHOW_SIZE

#ifdef IS_SHOW_SIZE
size_t g_TotalSize = 0;
#endif


CDx9Texture::CDx9Texture( IResTexture* pParent )
	: CTextureImp(pParent)
	, m_pActiveCoder(NULL)
	, m_IsLoaded(false)
{
	memset( &m_Head, 0, sizeof( m_Head ) );
	m_Head.uWidth	= -1;
	m_Head.uHeight	= -1;
	m_Head.uMipMap	= -1;
	m_pTextures	= NULL;
	m_bExa		= false;
	m_Head.uFrameCount = 0;
	m_pTextures = NULL;
	m_pTexDC = NULL;
	//m_pTextures[0] = NULL;
}

CDx9Texture::~CDx9Texture(void)
{
	ReleaseD3D();

}

void CDx9Texture::ReleaseD3D()
{
	if(m_pActiveCoder)
	{
		CSyncSystem::SafeDelCoder(m_pActiveCoder);
		m_pActiveCoder->Release();
		m_pActiveCoder = NULL;
	}

	if ( m_pTextures )
	{
		//if( CTextureQualityMgr::HasInst() )
		//	CTextureQualityMgr::GetInst()->_ChangeTextureSize( -int32(GetBufSize()),this  );

		if(m_pTexDC)
		{
			LPDIRECT3DSURFACE9 pSrcSuf;
			((IDirect3DTexture9*)m_pTextures[0])->GetSurfaceLevel(0,&pSrcSuf);
			if(pSrcSuf)
			{
				pSrcSuf->ReleaseDC(m_pTexDC);
				m_pTexDC=NULL;
				pSrcSuf->Release();
			}
		}

		SetBufSize(0);
		for ( uint32 i = 0; i < m_Head.uFrameCount; ++i )
			SafeRelease( m_pTextures[i] );
		SafeDelAry( m_pTextures );
		m_Head.uFrameCount = 0;
	}


}

//void CDx9Texture::_FreeRes(void)
//{
//	if(!m_IsLoaded && m_pActiveCoder==NULL)
//		ReleaseD3D();
//}
//
//void CDx9Texture::_Reclaim(void)
//{
//	if(!m_IsLoaded && m_pActiveCoder==NULL && CResUnit::IsValid())
//		ReLoadPic();
//}


////////////////////////////////////////////////////////////////////////
// 功能：创建空贴图
// 参数：CD3d* D3d 渲染设备,
//       const char* TextName 文件名
// 返回：>=0 成功，<0 失败
////////////////////////////////////////////////////////////////////////
int CDx9Texture::CreateEmpty( int32 nWidth, int32 nHeight, uint32 nLevel, eTextureFormat eFormat )
{
	ReleaseD3D();
	memset( &m_Head, 0, sizeof(sTextureInfo) );
	m_Head.eFormat		= eFormat;
	m_Head.uFrameCount	= 1;

	m_pTextures = new LPDIRTEX[1];
	m_pTextures[0] = NULL;

#ifdef USE_TEXTURE
	if (D3DXCreateTexture(GetD3DDevice(),
	                      nWidth, nHeight, nLevel, 0,
						  CDx9TexFmtMapper::GetInst()->GetSuitableTextureFormat( m_Head.eFormat ), D3DPOOL_MANAGED, (LPDIRECT3DTEXTURE9*)&m_pTextures[0] ) != D3D_OK )
		return GERROR;
#endif

	D3DSURFACE_DESC surf_desc;
	memset(&surf_desc, 0, sizeof(surf_desc));
	((LPDIRECT3DTEXTURE9)(m_pTextures[0]))->GetLevelDesc(0, &surf_desc);
	m_uWidth = surf_desc.Width;
	m_uHeight = surf_desc.Height;
	m_uMipMap = nLevel;

	m_Head.uWidth = surf_desc.Width;
	m_Head.uHeight = surf_desc.Height;
	if ( nLevel == 0 )
		m_Head.uMipMap	= -1;
	else
		m_Head.uMipMap	= nLevel;

	m_IsLoaded = true;
	return 0;
}

void CDx9Texture::ReadDataFromMem( CRefBufFile& GraphicFile )
{
}

int CDx9Texture::LoadCOMEx( CRefBufFile* pGraphicFile )                      //载入标准文件，bmp，tga，jpg
{
	//SetBufSize(0);
	ReleaseD3D();
	m_Head.uFrameCount = 1;
	m_pTextures = new LPDIRTEX[m_Head.uFrameCount];
	m_pTextures[0] = NULL;
	
	//重新生成D3DFMT_A8R8G8B8格式贴图
	
#ifdef USE_TEXTURE
	uint32 width	= m_Head.uWidth;
	uint32 height	= m_Head.uHeight;

	if ( pGraphicFile )
	{
		D3DXIMAGE_INFO ImageInf;
		D3DXGetImageInfoFromFileInMemory(pGraphicFile->GetBuf(), pGraphicFile->GetBufSize(), &ImageInf);
		if (-1 == m_Head.uWidth)
			m_Head.uWidth = ImageInf.Width;
		if (-1 == m_Head.uHeight)
			m_Head.uHeight = ImageInf.Height;
		if  (-1==m_Head.uMipMap)
			m_Head.uMipMap = ImageInf.MipLevels;

		width	= m_Head.uWidth;
		height	= m_Head.uHeight;
		uint32 mipLevel = m_Head.uMipMap;
		
		UINT QualityLevel = CTextureQualityMgr::GetInst()->GetQualityLevel();
		for( UINT level = 0; level < QualityLevel && mipLevel > 1 
			&& width % 2 == 0 && height % 2 == 0; ++level)
		{
			width  /= 2;
			height /= 2;
			-- mipLevel;
		}	

		HRESULT hr=D3DXCreateTextureFromFileInMemoryEx(GetD3DDevice(), pGraphicFile->GetBuf(), pGraphicFile->GetBufSize(),
			width , height, mipLevel, 0, CDx9TexFmtMapper::GetInst()->GetDataFormat(m_Head.eFormat), D3DPOOL_MANAGED, m_Filter, m_MinMapFilter, 0, NULL, NULL, (LPDIRECT3DTEXTURE9*)&m_pTextures[0]);

		if(hr == E_OUTOFMEMORY || hr == D3DERR_OUTOFVIDEOMEMORY)
		{
			CGraphic::GetInst()->ClearRecycle();
			hr=D3DXCreateTextureFromFileInMemoryEx(GetD3DDevice(), pGraphicFile->GetBuf(), pGraphicFile->GetBufSize(),
				width , height, mipLevel, 0, CDx9TexFmtMapper::GetInst()->GetDataFormat(m_Head.eFormat), D3DPOOL_MANAGED, m_Filter, m_MinMapFilter, 0, NULL, NULL, (LPDIRECT3DTEXTURE9*)&m_pTextures[0]);
		}

		if ( FAILED( hr ) )
		{
			string ErrStr = m_pParent->GetName();
			GraphicErrType ErrType = TextureCreate_Err;
			switch ( hr )
			{
			case D3DERR_NOTAVAILABLE:
				ErrStr += ": D3DERR_NOTAVAILABLE";
				break;
			case D3DERR_OUTOFVIDEOMEMORY:
				ErrStr += ": D3DERR_OUTOFVIDEOMEMORY";
				ErrType = Memory_Err;
				break;
			case D3DERR_INVALIDCALL:
				ErrStr += ": D3DERR_INVALIDCALL";
				break;
			case D3DXERR_INVALIDDATA:
				ErrStr += ": D3DXERR_INVALIDDATA";
				break;
			case E_OUTOFMEMORY:
				ErrStr += ": E_OUTOFMEMORY";
				ErrType = Memory_Err;
				break;
			default:
				ErrStr += ": D3DXCreateTextureFromFileInMemoryEx failed with unknown error";
			}
			GenErr( GraphicErr::GetErrTypeStr(ErrType),ErrStr );
		}
	}
	else
	{
		if ( FAILED( D3DXCreateTextureFromResourceEx(GetD3DDevice(), NULL, m_pParent->GetName(), 
			m_Head.uWidth , m_Head.uHeight, m_Head.uMipMap, 0, CDx9TexFmtMapper::GetInst()->GetDataFormat(m_Head.eFormat)
			, D3DPOOL_MANAGED, m_Filter, m_MinMapFilter, 0, NULL, NULL, (LPDIRECT3DTEXTURE9*)&m_pTextures[0] ) ) )
		{
			ostringstream strm;
			strm << "D3DXCreateTextureFromResourceEx failed:\"" << m_pParent->GetName() << "\"";
			GenErr( GraphicErr::GetErrTypeStr(TextureCreate_Err),strm.str() );
		}
	}

	if ( FAILED( D3DXFilterTexture( m_pTextures[0], NULL, 0, D3DX_FILTER_LINEAR ) ) )
	{
		string ErrStr = m_pParent->GetName();
		ErrStr += ": D3DXFilterTexture failed.";
		GenErr( GraphicErr::GetErrTypeStr(TextureCreate_Err),ErrStr);
	}

	//设置文件头
	SetBufSize( width*height*CDx9TexFmtMapper::GetInst()->GetBitsPerPixel(m_Head.eFormat)/8 );
	m_IsLoaded = true;
#endif

	return 0;
}

int CDx9Texture::LoadCOM( CRefBufFile* pGraphicFile )
{
	
	ReleaseD3D();
	m_Head.uFrameCount = 1;
	m_pTextures = new LPDIRTEX[m_Head.uFrameCount];
	m_pTextures[0] = NULL;
	

	D3DXIMAGE_INFO ImageInf;
	D3DXGetImageInfoFromFileInMemory(pGraphicFile->GetBuf(), pGraphicFile->GetBufSize(), &ImageInf);

	if ( -1 == m_Head.uWidth)
		m_Head.uWidth = ImageInf.Width;
	if ( -1 == m_Head.uHeight)
		m_Head.uHeight = ImageInf.Height;
	if  (-1== m_Head.uMipMap)
		m_Head.uMipMap = ImageInf.MipLevels;


	D3DFORMAT fmt = ImageInf.Format;
	if ( fmt == D3DFMT_R8G8B8 )
		fmt = D3DFMT_X8R8G8B8;

	uint32 width	= m_Head.uWidth;
	uint32 height	= m_Head.uHeight;
	uint32 mipLevel = m_Head.uMipMap;

	eTextureFormat eMft = TF_DXT1;
	while ( eMft < TF_COUNT && CDx9TexFmtMapper::GetInst()->GetDataFormat(eMft) != fmt )
		eMft = (eTextureFormat)( eMft + 1 );
	m_Head.eFormat = eMft;

	UINT QualityLevel = CTextureQualityMgr::GetInst()->GetQualityLevel();

	for( UINT level = 0; level < QualityLevel && mipLevel > 1 
		&& width % 2 == 0 && height % 2 == 0 && width>4 && height>4; ++level)
	{
		width  /= 2;
		height /= 2;
		-- mipLevel;
	}	
	if( CDx9TexFmtMapper::GetInst()->GetUseFormat(eMft) != CDx9TexFmtMapper::GetInst()->GetDataFormat(eMft) )
	{
		CSyncSystem::PushCoder(new CDx9DefaultTexCoder(this, pGraphicFile, width, height, mipLevel ));
		return 0;
	}

	switch(ImageInf.ImageFileFormat)
	{
	case D3DXIFF_DDS:
		{
			DDSCREATEPARAMS ddsParams;
			ddsParams.m_eTexFmt		= CDx9TexFmtMapper::GetInst()->GetUseFormat(eMft);
			ddsParams.m_uHeight		= height;
			ddsParams.m_uWidth		= width;
			ddsParams.m_uMipMap		= mipLevel;

			ddsParams.m_eOrgTexFmt	= ImageInf.Format;
			ddsParams.m_uOrgHeight	= ImageInf.Height;
			ddsParams.m_uOrgWidth	= ImageInf.Width;
			ddsParams.m_uOrgMipMap	= ImageInf.MipLevels;

			switch(ImageInf.ResourceType)
			{
			case D3DRTYPE_TEXTURE:
				CSyncSystem::PushCoder(new CDx9DDSTexture(this, pGraphicFile, ddsParams));
				break;
			case D3DRTYPE_VOLUMETEXTURE:
				CSyncSystem::PushCoder(new CDx9DDSVolumeTexture(this, pGraphicFile, ddsParams,ImageInf.Depth));
				break;
			case D3DRTYPE_CUBETEXTURE:
				CSyncSystem::PushCoder(new CDx9DDSCubeTexture(this, pGraphicFile, ddsParams));
				break;
			}
		}
		break;
	default:
		CSyncSystem::PushCoder(new CDx9DefaultTexCoder(this, pGraphicFile, width, height, mipLevel ));
	}

	return 0;
}

int CDx9Texture::LoadTEX( CRefBufFile* pGraphicFile )
{
	SQR_TRY
	{
		CBufFile& GraphicFile = *pGraphicFile;
		
		//读入文件头
		uint32 Res;
		uint32 Version;
		GraphicFile.read( &Res, sizeof(uint32) );
		GraphicFile.read( &Version, sizeof(uint32) );

		if ( Res != MAKEFOURCC( 'T','E','X', 0 ) )
			GenErr( GraphicErr::GetErrTypeStr(FileFormat_Err), m_pParent->GetName());


		if( Version < TEXTURE_VERSION - 1 || Version > TEXTURE_VERSION )
			GenErr( GraphicErr::GetErrTypeStr(FileVer_Err), m_pParent->GetName());
		
		ReleaseD3D();
		
		if ( Version == TEXTURE_VERSION )
			GraphicFile.read( (char*)&m_Head,sizeof(m_Head) );
		else
		{
			int32 uBuf[16];
			GraphicFile.read( (char*)uBuf, sizeof(uBuf) );
			m_Head.eFormat = (eTextureFormat)uBuf[1];
			m_Head.uWidth = uBuf[3];
			m_Head.uHeight = uBuf[4];
			m_Head.uMipMap = 0;
			m_Head.uFrameCount = 1;
			m_Head.uFrameCircle = 0;
		}

		D3DFORMAT Format	= CDx9TexFmtMapper::GetInst()->GetDataFormat(m_Head.eFormat);

		//按D3DFMT_A8R8G8B8格式读入数据
		m_pTextures = new LPDIRTEX[ m_Head.uFrameCount ];
		memset( m_pTextures, 0, sizeof( m_pTextures ) );
		for ( uint32 i = 0; i < m_Head.uFrameCount; i++ )
		{
			LPDIRECT3DTEXTURE9	pText;
			HRESULT hr = D3DXCreateTexture(
				GetD3DDevice(), m_Head.uWidth,m_Head.uHeight,m_Head.uMipMap, 0,
				CDx9TexFmtMapper::GetInst()->GetSuitableTextureFormat( m_Head.eFormat ), 
				D3DPOOL_MANAGED, &pText 
				);
			if ( hr != D3D_OK )
			{
				stringstream ss;
				GString ErrStr = m_pParent->GetName();
				ErrStr += ": D3DXCreateTexture failed. ";
				ss << " w:" << m_Head.uWidth << " h:" << m_Head.uHeight << " mip:" << m_Head.uMipMap
					<< "fmt:" << m_Head.eFormat << " ";
				ErrStr += ss.str().c_str();
				ErrStr += DXGetErrorString(hr);
				ErrStr += " - ";
				ErrStr += DXGetErrorDescription(hr);
				GenErr( GraphicErr::GetErrTypeStr(TextureCreate_Err),ErrStr.c_str() );
			}
			m_pTextures[i] = pText;

			uint32 Size = 0;
			if ( Version == TEXTURE_VERSION - 1 )
			{
				m_Head.uMipMap = MAX_MIPMAP;
				GraphicFile.read( (char*)&Size,sizeof(Size) );
			}
			else
			{
				Ast( m_Head.uMipMap );
				Size = m_Head.uWidth*m_Head.uHeight*CDx9TexFmtMapper::GetInst()->GetBitsPerPixel(m_Head.eFormat)/8;
			}

	#ifdef IS_SHOW_SIZE
			g_TotalSize += Size;
	#endif

			RECT		rect		= { 0, 0, m_Head.uWidth,m_Head.uHeight };
			int32		nSpace		= CDx9TexFmtMapper::GetInst()->GetPitch(m_Head.eFormat, m_Head.uWidth);
			size_t		nCurPos		= GraphicFile.Tell();
			const char* pBuf		= GraphicFile.GetBuf() + nCurPos;
			SetBufSize(Size);

			for ( uint32 j = 0; j < m_Head.uMipMap; j++ )
			{
				if ( GraphicFile.GetBufSize() < static_cast<int32>(GraphicFile.Tell() + Size) )
				{
					string ErrStr = m_pParent->GetName();
					ErrStr += ": GraphicFile pos error";
					GenErr( GraphicErr::GetErrTypeStr(FileRead_Err),ErrStr );
				}

				LPSURFACE pSur;
				pText->GetSurfaceLevel( j, &pSur );

				if ( !pSur )
				{
					m_Head.uMipMap = j;
					return 0;	//贴图太小，达不到要求的那么多层MipMap
				}

				if ( FAILED( D3DXLoadSurfaceFromMemory( pSur, NULL, NULL, pBuf, Format,
														nSpace, NULL, &rect, 
														D3DX_FILTER_LINEAR, 0 ) ) )
				{
					GString ErrStr = m_pParent->GetName();
					ErrStr += ": D3DXLoadSurfaceFromMemory failed.";
					SafeRelease( pSur );
					GenErr( GraphicErr::GetErrTypeStr(TextureCreate_Err),ErrStr.c_str() );
				}
				SafeRelease( pSur );

				if ( Version == TEXTURE_VERSION )
				{
					GraphicFile.Seek( GraphicFile.Tell() + Size );
					pBuf += Size;
					Size = Size>>2;
					rect.right = rect.right>>1;
					rect.bottom = rect.bottom>>1;
					nSpace = nSpace>>1;
				}
			}

			if ( Version == TEXTURE_VERSION - 1 )
			{
				GraphicFile.Seek( nCurPos + Size );
			}
		}
		m_IsLoaded = true;
	}
	SQR_CATCH(exp)
	{
		GenErr(exp.ErrorTitle(),exp.ErrorMsg());
	}
	SQR_TRY_END;

	return 0;
}

void* CDx9Texture::GetBuffer() const
{
#ifdef USE_TEXTURE
	if ( m_Head.uFrameCount > 1 )
		return m_pTextures[ ( GetProcessTime()/max( m_Head.uFrameCircle, 1 ) ) %( m_Head.uFrameCount ) ];
	else
		return m_pTextures ? m_pTextures[0] : NULL;
#else
	return NULL;
#endif
}

int CDx9Texture::FillFromHBitmap( HBITMAP hBitmap, HBITMAP hMask )
{

#ifdef USE_TEXTURE
	if ( !hBitmap )
		return m_pParent->LoadPic();

	char BmpInfo[2048];
	ZeroMemory( BmpInfo, 2048 );
	BITMAPINFO& bmi = *( (BITMAPINFO*)BmpInfo );
	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	HDC hdcScreen = GetDC( NULL );
	if ( !GetDIBits( hdcScreen, hBitmap, 0, 0, 0, &bmi, DIB_RGB_COLORS ) )
	{
		ReleaseDC( NULL, hdcScreen );
		return GERROR;
	}

	LPDIRECT3DTEXTURE9 pTexture;
	if ( D3DXCreateTexture(GetD3DDevice(), bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight, MAX_MIPMAP, 0,
	                       D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture ) != D3D_OK )
	{
		ReleaseDC( NULL, hdcScreen );
		return GERROR;
	}

	ReleaseD3D();
	m_Head.uFrameCount = 1;
	m_pTextures = new LPDIRTEX[m_Head.uFrameCount];
	m_pTextures[0] = pTexture;
	m_Head.uWidth		= bmi.bmiHeader.biWidth;
	m_Head.uHeight		= bmi.bmiHeader.biHeight;
	m_Head.eFormat		= TF_UNKNOW;
	m_Head.uFrameCount	= 1;
	m_Head.uMipMap		= 1;

	uint32* Buf = new uint32[bmi.bmiHeader.biWidth*bmi.bmiHeader.biHeight*3];
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = 0;
	GetDIBits( hdcScreen, hBitmap, 0, bmi.bmiHeader.biHeight, Buf, &bmi, DIB_RGB_COLORS );
	if ( hMask )
	{
		uint32* MaskBuf = new uint32[bmi.bmiHeader.biWidth*bmi.bmiHeader.biHeight*3];
		GetDIBits( hdcScreen, hMask, 0, bmi.bmiHeader.biHeight, MaskBuf, &bmi, DIB_RGB_COLORS );
		for ( int i = 0; i < bmi.bmiHeader.biWidth*bmi.bmiHeader.biHeight; i++ )
			Buf[i] = MaskBuf[i] ? ( Buf[i]&0xffffff ) : ( Buf[i]|0xff000000 );
		delete[]MaskBuf;
	}
	else
	{
		for ( int i = 0; i < bmi.bmiHeader.biWidth*bmi.bmiHeader.biHeight; i++ )
			Buf[i] |= 0xff000000;
	}
	ReleaseDC( NULL, hdcScreen );

	for ( int i = 0; i < bmi.bmiHeader.biHeight/2; i++ )
	{
		uint32* Line1 = Buf + i*bmi.bmiHeader.biWidth;
		uint32* Line2 = Buf + ( bmi.bmiHeader.biHeight - 1 - i )*bmi.bmiHeader.biWidth;
		for ( int j = 0; j < bmi.bmiHeader.biWidth; j++ )
		{
			uint32 c = Line1[j];
			Line1[j] = Line2[j];
			Line2[j] = c;
		}
	}

	LPSURFACE pSur;
	((LPDIRECT3DTEXTURE9)m_pTextures[0])->GetSurfaceLevel( 0, &pSur );
	RECT rect = { 0, 0, m_Head.uWidth,m_Head.uHeight };
	D3DXLoadSurfaceFromMemory( pSur, NULL, NULL, Buf, D3DFMT_A8R8G8B8, m_Head.uWidth*4, NULL, &rect, D3DX_FILTER_NONE, 0 );
	SafeRelease( pSur );
	delete []Buf;
#endif
	return 0;//D3DXFilterTexture( m_Texture, NULL, 0, D3DX_FILTER_TRIANGLE );
}

int CDx9Texture::FillFromMemory( const void* pMemory, eTextureFormat Format, uint32 nWidth, const CIRect& rtSrc, const CIRect& rtDes )
{
	
#ifdef USE_TEXTURE
	if( m_pTextures[0]->GetType() != D3DRTYPE_TEXTURE )
		return GERROR;

	D3DFORMAT D3DFormat;
	uint32 Space;

	if     ( Format == TF_DXT1   )
	{
		Space  = nWidth*2;
		D3DFormat = D3DFMT_DXT1;
	}
	else if ( Format == TF_DXT5   )
	{
		Space  = nWidth*4;
		D3DFormat = D3DFMT_DXT5;
	}
	else if ( Format == TF_RGB24  )
	{
		Space  = nWidth*3;
		D3DFormat = D3DFMT_R8G8B8;
	}
	else if ( Format == TF_ARGB32 )
	{
		Space  = nWidth*4;
		D3DFormat = D3DFMT_A8R8G8B8;
	}
	else if ( Format == TF_XRGB32 )
	{
		Space  = nWidth*4;
		D3DFormat = D3DFMT_X8R8G8B8;
	}
	else if ( Format == TF_R5G6B5 )
	{
		Space  = nWidth*2;
		D3DFormat = D3DFMT_R5G6B5;
	}
	else if ( Format == TF_A4R4G4B4 )
	{
		Space  = nWidth*2;
		D3DFormat = D3DFMT_A4R4G4B4;
	}
	else return GERROR;

	if ( rtDes.right <= rtDes.left || rtDes.bottom <= rtDes.top || rtSrc.right <= rtSrc.left || rtSrc.bottom <= rtSrc.top )
		return GERROR;

	if ( rtDes.left < 0 || rtDes.right > (long)m_Head.uWidth || rtDes.top < 0 || rtDes.bottom > (long)m_Head.uHeight )
	{
		GString ErrStr = m_pParent->GetName();
		ErrStr += ": 尺寸不合适";
		GenErr( GraphicErr::GetErrTypeStr(TextureCreate_Err),ErrStr.c_str() );
	}

	CIRect destRect = rtDes;
	CIRect* pDes = &destRect;
	uint32 uFilter = rtDes.Width() == rtSrc.Width() && rtDes.Height() == rtSrc.Height() ? D3DX_FILTER_NONE : D3DX_FILTER_LINEAR;
	if ( rtDes.left == 0 && rtDes.top == 0 && rtDes.right == (long)m_Head.uWidth && rtDes.bottom == (long)m_Head.uHeight )
		pDes = NULL;
	
	for ( uint32 level=0; level<GetLevelNum(); ++level )
	{
		if ( level > 0 )
			uFilter = D3DX_FILTER_LINEAR;

		for ( uint32 i = 0; i < m_Head.uFrameCount; i++ )
		{
			LPSURFACE pSur;
			((LPDIRECT3DTEXTURE9)m_pTextures[i])->GetSurfaceLevel( level, &pSur );
			D3DXLoadSurfaceFromMemory( pSur, NULL, (const RECT*)pDes, pMemory, D3DFormat, Space, NULL, (const RECT*)&rtSrc, uFilter, 0 );
			SafeRelease( pSur );
		}

		if ( pDes )
		{
			pDes->left		>>= 1;
			pDes->top		>>= 1;
			pDes->right		>>= 1;
			pDes->bottom	>>= 1;
		}
	}

#endif
	return 0;//D3DXFilterTexture( m_Texture, NULL, 0, D3DX_FILTER_TRIANGLE );
}

HRESULT	CDx9Texture::Lock( uint32 level, _TEXLOCKED_RECT* pTexLocked, CIRect* Rect,uint32 Num, uint32 Flags )
{
	//为了保证万一通用CIRect的基础数据类型发生变化产生的BUG，这个地方不用指针转换
	RECT lockRect;
	RECT* pLockRect = NULL;
	if( Rect != NULL )
	{
		pLockRect	= &lockRect;
		lockRect.left	=  Rect->left;
		lockRect.top	=  Rect->top;
		lockRect.right	=  Rect->right;
		lockRect.bottom	=  Rect->bottom; 
	}

	switch(m_pTextures[0]->GetType())
	{
	case D3DRTYPE_TEXTURE:
		{
			LPDIRECT3DTEXTURE9 pText = (LPDIRECT3DTEXTURE9)m_pTextures[Num];
			if(NULL!=pText)
				return pText->LockRect( level,(D3DLOCKED_RECT*)pTexLocked,pLockRect,Flags );
		}
		break;
	case D3DRTYPE_CUBETEXTURE:
		{
			LPDIRECT3DCUBETEXTURE9 pCubeTex = (LPDIRECT3DCUBETEXTURE9)m_pTextures[0];
			if(NULL!=pCubeTex)
				return pCubeTex->LockRect( (D3DCUBEMAP_FACES)Num,level,(D3DLOCKED_RECT*)pTexLocked,pLockRect,Flags);
		}
		break;
	default:	
		break;
	}
	return GERROR;
}

HRESULT	CDx9Texture::Unlock( uint32 level ,uint32 Num )
{
	switch(m_pTextures[0]->GetType())
	{
	case D3DRTYPE_TEXTURE:
		{
			LPDIRECT3DTEXTURE9 pText = (LPDIRECT3DTEXTURE9)m_pTextures[Num];
			if(NULL!=pText)
				return pText->UnlockRect( level );
		}
		break;
	case D3DRTYPE_CUBETEXTURE:
		{
			LPDIRECT3DCUBETEXTURE9 pCubeTex = (LPDIRECT3DCUBETEXTURE9)m_pTextures[0];
			if(NULL!=pCubeTex)
				return pCubeTex->UnlockRect( (D3DCUBEMAP_FACES)Num,level );
		}
		break;
	default:	
		break;
	}
	return GERROR;
}

bool	CDx9Texture::IsTransparent(float x,float y) const
{
	if (m_Head.eFormat != TF_A8 || m_pTextures ==NULL ||  
		m_pTextures[0] == NULL ||m_pTextures[0]->GetType() != D3DRTYPE_TEXTURE )
		return false;

	LPDIRECT3DTEXTURE9 pText = (LPDIRECT3DTEXTURE9)m_pTextures[0];
	RECT rect0;
	D3DLOCKED_RECT lockO;
	uint8 ret;
	rect0.top = (LONG)(y*m_Head.uHeight);// - (LONG(y)/(m_Head.uHeight))*(m_Head.uHeight);
	rect0.left = (LONG)(x*m_Head.uWidth);// - (LONG(x)/(m_Head.uWidth))*(m_Head.uWidth);
	rect0.bottom = min( rect0.top + 1, (LONG)m_Head.uHeight );
	rect0.right =  min( rect0.left + 1,(LONG)m_Head.uWidth );
	
	HRESULT hr = pText->LockRect(0,&lockO,&rect0,D3DLOCK_READONLY);
	if (FAILED(hr))
		return false;
	if(CDx9TexFmtMapper::GetInst()->GetUseFormat(TF_A8) == D3DFMT_A8)
		ret = *((uint8*)(lockO.pBits));
	else
		ret = *((uint8*)(lockO.pBits) + 3);
	pText->UnlockRect(0);
	return ret == 0;
}

void CDx9Texture::SetBufSize( uint32 uSize )
{
	m_pParent->SetBufSize(uSize);
}

HRESULT CDx9Texture::SaveToFile(const string& filename, IMAGE_FILEFORMAT Format)
{
	if(Format>=IFF_DXT1)
		Format = IFF_DDS;
	return D3DXSaveTextureToFileA(filename.c_str(),(D3DXIMAGE_FILEFORMAT)Format, m_pTextures[0], NULL);
}

bool CDx9Texture::FillFromTexture( const ITexture* pText, const CIRect* rtSrc, const CIRect* rtDes )
{
	if(pText==NULL)
		return false;

	LPDIRTEX pSrcTex = (LPDIRTEX)pText->GetBuffer();
	
	if( pSrcTex == NULL || pSrcTex->GetType()!=D3DRTYPE_TEXTURE || m_pTextures[0] == NULL || m_pTextures[0]->GetType()!=D3DRTYPE_TEXTURE )
		return false;
	
	LPDIRECT3DSURFACE9 pSrcSuf,pDestSuf;
	((IDirect3DTexture9*)pSrcTex)->GetSurfaceLevel(0,&pSrcSuf);
	((IDirect3DTexture9*)m_pTextures[0])->GetSurfaceLevel(0,&pDestSuf);

	D3DXLoadSurfaceFromSurface(pDestSuf,NULL,(RECT*)rtDes,pSrcSuf,NULL,(RECT*)rtSrc,D3DX_FILTER_POINT,0 );
	pSrcSuf->Release();
	pDestSuf->Release();
	return 0;
}

void CDx9Texture::SetCustomParams( uint Width, uint Height,uint MinMap, eTextureFormat eFormat, uint32 Filter, uint32 MipFilter )
{
	m_Filter		= Filter;
	m_MinMapFilter	= MipFilter;
	m_Head.eFormat	= eFormat;
	m_Head.uWidth	= Width;			
	m_Head.uHeight	= Height;		
	m_Head.uMipMap	= MinMap;
	m_Head.uFrameCount = 1;
	m_Head.uFrameCircle= 0;
}

const char* sqr::CDx9Texture::GetName() const
{
	return m_pParent->GetName();
}

uint32 sqr::CDx9Texture::GetRefCount()
{
	return m_pParent->GetRefCount();
}

void sqr::CDx9Texture::AddRef()
{
	m_pParent->AddRef();
}

void sqr::CDx9Texture::Release()
{
	return m_pParent->Release();
}

void sqr::CDx9Texture::_Reclaim()
{
	if(m_pActiveCoder)
	{
		m_pActiveCoder->SetPRI(SPRI_High);
	}
}

void sqr::CDx9Texture::Reclaim()
{
	m_pParent->Reclaim();
}

const char* sqr::CDx9Texture::GetType() const
{
	return CDx9Texture::Type();
}

const char* sqr::CDx9Texture::Type()
{
	return "told";
}

void sqr::CDx9Texture::SetLod( uint32 lod )
{
	for ( uint32 i = 0; i < m_Head.uFrameCount; i++ )
		((LPDIRECT3DTEXTURE9)m_pTextures[i])->SetLOD(lod);
}

void CDx9Texture::AutoGenMip()
{
	if ( GetLevelNum() <= 1 )
		return;

	for ( uint32 i = 0; i < m_Head.uFrameCount; i++ )
	{
		LPSURFACE pSur0 = NULL;
		((LPDIRECT3DTEXTURE9)m_pTextures[i])->GetSurfaceLevel(0, &pSur0);
		for ( uint32 level=1; level<GetLevelNum(); ++level )
		{
			LPSURFACE pSur;
			((LPDIRECT3DTEXTURE9)m_pTextures[i])->GetSurfaceLevel( level, &pSur );
			D3DXLoadSurfaceFromSurface(pSur, NULL, NULL, pSur0, NULL, NULL, D3DX_FILTER_LINEAR, 0);
			SafeRelease( pSur );
		}
		SafeRelease(pSur0);
	}
}

HDC	 CDx9Texture::GetHDC(void)
{
	SQR_TRY
	{
		LPDIRECT3DSURFACE9 pSrcSuf;
		((IDirect3DTexture9*)m_pTextures[0])->GetSurfaceLevel(0,&pSrcSuf);
		if(pSrcSuf)
		{
			if(m_pTexDC)
			{
				pSrcSuf->ReleaseDC(m_pTexDC);
				m_pTexDC=NULL;
			}
			pSrcSuf->GetDC(&m_pTexDC);
			::SetMapMode(m_pTexDC,MM_TEXT);
			pSrcSuf->Release();
		}
	}
	SQR_CATCH(exp)
	{
		GenErr(exp.ErrorTitle(),exp.ErrorMsg());
	}
	SQR_TRY_END;

	return m_pTexDC;
}