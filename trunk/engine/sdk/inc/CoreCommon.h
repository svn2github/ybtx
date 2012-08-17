#pragma once
#include "CSceneCommon.h"
#include "GuidTypes.h"

namespace sqr
{
	// |=== Quarter ===========================================================
	inline float GetFloatPixelOfQuarGrid( int32 LogicCrd, float Offset = 16.0f )
	{
		return static_cast<float>( LogicCrd << 5 ) + Offset;
	}

	inline float GetPixelOfQuarGrid( int32 LogicCrd, float Offset = 16.0f )
	{
		return static_cast<float>( LogicCrd << 5 ) + Offset;
	}

	inline int32 GetQuarGridByPixel( float fPixelCrd )
	{
		return static_cast<int32>(fPixelCrd) >> 5;// return LogicCrd*128 + Offset;
	}

	inline float GetOfsInQuarGrid( float fPixelCrd )
	{
		float fOffset = fPixelCrd - static_cast<int32>(fPixelCrd);
		return (static_cast<int32>(fPixelCrd) & 0x1f) + fOffset;// return LogicCrd*64 + Offset;
	}

	//根据格子坐标和偏移得到象素坐标
	inline float GetPixelOfGrid( int32 LogicCrd, float Offset = 32.0f )
	{
		return static_cast<float>( LogicCrd << 6 ) + Offset;// return LogicCrd*128 + Offset;
	}

	void PixelPos2GridPos( CPos& GridPos,const CFPos& PixelPos );
	void GridPos2PixelPos( CFPos& PixelPos,const CPos& GridPos );
	void SlowSignOnData(guid_t guid,const void* pData,size_t stDataSize,uint8 uCounter,uint16& uSeed,uint8& uCheckSum);
	bool FastVerifyData(guid_t guid,const void* pData,size_t stDataSize,uint8 uCounter,uint16 uSeed,uint8 uCheckSum);
	CPos GetQuarGridPos(const CFPos& PixelPos);

}


