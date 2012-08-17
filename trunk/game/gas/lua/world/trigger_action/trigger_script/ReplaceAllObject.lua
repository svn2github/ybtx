local function Script(Arg, Trigger, Player)

	local scene = Trigger.m_Scene
	local SceneId = scene.m_SceneId
	local TempNpc = {}
	for i, tbl in pairs(Arg) do
		local Name = tbl[1]
		local newNpcName = tbl[2]
		if g_NpcBornMgr._m_AllNpc[SceneId] then 
			for NpcId, NpcObj in pairs(g_NpcBornMgr._m_AllNpc[SceneId]) do
				if NpcObj.m_Properties:GetCharName() == Name then
					if not NpcObj:CppIsLive() then
						table.insert(TempNpc, {NpcObj,newNpcName})
					end						
				end
			end
		end
	end
	for i, v in pairs(TempNpc) do
		local Npc = v[1]
		local newNpcName = v[2]
		local pos = CFPos:new()
		Npc:GetPixelPos(pos)
		local dir = Npc:GetActionDir()
		Npc:SetNoDissolve()
		Npc:SetOnDisappear(true)
		
		local level = g_NpcBornMgr:GetNpcBornLevel(newNpcName)
		local otherData = {}
		if IsCppBound(Player) then
			otherData["m_CreatorEntityID"] = Player:GetEntityID()
			otherData["m_OwnerId"] = Player.m_uID
		end
		local NewNpc = g_NpcServerMgr:CreateServerNpc(newNpcName, level, scene, pos, otherData)
		if IsServerObjValid(NewNpc) then
			NewNpc:SetAndSyncActionDir(dir)
		end
		v[1] = nil
	end
	TempNpc = nil
end

return Script
