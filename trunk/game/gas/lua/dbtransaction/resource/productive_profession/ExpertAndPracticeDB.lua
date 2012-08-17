gas_require "resource/productive_profession/GasSkillMgr"
gac_gas_require "liveskill/LiveSkillMgr"

local GasSkillMgr = CGasSkillMgr:new()
local CommonSkillMgr = CLiveSkillMgr:new()
local StmtOperater = {}	
local event_type_tbl = event_type_tbl
local ExpertAndPracticeDB = CreateDbBox(...)
------------------------------------------------------------------------------------------
--【获得所有熟练度信息】
local StmtDef = {
	"_GetPracticedInfo",
	[[
		select ss_uType,ss_uSpecialize from tbl_specialize_smithing 
		where cs_uId = ? and ss_uSkillType = ?
	]]
}
DefineSql( StmtDef, StmtOperater )
function ExpertAndPracticeDB.GetPracticedInfo(uPlayerID,nSkillType)
	local query_result = StmtOperater._GetPracticedInfo:ExecStat(uPlayerID,nSkillType)

	return query_result
end

local StmtDef = {
	"_AddExpert",
	[[
		replace into tbl_expert_smithing(cs_uId,es_uSkillType,es_uType,es_uLevel)
			values(?,?,?,?)
	]]
}
DefineSql( StmtDef, StmtOperater )

--【获得某项熟练度】
local StmtDef = {
	"_GetSpecializeByType",
	[[
		select ss_uSpecialize from tbl_specialize_smithing  
		where cs_uId  = ? and ss_uType = ?
	]]
}
DefineSql( StmtDef, StmtOperater )
function ExpertAndPracticeDB.GetSpecializeByType(uPlayerID,sType)
	local query_result = StmtOperater._GetSpecializeByType:ExecStat(uPlayerID,sType)
  if query_result:GetRowNum() == 0 then
  	return -1
  end
  return query_result:GetNumber(0,0)
end

--【获得某玩家学习的专精】
local StmtDef = {
	"_GetUserExpert",
	[[	
		select es_uType,es_uLevel from tbl_expert_smithing  
		where cs_uId  = ?	and es_uSkillType = ?
	]]
}
DefineSql( StmtDef, StmtOperater )
function ExpertAndPracticeDB.GetExpert(uPlayerID,nSkillType)
	local query_result = StmtOperater._GetUserExpert:ExecSql('s[32]n', uPlayerID,nSkillType)
  if query_result:GetRowNum() == 0 then
  	return {}
  else
  	return {query_result:GetString(0,0),query_result:GetNumber(0,1)}
  end
end

local StmtDef = {
	"_CountExpertByType",
	[[	
		select count(1) from tbl_expert_smithing  
		where cs_uId  = ?	and es_uType = ?
	]]
}
DefineSql( StmtDef, StmtOperater )
function ExpertAndPracticeDB.IsStudyExpert(uPlayerID,sExpertType)
	local query_result = StmtOperater._CountExpertByType:ExecStat(uPlayerID,sExpertType)
  return query_result:GetData(0,0) > 0
end

--【获得锻造熟练度信息】
function ExpertAndPracticeDB.GetPracticedInfoDB(parameters)
	local uPlayerID = parameters.uPlayerID
	local nSkillType = parameters.nSkillType
	local info = ExpertAndPracticeDB.GetPracticedInfo(uPlayerID,nSkillType)
	local tblRes = {}
	--获得该技能的所有熟练度类型，然后把学过的熟练度按此顺序排序
	local tblPracticed = CommonSkillMgr:GetPracticedListByIndex(nSkillType)
	for i = 1,#tblPracticed do
		local practiced = 0
		local row = info:GetRowNum()
		for j =1,row do
			if info(j,1) == tblPracticed[i] then
				practiced = info(j,2)
				break
			end
		end
		table.insert(tblRes,{tblPracticed[i],practiced})
	end
	local tblExpert = ExpertAndPracticeDB.GetExpert(uPlayerID,nSkillType)
	tblRes.tblExpert = tblExpert
	return tblRes
end
-----------------------------------------------------------------------------
--【学习专精】
function ExpertAndPracticeDB.StudyExpertDB(parameters)
	local uPlayerID = parameters.uPlayerID	
	local sSkillName = parameters.sSkillName
	local sEquipType = parameters.sEquipType
	
	local nSkillType = CommonSkillMgr:GetSkillTypeByName(sSkillName)
	if not nSkillType then return end

	local specialize = ExpertAndPracticeDB.GetSpecializeByType(uPlayerID,sEquipType)
	local tblExpert = ExpertAndPracticeDB.GetExpert(uPlayerID,nSkillType)
	
	
	local uLevel = 1
	if #tblExpert > 0  then
		--学习过该专精,则升级
		if  tblExpert[1] ~= sEquipType then
			--只能学习一种专精
			return false,9521
		else
			uLevel = tblExpert[2] +1
		end
	end
	
	local expertinfo = GasSkillMgr:GetExpertInfoByLevel(uLevel)
	if nil == expertinfo then
		--该专精已经达到最高等级
		return false,9522
	end
	
	local need_spec = expertinfo("Specialize") or 1
	if specialize < need_spec then
		--熟练度不够
		return false,9523,need_spec
	end
	--添加专精
	StmtOperater._AddExpert:ExecStat(uPlayerID,nSkillType,sEquipType,uLevel)
	local g_LogMgr = RequireDbBox("LogMgrDB")
	local uEventId = g_LogMgr.SaveLiveskillExpert(uPlayerID,nSkillType,sEquipType,uLevel,event_type_tbl["生活技能专精学习"])
	local NeedMoney = expertinfo("MoneySpend") or 0
	local LiveSkillBaseDB = RequireDbBox("LiveSkillBaseDB")
	local add_Money,add_bMoney = LiveSkillBaseDB.UpSkillAndExpertMoney(uPlayerID,NeedMoney,uEventId)
	if not add_Money then
		CancelTran()
		local uMsgID = add_bMoney
		return false,uMsgID
	end
	
	local RetRes = {}
	RetRes.sEquipType = sEquipType
	RetRes.uLevel = uLevel
	RetRes.add_Money = -add_Money
	RetRes.add_bMoney = -add_bMoney
	return true, RetRes
end

--【获得专精信息】
function ExpertAndPracticeDB.GetExpertDB(parameters)
	local uPlayerID = parameters.uPlayerID	
	local sSkillName = parameters.sSkillName	
	local uSkillType = CommonSkillMgr:GetSkillTypeByName(sSkillName)
	return ExpertAndPracticeDB.GetExpert(uPlayerID,uSkillType)
end

local StmtDef = {
	"_DelAllExpert",
	[[	
		delete from tbl_expert_smithing where cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtOperater )

local StmtDef = {
	"_CountExpert",
	[[	
		select count(*) from  tbl_expert_smithing where cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtOperater )
function ExpertAndPracticeDB.WashExpert(parameters)
	local uPlayerID = parameters.uPlayerID		
	local tblCount = StmtOperater._CountExpert:ExecStat(uPlayerID)
	if tblCount(1,1) == 0 then return 9555 end
	StmtOperater._DelAllExpert:ExecStat(uPlayerID)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end

SetDbLocalFuncType(ExpertAndPracticeDB.GetPracticedInfoDB)
SetDbLocalFuncType(ExpertAndPracticeDB.StudyExpertDB)
SetDbLocalFuncType(ExpertAndPracticeDB.GetExpertDB)

return ExpertAndPracticeDB
