#pragma once
#include "CMoveMagicDefs.h"
#include "TConfigAllocator.h"
#include "TSqrAllocator.inl"

class CMoveMagicCfg : public CConfigMallocObject
{
public:
	typedef map<string, EMoveMagicArgLimit, less<string>, 
		TConfigAllocator<pair<string, EMoveMagicArgLimit > > >	MapMoveArgLimit;
	typedef map<string,	EMoveMagicArg, less<string>, 
		TConfigAllocator<pair<string, EMoveMagicArg > > >			MapMoveArg;
	typedef map<string, EMoveMagicType, less<string>, 
		TConfigAllocator<pair<string, EMoveMagicType > > >		MapMoveType;
	typedef map<string, EBarrierType, less<string>, 
		TConfigAllocator<pair<string, EBarrierType > > >			MapBarrierType;
	typedef map<string, EMoveActionType, less<string>, 
		TConfigAllocator<pair<string, EMoveActionType > > >		MapActionType;

	static MapMoveArgLimit						ms_mapMoveArgLimit;
	static MapMoveArg							ms_mapMoveArg;
	static MapMoveType							ms_mapMoveType;
	static MapBarrierType						ms_mapBarrierType;
	static MapActionType						ms_mapActionType;
	
	inline static bool							InitMapMoveArgLimit()
	{
		ms_mapMoveArgLimit[""]				= eMMAL_None;
		ms_mapMoveArgLimit["距离限制"]		= eMMAL_DistanceLimit;
		ms_mapMoveArgLimit["范围限制"]		= eMMAL_AreaLimit;
		ms_mapMoveArgLimit["地点限制"]		= eMMAL_PositionLimit;
		ms_mapMoveArgLimit["距离限制随机"]	= eMMAL_DistanceLimitRandom;
		ms_mapMoveArgLimit["范围限制随机"]	= eMMAL_AreaLimitRandom;
		ms_mapMoveArgLimit["地点限制随机"]	= eMMAL_PositonLimitRandom;
		ms_mapMoveArgLimit["偏移"]			= eMMAL_Offset;
		return true;
	}

	inline static bool								InitMapMoveArg()
	{
		ms_mapMoveArg[""]			= eMMA_None;
		ms_mapMoveArg["自身位置"]	= eMMA_SelfPosition;
		ms_mapMoveArg["目标位置"]	= eMMA_TargetPosition;
		ms_mapMoveArg["正向朝向"]	= eMMA_Direction;
		ms_mapMoveArg["反向朝向"]	= eMMA_OppositeDirection;
		ms_mapMoveArg["地点"]		= eMMA_Position;
		ms_mapMoveArg["范围"]		= eMMA_Area;
		return true;
	}

	inline static bool								InitMapMoveType()
	{
		ms_mapMoveType["移动"]	= eMMT_Move;
		ms_mapMoveType["方向"]	= eMMT_Direction;
		ms_mapMoveType["随机"]	= eMMT_Random;
		ms_mapMoveType["范围移动"]	= eMMT_AreaMove;
		ms_mapMoveType["范围方向"]	= eMMT_AreaDirection;
		ms_mapMoveType["范围随机"]	= eMMT_AreaRandom;
		return true;
	}

	inline static bool								InitMapBarrierType()
	{
		ms_mapBarrierType["低障"]	= eBT_LowBarrier;
		ms_mapBarrierType["中障"]	= eBT_MidBarrier;
		return true;
	}

	inline static bool								InitMapActionType()
	{
		ms_mapActionType[""] = eMAT_None;
		ms_mapActionType["击飞"] = eMAT_HitFly;
		ms_mapActionType["击退"] = eMAT_HitAway;
		ms_mapActionType["跳跃"] = eMAT_Jump;
		ms_mapActionType["旋转"] = eMAT_Eddy;
		return true;
	}
};

