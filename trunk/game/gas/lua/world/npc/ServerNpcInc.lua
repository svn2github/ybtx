gas_require "framework/main_frame/ItemHolderInc"

CNpcServerMgr = class()

-- NPC 是否重生种类
ENpcIsReborn = 
{
	eNT_NoReborn 	= 0,	-- 用于重生管理器不需要管理的特殊NPC
	eNT_Reborn 			= 1,	-- 普通
}

ENpcColonyType = 
{
	eNCT_Null				= 0,	--打酱油的，啥都不是
	eNCT_Boss				= 1,	--怪物群中的老大
	eNCT_Servant		= 2,	--怪物群中的小弟
}

MapMoveType2Name = 
{
	[ENpcMoveType.ENpcMove_None] 				= "固定",
	[ENpcMoveType.ENpcMove_Cycle]				= "循环巡逻",
	[ENpcMoveType.ENpcMove_Trip]				= "往返巡逻",
	[ENpcMoveType.ENpcMove_Random]				= "随即移动",
	[ENpcMoveType.ENpcMove_SingleLine]			= "单线巡逻",
	[ENpcMoveType.ENpcMove_Single2Random]		= "先单线后随机",
	[ENpcMoveType.ENpcMove_MoveInPath]			= "组合路径巡逻",
}

