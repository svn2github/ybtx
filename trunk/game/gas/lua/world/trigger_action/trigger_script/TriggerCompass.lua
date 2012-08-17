local function Script(Arg, Trigger, Player)
	local CreatorID = Trigger.m_CreatorEntityID
	local Creator = nil
	if CreatorID ~= nil then
		Creator = CEntityServerManager_GetEntityByID(CreatorID)
	end
	if Creator ~= nil and not IsCppBound(Creator) then
		return
	end
	local CreaterName = Creator.m_Properties:GetCharName()
	local NpcType = Arg[1]
	local ShowTime = Arg[2]
	local NpcName = Arg[3]
	if not NpcName then
		GetArrowHeadFromItem(Creator.m_Conn,NpcType,CreaterName,ShowTime)
	else
		GetArrowHeadFromItem(Creator.m_Conn,NpcType,NpcName,ShowTime)
	end
end

return Script