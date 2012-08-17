local SetLevelMsg = MsgTbl.SetLevelMsg

local TestConsignmentCase = TestCase("TestConsignment")

local function PrintInfo(...)
	print(...)
end

function TestConsignmentCase:test_Consignment(case_data)
	local UserName = case_data.m_UserName
	
	if not case_data.m_aa then
		case_data.m_aa = 0
	end
	case_data.m_aa = case_data.m_aa + 1
	--PrintInfo(1,UserName,case_data.m_aa)
	StressRpcToServer("GM_Execute", case_data.m_Conn, "$addlevel(80)" )
	WaitForMsg(SetLevelMsg)
	--PrintInfo(2,UserName,case_data.m_aa)
	PrintInfo(UserName,"完全退出副本.............1")
end

return TestConsignmentCase

--
--	{"CSMBuyOrder",    "I"},					--购买订单物品， 参数：订单ID
--	{"CSMSearchBuyOrder",   "IIIsIIiII" },	--查询购买列表，参数：物品类型，物品名称，起始等级，终止等级，排序方式，页数
--	{"CSMSearchSellOrder",   "sIIIIIIiIiib" },	--查询购买列表，参数：物品名称，起始等级，终止等级，是否可用，排序方式，页数，1-物品、2-套装，战神，魔法
--	{"CSMGetOrderByItemType","IIsIIIIIIiIiib"},	--同上，比上边多两个参数：面板，物品类别
--	{"CSMGetOrderByItemAttr","IIsssssIIIIIIiIiib"},--同上，多如右参数：面板，属性列名， parentNodeText，属性列名， 具体属性名
--	{"CSMGetOrderBySomeItemType","IIsIIIIIIiIiib"},--查询购买列表，参数：面板， 某几类物品的代表数字（某一数字代表包含的几类物品）,物品名称，起始等级，终止等级，是否可用，排序方式，页数
--	{"CSMGetOrderByExactItemAttr","IIIsIIIIIIiIiib"},--查询购买列表，参数：面板，物品类型，属性列列名，具体某个属性,物品名称，起始等级，终止等级，是否可用，排序方式，页数
--	{"CSMSearchCharBuyOrder",   "" },		--玩家查询个人购买列表
--	{"CSMSearchCharSellOrder",   "II"},		--玩家查询个人出售类表，参数：排序方式，页数
--	{"CSMAddBuyOrder",   "sIdI" },						--玩家添加收购订单，参数：物品名，数目，价格，时间
--	{"CSMAddSellOrder",   "iIIdIb"},						--玩家添加出售订单，参数：RoomIndex，pos，数目，价格，时间,是否激活记住价格功能（true或false）
--	{"CSMCancelBuyOrder",   "I" },					--玩家取消收购订单，参数：订单ID
--	{"CSMCancelSellOrder",   "I"},					--玩家取消出售订单，参数：订单ID
--	{"CSMTakeAttachment",    "I"},					--玩家提取附件， 参数：订单ID
--	{"CSMSellGoods2Order",       "IiI"},					--支付订单,参数：订单ID，RoomIndex，pos
--	{"CSMFastSearchBuyOrderItem",       ""},					--支付订单,参数：订单ID，RoomIndex，pos
--	{"GetCSMRememberPrice",	"Iss"},					--得到玩家上次出售某种物品的价格，参数:面板no，物品类型，物品名称
--	{"CSMGetOrderBySeveralSortItem", "IIIsIIIIIIiIiib"},
--	{"CSMGetTopPriceBuyOrderByItemName","s"},
--	{"CSMGetAveragePriceByItemName","s"},
--	{"SetMoneyType",       "I"},	  --设置包裹中玩家选中的货币类型