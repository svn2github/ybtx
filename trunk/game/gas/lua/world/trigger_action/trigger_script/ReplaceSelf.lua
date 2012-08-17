local function Script(Arg, Trigger, Player)

	local scene = Trigger.m_Scene
	local pos = CFPos:new()
	Trigger:GetPixelPos(pos)
	local dir = Trigger:GetActionDir()
	local newNpcName = Arg[1]
	if Arg[2] and Arg[2] == "消失不渐隐" then
		Trigger:SetNoDissolve()
	end
	Trigger:SetOnDisappear(true)
	
	if newNpcName == nil or newNpcName == "" then
		return
	end
	local level =  g_NpcBornMgr:GetNpcBornLevel(newNpcName)
	local otherData = {}
	if IsCppBound(Player) then
		otherData["m_CreatorEntityID"] = Player:GetEntityID()
		otherData["m_OwnerId"] = Player.m_uID
	end
	if GetOnlyNpc(scene.m_SceneId, newNpcName) then
		CfgLogErr("Npc "..newNpcName.." 必须是场景内唯一的(见配置表Npc_Common)")
		return
	end
	local NewNpc = g_NpcServerMgr:CreateServerNpc(newNpcName, level, scene, pos, otherData)
	if IsServerObjValid(NewNpc) then
		NewNpc:SetAndSyncActionDir(dir)
	end
end

return Script
