local function Script(Arg, Trigger, Player)

	for i, tbl in pairs(Arg) do
		local Type = tbl[1]
		local Name = tbl[2]
		local SceneId = Trigger.m_Scene.m_SceneId
		if Type == "Npc" then
			if g_NpcBornMgr._m_AllNpc[SceneId] then 
				for _, NpcObj in pairs(g_NpcBornMgr._m_AllNpc[SceneId]) do
					if NpcObj.m_Properties:GetCharName() == Name then
						g_NpcServerMgr:DestroyServerNpcNow(NpcObj, false)
					end
				end
			end
		elseif Type == "Obj" then
			if g_AllIntObj[SceneId] then
				for _, IntObj in pairs(g_AllIntObj[SceneId]) do
					if IntObj.m_Properties:GetCharName() == Name then
						g_IntObjServerMgr:Destroy(IntObj,false)
					end
				end
			end
		elseif Type == "Trap" then
			if g_AllTrap[SceneId] then
				for _, Trap in pairs(g_AllTrap[SceneId]) do
					if Trap.m_Properties:GetCharName() == Name then
						DestroyServerTrap(Trap, false)
					end
				end
			end
		elseif Type == "Skill" then
			local PlayerTbl = Trigger.m_Scene.m_tbl_Player
			if PlayerTbl then
				for _, Player in pairs(PlayerTbl) do
					if Player then
						Player:RemoveTempSkill(Name, 1)
					end
				end
			end
		end
		Gas2Gac:DelNpcFunLinkTbl(Trigger.m_Scene.m_CoreScene, Name)
	end
end

return Script
