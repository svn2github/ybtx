#pragma once

#include "TCoreSceneAllocator.h"

namespace sqr
{
	//寻路3种模式，1：直线（碰到障碍返回NULL），2：a*，3：直线（碰到障碍后返回前一个点坐标）
	//             4：直线忽略目标障碍          5：a* 忽略目标障碍

	//*********严重注意，目前寻路类型最多支持8种，如果要增加超过8种，需要修改网络协议中的usbFindPathType的位数************
	enum EFindPathType
	{
		eFPT_Line = 0,       // 直线寻路
		eFPT_HypoLine,       // 直线寻路 -> 返回直线第一个障碍点寻路
		eFPT_LineIgnoreEnd,  // 直线寻路忽略终点为障碍

		eFPT_AStar,          // 直线寻路 -> A*寻路
		eFPT_HypoAStar,      // 直线寻路 -> A*寻路 -> 返回直线第一个障碍点寻路(如果终点是障碍点直接用Hypoline寻路)
		eFPT_TerrainAStar,   // 考虑地形影响的A*寻路 -> 返回直线第一个障碍点寻路(如果终点是障碍点直接用Hypoline寻路)
		eFPT_AStarIgnoreEnd, // A*寻路忽略终点为障碍
		eFPT_HypoAStarOpti,   // Optimization Hypo AStar，当目标点无法到达时，选择终点的最近点作为寻路终点

		eFPT_AStarUseWeight,	//考虑权值的 A*寻路
		eFPT_HypoAStarOptiUseWeight,	//考虑权值的 eFPT_HypoAStarOpti寻路, 且直接使用A* 不预先使用直线寻路(直线寻路无法考虑权值, 当直线能通时不一定是预想的结果)

		eFPT_Custom, //自定义路点
	};

	//*********严重注意，目前障碍类型最多支持64种，如果要增加超过64种，需要修改网络协议中的usbBarrierType的位数************
	enum EBarrierType
	{
		eBT_OutRange = -1,
		eBT_NoBarrier = 0,
		eBT_LowBarrier = 1,
		eBT_MidBarrier = 2,
		eBT_HighBarrier = 3,
	};

	class IPath;
	class IMap;

}
