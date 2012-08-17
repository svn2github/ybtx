local function Script(Arg, Creator, Player)
	local scene = nil
	local CreatType  = Arg[1]
	local CreatName  = Arg[2]
	local CreatScene = Arg["Scene"] or Creator.m_Scene
	
	local pos 
	if Arg[3] == "相对坐标" then --相对坐标
		pos = Arg["Pos"] or GetCreatePos(Creator)
		pos.x = pos.x + Arg[4]
		pos.y = pos.y + Arg[5]
	elseif Arg[3] == "绝对坐标" then
		local ID = Arg[4]
		pos = CFPos:new()
		pos.x, pos.y, scene = GetScenePosition(ID, CreatScene)
		if scene ~= CreatScene.m_SceneName then
			CfgLogErr("触发脚本填写出现错误!", "(创建)"..CreatType..":("..CreatName..")脚本中填写的坐标ID("..ID..")在坐标表中填写的地图(".. scene ..")与实际地图(".. CreatScene.m_SceneName ..")不符！")
			return
		end
	elseif Arg[3] == "随机" then
		pos = Arg["Pos"] or GetCreatePos(Creator)
		pos = RandomCreatPos(pos,Arg[4])
	else
		pos = Arg["Pos"] or GetCreatePos(Creator)
	end
	
	if CreatType == "Npc" then
		local Level = g_NpcBornMgr:GetNpcBornLevel(CreatName)
		local fPos = CFPos:new( pos.x * EUnits.eGridSpan, pos.y * EUnits.eGridSpan )	
--		local otherData = {["m_CreatorEntityID"]=Arg["CreatorEntityID"] or Creator:GetEntityID()}
--		local crnpc = g_NpcServerMgr:CreateServerNpc(CreatName,Level, CreatScene, fPos, otherData)
		if IsCppBound(Player) then
			Creator = Player
		end
		local NpcInfo = Npc_Common(CreatName)
		local otherData =	{["m_CreatorEntityID"] = Arg["CreatorEntityID"]}
		if Creator then
			otherData["m_CreatorEntityID"] = otherData["m_CreatorEntityID"] or Creator:GetEntityID()
			otherData["m_OwnerId"] = Creator.m_uID
		end
		
		local Npc = nil
		if IsServantType(NpcInfo("Type")) then
			Npc = g_NpcServerMgr:CreateServerNpc(CreatName, Level, CreatScene, fPos, otherData, Creator:GetEntityID())
		else
			Npc = g_NpcServerMgr:CreateServerNpc(CreatName, Level, CreatScene, fPos, otherData)
		end
		
		if Npc ~= nil then 
			Npc:SetAndSyncActionState(EActionState.eAS_Idle_BackWpn)
		end
	elseif CreatType == "Obj" then
		local Obj = CreateIntObj(CreatScene,pos,CreatName,true)
		if Obj == nil then
			return 
		end
		Obj.m_CreateTime = os.time()
	elseif CreatType == "Trap" then
		CreateServerTrap(CreatScene,pos,CreatName)
	end
end

return Script
