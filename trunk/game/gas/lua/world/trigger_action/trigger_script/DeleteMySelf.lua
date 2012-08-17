local function Script(Arg, Trigger, Player)

	if IsPlayer(Trigger) then
		Trigger = Player
	end
	
	if not Trigger then
		return
	end
	
	local Type = Trigger.m_Properties:GetType()
	
	if Type == ECharacterType.Npc then
		Trigger:SetOnDisappear(true)
		
	elseif Type == ECharacterType.IntObj then
		g_IntObjServerMgr:Destroy(Trigger, true)
		
	elseif Type == ECharacterType.Trap then
		DestroyServerTrap(Trigger, true)
		
	end
	
end

return Script