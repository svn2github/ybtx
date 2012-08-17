local function Script(Arg, Npc)

	local range = Arg[1] or 3
	Npc:SetMoveType(ENpcMoveType.ENpcMove_Random)
	Npc:SetMoveRange(range)
	Npc:OnBeginRandomMove()
end

return Script