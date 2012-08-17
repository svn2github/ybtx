gac_gas_require "activity/quest/QuestMgrInc"
gac_gas_require "item/store_room_cfg/StoreRoomCfg"
gac_gas_require "activity/npc/CheckNpcDropSetting"

local os = os
local EAssignMode =
{
	eAM_FreePickup 		   = 0, --自由拾取
	eAM_AverageAssign 	 = 1, --平均分配
	eAM_RespectivePickup = 2, --各自拾取
	eAM_AuctionAssign 	 = 3, --拍卖模式
	eAM_NeedAssign			= 4, --需求分配
}
local EAuctionStandard =
{
	eAS_GrayStandard   = 0, --灰色
	eAS_WhiteStandard  = 1, --白色
	eAS_GreenStandard  = 2, --绿色
	eAS_BlueStandard   = 3, --蓝色
	eAS_PurpleStandard = 4, --紫色
	eAS_OrangeStandard = 5, --橙色
	eAS_YellowStandard = 6, --黄色
	eAS_CyanStandard   = 7, --青色
}
local QuestState = {
	init	= 1,
	failure	= 2,
	finish	= 3
}

local EClass = EClass
local ECamp = ECamp
local ShareArea = 38
local DropTeamNum = 30
local TeamItemNum = 30
local DropProbability = 100000000
local DropItemProtectTime = 20
local g_ItemInfoMgr = CItemInfoMgr:new()
local g_NpcDropObjTable = g_NpcDropObjTable
local g_NpcDropItemTable = g_NpcDropItemTable
local g_QuestDropItemMgr = g_QuestDropItemMgr
--local NpcDropItem_Common = NpcDropItem_Common
local DropTeam_Server = DropTeam_Server
local g_WhereGiveQuestMgr = g_WhereGiveQuestMgr
local DropItemObjTeam_Server = DropItemObjTeam_Server
local RoleQuestDB = RequireDbBox("RoleQuestDB")
local RoomMgrDB = RequireDbBox("GasRoomMgrDB")
local LoginServerDB = RequireDbBox("LoginServerDB")
local GasTeamDB = RequireDbBox("GasTeamDB")
local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
local AreaDB = RequireDbBox("AreaDB")
local g_NpcDropObjTeamRate = g_NpcDropObjTeamRate
local g_NpcDropItemTeamRate = g_NpcDropItemTeamRate
local g_RepeatQuestMgr = g_RepeatQuestMgr
local event_type_tbl = event_type_tbl

local NpcDropItemDB = CreateDbBox(...)

local function Get2PosDistance(pos1, pos2)
	return math.max( math.abs( pos1.x - pos2.x ), math.abs( pos1.y - pos2.y ) )
end

--从ItemTeam随机物品
local function RandomTeamItems(PlayerId, teamindex)
	local ClassNameTbl = {
		[EClass.eCL_Warrior]				= "大剑",
		[EClass.eCL_MagicWarrior]		= "魔剑",
		[EClass.eCL_Paladin]				= "骑士",
		[EClass.eCL_NatureElf]			= "法师",
		[EClass.eCL_EvilElf]				= "邪魔",
		[EClass.eCL_Priest]					= "牧师",
		[EClass.eCL_DwarfPaladin]		= "矮骑",
		[EClass.eCL_ElfHunter]			= "精灵弓",
		[EClass.eCL_OrcWarrior]			= "兽战",
	}
	local CampNameTbl = {
		[ECamp.eCamp_AmyEmpire]			= "艾米",
		[ECamp.eCamp_WestEmpire]		= "神圣",
		[ECamp.eCamp_DevilIsland]		= "修斯",
	}
	if string.find(teamindex, "#class#") then	
		local FightSkillDB = RequireDbBox("FightSkillDB")
		local ClassID = FightSkillDB.Dbs_SelectPlayerClass(PlayerId)
		teamindex = string.gsub(teamindex, "#class#", ClassNameTbl[ClassID])
	end
	if string.find(teamindex, "#camp#") then
		local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
		local Camp = CharacterMediatorDB.GetCamp(PlayerId)
		teamindex = string.gsub(teamindex, "#camp#", CampNameTbl[Camp])
	end
	local AllRateSum=g_NpcDropItemTeamRate[teamindex]
	local Items_List={}

--	for i=1,TeamItemNum do
--		local DropRate="DropRate"..i
--		local Rate = DropTeam_Server(teamindex, "DropRate")
--		if Rate == "" or Rate == nil or not IsNumber(Rate) then
--			Rate = 0
--		end
--		AllRateSum=AllRateSum+Rate  --以每一组的物品概率值相加组成总概率
--	end
	
	local RandomItemRate=math.random(0,AllRateSum)  --随机一个概率值
	local ItemRateIndex=0
	
	local droprate1 = DropTeam_Server(teamindex,"DropRate1")
	if droprate1 == "" or droprate1 == nil then
		droprate1 = 0
	else
		droprate1 = tonumber(droprate1)
	end
	--比较随机得到的概率值和物品概率值
	if RandomItemRate>=0 and RandomItemRate<=droprate1 then
		ItemRateIndex=1
	else
		local droprate1 = 0
		local droprate2 = DropTeam_Server(teamindex,"DropRate1")
		if droprate2 == "" or droprate2 == nil then
			droprate2 = 0
		else
			droprate2 = tonumber(droprate2)
		end
		
		for i=2,TeamItemNum do	
			local dropr1 = DropTeam_Server(teamindex,"DropRate"..(i-1))
			if dropr1=="" or not dropr1 then
				dropr1 = 0
			else
				dropr1 = tonumber(dropr1)
			end
			local dropr2 = DropTeam_Server(teamindex,"DropRate"..i)
			if dropr2=="" or not dropr2 then
				dropr2 = 0
			else
				dropr2 = tonumber(dropr2)
			end
			
			droprate2 = dropr2 + droprate2
			droprate1 = dropr1 + droprate1
			if RandomItemRate>droprate1 and RandomItemRate<=droprate2 then
				ItemRateIndex=i
				break
			end
		end
		
	end
	
	if ItemRateIndex>0 and ItemRateIndex<=TeamItemNum then
		local ItemTypeIndex="DropType"..ItemRateIndex
		local ItemNameIndex="DropName"..ItemRateIndex
		local ItemType=DropTeam_Server(teamindex,ItemTypeIndex)
		local ItemName=DropTeam_Server(teamindex,ItemNameIndex)
		local DropMinNum = DropTeam_Server(teamindex,"DropMinNum")
		local DropMaxNum = DropTeam_Server(teamindex,"DropMaxNum")
		local ItemNum = math.random(DropMinNum,DropMaxNum)
		--Items_List = {ItemType,ItemName,ItemNum}
		local Only = g_ItemInfoMgr:GetItemInfo( ItemType,ItemName,"Only" )
		if Only ~= 1 then
			local FoldLimit = g_ItemInfoMgr:GetItemInfo( ItemType,ItemName,"FoldLimit" ) or 1
			if FoldLimit == 1 then
				for i = 1, ItemNum do
					table.insert(Items_List, {ItemType,ItemName,1})
				end
			else
				table.insert(Items_List, {ItemType,ItemName,ItemNum})
			end
		elseif Only == 1 then
			local havenum = RoomMgrDB.GetItemCount(PlayerId, ItemType,ItemName)
			if havenum == 0 then
				if g_WhereGiveQuestMgr["Goods"][ItemName] and ItemType == 16 then
					local questname = g_WhereGiveQuestMgr["Goods"][ItemName][1]
					local queststate = RoleQuestDB.GetQuestState({PlayerId,questname})
					if queststate == nil or (queststate == QuestState.finish and g_RepeatQuestMgr[questname]) then
						table.insert(Items_List, {ItemType,ItemName,1,["QuestDropOwner"] = PlayerId})
					end
				end
			end
		end
	end
	return Items_List
end

--随机产生掉落OBJ表
local function RandomTeamObjs(PlayerId, teamindex)
	local ClassNameTbl = {
		[EClass.eCL_Warrior]				= "大剑",
		[EClass.eCL_MagicWarrior]		= "魔剑",
		[EClass.eCL_Paladin]				= "骑士",
		[EClass.eCL_NatureElf]			= "法师",
		[EClass.eCL_EvilElf]				= "邪魔",
		[EClass.eCL_Priest]					= "牧师",
		[EClass.eCL_DwarfPaladin]		= "矮骑",
		[EClass.eCL_ElfHunter]			= "精灵弓",
		[EClass.eCL_OrcWarrior]			= "兽战",
	}
	local CampNameTbl = {
		[ECamp.eCamp_AmyEmpire]			= "艾米",
		[ECamp.eCamp_WestEmpire]		= "神圣",
		[ECamp.eCamp_DevilIsland]		= "修斯",
	}
	if string.find(teamindex, "#class#") then	
		local FightSkillDB = RequireDbBox("FightSkillDB")
		local ClassID = FightSkillDB.Dbs_SelectPlayerClass(PlayerId)
		teamindex = string.gsub(teamindex, "#class#", ClassNameTbl[ClassID])
	end
	if string.find(teamindex, "#camp#") then
		local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
		local Camp = CharacterMediatorDB.GetCamp(PlayerId)
		teamindex = string.gsub(teamindex, "#camp#", CampNameTbl[Camp])
	end
	local AllRateSum=g_NpcDropObjTeamRate[teamindex]
	local Objs_List={}
--	for i=1,TeamItemNum do
--		local DropRate="DropRate"..i
--		local Rate = DropItemObjTeam_Server[teamindex][DropRate]
--		if Rate == "" or Rate == nil or not IsNumber(Rate) then
--			Rate = 0
--		end
--		AllRateSum=AllRateSum+Rate
--	end
	
	local RandomItemRate=math.random(0,AllRateSum)
	local ItemRateIndex=0
	
	local droprate1 = DropItemObjTeam_Server(teamindex,"DropRate1")
	if droprate1 == "" or droprate1 == nil then
		droprate1 = 0
	else
		droprate1 = tonumber(droprate1)
	end
	
	if RandomItemRate>=0 and RandomItemRate<=droprate1 then
		ItemRateIndex=1
	else
		local droprate1 = 0
		local droprate2 = DropItemObjTeam_Server(teamindex,"DropRate1")
		if droprate2 == "" or droprate2 == nil then
			droprate2 = 0
		else
			droprate2 = tonumber(droprate2)
		end
		
		for i=2,TeamItemNum do	
			local dropr1 = DropItemObjTeam_Server(teamindex,"DropRate"..(i-1))
			if dropr1=="" or not dropr1 then
				dropr1 = 0
			else
				dropr1 = tonumber(dropr1)
			end
			local dropr2 = DropItemObjTeam_Server(teamindex,"DropRate"..i)
			if dropr2=="" or not dropr2 then
				dropr2 = 0
			else
				dropr2 = tonumber(dropr2)
			end

			droprate2 = dropr2 + droprate2
			droprate1 = dropr1 + droprate1
			if RandomItemRate>droprate1 and RandomItemRate<=droprate2 then
				ItemRateIndex=i
				break
			end
		end
		
	end
	if ItemRateIndex>0 and ItemRateIndex<=TeamItemNum then
		local ObjNameIndex = "DropName"..ItemRateIndex
		local ObjName = DropItemObjTeam_Server(teamindex,ObjNameIndex)
		local DropMinNum = DropItemObjTeam_Server(teamindex,"DropMinNum")
		local DropMaxNum = DropItemObjTeam_Server(teamindex,"DropMaxNum")
		local ObjNum = math.random(DropMinNum,DropMaxNum)
--		print("DropObjName ",ObjName)
		if RoleQuestDB.CountObjDropLimitByName(ObjName) <= 0 then
			if g_WhereGiveQuestMgr["Obj"][ObjName] then
				local questname = g_WhereGiveQuestMgr["Obj"][ObjName][1]
				local queststate = RoleQuestDB.GetQuestState({PlayerId,questname})
				if queststate == nil or (queststate == QuestState.finish and g_RepeatQuestMgr[questname]) then
					table.insert(Objs_List, {ObjName,1})
				end
			else
				for i = 1, ObjNum do
					table.insert(Objs_List, {ObjName,1})
				end
			end
		end
	end
	return Objs_List
end

--杀死怪后,产生掉落物品的tal表
local function NpcDeadDropItem(PlayerId,NpcName,NpcLevel,TeamID,CanSharePlayerTbl)
	local DropItemTbl = {}
	DropItemTbl.m_Items_List = {}
	local NpcDropItemTbl = g_NpcDropItemTable[NpcName]
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	local PlayerLevel = CharacterMediatorDB.GetPlayerLevel(PlayerId)
	if NpcDropItemTbl ~= nil then
		for i = 1, table.getn(NpcDropItemTbl) do
			---对NpcDropItem_Common[NpcName]的每一Team进行随机
			local dropLevel = NpcDropItemTbl[i]["Level"]
			if dropLevel == nil or dropLevel == "" or (dropLevel[1] <= NpcLevel and dropLevel[2] >= NpcLevel) then
				local dropr = NpcDropItemTbl[i]["DropRate"](NpcLevel, PlayerLevel)
				if (dropr*DropProbability) >= math.random(0,DropProbability) then
					local team_index = NpcDropItemTbl[i]["DropTeam"]
					if team_index ~="" then
						--从Team中随机物品
						local tbl = RandomTeamItems(PlayerId,team_index)
						for j = 1, table.getn(tbl) do
							table.insert(DropItemTbl.m_Items_List, tbl[j] )
						end
					end
				end
			end
		end
	end
--	if NpcDropItem_Common[NpcName]~=nil then
--		for i=1,DropTeamNum do
--			---对NpcDropItem_Common[NpcName]的每一Team进行随机
----		  local dropLevel = NpcDropItem_Common[NpcName]["DropTeamLevel"..i]
--			local dropLevel = GetCfgTransformValue(false, "ObjRandomDropItem_Server", NpcName, "DropTeamLevel"..i)
--			if dropLevel == nil or dropLevel == "" or (dropLevel[1] <= NpcLevel and dropLevel[2] >= NpcLevel) then
--   			--local RandomRate = math.random(0,DropProbability)
--				local dropr = NpcDropItem_Common[NpcName]["DropTeamRate"..i]
--				if dropr ~= nil and dropr ~= "" and (dropr*DropProbability) >= math.random(0,DropProbability) then
--					local team_index = NpcDropItem_Common[NpcName]["DropTeam"..i]
--					if team_index ~="" then
--						--从Team中随机物品
--						local tbl = RandomTeamItems(PlayerId,team_index)
--						for j = 1, table.getn(tbl) do
--							table.insert(DropItemTbl.m_Items_List, tbl[j] )
--						end
--					end
--				end
--			end
--		end
--	end

	DropItemTbl.m_OwnerId = PlayerId
	DropItemTbl.m_CreateTime = os.time()
	if table.getn(CanSharePlayerTbl) > 1 then
		DropItemTbl.m_BelongToTeam = TeamID
		DropItemTbl.m_CanSharePlayer = CanSharePlayerTbl
	end
	
	return DropItemTbl
end

local function QuestDropObj(NpcName,ShowName,DropObjTbl,CanSharePlayerTbl,sceneName)
	local AddItemResTbl = {}
	local DropInfo = g_QuestDropItemMgr[NpcName] or g_QuestDropItemMgr[ShowName]
	if not DropInfo then
		return AddItemResTbl
	end
	
	for questname , p in pairs(DropInfo) do
		for i = 1, table.getn(p) do
			local droptype,dropname,droprate = p[i].Type,p[i].Object,p[i].Rate
			local RandomRate = math.random(0,DropProbability)  
			if droprate*DropProbability >= RandomRate then
				for j = 1, table.getn(CanSharePlayerTbl) do
					local PlayerId = CanSharePlayerTbl[j]
					local state = RoleQuestDB.GetQuestState({PlayerId, questname})
					if state == QuestState.init then			
						--填加任务物品(直接向背包中添加)
						local param = {}
						param["char_id"] = PlayerId
						param["nType"] = droptype
						param["sName"] = dropname
						param["nCount"] = 1
						param["CheckQuestNeed"] = true
						param["sceneName"] = sceneName
						param["createType"] = event_type_tbl["任务掉落物品"]
						local AddItemRes = CharacterMediatorDB.AddItem(param)
						local tbl = {AddItemRes,["PlayerId"]=PlayerId}
						if not IsNumber(AddItemRes) then  --不是数字添加成功
							tbl["ItemType"]=droptype
							tbl["ItemName"]=dropname
							tbl["ItemNum"]=1
							table.insert(AddItemResTbl,tbl)
						else
							---暂时屏蔽
--							if AddItemRes == 3 then --添加不成功的原因是背包满，则转为传奇掉落
--								local tbl1 = {dropname,1,["QuestDropOwner"] = PlayerId}
--								table.insert(DropObjTbl.m_Objs_List, tbl1)
--							end
							table.insert(AddItemResTbl,tbl)
						end
					end
				end
			end
		end
	end
	return AddItemResTbl
end

--杀死怪后,掉落Obj的tal表
local function NpcDeadDropObj(PlayerId,NpcName,NpcLevel,TeamID,CanSharePlayerTbl, TempTeamMemberID)
	local DropObjTbl = {}
	DropObjTbl.m_Objs_List = {}
	local NpcDropItemObjTbl = g_NpcDropObjTable[NpcName]
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	local PlayerLevel = CharacterMediatorDB.GetPlayerLevel(PlayerId)
	if NpcDropItemObjTbl~=nil then
		for i = 1, table.getn(NpcDropItemObjTbl) do
			local dropLevel = NpcDropItemObjTbl[i]["Level"]
			if dropLevel == nil or dropLevel == "" or (dropLevel[1] <= NpcLevel and dropLevel[2] >= NpcLevel) then
				local dropr = NpcDropItemObjTbl[i]["DropRate"](NpcLevel, PlayerLevel)
				if (dropr*DropProbability) >= math.random(0,DropProbability) then
					local team_index = NpcDropItemObjTbl[i]["DropTeam"]
					if team_index ~="" then
						local tbl = RandomTeamObjs(PlayerId,team_index)
						for j = 1, table.getn(tbl) do
							table.insert(DropObjTbl.m_Objs_List, tbl[j] )
						end
					end
				end
			end
		end
	end
	DropObjTbl.m_OwnerId = PlayerId
	DropObjTbl.m_TempTeamMemberID = TempTeamMemberID
	if table.getn(CanSharePlayerTbl) > 1 then
		local eAM = GasTeamDB.GetAssignMode(TeamID) or EAssignMode.eAM_NeedAssign     --按小队分配
		local AuctionStandard = GasTeamDB.GetAuctionStandard(TeamID) or EAuctionStandard.eAS_GreenStandard --默认为0   --拍卖方式
		local AuctionBasePrice = GasTeamDB.GetAuctionBasePrice(TeamID) or 0 --默认为0   --拍卖方式
		---分配模式为各自拾取时就不用指定队伍归属和可共享者列表了
		DropObjTbl.m_CanSharePlayer = CanSharePlayerTbl
		if eAM ~= EAssignMode.eAM_RespectivePickup then  --不属于自动拾取
			DropObjTbl.m_BelongToTeam = TeamID
			DropObjTbl.m_AssignMode = eAM
			DropObjTbl.m_AuctionStandard = AuctionStandard
			DropObjTbl.m_AuctionBasePrice = AuctionBasePrice
		end
	end
	return DropObjTbl
end

function NpcDropItemDB.NpcDeadDrop(PlayerId,data,CanShareTeamMateTbl,sceneName)
	
	--获取符合分配条件的SharePlayerTbl
	local AreaName = data["AreaName"]
	local CanSharePlayerTbl = {}
	table.insert(CanSharePlayerTbl,PlayerId)
	for i = 1, table.getn(CanShareTeamMateTbl) do
		table.insert(CanSharePlayerTbl,CanShareTeamMateTbl[i])
	end
	local NpcName = data["NpcName"]
	local NpcLevel = data["MonsterLevel"]
	local ShowName = data["ShowName"]
	local TeamID = data["TeamID"]
	local DropObjTbl = NpcDeadDropObj(PlayerId,NpcName,NpcLevel,TeamID,CanSharePlayerTbl, data["TempTeamMemberID"])
	local DropItemTbl = NpcDeadDropItem(PlayerId,NpcName,NpcLevel,TeamID,CanSharePlayerTbl)
	local AddItemResTbl = QuestDropObj(NpcName,ShowName,DropObjTbl,CanSharePlayerTbl,sceneName )
	return DropObjTbl,DropItemTbl,AddItemResTbl
end

local	function GetCanPickUpIndexTbl(PlayerId, ItemList)
	local PickUpIndex = {}
	local RoleQuestDB = RequireDbBox("RoleQuestDB")
	for index = 1,table.maxn(ItemList) do
		if ItemList[index] then
			local ItemType,ItemName ,ItemNum = ItemList[index][1],ItemList[index][2],ItemList[index][3]
			local itemtype = tonumber(ItemType)
			if (not ItemList[index]["QuestDropOwner"]) then
				--不是任务掉落，也不是等待拍卖的物品，可以拾取
				table.insert(PickUpIndex, index)
			elseif ItemList[index]["QuestDropOwner"] == PlayerId 
				and RoleQuestDB.CheckNeedItemNum(PlayerId, ItemType, ItemName, true)[1] then
				--是任务掉落，而且掉落归属自己，并且通过任务检测
				table.insert(PickUpIndex, index)
			end
		end
	end
	return PickUpIndex  --返回可以拾取的物品Index表
end

function NpcDropItemDB.ShowNpcDropItem(data)
	local PlayerId = data["PlayerId"]
	local Items_List = data["DropItemTbl"].m_Items_List
	return GetCanPickUpIndexTbl(PlayerId, Items_List)
end

function NpcDropItemDB.SelAllDropItems(data)
	local DropItemTbl = data["DropItemTbl"]
	local ItemList = DropItemTbl.m_Items_List
	local RemoveIndexTbl = {}
	local AddResTbl = {}
	local param = {}
	param["char_id"] = data["PlayerId"]
	param["createType"] = event_type_tbl["点npc尸体掉落(点所有格子)"]
	param["sceneName"] = data["sceneName"]
	
	if DropItemTbl.m_PickUpIndex then
		for index = 1, table.getn(DropItemTbl.m_PickUpIndex) do
			local GetIndex = DropItemTbl.m_PickUpIndex[index]
			local itemtype = ItemList[GetIndex][1]
			local itemname = ItemList[GetIndex][2]
			local itemnum = ItemList[GetIndex][3]
			
			param["nType"]	= itemtype
			param["sName"] 	= itemname
			param["nCount"]	= itemnum
			if ItemList[GetIndex]["QuestDropOwner"] then
				param["CheckQuestNeed"]	= true
			end
			local ResTbl = NpcDropItemDB.AddOneGridItem(param,true)
			if ResTbl[1] then
				table.insert(RemoveIndexTbl,GetIndex)
			end
			ResTbl["nType"]	= itemtype
			ResTbl["sName"] 	= itemname
			ResTbl["nCount"]	= itemnum
			table.insert(AddResTbl,ResTbl)
		end
	end
	return {AddResTbl,RemoveIndexTbl}
end

--添加物品,方法1
function NpcDropItemDB.AddOneGridItem(data,IsTran)
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	local res1 = CharacterMediatorDB.AddItem(data)
	if IsNumber(res1) then
		if not IsTran then
			CancelTran()
		end
		local RoomMgr = RequireDbBox("GasRoomMgrDB")
		return {false,res1}
	end
	return {true,res1}
end

function NpcDropItemDB.SelOneGridDropItem(data)
----	local PlayerId = data["PlayerId"]
----	local DropItemTbl = data["DropItemTbl"]
----	local GetIndex = data["GetIndex"]
----	local ItemList = DropItemTbl.m_Items_List
--	local Param = {}
--	
--	if not ItemList[GetIndex]["QuestDropOwner"] then  
--		Param["CheckQuestNeed"] = true
--	end
--	
--	Param["char_id"]	= PlayerId
--	Param["nType"]	= ItemList[GetIndex][1]
--	Param["sName"]	= ItemList[GetIndex][2]
--	Param["nCount"]	= ItemList[GetIndex][3]
	data["createType"] = event_type_tbl["点npc尸体掉落(点一个格子)"]
	return NpcDropItemDB.AddOneGridItem(data)
end

SetDbLocalFuncType(NpcDropItemDB.ShowNpcDropItem)
SetDbLocalFuncType(NpcDropItemDB.SelOneGridDropItem)
SetDbLocalFuncType(NpcDropItemDB.SelAllDropItems)
return NpcDropItemDB
