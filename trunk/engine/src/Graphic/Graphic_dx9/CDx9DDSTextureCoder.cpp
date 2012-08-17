#include "stdafx.h"
#include "CDx9DDSTextureCoder.h"
#include "CDx9Texture.h"
#include "BaseHelper.h"
#include "StringHelper.h"
#include "CGraphicExp.h"
#include "CGraphic.h"
#include "CDx9MainWindow.h"
#include "DebugHelper.h"
#include <Dxerr.h>

namespace sqr
{
	static const uint32 MIN_SKIP_SIZE = 4;

	static uint32 getSkipLevelNum( uint32 uOrgWidth, uint32 uOrgHeight, uint32 uOrgMip, D3DFORMAT orgFmt )
	{
		uint32 skipLevelNum = 0;
		if ( orgFmt == D3DFMT_DXT1 || orgFmt == D3DFMT_DXT3 || orgFmt == D3DFMT_DXT5 )
		{
			uint32 uMinSize = max(1, min(uOrgWidth >> (uOrgMip-1), uOrgHeight >> (uOrgMip-1)));
			if ( uMinSize < MIN_SKIP_SIZE )
				skipLevelNum = uint32(CMath::Logbase(MIN_SKIP_SIZE, 2) - CMath::Logbase(uMinSize, 2));
		}

		return skipLevelNum;
	}

	UINT BitsPerPixel( D3DFORMAT fmt )
	{
		UINT fmtU = (UINT)fmt;
		switch( fmtU )
		{
		case D3DFMT_A32B32G32R32F:
			return 128;

		case D3DFMT_A16B16G16R16:
		case D3DFMT_Q16W16V16U16:
		case D3DFMT_A16B16G16R16F:
		case D3DFMT_G32R32F:
			return 64;

		case D3DFMT_A8R8G8B8:
		case D3DFMT_X8R8G8B8:
		case D3DFMT_A2B10G10R10:
		case D3DFMT_A8B8G8R8:
		case D3DFMT_X8B8G8R8:
		case D3DFMT_G16R16:
		case D3DFMT_A2R10G10B10:
		case D3DFMT_Q8W8V8U8:
		case D3DFMT_V16U16:
		case D3DFMT_X8L8V8U8:
		case D3DFMT_A2W10V10U10:
		case D3DFMT_D32:
		case D3DFMT_D24S8:
		case D3DFMT_D24X8:
		case D3DFMT_D24X4S4:
		case D3DFMT_D32F_LOCKABLE:
		case D3DFMT_D24FS8:
		case D3DFMT_INDEX32:
		case D3DFMT_G16R16F:
		case D3DFMT_R32F:
			return 32;

		case D3DFMT_R8G8B8:
			return 24;

		case D3DFMT_A4R4G4B4:
		case D3DFMT_X4R4G4B4:
		case D3DFMT_R5G6B5:
		case D3DFMT_L16:
		case D3DFMT_A8L8:
		case D3DFMT_X1R5G5B5:
		case D3DFMT_A1R5G5B5:
		case D3DFMT_A8R3G3B2:
		case D3DFMT_V8U8:
		case D3DFMT_CxV8U8:
		case D3DFMT_L6V5U5:
		case D3DFMT_G8R8_G8B8:
		case D3DFMT_R8G8_B8G8:
		case D3DFMT_D16_LOCKABLE:
		case D3DFMT_D15S1:
		case D3DFMT_D16:
		case D3DFMT_INDEX16:
		case D3DFMT_R16F:
		case D3DFMT_YUY2:
			return 16;

		case D3DFMT_R3G3B2:
		case D3DFMT_A8:
		case D3DFMT_A8P8:
		case D3DFMT_P8:
		case D3DFMT_L8:
		case D3DFMT_A4L4:
			return 8;

		case D3DFMT_DXT1:
			return 4;
		case D3DFMT_DXT2:
		case D3DFMT_DXT3:
		case D3DFMT_DXT4:
		case D3DFMT_DXT5:
			return  8;

		case D3DFMT_UYVY:
			return 16;

		case MAKEFOURCC( 'A', 'I', '4', '4' ):
		case MAKEFOURCC( 'I', 'A', '4', '4' ):
			return 8;

		case MAKEFOURCC( 'Y', 'V', '1', '2' ):
			return 12;

		default:
			Ast(FALSE); // unhandled format
			return 0;
		}
	}

	void GetMipMapInfo( UINT uWidth, UINT uHeight, D3DFORMAT fmt, UINT uMipMap,
		UINT* pNumBytes, UINT* pRowBytes, UINT* pNumRows )
	{
		UINT numBytes = 0;
		UINT rowBytes = 0;
		UINT numRows  = 0;

		uWidth  >>= uMipMap;
		uHeight >>= uMipMap;
		if( uWidth == 0 )
			uWidth = 1;
		if( uHeight == 0 )
			uHeight = 1;

		if( fmt == TFMT_DXT1 || fmt == TFMT_DXT2 || fmt == TFMT_DXT3 || 
			fmt == TFMT_DXT4 || fmt == TFMT_DXT5 )
		{
			int numBlocksWide = 0;
			numBlocksWide = max( 1, uWidth / 4 );
			int numBlocksHigh = 0;
			numBlocksHigh = max( 1, uHeight / 4 );
			int numBytesPerBlock = ( fmt == D3DFMT_DXT1 ? 8 : 16 );
			rowBytes = numBlocksWide * numBytesPerBlock;
			numRows = numBlocksHigh;
		}
		else
		{
			UINT bpp = BitsPerPixel( fmt );
			rowBytes = ( uWidth * bpp + 7) / 8; // round up to nearest byte
			numRows = uHeight;
		}
		numBytes = rowBytes * numRows;
		if( pNumBytes != NULL )
			*pNumBytes = numBytes;
		if( pRowBytes != NULL )
			*pRowBytes = rowBytes;
		if( pNumRows != NULL )
			*pNumRows = numRows;
	}


	CDx9DDSCoder::CDx9DDSCoder(CDx9Texture* pText, CRefBufFile* pBufFile, const _DDSCreateParams& ddsParams )
		: CDx9TexCoder( pText, pBufFile )
		, m_stCreateParams(ddsParams)
		, m_uSkipBytes(0)
	{
		//m_TextureName = pText->GetName();
		m_eSpri = SPRI_High;
		AddRef();
	}

	CDx9DDSCoder::~CDx9DDSCoder()
	{
		//m_pTexture->m_IsLoaded = (m_stCreateParams.m_uMipMap == 0);	
	}

	bool CDx9DDSCoder::CopyDataToSurface( const string& strTexName, BYTE *pSuf, UINT uWidth, UINT uHeight, UINT uMipMap, D3DFORMAT fmt, UINT pitch )
	{
		// 取得当前mipmap表面尺寸
		UINT numBytes = 0;
		UINT rowBytes = 0;
		UINT numRows  = 0;
		GetMipMapInfo(uWidth, uHeight, fmt, uMipMap, &numBytes, &rowBytes, &numRows);
		
		if ( pitch < rowBytes )
		{
			stringstream ss;
			ss	<< strTexName << " w:" << uWidth << " h:" << uHeight
				<< " mipmap:" << uMipMap << " fmt:" << fmt << " pitch:" << pitch << " row bytes" << rowBytes;
			GfkLogErr("显存越界，GetMipMapInfo 错误", ss.str());
			return true;
		}

		// 将数据源指针前移至当前mipmap所占空间的起始位置

		m_pBufFile->Seek( m_pBufFile->Tell()- numBytes );
		const char* pBufTmp = m_pBufFile->GetBuf() + m_pBufFile->Tell();
		// copy 数据
		if ( pitch == rowBytes )
		{
			memcpy( pSuf, pBufTmp, numBytes );
		}
		else if ( fmt == D3DFMT_R8G8B8 )
		{
			UINT rowWidth = rowBytes/3;
			for ( UINT j=0; j<numRows; ++j )
			{
				for ( UINT k=0; k<rowWidth; ++k )
				{
					memcpy(&pSuf[k*4], &pBufTmp[k*3], 3);
				}
				pSuf	+= pitch;
				pBufTmp	+= rowBytes;
				SyncCoder_BreakPoint();
			}
		}
		else
		{
			for ( UINT j=0; j<numRows; ++j )
			{
				memcpy( pSuf, pBufTmp, rowBytes ); 
				pSuf	+= pitch;
				pBufTmp	+= rowBytes;
				SyncCoder_BreakPoint();
			}
		}
		return false;
	}

	bool CDx9DDSCoder::AutoDeCode(void)
	{
		if( m_pTexture==NULL ||m_pBufFile==NULL )
			return true;

		if ( m_pTexture->m_pTextures==NULL )
			return true;

		if ( NULL == m_pTexture->m_pTextures[0] )
		{
			if(m_pTexture->GetRefCount() == 0)
			{
				if(m_eSpri != SPRI_Low)
				{
					m_eSpri = SPRI_Low;
					return false;
				}
				m_pTexture->m_pActiveCoder = NULL;
				m_pTexture->Reclaim();
				m_pTexture->m_pActiveCoder = this;
				SetPRI(SPRI_Low);
			}
			else
				m_pTexture->AddRef();

			calculateSkipBytes();
			HRESULT hr = _CreateTexture();
			if( hr ==  E_OUTOFMEMORY || hr == D3DERR_OUTOFVIDEOMEMORY )
			{
				CGraphic::GetInst()->ClearRecycle();
				SQR_TRY
				{
					hr = _CreateTexture();
				}
				SQR_CATCH(exp)
				{
					exp.AppendMsg("CDx9DDSCoder::AutoDeCode");
					GenErr(exp.ErrorTitle(),exp.ErrorMsg());
				}
				SQR_TRY_END;

			}

			if ( FAILED( hr ) )
			{						
				string ErrStr = "_CreateTexture(): ";
				GraphicErrType ErrType = TextureCreate_Err;
				switch ( hr )
				{
				case D3DERR_NOTAVAILABLE:
					ErrStr += "D3DERR_NOTAVAILABLE";
					break;
				case D3DERR_OUTOFVIDEOMEMORY:
					ErrStr += "D3DERR_OUTOFVIDEOMEMORY";
					ErrType = Memory_Err;
					break;
				case D3DERR_INVALIDCALL:
					ErrStr += "D3DERR_INVALIDCALL";
					break;
				case D3DXERR_INVALIDDATA:
					ErrStr += "D3DXERR_INVALIDDATA";
					break;
				case E_OUTOFMEMORY:
					ErrStr += "E_OUTOFMEMORY";
					ErrType = Memory_Err;
					break;
				default:
					ErrStr += "D3DXCreateTextureFromFileInMemoryEx failed with unknown error";
				}
				ErrStr += sqr::format("width: %u height: %u mipmap: %u format: %u ",
					m_stCreateParams.m_uWidth, m_stCreateParams.m_uHeight,
					m_stCreateParams.m_uMipMap, m_stCreateParams.m_eOrgTexFmt);
				
				CError exp(GraphicErr::GetErrTypeStr(ErrType));
				exp.AppendMsg(ErrStr);
				GfkLogPkgErr(exp, m_pTexture->GetName(), *m_pBufFile);
				SafeRelease(m_pTexture->m_pActiveCoder);
				m_pTexture->Release();
				return true;
			}
			checkTextureSize();
			_UpdateParams();
			m_pTexture->Release();
		}
		else
		{
			if(m_stCreateParams.m_uMipMap==0)
			{
				stringstream tmp;
				tmp<<"加载已经完成的纹理"<<m_stCreateParams.m_uOrgMipMap;
				sqr::GfkLogErr(tmp.str().c_str());
				SafeRelease(m_pTexture->m_pActiveCoder);
				return true;
			}
		}
	
		if(m_stCreateParams.m_uMipMap==0)
		{
			stringstream tmp;
			tmp<<"MipMap非法读取终止"<<m_stCreateParams.m_uOrgMipMap;
			sqr::GfkLogErr(tmp.str().c_str());
			SafeRelease(m_pTexture->m_pActiveCoder);
			return true;
		}
		--m_stCreateParams.m_uOrgMipMap;
		--m_stCreateParams.m_uMipMap;

		if(m_pTexture->m_pTextures[0] == NULL || _LoadTextureLod() || m_stCreateParams.m_uMipMap == 0)
		{
			SafeRelease(m_pTexture->m_pActiveCoder);
			return true;
		}

		m_eSpri = m_pTexture->GetRefCount() == 0?SPRI_Low:SPRI_Middle;
		return false;
	}

	void CDx9DDSCoder::calculateSkipBytes()
	{
		uint32 uSkipLevelNum = getSkipLevelNum(m_stCreateParams.m_uOrgWidth, m_stCreateParams.m_uOrgHeight, 
			m_stCreateParams.m_uOrgMipMap, m_stCreateParams.m_eOrgTexFmt
			);
		m_stCreateParams.m_uMipMap -= uSkipLevelNum;
		for ( uint i = 1; i<=uSkipLevelNum; ++i )
		{
			uint32 curLevelBytes = 0;
			GetMipMapInfo(m_stCreateParams.m_uOrgWidth, m_stCreateParams.m_uOrgHeight, 
				m_stCreateParams.m_eOrgTexFmt, m_stCreateParams.m_uOrgMipMap - i, &curLevelBytes, NULL, NULL
				);
			m_uSkipBytes += curLevelBytes;
		}
	}

	CDx9DDSTexture::CDx9DDSTexture(CDx9Texture* pText, CRefBufFile* pBufFile, const _DDSCreateParams& ddsParams  )
		: CDx9DDSCoder( pText,pBufFile, ddsParams )
	{
	}

	HRESULT CDx9DDSTexture::_CreateTexture(void)
	{
		return GetD3DDevice()->CreateTexture(m_stCreateParams.m_uWidth, m_stCreateParams.m_uHeight,  m_stCreateParams.m_uMipMap,0, m_stCreateParams.m_eTexFmt, D3DPOOL_MANAGED,(LPDIRECT3DTEXTURE9*)&(m_pTexture->m_pTextures[0]) , NULL);
	}

	void CDx9DDSTexture::_UpdateParams(void)
	{
		m_pBufFile->Seek(m_pBufFile->GetBufSize() - m_uSkipBytes);
		m_pTexture->SetBufSize( m_stCreateParams.m_uWidth*m_stCreateParams.m_uHeight*BitsPerPixel(m_stCreateParams.m_eTexFmt)/8 );
	}

	void CDx9DDSTexture::checkTextureSize()
	{
		D3DSURFACE_DESC surf_desc;
		memset(&surf_desc, 0, sizeof(surf_desc));
		((LPDIRECT3DTEXTURE9)(m_pTexture->m_pTextures[0]))->GetLevelDesc(0, &surf_desc);
		if ( surf_desc.Width < m_stCreateParams.m_uWidth || surf_desc.Height < m_stCreateParams.m_uHeight || m_stCreateParams.m_uMipMap == 0 )
		{
			m_stCreateParams.m_uWidth = surf_desc.Width;
			m_stCreateParams.m_uHeight = surf_desc.Height;
			m_stCreateParams.m_uMipMap = m_pTexture->m_pTextures[0]->GetLevelCount();
			if(m_stCreateParams.m_uMipMap == 0)
				GfkLogErr("CDx9DDSTexture获取MipMap失败");
		}
	}

	bool CDx9DDSTexture::_LoadTextureLod(void)
	{
		bool ret = false;
		SQR_TRY
		{
			D3DLOCKED_RECT lockedRect;
			LPDIRECT3DTEXTURE9 pTexture = (LPDIRECT3DTEXTURE9)(m_pTexture->m_pTextures[0]);
			
			HRESULT hr = pTexture->LockRect(m_stCreateParams.m_uMipMap, &lockedRect, NULL, 0);
			if ( FAILED(hr) )
			{
				stringstream ss;
				ss << "pTexture->LockRect()" << " - " << DXGetErrorString(hr)
				   << " width: "  << m_stCreateParams.m_uWidth
				   << " height: " << m_stCreateParams.m_uHeight
				   << " mipmap: " << m_stCreateParams.m_uMipMap;
				
				CError exp(GraphicErr::GetErrTypeStr(TextureLock_Err));
				exp.AppendMsg(ss.str());
				GfkLogPkgErr(exp, m_pTexture->GetName(), *m_pBufFile);
				return true;
			}

			ret = CopyDataToSurface( m_pTexture->GetName(), (BYTE*)lockedRect.pBits, m_stCreateParams.m_uWidth, m_stCreateParams.m_uHeight, m_stCreateParams.m_uMipMap, m_stCreateParams.m_eTexFmt, 
				lockedRect.Pitch );

			pTexture->UnlockRect(m_stCreateParams.m_uMipMap);
			pTexture->SetLOD(m_stCreateParams.m_uMipMap);
			m_pTexture->m_IsLoaded = true;
		}
		SQR_CATCH(exp)
		{
			GfkLogPkgErr(exp, m_pTexture->GetName(), *m_pBufFile);
		}
		SQR_TRY_END;

		return ret;
	}

	void CDx9DDSTexture::calculateSkipBytes()
	{
		// no skip
	}

	CDx9DDSVolumeTexture::CDx9DDSVolumeTexture(CDx9Texture* pText, CRefBufFile* pBufFile, const _DDSCreateParams& ddsParams ,UINT Depth )
		: CDx9DDSCoder( pText, pBufFile, ddsParams )
		, m_uDepth(Depth)
	{
	}
	
	HRESULT CDx9DDSVolumeTexture::_CreateTexture(void)
	{
		return GetD3DDevice()->CreateVolumeTexture(m_stCreateParams.m_uWidth, m_stCreateParams.m_uHeight, m_uDepth,  m_stCreateParams.m_uMipMap, 0, m_stCreateParams.m_eTexFmt, D3DPOOL_MANAGED,(LPDIRECT3DVOLUMETEXTURE9*)&(m_pTexture->m_pTextures[0]), NULL );
	}

	void CDx9DDSVolumeTexture::_UpdateParams(void)
	{
		m_pBufFile->Seek(m_pBufFile->GetBufSize()-m_uSkipBytes);
		m_pTexture->SetBufSize( m_stCreateParams.m_uWidth*m_stCreateParams.m_uHeight*BitsPerPixel(m_stCreateParams.m_eTexFmt)/8 );
	}
	
	void CDx9DDSVolumeTexture::checkTextureSize()
	{
		D3DVOLUME_DESC surf_desc;
		memset(&surf_desc, 0, sizeof(surf_desc));
		((LPDIRECT3DVOLUMETEXTURE9)(m_pTexture->m_pTextures[0]))->GetLevelDesc(0, &surf_desc);
		if ( surf_desc.Width < m_stCreateParams.m_uWidth || 
			 surf_desc.Height < m_stCreateParams.m_uHeight || 
			 surf_desc.Depth < m_uDepth || m_stCreateParams.m_uMipMap == 0)
		{
			m_stCreateParams.m_uWidth = surf_desc.Width;
			m_stCreateParams.m_uHeight = surf_desc.Height;
			m_uDepth = surf_desc.Depth;
			m_stCreateParams.m_uMipMap = (m_pTexture->m_pTextures[0])->GetLevelCount();
			if(m_stCreateParams.m_uMipMap == 0)
				GfkLogErr("CDx9DDSVolumeTexture获取MipMap失败");
		}
	}

	bool CDx9DDSVolumeTexture::_LoadTextureLod(void)
	{
		bool ret = false;
		SQR_TRY
		{
			D3DLOCKED_BOX lockedBox;	
			LPDIRECT3DVOLUMETEXTURE9 pTexture = (LPDIRECT3DVOLUMETEXTURE9)(m_pTexture->m_pTextures[0]);
			UINT uCurrDepthCnt = m_uDepth >> m_stCreateParams.m_uMipMap;
			if ( uCurrDepthCnt == 0 )
				uCurrDepthCnt = 1;

			HRESULT hr = pTexture->LockBox(m_stCreateParams.m_uMipMap, &lockedBox, NULL, 0);
			if ( FAILED(hr) )
			{
				stringstream ss;
				ss << "volume texture LockRect()" << " - " << DXGetErrorString(hr)
					<< " width: "  << m_stCreateParams.m_uWidth
					<< " height: " << m_stCreateParams.m_uHeight
					<< " mipmap: " << m_stCreateParams.m_uMipMap;

				CError exp(GraphicErr::GetErrTypeStr(TextureLock_Err));
				exp.AppendMsg(ss.str());
				GfkLogPkgErr(exp, m_pTexture->GetName(), *m_pBufFile);
				return true;
			}

			// 将表面指针指向表面的末尾，用于从后向前加载纹理
			BYTE *pDest = (BYTE*)lockedBox.pBits + lockedBox.SlicePitch*uCurrDepthCnt;

			for ( int sliceIndex = uCurrDepthCnt-1; sliceIndex>=0; --sliceIndex )
			{
				pDest -= lockedBox.SlicePitch;
				if( CopyDataToSurface(m_pTexture->GetName(), pDest, m_stCreateParams.m_uWidth, m_stCreateParams.m_uHeight, m_stCreateParams.m_uMipMap, m_stCreateParams.m_eOrgTexFmt, lockedBox.RowPitch) ) 
					return true;
			}

			pTexture->UnlockBox(m_stCreateParams.m_uMipMap);		
			pTexture->SetLOD(m_stCreateParams.m_uMipMap);
			m_pTexture->m_IsLoaded = true;
		}
		SQR_CATCH(exp)
		{
			GfkLogPkgErr(exp, m_pTexture->GetName(), *m_pBufFile);
		}
		SQR_TRY_END;

		return ret;
	}

	void CDx9DDSVolumeTexture::calculateSkipBytes()
	{
		uint32 uSkipLevelNum = getSkipLevelNum(m_stCreateParams.m_uOrgWidth, m_stCreateParams.m_uOrgHeight, 
			m_stCreateParams.m_uOrgMipMap, m_stCreateParams.m_eOrgTexFmt
			);
		m_stCreateParams.m_uMipMap -= uSkipLevelNum;
		for ( uint i = 1; i<=uSkipLevelNum; ++i )
		{
			uint32 curLevelBytes = 0;
			GetMipMapInfo(m_stCreateParams.m_uOrgWidth, m_stCreateParams.m_uOrgHeight, m_stCreateParams.m_eOrgTexFmt, 
				m_stCreateParams.m_uOrgMipMap - i, &curLevelBytes, NULL, NULL
				);
			uint32 curDepth = max(1, m_uDepth >> (m_stCreateParams.m_uOrgMipMap - i));
			m_uSkipBytes += curLevelBytes * curDepth;
		}
	}

	CDx9DDSCubeTexture::CDx9DDSCubeTexture(CDx9Texture* pText, CRefBufFile* pBufFile, const _DDSCreateParams& ddsParams  )
		: CDx9DDSCoder( pText, pBufFile, ddsParams )
		, m_eCubeFace(ECMF_COUNT)
	{
	}
	
	HRESULT CDx9DDSCubeTexture::_CreateTexture(void)
	{
		return GetD3DDevice()->CreateCubeTexture( m_stCreateParams.m_uWidth,  m_stCreateParams.m_uMipMap, 0, m_stCreateParams.m_eTexFmt, D3DPOOL_MANAGED, (LPDIRECT3DCUBETEXTURE9*)&(m_pTexture->m_pTextures[0]), NULL );
	}

	void CDx9DDSCubeTexture::_UpdateParams(void)
	{
		m_pTexture->SetBufSize( m_stCreateParams.m_uWidth*m_stCreateParams.m_uHeight*BitsPerPixel(m_stCreateParams.m_eTexFmt)*6/8 );
		
		UINT numBytes = 0;
		UINT rowBytes = 0;
		UINT numRows  = 0;
		GetMipMapInfo(m_stCreateParams.m_uOrgWidth,m_stCreateParams.m_uOrgHeight, m_stCreateParams.m_eOrgTexFmt, 0, &numBytes, &rowBytes, &numRows );
		UINT TotalBytes = 0;
		uint32 blockSize = m_stCreateParams.m_eOrgTexFmt == D3DFMT_DXT1 ? 8 : 16;
		for(UINT i = 0; i < m_stCreateParams.m_uOrgMipMap; ++i )
		{
			TotalBytes += numBytes<blockSize?blockSize:numBytes;
			numBytes >>= 2;
		}

		m_sCubeOffset[ECMF_NEGATIVE_Z] = m_pBufFile->GetBufSize() - m_uSkipBytes;
		for(m_eCubeFace = ECMF_NEGATIVE_Z; m_eCubeFace>0; --m_eCubeFace)
			m_sCubeOffset[m_eCubeFace-1] = m_sCubeOffset[m_eCubeFace] - TotalBytes;
	}

	void CDx9DDSCubeTexture::checkTextureSize()
	{
		D3DSURFACE_DESC surf_desc;
		memset(&surf_desc, 0, sizeof(surf_desc));
		((LPDIRECT3DCUBETEXTURE9)(m_pTexture->m_pTextures[0]))->GetLevelDesc(0, &surf_desc);
		if ( surf_desc.Width < m_stCreateParams.m_uWidth || surf_desc.Height < m_stCreateParams.m_uHeight || m_stCreateParams.m_uMipMap == 0)
		{
			m_stCreateParams.m_uWidth = surf_desc.Width;
			m_stCreateParams.m_uHeight = surf_desc.Height;
			m_stCreateParams.m_uMipMap = (m_pTexture->m_pTextures[0])->GetLevelCount();
			if(m_stCreateParams.m_uMipMap == 0)
				GfkLogErr("CDx9DDSCubeTexture获取MipMap失败");
		}
	}

	bool CDx9DDSCubeTexture::_LoadTextureLod(void)
	{
		bool ret = false;
	
		SQR_TRY
		{
			D3DLOCKED_RECT lockedRect;
			LPDIRECT3DCUBETEXTURE9 pCubemapTexture = (LPDIRECT3DCUBETEXTURE9)(m_pTexture->m_pTextures[0]);
			m_pBufFile->Seek(m_sCubeOffset[m_eCubeFace]);
			HRESULT hr = pCubemapTexture->LockRect((D3DCUBEMAP_FACES)m_eCubeFace, m_stCreateParams.m_uMipMap, &lockedRect, NULL, 0);
			if ( FAILED(hr) )
			{
				stringstream ss;
				ss << "pCubemapTexture->LockRect()" << " - " << DXGetErrorString(hr)
					<< " width: "  << m_stCreateParams.m_uWidth
					<< " height: " << m_stCreateParams.m_uHeight
					<< " mipmap: " << m_stCreateParams.m_uMipMap;

				CError exp(GraphicErr::GetErrTypeStr(TextureLock_Err));
				exp.AppendMsg(ss.str());
				GfkLogPkgErr(exp, m_pTexture->GetName(), *m_pBufFile);
				return true;
			}

			ret = CopyDataToSurface( m_pTexture->GetName(), (BYTE*)lockedRect.pBits, m_stCreateParams.m_uWidth, m_stCreateParams.m_uHeight, m_stCreateParams.m_uMipMap, m_stCreateParams.m_eOrgTexFmt, 
				lockedRect.Pitch );

			pCubemapTexture->UnlockRect((D3DCUBEMAP_FACES)m_eCubeFace, m_stCreateParams.m_uMipMap);
			m_sCubeOffset[m_eCubeFace] = m_pBufFile->Tell();

			++m_eCubeFace;
			if( m_eCubeFace!=ECMF_COUNT )
			{
				m_eSpri = SPRI_High;
				++m_stCreateParams.m_uOrgMipMap;
				++m_stCreateParams.m_uMipMap;
			}
			else
			{
				m_eSpri = SPRI_Low;
				m_eCubeFace = ECMF_POSITIVE_X;
				pCubemapTexture->SetLOD(m_stCreateParams.m_uMipMap);
				m_pTexture->m_IsLoaded = true;
			}
		}
		SQR_CATCH(exp)
		{
			GfkLogPkgErr(exp, m_pTexture->GetName(), *m_pBufFile);
		}
		SQR_TRY_END;

		return ret;
	}
}

