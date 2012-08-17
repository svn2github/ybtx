#include "stdafx.h"
#include "QuadTreeFun.h"

namespace sqr
{
	CFRect GetQuadRect(uint32 uQuadWidth, uint32 uQuadHeight)
	{	
		uint32 uWidth = uQuadWidth;
		uint32 uHeight = uQuadHeight;

		uint32 uLength = (uWidth > uHeight) ? uWidth : uHeight;

		uint32 uSide;
		do 
		{
			uSide = uLength;
			uLength &= uLength - 1;
		} while(uLength);

		uSide = uSide << 1;

		int32 iSide = (int)uSide;

		//场景大小为2的n次倍数
		CFRect rect;
		rect.left = (float)(-iSide / 2);
		rect.right = (float)(iSide / 2 + iSide );
		rect.top = (float)(iSide / 2 + iSide);
		rect.bottom = (float)(-iSide / 2);

		return rect;
	}
}

