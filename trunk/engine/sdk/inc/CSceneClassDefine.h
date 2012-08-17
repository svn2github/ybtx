#pragma once
#include "CSceneBase.h"

namespace sqr
{
	struct CWaterWaveInfo : public CSceneMallocObject
	{
		struct WaterPhysic
		{
			int32	nWavePerGrid;			// 每个格子的波浪个数
			int32	nWaveSize;				// 波浪大小
			int32	nWaveOff;				// 波浪偏移
			float	fWaveSpeed;				// 波浪速度
			int32	nWaveLife;				// 波浪生命周期
		};
		WaterPhysic	WavePhysicInfo;
		SString		szWaveTexture;			// 波浪贴图
		SString		szMoveFxFile;			// 移动波浪特效文件
		SString		szMoveFxName;			// 移动波浪特效名
		SString		szStandFxFile;			// 移动波浪特效文件
		SString		szStandFxName;			// 移动波浪特效名
	};

	class CBufFile;
	class CTreeScene;
	class CElementCoder;
	class CElementManager;
	class CBaseScene;
	class CRegion;
}