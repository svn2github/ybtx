
local action = {
	["start01"] 		= EActionState.eAS_Throw,
	["stand01"]			= EActionState.eAS_WaitBite,
	["disturbed01"] = EActionState.eAS_Disturbed01,
	["disturbed02"] = EActionState.eAS_Disturbed02,
	["disturbed03"] = EActionState.eAS_Disturbed03,
	["bite01"] 			= EActionState.eAS_Bite,
	["stand02"] 		= EActionState.eAS_WaitPull,
	["pull01"] 			= EActionState.eAS_Pull01,
	["pull02"] 			= EActionState.eAS_Pull02,
	["pull03"] 			= EActionState.eAS_Pull03,
	["succeed01"] 	= EActionState.eAS_Succeed01,
	["succeed02"]		= EActionState.eAS_Succeed02,
	["end01"]				= EActionState.eAS_Idle_BackWpn,
}
--播放动作
function Gac2Gas:HeadGameChangeAction(Conn, ActionName)
	if not (IsCppBound(Conn) and IsCppBound(Conn.m_Player)) then
		return
	end
	Conn.m_Player:SetAndSyncActionState(action[ActionName])
end
--更换动作包
function Gac2Gas:HeadGameChangeActionBag(Conn, AniFileTailName)
	if not (IsCppBound(Conn) and IsCppBound(Conn.m_Player)) then
		return
	end
	local player = Conn.m_Player
	local entityID = player:GetEntityID()
	player.m_Properties:SetBehaviorType(AniFileTailName)
	Gas2Gac:UpdateModel( player:GetConnection(), entityID)
	Gas2Gac:UpdateModel( player:GetIS(0), entityID)
	if AniFileTailName == "" then
		Gas2Gac:UpdateAni( player:GetConnection(), entityID)
		Gas2Gac:UpdateAni( player:GetIS(0), entityID)
		Conn.m_Player:SetAndSyncActionState(action["end01"])
	end
end
