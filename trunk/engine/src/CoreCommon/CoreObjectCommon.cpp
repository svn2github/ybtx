#include "stdafx.h"
#include "IPathFinding.h"
#include "CPos.h"
#include "CoreObjectCommon.h"
#include "CoreCommon.h"


namespace sqr
{
	//******************************************************
	//本地对象的step参数
	//******************************************************

	//客户端对象每多少次step通知一次远程对象进行step
	const uint32 g_uClientTellStepInterval = 6;

	//客户端和服务端在玩家层Aoi的精度(单位：格)
	const uint32 g_uPlayerAoiPrecision = 1;

	//Dir对象通知Med对象step的步长间隔
	const float g_fDirStepMaxSentMovedDist = (float)g_uPlayerAoiPrecision*eGridSpan / 2;
	//Dir对象通知Med对象step的最大时间间隔(毫秒)
	const uint32 g_uDirTellMedStepMaxInterval = 1000;

	//根据速度和移动周期，计算移动一步走多少距离
	float CalMoveStep(float fSpeed,uint32 uMilliseconds)
	{
		float fStep = fSpeed * eGridSpanForObj;
		fStep = fStep * uMilliseconds;
		fStep = fStep / 1000;
		return fStep;
	}
	float CalMoveStepInMicroSeconds(float fSpeed,uint32 uMicroseconds)
	{
		float fStep = fSpeed * eGridSpanForObj;
		fStep = fStep * uMicroseconds;
		fStep = fStep / 1000000;
		return fStep;
	}

	void CalcBarrierArea(vector<CPos>& vecBarrierArea, const CFPos& pixelPosCenter, uint8 uRadius, uint32 uSceneWidth, uint32 uSceneHeight)
	{
		vecBarrierArea.clear();
		if(uRadius > 0)
		{
			uint32 uDist;
			uint32 uMaxX;
			uint32 uMinX;
			uint32 uMaxY;
			uint32 uMinY;
			CPos gridPosCenter;
			if(uRadius % 2 == 0)
			{
				gridPosCenter.x = int32(pixelPosCenter.x / eGridSpanForObj + 0.5f);
				gridPosCenter.y = int32(pixelPosCenter.y / eGridSpanForObj + 0.5f);
				uDist = uRadius / 2;
				uMaxX = gridPosCenter.x + uDist - 1;
				uMinX = uint32(gridPosCenter.x) > uDist ? gridPosCenter.x - uDist : 0;
				uMaxY = gridPosCenter.y + uDist - 1;
				uMinY = uint32(gridPosCenter.y) > uDist ? gridPosCenter.y - uDist : 0;
			}
			else
			{
				gridPosCenter.x = int32(pixelPosCenter.x / eGridSpanForObj);
				gridPosCenter.y = int32(pixelPosCenter.y / eGridSpanForObj);
				uDist = (uRadius - 1) / 2;
				uMaxX = gridPosCenter.x + uDist;
				uMinX = uint32(gridPosCenter.x) > uDist ? gridPosCenter.x - uDist : 0;
				uMaxY = gridPosCenter.y + uDist;
				uMinY = uint32(gridPosCenter.y) > uDist ? gridPosCenter.y - uDist : 0;
			}

			uMaxX = min(uMaxX, uSceneWidth);
			uMaxY = min(uMaxY, uSceneHeight);
			for(uint32 x = uMinX; x <= uMaxX; ++x)
			{
				for(uint32 y = uMinY; y <= uMaxY; ++y)
				{
					vecBarrierArea.push_back(CPos(x, y));
				}
			}
		}
	}
}
