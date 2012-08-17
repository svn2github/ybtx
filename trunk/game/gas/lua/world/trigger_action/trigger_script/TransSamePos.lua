local function Script(Arg, Trigger, Player)  --玩家踩Trap传送到指定位置
	if not IsCppBound(Player) then
		return
	end
	if Player.m_Properties:GetType() == ECharacterType.Player then
		local npc = Player.m_ReplaceModel
		local NpcPos = CPos:new()
		npc:GetGridPos(NpcPos)
		Player:SetGridPosByTransport(NpcPos)
		if IsCppBound(npc) then
			g_NpcServerMgr:DestroyServerNpcNow(npc, false)
			npc = nil
		end
	end
end

return Script