lan_load "npc/Lan_NotFunNpcTypeMap"
CNpcDialogBox             = class( SQRDialog )
CNpcDialogRichTextItem    = class( SQRDialog )
CNpcDialogBoxListCtrlItem = class( SQRDialog )
CNpcDialogBoxListCtrlItemListItem = ( SQRDialog )
CNpcFuncListCtrlItem      = class( SQRDialog )

CNpcDialogBox.DlgTextType = 
{
	["接取任务"]				= 0,  --["任务"]
	["提交任务"]				= 1,  --["任务"]
	["商店"]					= 2,
	["寄售交易所"]				= 3,
	["商会创建界面"]			= 4,
	["商会主界面"]				= 5,
	["股票"]					= 6,
	["佣兵小队创建处"]			= 7,
	["佣兵团大本营交互界面"]	= 8,
	["角斗场报名处"]			= 9,  --玩法
	["竞技场报名处"]			= 10, --玩法
	["地下城传送处"]			= 11,
	--["区域副本任务发布处"]		= 12,
	["日常活动报名处"]			= 13,
	["跳跳大夺宝报名处"]		= 14,
	["剧情探险报名处"]			= 15,
	["战区传送"]				= 16,
	["答题"]					= 17, --玩法
	["进入帮会"]				= 18, --帮会
	["喝酒小游戏报名处"]		= 19, --喝酒
	["修行塔报名处"]			= 20, --玩法
	["仓库"]					= 21,
	["佣兵团引荐处"]			= 22,
	["佣兵小队申请处"]			= 23,
	["佣兵团战争管理处"]		= 24,
	["建造建筑"]				= 25,
	["捐献金钱"]				= 26,
	["打开佣兵团仓库"]			= 27,
	["交割粮草"]				= 28,
	["更新等级"]				= 29,
	["拆除"]					= 30,
	["进行制造"]				= 31,
	["科技研发"]				= 32,
	["科技学习"]				= 33,
	["申请撤退"]				= 36,
	["占领此驻地"]				= 42,
	["结盟"]					= 43,
	["佣兵团挑战进场处"] 		= 44,
	["佣兵小队更名处"]			= 45,
	--["资源收购查询处"]			= 46,
	["资源领取处"]				= 47,
	["帮会建筑领取处"] 			= 48,
	["帮会挑战传送处"] 			= 49,
	["锻造"]					= 50,
	["铸甲"]					= 51,
	["烹饪"]					= 52,
	["制药"]					= 53,
	["采矿"]					= 54,
	["花卉"]					= 55,
	["技能升级学习Npc"]			= 56,
	["技能转职Npc1"]			= 57,
	["技能转职Npc2"]			= 58,
	["技能转职Npc3"]			= 59,
	["技能转职Npc4"]			= 60,
	["技能转职Npc5"]			= 61,
	["技能转职Npc6"]			= 62,
	["帮会驻地传送处"] 			= 63,
	["帮会资源运输处"] 			= 64,
	["佣兵团刷怪本传送处"]		= 65,
	["修理匠师"]				= 66,
	["挑战介绍"]				= 67,
	["国战介绍"]				= 68,
	["资源领取介绍"]			= 69,
	["领域传送"]				= 70,
	["工艺"]					= 71,
	["活动礼品领取"]	= 72,
	
	["佣兵小队提升"]				= 80,
	["佣兵团创建处"]				= 81,
	["佣兵小队类型转变"]		= 82,
	["佣兵小队类型重置"]		= 83,
	["积分赛面板测试"]			= 100,
	
	["佣兵指引登记"]			= 101,
	["佣兵指引定级"]			= 102,
	["佣兵指引领特权"]			= 103,
--	["怪物攻城介绍"]			= 104,
--	["抢夺资源点介绍"]			= 105,
--	["特殊NPC"] = 106,
	
	["地下城报名处"]			= 107,
	["多人活动报名处"]			= 108,
	["佣兵任务报名处"]			= 109,
	["上交军资勋章"]			= 110,
	["Boss争夺战报名"]			= 111,
	["积分商店"]				= 112,
	["交割帮会粮草"]			= 114,
	["生活技能代工所"]			= 113,
	["发布征粮令"]				= 115,
	["喝酒射箭活动报名处"]		=116,
	["装备精炼"]                =   117,
	["装备追加"]                = 118,
	["流通商店"]                = 119,
	["地下城首领"]                = 120,
	["求购行"]							= 121,
	["快速出售"]						= 122,
	["佣兵团更名处"]               = 123,
	["篝火"] = 124,
	["资金捐献(经验)"] = 125,
	["资金捐献(魂值)"] = 126,
	["资金捐献(军资)"] = 127,
	["国贡领取介绍"] = 128,
	["击杀积分介绍"] = 129,
	["红龙之穴"]	   = 130,
	["佣兵队员招募处"] = 131
}

--功能NPC 在Npc功能面板上显示的文字
--ENpcShowDlg = {}
CNpcDialogBox.ENpcShowDlg = Lan_NotFunNpcTypeMap

function CNpcDialogBox:InitFunLink()
	self.linkfun = {}
	self.linkfun[0] = self.AcceptQuestFun
	self.linkfun[1] = self.FinishQuestFun
	self.linkfun[2] = self.ShopSellFun
	self.linkfun[3] = self.SellOrderFun
	self.linkfun[4] = self.CreatCofCFun
	self.linkfun[5] = self.CofCMainFun
	self.linkfun[6] = self.StockMainFun
	self.linkfun[7] = self.TongCreateFun
	self.linkfun[8] = self.TongMainFun
	self.linkfun[9] = self.DaTaoShaFun
	self.linkfun[10] = self.JiFenSaiFun
	self.linkfun[11] = self.AreaFbSelFun
--	self.linkfun[11] = self.MercenaryGroupFbFun
	-- libing self.linkfun[12] = self.AreaFbQuestFun
	self.linkfun[13] = self.MatchGameSelFun
	self.linkfun[14] = self.DaDuoBaoFun
	self.linkfun[15] = self.JuQingFun
	self.linkfun[16] = self.WarZoneSelFun
	self.linkfun[17] = self.StartQuestionFun
	self.linkfun[18] = self.EnterTongAreaFun
	self.linkfun[19] = self.DrinkFun
	self.linkfun[20] = self.MercenaryEducateActionFun
	self.linkfun[21] = self.DepotFun
	self.linkfun[22] = self.TongRecommendFun
	self.linkfun[23] = self.TongRequestFun
	self.linkfun[24] = self.TongChallengeFun
	self.linkfun[25] = self.CreateBuildingFun
	self.linkfun[26] = self.ContributeMoneyFun
	self.linkfun[27] = self.OpenMercenaryDepotFun
	self.linkfun[28] = self.PromptForageRandomFun
	self.linkfun[29] = self.UpdateOrderFun
	self.linkfun[30] = self.BackoutFun
	self.linkfun[31] = self.MakingFun
	self.linkfun[32] = self.TongOpenScienceFun
	self.linkfun[33] = self.TongLearnScienceFun
	self.linkfun[36] = self.ApplyRetreatFun
	self.linkfun[42] = self.TakeStationFun
	self.linkfun[43] = self.TongAlignmentFun
	self.linkfun[44] = self.EnterTongChallengeAreaFun
	self.linkfun[45] = self.TongChangeNameFun
	--self.linkfun[46] = self.TongViewResOrderFun
	self.linkfun[47] = self.TongFetchResFun
	self.linkfun[48] = self.TongFetchBuildingFun
	self.linkfun[49] = self.ExitTongChallengeFun
	self.linkfun[50] = self.LiveSkillSmithingNPCFun
	self.linkfun[51] = self.LiveSkillCastingNPCFun
	self.linkfun[52] = self.LiveSkillCuisineNPCFun
	self.linkfun[53] = self.LiveSkillPharmacyNPCFun
	self.linkfun[54] = self.LiveSkillCollectOreNPCFun
	self.linkfun[55] = self.LiveSkillFlowerNPCFun
	self.linkfun[56] = self.LearnCommonSkillNpcFun
	self.linkfun[57] = self.LearnSkillNpc1Fun
	self.linkfun[58] = self.LearnSkillNpc2Fun
	self.linkfun[59] = self.LearnSkillNpc3Fun
	self.linkfun[60] = self.LearnSkillNpc4Fun
	self.linkfun[61] = self.LearnSkillNpc5Fun	
	self.linkfun[62] = self.LearnSkillNpc6Fun	
	self.linkfun[63] = self.LeaveTongAreaFun
	self.linkfun[64] = self.TongResTransFun
	self.linkfun[65] = self.MercenaryMonsterFbFun
	self.linkfun[66] = self.OpenFixEquipWndFun
	self.linkfun[67] = self.ChallengeIntroduceFun
	self.linkfun[68] = self.NationIntroduceFun
	self.linkfun[69] = self.RescourseIntroduceFun
	self.linkfun[70] = self.LingYuTransmitFun
	self.linkfun[71] = self.LiveSkillCraftNPCFun
	self.linkfun[72] = self.GMCompenateFun
	self.linkfun[80] = self.CanUpTongTypeFun
	self.linkfun[81] = self.CreateArmyCorpsFun
	self.linkfun[82] = self.CanChangeTongTypeFun
	self.linkfun[83] = self.ResetTongTypeFun
	self.linkfun[100] = self.JiFenSaiInfoFun
	self.linkfun[101] = self.BeComeYbFun
	self.linkfun[102] = self.DingJiYbFun
	self.linkfun[103] = self.ReceivePowerYbFun
--	self.linkfun[104] = self.MonsterAttackIntroduceFun
--	self.linkfun[106] = self.SpecialNpcFun
	
	self.linkfun[107] = self.UndergroudTeamList
	self.linkfun[108] = self.ActionTeamList
	self.linkfun[109] = self.TaskTeamList
	self.linkfun[110] = self.HandForage
	self.linkfun[111] = self.BossBattleFun
	self.linkfun[112] = self.ShopSellFun
	self.linkfun[113] = self.OpenContractManuMarketWnd
	self.linkfun[114] = self.PromptForageRandomFun
	self.linkfun[115] = self.IssueFetchFun
	self.linkfun[116] = self.DrinkShootFun
	self.linkfun[117] = self.OpenEquipRefine
	self.linkfun[118] = self.OpenEquipSuperaddWnd
	self.linkfun[119] = self.ShopSellFun
	self.linkfun[120] = self.AreaFbFun
	self.linkfun[121] = self.BuyOrderFun
	self.linkfun[122] = self.QuickSell
	self.linkfun[123] = self.ArmyCorpsChangeNameFun
	self.linkfun[124] = self.OpenNeedFireActivity
	self.linkfun[125] = self.OpenTongContributeMoneyExp
	self.linkfun[126] = self.OpenTongContributeMoneySoul
	self.linkfun[127] = self.OpenTongContributeMoneyResource
	self.linkfun[128] = self.GivePointIntroduceFun
	self.linkfun[129] = self.PkPointIntroduceFun
	self.linkfun[130] = self.DragonCaveFun
	self.linkfun[131] = self.TongRecruitFun
	
end

function CNpcDialogBox:RunLinkFun(Index, uParam1)
	assert(self.linkfun[Index],"Index["..Index.."]不存在")
	if g_MainPlayer.m_LockCenter ~= nil and g_MainPlayer.m_LockCenter.m_LockingObj ~= nil then
 		Gac2Gas:SetLockingNPCID(g_Conn,g_MainPlayer.m_LockCenter.m_LockingObj:GetEntityID())
  end
	self.linkfun[Index](self, uParam1)
end	
	
function CNpcDialogBox:AcceptQuestFun(uParam1)
	local questname= self.QuestNameList[uParam1+1]
--	local GlobalID = g_QuestGiverGlobalID[questname].GiverGlobalID
--	Gac2Gas:ShowTakeQuestWnd(g_Conn, questname, GlobalID)
--	print("AcceptQuestFun " .. (self.m_TargetEntityID or "空") )
	g_GameMain.m_takeTask:ShowTakeQuestWnd(questname,self.m_TargetEntityID)
end

function CNpcDialogBox:FinishQuestFun(uParam1)
	local questname=self.QuestNameList[uParam1+1]
	if g_AllMercenaryQuestMgr[questname] then
		local GlobalID = g_QuestFinishGlobalID[questname].FinishGlobalID
		Gac2Gas:ShowFinishQuestWnd(g_Conn, questname, GlobalID)
	else
		g_GameMain.m_finishTask:ShowFinishQuestWnd(questname,self.m_TargetEntityID)
	end
end

function CNpcDialogBox:ShopSellFun(uParam1)

	g_GameMain.m_NPCShopSell:NPCShopSellWnd(self.m_NpcNameText)

end

function CNpcDialogBox:AreaFbFun(uParam1)

end


function CNpcDialogBox:SellOrderFun(uParam1)
	g_GameMain.m_CSMSellOrderWnd:OpenCSMSellOrderWnd()
end

function CNpcDialogBox:BuyOrderFun(uParam1)
	CPurchasingAgentMainWnd.GetWnd():OpenPanel(true)
end

function CNpcDialogBox:QuickSell(uParam1)
	CPurchasingAgentMainWnd.GetWnd():OnQuickSellBtn()
end

function CNpcDialogBox:CreatCofCFun(uParam1)
	Gac2Gas:OpenCofCNpcDialog(g_Conn,0)
end

--function CNpcDialogBox:CofCMainFun(uParam1)
--	local cofc_id = g_MainPlayer.m_Properties:GetCofcID()
--	Gac2Gas:GetCofcInfo(g_Conn, cofc_id)
--end

function CNpcDialogBox:StockMainFun(uParam1)
	if g_GameMain.m_CofCMainStockWnd == nil then
		g_GameMain.m_CofCMainStockWnd = CreateStockMainWnd(g_GameMain)
		g_GameMain.m_CofCMainStockWnd:ShowWnd(true)
	elseif g_GameMain.m_CofCMainStockWnd:IsShow() == false then
		g_GameMain.m_CofCMainStockWnd:ShowWnd(true)
	end
	Gac2Gas:GetCofCStockInfo(g_Conn)
end

function CNpcDialogBox:CreateArmyCorpsFun(uParam1)
	if(not g_GameMain.m_TongCreate) then
		gac_require "relation/tong/TongCreate"
		g_GameMain.m_TongCreate = CreateTongCreateWnd(g_GameMain)
	end
	g_GameMain.m_TongCreate:OpenPanel(2)
end

---------------------佣兵团改名
function CNpcDialogBox:ArmyCorpsChangeNameFun(uParam1)
	if g_MainPlayer.m_Properties:GetArmyCorpsName() == "" then
		MsgClient(8509)
		return
	end
	if( not string.find(g_MainPlayer.m_Properties:GetArmyCorpsName(), "&") ) then
		MsgClient(8544)
		return
	end
	Gac2Gas:PreChangeArmyCorpsName(g_Conn)
end

function CNpcDialogBox:TongCreateFun(uParam1)
	if(not g_GameMain.m_TongCreate) then
		gac_require "relation/tong/TongCreate"
		g_GameMain.m_TongCreate = CreateTongCreateWnd(g_GameMain)
	end
	g_GameMain.m_TongCreate:OpenPanel(1)
end

function CNpcDialogBox:CanUpTongTypeFun(uParam1)
	if g_GameMain.m_TongBase.m_TongPos ~= g_TongMgr.m_tblPosInfo["团长"] then
		MsgClient(9355)
		return false
	end
	CTongTypeUpdateWnd.GetWnd():OnUpTongTypeFun()
end

function CNpcDialogBox:CanChangeTongTypeFun(uParam1)
	if g_GameMain.m_TongBase.m_TongPos ~= g_TongMgr.m_tblPosInfo["团长"] then
		MsgClient(9355)
		return false
	end
	CTongTypeChangeWnd.GetWnd():OpenPanel(true)
end

function CNpcDialogBox:ResetTongTypeFun(uParam1)
	if g_GameMain.m_TongBase.m_TongPos ~= g_TongMgr.m_tblPosInfo["团长"] then
		MsgClient(9370)
		return false
	end
	CTongTypeChangeWnd.GetWnd():ResetTongType()
end

function CNpcDialogBox:TongMainFun(uParam1)
	local tongId =g_MainPlayer.m_Properties:GetTongID()
	if(0 == tongId) then
		MsgClient(9401)
		return
	else
		g_GameMain.m_TongMainBuildPan:ShowWnd(true)
		local AutoCloseWnd = CAutoCloseWnd:new()
		AutoCloseWnd:AutoCloseWnd(g_GameMain.m_TongMainBuildPan)
	end
end

function CNpcDialogBox:TongRecommendFun(uParam1)
	if(not g_GameMain.m_TongRecommend) then
		gac_require "relation/tong/TongRecommend"
		g_GameMain.m_TongRecommend = CreateTongRecommendWnd(g_GameMain)
	end
	g_GameMain.m_TongRecommend:OpenPanel(true) --这个参数不是用于开启和关闭面板的
end

function CNpcDialogBox:TongRequestFun(uParam1)
	if(not g_GameMain.m_TongRequest) then
		gac_require "relation/tong/TongRequest"
		g_GameMain.m_TongRequest = CreateTongRequestWnd(g_GameMain)
	end
	g_GameMain.m_TongRequest:OpenPanel()
end

function CNpcDialogBox:TongRecruitFun(uParam1)
	if(not g_GameMain.m_TongRecruit) then
		local TongRecruit = CTongRecruit.GetWnd(g_GameMain)
	end
	g_GameMain.m_TongRecruit:OpenPanel()
end

function CNpcDialogBox:TongChallengeFun(uParam1)
	local tongId = g_MainPlayer.m_Properties:GetTongID()
	if(0 == tongId) then
		MsgClient(9401)
		return
	else
		if(not g_GameMain.m_TongChallenge) then
			gac_require "relation/tong/TongChallenge"
			g_GameMain.m_TongChallenge = CreateTongChallengeWnd(g_GameMain)
		end
		g_GameMain.m_TongChallenge:OpenPanel()
		local AutoCloseWnd = CAutoCloseWnd:new()
		AutoCloseWnd:AutoCloseWnd(g_GameMain.m_TongChallenge)
	end
end

function CNpcDialogBox:TongFetchBuildingFun(uParam1)
	if not g_GameMain.m_TongFetchBuildingWnd then
		gac_require "relation/tong/TongFetchBuildingWnd"
		g_GameMain.m_TongFetchBuildingWnd = CreateTongFetchBuildingWnd(g_GameMain)
	end
	g_GameMain.m_TongFetchBuildingWnd:OpenPanel()
end

function CNpcDialogBox:ExitTongChallengeFun(uParam1)
	Gac2Gas:ExitTongChallenge(g_Conn)
end

function CNpcDialogBox:TongFetchResFun(uParam1)
	Gac2Gas:GetTongFethResInfo(g_Conn)
end

--function CNpcDialogBox:TongViewResOrderFun(uParam1)
--	local tongId = g_MainPlayer.m_Properties:GetTongID()
--	if(0 == tongId) then
--		MsgClient(9401)
--		return
--	else
--		if(not g_GameMain.m_TongViewResOrder) then
--			gac_require "relation/tong/TongViewResOrderWnd"
--			g_GameMain.m_TongViewResOrder = CreateTongViewResOrderWnd(g_GameMain)
--		end
--		g_GameMain.m_TongViewResOrder:OpenPanel()
--	end
--end

function CNpcDialogBox:TongChangeNameFun(uParam1)
	if( g_MainPlayer.m_Properties:GetTongID() == 0 ) then
		MsgClient(9401)
		return
	end
	if( not string.find(g_MainPlayer.m_Properties:GetTongName(), "&") ) then
		MsgClient(8999)
		return
	end
	Gac2Gas:PreChangeTongName(g_Conn)
end

function CNpcDialogBox:LiveSkillSmithingNPCFun(uParam1)
	local bFlag = g_GameMain.m_LiveSkillMainWnd:GetStateByName("锻造")
	g_GameMain.m_NpcDlgCopy:OpenPanel("锻造", bFlag)
end

function CNpcDialogBox:LiveSkillCastingNPCFun(uParam1)
	local bFlag = g_GameMain.m_LiveSkillMainWnd:GetStateByName("铸甲")
	g_GameMain.m_NpcDlgCopy:OpenPanel("铸甲", bFlag)
end

function CNpcDialogBox:LiveSkillCraftNPCFun(uParam1)
	local bFlag = g_GameMain.m_LiveSkillMainWnd:GetStateByName("工艺")
	g_GameMain.m_NpcDlgCopy:OpenPanel("工艺", bFlag)
end

function CNpcDialogBox:LiveSkillCuisineNPCFun(uParam1)
	local bFlag = g_GameMain.m_LiveSkillMainWnd:GetStateByName("烹饪")
	g_GameMain.m_NpcDlgCopy:OpenPanel("烹饪", bFlag)
end

function CNpcDialogBox:LiveSkillPharmacyNPCFun(uParam1)
	local bFlag = g_GameMain.m_LiveSkillMainWnd:GetStateByName("制药")
	g_GameMain.m_NpcDlgCopy:OpenPanel("制药", bFlag)
end

function CNpcDialogBox:LiveSkillCollectOreNPCFun(uParam1)
	local bFlag = g_GameMain.m_LiveSkillMainWnd:GetStateByName("采矿")
	g_GameMain.m_NpcDlgCopy:OpenPanel("采矿", bFlag)
end

function CNpcDialogBox:LiveSkillFlowerNPCFun(uParam1)
	local bFlag = g_GameMain.m_LiveSkillMainWnd:GetStateByName("花卉")
	g_GameMain.m_NpcDlgCopy:OpenPanel("花卉", bFlag)
end

function CNpcDialogBox:DaTaoShaFun(uParam1)
	if  g_MainPlayer and g_MainPlayer.m_LockCenter.m_LockingObj then
		local EntityID = g_MainPlayer.m_LockCenter.m_LockingObj:GetEntityID()
		Gac2Gas:ShowDaTaoShaWnd(g_Conn, EntityID)
	end
end

function CNpcDialogBox:JiFenSaiFun(uParam1)
	if g_MainPlayer and g_MainPlayer.m_LockCenter.m_LockingObj then
		local EntityID = g_MainPlayer.m_LockCenter.m_LockingObj:GetEntityID()
		Gac2Gas:ShowJiFenSaiWnd(g_Conn, EntityID)
	end
end

function CNpcDialogBox:AreaFbSelFun(uParam1)
	if g_MainPlayer and g_MainPlayer.m_LockCenter.m_LockingObj then
		local EntityID = g_MainPlayer.m_LockCenter.m_LockingObj:GetEntityID()
		Gac2Gas:ShowAreaFbSelWnd(g_Conn, EntityID)
	end
end

function CNpcDialogBox:MatchGameSelFun(uParam1)
	if g_MainPlayer and g_MainPlayer.m_LockCenter.m_LockingObj then
		local EntityID = g_MainPlayer.m_LockCenter.m_LockingObj:GetEntityID()
		Gac2Gas:ShowMatchGameSelWnd(g_Conn, EntityID)
	end
end

function CNpcDialogBox:DaDuoBaoFun(uParam1)
	if g_MainPlayer and g_MainPlayer.m_LockCenter.m_LockingObj then
		local EntityID = g_MainPlayer.m_LockCenter.m_LockingObj:GetEntityID()
		Gac2Gas:ShowDaDuoBaoWnd(g_Conn, EntityID)
	end
end

function CNpcDialogBox:JuQingFun(uParam1)
	if g_MainPlayer and g_MainPlayer.m_LockCenter.m_LockingObj then
		local EntityID = g_MainPlayer.m_LockCenter.m_LockingObj:GetEntityID()
		Gac2Gas:ShowJuQingWnd(g_Conn, EntityID)
	end
end

function CNpcDialogBox:DrinkFun(uParam1)
	if g_MainPlayer and g_MainPlayer.m_LockCenter.m_LockingObj then
		local EntityID = g_MainPlayer.m_LockCenter.m_LockingObj:GetEntityID()
		Gac2Gas:ShowDrinkWnd(g_Conn, EntityID)
	end 
end

function CNpcDialogBox:MercenaryEducateActionFun(uParam1)
	if g_MainPlayer and g_MainPlayer.m_LockCenter.m_LockingObj then
		local EntityID = g_MainPlayer.m_LockCenter.m_LockingObj:GetEntityID()
		Gac2Gas:ShowYbEducateActionWnd(g_Conn, EntityID)
	end
end

function CNpcDialogBox:MercenaryMonsterFbFun(uParam1)
	if g_MainPlayer and g_MainPlayer.m_LockCenter.m_LockingObj then
		local EntityID = g_MainPlayer.m_LockCenter.m_LockingObj:GetEntityID()
		Gac2Gas:ShowMercenaryMonsterFbWnd(g_Conn, EntityID)
	end
end

function CNpcDialogBox:JiFenSaiInfoFun(uParam1)
	if g_MainPlayer and g_MainPlayer.m_LockCenter.m_LockingObj then
		local EntityID = g_MainPlayer.m_LockCenter.m_LockingObj:GetEntityID()
		Gac2Gas:ShowJiFenSaiInfoWnd(g_Conn, EntityID)
	end
end

function CNpcDialogBox:WarZoneSelFun(uParam1)
	if g_MainPlayer and g_MainPlayer.m_LockCenter.m_LockingObj then
		local EntityID = g_MainPlayer.m_LockCenter.m_LockingObj:GetEntityID()
		ShowWarZoneSelWnd(EntityID)
	end
end



--local NpcTriggerEvent_Common = NpcTriggerEvent_Common
function CNpcDialogBox:StartQuestionFun( uParam1)
	if g_MainPlayer and g_MainPlayer.m_LockCenter.m_LockingObj then
		local EntityID = g_MainPlayer.m_LockCenter.m_LockingObj:GetEntityID()
--		local QuestionTypeName = nil 
		local NpcName = self.m_NpcNameText
--		local TriggerTbl = NpcTriggerEvent_Common[npcName]
--		for _, v in pairs(TriggerTbl) do 
--			if v.TriggerType == "选择" then
--				for _, Arg in pairs( v.ScriptArg) do
--					if Arg[1] == "答题" then
--						QuestionTypeName = Arg[2]
--						break
--					end
--				end
--				break
--			end
--		end
		local TriggerTbl = GetNpcTriggerArg(NpcName, "选择", "答题")
		if not TriggerTbl then
			return
		end
		local QuestionTypeName = TriggerTbl["Arg"][1][1]
		if QuestionTypeName == nil or QuestionTypeName == "" then
			return
		end		
		Gas2Gac:RetShowEssayQuestionWnd(g_Conn, QuestionTypeName, "", EntityID)
	end
end


function CNpcDialogBox:EnterTongAreaFun(uParam1)
	if(not g_MainPlayer) then return end
	local tongId = g_MainPlayer.m_Properties:GetTongID()
	if(0 == tongId) then
		MsgClient(9401)
		return
	else
		if(g_MainPlayer.m_LockCenter.m_LockingObj) then
			local EntityID = g_MainPlayer.m_LockCenter.m_LockingObj:GetEntityID()
			Gac2Gas:EnterTongArea(g_Conn, EntityID)
		end
	end
end

function CNpcDialogBox:LeaveTongAreaFun(uParam1)
	if(not g_MainPlayer) then return end
	local tongId = g_MainPlayer.m_Properties:GetTongID()
	if(0 == tongId) then
		MsgClient(9401)
		return
	else
		if(g_MainPlayer.m_LockCenter.m_LockingObj) then
			local EntityID = g_MainPlayer.m_LockCenter.m_LockingObj:GetEntityID()
			Gac2Gas:LeaveTongArea(g_Conn, EntityID)
		end
	end
end

function CNpcDialogBox:TongResTransFun(uParam1)
	if(not g_MainPlayer) then return end
	local tongId = g_MainPlayer.m_Properties:GetTongID()
	if(0 == tongId) then
		MsgClient(9401)
		return
	else
		if(g_MainPlayer.m_LockCenter.m_LockingObj) then
			local EntityID = g_MainPlayer.m_LockCenter.m_LockingObj:GetEntityID()
			Gac2Gas:ShowTongResTransWnd(g_Conn, EntityID)
		end
	end
end

function CNpcDialogBox:EnterTongChallengeAreaFun(uParam1)
	if(not g_MainPlayer) then return end
	local tongId = g_MainPlayer.m_Properties:GetTongID()
	if(0 == tongId) then
		MsgClient(9401)
		return
	else
		if(g_MainPlayer.m_LockCenter.m_LockingObj) then
			local EntityID = g_MainPlayer.m_LockCenter.m_LockingObj:GetEntityID()
			Gac2Gas:EnterTongChallengeArea(g_Conn, EntityID)
		end
	end
end

function CNpcDialogBox:DepotFun(uParam1)
	g_GameMain.m_Depot:OpenPanel()
	g_GameMain.m_WndMainBag:ShowWnd(true)
	g_GameMain.m_WndBagSlots.m_lcAddBagSlots:UpdateBagSlotsPos()
end

--打开技能升级学习面板
function CNpcDialogBox:LearnCommonSkillNpcFun(uParam1)
	local Wnd = g_GameMain.m_LearnCommonSkillWnd
	if not Wnd:IsShow() then
		OpenLearnCommonSkillWnd()
	end
end

local function OpenNpcSkillLearnWnd()
	if not g_GameMain.m_NpcSkillLearnWnd.m_InitWnd then
		Gac2Gas:RequestSkillSeries(g_Conn)
		g_GameMain.m_NpcSkillLearnWnd.IsOpenWnd = true
	else
		g_GameMain.m_NpcSkillLearnWnd:OpenSelf()
	end
end

--大剑师导师
function CNpcDialogBox:LearnSkillNpc1Fun(uParam1)
	local ClassID = g_MainPlayer:CppGetClass()
	if ClassID ==  EClass.eCL_Warrior then
		OpenNpcSkillLearnWnd()
	end		
end

--魔剑士导师
function CNpcDialogBox:LearnSkillNpc2Fun(uParam1)
	local ClassID = g_MainPlayer:CppGetClass()
	if ClassID ==  EClass.eCL_MagicWarrior then
		OpenNpcSkillLearnWnd()
	end		
end

--法师导师
function CNpcDialogBox:LearnSkillNpc3Fun(uParam1)
	local ClassID = g_MainPlayer:CppGetClass()
	if ClassID ==  EClass.eCL_NatureElf then
		OpenNpcSkillLearnWnd()	
	end		
end

--邪魔导师
function CNpcDialogBox:LearnSkillNpc4Fun(uParam1)
	local ClassID = g_MainPlayer:CppGetClass()
	if ClassID ==  EClass.eCL_EvilElf then
		OpenNpcSkillLearnWnd()	
	end		
end

--牧师导师
function CNpcDialogBox:LearnSkillNpc5Fun(uParam1)
	local ClassID = g_MainPlayer:CppGetClass()
	if ClassID ==  EClass.eCL_Priest then
		OpenNpcSkillLearnWnd()	
	end		
end

--兽人导师
function CNpcDialogBox:LearnSkillNpc6Fun(uParam1)
	local ClassID = g_MainPlayer:CppGetClass()
	if ClassID ==  EClass.eCL_OrcWarrior then
		OpenNpcSkillLearnWnd()	
	end		
end

--建造建筑--(显示帮会建筑面板)
function CNpcDialogBox:CreateBuildingFun(uParam1)
	if(not g_GameMain.m_TongCreateBuilding) then
		g_GameMain.m_TongCreateBuilding = CTongBuildingItemCreateWnd:new()
	end
	g_GameMain.m_TongCreateBuilding:OpenPanel(true)
end

--捐献金钱--
function CNpcDialogBox:ContributeMoneyFun(uParam1)
	if(not g_GameMain.m_ContributeMoney) then
		gac_require "relation/tong/TongMsgBox"
		g_GameMain.m_ContributeMoney = CreateTongInputBoxWnd(g_GameMain)
	end
	Gac2Gas:GetMoneyCanContribute(g_Conn)
end

--显示佣兵团仓库--(显示帮会仓库面板)
function CNpcDialogBox:OpenMercenaryDepotFun(uParam1)
	g_GameMain.m_TongDepot:OpenPanel()
end

--交割粮草(交割帮会粮草)
function CNpcDialogBox:PromptForageRandomFun(uParam1)
	if not IsCppBound(g_MainPlayer) then
		return
	end
	if(g_MainPlayer.m_LockCenter.m_LockingObj) then
		local EntityID = g_MainPlayer.m_LockCenter.m_LockingObj:GetEntityID()
		Gac2Gas:RandomShowTongSellResWnd(g_Conn, EntityID)
	end
end

--更新NPC等级--
function CNpcDialogBox:UpdateOrderFun(uParam1)
	local function IsContinue(GameWnd, uButton)
		if uButton == MB_BtnOK then
			if  g_MainPlayer and g_MainPlayer.m_LockCenter.m_LockingObj then
				local EntityID = g_MainPlayer.m_LockCenter.m_LockingObj:GetEntityID()
				Gac2Gas:UpdateBuildingLevel(g_Conn, EntityID)
			end
		end
		return true
	end	
	MessageBox(g_GameMain, GetStaticTextClient(9102), BitOr(MB_BtnOK, MB_BtnCancel), IsContinue, g_GameMain)
end

--拆除--
function CNpcDialogBox:BackoutFun(uParam1)
	local function IsContinue(GameWnd, uButton)
		if uButton == MB_BtnOK then
			if  g_MainPlayer and g_MainPlayer.m_LockCenter.m_LockingObj then
				local EntityID = g_MainPlayer.m_LockCenter.m_LockingObj:GetEntityID()
				Gac2Gas:RemoveBuilding(g_Conn, EntityID)
			end	
		end
		return true
	end
	MessageBox(g_GameMain, GetStaticTextClient(9101), BitOr(MB_BtnOK, MB_BtnCancel), IsContinue, g_GameMain)
end

--进行制造--
function CNpcDialogBox:MakingFun(uParam1)
	if(not g_GameMain.m_TongProductionCenter) then
		g_GameMain.m_TongProductionCenter = CTongProductionCenterWnd:new()
	end
	g_GameMain.m_TongProductionCenter:OpenPanel(true)
end

--科技研发--
function CNpcDialogBox:TongOpenScienceFun(uParam1)
	g_GameMain.m_TongScience:OpenPanel(true)
end

--科技学习--
function CNpcDialogBox:TongLearnScienceFun(uParam1)
	g_GameMain.m_TongLearnScience:OpenPanel(true)
end

--申请撤退--
function CNpcDialogBox:ApplyRetreatFun(uParam1)
	if not IsCppBound(g_MainPlayer) then
		return
	end

	local function RetreatStationWarn(g_GameMain,uButton)
		if uButton == MB_BtnOK then
			Gac2Gas:RequestRetreatStation(g_Conn)
		end

		g_GameMain.m_RetreatStationWarn = nil
		return true
	end
	
	local str = MsgBoxMsg(17012)
	g_GameMain.m_RetreatStationWarn = MessageBox( g_GameMain, str, BitOr( MB_BtnOK, MB_BtnCancel), RetreatStationWarn, g_GameMain)
end

--占领此驻地--
function CNpcDialogBox:TakeStationFun(uParam1)

end

--结盟--
function CNpcDialogBox:TongAlignmentFun(uParam1)

end

--打开装备修理界面
function CNpcDialogBox:OpenFixEquipWndFun(uParam1)
--    g_GameMain.m_FixEquipWnd:ShowWnd(true)
--    if   g_GameMain.m_WndMainBag:IsShow() == false then
--        g_GameMain.m_WndMainBag:ShowWnd(true)  
--    end
--    if g_GameMain.m_RoleStatus:IsShow() == false then
--        g_GameMain.m_RoleStatus:ShowPanel(true)  
--    end
--    g_GameMain.m_FixEquipWnd:SetFocus()
--    g_GameMain.m_RoleStatus:UpdataEquipTips()
--	g_GameMain.m_WndMainBag.m_ctItemRoom:UpdateBagTips()
--		local AutoCloseWnd = CAutoCloseWnd:new()
--		AutoCloseWnd:AutoCloseWnd(g_GameMain.m_FixEquipWnd)

	
	g_GameMain.m_NewFixEquipWnd.m_EquipInRoleCheckBtn:SetCheck(true)
	g_GameMain.m_NewFixEquipWnd.m_EquipInBagCheckBtn:SetCheck(false)
	if not g_GameMain.m_NewFixEquipWnd:IsShow() then	
		g_GameMain.m_CEquipInRoleWnd:SetData()
		g_GameMain.m_CEquipInBagWnd:SetData()
		g_GameMain.m_NewFixEquipWnd:ShowWnd(true)
	end
	if not g_GameMain.m_CEquipInRoleWnd:IsShow() then
		g_GameMain.m_CEquipInRoleWnd:ShowWnd(true)
	end
	g_GameMain.m_CEquipInBagWnd:ShowWnd(flase)
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(g_GameMain.m_NewFixEquipWnd)
end

function CNpcDialogBox:ChallengeIntroduceFun(uParam1)
	CShowRulesWnd.GetWnd(nil,"挑战介绍")
end

function CNpcDialogBox:NationIntroduceFun(uParam1)
	CShowRulesWnd.GetWnd(nil,"国战介绍")
end

function CNpcDialogBox:RescourseIntroduceFun(uParam1)
	CShowRulesWnd.GetWnd(nil,"资源领取介绍")
end

function CNpcDialogBox:GivePointIntroduceFun(uParam1)
	CShowRulesWnd.GetWnd(nil,"国贡领取介绍")
end

function CNpcDialogBox:PkPointIntroduceFun(uParam1)
	CShowRulesWnd.GetWnd(nil,"击杀积分介绍")
end


function CNpcDialogBox:BeComeYbFun(uParam1)
	if g_GameMain.m_MercenaryLevelTrace.m_IsCheckIn then
		local InfoWnd = g_GameMain.m_InformationWnd
		if InfoWnd:IsShow() then
			InfoWnd:ShowWnd(false)
		end
		g_GameMain.m_MercenaryDirWnd:InitWnd()
	end
end

function CNpcDialogBox:DingJiYbFun(uParam1)
	--Gac2Gas:DoMercenaryLevelAppraise(g_Conn)
end

function CNpcDialogBox:ReceivePowerYbFun(uParam1)
	--ShowMercenaryLevelAward()
end

function CNpcDialogBox:LingYuTransmitFun(uParam1)
	local Wnd = CLingYuTransmitWnd.GetWnd()
	Wnd:ShowLingYuTransmitWnd()
end
--
--function CNpcDialogBox:SpecialNpcFun(uParam1)
--	local Wnd = g_GameMain.m_SpecialNpcWnd
--	Wnd:InitWnd()
--end

function CNpcDialogBox:SpecialNpcFun(uParam1)
	local Wnd = g_GameMain.m_SpecialNpcWnd
	Wnd:InitWnd()
end

function CNpcDialogBox:UndergroudTeamList() 	 --地下城副本排队
	g_GameMain.m_TeamListOneItem.state = 1
	g_GameMain.m_TeamListOneItem.m_Text2:SetWndText(GetStaticTextClient(26006))
	Gac2Gas:getAppTeamList(g_Conn,1)
	g_GameMain.m_TeamAppUnderList:ShowWnd(true)
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(g_GameMain.m_TeamAppUnderList)
end

function CNpcDialogBox:ActionTeamList()   	  	--多人活动报名列表
	g_GameMain.m_TeamListOneItem.state = 2
	g_GameMain.m_TeamListOneItem.m_Text2:SetWndText(GetStaticTextClient(26007))
	Gac2Gas:getAppTeamList(g_Conn,2)
	g_GameMain.m_TeamAppActList:ShowWnd(true)
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(g_GameMain.m_TeamAppActList)
end

function CNpcDialogBox:TaskTeamList()						--	佣兵任务报名列表
	g_GameMain.m_TeamListOneItem.state = 3
		g_GameMain.m_TeamListOneItem.m_Text2:SetWndText(GetStaticTextClient(26008))
		Gac2Gas:getAppTeamList(g_Conn,3)
	g_GameMain.m_TeamAppTaskList:ShowWnd(true)
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(g_GameMain.m_TeamAppTaskList)
end

local function SetData(nItemCount)
	if nItemCount > 0 then
		g_GameMain.m_HandForage.m_ShowText:SetWndText(GetStaticTextClient(210000, nItemCount,nItemCount * 10)) 
	end
end

function CNpcDialogBox:HandForage(uParam1)
	local nItemCount = g_GameMain.m_BagSpaceMgr:GetItemCountBySpace(g_StoreRoomIndex.PlayerBag,1, "军资勋章" )
	
	if (not g_GameMain.m_HandForage) then
		--gac_require "relation/tong/TongForageWnd"
		if nItemCount > 0 then
			g_GameMain.m_HandForage = CTongForageWnd.GetWnd(nItemCount, nItemCount * 10)
		end
	end
	if nItemCount > 0 then
		SetData(nItemCount)
	else
		MsgClient(9446)
		return
	end
	g_GameMain.m_HandForage:ShowWnd(true)
end

function CNpcDialogBox:BossBattleFun(uParam1)
	if g_MainPlayer and g_MainPlayer.m_LockCenter.m_LockingObj then
		local EntityID = g_MainPlayer.m_LockCenter.m_LockingObj:GetEntityID()
		Gac2Gas:ShowBossBattleWnd(g_Conn, EntityID)
	end
end

function CNpcDialogBox:DragonCaveFun(uParam1)
	if g_MainPlayer and g_MainPlayer.m_LockCenter.m_LockingObj then
		local EntityID = g_MainPlayer.m_LockCenter.m_LockingObj:GetEntityID()
		Gac2Gas:ShowDragonCaveWnd(g_Conn, EntityID)
	end
end
--function CNpcDialogBox:IssueFetchFun(uParam1)
--	if (not g_GameMain.m_IssueFetchWnd) then
--		g_GameMain.m_IssueFetchWnd = CTongForageQueryWnd.CreateTongForageQuery(g_GameMain)
--	end
--	g_GameMain.m_IssueFetchWnd:ShowWnd(true)
--end

function CNpcDialogBox:IssueFetchFun(uParam1)
	if g_MainPlayer then
		Gac2Gas:IssueFetchQueryWnd(g_Conn)
	end
end

function CNpcDialogBox:DrinkShootFun(uParam1)
	if g_MainPlayer then
		Gac2Gas:DrinkShootGetReady(g_Conn)
	end
end

function CNpcDialogBox:GMCompenateFun(uParam1)
	g_GameMain.m_GmActivity:OpenWnd()
end

function CNpcDialogBox:OpenContractManuMarketWnd()
    CContractManufactureMarketWnd.OpenContractManufMarketWnd()
end

function CNpcDialogBox:OpenEquipRefine()
    CEquipRefine.OpenEquipRefineWnd()
end


function CNpcDialogBox:OpenEquipSuperaddWnd()
    CEquipSuperaddWnd.OpenSuperaddWnd()
end

function CNpcDialogBox:OpenNeedFireActivity()
	if g_MainPlayer.m_LockCenter.m_LockingObj then
		local EntityID = g_MainPlayer.m_LockCenter.m_LockingObj:GetEntityID()
		CTongNeedFireActivityWnd.CheckNeedFireIsOpen(EntityID)
	end
end

function CNpcDialogBox:OpenTongContributeMoneyExp()
	local tongId =g_MainPlayer.m_Properties:GetTongID()
	if(0 == tongId) then
		MsgClient(9401)
		return
	end
	Gac2Gas:GetTongContributeMoneyInfo(g_Conn,1)
end
function CNpcDialogBox:OpenTongContributeMoneySoul()
	local tongId =g_MainPlayer.m_Properties:GetTongID()
	if(0 == tongId) then
		MsgClient(9401)
		return
	end
	Gac2Gas:GetTongContributeMoneyInfo(g_Conn,2)
end
function CNpcDialogBox:OpenTongContributeMoneyResource()
	local tongId =g_MainPlayer.m_Properties:GetTongID()
	if(0 == tongId) then
		MsgClient(9401)
		return
	end
	Gac2Gas:GetTongContributeMoneyInfo(g_Conn,3)
end
