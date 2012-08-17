gac_gas_require "shortcut/ShortcutCommon"
gac_gas_require "skill/SkillCommon"
cfg_load "skill/NonFightSkill_Common"
cfg_load "skill/SkillPart_Common"
lan_load "appellation/Lan_AppellationText_Common"

local NonFightSkill_Common = NonFightSkill_Common
local SkillPart_Common = SkillPart_Common
local Lan_AppellationText_Common = Lan_AppellationText_Common
local FightSkillKind = FightSkillKind
local loadGeniusfile = loadGeniusfile
local GetNodeName = GetNodeName
local ESkillTreeNode = ESkillTreeNode
local loadstring = loadstring
local g_PlayerLevelToSkillLevelTbl = g_PlayerLevelToSkillLevelTbl
local g_MoneyMgr = CMoney:new()
local MemH64 = MemH64
local GetLearnSkillConsume = GetLearnSkillConsume
local GetUpSkillConsume = GetUpSkillConsume
local EShortcutPieceState = EShortcutPieceState
local g_QuestNeedMgr = g_QuestNeedMgr
local g_LearnSkillMgr = g_LearnSkillMgr
local event_type_tbl = event_type_tbl
local GetClearTalentConsumeItem = GetClearTalentConsumeItem
local g_ItemInfoMgr = g_ItemInfoMgr
local FightSkill = CreateDbBox(...)
local FightSkillSql = {}
local QuestState = {
	init	= 1,
	failure	= 2,
	finish	= 3
}

local SkillNode = 
 	 { 
 	 		[1] = 1,  --索引代表结点，值对应层，共7个结点，4层
 	 		[2] = 2,
 	 		[3] = 3,
 	 		[4] = 3,
 	 		[5] = 4,
 	 		[6] = 4,
 	 		[7] = 4
 	 }
 	 
local SkillLayer = 
 	 { 
 	 		[1] = {1},  --每层对应的结点
 	 		[2] = {2},
 	 		[3] = {3,4},
 	 		[4] = {5,6,7},
 	 }
local LayerLimitTbl =
	{
		[2] = 2,
		[3] = 4,
		[4] = 7,
		[5] = 4,
		[6] = 2,
		[7] = 4,
		[8] = 2,
	}

--得到玩家的等级
local StmtDef =
{
	"_GetPlayerLevel",
	"select cb_uLevel from tbl_char_basic where cs_uId = ? "
}
DefineSql( StmtDef , FightSkillSql )

--根据角色ID查询已学天赋个数
local StmtDef = 
{
	"Dbs_SelectTalentCount",
	[[
		select count(*) from tbl_fight_skill
		where cs_uId = ? and fs_uKind = ?
	]]
}
DefineSql(StmtDef,FightSkillSql)

function FightSkill.CheckTalentPoint(PlayerId)
	local level_result = FightSkillSql._GetPlayerLevel:ExecStat(PlayerId)
	level_result = level_result:GetData(0,0)
	local allTalentNumber = 0
	if level_result < 10 then
		return false
	elseif level_result >= 10 then
		allTalentNumber= math.floor((level_result-10)/5)+1
	end
	
	local talentCount = FightSkillSql.Dbs_SelectTalentCount:ExecStat(PlayerId,FightSkillKind.Talent)
	talentCount = talentCount:GetData(0,0)
	local result = (allTalentNumber - talentCount) > 0 and true or false
	return result
end

local StmtDef=
{ 
	"Dbs_SaveFightSkill",
	"replace into tbl_fight_skill( cs_uId, fs_sName, fs_uKind ) values(?,?,?);"
}
DefineSql(StmtDef,FightSkillSql)

function FightSkill.Dbs_SaveFightSkill(PlayerId, name, fs_kind)
	FightSkillSql.Dbs_SaveFightSkill:ExecSql( "", PlayerId, name, fs_kind )
end

local StmtDef=
{ 
	"_AddSkillLayer",
	"replace into tbl_skill_layer( cs_uId, fs_sName, sl_uLayer ) values(?,?,?);"
}
DefineSql(StmtDef,FightSkillSql)

function FightSkill.AddSkillLayer(nCharID, sSkillName, nLayer)
	FightSkillSql._AddSkillLayer:ExecSql( "", nCharID, sSkillName, nLayer )
end

local StmtDef=
{ 
	"Dbs_SelectPlayerClass",
	"select cs_uClass from tbl_char_static where cs_uId = ?;"
}
DefineSql(StmtDef,FightSkillSql)

function FightSkill.Dbs_SelectPlayerClass(PlayerId)
	local ret = FightSkillSql.Dbs_SelectPlayerClass:ExecSql("n",PlayerId)
	return ret:GetData(0,0)
end

local StmtDef = 
{
	"Dbs_UpgradeFightSkill",
	[[
		update tbl_fight_skill 
		set fs_uLevel = ?
		where cs_uId = ? and fs_sName = ? and fs_uKind = ?
	]]
}
DefineSql(StmtDef,FightSkillSql)

function FightSkill.Dbs_UpgradeFightSkill(current_level, PlayerId, name, fs_kind)
	FightSkillSql.Dbs_UpgradeFightSkill:ExecSql("", current_level, PlayerId, name, fs_kind)
end

--判断数据
local StmtDef = 
{
	"Dbs_SelectFightSkill",
	[[
		select fs_uLevel from tbl_fight_skill
		where cs_uId = ? and fs_sName = ? and fs_uKind = ?
	]]
}
DefineSql(StmtDef,FightSkillSql)

local StmtDef = 
{
	"Dbs_InsertFightSkill",
	[[
		insert into tbl_fight_skill(cs_uId,fs_sName,fs_uLevel,fs_uKind) values (?,?,?,?)
	]]
}
DefineSql(StmtDef,FightSkillSql)

--存储天赋技能
function FightSkill.Dbs_SaveTalentSkill(PlayerId,TalentName,Level)
	local res = FightSkillSql.Dbs_SelectFightSkill:ExecStat(PlayerId, TalentName, FightSkillKind.TalentSkill)
	if res:GetRowNum() == 0 then
		FightSkillSql.Dbs_InsertFightSkill:ExecStat(PlayerId, TalentName,Level,FightSkillKind.TalentSkill)
		if g_DbChannelMgr:LastAffectedRowNum() <= 0 then
			CancelTran()
			return false
		end
		return true,Level
	else
		return true	,res:GetData(0,0)
	end
end

--函数功能:从数据库中查询指定玩家指定技能的等级
--参数:data:玩家ID,技能名，技能类型，返回结果集的行数是否>0,和等级(初始为0)
function FightSkill.Lua_SelectFightSkill(data)
	local PlayerId = data["PlayerId"]
	local SkillName = data["SkillName"]
	local fs_kind = data["fs_kind"]
	local res = FightSkillSql.Dbs_SelectFightSkill:ExecStat(PlayerId, SkillName, fs_kind)
	--Renton:返回技能的等级
	local skilllevel_tbl={}
	local skilllevel = -1
	if res:GetRowNum() > 0 then
		table.insert(skilllevel_tbl,res:GetData(0,0))
		skilllevel = skilllevel_tbl[1]
		--print("skilllevel"..skilllevel)
	end
	return res:GetRowNum() > 0,skilllevel
end

--学习非战斗技能
function FightSkill.LuaDB_AddNonFightSkill(data)
	local SkillName = data["SkillName"]
	local PlayerId = data["PlayerId"]
	local sceneName = data["sceneName"]
	
	local skill = NonFightSkill_Common(SkillName)
	if skill == nil then
		return false,2038
	end
	
	local res = FightSkillSql.Dbs_SelectFightSkill:ExecStat(PlayerId, SkillName, FightSkillKind.NonFightSkill)
	if res:GetRowNum() ~= 0 then
		return false,2037
	end	
	
	FightSkill.Dbs_SaveFightSkill(PlayerId, SkillName, FightSkillKind.NonFightSkill )	--DBS
	if g_DbChannelMgr:LastAffectedRowNum() <= 0 then
		CancelTran()
		return false,2038
	end
	
	FightSkill.Dbs_UpgradeFightSkill(1, PlayerId, SkillName, FightSkillKind.NonFightSkill)
	if g_DbChannelMgr:LastAffectedRowNum() <= 0 then
		CancelTran()
		return false,2038
	end
	
	local IsSaveShortCut = false
	if SkillName == "气魂击" then --学会气魂击时把 气魂击 自动放到快捷栏 空格位置
		local ShortCutDB = RequireDbBox("ShortcutDB")
		ShortCutDB.Player_SaveShortcut(PlayerId, 10, EShortcutPieceState.eSPS_Skill, "气魂击", 1, 1)
		IsSaveShortCut = true
	end
	
	--加任务计数
	local QuestTbl = nil
	local QuestInfo = data["QuestInfo"]
	if QuestInfo then
		local param = {}
		param["iNum"] = 1
		param["sQuestName"] = QuestInfo[1]
		param["sVarName"] = QuestInfo[2]
		param["char_id"] = PlayerId
		local RoleQuestDB = RequireDbBox("RoleQuestDB")
		local IsTrue = RoleQuestDB.AddQuestVarNum(param)
		if IsTrue then
			QuestTbl = param
		else
			CancelTran()
			return false,0
		end
	end
	
	--低速加佣兵等级指引计数
	--local MercenaryLevelDB = RequireDbBox("MercenaryLevelDB")
	--local MLRes = MercenaryLevelDB.SlowAddMercenaryLevelCount(PlayerId, "气魂击")
	--佣兵等级特权计数
	--local MLPower = nil
	--if SkillName == "返回树屋酒吧" or SkillName == "骑术" then
	--	MLPower = MercenaryLevelDB.SlowSetMercenaryLevelAward(PlayerId, SkillName)
	--	if MLPower == nil then
	--		CancelTran()
	--		return false,0
	--	end
	--end
	
	local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.SavePlayerSkillInfo( PlayerId,SkillName,1,0,1)
			
	local tbl = 
	{
		["SkillName"] = SkillName,
		["QuestInfo"] = QuestTbl,
		--["MLRes"] = MLRes,
		--["MLPower"] = MLPower,
		["IsSaveShortCut"] = IsSaveShortCut
	}
	return true,tbl
end

--只留给GM指令使用,需要优化
function FightSkill.Lua_AddFightSkill(data)
	local name = data["name"]
	local level = data["level"]
	local fs_kind = data["kind"]
	local PlayerId = data["PlayerId"]
	local Class = data["Class"]
	local PlayerLevel = data["PlayerLevel"]
	local IsGMcmd = data["IsGMcmd"]
	local layer  = data["layer"]
	local sceneName = data["sceneName"]
	local g_LogMgr = RequireDbBox("LogMgrDB")
	 
	--判断该技能是否已学习
	if not IsGMcmd then 
		local res = FightSkillSql.Dbs_SelectFightSkill:ExecSql("n", PlayerId, name, fs_kind)
		if res:GetRowNum() ~= 0 then
			return false,2037
		end
	end

	local current_level = 0
	if fs_kind == FightSkillKind.Talent then
		--学习天赋判断
		local TalentTbl =  loadGeniusfile(Class)
		if not TalentTbl(name) then
			return false,2037
		end
		if SkillPart_Common(name) then
			local LevelTbl  = g_PlayerLevelToSkillLevelTbl[name]
			if #(LevelTbl) > 1 then
				current_level = LevelTbl[1][3]
			else
				current_level = PlayerLevel
			end
			 
		else
			current_level =  level + 10
		end
		
		--天赋点判断 Todo
		if not FightSkill.CheckTalentPoint(PlayerId) then
			return false,2035 
		end	
		
		if layer > 0 then
			local tblLayerCount = FightSkillSql._GetTalentNameByLayer:ExecStat(PlayerId,layer)
			if tblLayerCount:GetNumber(0,0) < LayerLimitTbl[layer] then
				--上一阶段学习技能数量不够，不能学习该阶段
				return false,1606
			end		
			--添加阶段信息
			--layer为0说明是公共技能，不用存
			FightSkill.AddSkillLayer(PlayerId, name, layer )	
			if g_DbChannelMgr:LastAffectedRowNum() <= 0 then
				CancelTran()
				return false,1600
			end
		end
	elseif fs_kind == FightSkillKind.Skill then
		--学习技能判断
		local skill = SkillPart_Common(name)
		if skill == nil then
			return false,2037
		end
		if skill("ClassLimit") ~= Class then
			return false,2036
		end	
			
		current_level = level + 1
	end
	
	FightSkill.Dbs_SaveFightSkill(PlayerId, name, fs_kind )	--DBS
	if g_DbChannelMgr:LastAffectedRowNum() <= 0 then
		CancelTran()
		return false,2038
	end
	
	FightSkill.Dbs_UpgradeFightSkill(current_level, PlayerId, name, fs_kind)
	if g_DbChannelMgr:LastAffectedRowNum() <= 0 then
		CancelTran()
		return false,2038
	end

	if fs_kind == FightSkillKind.Talent then --1代表学习 2洗天赋 
		g_LogMgr.SavePlayerTelentInfo( PlayerId,name,1 ) 
	elseif fs_kind == FightSkillKind.Skill then --1代表学习，2代表升级
		g_LogMgr.SavePlayerSkillInfo( PlayerId,name,current_level,0,0)
	end
	
	local tbl = 
	{
		["kind"] = fs_kind,
		["current_level"] = current_level,
		["IsGMcmd"] = IsGMcmd,
		["name"] = name,
		["QuestInfo"] = QuestTbl,
	}
	return true,tbl
end

function FightSkill.LuaDB_AddTalentSkill(data)
	local TalentName = data["TalentName"]
	local PlayerId = data["PlayerId"]
	local PlayerLevel = data["PlayerLevel"]
	local Class = data["Class"]
	local Layer  = data["TreeNode"]
	local SceneName = data["SceneName"]
	
	--判断该天赋是否学习
	local res = FightSkillSql.Dbs_SelectFightSkill:ExecStat(PlayerId, TalentName, FightSkillKind.Talent)
	if res:GetRowNum() ~= 0 then
		return false,2037
	end

	local TalentTbl =  loadGeniusfile(Class)
	if not TalentTbl(TalentName) then
		return false,2037
	end
	
	--天赋点判断
	if not FightSkill.CheckTalentPoint(PlayerId) then
		return false,2035 
	end	
	
	if Layer >= 2 then
		--判断学习该阶段天赋点总数是否正确
		local nTalentCount = 0
		if Layer == ESkillTreeNode.eSTN_MainFirst or Layer == ESkillTreeNode.eSTN_MainSecond
			 or Layer == ESkillTreeNode.eSTN_MainThird or Layer == ESkillTreeNode.eSTN_MainFourth 
			 or Layer == ESkillTreeNode.eSTN_LeftFirst or Layer == ESkillTreeNode.eSTN_RightFirst then
			nTalentCount = FightSkillSql._CountMainTalentNameByLayer:ExecStat(PlayerId,Layer)
		elseif Layer == ESkillTreeNode.eSTN_LeftSecond or Layer == ESkillTreeNode.eSTN_RightSecond then
			nTalentCount = FightSkillSql._CountTalentNameByLayer:ExecStat(PlayerId,Layer-1)
		else
			return false,1606
		end

		if nTalentCount:GetData(0,0) < LayerLimitTbl[Layer] then
			return false,1606
		end
	elseif Layer ~= 1 then
		return false,1600
	end
	
		--添加阶段信息
	FightSkill.AddSkillLayer(PlayerId, TalentName, Layer )	
	if g_DbChannelMgr:LastAffectedRowNum() <= 0 then
		CancelTran()
		return false,1600
	end	
	--判断是否为天赋技能
	local current_level = 0
	if SkillPart_Common(TalentName) and tonumber(SkillPart_Common(TalentName,"ClassLimit")) == Class then 
		local LevelTbl  = g_PlayerLevelToSkillLevelTbl[TalentName]
		if #(LevelTbl) > 1 then
			current_level = LevelTbl[1][3]
		else
			current_level = PlayerLevel 
		end
		local result,level = FightSkill.Dbs_SaveTalentSkill(PlayerId,TalentName,current_level)
		if result then
			current_level = level
		else
			return false,2038
		end			
	else
		current_level =  10
	end	
	
	FightSkill.Dbs_SaveFightSkill(PlayerId, TalentName, FightSkillKind.Talent )
	if g_DbChannelMgr:LastAffectedRowNum() <= 0 then
		CancelTran()
		return false,2038
	end
	
	FightSkill.Dbs_UpgradeFightSkill(10, PlayerId, TalentName, FightSkillKind.Talent)
	if g_DbChannelMgr:LastAffectedRowNum() <= 0 then
		CancelTran()
		return false,2038
	end

	local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.SavePlayerTelentInfo( PlayerId,TalentName,1 ) --1代表学习 2洗天赋 
	local tbl = 
	{
		["current_level"] = current_level,
		["name"] = TalentName,
		["kind"] = FightSkillKind.Talent
	}
	return true,tbl	
end

function FightSkill.Lua_UpdateFightSkill(data)
	local name = data["name"]
	local type = data["ActionType"] --type为0代表学习，1代表升级
	local PlayerId = data["PlayerId"]
	local Class = data["Class"]
	local PlayerLevel = data["PlayerLevel"]
	local SkillLevel = data["SkillLevel"]
	local g_LogMgr = RequireDbBox("LogMgrDB")
	local sceneName = data["sceneName"]
	local fs_kind = 0
	if SkillPart_Common(name) and SkillPart_Common(name,"SkillLearnType") and SkillPart_Common(name,"ClassLimit") ~= 0 then
		if tonumber(SkillPart_Common(name,"SkillLearnType")) == 4 then
			fs_kind = FightSkillKind.Skill
		elseif tonumber(SkillPart_Common(name,"SkillLearnType")) == 1 then
			fs_kind = FightSkillKind.TalentSkill
		end
	else 
		return false,2038
	end	

	--判断该技能是否已学习
	local res = FightSkillSql.Dbs_SelectFightSkill:ExecSql("n", PlayerId, name,fs_kind)
	if type == 1 and res:GetRowNum() == 0 then
		return false,2037
	end

	local next_level,need_Level = 0,0
	local LevelTbl  = g_PlayerLevelToSkillLevelTbl[name]
	if type == 0 then --学习公共技能
		if SkillPart_Common(name,"SkillLearnType") == 4 then
			if PlayerLevel < SkillPart_Common(name,"NeedLevel") then
				return false,2038 --"你的等级还不够"
			end
		end		
		if (#(LevelTbl) == 1 and tostring(LevelTbl[1][3]) == "l") then
			next_level = PlayerLevel
		else
			next_level = LevelTbl[1][3]
		end
		need_Level = SkillPart_Common(name,"NeedLevel")
	elseif type == 1 and #(LevelTbl) > 1 then --升级公共技能和天赋技能
		local current_level = res:GetData(0,0)
		for i = 1 ,#(LevelTbl) do
			if LevelTbl[i][3] == current_level then
				if LevelTbl[i+1] == nil then --达到最高等级
					return false,2038
				end
				if PlayerLevel >= LevelTbl[i+1][1] then
					next_level = LevelTbl[i+1][3]
					need_Level = LevelTbl[i+1][1]
				else
					return false,2038 --"你的等级还不够"
				end
				break
			elseif  LevelTbl[i][3] > current_level then
				next_level = LevelTbl[i][3]
				need_Level = LevelTbl[i][1]
				break
			end
		end	
	end	
	
	if need_Level > 80 then
		return false,2038
	end
	
--	if need_Level >= 10 then
--		local tblSysCount = FightSkillSql._CountSys:ExecSql('n',PlayerId)
--		if tblSysCount:GetData(0,0) == 0 then
--			if type == 0 then
--				return false,2034
--			else
--				return false,2033
--			end
--		end	
--	end
	
	local money_count,soul_count = 0,0
	if type == 0 then
		local ConsumeTbl = GetLearnSkillConsume(name)
		soul_count = ConsumeTbl.SoulNumber
		money_count = ConsumeTbl.MoneyNumber
	elseif type == 1 then
		local  ConsumeTbl = GetUpSkillConsume(name,need_Level)
		soul_count = ConsumeTbl.SoulNumber
		money_count = ConsumeTbl.MoneyNumber
	end	
    local fun_info = g_MoneyMgr:GetFuncInfo("LearnSkill")
    g_LogMgr.SavePlayerSkillInfo( PlayerId,name,next_level,type,0)
    local MoneyManagerDB=	RequireDbBox("MoneyMgrDB")
	local suc, msgID, moneyCount = MoneyManagerDB.SubtractMoneyBySysFee(money_count, PlayerId, fun_info,fun_info["LearnSkill"],event_type_tbl["技能学习管理费"])
	local add_Money = moneyCount
	local add_bMoney = msgID
	if suc == false then
        return false, msgID
	end
	    
	local tblSoul = {}
	tblSoul["PlayerId"]  = PlayerId
	tblSoul["addSoulType"]  = event_type_tbl["技能学习消耗魂"]
	tblSoul["soulCount"] = -soul_count
	local g_EquipMgr= RequireDbBox("EquipMgrDB")
	local Flag,SoulRet = g_EquipMgr.ModifyPlayerSoul(tblSoul)
	if not Flag then
		CancelTran()
		return false,2038
	end
	
	if type == 0 then
		FightSkill.Dbs_SaveFightSkill(PlayerId, name, fs_kind )	--DBS
		if g_DbChannelMgr:LastAffectedRowNum() <= 0 then
			CancelTran()
			return false,2038
		end
	end
	--Renton,得到该技能可能对应的任务,技能等级得换算
	--备注，有张表的TABLE没转
	local skillname = data["name"]
	--技能等级1
	local now_skill_level = 0
	local temptbl = loadstring("return "..SkillPart_Common(skillname,"PlayerLevelToSkillLevel"))()
	for i,v in ipairs(temptbl) do
		if v[3] == SkillLevel then
			now_skill_level = i
		end
	end
	--技能等级2
	local next_skill_level = now_skill_level + 1
	--数值等级2
	local next_skill_level_value = -1
	if temptbl[next_skill_level] then
		next_skill_level_value = temptbl[next_skill_level][3]
	end
	local RoleQuestDB = RequireDbBox("RoleQuestDB")
	local datatbl = {}
	datatbl[1] = PlayerId
	local restbl = nil
	local varname = ""
	local skillQuestTbl = {}
	--判断是否有任务并加计数
	--待重构，有可能以后有多个任务对同一个技能的同一等级都有需求，需要把结果存在一个table并返回并提示客户端
	if g_LearnSkillMgr[skillname] ~= nil then
		if g_LearnSkillMgr[skillname][next_skill_level] ~= nil then
			for _,v in ipairs(g_LearnSkillMgr[skillname][next_skill_level]) do
				datatbl[2] = v
				restbl = RoleQuestDB.GetQuestState(datatbl)
				if restbl == QuestState.init then
					local addnum_tbl= {}
					addnum_tbl["iNum"] = 1
					addnum_tbl["char_id"] = PlayerId
					addnum_tbl["sQuestName"] = datatbl[2]
					varname = skillname..next_skill_level
					addnum_tbl["sVarName"] = varname
					local IsTrue = RoleQuestDB.AddQuestVarNum(addnum_tbl)
		
					if IsTrue then
						table.insert(skillQuestTbl,addnum_tbl)
					else
						CancelTran()
						return false,0
					end
				end
			end
		end
	end
------------------------------------------------------------------------------------	
	FightSkill.Dbs_UpgradeFightSkill(next_level, PlayerId, name,fs_kind)
	if g_DbChannelMgr:LastAffectedRowNum() <= 0 then
		CancelTran()
		return false,2038
	end
	
	local ShortcutInfo
	if type == 0 then
		local ShortCutDB = RequireDbBox("ShortcutDB")
		ShortcutInfo = ShortCutDB.InitSkillState(PlayerId,name,Class)
	end
	
	local tbl = 
	{
		["current_level"] = next_level,
		["name"] = name,
		["money_count"] = add_Money,
		["bmoney_count"] = add_bMoney,
		["SoulRet"] = SoulRet,
		["ShortcutInfo"] = ShortcutInfo
	}
	return true,tbl,skillQuestTbl
end

-------------------------------主修系、节点、阶段-------------------------------
local StmtDef =
{
	"_CountSys",
	"select count(*) from tbl_skill_Series where cs_uId = ?;"
}
DefineSql(StmtDef,FightSkillSql)

local StmtDef =
{
	"_AddSys",
	"replace into tbl_skill_Series(cs_uId,ss_uSeries) values(?,?);"
}
DefineSql(StmtDef,FightSkillSql)

local StmtDef =
{
	"_AddNode",
	"replace into tbl_skill_node(cs_uId,sn_uNode) values(?,?);"
}
DefineSql(StmtDef,FightSkillSql)

function FightSkill.IsSeries(nCharID)
	local tblSysCount = FightSkillSql._CountSys:ExecStat(nCharID)
	if tblSysCount:GetData(0,0) > 0 then
		return true
	else
		return false
	end 
end

local CampIDToQuestNameTbl = {
								[1] = "小佣兵的转职之路（艾米）",
								[2] = "小佣兵的转职之路（神圣）",
								[3] = "小佣兵的转职之路（修斯）"
							}

local QuestStateTbl = {
	init	= 1, 
	failure	= 2,
	finish	= 3
}

--选择主修系
function FightSkill.SetSeriesDB(data)
	local nSys = data["nSys"]
	local nCharID = data["nCharID"]
	local classId = data["ClassID"]
	local CampID = data["CampID"]
	
--	local RoleQuestDB = RequireDbBox("RoleQuestDB")
--	local QuestName = CampIDToQuestNameTbl[CampID]
--	local QuestState = RoleQuestDB.GetQuestState({nCharID,QuestName})
--	if QuestState ~= QuestStateTbl.init and QuestState ~= QuestStateTbl.finish then
--		return false,1608
--	end

	--已经修过
	local tblSysCount = FightSkillSql._CountSys:ExecSql('n',nCharID)
	if tblSysCount:GetData(0,0) > 0 then
		return false
	end
	
	FightSkillSql._AddSys:ExecSql('',nCharID,nSys)
	if g_DbChannelMgr:LastAffectedRowNum() <= 0 then
		CancelTran()
		return false
	end
	
	FightSkillSql._AddNode:ExecSql('',nCharID,1)
	if g_DbChannelMgr:LastAffectedRowNum() <= 0 then
		CancelTran()
		return false
	end
	
	local FirstTimeName = "玩家转职"
	local NoviceDirectDB = RequireDbBox("NoviceDirectDB")
	local TimeCount = NoviceDirectDB.GetPlayerFirstTimeCount(nCharID,FirstTimeName)
	local TalentSkillTbl = {}
	if TimeCount < 1 then --第一次转职时主动学习第一个天赋
		NoviceDirectDB.InsertPlayerFirstFinishInfo({["PlayerId"] = nCharID,["FirstTimeName"] = "玩家转职"})
		if g_DbChannelMgr:LastAffectedRowNum() <= 0 then
			CancelTran()
			return false,2038
		end	
		
		local current_level = 0
		local TalentName = ""
		local TalentTbl =  loadGeniusfile(classId)
		local KeysTbl = TalentTbl:GetKeys()
		for i ,p in pairs(KeysTbl) do 
			if TalentTbl(p,"GiftSkillSeries") == nSys and TalentTbl(p,"ViewPosition") == 1 then
				TalentName = p
				FightSkill.AddSkillLayer(nCharID,TalentName,1)	
				if g_DbChannelMgr:LastAffectedRowNum() <= 0 then
					CancelTran()
					return false,2038
				end	
						
				FightSkill.Dbs_SaveFightSkill(nCharID, TalentName, FightSkillKind.Talent )	--DBS
				if g_DbChannelMgr:LastAffectedRowNum() <= 0 then
					CancelTran()
					return false,2038
				end
	
				if SkillPart_Common(TalentName) and tonumber(SkillPart_Common(TalentName,"ClassLimit")) == classId then 
					local LevelTbl  = g_PlayerLevelToSkillLevelTbl[TalentName]
					if #(LevelTbl) > 1 then
						current_level = LevelTbl[1][3]
					else
						current_level = 10 
					end
					local result,level = FightSkill.Dbs_SaveTalentSkill(nCharID,TalentName,current_level)
					if result then
						current_level = level
					else
						return false,2038
					end			
				else
					current_level =  10
				end	
	
				FightSkill.Dbs_UpgradeFightSkill(current_level, nCharID, TalentName, FightSkillKind.Talent)
				if g_DbChannelMgr:LastAffectedRowNum() <= 0 then
					CancelTran()
					return false,2038
				end
				TalentSkillTbl = {
									["name"] = TalentName,
									["kind"] = FightSkillKind.Talent,
									["current_level"] = current_level
								}
				break
			end
		end
	end
	
	--任务事件需求加计数
	local param = {}
	param["char_id"] = nCharID
	param["sVarName"] = "完成转职"
	param["iNum"] = 1
	param["CanShareTeamMateTbl"] = {}
	local RoleQuestDB = RequireDbBox("RoleQuestDB")
	local QuestVarTbl = RoleQuestDB.AddVarNumForTeamQuests(param)
	if QuestVarTbl and QuestVarTbl[1] and QuestVarTbl[1][nCharID][1][3] == false then
		CancelTran()
		return false
	end
	
	--低速加佣兵等级指引计数
	--local MercenaryLevelDB = RequireDbBox("MercenaryLevelDB")
	--local MLRes = MercenaryLevelDB.SlowAddMercenaryLevelCount(nCharID, "转职")
	local tbl = {
					--["MLRes"] = MLRes,
					["QuestVarTbl"] = QuestVarTbl,
					["TalentSkillTbl"] = TalentSkillTbl,
				}
	return true,tbl
end

local StmtDef =
{
	"_GetSys",
	"select ss_uSeries from tbl_skill_Series where cs_uId = ?;"
}
DefineSql(StmtDef,FightSkillSql)
local StmtDef =
{
	"_GetNodes",
	"select distinct(sl_uLayer) from tbl_skill_layer where cs_uId = ?;"
}
DefineSql(StmtDef,FightSkillSql)

--获得主修系，技能，结点信息
function FightSkill.GetSeriesDB(data)
	local nCharID = data["nCharID"]
	--主修系
	local tblSeries = FightSkillSql._GetSys:ExecSql('n',nCharID)
	local series = 0
	if tblSeries:GetRowNum() ~= 0 then
		series = tblSeries:GetNumber(0,0)
	end
	
	--技能
	local SkillRet = FightSkill.Dbs_LoadFightSkill(nCharID,true)
	
	--结点
	local tblNodes = FightSkillSql._GetNodes:ExecSql('n',nCharID)
	local row = tblNodes:GetRowNum()
	local Nodes = {}
	for i=1,row do
		table.insert(Nodes,tblNodes:GetNumber(i-1,0))
	end	
	
	return series,SkillRet,Nodes
end

--获得主修系
function FightSkill.NpcGetSeriesDB(data)
	local nCharID = data["nCharID"]
	
	--主修系
	local tblSeries = FightSkillSql._GetSys:ExecSql('n',nCharID)
	local series = 0
	if tblSeries:GetRowNum() ~= 0 then
		series = tblSeries:GetNumber(0,0)
	end
	return series
end

local StmtDef =
{
	"_CountMainTalentNameByLayer",
	"select  count(*) from tbl_skill_layer where cs_uId = ? and sl_uLayer < ?;"
}
DefineSql(StmtDef,FightSkillSql)

local StmtDef =
{
	"_CountTalentNameByLayer",
	"select  count(*) from tbl_skill_layer where cs_uId = ? and sl_uLayer = ?;"
}
DefineSql(StmtDef,FightSkillSql)

local StmtDef =
{
	"_GetUpperNode",
	"select sn_uNode from tbl_skill_node where cs_uId = ? order by sn_uNode desc limit 1;"
}
DefineSql(StmtDef,FightSkillSql)

--判断该层是否已经学习了
function FightSkill.IsStudyALayer(nCharID,layer)
		local str = "select count(*) from tbl_skill_node  where cs_uId = " .. nCharID .. " and sn_uNode in("
		local list_str = ""
		local nodes = SkillLayer[layer]
		for i =1, #nodes-1 do
			list_str = list_str .. nodes[i] .. ", "
		end
		list_str = list_str .. nodes[#nodes] .. ")"
		local query_string = str .. list_str
		local _,result = g_DbChannelMgr:TextExecute(query_string)
		return result:GetNumber(0,0) > 0 
end
----------------------------------------洗天赋---------------------------------------
--查询所有的天赋信息
local StmtDef =
{
	"_GetAllGenius",
	"select fs_sName from tbl_fight_skill where cs_uId = ? and fs_uKind = 2 ;"
}
DefineSql(StmtDef,FightSkillSql)

--删除所有的天赋
local StmtDef =
{
	"Dbs_ClearAllGenius",
	"delete from tbl_fight_skill where cs_uId = ? and fs_uKind = 2 ;"
}
DefineSql(StmtDef,FightSkillSql)

--删除所有天赋对应的节点
local StmtDef =
{
	"Dbs_ClearAllSkillNode",
	"delete from tbl_skill_node where cs_uId = ?;"
}
DefineSql(StmtDef,FightSkillSql)

--删除该玩家对应的主修系
local StmtDef =
{
	"Dbs_ClearSkillSeries",
	"delete from tbl_skill_Series where cs_uId = ?;"
}
DefineSql(StmtDef,FightSkillSql)

--删除所有技能所属阶段
local StmtDef =
{
	"Dbs_ClearSkillLayer",
	"delete from tbl_skill_layer where cs_uId = ?;"
}
DefineSql(StmtDef,FightSkillSql)

----------------------------------获得所有技能信息-------------------------------------------
local StmtDef=
{ 
	"Dbs_LoadFightSkill",
	[[
		select tfs2.fs_sName, tfs2.fs_uLevel, tfs2.fs_uKind from tbl_fight_skill as tfs1 , tbl_fight_skill as tfs2 
		where tfs1.fs_sName = tfs2.fs_sName and tfs1.fs_uKind = 2 and tfs2.fs_uKind = 5 and tfs1.cs_uId = tfs2.cs_uId and tfs1.cs_uId = ? 
		union 
		select fs_sName, fs_uLevel, fs_uKind from tbl_fight_skill where cs_uId = ? and fs_uKind != 5;
	]]
}
DefineSql(StmtDef,FightSkillSql)

local StmtDef=
{ 
	"Dbs_LoadFightSkillMerge",
	[[ 
		select tfs1.fs_sName, ifnull( tfs2.fs_uLevel,tfs1.fs_uLevel), tfs1.fs_uKind from tbl_fight_skill as tfs1 
		left join tbl_fight_skill as tfs2 
		on
		(tfs1.fs_sName = tfs2.fs_sName and  tfs1.cs_uId = tfs2.cs_uId and tfs2.fs_uKind = 5 ) where tfs1.fs_uKind = 2 and tfs1.cs_uId = ? 
		union 
		select fs_sName, fs_uLevel, fs_uKind from tbl_fight_skill where fs_uKind not in (2,5) and cs_uId = ?; 
	]]
}
DefineSql(StmtDef,FightSkillSql)

function FightSkill.Lua_ClearAllGenius(data)
	local PlayerId 		= data["PlayerId"]
	local PlayerLevel 	= data["PlayerLevel"]
	local ConsumeItemName = ""
	local ConsumeBigID = g_ItemInfoMgr:GetBasicItemBigID()
	local ConsumeItemNumber = 0	
	
	local ConsumeItemTbl = GetClearTalentConsumeItem(PlayerLevel)
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")	
	for i , ItemTbl in pairs (ConsumeItemTbl["ConsumeItem"]) do
		local ItemNum =  g_RoomMgr.GetItemNum({["nCharId"] = PlayerId,["ItemType"] = ConsumeBigID, ["ItemName"] = ItemTbl[1]})
		if ItemNum >= ItemTbl[2] then
			ConsumeItemName = ItemTbl[1]
			ConsumeItemNumber = ItemTbl[2]
			break
		end
	end
	
	--转职消耗物品
	local DelItemResult = nil
	if ConsumeItemNumber and ConsumeItemNumber >= 0 and ConsumeItemName and ConsumeItemName ~= "" then
		DelItemResult = g_RoomMgr.DelItem(PlayerId,ConsumeBigID,ConsumeItemName,ConsumeItemNumber,nil,event_type_tbl["玩家转职"])
		if IsNumber(DelItemResult) and ConsumeItemNumber > 0 then
			CancelTran()
			return false,2039
		end
		
	else
		return false,2039
	end
	
	--转职扣钱
	local MoneyManagerDB = RequireDbBox("MoneyMgrDB")
	local FunInfo = g_MoneyMgr:GetFuncInfo("ClearTalent")
	local Suc, BindingMoney, Money = MoneyManagerDB.SubtractMoneyBySysFee(ConsumeItemTbl["ConsumeMoney"], PlayerId, FunInfo,FunInfo["ClearTalent"], event_type_tbl["玩家转职"])
	if not Suc then
        return false, BindingMoney
	end		
	
	--转职扣魂
	local SoulTbl = {}
	SoulTbl["PlayerId"]  	= PlayerId
	SoulTbl["addSoulType"]  = event_type_tbl["玩家转职"]		
	SoulTbl["soulCount"] 	= -ConsumeItemTbl["ConsumeSoul"]
	local EquipMgr 		= RequireDbBox("EquipMgrDB")
	local Suc,SoulRet 	= EquipMgr.ModifyPlayerSoul(SoulTbl)	
	if not Suc then
		CancelTran()
		return false,2039
	end
		
	local skills = FightSkill.Dbs_LoadFightSkill(PlayerId)
	local tblSeries = FightSkillSql._GetSys:ExecSql('n',PlayerId)
	if tblSeries:GetRowNum() == 0 then
		return false,2039
	end
	
	local nSys = tblSeries:GetData(0,0)
	FightSkillSql.Dbs_ClearSkillSeries:ExecSql("",PlayerId )
	if g_DbChannelMgr:LastAffectedRowNum() <= 0 then
		CancelTran()
		return false,2039
	end
	local AllGenius =  FightSkillSql._GetAllGenius:ExecStat(PlayerId)
	if AllGenius:GetRowNum() > 0 then
		local g_LogMgr = RequireDbBox("LogMgrDB")
		for i =1,AllGenius:GetRowNum() do
			g_LogMgr.SavePlayerTelentInfo( PlayerId,AllGenius(i,1),2 )
		end
	end
	FightSkillSql.Dbs_ClearAllGenius:ExecSql("",PlayerId )
	FightSkillSql.Dbs_ClearAllSkillNode:ExecSql("",PlayerId )
	FightSkillSql.Dbs_ClearSkillLayer:ExecSql("",PlayerId )
	local ResultTbl = 
					{
						["Skills"] 			= skills,
						["DelItemResult"] 	= DelItemResult,
						["BindingMoney"] 	= BindingMoney,
						["Money"]			= Money,
						["SoulRet"]			= SoulRet,
					}
	return true,ResultTbl
end

function FightSkill.Dbs_LoadFightSkill(PlayerId,IsMergeTalent)
	local SkillRet
	if IsMergeTalent then
		SkillRet = FightSkillSql.Dbs_LoadFightSkillMerge:ExecStat( PlayerId,PlayerId )
	else		
		SkillRet = FightSkillSql.Dbs_LoadFightSkill:ExecStat( PlayerId,PlayerId )
	end
	local row = SkillRet:GetRowNum()
	local SkillRetTbl = {}
	for i=1,row do
		local name = SkillRet:GetData(i-1,0)
		local level = SkillRet:GetData(i-1,1)
		local kind 	= SkillRet:GetData(i-1,2)
		table.insert(SkillRetTbl,{name,level,kind})
	end	
	return SkillRetTbl
end


------------------------------普攻技能----------------------------------------
--得到普攻技能
local StmtDef=
{ 
	"Dbs_LoadCommonlyFightSkill",
	"select fs_sName, fs_uLevel, fs_uKind from tbl_fight_skill where cs_uId = ? and fs_uKind = ?;"
}
DefineSql(StmtDef,FightSkillSql)

function FightSkill.Dbs_LoadCommonlyFightSkill(PlayerId)
	local SkillRet = FightSkillSql.Dbs_LoadCommonlyFightSkill:ExecSql( "s[32]nn", PlayerId,FightSkillKind.NormSkill )
	return SkillRet
end

------------------------------------召唤兽相关--------------------------------------
--修改召唤兽的名字RealName
local StmtDef=
{ 
	"Dbs_SetServantName",
	"replace into tbl_char_servant_name( cs_uId, csn_sServantResName, csn_sServantRealName ) values(?,?,?);"
}
DefineSql(StmtDef,FightSkillSql)

function FightSkill.Dbs_SetServantName(data)
	local PlayerId 				= data["PlayerId"]
	local ServantResName 		= data["ServantResName"]
	local ServantRealNameName 	= data["ServantRealNameName"]
	FightSkillSql.Dbs_SetServantName:ExecSql("", PlayerId, ServantResName, ServantRealNameName)
	if g_DbChannelMgr:LastAffectedRowNum() <= 0 then
		CancelTran()
		return false
	end
	return true
end

--得到召唤兽的名字RealName
local StmtDef=
{ 
	"Dbs_SelectServantName",
	"select csn_sServantRealName from tbl_char_servant_name where cs_uId = ? and csn_sServantResName = ?;"
}
DefineSql(StmtDef,FightSkillSql)

function FightSkill.Dbs_SelectServantName(data)
	local PlayerId 			= data["PlayerId"]
	local ServantResName 	= data["ServantResName"]
	local NameRet = FightSkillSql.Dbs_SelectServantName:ExecSql("s[32]", PlayerId, ServantResName)
	return NameRet
end

--得到召唤兽的名字RealName
local StmtDef=
{ 
	"Dbs_SelectAllServantName",
	"select csn_sServantResName,csn_sServantRealName from tbl_char_servant_name where cs_uId = ?;"
}
DefineSql(StmtDef,FightSkillSql)

function FightSkill.Dbs_SelectAllServantName(PlayerId)
	local NameRet = FightSkillSql.Dbs_SelectAllServantName:ExecSql("s[32]s[32]", PlayerId)
	return NameRet
end


SetDbLocalFuncType(FightSkill.LuaDB_AddNonFightSkill)
SetDbLocalFuncType(FightSkill.Lua_SelectFightSkill) 
SetDbLocalFuncType(FightSkill.Lua_UpdateFightSkill)
SetDbLocalFuncType(FightSkill.SetSeriesDB)
SetDbLocalFuncType(FightSkill.GetSeriesDB)
SetDbLocalFuncType(FightSkill.Lua_ClearAllGenius)
SetDbLocalFuncType(FightSkill.Dbs_SetServantName)
SetDbLocalFuncType(FightSkill.LuaDB_AddTalentSkill)
return FightSkill
