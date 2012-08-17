#include "stdafx.h"
#include "CoreCommon.h"
#include "CPos.h"
#include "CoreCommon.inl"
#include "CQuickRand.h"
#include "GuidTypes.h"

namespace sqr
{
	void PixelPos2GridPos( CPos& GridPos,const CFPos& PixelPos )
	{
		InlPixelPos2GridPos(GridPos, PixelPos);
	} 

	void GridPos2PixelPos( CFPos& PixelPos,const CPos& GridPos )
	{
		InlGridPos2PixelPos(PixelPos, GridPos);
	}

	static const uint8 gs_uSignMod = 51;

	void SlowSignOnData(guid_t guid,const void* pVoidData,size_t stDataSize,uint8 uCounter,uint16& uSeed,uint8& uCheckSum)
	{
		uint16	udbSeed = 0;
		uint8	usbCheckSum;

		const char* pData=reinterpret_cast<const char*>(pVoidData);

		CQuickRand rand;

		do
		{
			rand = ++udbSeed;
			usbCheckSum = uCounter;

			for( size_t i=0 ; i<sizeof(guid_t) ; ++i )
			{
				usbCheckSum = (((usbCheckSum-1) ^ guid[i]) + 1) ^ rand;
			}

			for( size_t i=0 ; i<stDataSize ; ++i )
			{
				usbCheckSum = (((usbCheckSum-1) ^ pData[i]) + 1) ^ rand;
			}
		}
		while( usbCheckSum % gs_uSignMod != 0 );

		uSeed = udbSeed;
		uCheckSum = usbCheckSum;
	}

	bool FastVerifyData(guid_t guid,const void* pVoidData,size_t stDataSize,uint8 uCounter,uint16 uSeed,uint8 uCheckSum)
	{
		//cout<< uSeed<<","<<uint32(uCheckSum)<<endl;

		const char* pData=reinterpret_cast<const char*>(pVoidData);

		if( uCheckSum % gs_uSignMod != 0 )
			return false;

		CQuickRand rand(uSeed);

		uint8 usbCheckSum=uCounter;

		for( size_t i=0 ; i<sizeof(guid_t) ; ++i )
		{
			usbCheckSum = (((usbCheckSum-1) ^ guid[i]) + 1) ^ rand;
		}

		for( size_t i=0 ; i<stDataSize ; ++i )
		{
			usbCheckSum = (((usbCheckSum-1) ^ pData[i]) + 1) ^ rand;
		}

		return usbCheckSum == uCheckSum;
	}

	// 计算坐标Pixel点位于1/4 grid的具体格
	CPos GetQuarGridPos(const CFPos& PixelPos)
	{
		uint32 PosX = static_cast<uint32>(PixelPos.x) % 64;
		uint32 PosY = static_cast<uint32>(PixelPos.y) % 64;

		CPos CurPos;
		CPos QuarPos;
		CurPos.x = GetGridByPixel(PixelPos.x);
		CurPos.y = GetGridByPixel(PixelPos.y);	
		if ( PosX >= 32 )
		{
			if ( PosY >= 32 )
			{
				QuarPos.x = CurPos.x * 2 + 1;
				QuarPos.y = CurPos.y * 2 + 1;
			}
			else
			{
				QuarPos.x = CurPos.x * 2 + 1;
				QuarPos.y = CurPos.y * 2;
			}
		}
		else
		{
			if ( PosY >= 32 )
			{
				QuarPos.x = CurPos.x * 2;
				QuarPos.y = CurPos.y * 2 + 1;
			}
			else
			{
				QuarPos.x = CurPos.x * 2;
				QuarPos.y = CurPos.y * 2;
			}
		}

		return QuarPos;
	}

}

