local function Script(Arg, Trigger, Player)
	if Player.m_OwnerId ~= nil then
		Player = g_GetPlayerInfo(Player.m_OwnerId)
	end
	
	if not IsCppBound(Player) then
		return
	end
	local TargetName = Arg[1]
	if not TargetName or TargetName == "" then
		return
	end
	CMercenaryRoomCreate.KillTargetAddNum(Player.m_Scene, TargetName)
end

return Script
