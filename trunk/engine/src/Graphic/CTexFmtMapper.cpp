#include "stdafx.h"
#include "CTexFmtMapper.h"

namespace sqr
{
	void GetLevelInfo( uint32 uOrgWidth, uint32 uOrgHeight, eTextureFormat fmt, uint32 level,
		uint32* pNumBytes, uint32* pRowBytes, uint32* pNumRows )
	{
		uint32 numBytes = 0;
		uint32 rowBytes = 0;
		uint32 numRows  = 0;

		uOrgWidth  >>= level;
		uOrgHeight >>= level;
		if( uOrgWidth == 0 )
			uOrgWidth = 1;
		if( uOrgHeight == 0 )
			uOrgHeight = 1;

		if( fmt == TF_DXT1 || fmt == TF_DXT5 )
		{
			int numBlocksWide = 0;
			numBlocksWide = max( 1, uOrgWidth / 4 );
			int numBlocksHigh = 0;
			numBlocksHigh = max( 1, uOrgHeight / 4 );
			int numBytesPerBlock = ( fmt == TF_DXT1 ? 8 : 16 );
			rowBytes = numBlocksWide * numBytesPerBlock;
			numRows = numBlocksHigh;
		}
		else
		{
			uint32 bpp = CTexFmtMapper::GetInst()->GetBitsPerPixel( fmt );
			rowBytes = ( uOrgWidth * bpp + 7) / 8; // round up to nearest byte
			numRows = uOrgHeight;
		}
		numBytes = rowBytes * numRows;
		if( pNumBytes != NULL )
			*pNumBytes = numBytes;
		if( pRowBytes != NULL )
			*pRowBytes = rowBytes;
		if( pNumRows != NULL )
			*pNumRows = numRows;
	}

	bool CopyMemToSurface( uint8* pSuf, const TexBuffer& srcBuf, uint32 uOrgWidth, 
		uint32 uOrgHeight, uint32 level, eTextureFormat fmt, uint32 uSufPitch )
	{
		// 取得当前mipmap表面尺寸
		uint32 numBytes = 0;
		uint32 rowBytes = 0;
		uint32 numRows  = 0;
		GetLevelInfo(uOrgWidth, uOrgHeight, fmt, level, &numBytes, &rowBytes, &numRows);

		const uint8* pBufTmp = &srcBuf[0];
		// copy 数据
		if ( uSufPitch == rowBytes )
		{
			memcpy( pSuf, pBufTmp, numBytes );
		}
		else if ( fmt == TF_RGB24 )
		{
			uint32 rowWidth = rowBytes/3;
			for ( uint32 j=0; j<numRows; ++j )
			{
				for ( uint32 k=0; k<rowWidth; ++k )
				{
					memcpy(&pSuf[k*4], &pBufTmp[k*3], 3);
				}
				pSuf	+= uSufPitch;
				pBufTmp	+= rowBytes;
			}
		}
		else
		{
			for ( uint32 j=0; j<numRows; ++j )
			{
				memcpy( pSuf, pBufTmp, rowBytes ); 
				pSuf	+= uSufPitch;
				pBufTmp	+= rowBytes;
			}
		}
		return false;
	}

	bool CopySurfaceToMem( TexBuffer& pMemBuf, uint8* pSuf, uint32 uOrgWidth, uint32 uOrgHeight, 
		uint32 level, eTextureFormat fmt, uint32 uSufPitch )
	{
		// 取得当前mipmap表面尺寸
		uint32 numBytes = 0;
		uint32 rowBytes = 0;
		uint32 numRows  = 0;
		GetLevelInfo(uOrgWidth, uOrgHeight, fmt, level, &numBytes, &rowBytes, &numRows);

		pMemBuf.resize(numBytes);
		uint8* pMemDest = &pMemBuf[0];
		// copy 数据
		if ( uSufPitch == rowBytes )
		{
			memcpy(pMemDest, pSuf, numBytes);
		}
		else if ( fmt == TF_RGB24 )
		{
			uint32 rowWidth = rowBytes/3;
			for ( uint32 j=0; j<numRows; ++j )
			{
				for ( uint32 k=0; k<rowWidth; ++k )
				{
					memcpy(&pMemDest[k*3], &pSuf[k*4], 3);
				}
				pSuf	 += uSufPitch;
				pMemDest += rowBytes;
			}
		}
		else
		{
			for ( uint32 j=0; j<numRows; ++j )
			{
				memcpy(pMemDest, pSuf, rowBytes); 
				pSuf	 += uSufPitch;
				pMemDest += rowBytes;
			}
		}
		return false;
	}
}
