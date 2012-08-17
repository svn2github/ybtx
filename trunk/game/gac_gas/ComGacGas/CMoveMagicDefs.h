#pragma once
#include "CPos.h"
#include "FindPathDefs.h"
#include "CFighterMallocObject.h"

// 移动方式类型
enum EMoveMagicType
{
	eMMT_Move,				// 平移
	eMMT_Direction,			// 方向
	eMMT_Random,			// 随机
	eMMT_AreaMove,			// 范围平移
	eMMT_AreaDirection,		// 范围方向
	eMMT_AreaRandom,		// 范围随机
};

enum EMoveMagicArg
{
	eMMA_None,				// 无
	eMMA_SelfPosition,		// 自身位置
	eMMA_TargetPosition,	// 目标位置
	eMMA_Direction,
	eMMA_OppositeDirection,
	eMMA_Position,			// 地点
	eMMA_Area,				// 范围
	eMMA_Distance,			// 距离
};

enum EMoveMagicArgLimit
{
	eMMAL_None,				// 无
	eMMAL_DistanceLimit,
	eMMAL_AreaLimit,
	eMMAL_PositionLimit,
	eMMAL_DistanceLimitRandom,
	eMMAL_AreaLimitRandom,
	eMMAL_PositonLimitRandom,
	eMMAL_Offset,
};

enum EMoveActionType
{
	eMAT_None,
	eMAT_HitFly,
	eMAT_HitAway,
	eMAT_Jump,
	eMAT_Eddy,
};

enum EMoveType
{
	eMT_Move,			// 平移
	eMT_Direction,		// 方向
	eMT_Random,			// 随机
	eMT_Teleport,		// 传送
};

struct MoveInfo : public CFighterMallocObject
{
	EMoveType eMoveType;
	EBarrierType eBarrierType;
	EMoveActionType	eAction;
	CFPos DesPixelPos;
	float fSpeed;
	float fDistance;
	uint8 uDir;
};
