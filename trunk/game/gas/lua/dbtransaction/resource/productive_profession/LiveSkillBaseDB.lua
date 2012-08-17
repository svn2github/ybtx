gac_gas_require "liveskill/LiveSkillMgr"
gac_gas_require "framework/common/CMoney"

local g_MoneyMgr = CMoney:new()
local CommonSkillMgr = CLiveSkillMgr:new()
local StmtOperater = {}	
local event_type_tbl = event_type_tbl
local LiveSkillBase = CreateDbBox(...)
------------------------------------------------------------------------------------------
--【添加技能】
local StmtDef = {
	"_AddSkills",
	[[
		replace into tbl_commerce_skill(cs_uId,cs_uSkillType,cs_sSkillName,cs_uSkillLevel,cs_uExperience) 
		values(?,?,?,?,?)
	]]
}
DefineSql( StmtDef, StmtOperater )

--【得到某技能信息】
local StmtDef = {
	"_GetSkillInfoByName",
	[[
		select cs_uSkillType,cs_uSkillLevel,cs_uExperience from tbl_commerce_skill 
		where cs_uId = ? and cs_sSkillName = ?
	]]
}
DefineSql( StmtDef, StmtOperater )
function LiveSkillBase.GetSkillInfo(uPlayerID,skillName)
	local query_result = StmtOperater._GetSkillInfoByName:ExecStat(uPlayerID,skillName)
	if query_result:GetRowNum() ~= 0 then
		return {query_result:GetNumber(0,0),query_result:GetNumber(0,1),query_result:GetNumber(0,2)}
	else
		return {}
	end
end

--【得到某类型的生活技能的数量】
local StmtDef = {
	"_CountCommerceSkills",
	[[
		select count(1) from tbl_commerce_skill 
		where cs_uId = ? and cs_uSkillType = ?
	]]
}
DefineSql( StmtDef, StmtOperater )
function LiveSkillBase.CountCommerceSkills(uPlayerID,uSkillType)
	local query_result = StmtOperater._CountCommerceSkills:ExecStat(uPlayerID,uSkillType)
	if query_result:GetRowNum() == 0 then
		return 0
	else
		return query_result:GetNumber(0,0)
	end
end

--【获得所有生活技能信息】
local StmtDef = {
	"_GetAllSkillsInfo",
	[[
		select 	cs_sSkillName,cs_uSkillLevel,cs_uExperience 
		from 		tbl_commerce_skill 
		where 	cs_uId = ? order by cs_uSID ASC
	]]
}
DefineSql( StmtDef, StmtOperater )
function LiveSkillBase.GetAllSkillsInfo(uPlayerID)
	local query_result = StmtOperater._GetAllSkillsInfo:ExecStat(uPlayerID)
	
	return query_result
end

--【判断是否学习了某技能】
local StmtDef = {
	"_CountSkillByType",
	[[
		select count(*) from tbl_commerce_skill  
		where cs_uId  = ?	and cs_sSkillName = ?
	]]
}
DefineSql( StmtDef, StmtOperater )
function LiveSkillBase.IsStudyASkill(uPlayerID,sSkillName)
	local tblCount = StmtOperater._CountSkillByType:ExecStat(uPlayerID,sSkillName)
	return tblCount:GetNumber(0,0) > 0
end

local StmtDef = {
	"_CountLiveSkill",
	[[
		select count(*) from tbl_commerce_skill  
		where cs_uId  = ?
	]]
}
DefineSql( StmtDef, StmtOperater )
function LiveSkillBase.CountLiveskillHaveLearned(uPlayerID)
	local tblCount = StmtOperater._CountLiveSkill:ExecStat(uPlayerID)
	return tblCount:GetNumber(0,0)
end

--【获得某项技能的等级】
local StmtDef = {
	"_GetSkillLevelByType",
	[[
		select cs_uSkillLevel from tbl_commerce_skill  
		where cs_uId  = ?	and cs_sSkillName = ?
	]]
}
DefineSql( StmtDef, StmtOperater )

-------------------------------------------------------

--【得到某技能的等级】
function LiveSkillBase.GetSkillLevelByType(uPlayerID,sSkillType)
	local tblLevel = StmtOperater._GetSkillLevelByType:ExecStat(uPlayerID,sSkillType)
	if tblLevel:GetRowNum() == 0 then
		return 0
	end
	return tblLevel:GetNumber(0,0)
end

------------------------------------------------------
--【获得已经学习的技能信息】
function LiveSkillBase.GetLiveSkillInfosDB(parameters)
	local uPlayerID = parameters.uPlayerID
	
	return LiveSkillBase.GetAllSkillsInfo(uPlayerID)
end

function LiveSkillBase.UpSkillAndExpertMoney(uPlayerID,NeedMoney,uEventId)
	local add_Money ,add_bMoney = 0,0
	if NeedMoney and NeedMoney > 0 then
		local MoneyManagerDB = RequireDbBox("MoneyMgrDB")
		local uBindingMoney = MoneyManagerDB.GetBindingMoney(uPlayerID)
		local Money = MoneyManagerDB.GetMoney(uPlayerID)
		if uBindingMoney + Money < NeedMoney then
			CancelTran()
			return false,305
		end
		if uBindingMoney <= NeedMoney then 
			add_bMoney = uBindingMoney
			add_Money = NeedMoney - add_bMoney 
		else
			add_bMoney = NeedMoney
		end
		local fun_info = g_MoneyMgr:GetFuncInfo("LiveSkill")
		if add_bMoney > 0  then
			local bFlag,uMsgID = MoneyManagerDB.AddMoneyByType(fun_info["FunName"],fun_info["LearnSkill"],uPlayerID,2, -add_bMoney,uEventId)
			if not bFlag then
				CancelTran()
				return false,uMsgID
			end
		end		
		if add_Money > 0 then
			local bFlag,uMsgID = MoneyManagerDB.AddMoneyByType(fun_info["FunName"],fun_info["LearnSkill"],uPlayerID,1, -add_Money,uEventId)
			if not bFlag then
				CancelTran()
				return false,uMsgID
			end
		end
	end		
	return add_Money,add_bMoney
end

--【学习技能/升级】
function LiveSkillBase.LearnLiveSkillDB(parameters)
	local uPlayerID = parameters.uPlayerID
	local uPlayerLevel = parameters.uPlayerLevel
	local skillName = parameters.skillName
	local info = LiveSkillBase.GetSkillInfo(uPlayerID,skillName)
	local uSkillLevel,uExper = 0,0
	if #info > 0 then
		--已经学习过该技能
		uSkillLevel,uExper = info[2],info[3]
	end
	local hasSkillInfo = CommonSkillMgr:HasSkillInfo(skillName, uSkillLevel+1)
	if not hasSkillInfo then
		--配置表没有该技能，不能学习
		local uMsgID = 9507
		if #info >0 then
			uMsgID = 9512
		end
		return false,uMsgID
	end
	if uPlayerLevel < ( CommonSkillMgr:GetSkillInfoByName(skillName, uSkillLevel+1, "BaseLevel") )then
		--等级不够
		return false,9508
	elseif uExper < ( CommonSkillMgr:GetSkillInfoByName(skillName, uSkillLevel+1, "ExperienceNeed") ) then
		--经验不够
		local uMsgID = 9511
		return false,uMsgID
	end
	--没有学习过
	--local MLRes
	local nSkillBigType = CommonSkillMgr:GetSkillBigType(skillName)
	if #info == 0 then
		if nSkillBigType > 1 then
			--判断是否只能学习一种的技能
			if LiveSkillBase.CountCommerceSkills(uPlayerID,nSkillBigType) > 0 then
				return false,9510
			end
		end
		--低速加佣兵等级指引计数
		--local MercenaryLevelDB = RequireDbBox("MercenaryLevelDB")
		--MLRes = MercenaryLevelDB.SlowAddMercenaryLevelCount(uPlayerID, "生活技能")
	end
	StmtOperater._AddSkills:ExecStat(uPlayerID,info[1] or tonumber(nSkillBigType),skillName,uSkillLevel+1,uExper)
	if g_DbChannelMgr:LastAffectedRowNum() == 0 then
		CancelTran()
		return false
	end
	--添加log
	local g_LogMgr = RequireDbBox("LogMgrDB")
	local uEventId = g_LogMgr.SaveLiveskillInfo(uPlayerID,skillName,uSkillLevel+1,uExper,event_type_tbl["生活技能学习或升级"])
	local NeedMoney = CommonSkillMgr:GetLearnSkillMoney(skillName,uSkillLevel+1)
	
	local add_Money,add_bMoney = LiveSkillBase.UpSkillAndExpertMoney(uPlayerID,NeedMoney,uEventId)
	if not add_Money then
		CancelTran()
		local uMsgID = add_bMoney
		return false,uMsgID
	end
	
	local RetRes = {}
	RetRes.SkillsInfo = LiveSkillBase.GetAllSkillsInfo(uPlayerID)
	RetRes.add_Money = -add_Money
	RetRes.add_bMoney = -add_bMoney
	return true,RetRes
end
-----------------------------------------------------------------------------
--【洗技能】
local StmtDef = {
	"_GetSkillType",
	[[	
		select cs_uSkillType from tbl_commerce_skill where cs_uId = ? and cs_sSkillName = ?
	]]
}
DefineSql( StmtDef, StmtOperater )
local StmtDef = {
	"_DeleteSkill",
	[[	
		delete from tbl_commerce_skill where cs_uId = ? and cs_sSkillName = ?
	]]
}
DefineSql( StmtDef, StmtOperater )
local StmtDef = {
	"_DeleteDirections",
	[[	
		delete from tbl_skill_directions where cs_uId = ? and sd_sSkillName = ?
	]]
}
DefineSql( StmtDef, StmtOperater )
local StmtDef = {
	"_DeleteSpecialize",
	[[	
		delete from tbl_specialize_smithing where cs_uId = ? and ss_uSkillType = ?
	]]
}
DefineSql( StmtDef, StmtOperater )
local StmtDef = {
	"_DeleteExpert",
	[[	
		delete from tbl_expert_smithing where cs_uId = ? and es_uSkillType = ?
	]]
}
DefineSql( StmtDef, StmtOperater )
function LiveSkillBase.WashLiveSkill(parameters)
	local uPlayerID = parameters.uPlayerID	
	local sSkillName = parameters.sSkillName
	local nSkillBigType = CommonSkillMgr:GetSkillBigType(sSkillName)
	if nSkillBigType == 0 then
		--默认学会的技能不能洗白
		return 9532
	end
	local res = StmtOperater._GetSkillType:ExecStat(uPlayerID,sSkillName)
	if res:GetRowNum() == 0 then
		return 9530
	end
	--删除技能
	local uSkillType = CommonSkillMgr:GetSkillTypeByName(sSkillName)
	StmtOperater._DeleteSkill:ExecStat(uPlayerID,sSkillName)
	StmtOperater._DeleteDirections:ExecStat(uPlayerID,sSkillName)
	StmtOperater._DeleteSpecialize:ExecStat(uPlayerID,uSkillType)
	StmtOperater._DeleteExpert:ExecStat(uPlayerID,uSkillType)
	
	local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.SaveLiveskillInfo(uPlayerID,sSkillName,0,0,event_type_tbl["生活技能遗忘"])
	
	return LiveSkillBase.GetAllSkillsInfo(uPlayerID)
end
--------------------------------------

--【GM指令加skill】
function LiveSkillBase.AddLiveskillGM(data)
	local uPlayerID = data.uPlayerID
	local nExp = data.nExp
	local sSkillName = data.sSkillName
	local nSkillBigType = CommonSkillMgr:GetSkillBigType(sSkillName)
	if not nSkillBigType then return end
	local info = LiveSkillBase.GetSkillInfo(uPlayerID,sSkillName)
	local level = 1
	if #info > 0 then
		--已经学习过该技能
		level= info[2]
		nExp = info[3]+nExp
	end
	StmtOperater._AddSkills:ExecStat(uPlayerID,tonumber(nSkillBigType),sSkillName,level,nExp)
	--添加log
	local g_LogMgr = RequireDbBox("LogMgrDB")
	local enent_id = g_LogMgr.SaveLiveskillInfo(uPlayerID,sSkillName,level,nExp,97)

	return LiveSkillBase.GetAllSkillsInfo(uPlayerID)
end

--【增加某类锻造的熟练度】
local StmtDef = {
	"_AddSpecialize",
	[[
		replace into tbl_specialize_smithing(cs_uId,ss_uType,ss_uSpecialize,ss_uSkillType) values(?,?,?,?)
	]]
}
DefineSql( StmtDef, StmtOperater )
--【gm指令加熟练度】
function LiveSkillBase.AddLiveSkillPracticeGM(data)
	local uPlayerID = data.uPlayerID
	local sSkillName = data.sSkillName
	local nPract = data.nPractice
	local sPracticeName = data.sPracticeName
	if  not LiveSkillBase.IsStudyASkill(uPlayerID,sSkillName) then
		return false,9515
	end
	local nSkillType = CommonSkillMgr:GetSkillTypeByName(sSkillName)
	if not CommonSkillMgr:ExistPractice(nSkillType,sPracticeName) then
		return false,9563
	end
	StmtOperater._AddSpecialize:ExecStat(uPlayerID,sPracticeName,nPract,nSkillType)
	local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.SaveLiveskillPractice(uPlayerID,nSkillType,sPracticeName,nPract,98)
	
	return g_DbChannelMgr:LastAffectedRowNum() > 0 
end

--【默认学会的生活技能】
local StmtDef = {
	"_AddDefaultSkills",
	[[	
		replace into tbl_commerce_skill(cs_uId,cs_uSkillType,cs_sSkillName,cs_uSkillLevel,cs_uExperience)
		values(?,?,?,?,?)
	]]
}
DefineSql( StmtDef, StmtOperater )
function LiveSkillBase.AddDefaultSkills(nCHarID)
  local default_skills = CommonSkillMgr:GetDefaultLearnedSkillInfo()
  for i =1,#default_skills do
		StmtOperater._AddDefaultSkills:ExecStat(nCHarID,0,default_skills[i],1,0)
	end
end
----------------------------
SetDbLocalFuncType(LiveSkillBase.GetLiveSkillInfosDB)
SetDbLocalFuncType(LiveSkillBase.LearnLiveSkillDB)
SetDbLocalFuncType(LiveSkillBase.WashLiveSkill)
SetDbLocalFuncType(LiveSkillBase.AddLiveSkillPracticeGM)
SetDbLocalFuncType(LiveSkillBase.AddLiveskillGM)

return LiveSkillBase
