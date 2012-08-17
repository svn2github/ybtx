gac_gas_require "liveskill/LiveSkillMgr"
gac_gas_require "activity/item/FlowerInfoMgr"
cfg_load "item/PickOreItem_Common"

local StmtContainer = {}	
	
local CommonSkillMgr = CLiveSkillMgr:new()
local CultivateFlowerParams = CultivateFlowerParams
local PickOreItem_Common = PickOreItem_Common
local ECamp = ECamp
local EClass = EClass
local FlowerSkillAddHealthPoint = FlowerSkillAddHealthPoint
local FlowerSkillMinusHealthPoint = FlowerSkillMinusHealthPoint
local g_ItemInfoMgr = CItemInfoMgr:new()
local g_FlowerInfoMgr = g_FlowerInfoMgr
local g_ObjDropNum = g_ObjDropNum
local g_ObjTeamNum = g_ObjTeamNum
local GetCfgTransformValue = GetCfgTransformValue
local GetFlowerDispalyName = GetFlowerDispalyName
local ObjDropItemTeam_Server = ObjDropItemTeam_Server
local ObjRandomDropItem_Server = ObjRandomDropItem_Server
local event_type_tbl = event_type_tbl

local GatherLiveSkillDB = CreateDbBox(...)	

local StmtDef = {
	"_AddExperience",
	[[
		update tbl_commerce_skill set cs_uExperience = cs_uExperience + ?  
		where cs_uId = ? and cs_sSkillName = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"_CultivateFlower",
	[[
		insert into tbl_flower set
		cs_uId = ?,
		f_sFlowerName = ?,
		f_dStartTime = now(),
		f_uHealthPoint = ?,
		f_uGetCount = ?,
		f_uLevel = ?,
		f_uEventId = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"_DeleteFlowerInfo",
	[[
		delete from tbl_flower where cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"_QueryFlower",
	[[
		select f_sFlowerName, unix_timestamp(now()) - unix_timestamp(f_dStartTime), unix_timestamp(now()) - unix_timestamp(f_dCoolDownTime),f_uHealthPoint, f_uGetCount from tbl_flower where cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"_QueryFlowerGrowthTime",
	[[
		select f_sFlowerName, unix_timestamp(now()) - unix_timestamp(f_dStartTime), f_uGetCount, f_uHealthPoint, f_uLevel, f_uEventId  from tbl_flower where cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"_QueryFlowerCoolDownTime",
	[[
		select unix_timestamp(now()) - unix_timestamp(f_dCoolDownTime) from tbl_flower where cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"_QueryFlowerHealthPoint",
	[[
		select f_uHealthPoint, f_uMaxHealth, f_uGetCount from tbl_flower where cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"_UpdateFlowerHealthPoint",
	[[
		update tbl_flower set
			f_uHealthPoint = f_uHealthPoint + ?
		where	
			cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"_UpdateFlowerMaxHealth",
	[[
		update tbl_flower set
			f_uMaxHealth = f_uMaxHealth + ?,
			f_uGetCount = f_uGetCount + 1
		where	
			cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"_UpdateFlowerCoolDown",
	[[
		update tbl_flower set
			f_dCoolDownTime = now()
		where	
			cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"_UpdateFlowerGetCount",
	[[
		update tbl_flower set
			f_uGetCount = f_uGetCount - 1
		where	
			cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

local function GetSkillLevel(uCharId,sSkillName)
	local LiveSkillBaseDB = RequireDbBox("LiveSkillBaseDB")
	local DBSkillInfo = LiveSkillBaseDB.GetSkillInfo(uCharId,sSkillName)
	if #DBSkillInfo == 0 then return end
	return DBSkillInfo[2]
end

function GatherLiveSkillDB.GetOreSkillAveLevel(data)
	local TeamSceneMgrDB = RequireDbBox("TeamSceneMgrDB")
	local PlayerTbl = data["PlayerTbl"]
	local Players = 0
	local AllLevel = 0
	
	local res = TeamSceneMgrDB.CheckInFbScene(data)
	if not res then
		return res
	end
	for _, id in pairs(PlayerTbl) do
		local tempLevel = GetSkillLevel(id,"采矿")
		if IsNumber(tempLevel) then
			AllLevel = AllLevel + tempLevel
			Players = Players + 1
		end
	end
	local AveLevel = AllLevel / Players
	return AveLevel
end

function GatherLiveSkillDB.CreateOreAreaScene(data)
	local SceneMgrDB = RequireDbBox("SceneMgrDB")
	local TeamSceneMgrDB = RequireDbBox("TeamSceneMgrDB")
	local OreMapItemDB = RequireDbBox("OreMapItemDB")
	--删除矿藏图
	local res
	if data["DelTbl"] then
		res = OreMapItemDB.DeleteOreMapByID(data["DelTbl"])
		if not res then
			return
		end
	end
	
	local teamId
	if data["TeamId"] then
		teamId = data["TeamId"]
	end
	local sceneId, serverId = SceneMgrDB._CreateScene(data)
	if not sceneId then --场景失败
		CancelTran()
		return
	end
	if teamId then
		TeamSceneMgrDB.OnCreateScene(sceneId, teamId)
	end
	return sceneId, serverId, res
end

function GatherLiveSkillDB.PickOreStart(data)
	local CharId = data["PlayerID"]
	local needLevel = data["needLevel"]
	local LiveSkillBaseDB = RequireDbBox("LiveSkillBaseDB")
	local nowLevel = LiveSkillBaseDB.GetSkillLevelByType(CharId,"采矿")
	if needLevel > nowLevel then
		return {false, {9620, needLevel}}
	end
	local PickOreItemDB = RequireDbBox("PickOreItemDB")
	local res = PickOreItemDB.QueryPickOreItemInfo(data)
	if not res then
		return {false}
	end
	if res[2] == 0 then
		return {false, {9626}}
	end
	return {true, res[1]}
end

local function GetDropItemTbl(PlayerId,TeamIndex)
	local AllRateSum=0
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
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
	local RateIndex=0
	
	local droprate1=ObjDropItemTeam("Rate1")
	if droprate1 == "" or droprate1 == nil then
		droprate1 = 0
	else
		droprate1 = tonumber(droprate1)
	end
	if RandomItemRate>=0 and RandomItemRate<=droprate1 then
		RateIndex=1
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
				RateIndex=i
				break
			end
		end
	end
	
	if RateIndex>0 and RateIndex<=g_ObjTeamNum then
		local ItemType=ObjDropItemTeam("ItemType"..RateIndex)
		ItemType = tonumber(ItemType)
		local ItemNameIndex="ItemName"..RateIndex
		local ItemName=ObjDropItemTeam(ItemNameIndex)
		local ItemNum=math.random(ObjDropItemTeam("MinNum"),ObjDropItemTeam("MaxNum"))
		if IsNumber(ItemType) then
			local Only = g_ItemInfoMgr:GetItemInfo(ItemType,ItemName,"Only")
			local havenum = g_RoomMgr.GetItemCount(PlayerId,ItemType,ItemName)
			if Only == 1 then
				ItemNum = 1
			end
			if Only ~= 1 or (Only == 1 and havenum == 0) then
				return {ItemType,ItemName,ItemNum}
			end
		end
	end
	return false
end

local function PickOreDrop(ItemName, PlayerId)
	local DropItemTbl = {}
	local DropItemInfo = ObjRandomDropItem_Server(ItemName)
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	local PlayerLevel = CharacterMediatorDB.GetPlayerLevel(PlayerId)
	if DropItemInfo then
		for i=1,g_ObjDropNum do --循环大小依赖策划案
			local dropLevel = GetCfgTransformValue(false, "ObjRandomDropItem_Server", ItemName, "DropTeamLevel"..i)
			if dropLevel == nil or dropLevel == "" or (dropLevel[1] <= PlayerLevel and dropLevel[2] >= PlayerLevel) then
				local TeamRate="TeamRate"..i  --TeamRate1的值表示ObjRandomDropItem_Server表中产生该组物品的几率
				local GetItemTeam="GetItemTeam"..i  --GetItemTeam1的值表示组的Index，对应ObjDropItemTeam_Server的Index
				local Rate=DropItemInfo(TeamRate)
				if Rate~="" and Rate~=0 and Rate ~= nil then
					if math.random(0,10000)<=(Rate*10000) then
						local TeamIndex=DropItemInfo(GetItemTeam) --TeamIndex对应ObjDropItemTeam_Server的Index
						if TeamIndex~="" and TeamIndex~=0 then
							local result = GetDropItemTbl(PlayerId,TeamIndex)
							if result ~= false then
								table.insert(DropItemTbl,result)
							end
						end
					end
				end
			end
		end
	end
	return DropItemTbl
end

function GatherLiveSkillDB.PickOre(data)
	local CharId = data["PlayerID"]
	local ItemId = data["ItemId"]
	local ItemName = data["ItemName"]

	local PickOreItemDB = RequireDbBox("PickOreItemDB")
	local DuraResult, DuraTbl = PickOreItemDB.UpdateDuraValue(data)
	if not DuraResult then
		return {}
	end
	
	local ExpAddNum = PickOreItem_Common(ItemName,"AddSkillExp")
	local Exp, Info = GatherLiveSkillDB.AddExperience(CharId, "采矿", ExpAddNum)
	
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	local ItemsTbl = PickOreDrop(ItemName, PlayerId)
	local AddItemsTbl = {}
	if ItemsTbl ~= nil then
		local param = {}
		param["char_id"] = CharId
		for i=1, table.getn(ItemsTbl) do
			param["nType"]	= ItemsTbl[i][1]
			param["sName"] 	= ItemsTbl[i][2]
			param["nCount"]	= ItemsTbl[i][3]
			param["createType"]	= event_type_tbl["挂机采矿产出"]
			local res1 = CharacterMediatorDB.AddItem(param)
			if IsNumber(res1) then
				CancelTran()
				if res1 == 3 then
					return {nil,nil,9630}
				end
				return {}
			end
			local Tbl = {}
			Tbl["AddItemRes"] = res1
			Tbl["nType"] = param["nType"]
			Tbl["sName"] = param["sName"]
			Tbl["nCount"] = param["nCount"]
			Tbl["Index"] = i
			table.insert(AddItemsTbl,Tbl)
		end
	end
	local CurOffLineExp
	if data["uOffLineExpValue"] and data["uOffLineExpValue"] ~= 0 then
		local param = {}
		param["char_id"] = CharId
		param["uOffLineExpValue"] = data["uOffLineExpValue"]
		CharacterMediatorDB.AddOffLineExpValue(param)
		CurOffLineExp = CharacterMediatorDB.GetOffLineExp(CharId)
	end
	
	return {DuraTbl,{Exp,Info},AddItemsTbl,CurOffLineExp}
end

function GatherLiveSkillDB.AddExperience(uCharId, sSkillName, ExpAddNum)

	local LiveSkillBaseDB = RequireDbBox("LiveSkillBaseDB")
	local DBSkillInfo = LiveSkillBaseDB.GetSkillInfo(uCharId,sSkillName)
	if #DBSkillInfo == 0 then return end
	local nNowLevel,nNowExp = DBSkillInfo[2],DBSkillInfo[3]
	
	--获得经验上限
	local nMaxExp = CommonSkillMgr:GetSkillInfoByName(sSkillName,nNowLevel+1,"ExperienceNeed") or CommonSkillMgr:GetSkillInfoByName(sSkillName,nNowLevel,"ExperienceNeed")

	--增加X点经验
	--经验达到可升级经验就不再长，升级后才可继续长经验
	if nNowExp < nMaxExp then
		if (nNowExp + ExpAddNum) > nMaxExp then
			ExpAddNum = nMaxExp - nNowExp
		end
		StmtContainer._AddExperience:ExecSql("", ExpAddNum, uCharId, sSkillName)
		if g_DbChannelMgr:LastAffectedRowNum() > 0 then
			return ExpAddNum,LiveSkillBaseDB.GetAllSkillsInfo(uCharId)
		end
	else
		return 0
	end
end

function GatherLiveSkillDB.AddLiveSkillExp(uCharId,sSkillName,uLevel,multiple)
	--根据等级获得提升的经验和熟练度
	local uSpecialAddNum,ExpAddNum = CommonSkillMgr:GetSpecialAndExpBySkillLevel(sSkillName,uLevel)
	ExpAddNum = ExpAddNum * (multiple or 1)
	
	return GatherLiveSkillDB.AddExperience(uCharId,sSkillName,ExpAddNum)
end

function GatherLiveSkillDB.AddOreMonsterExp(uCharId)
	return GatherLiveSkillDB.AddLiveSkillExp(uCharId, "采矿", 1)
end

function GatherLiveSkillDB.DeleteFlowerInfo(uCharId)
	StmtContainer._DeleteFlowerInfo:ExecStat(uCharId)
end

function GatherLiveSkillDB.UpdateFlowerGetCount(uCharId)
	StmtContainer._UpdateFlowerGetCount:ExecStat(uCharId)
end

local function GetRankByGrowthTime(FlowerInfo, GrowthTime)
	local rank = 1
	local Arg = FlowerInfo[rank]
	while IsNumber(Arg) do
		if (GrowthTime - Arg) <= 0 then
			return rank
		else
			GrowthTime = GrowthTime - Arg
		end
		rank = rank + 1
		Arg = FlowerInfo[rank]
	end
end

--收获物品表
local function GetAllItemTbl(allItemTbl,GetTbl,Rate,Level)
	if GetTbl == nil or GetTbl == "" then
		return allItemTbl
	end
	for i, tbl in pairs(GetTbl) do
		if tbl[5] ~= nil and tbl[5] ~= "" then
			local n = math.random(100)
			if n > tbl[5](Rate,Level) then
				return allItemTbl
			end
		end
		table.insert(allItemTbl,{tbl[1],tbl[2],math.random(tbl[3],tbl[4])})
	end
	return allItemTbl
end

function GatherLiveSkillDB.GetItemAndAddLiveSkillExp(data)
	local AddItemsTbl = {}
	local uCharId 	= data["PlayerId"]
	local IsPerish	= data["IsPerish"]
	local AddExp,SkillInfo = nil
	local ItemsTbl = {}
	local FlowerName, GrowthTime, GetCount, HealthPoint, uLevel, EventId = GatherLiveSkillDB.QueryFlowerGrowthTime(uCharId)
	local SkillLevel = GetSkillLevel(uCharId,"花卉")
	if not FlowerName then
		return IsPerish, false, 9607																						--没有种花
	end
	local FlowerInfo = g_FlowerInfoMgr:GetFlowerInfo(FlowerName)
	if IsPerish or GrowthTime >= FlowerInfo["总生长时间"] then																			--枯萎
		--print(GrowthTime,FlowerInfo["总生长时间"])
		if GrowthTime < FlowerInfo["总生长时间"] then
			return true, GrowthTime
		end
		GatherLiveSkillDB.DeleteFlowerInfo(uCharId)
		ItemsTbl = GetAllItemTbl(ItemsTbl, FlowerInfo["枯萎收获"],HealthPoint,SkillLevel)
		if table.maxn(ItemsTbl) == 0 then		--没有枯萎收获
			return true
		end
	else																									--收获
		local Rank = GetRankByGrowthTime(FlowerInfo["生长时间"], GrowthTime)
		
		local Normal = FlowerInfo["常规产出"][Rank]
		local Special = FlowerInfo["特殊产出"][Rank]
		if table.maxn(Normal) == 0 and table.maxn(Special) == 0 then
			return false, false, 9607
		end
		ItemsTbl = GetAllItemTbl(ItemsTbl,Normal,HealthPoint,SkillLevel)
		ItemsTbl = GetAllItemTbl(ItemsTbl,Special,HealthPoint,SkillLevel)
		
		--AddExp,SkillInfo = GatherLiveSkillDB.AddLiveSkillExp(uCharId, "花卉", uLevel, data["multiple"])
		
		if GetCount <= 0 then
			GatherLiveSkillDB.DeleteFlowerInfo(uCharId)	
			return false, false, true
		elseif GetCount == 1 then
			GatherLiveSkillDB.DeleteFlowerInfo(uCharId)
			AddItemsTbl["IsDead"] = true
			GetCount = nil
		else
			GatherLiveSkillDB.UpdateFlowerGetCount(uCharId, -1)
			GetCount = GetCount - 1
		end
	end
	
	if ItemsTbl ~= nil then
		local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
		local param = {}
		param["char_id"] = uCharId
		for i=1, table.getn(ItemsTbl) do
			param["nType"]	= ItemsTbl[i][1]
			param["sName"] 	= ItemsTbl[i][2]
			param["nCount"]	= ItemsTbl[i][3]
			param["uEventId"] = EventId
			local res1 = CharacterMediatorDB.AddItem(param)
			if IsNumber(res1) then
				CancelTran()
				if not IsPerish then
					if res1 == 3 then
						return IsPerish,false,9621
					end
				end
				if res1 == 3 then
					local MailData = {}
					MailData["PlayerId"]    = uCharId
					MailData["MailTitle"]		= 1015 .. "_\"" .. GetFlowerDispalyName(FlowerName) .. "\""
					MailData["MailContent"]	= 1016
					GatherLiveSkillDB.SendMailGiveGatherItem(MailData)
					return IsPerish,false,9625
				end
				return IsPerish,false
			end
			local Tbl = {}
			Tbl["AddItemRes"] = res1
			--Tbl["AddQuestVarRes"] = res2
			Tbl["nType"] = param["nType"]
			Tbl["sName"] = param["sName"]
			Tbl["nCount"] = param["nCount"]
			Tbl["Index"] = i
			table.insert(AddItemsTbl,Tbl)
		end
	end
	return IsPerish,true,AddItemsTbl,AddExp,SkillInfo,GetCount
end

function GatherLiveSkillDB.SendMailGiveGatherItem(data)

	local AddItemsTbl = {}
	local ItemsTbl	= {}
	local uCharId 	= data["PlayerId"]
	local MailTitle = data["MailTitle"]
	local MailContent = data["MailContent"]
	
	local FlowerName, GrowthTime, GetCount, HealthPoint, uLevel, EventId = GatherLiveSkillDB.QueryFlowerGrowthTime(uCharId)
	local SkillLevel = GetSkillLevel(uCharId,"花卉")
	if not FlowerName then
		return false							--没有种花
	end
	local FlowerInfo = g_FlowerInfoMgr:GetFlowerInfo(FlowerName)
	
	if GrowthTime < FlowerInfo["总生长时间"] then
		return GrowthTime
	end
	GatherLiveSkillDB.DeleteFlowerInfo(uCharId)
	local ItemsTbl = GetAllItemTbl(ItemsTbl, FlowerInfo["枯萎收获"],HealthPoint,SkillLevel)
	if table.maxn(ItemsTbl) == 0 then		--没有枯萎收获
		return
	end
	
	if ItemsTbl ~= nil then
		local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
		local sysMailExecutor = RequireDbBox("SysMailDB")
		local AddItemsTbl = {}
		for i=1, table.getn(ItemsTbl) do
			local BigID	= ItemsTbl[i][1]
			local ItemName 	= ItemsTbl[i][2]
			local Count	= ItemsTbl[i][3]
			local params= {}
			params.m_nType = BigID
			params.m_sName = ItemName
			params.m_nBindingType = g_ItemInfoMgr:GetItemInfo(BigID,ItemName,"BindingStyle") or 0
			params.m_nCharID = uCharId
			params.m_nEventID = EventId
			for j=1, Count do
				local itemID = g_RoomMgr.CreateItem(params)
				if not itemID then
					CancelTran()
					return 
				end
				table.insert(AddItemsTbl,{itemID})
			end
		end
		local sendRet= sysMailExecutor.SendSysMail(1014,uCharId, MailTitle, MailContent, AddItemsTbl, 0,event_type_tbl["种花系统邮件"])
		if sendRet == false then
			CancelTran()
			return false
		end
	end
end

function GatherLiveSkillDB.DelItem(data)
	local result = {}
	local uCharId = data["PlayerID"]
	local ItemRoomIndex = data["RoomIndex"]
	local ItemPos = data["Pos"]
	local ItemName = data["ItemName"]
	local ItemNum = data["ItemNum"]
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	
	local itemType,itemName,_ = g_RoomMgr.GetOneItemByPosition(uCharId,ItemRoomIndex,ItemPos)
	if itemType ~= 32 or ItemName ~= itemName then
		return false
	end
	
	local DelRet = g_RoomMgr.DelItemByPos(uCharId,ItemRoomIndex,ItemPos,ItemNum,event_type_tbl["物品使用"])
	if IsNumber(DelRet) then
		return false
	end
	DelRet.m_nRoomIndex = ItemRoomIndex
	DelRet.m_nPos = ItemPos
	
	table.insert(result,DelRet)
	return result
end

function GatherLiveSkillDB.QueryFlowerInfo(uCharId)
	local result = StmtContainer._QueryFlower:ExecStat(uCharId)
	local tbl = nil
	if result:GetRowNum() > 0 then
		tbl = {}
		tbl["flowerName"] 	= result:GetData(0,0)
		tbl["GrowthTime"] 	= result:GetData(0,1)
		tbl["CoolDownTime"] = result:GetData(0,2)
		tbl["HealthPoint"] 	= result:GetData(0,3)
		tbl["GetCount"] 		= result:GetData(0,4)
		if tbl["CoolDownTime"] then
			tbl["CoolDownTime"] = CultivateFlowerParams["SkillCooldownTime"] - tbl["CoolDownTime"]
			if tbl["CoolDownTime"] < 0 then
				tbl["CoolDownTime"] = 0
			end
		end
		if tbl["GrowthTime"] > g_FlowerInfoMgr:GetFlowerInfo(tbl["flowerName"],"总生长时间") then
			local data = {}
			data["PlayerId"]    = uCharId
			data["MailTitle"]		= 1015 .. "_\"" .. GetFlowerDispalyName(tbl["flowerName"]) .. "\""
			data["MailContent"]	= 1016
			GatherLiveSkillDB.SendMailGiveGatherItem(data)
			return
		end
	end
	return tbl
end

function GatherLiveSkillDB.CultivateFlower(data)
	local uCharId = data["PlayerID"]
	local flowerName = data["flowerName"]
	local needLevel = data["needLevel"]
	local LiveSkillBaseDB = RequireDbBox("LiveSkillBaseDB")
	local nowLevel = LiveSkillBaseDB.GetSkillLevelByType(uCharId,"花卉")
	local FlowerInfo = g_FlowerInfoMgr:GetFlowerInfo(flowerName)
	
	local HealthPoint = CultivateFlowerParams["HealthPoint"]
	local GetCount = FlowerInfo["收获次数"]
	if needLevel > nowLevel then
		return {false, {9617, flowerName}}
	end
	
	local res = StmtContainer._QueryFlower:ExecStat(uCharId)
	if res:GetRowNum() > 0 then
		return {false, {9616}}
	end
	local DelRes = GatherLiveSkillDB.DelItem(data)
	if not DelRes then
		return {false, {9601}}
	end
	local AddExp,SkillInfo = GatherLiveSkillDB.AddLiveSkillExp(uCharId, "花卉", needLevel, data["multiple"])
	local LogMgr = RequireDbBox("LogMgrDB")
	local EventId = LogMgr.CultivateFlowersLog()
	StmtContainer._CultivateFlower:ExecStat(uCharId, flowerName, HealthPoint, GetCount, needLevel, EventId)
	if g_DbChannelMgr:LastAffectedRowNum() <= 0 then
		CancelTran()
	end
	return {true,DelRes,AddExp,SkillInfo}
end

function GatherLiveSkillDB.QueryFlowerGrowthTime(uCharId)
	local result = StmtContainer._QueryFlowerGrowthTime:ExecStat(uCharId)
	if result:GetRowNum() > 0 then
		return result:GetData(0,0), result:GetData(0,1), result:GetData(0,2), result:GetData(0,3), result:GetData(0,4), result:GetData(0,5)
	end
end

function GatherLiveSkillDB.UpdateFlowerCoolDown(uCharId)
	local result = StmtContainer._QueryFlowerCoolDownTime:ExecStat(uCharId)
	if result:GetRowNum() > 0 then
		local leftTime = result:GetData(0,0)
		if leftTime == nil then
			StmtContainer._UpdateFlowerCoolDown:ExecStat(uCharId)
			if g_DbChannelMgr:LastAffectedRowNum() > 0 then
				return true
			end
		end
			
		if leftTime >= CultivateFlowerParams["SkillCooldownTime"] then
			StmtContainer._UpdateFlowerCoolDown:ExecStat(uCharId)
			if g_DbChannelMgr:LastAffectedRowNum() > 0 then
				return true
			end
		else
			return CultivateFlowerParams["SkillCooldownTime"] - leftTime
		end
	end
end

function GatherLiveSkillDB.UpdateFlowerMaxHealth(uCharId)
	local res = StmtContainer._QueryFlowerHealthPoint:ExecStat(uCharId)
	if res:GetRowNum() > 0 then
		local nowHealth = res:GetData(0,0)
		local maxHealth = res:GetData(0,1)
		local GetCount = res:GetData(0,2)
		if nowHealth >= maxHealth + CultivateFlowerParams["HealthPointStep"] then
			StmtContainer._UpdateFlowerMaxHealth:ExecStat(CultivateFlowerParams["HealthPointStep"], uCharId)
			if g_DbChannelMgr:LastAffectedRowNum() > 0 then
				return GetCount + 1
			end
		end
	end
end

function GatherLiveSkillDB.UpdateFlowerHealthPoint(data)
	local uCharId = data["PlayerID"]
	local HealthPoint = data["HealthPoint"]
	local IsSkill = data["IsSkill"]
	if IsSkill then
		local SkillRes = GatherLiveSkillDB.UpdateFlowerCoolDown(uCharId)
		if IsNumber(SkillRes) or SkillRes == nil then
			return false, SkillRes
		end
	end

	if HealthPoint == nil then
		if data["SkillType"] then
			HealthPoint = FlowerSkillAddHealthPoint[data["SkillType"]]
		elseif data["StateType"] then
			HealthPoint = FlowerSkillMinusHealthPoint[data["StateType"]]
		end
	end
	
	local res = StmtContainer._QueryFlowerHealthPoint:ExecStat(uCharId)
	if res:GetRowNum() > 0 then
		local nowHealth = res:GetData(0,0)
		local resHealth = nowHealth + HealthPoint
		if resHealth < 0 then
			HealthPoint = HealthPoint - resHealth
		end
		if HealthPoint == 0 then
			return true, HealthPoint
		end
		
		StmtContainer._UpdateFlowerHealthPoint:ExecStat(HealthPoint, uCharId)
		if g_DbChannelMgr:LastAffectedRowNum() > 0 then
			local IsAddCount = GatherLiveSkillDB.UpdateFlowerMaxHealth(uCharId)
			return true, HealthPoint, IsAddCount
		end
	end
	CancelTran()
end


function GatherLiveSkillDB.DelMatchGameItem(data)
	local result = {}
	local uCharId = data["PlayerID"]
	local ItemRoomIndex = data["RoomIndex"]
	local ItemPos = data["Pos"]
	local itemName = data["ItemName"]
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local ItemType,ItemName,ItemId = g_RoomMgr.GetOneItemByPosition(uCharId,ItemRoomIndex,ItemPos)
	if not (ItemType == 46) or not (ItemName == itemName) then
		return 
	end
	local DelRet = g_RoomMgr.DelItemByPos(uCharId,ItemRoomIndex,ItemPos,1,event_type_tbl["物品使用"])
	if IsNumber(DelRet) then
		return false
	end
	DelRet.m_nRoomIndex = ItemRoomIndex
	DelRet.m_nPos = ItemPos
	
	table.insert(result,DelRet)
	return result
end

SetDbLocalFuncType(GatherLiveSkillDB.PickOreStart)
SetDbLocalFuncType(GatherLiveSkillDB.PickOre)
SetDbLocalFuncType(GatherLiveSkillDB.GetItemAndAddLiveSkillExp)
SetDbLocalFuncType(GatherLiveSkillDB.SendMailGiveGatherItem)
SetDbLocalFuncType(GatherLiveSkillDB.CultivateFlower)
SetDbLocalFuncType(GatherLiveSkillDB.UpdateFlowerHealthPoint)
SetDbLocalFuncType(GatherLiveSkillDB.DelMatchGameItem)

return GatherLiveSkillDB
