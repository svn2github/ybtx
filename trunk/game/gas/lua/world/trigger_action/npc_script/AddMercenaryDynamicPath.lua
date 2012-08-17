local function Script(Arg, Trigger, Player)

	local Npc = nil
	if IsNpc(Trigger) then
		Npc = Trigger
	end
	if IsNpc(Player) then
		Npc = Player
	end
	if not Npc then
		return
	end
	
	Npc:SetNpcSleepState(false)
	g_DynamicCreateMgr:CreateMercenaryPath(Npc, Arg[1], Arg[2])
end

return Script