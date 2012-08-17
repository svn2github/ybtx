cfg_load "int_obj/IntObj_Common"

local TempSuccFun = {}
local TempFailFun = {}
local TempArg = {}

local function DoProgressTick(Tick, data)
	local Player, SuccFun, Arg = unpack(data, 1, 3)
	if not IsCppBound(Player) then
		return
	end
	local PlayerId = Player.m_uID
	UnRegisterTick(Player.m_ActionLoadingTick)
	Player.m_ActionLoadingTick = nil
	Player.m_IsBattle = nil
	Player:LuaSetDoSkillCtrlState( EDoSkillCtrlState.eDSCS_InDoingSkill, false)
	Player.m_SetCtrlState = 8
	Player:SetInGatherProcess(false)
	SuccFun(Player, Arg)
	Player:DelListeningWnd()
	TempSuccFun[PlayerId] = nil
	TempFailFun[PlayerId] = nil
	TempArg[PlayerId] = nil
	return
end

function ServerBeginProgress(ProTime, IsPerfectColl, Conn, SuccFun, FailFun, Arg)
	if not IsCppBound(Conn) then
		return
	end
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	local PlayerId = Player.m_uID
	if Player.m_UseItemLoadingTick then
		Gas2Gac:NotifyUseItemProgressStop(Conn)
		Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitAutoTrack, false)
		Player:SetInGatherProcess(false)
		Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitMove, false)
		Player:LuaSetDoSkillCtrlState( EDoSkillCtrlState.eDSCS_InDoingSkill, false)
		Player.m_SetCtrlState = 9
		if TempItemFailFun[PlayerId] ~= nil then
			TempItemFailFun[PlayerId](Player, TempItemArg[PlayerId])
			TempItemFailFun[PlayerId] = nil
			TempItemArg[PlayerId] = nil
		end
		UnRegisterTick(Player.m_UseItemLoadingTick)
		Player.m_UseItemLoadingTick = nil
		Player.m_IsBattle = nil
	end
	
	if Player.m_ActionLoadingTick then
		if TempFailFun[PlayerId] ~= nil then
			Player:DelListeningWnd()
			TempFailFun[PlayerId](Player, TempArg[PlayerId])
			TempSuccFun[PlayerId] = nil
			TempFailFun[PlayerId] = nil
			TempArg[PlayerId] = nil
		end
		UnRegisterTick(Player.m_ActionLoadingTick)
		Player:LuaSetDoSkillCtrlState( EDoSkillCtrlState.eDSCS_InDoingSkill, false)
		Player.m_SetCtrlState = 10
		Player.m_ActionLoadingTick = nil
		Player.m_IsBattle = nil
	end
--	if CheckPlayerFightStateStopProgress(Conn.m_Player) then
--		MsgToConn(Conn, 3000, "你现在的状态下不能进行采集")
--		return false
--	end
	if Player:CppGetDoSkillCtrlState( EDoSkillCtrlState.eDSCS_InDoingSkill) then
		MsgToConn(Conn, 838)
		FailFun(Player, Arg)
		return
	end
	Player:SetAndSyncActionState(EActionState.eAS_Gather)
	Player:SetInGatherProcess(true)
	if ProTime ~= 0 then
		Player:LuaSetDoSkillCtrlState( EDoSkillCtrlState.eDSCS_InDoingSkill, true)
		Gas2Gac:NotifyLoadProgressBar(Conn, ProTime,IsPerfectColl, Arg["ActionName"] or "")
		local BeginTime = GetProcessTime()
		local data = {Player, SuccFun, Arg}
		TempSuccFun[PlayerId] = SuccFun
		TempFailFun[PlayerId] = FailFun
		TempArg[PlayerId] = Arg
		
		Player:AddListeningWnd()
		Player.m_ActionLoadingTick = RegisterTick("ProgressBarTick", DoProgressTick, ProTime, data)
	else
		SuccFun(Player,Arg)
	end
	return true
end

function BreakPlayerActionLoading(Player,isStop)
	local function Init()
		local PlayerId = Player.m_uID
		local GlobalID = Player.m_ProcessingObjID
		local CollObj = CIntObjServer_GetIntObjServerByID( GlobalID )
		if isStop and CollObj then	
			local ObjName = CollObj.m_Properties:GetCharName()
			local sTriggerAction = IntObj_Common(ObjName, "TriggerAction")
			if sTriggerAction == "读条不被攻击打断" then
				return
			end
		end
		Player.m_ProcessingObjID = nil
		Player:DelListeningWnd()
		if Player.m_ActionLoadingTick then
			UnRegisterTick(Player.m_ActionLoadingTick)
			Player.m_ActionLoadingTick = nil
			Player:LuaSetDoSkillCtrlState( EDoSkillCtrlState.eDSCS_InDoingSkill, false)
			Player.m_SetCtrlState = 11
		end
		Player:SetAndSyncActionState(EActionState.eAS_Idle_BackWpn)
		if IsFunction(TempFailFun[PlayerId]) then
			TempFailFun[PlayerId](Player, TempArg[PlayerId])
		end
		Gas2Gac:NotifyStopProgressBar(Player.m_Conn)
		TempSuccFun[PlayerId] = nil
		TempFailFun[PlayerId] = nil
		TempArg[PlayerId] = nil
	end
	apcall(Init)
end

function PerfectCollSuccStopLoading(Player)
	local PlayerId = Player.m_uID
	UnRegisterTick(Player.m_ActionLoadingTick)
	Player.m_ActionLoadingTick = nil
	Player:LuaSetDoSkillCtrlState( EDoSkillCtrlState.eDSCS_InDoingSkill, false)
	Player.m_SetCtrlState = 12
	Player:SetInGatherProcess(false)
	Gas2Gac:NotifyStopProgressBar(Player.m_Conn)
	Player:DelListeningWnd()
	TempSuccFun[PlayerId](Player, TempArg[PlayerId])
	TempSuccFun[PlayerId] = nil
	TempFailFun[PlayerId] = nil
	TempArg[PlayerId] = nil
end

function EndServerProgress(Player)
	local PlayerId = Player.m_uID
	if TempFailFun[PlayerId] then
		TempFailFun[PlayerId](Player, TempArg[PlayerId])
		TempSuccFun[PlayerId] = nil
		TempFailFun[PlayerId] = nil
		TempArg[PlayerId] = nil
	end
end

function Gac2Gas:BreakPlayerActionLoading(Conn)
	if IsCppBound(Conn) and IsCppBound(Conn.m_Player) then
		BreakPlayerActionLoading(Conn.m_Player)
	end
end