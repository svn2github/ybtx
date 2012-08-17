local function Script(Arg, Trigger, Player)

	local PlayerID = Trigger.m_ExpOwnerId 
	local ExpOwner = CEntityServerManager_GetEntityByID(PlayerID) or Player
	
	if not ExpOwner or not IsCppBound(ExpOwner) then
		return
	end
	
 	TriggerHideQuest(ExpOwner, Arg[1])
end

return Script