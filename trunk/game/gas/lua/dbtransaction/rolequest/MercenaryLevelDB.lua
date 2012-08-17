gac_gas_require "activity/quest/MercenaryLevelCfg"
gac_gas_require "liveskill/LiveSkillMgr"

local g_MercenaryLevelTbl = g_MercenaryLevelTbl
local event_type_tbl = event_type_tbl
local LiveSkillMgr = CLiveSkillMgr:new()

local StmtContainer = class()

local StmtDef = 
{
	"_AddNewMercenaryLevel",
	[[
		insert into tbl_mercenary_level
			(cs_uId)
		values
			(?)
	]]
}
DefineSql(StmtDef, StmtContainer)

local StmtDef = 
{
	"_AddMercenaryLevelCount",
	[[
		update tbl_mercenary_level
		set
			ml_uCount1 = ml_uCount1 + ?,
			ml_uCount2 = ml_uCount2 + ?,
			ml_uCount3 = ml_uCount3 + ?,
			ml_uCallenge = ml_uCallenge + ?
		where
			cs_uId = ?
	]]
}
DefineSql(StmtDef, StmtContainer)

local StmtDef = 
{
	"_AddMercenaryLevelStatus",
	[[
		update tbl_mercenary_level
		set
			ml_uStatus = ml_uStatus + ?
		where
			cs_uId = ?
	]]
}
DefineSql(StmtDef, StmtContainer)

local StmtDef = 
{
	"_MercenaryLevelUpInit",
	[[
		update tbl_mercenary_level
		set
			ml_uStatus = 1,
			ml_sSubject1 = ?,
			ml_sSubject2 = ?,
			ml_sSubject3 = ?,
			ml_uCount1 = 0,
			ml_uCount2 = 0,
			ml_uCount3 = 0,
			ml_uCallenge = 0
		where
			cs_uId = ?
	]]
}
DefineSql(StmtDef, StmtContainer)

local StmtDef = 
{
	"_MercenaryLevelUp",
	[[
		update tbl_char_basic
		set
			cb_uMercenaryLevel = cb_uMercenaryLevel + ?
		where
			cs_uId = ?
	]]
}
DefineSql(StmtDef, StmtContainer)

local StmtDef = 
{
	"_GetMercenaryLevelInfo",
	[[
		select
			ml_uStatus, ml_sSubject1, ml_sSubject2, ml_sSubject3, ml_uCount1, ml_uCount2, ml_uCount3, ml_uCallenge
		from
			tbl_mercenary_level
		where
			cs_uId = ?
	]]
}
DefineSql(StmtDef, StmtContainer)

local StmtDef = 
{
	"_GetPlayerLevel",
	[[
		select 
			cb_uLevel, cb_uMercenaryLevel
		from
			tbl_char_basic
		where
			cs_uId = ?
	]]
}
DefineSql(StmtDef, StmtContainer)

local StmtDef = 
{
	"_GetMercenaryIntegral",
	[[
		select 
			cs_uLevelIntegral
		from
			tbl_char_integral
		where
			cs_uId = ?
	]]
}
DefineSql(StmtDef, StmtContainer)

local StmtDef = 
{
	"_AddMercenaryLevelAward",
	[[
		insert into tbl_mercenary_level_award
			(cs_uId, mla_sAward)
		values
			(?,?)
	]]
}
DefineSql(StmtDef, StmtContainer)

local StmtDef = 
{
	"_GetMercenaryLevelAward",
	[[
		select
			mla_sAward
		from
			tbl_mercenary_level_award
		where
			cs_uId = ?
	]]
}
DefineSql(StmtDef, StmtContainer)

local StmtDef = 
{
	"_DeleteAllMercenaryLevelAward",
	[[
		delete from
			tbl_mercenary_level_award
		where
			cs_uId = ?
	]]
}
DefineSql(StmtDef, StmtContainer)

local MercenaryLevelDB = CreateDbBox(...)

------------------------------------------------------------------------
local func = function(Subject, LevelTbl, Status)
	if Status == 1 and LevelTbl["达成条件"][Subject] then
		return true, LevelTbl["达成条件"][Subject].Arg
	end
end

local CheckSubjects = {
	--["排名"]
	["挑战试炼"] = function(Param, LevelTbl, Status)
		if Status == 2 and LevelTbl["挑战条件"].Arg then
			local QuestNames = LevelTbl["挑战条件"].Arg
			if Param == QuestNames[1] or Param == QuestNames[2] or Param == QuestNames[3] then
				return true, 1
			end
		end
	end,
	
	["气魂击"] = function(Param, LevelTbl, Status)
		if Status == 1 and LevelTbl["达成条件"]["气魂击"] and Param == "气魂击" then
			return true, LevelTbl["达成条件"]["气魂击"].Arg
		end
	end,
	
	["护甲值"] = function(Param, LevelTbl, Status)
		if Status == 1 and LevelTbl["达成条件"]["护甲值"] then
			if Param > LevelTbl["达成条件"]["护甲值"].Arg then
				Param = LevelTbl["达成条件"]["护甲值"].Arg
			end
			return Param, LevelTbl["达成条件"]["护甲值"].Arg
		end
	end,
	
	["佣兵比赛"] = function(Param, LevelTbl, Status)
		if Status == 1 then
			if (Param == "异界守护塔单人" or Param == "异界守护塔队伍") and LevelTbl["达成条件"]["守塔"] then
				return true, LevelTbl["达成条件"]["守塔"].Arg
			elseif LevelTbl["达成条件"]["佣兵比赛"] then
				return true, LevelTbl["达成条件"]["佣兵比赛"].Arg
			end
		end
	end,
	
	["紫装"] = function(Param, LevelTbl, Status)
		if Status == 2 and Subject == LevelTbl["挑战条件"].Subject then
			return true, LevelTbl["挑战条件"].Arg
		end
	end,
	
	["转职"]		= func,
	["修行塔"]	= func,
	["佣兵任务"]= func,
	["生活技能"]= func,
}

local func2 = function(LevelTbl, data)
	local Award = data["Award"]
	if LevelTbl[Award] then
		return true
	end
end

local CheckAwards = {
	["技能"] = function(LevelTbl, data)
		local Award = data["Award"]
		if LevelTbl[Award] then
			return true
		end
	end,
	
	["物品"] = function(LevelTbl, data)
		local Award = data["Award"]
		local Arg = LevelTbl[Award].Arg
		if LevelTbl[Award] then
			local param = {}
			param["nType"]			= Arg[1]
			param["sName"] 			= Arg[2]
			param["nCount"]			= Arg[3]
			param["char_id"] 		= data["CharId"]
			param["createType"]	= event_type_tbl["副本奖励"]
			local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
			local res = CharacterMediatorDB.AddItem(param)
			if IsNumber(res) then
				CancelTran()
				return
			else
				return {Arg[1],Arg[2],Arg[3],res}
			end
		end
	end,
	
	["称号"] = function(LevelTbl, data)
		local Award = data["Award"]
		if LevelTbl[Award] and LevelTbl[Award].Arg then
			return true
		end
	end,
	
	["经验提升"] = func2,
	["列表传送"] = func2,
	["开放等级"] = func2,
	--["技能"] = func2,
	["行为"] = func2,
	
	["去掉CD"] = func2,
	["敬礼"] = func2,
}

------------------------------------------------------------------------

local function GetMercenaryLevelAward(data)
	local res = StmtContainer._GetMercenaryLevelAward:ExecSql("s", data["CharId"])
	local n = res:GetRowNum()
	local tbl = {}
	if res and n>0 then
		for i=1,n do
			local award = res:GetData(i-1,0)
			tbl[award] = true
		end
	end
	return tbl
end

local function GetMercenaryLevelInfo(data)
	local CharId = data["CharId"]
	local res = StmtContainer._GetMercenaryLevelInfo:ExecSql("nsssnnnn", CharId)
	if res and res:GetRowNum()>0 then
		local tbl = {}
		local Status = res:GetData(0,0)
		tbl.Subject1 = res:GetData(0,1)
		tbl.Subject2 = res:GetData(0,2)
		tbl.Subject3 = res:GetData(0,3)
		tbl.Count1 = res:GetData(0,4)
		tbl.Count2 = res:GetData(0,5)
		tbl.Count3 = res:GetData(0,6)
		local Challenge = res:GetData(0,7)
		res = StmtContainer._GetMercenaryIntegral:ExecSql("n", CharId)
		local Integral = res:GetData(0,0)
		res = StmtContainer._GetPlayerLevel:ExecSql("n", CharId)
		local Level = res:GetData(0,0)
		local MercenaryLevel = res:GetData(0,1)
		return MercenaryLevel, Status, tbl, Challenge, Integral, Level
	end
	return false
end

function MercenaryLevelDB.GetMercenaryLevelInfo(data)
	return GetMercenaryLevelInfo(data)
end

local function CheckMercenaryLevel(data)
	local MercenaryLevel, Status, CountTbl, Challenge, Integral, Level = GetMercenaryLevelInfo(data)
	if not MercenaryLevel then
		return
	end
	
	local IsReach = true
	if MercenaryLevel ~= g_MercenaryLevelTbl.MaxLevel then
		local LevelTbl = g_MercenaryLevelTbl[MercenaryLevel]
		if Status == 1 then
			if (LevelTbl["达成条件"]["佣兵积分"] and Integral < LevelTbl["达成条件"]["佣兵积分"].Arg)
				or (LevelTbl["达成条件"]["人物等级"] and Level < LevelTbl["达成条件"]["人物等级"].Arg)
				or (CountTbl.Subject1 ~= "" and CountTbl.Count1 < LevelTbl["达成条件"][CountTbl.Subject1].Arg)
				or (CountTbl.Subject2 ~= "" and CountTbl.Count2 < LevelTbl["达成条件"][CountTbl.Subject2].Arg)
				or (CountTbl.Subject3 ~= "" and CountTbl.Count3 < LevelTbl["达成条件"][CountTbl.Subject3].Arg)
				then
					IsReach = false
			end
		elseif Status == 2 then
			local Subject = LevelTbl["挑战条件"].Subject
			if Subject == "挑战试炼" then
				if Challenge < 1 then
					IsReach = false
				end
			elseif Subject == "排名" then
				local pos = 0----------------------------------------------获得名次
				if pos > LevelTbl["挑战条件"].Arg then
					IsReach = false
				end
			else
				if Challenge < LevelTbl["挑战条件"].Arg then
					IsReach = false
				end
			end
		else
			IsReach = false
		end
		if Status < 3 and IsReach then
			StmtContainer._AddMercenaryLevelStatus:ExecSql("", 1, data["CharId"])
			Status = Status + 1
		end
		
	else
		IsReach = false
	end
	return {MercenaryLevel, Status, CountTbl, Challenge, Integral, Level}
end

function MercenaryLevelDB.CheckMercenaryLevel(data)
	return CheckMercenaryLevel(data)
end

local function GetCount(Subject, Result, Count)
	if Subject == "护甲值" then
		local def = Result - Count
		return (def<0 and 0) or def
	else
		return 1
	end
end

local function AddMercenaryLevelCount(data)
	local CharId = data["CharId"]
	local Subject = data["Subject"]
	local Param = data["Param"]
	local MercenaryLevel, Status, CountTbl, Challenge, _, _ = GetMercenaryLevelInfo(data)
	if not MercenaryLevel or MercenaryLevel==g_MercenaryLevelTbl.MaxLevel then
		return
	end
	local LevelTbl = g_MercenaryLevelTbl[MercenaryLevel]
	
	Param = Param or Subject
	local result, max = CheckSubjects[Subject](Param, LevelTbl, Status)
	
	if result then
		local count1, count2, count3, chg = 0, 0, 0, 0
		if Status == 1 then
			if Subject == CountTbl.Subject1 and CountTbl.Count1<max then
				count1 = GetCount(Subject, result, CountTbl.Count1)
			elseif Subject == CountTbl.Subject2 and CountTbl.Count2<max then
				count2 = GetCount(Subject, result, CountTbl.Count2)
			elseif Subject == CountTbl.Subject3 and CountTbl.Count3<max then
				count3 = GetCount(Subject, result, CountTbl.Count3)
			end
		elseif Status == 2 then
			if Subject~="排名" and Challenge<max then
				chg = 1
			elseif Subject=="排名" and Challenge>max then
				--排名处理
			end
		end
		if not (count1==0 and count2==0 and count3==0 and chg==0) then
			StmtContainer._AddMercenaryLevelCount:ExecSql("", count1, count2, count3, chg, CharId)
			return CheckMercenaryLevel(data)
		end
	end
end

function MercenaryLevelDB.AddMercenaryLevelCount(data) 
	return AddMercenaryLevelCount(data)
end

function MercenaryLevelDB.SlowAddMercenaryLevelCount(CharId, Subject, Param)
	local data = {}
	data["CharId"] = CharId
	data["Subject"] = Subject
	data["Param"] = Param
	return AddMercenaryLevelCount(data)
end

local function DoMercenaryLevelUp(CharId, NewLevel, AddLevel, Defence)
	StmtContainer._MercenaryLevelUp:ExecSql("", AddLevel, CharId)
	StmtContainer._DeleteAllMercenaryLevelAward:ExecSql("", CharId)
	local ActivityCountDB = RequireDbBox("ActivityCountDB")
	ActivityCountDB.ClearCurCycJoinCount(CharId, "佣兵日常任务")	--清佣兵任务每日限制
	ActivityCountDB.ClearCurCycJoinCount(CharId, "佣兵训练活动")	--清修行塔每日限制
	
	if NewLevel == g_MercenaryLevelTbl.MaxLevel then
		StmtContainer._MercenaryLevelUpInit:ExecSql("", "", "", "", CharId)
	else
		local Subject = {}
		local i = 1
		local j,k,l = 0,0,0
		for sub, _ in pairs(g_MercenaryLevelTbl[NewLevel]["达成条件"]) do
			if sub ~= "佣兵积分" and sub ~= "人物等级" then
				if sub=="修行塔" then	j=i end
				if sub=="护甲值" and Defence then	k=i	end
				if sub=="生活技能" then l=i	end
				Subject[i] = sub
				i = i+1
			end
		end
		StmtContainer._MercenaryLevelUpInit:ExecSql("", Subject[1] or "", Subject[2] or "", Subject[3] or "", CharId)
		
		local v = {0,0,0,0}
		local SpacialCount = ActivityCountDB.GetActivityHistoryTimes(CharId, "佣兵训练活动")
		local LiveSkillBaseDB = RequireDbBox("LiveSkillBaseDB")
		local DefaultSkills = LiveSkillMgr:GetDefaultLearnedSkillInfo()
		local CountOfLiveskill = LiveSkillBaseDB.CountLiveskillHaveLearned(CharId)
		v[j] = SpacialCount	--修行塔特殊处理
		v[k] = Defence or 0	--护甲值特殊处理
		v[l] = ((CountOfLiveskill - #DefaultSkills)>0 and 1) or 0	--生活技能特殊处理
		--v[4] = --排名处理
		StmtContainer._AddMercenaryLevelCount:ExecSql("", v[1],v[2],v[3],v[4], CharId)
	end
	return CheckMercenaryLevel( {["CharId"]=CharId} )
end

function MercenaryLevelDB.AddNewMercenaryLevel(data)
	local CharId = data["CharId"]
	local res = GetMercenaryLevelInfo(data)
	if not res then
		StmtContainer._AddNewMercenaryLevel:ExecSql("", CharId)
		return DoMercenaryLevelUp(CharId, 0, 0)
	end
end

function MercenaryLevelDB.MercenaryLevelUp(data)
	local CharId = data["CharId"]
	local MercenaryLevel, Status = GetMercenaryLevelInfo(data)
	if Status ~= 4 or MercenaryLevel == g_MercenaryLevelTbl.MaxLevel then
		return
	end
	
	local FinishAwards = GetMercenaryLevelAward(data)
	for award,_ in pairs(g_MercenaryLevelTbl[MercenaryLevel]["特权"]) do
		if not FinishAwards[award] then
			return
		end
	end
	return DoMercenaryLevelUp(CharId, MercenaryLevel+1, 1, data["Defence"])
end

function MercenaryLevelDB.GMAddMercenaryLevelUp(data)
	local CharId = data["CharId"]
	local ResultMercLev = data["ResultMercLev"]
	local MercenaryLevel = GetMercenaryLevelInfo(data)
	if not MercenaryLevel then
		return
	end
	local AddLevel = ResultMercLev - MercenaryLevel
	if AddLevel == 0 then
		return
	end
	return DoMercenaryLevelUp(CharId, ResultMercLev, AddLevel)
end

function MercenaryLevelDB.MercenaryLevelAppraise(data)
	local MercenaryLevel, Status = GetMercenaryLevelInfo(data)
	if MercenaryLevel == g_MercenaryLevelTbl.MaxLevel then
		return
	end
	if Status == 3 then
		StmtContainer._AddMercenaryLevelStatus:ExecSql("", 1, data["CharId"])
		return CheckMercenaryLevel(data)
	elseif Status == 4 then
		return -1
	end
end

function MercenaryLevelDB.GetMercenaryLevelAward(data)
	local MercenaryLevel, Status = GetMercenaryLevelInfo(data)
	if MercenaryLevel == g_MercenaryLevelTbl.MaxLevel then
		return {}
	end
	if data["Type"] then
		if Status == 4 then
			return GetMercenaryLevelAward(data)
		end
	else
		return GetMercenaryLevelAward(data)
	end
end

local function SetMercenaryLevelAward(data)
	local MercenaryLevel, Status = GetMercenaryLevelInfo(data)
	if (not MercenaryLevel) or Status~=4 or MercenaryLevel==g_MercenaryLevelTbl.MaxLevel then
		return
	end
	
	--学习回城技能特殊处理
	if data["SkillName"]=="返回树屋酒吧" or data["SkillName"]=="骑术" then
		for sub,t in pairs(g_MercenaryLevelTbl[MercenaryLevel]["特权"]) do
			if data["SkillName"] == t.Arg then
				data["Award"] = sub
				break
			end
		end
	end
	if not data["Award"] then
		return false
	end
	
	local CharId = data["CharId"]
	local Award = data["Award"]
	local result = nil
	if string.sub(Award, 1, 4)=="技能" or string.sub(Award, 1, 4)=="物品" or string.sub(Award, 1, 4)=="行为" then
		local MultiAward = string.sub(Award, 1, 4)
		result = CheckAwards[MultiAward](g_MercenaryLevelTbl[MercenaryLevel]["特权"], data)
	else
		result = CheckAwards[Award](g_MercenaryLevelTbl[MercenaryLevel]["特权"], data)
	end
	if result then
		local tbl = GetMercenaryLevelAward(data)
		if not tbl[Award] then
			StmtContainer._AddMercenaryLevelAward:ExecSql("", CharId, Award)
			if data["SkillName"]=="返回树屋酒吧" or data["SkillName"]=="骑术" then
				result = Award
			end
			return result
		end
	end
end

function MercenaryLevelDB.SetMercenaryLevelAward(data)
	return SetMercenaryLevelAward(data)
end

function MercenaryLevelDB.SlowSetMercenaryLevelAward(CharId, SkillName)
	local data = {}
	data["CharId"] = CharId
	data["SkillName"] = SkillName
	return SetMercenaryLevelAward(data)
end

function MercenaryLevelDB.InitMercenaryLevel(CharId)
	local data = {}
	data["CharId"] = CharId
	local MLRes = CheckMercenaryLevel(data)
	local Awards = GetMercenaryLevelAward(data)
	return MLRes, Awards
end

return MercenaryLevelDB
