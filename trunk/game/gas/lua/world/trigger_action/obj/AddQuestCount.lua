gas_require "framework/main_frame/SandBoxDef"
local AddVarNumForTeamQuest = AddVarNumForTeamQuest
local g_IntObjServerMgr = g_IntObjServerMgr
local g_ObjActionArgTbl = g_ObjActionArgTbl
local IntObj_Common = IntObj_Common
local ServerBeginProgress = ServerBeginProgress
local IsCppBound = IsCppBound
local EActionState = EActionState

local g_GetPlayerInfo = g_GetPlayerInfo
local Entry = CreateSandBox(...)

function Entry.Exec(Conn, IntObj, ObjName, GlobalID)
	local ActionArg = g_ObjActionArgTbl[ObjName][1]
	local TriggerAction = IntObj_Common(ObjName, "TriggerAction")
	
	if IntObj.m_Click then
		return
	end
	
	local function Succeed(Player, Arg)	
		Player:SetAndSyncActionState(EActionState.eAS_Idle_BackWpn)
		local varname = "使用Obj"..ObjName
		AddVarNumForTeamQuest(Player, varname,1)
		IntObj.m_Click = nil
		if TriggerAction == "加任务计数清除Obj" then
			g_IntObjServerMgr:Destroy(IntObj,true)
		end
	end
	
	local function Failed(Player, Arg)
		IntObj.m_Click = nil
		if IsCppBound(Player) then
			Gas2Gac:NotifyStopProgressBar(Player.m_Conn)
		end
	end
	
	IntObj.m_Click = true
	local data = {GlobalID}
	if ActionArg ~= "" and ActionArg > 0 and ActionArg ~= nil and IsNumber(ActionArg) then
		local ProTime = ActionArg * 1000
		ServerBeginProgress(ProTime, false, Conn, Succeed, Failed, data)
	else
		Succeed(Conn.m_Player, data)
	end
end

return Entry
 