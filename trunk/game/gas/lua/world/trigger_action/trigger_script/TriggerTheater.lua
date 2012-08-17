local function Script(Arg, Trigger, Player)
	
	local Scene = Trigger.m_Scene
	local PlayerId = nil
	
	if IsCppBound(Player) then
		PlayerId = Player:GetEntityID()
	end
	
	if Arg[2] and Arg[2] == "ÐÔ±ð" then
		if not PlayerId then
			return
		end
		Scene.m_TheaterMgr:RunTriggerTheaterBySex(Arg[1], PlayerId)
	elseif Arg[2] and Arg[2] == "Ëæ»ú" then
		Scene.m_TheaterMgr:RunTriggerRandomTheater(Arg[1], PlayerId)
	else
		Scene.m_TheaterMgr:RunTriggerTheater(Arg[1], PlayerId)
	end
	
end

return Script