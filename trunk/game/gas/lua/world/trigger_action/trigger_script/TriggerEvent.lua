local function Script(Arg, Trigger, Player)

	local Master = nil
	if Player then
		Master = Player
	end
	for i = 1, table.getn(Arg) do
		g_TriggerScript:RunScriptByIndex(Arg[i], Trigger, Master)
	end	
end

return Script