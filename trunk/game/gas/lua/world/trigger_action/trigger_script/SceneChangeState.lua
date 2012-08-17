local function Script(Arg, Trigger)

	local Scene = Trigger.m_Scene
	if not Scene then
		return
	end
	local State = Arg[1]
--	if State == "ºÚÒ¹" then
--		Scene.m_IsDarkness = true
--	elseif State == "°×Ìì" then
--		Scene.m_IsDarkness = false
--	end
	for id, Player in pairs(Scene.m_tbl_Player) do
		Player:PlayerDoPassiveSkill(State, 1)
		--Gas2Gac:SetSceneState(Player.m_Conn,Scene.m_IsDarkness)
	end
end

return Script
