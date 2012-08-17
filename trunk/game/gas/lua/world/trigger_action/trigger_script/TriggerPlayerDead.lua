-- 某Npc出生,触发(陷阱内)Player死亡
local function Script(Arg, Trigger)
	
	local Scene = Trigger.m_Scene
	if not Scene then
		return
	end
	local PlayerTbl = Scene.m_tbl_Player 
	if not PlayerTbl then
		return 
	end
	for _,Player in pairs(PlayerTbl) do
		if not IsCppBound(Player) then
			return
		end
		if Player.m_ConfirmFlag then
			local npc = Player.m_ReplaceModel 
			g_NpcServerMgr:DestroyServerNpcNow(npc, false)
			Player:PlayerDoFightSkillWithoutLevel("自杀")
			Player.m_ConfirmFlag = false
		end
	end
end

return Script
