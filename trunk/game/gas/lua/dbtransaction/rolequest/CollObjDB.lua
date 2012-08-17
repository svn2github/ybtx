
gac_gas_require "activity/int_obj/LoadIntObjActionArg"
cfg_load "int_obj/ObjDropItemTeam_Server"
gac_gas_require "activity/quest/QuestMgrInc"

local os = os
local QuestState = {
	init	= 1,
	failure	= 2,
	finish	= 3
}
--local PerfectType = 3
local g_ObjDropNum = g_ObjDropNum
local g_ObjTeamNum = g_ObjTeamNum
local g_ItemInfoMgr = CItemInfoMgr:new()
local ObjRandomDropItem_Server = ObjRandomDropItem_Server
local ObjDropItemTeam_Server = ObjDropItemTeam_Server
local g_QuestDropItemMgr = g_QuestDropItemMgr
local g_QuestNeedMgr = g_QuestNeedMgr
local EClass = EClass
local ECamp = ECamp
local GetCfgTransformValue = GetCfgTransformValue

local CollObjDB = CreateDbBox(...)

local function ClickCollItemObj(data)
	local PlayerId = data["nCharId"]
	local ObjName = data["ObjName"]
	--掉落任务物品
	local QuestDrop = false
 	if g_QuestDropItemMgr[ObjName] then
		for questname , p in pairs(g_QuestDropItemMgr[ObjName]) do
			for i = 1, table.getn(p) do
				local droptype = p[i].Type
				local dropname = p[i].Object
				if g_QuestNeedMgr[questname] and g_QuestNeedMgr[questname][dropname] then
					local data =
					{
						["sQuestName"] = questname,
						["iItemType"] = droptype,
						["sItemName"] = dropname,
						["uCharId"] = PlayerId
					}
					local RoleQuestDB = RequireDbBox("RoleQuestDB")
					local NeedNum = RoleQuestDB.CheckQuestItemNeedNum(data)
					if NeedNum then
						QuestDrop = true
						break
					end
				else
					local Only = g_ItemInfoMgr:GetItemInfo( droptype, dropname,"Only" )
					local RoomMgr = RequireDbBox("GasRoomMgrDB")
					local havenum = RoomMgr.GetItemCount(PlayerId, droptype, dropname)
					if Only ~= 1 or (Only == 1 and havenum == 0) then
						QuestDrop = true
						break
					end
				end
			end
			if QuestDrop then
				break
			end
		end
	end
	
	if QuestDrop == false and  ObjRandomDropItem_Server(ObjName)==nil  then
		return {false}
	end
	
	local ItemNum = nil
	if data["ItemType"] then
		local RoomMgr = RequireDbBox("GasRoomMgrDB")	
		ItemNum = RoomMgr.GetItemNum(data)
	end
	return {true,ItemNum}
end

function CollObjDB.ClickCollItemObj(data)
	if data["OreLevelLimit"] then
		local LiveSkillBaseDB = RequireDbBox("LiveSkillBaseDB")
		local skillLevel = LiveSkillBaseDB.GetSkillLevelByType(data["nCharId"], "采矿")
		if skillLevel < data["OreLevelLimit"] then
			return {false,{9620,data["OreLevelLimit"]}}
		end
	end
	return ClickCollItemObj(data)
end

local function GetDropItemTbl(PerfectType,PlayerId,TeamIndex)
	local AllRateSum=0
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
	if string.find(TeamIndex, "#class#") then	
		local FightSkillDB = RequireDbBox("FightSkillDB")
		local ClassID = FightSkillDB.Dbs_SelectPlayerClass(PlayerId)
		TeamIndex = string.gsub(TeamIndex, "#class#", ClassNameTbl[ClassID])
	end
	if string.find(TeamIndex, "#camp#") then
		local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
		local Camp = CharacterMediatorDB.GetCamp(PlayerId)
		TeamIndex = string.gsub(TeamIndex, "#camp#", CampNameTbl[Camp])
	end
	local ObjDropItemTeam=ObjDropItemTeam_Server(TeamIndex)
	if ObjDropItemTeam==nil then
		return false
	end
	
	for i=1,g_ObjTeamNum do
		local DropRate="Rate"..i
		local Rate = ObjDropItemTeam(DropRate)
		if Rate == "" or Rate == nil then
			Rate = 0
		else
			Rate = tonumber(Rate)
		end
		AllRateSum = AllRateSum + Rate
	end
	
	local RandomItemRate=math.random(0,AllRateSum)
	local ItemRateIndex=0
	local droprate1=ObjDropItemTeam("Rate1")
	if droprate1 == "" or droprate1 == nil then
		droprate1 = 0
	else
		droprate1 = tonumber(droprate1)
	end
	if RandomItemRate>=0 and RandomItemRate<=droprate1 then
		ItemRateIndex=1
	else
		local droprate1 = 0
		local droprate2 = ObjDropItemTeam("Rate1")
		if droprate2 == nil or droprate2 == "" then
			droprate2 = 0 
		else
			droprate2 = tonumber(droprate2)
		end
		for i=2,g_ObjTeamNum do	
			local AddRate2=ObjDropItemTeam("Rate"..i)
			if AddRate2 == nil or AddRate2 == "" then
				AddRate2 = 0 
			else
				AddRate2 = tonumber(AddRate2)
			end
			
			local AddRate1=ObjDropItemTeam("Rate"..(i-1))
			if AddRate1 == nil or AddRate1 == "" then
				AddRate1 = 0 
			else
				AddRate1 = tonumber(AddRate1)
			end
			
			droprate2 = AddRate2 + droprate2
			droprate1 = AddRate1 + droprate1
			
			if RandomItemRate>droprate1 and RandomItemRate<=droprate2 then
				ItemRateIndex=i
				break
			end
		end
	end
	
	if ItemRateIndex>0 and ItemRateIndex<=g_ObjTeamNum then
		local ItemType=ObjDropItemTeam("ItemType"..ItemRateIndex)
		local ItemNameIndex="ItemName"..ItemRateIndex
		local ItemName=ObjDropItemTeam(ItemNameIndex)
		local ItemNum=math.random(ObjDropItemTeam("MinNum"),ObjDropItemTeam("MaxNum"))
		local GiveNum = ItemNum
		GiveNum = ItemNum*PerfectType
		if ItemType == "Npc" then
			local param = {}					---这几行代码看起来别扭也没办法啊
			param["ItemType"] = "Npc" ---策划提到新需求(超恶心),采集宝箱可能刷个Npc出来
			param["ItemName"] = ItemName
			param["ItemNum"] = ItemNum  ---完美采集时刷出的Npc就不加倍啦，Npc是刷出来捣乱的
			return param
		elseif ItemType == "Obj" then
			local param = {}					---这几行代码看起来别扭也没办法啊
			param["ItemType"] = "Obj" ---策划提到新需求(超恶心),采集宝箱可能刷个Npc出来
			param["ItemName"] = ItemName
			param["ItemNum"] = GiveNum
			return param
		else
			ItemType = tonumber(ItemType)
			local Only = g_ItemInfoMgr:GetItemInfo( ItemType,ItemName,"Only" )
			local GasRoomMgrDB = RequireDbBox("GasRoomMgrDB")
			local havenum = GasRoomMgrDB.GetItemCount(PlayerId,ItemType,ItemName)		
			if Only ~= 1 then
				local param = {}
				param["ItemType"] = ItemType
				param["ItemName"] = ItemName
				param["ItemNum"] = GiveNum
				return param
			elseif havenum == 0 then
				local param = {}
				param["ItemType"] = ItemType
				param["ItemName"] = ItemName
				param["ItemNum"] = 1
				return param
			end
		end
	end
	return false
end

local function GetDropItems(data)
	local PlayerId = data["PlayerId"]
	local ObjName = data["ObjName"]
	local DropItemsTbl = data["DropItemsTbl"]
	local IsPerfectColl = data["IsPerfectColl"]
	local PerfectType = data["PerfectType"]
	local ShowItemsTbl = {}
	
	if IsTable(DropItemsTbl) and table.getn(DropItemsTbl) > 0 and not IsPerfectColl then
		for i = 1, table.getn(DropItemsTbl) do
			local ItemType,ItemName,ItemNum = DropItemsTbl[i][1],DropItemsTbl[i][2],DropItemsTbl[i][3]
			local RoleQuestDB = RequireDbBox("RoleQuestDB")
			local checkres = RoleQuestDB.CheckNeedItemNum(PlayerId, ItemType,ItemName, false)
			if checkres[1] then
				local NeedNum = checkres[2]
				local GiveNum = ItemNum 
				if NeedNum ~= 0 and NeedNum <= ItemNum then
					GiveNum = NeedNum
				else
					GiveNum = ItemNum
				end
				table.insert(ShowItemsTbl,{["ItemType"] = ItemType,["ItemName"] = ItemName,["ItemNum"] = GiveNum})
			end
		end
	else
		if g_QuestDropItemMgr[ObjName] then
			--掉落任务物品
			for questname , p in pairs(g_QuestDropItemMgr[ObjName]) do
				for i = 1, table.getn(p) do
					local droptype = p[i].Type
					local dropname = p[i].Object
					local DropRate = p[i].Rate
					local randomrate = math.random(0,10000)
					if (DropRate*10000) >= randomrate then
						--判断目前任务物品需求是否满足(如果已经满足 就不执行掉落了)
						if g_QuestNeedMgr[questname] and g_QuestNeedMgr[questname][dropname] then
							local data =
							{
								["sQuestName"] = questname,
								["iItemType"] = droptype,
								["sItemName"] = dropname,
								["uCharId"] = PlayerId
							}
							local RoleQuestDB = RequireDbBox("RoleQuestDB")
							local NeedNum = RoleQuestDB.CheckQuestItemNeedNum(data)
							if NeedNum then
								local GiveNum = 1 
								if NeedNum <= PerfectType then
									GiveNum = NeedNum
								else
									GiveNum = PerfectType
								end
								table.insert(ShowItemsTbl,{["ItemType"] = droptype,["ItemName"] = dropname,["ItemNum"] = GiveNum})
							end
						else
							local Only = g_ItemInfoMgr:GetItemInfo( droptype, dropname,"Only" )
							local RoomMgr = RequireDbBox("GasRoomMgrDB")
							local havenum = RoomMgr.GetItemCount(PlayerId, droptype, dropname)
							if Only ~= 1 then
								table.insert(ShowItemsTbl,{["ItemType"] = droptype,["ItemName"] = dropname,["ItemNum"] = PerfectType})
							elseif havenum == 0 then
								table.insert(ShowItemsTbl,{["ItemType"] = droptype,["ItemName"] = dropname,["ItemNum"] = 1})
							end
						end
					end
				end
			end
		end
		
		local CollObjInfo = ObjRandomDropItem_Server(ObjName)
		local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
		local PlayerLevel = CharacterMediatorDB.GetPlayerLevel(PlayerId)
		if CollObjInfo then
			for i=1,g_ObjDropNum do --循环大小依赖策划案
				--local dropLevel = CollObjInfo("DropTeamLevel"..i)
				local dropLevel = GetCfgTransformValue(false, "ObjRandomDropItem_Server", ObjName, "DropTeamLevel"..i)
				if dropLevel == nil or dropLevel == "" or (dropLevel[1] <= PlayerLevel and dropLevel[2] >= PlayerLevel) then
					local TeamRate="TeamRate"..i  --TeamRate1的值表示ObjRandomDropItem_Server表中产生该组物品的几率
					local GetItemTeam="GetItemTeam"..i  --GetItemTeam1的值表示组的Index，对应ObjDropItemTeam_Server的Index
					local Rate=CollObjInfo(TeamRate)
					if Rate~="" and Rate~=0 and Rate ~= nil then
						if math.random(0,10000)<=(Rate*10000) then
							local TeamIndex = CollObjInfo(GetItemTeam) --TeamIndex对应ObjDropItemTeam_Server的Index
							if TeamIndex~="" and TeamIndex~=0 then
								local result = GetDropItemTbl(PerfectType,PlayerId,TeamIndex)
								if result ~= false then
									table.insert(ShowItemsTbl,result)
								end
							end
						end
					end
				end
			end
		end
	end
	return ShowItemsTbl
end

local function ShowItemsToPlayer(data)
	local ObjName = data["ObjName"]
	local DelRes = nil
	local AddExp,SkillInfo = nil
	if data["ItemType"] then
		local RoleQuestDB = RequireDbBox("RoleQuestDB")
		local result = RoleQuestDB.DeleteItem(data)
		if not result[1] then
			return result
		end
		DelRes = result[2]
	end
	if data["AddOreExp"] then
		local GatherLiveSkillDB = RequireDbBox("GatherLiveSkillDB")
		AddExp,SkillInfo = GatherLiveSkillDB.AddExperience(data["PlayerId"], "采矿", data["AddOreExp"])
	end
	
	local ShowItemTbl = GetDropItems(data)
	return {true,ShowItemTbl,DelRes,AddExp,SkillInfo}
end

--采集物品时,进度条读完后,显示物品拾取窗口
function CollObjDB.ShowItemsToPlayer(data)
	return ShowItemsToPlayer(data)
end

local function CollSingleGridItem(data,IsTran)
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
--采集单个格子的物品
function CollObjDB.CollSingleGridItem(data)
	return CollSingleGridItem(data,false)
end

local function CollAllGridItem(data)
	local ItemsTbl = data["ItemsTbl"]
	local PlayerId = data["PlayerId"]
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	local RetItemsTbl = {}
	local removeindex = {}
	local param = {}
	param["char_id"] = PlayerId
	for i=1, table.getn(ItemsTbl) do
		param["nType"]	= ItemsTbl[i][1]
		param["sName"] 	= ItemsTbl[i][2]
		param["nCount"]	= ItemsTbl[i][3]
		param["createType"]	= data["createType"]
		local res1 = CharacterMediatorDB.AddItem(param)
		if not IsNumber(res1) then
			table.insert(removeindex,i)
		end
		local Tbl = {}
		Tbl["RetItemRes"] = res1
		--Tbl["RetQuestVarRes"] = res2
		Tbl["nType"] = param["nType"]
		Tbl["sName"] = param["sName"]
		Tbl["nCount"] = param["nCount"]
		Tbl["Index"] = i
		table.insert(RetItemsTbl,Tbl)
	end
	return RetItemsTbl,removeindex
end
----采集所有格子的物品
function CollObjDB.CollAllGridItem(data)
	return CollAllGridItem(data)
end

SetDbLocalFuncType(CollObjDB.CollSingleGridItem)
SetDbLocalFuncType(CollObjDB.CollAllGridItem)
SetDbLocalFuncType(CollObjDB.ShowItemsToPlayer)
SetDbLocalFuncType(CollObjDB.ClickCollItemObj)
return CollObjDB
