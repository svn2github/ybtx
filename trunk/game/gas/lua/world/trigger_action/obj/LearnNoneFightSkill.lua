--加buff
gas_require "world/int_obj/IntObjServer"
cfg_load "skill/NonFightSkill_Common"

local GetDistance = g_GetDistance
local IntObjServerMgr = g_IntObjServerMgr
local g_ObjActionArgTbl = g_ObjActionArgTbl
local ServerBeginProgress = ServerBeginProgress
local EActionState = EActionState
local FightSkillKind = FightSkillKind
local NonFightSkill_Common = NonFightSkill_Common
local FightSkillKind = FightSkillKind
local MsgToConn = MsgToConn
local CallAccountManualTrans = CallAccountManualTrans
local IsCppBound = IsCppBound

local g_GetPlayerInfo = g_GetPlayerInfo
local Entry = CreateSandBox(...)

local function LearnSkill(Conn, Target, TargetEntityID)
	local function ProgressSucc(Player, data)
		local Obj = data["Obj"]
		local GlobalID = data["GlobalID"]
		local SkillName = data["SkillName"]
		local QuestInfo = data["QuestInfo"]
		--调用加技能的接口
		if IsCppBound(Player) then
			Player:SetAndSyncActionState(EActionState.eAS_Idle_BackWpn)
			Player:Lua_AddNonFightSkill(SkillName, QuestInfo)
			Gas2Gac:CancelLearnNoneFightSkill(Player.m_Conn, GlobalID)
		end
	end
	
	local function ProgressFail(Player, data)
		local Obj = data["Obj"]
		local GlobalID = data["GlobalID"]
		if IsCppBound(Player) then
			Gas2Gac:CancelLearnNoneFightSkill(Player.m_Conn, GlobalID)
		end
	end
	
	local TargetName = Target.m_Properties:GetCharName()
	if not g_ObjActionArgTbl[TargetName] then
		return
	end
	
	if NonFightSkill_Common(g_ObjActionArgTbl[TargetName][1]) and NonFightSkill_Common(g_ObjActionArgTbl[TargetName][1],"NeedLevel") > Conn.m_Player:CppGetLevel() then
		MsgToConn(Conn,1500)
		return 
	end
	
	local data = {}
	data["Obj"] = Target
	data["GlobalID"] = TargetEntityID
	data["SkillName"] = g_ObjActionArgTbl[TargetName][1]
	if g_ObjActionArgTbl[TargetName][2] and g_ObjActionArgTbl[TargetName][3] then
		data["QuestInfo"] = {g_ObjActionArgTbl[TargetName][2],g_ObjActionArgTbl[TargetName][3]}
	end
	
	ServerBeginProgress(10*1000, false, Conn, ProgressSucc, ProgressFail, data)
	--在玩家和OBJ的身上加一个特效
	Gas2Gac:LearnNoneFightSkill(Conn, TargetEntityID, data["SkillName"])
end

function Entry.Exec(Conn, Obj, ObjName, GlobalID)
	local Player = Conn.m_Player
	local PlayerId = Player.m_uID
	
	if GetDistance(Player,Obj) > 6 then
		return 
	end
	
	if not g_ObjActionArgTbl[ObjName] then
		return
	end

	if NonFightSkill_Common(g_ObjActionArgTbl[ObjName][1]) and NonFightSkill_Common(g_ObjActionArgTbl[ObjName][1],"NeedLevel") > Player:CppGetLevel() then
		MsgToConn(Conn,1500)
		return 
	end	
	
	local QuestName = g_ObjActionArgTbl[ObjName][2]
	local VarName =	g_ObjActionArgTbl[ObjName][3]
	
	local function Quest_CallBack(isSuccess)
		if isSuccess and IsCppBound(Player) then
			Gas2Gac:RetAddQuestVar(Player.m_Conn, QuestName, VarName, 1)
		end
	end
	
	local function Skill_CallBack(res)
		if IsCppBound(Conn) then
			if res then
				MsgToConn(Conn,2041)
				if QuestName and VarName then
					local params = {
						["sQuestName"] = QuestName,
						["sVarName"] = VarName,
						["iNum"] = 1,
						["char_id"] = PlayerId
					}
					CallAccountManualTrans(Conn.m_Account, "RoleQuestDB", "AddQuestVarNum", Quest_CallBack, params)
				end
			else
				LearnSkill(Conn, Obj, GlobalID)
			end
		end
	end
	
	local function QuestCheck_CallBack(res)
		if IsCppBound(Conn) then
			if res then
				local data = {}
				data["PlayerId"] = PlayerId
				data["SkillName"] = g_ObjActionArgTbl[ObjName][1]
				data["fs_kind"] = FightSkillKind.NonFightSkill
				CallAccountManualTrans(Conn.m_Account, "FightSkillDB", "Lua_SelectFightSkill", Skill_CallBack, data)
			end
		end
	end
	
	if QuestName then
		CallAccountManualTrans(Conn.m_Account, "RoleQuestDB", "CheckQuestSatisfy", QuestCheck_CallBack, {["uCharId"]=PlayerId, ["sQuestName"]=QuestName})
	else
		QuestCheck_CallBack(true)
	end
end

return Entry
