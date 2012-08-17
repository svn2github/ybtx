engine_require "common/Misc/TypeCheck"
gac_gas_require "framework/distance/Distance"

EAssignMode =
{
	--eAM_TurnPickup		= 0, --轮流拾取
	eAM_FreePickup			= 0, --自由拾取
	eAM_AverageAssign		= 1, --平均分配
	eAM_RespectivePickup	= 2, --各自拾取
	eAM_AuctionAssign		= 3, --拍卖模式
	eAM_NeedAssign			= 4, --需求分配
}
AddCheckLeakFilterObj(EAssignMode)

EAuctionStandard =
{
	eAS_GrayStandard   = 0, --灰色
	eAS_WhiteStandard  = 1, --白色
	eAS_GreenStandard  = 2, --绿色
	eAS_BlueStandard   = 3, --蓝色
	eAS_PurpleStandard = 4, --紫色
	eAS_OrangeStandard = 5, --橙色
	eAS_YellowStandard = 6, --黄色
	eAS_CyanStandard   = 7, --青色
--	eAS_BlackStandard  = 2, --黑色
--	eAS_RedStandard    = 3, --红色
}
AddCheckLeakFilterObj(EAuctionStandard)

-------------------------------------------------------------------------------
