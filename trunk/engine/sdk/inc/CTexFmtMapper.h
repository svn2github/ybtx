#pragma once
#include "TSingleton.h"
#include "GraphicBase.h"

namespace sqr
{
	class CTexFmtMapper
		: public Singleton<CTexFmtMapper>
	{
	public:
		virtual ~CTexFmtMapper(){}
		virtual uint32
				GetBitsPerPixel( eTextureFormat fmt ) = 0;
	};

	typedef vector<uint8>	TexBuffer;

	bool	CopyMemToSurface( uint8* pSuf, const TexBuffer& srcBuf, uint32 uOrgWidth, uint32 uOrgHeight, 
				uint32 level, eTextureFormat fmt, uint32 uSufPitch );

	bool	CopySurfaceToMem( TexBuffer& pMemBuf, uint8* pSuf, uint32 uOrgWidth, uint32 uOrgHeight, 
				uint32 level, eTextureFormat fmt, uint32 uSufPitch );

	void	GetLevelInfo( uint32 uOrgWidth, uint32 uOrgHeight, eTextureFormat fmt, uint32 level,
				uint32* pNumBytes, uint32* pRowBytes, uint32* pNumRows );
}