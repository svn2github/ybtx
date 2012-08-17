#include "stdafx.h"
#include "CDx9TexFmtMapper.h"
#include "CWindowTarget.h"
#include "CHardwareCapacity.h"


namespace sqr
{
	// FORMAT		D3DFORMAT			BytePerPixel LinePerPitch Sapce			
	// TF_DXT1		D3DFMT_DXT1			4,			 4,			  2,   
	// TF_RGB24		D3DFMT_DXT5			8,			 4,			  4,
	// TF_ARGB32	D3DFMT_R8G8B8		24,			 1,			  3,
	// TF_R5G6B5	D3DFMT_A8R8G8B8		32,			 1,			  4,
	// TF_A4R4G4B4	D3DFMT_R5G6B5		16,			 1,			  2,
	// TF_A8		D3DFMT_A4R4G4B4		16,			 1,			  2,
	// TF_XRGB32	D3DFMT_A8			8,			 1,			  1,
	// TF_L8		D3DFMT_X8R8G8B8		32,			 1,			  4,
	// TF_R32F		D3DFMT_L8			8,			 1,			  1,
	
	

	CDx9TexFmtMapper::CDx9TexFmtMapper()
		: m_DepthTexFormat(D3DFMT_UNKNOWN)
	{
		RegisterTexureFormat(TF_DXT1,		4,	4,	2, D3DFMT_DXT1,		D3DFMT_DXT1);
		RegisterTexureFormat(TF_DXT3,		8,	4,	4, D3DFMT_DXT3,		D3DFMT_DXT3);
		RegisterTexureFormat(TF_DXT5,		8,	4,	4, D3DFMT_DXT5,		D3DFMT_DXT5);
		RegisterTexureFormat(TF_RGB24,		24,	1,	3, D3DFMT_R8G8B8,	D3DFMT_R8G8B8);
		RegisterTexureFormat(TF_ARGB32,		32,	1,	4, D3DFMT_A8R8G8B8,	D3DFMT_A8R8G8B8);
		RegisterTexureFormat(TF_R5G6B5,		16,	1,	2, D3DFMT_R5G6B5,	D3DFMT_R5G6B5);
		RegisterTexureFormat(TF_A4R4G4B4,	16,	1,	2, D3DFMT_A4R4G4B4,	D3DFMT_A4R4G4B4);
		RegisterTexureFormat(TF_A8,			8,	1,	1, D3DFMT_A8,		D3DFMT_A8);
		RegisterTexureFormat(TF_XRGB32,		32,	1,	4, D3DFMT_X8R8G8B8,	D3DFMT_X8R8G8B8);
		RegisterTexureFormat(TF_L8,			8,	1,	1, D3DFMT_L8,		D3DFMT_L8);
		RegisterTexureFormat(TF_R32F,		32,	1,	4, D3DFMT_R32F,		D3DFMT_R32F);

		if( !CHardwareCapacity::GetInst()->IsSupportTexture(TFMT_A8) )
		{
			RegisterTexureFormat(TF_A8,		8,	1,	1, D3DFMT_A8,		D3DFMT_A8R8G8B8);
		}
		EnumSuitableTextureFormat();
	}

	CDx9TexFmtMapper::~CDx9TexFmtMapper()
	{

	}

	void CDx9TexFmtMapper::RegisterTexureFormat( 
		eTextureFormat	fmt, 
		int32			nBitsPerPixel, 
		int32			nLinePerPitch, 
		int32			nPitch, 
		D3DFORMAT		dataFormat, 
		D3DFORMAT		useFormamt 
		)
	{
		TexFmtInfo fmtInfo;
		fmtInfo.nBitsPerPixel	= nBitsPerPixel;
		fmtInfo.nLinePerPitch	= nLinePerPitch;
		fmtInfo.nPitch			= nPitch;
		fmtInfo.dataFormat		= dataFormat;
		fmtInfo.useFormat		= useFormamt;
		m_fmtMap[fmt] = fmtInfo;
	}

	HRESULT CDx9TexFmtMapper::EnumSuitableTextureFormat()
	{
		// 不带通道贴图格式
		m_TextFmt[0][0] = D3DFMT_DXT1;
		// 带通道贴图格式
		m_TextFmt[0][1] = D3DFMT_DXT5;
		// 检测2d贴图格式

		// 32位
		if ( 32 == CMainWindowTarget::GetInst()->GetRenderSettings().GetColorDepth() )
		{
			// 不带通道贴图
			m_TextFmt[1][0] = D3DFMT_X8R8G8B8;
			// 带通道贴图
			m_TextFmt[1][1] = D3DFMT_A8R8G8B8;
		}
		else// 16位
		{
			// 不带通道
			m_TextFmt[1][0] = D3DFMT_R5G6B5;
			// 带通道贴图
			m_TextFmt[1][1] = D3DFMT_A4R4G4B4;
		}

		if ( CHardwareCapacity::GetInst()->IsSupportDepthTexture(TFMT_D32F_LOCKABLE) )
			m_DepthTexFormat = D3DFMT_D32F_LOCKABLE;
		else if ( CHardwareCapacity::GetInst()->IsSupportDepthTexture(TFMT_D24S8) )
			m_DepthTexFormat = D3DFMT_D24S8;
		else if ( CHardwareCapacity::GetInst()->IsSupportDepthTexture(TFMT_D24X8) )
			m_DepthTexFormat = D3DFMT_D24X8;
		else if ( CHardwareCapacity::GetInst()->IsSupportDepthTexture(TFMT_D16) )
			m_DepthTexFormat = D3DFMT_D16;

		return 0;
	}

	D3DFORMAT CDx9TexFmtMapper::GetSuitableTextureFormat( eTextureFormat fmt )
	{
		if ( fmt == TF_DXT1 )
			return m_TextFmt[0][0];
		if ( fmt == TF_DXT5 )
			return m_TextFmt[0][1];
		if ( fmt == TF_RGB24 || fmt == TF_R5G6B5 )
			return m_TextFmt[1][0];
		if ( fmt == TF_ARGB32 || fmt == TF_A4R4G4B4 )
			return m_TextFmt[1][1];
		if ( fmt == TF_A8 )
			return GetDx9Format(fmt);
		if ( fmt == TF_L8 )
			return D3DFMT_L8;
		if ( fmt == TF_R32F)
			return D3DFMT_R32F;
		if ( fmt == TF_DEPTH )
			return m_DepthTexFormat;

		return D3DFMT_UNKNOWN;
	}

	D3DFORMAT CDx9TexFmtMapper::GetDx9Format( eTextureFormat fmt )
	{
		return m_fmtMap[fmt].useFormat;
	}

	uint32 CDx9TexFmtMapper::GetBitsPerPixel( eTextureFormat fmt )
	{
		return m_fmtMap[fmt].nBitsPerPixel;
	}

	uint32 CDx9TexFmtMapper::GetLinesPerPitch( eTextureFormat fmt )
	{
		return m_fmtMap[fmt].nLinePerPitch;
	}

	uint32 CDx9TexFmtMapper::GetPitch( eTextureFormat fmt, uint32 nWidth )
	{
		if ( nWidth < 4 )
		{
			if ( fmt == TF_DXT1 )
				return 8;

			if ( fmt == TF_DXT3 || fmt == TF_DXT5 )
				return 16;
		}
			
		return m_fmtMap[fmt].nPitch * nWidth;
	}

	D3DFORMAT CDx9TexFmtMapper::GetDataFormat( eTextureFormat fmt )
	{
		return m_fmtMap[fmt].dataFormat;
	}

	D3DFORMAT CDx9TexFmtMapper::GetUseFormat( eTextureFormat fmt )
	{
		return m_fmtMap[fmt].useFormat;
	}

}
