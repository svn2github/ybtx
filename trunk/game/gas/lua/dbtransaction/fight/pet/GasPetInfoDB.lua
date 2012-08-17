cfg_load "servant/Pet_Common"
cfg_load "servant/Pet_Skill_Common"

local g_ItemInfoMgr = CItemInfoMgr:new()
local CGasPetInfoSql = class()
local os = os
local Pet_Common  = Pet_Common
local Pet_Skill_Common = Pet_Skill_Common
local CStaticAttribMgr_CalcBasicMaxHealthPoint = CStaticAttribMgr_CalcBasicMaxHealthPoint
local event_type_tbl = event_type_tbl

local CGasPetInfoDB = CreateDbBox(...)

--------------------------------------------------------------------------   
local StmtDef = {
	"_SelectCharPetInfo",
	[[
		select 
			cp.ps_uId,cp.cp_sPetName,ps.ps_sPetName,ps.ps_sPetType,cp.cp_uPetLevel,ps.ps_uAptitudeValue,cp.cp_uHealthPoint,cp.cp_uDamageValue,cp.cp_uExpValue,cp.cp_uVimValue
		from 
			tbl_char_pet cp,tbl_pet_static ps
		where 
			cp.ps_uId = ps.ps_uId and cp.cs_uId = ? order by cp.cp_dtHoldTime
	]]
}
DefineSql(StmtDef,CGasPetInfoSql)

function CGasPetInfoDB.InitCharPetInfo(charId)
	local petRes = CGasPetInfoSql._SelectCharPetInfo:ExecStat(charId)
	return petRes
end
-------------------------------------------------------------------------- 
local StmtDef = {
	"_SelectCharPetEggInfo",
	[[
		select 
			pe_sEggName
		from 
			tbl_pet_egg
		where 
			cs_uId = ?
	]]
}
DefineSql(StmtDef,CGasPetInfoSql)

function CGasPetInfoDB.InitCharPetEggInfo(charId)
	local petEggRes = CGasPetInfoSql._SelectCharPetEggInfo:ExecSql("s",charId)
	local petEggTbl = {}
	if petEggRes:GetRowNum() > 0 then
		local petEggName = petEggRes:GetData(0,0)
		
		local petEggItem = g_ItemInfoMgr:GetItemFunInfo(41, petEggName)
		if not petEggItem then
			return petEggTbl
		end
		local petName = petEggItem("PetName")
		local petType = Pet_Common(petName, "PetType")
		table.insert(petEggTbl,{petEggName,petType})
	end
	return petEggTbl
end
--------------------------------------------------------------------------
local StmtDef = {
	"_SaveIncubatePetEggInfo",
	[[
		insert into tbl_pet_egg(cs_uId,pe_uIncubateTimes,pe_sEggName,pe_uAptitudeValue,pe_uTotalIncubateTimes) values(?,?,?,?,?)
	]]
}
DefineSql(StmtDef, CGasPetInfoSql)

local StmtDef = {
	"_SelectCharIncubatePetEggInfo",
	[[
		select pe_uIncubateTimes,pe_uTotalIncubateTimes,pe_uAptitudeValue from tbl_pet_egg where cs_uId = ?
	]]
}
DefineSql(StmtDef, CGasPetInfoSql)

--@brief 幻兽蛋孵化
function CGasPetInfoDB.IncubatePetEgg(data)
	local charId = data["charId"]
	local ItemName = data["ItemName"]
	local nRoomIndex = data["nRoomIndex"]
	local nPos = data["nPos"]
	local petEggItem = g_ItemInfoMgr:GetItemFunInfo(41, ItemName)
	
	if not petEggItem then
		return false
	end
	
	local incubatePetEggRes =  CGasPetInfoSql._SelectCharIncubatePetEggInfo:ExecSql("nnn",charId)
	if incubatePetEggRes and incubatePetEggRes:GetRowNum() > 0 then
		return false,194105
	end
	
	local leastAptitude = tonumber(petEggItem("LeastAptitude"))
	local mostAptitude = tonumber(petEggItem("MostAptitude"))
	local aptitudeValue = math.random(leastAptitude, mostAptitude)
	
	local totalIncubateTimes = petEggItem("LowIncubateTimes") + aptitudeValue/petEggItem("IncubateParam")
	totalIncubateTimes = string.format("%.0f",totalIncubateTimes)
	
	CGasPetInfoSql._SaveIncubatePetEggInfo:ExecSql("",charId,0,ItemName,aptitudeValue,totalIncubateTimes)
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local petEggRes = g_RoomMgr.GetAllItemFromPosition(charId, nRoomIndex, nPos)
	if petEggRes:GetRowNum() == 0  then
		return false
	end
	
	local petName = petEggItem("PetName")
	local petItem = Pet_Common(petName)
	if not petItem then
		return false
	end
	local DelRet = g_RoomMgr.DelItemByPos(charId,nRoomIndex,nPos,1,event_type_tbl["物品使用"])
	if IsNumber(DelRet) then
		return false
	end

	return true,DelRet[1],petItem("PetType")
end
--------------------------------------------------------------------------
local StmtDef = {
	"_SelectIncubatePetEggTimes",
	[[
		select pe_uIncubateTimes,pe_uTotalIncubateTimes,pe_uAptitudeValue,unix_timestamp(pe_dtIncubateDate) from tbl_pet_egg where cs_uId = ? and pe_sEggName = ?
	]]
}
DefineSql(StmtDef, CGasPetInfoSql)

--查看玩家身上还剩下多少魂值
local StmtDef = {
	"_SelectPlayerSoulNum",
	[[ select cs_uSoulNum from tbl_char_soul where cs_uId = ?]] 
}
DefineSql(StmtDef, CGasPetInfoSql)

local StmtDef = {
	"_DeleteIncubatePetEggInfo",
	[[
		delete from tbl_pet_egg where cs_uId = ? and pe_sEggName = ?
	]]
}
DefineSql(StmtDef, CGasPetInfoSql)

local StmtDef = {
	"_SavePetStaticInfo",
	[[
		insert into tbl_pet_static(ps_sPetName,ps_uAptitudeValue,ps_sPetType) values(?,?,?)
	]]
}
DefineSql(StmtDef, CGasPetInfoSql)

local StmtDef = {
	"_SaveCharPetInfo",
	[[
		insert into tbl_char_pet(cs_uId,ps_uId,cp_sPetName,cp_uPetLevel,cp_dtHoldTime,cp_uHealthPoint,cp_uDamageValue,cp_uExpValue,cp_uVimValue) values(?,?,?,?,now(),?,?,?,?)
	]]
}
DefineSql(StmtDef, CGasPetInfoSql)

local StmtDef = {
	"_UpdateIncubatePetEggTimes",
	[[
		update tbl_pet_egg set pe_uIncubateTimes = ?,pe_dtIncubateDate = now() where cs_uId = ? and pe_sEggName = ?
	]]
}
DefineSql(StmtDef, CGasPetInfoSql)
 
local StmtDef = {
	"_SavePetSkill",
	[[
		insert into tbl_pet_skill(cs_uId,ps_uId,ps_sSkillName,ps_uIndex) values(?,?,?,?)
	]]
}
DefineSql(StmtDef, CGasPetInfoSql)

--@brief 根据随机计算幻兽学会的技能
--@param petName:幻兽名称
local function GetSkillName(petName)
	local skillIndexTbl1 = {}
	local skillIndexTbl2 = {}
	local skillName1,skillName2
	local skillType1 = Pet_Common(petName, "SkillType1")
	local skillType2 = Pet_Common(petName, "SkillType2")
	local skillType = math.random(1,2) 
	local skillIndex = math.random(1,2) 
	for i, v in ipairs( Pet_Skill_Common:GetKeys() ) do
		local info = Pet_Skill_Common(v)
		if skillType == 1 then
			if skillIndex == 1 then
				if info("SkillType") == skillType1 then
					table.insert(skillIndexTbl1, v)
				end
			else
				if info("SkillType") == skillType2 then
					table.insert(skillIndexTbl1, v)
				end
			end
		else
			if info("SkillType") == skillType1 then
				table.insert(skillIndexTbl1, v)
			end
			if info("SkillType") == skillType2 then
				table.insert(skillIndexTbl2, v)
			end
		end
	end
	if #skillIndexTbl1 > 0 then
		local index = math.random(1,#skillIndexTbl1)
		skillName1 = skillIndexTbl1[index]
	end
	if #skillIndexTbl2 > 0 then
		local index = math.random(1,#skillIndexTbl2)
		skillName2 = skillIndexTbl2[index]
	end
	return skillName1,skillName2
end
	
--@brief 幻兽蛋注魂孵化成幻兽
function CGasPetInfoDB.PetEggPourSoul(data)
	local charId = data["charId"]
	local petEggName = data["petEggName"]
	local costSoulValue = data["costSoulValue"]
	local total_soul = 0
	local incubatePetEggTimes = 0
	local total_times = 0
	local last_incubateTime = 0
	local petHealthPoint = data["petHealthPoint"]
	local petDamageValue = data["petDamageValue"]
	
	local incubatePetEggRes =  CGasPetInfoSql._SelectIncubatePetEggTimes:ExecSql("nnnn",charId,petEggName)
	if incubatePetEggRes and incubatePetEggRes:GetRowNum() > 0 then
		incubatePetEggTimes = incubatePetEggRes:GetData(0,0)
		total_times = incubatePetEggRes:GetData(0,1)
		last_incubateTime = incubatePetEggRes:GetData(0,3)
	else
		return false,194104
	end
	
	if last_incubateTime and last_incubateTime ~= 0 then
		local leftCDTime = math.floor(tonumber(os.time()- last_incubateTime)/60)
		if leftCDTime >= 0 and leftCDTime < 2 then
			return false,194106,2 - leftCDTime,incubatePetEggTimes + 1
		end
	end
	local soul_set = CGasPetInfoSql._SelectPlayerSoulNum:ExecSql("n", charId)
	if soul_set and soul_set:GetRowNum() > 0 then
		total_soul = soul_set:GetData(0,0)
	end
	
	if costSoulValue > total_soul then
		return false,194101
	end
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	CharacterMediatorDB.UpdateCharSoul(charId,-costSoulValue)
	local cur_incubatePetEggTimes = incubatePetEggTimes + 1
	local soulres = CGasPetInfoSql._SelectPlayerSoulNum:ExecSql("n", charId)
	local soul_value = 0
	if soulres and soulres:GetRowNum() > 0 then
		soul_value = soulres:GetData(0,0)
	end
	
	if cur_incubatePetEggTimes == total_times then
		local petEggItem = g_ItemInfoMgr:GetItemFunInfo(41, petEggName)
		if not petEggItem then
			return false,194104
		end
		
		local petName = petEggItem("PetName")
		local petVimValue = Pet_Common(petName, "VimValue")
		local aptitudeValue = incubatePetEggRes:GetData(0,2)
		CGasPetInfoSql._DeleteIncubatePetEggInfo:ExecSql("", charId,petEggName)
		local petType = Pet_Common(petName, "PetType")
		CGasPetInfoSql._SavePetStaticInfo:ExecSql("", petName,aptitudeValue,petType)
		
		if g_DbChannelMgr:LastAffectedRowNum() ~= 1 then
			CancelTran()
			return false,194104
		end
		--获得幻兽id
		local petId = g_DbChannelMgr:LastInsertId()
		CGasPetInfoSql._SaveCharPetInfo:ExecSql("", charId,petId,nil,1,petHealthPoint,petDamageValue,0,petVimValue)
		if g_DbChannelMgr:LastAffectedRowNum() ~= 1 then
			CancelTran()
			return false,194104
		end
		
		local skillName1,skillName2 = GetSkillName(petName)
		CGasPetInfoSql._SavePetSkill:ExecSql("",charId,petId,skillName1,1)
		if skillName2 then
			CGasPetInfoSql._SavePetSkill:ExecSql("",charId,petId,skillName2,2)
		end
		return true,194103,soul_value,petId,petName,petEggName,aptitudeValue,petType,petHealthPoint,petDamageValue,0,petVimValue,skillName1,skillName2
	else
		CGasPetInfoSql._UpdateIncubatePetEggTimes:ExecSql("", cur_incubatePetEggTimes,charId,petEggName)
		return false,194102,soul_value
	end
end
-----------------------------------------------------------------------------------------
local StmtDef = {
	"_DeletePetInfo",
	[[
		delete from tbl_pet_static where ps_uId = ?
	]]
}
DefineSql(StmtDef, CGasPetInfoSql)

local StmtDef = {
	"_CheckCharPetInfo",
	[[
		select 
			count(*)
		from 
			tbl_char_pet
		where 
			cs_uId = ? and ps_uId = ?
	]]
}
DefineSql(StmtDef,CGasPetInfoSql)

local StmtDef = {
	"_SelectPreparePetByPetId",
	[[
		select count(*) from tbl_pet_prepare where ps_uId = ?
	]]
}
DefineSql(StmtDef, CGasPetInfoSql)  

--@brief 幻兽放生
function CGasPetInfoDB.ThrowAwayPet(data)
	local charId = data["charId"]
	local petId = data["petId"]
	local bIsPreparePet = false
	local res = CGasPetInfoSql._CheckCharPetInfo:ExecSql("", charId,petId)
	if res:GetData(0,0) ~= 1 then
		return false,194108
	end
	local res = CGasPetInfoSql._SelectPreparePetByPetId:ExecStat(petId)
	if res:GetData(0,0) > 0 then
		bIsPreparePet = true
	end
	CGasPetInfoSql._DeletePetInfo:ExecSql("", petId)
	local succ,msgId
	if g_DbChannelMgr:LastAffectedRowNum() > 0 then
		succ = true
		msgId = 194107
	else
		succ = false
		msgId = 194108
	end
	return succ,msgId,bIsPreparePet
end

-----------------------------------------------------------------------------------------
local StmtDef = {
	"_DeletePetEggInfo",
	[[
		delete from tbl_pet_egg where pe_sEggName = ? and cs_uId = ?
	]]
}
DefineSql(StmtDef, CGasPetInfoSql)

function CGasPetInfoDB.ThrowAwayPetEgg(data)
	local charId = data["charId"]
	local petEggName = data["petEggName"]
	CGasPetInfoSql._DeletePetEggInfo:ExecStat(petEggName,charId)
	
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end
-----------------------------------------------------------------------------------------
local StmtDef = {
	"_ChangePetName",
	[[
		update tbl_char_pet set cp_sPetName = ? where cs_uId = ? and ps_uId = ?
	]]
}
DefineSql(StmtDef, CGasPetInfoSql)

--@brief 幻兽重命名
function CGasPetInfoDB.ChangePetName(data)
	local charId = data["charId"]
	local petId = data["petId"]
	local newName = data["newName"]
	
	local res = CGasPetInfoSql._CheckCharPetInfo:ExecSql("", charId,petId)
	if res:GetRowNum() ~= 1 then
		return false,194110
	end
	
	CGasPetInfoSql._ChangePetName:ExecSql("", newName,charId,petId)
	local succ,msgId
	if g_DbChannelMgr:LastAffectedRowNum() > 0 then
		succ = true
		msgId = 194109
	else
		succ = false
		msgId = 194110
	end
	return succ,msgId
end
-----------------------------------------------------------------------------------------
local StmtDef = {
	"_SavePreparePet",
	[[
		replace into tbl_pet_prepare(cs_uId,ps_uId) values(?,?)
	]]
}
DefineSql(StmtDef, CGasPetInfoSql)  
--@brief 准备幻兽
function CGasPetInfoDB.PreparePet(data)
	local charId = data["charId"]		
	local petId = data["petId"]	
	CGasPetInfoSql._SavePreparePet:ExecStat(charId,petId)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end
-----------------------------------------------------------------------------------------
local StmtDef = {
	"_SelectPreparePet",
	[[
		select ps_uId from tbl_pet_prepare where cs_uId = ?
	]]
}
DefineSql(StmtDef, CGasPetInfoSql)  
--@brief 查询准备的幻兽
function CGasPetInfoDB.SelectPreparePet(charId)		
	local res = CGasPetInfoSql._SelectPreparePet:ExecStat(charId)
	return res
end
-----------------------------------------------------------------------------------------
local StmtDef = {
	"_GetSkillNumByCharId",
	[[
		select count(*) from  tbl_pet_skill where cs_uId = ? and ps_uId = ?
	]]
}
DefineSql(StmtDef, CGasPetInfoSql)  

local StmtDef = {
	"_UpdateSkillByCharIdAndIndex",
	[[
		update tbl_pet_skill set ps_sSkillName = ?  where cs_uId = ? and ps_uIndex = ? and ps_uId = ?
	]]
}
DefineSql(StmtDef, CGasPetInfoSql)

local StmtDef = {
	"_GetNumByCharIdAndSkillName",
	[[
		select count(*) from  tbl_pet_skill where cs_uId = ? and ps_sSkillName = ? and ps_uId = ?
	]]
}
DefineSql(StmtDef, CGasPetInfoSql) 

--@brief 使用幻兽灵石学技能
function CGasPetInfoDB.UsePetSkillStone(data)
	local charId = data["charId"]		
	local ItemName = data["ItemName"]	
	local nRoomIndex = data["nRoomIndex"]
	local nPos = data["nPos"]
	local petId,posIndex
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local petEggRes = g_RoomMgr.GetAllItemFromPosition(charId, nRoomIndex, nPos)
	if petEggRes:GetRowNum() == 0  then
		return false,194111
	end
	local itemInfo = g_ItemInfoMgr:GetItemFunInfo(42, ItemName)
	if not itemInfo then
		return false,194111
	end
	local skillName = Pet_Skill_Common(itemInfo("SkillIndex"), "Index")
	if not skillName then
		return false,194111
	end
	
	local petRes = CGasPetInfoSql._SelectPreparePet:ExecStat(charId)
	if petRes:GetRowNum() > 0 then
		petId = petRes:GetData(0,0)
	else
		return false,194114
	end
	
	local IsLearnOrNot = CGasPetInfoSql._GetNumByCharIdAndSkillName:ExecStat(charId,skillName,petId)
	if IsLearnOrNot:GetData(0,0) > 0 then
		return false,194112
	end
	local res = CGasPetInfoSql._GetSkillNumByCharId:ExecStat(charId,petId)
	local skillNum = res:GetData(0,0)
	if skillNum == 1 then
		CGasPetInfoSql._SavePetSkill:ExecSql("",charId,petId,skillName,2)
		posIndex = 2
	elseif skillNum == 2 then
		CGasPetInfoSql._SavePetSkill:ExecSql("",charId,petId,skillName,3)
		posIndex = 3
	elseif skillNum == 3 then
		local index = math.random(1,100)
		if index <= 50 then
			local updateIndex = math.random(1,3)
			CGasPetInfoSql._UpdateSkillByCharIdAndIndex:ExecStat(skillName,charId,updateIndex,petId)
			posIndex = updateIndex
		else
			CGasPetInfoSql._SavePetSkill:ExecSql("",charId,petId,skillName,4)
			posIndex = 4
		end
	elseif skillNum == 4 then
		local index = math.random(1,100)
		if index <= 75 then
			local updateIndex = math.random(1,4)
			CGasPetInfoSql._UpdateSkillByCharIdAndIndex:ExecStat(skillName,charId,updateIndex,petId)
			posIndex = updateIndex
		else
			CGasPetInfoSql._SavePetSkill:ExecSql("",charId,petId,skillName,5)
			posIndex = 5
		end
	elseif skillNum == 5 then
		local index = math.random(1,100)
		if index <= 90 then
			local updateIndex = math.random(1,5)
			CGasPetInfoSql._UpdateSkillByCharIdAndIndex:ExecStat(skillName,charId,updateIndex,petId)
			posIndex = updateIndex
		else
			CGasPetInfoSql._SavePetSkill:ExecSql("",charId,petId,skillName,6)
			posIndex = 6
		end
	else
		CGasPetInfoSql._UpdateSkillByCharIdAndIndex:ExecStat(skillName,charId,6,petId)
	end
	if g_DbChannelMgr:LastAffectedRowNum() > 0 then
		local DelRet = g_RoomMgr.DelItemByPos(charId,nRoomIndex,nPos,1,event_type_tbl["物品使用"])
		if IsNumber(DelRet) then
			return false
		end
		return true,194113,DelRet[1],skillName,posIndex,petId
	else
		return false,194111
	end
end
---------------------------------------------------------------------------------------------------
local StmtDef = {
	"_GetPetSkill",
	[[
		select ps_uId,ps_sSkillName,ps_uIndex from tbl_pet_skill where cs_uId = ?
	]]
}
DefineSql(StmtDef, CGasPetInfoSql) 
  
--@brief 查询幻兽拥有的技能
function CGasPetInfoDB.GetPetSkill(charId)
	local res = CGasPetInfoSql._GetPetSkill:ExecStat(charId)
	return res
end
---------------------------------------------------------------------------------------------------
--@brief 初始化幻兽面板信息
function CGasPetInfoDB.InitPetWndInfo(data)
	local charId = data["charId"]
	local result = {}
	result["charPetInfoTbl"] = CGasPetInfoDB.InitCharPetInfo(charId)
	result["charpetEggTbl"] = CGasPetInfoDB.InitCharPetEggInfo(charId)
	result["preparePet"]	= CGasPetInfoDB.SelectPreparePet(charId)
	result["petSkill"]	= CGasPetInfoDB.GetPetSkill(charId)
	return result
end

return CGasPetInfoDB