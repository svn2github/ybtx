gas_require "fb_game/PublicArea/PublicAreaSceneMgrInc"

--因为公共地图区域本都是在其主场景的服务器创建的, 故只需处理单服的逻辑

g_PublicAreaSceneMgr = CPublicAreaSceneMgr:new()
g_PublicAreaSceneMgr.m_SceneTbl = {}

function CPublicAreaSceneMgr:AddPublicAreaScene(scene)
	local sceneName = scene.m_SceneAttr.SceneName
	if not self.m_SceneTbl[sceneName] then
		self.m_SceneTbl[sceneName] = {}
	end
	local index = 1
	while true do
		if not self.m_SceneTbl[sceneName][index] then
			self.m_SceneTbl[sceneName][index] = scene
			scene._area_index = index
			break
		end
		index = index + 1
	end
end

function CPublicAreaSceneMgr:DelPublicAreaScene(scene)
	local sceneName = scene.m_SceneAttr.SceneName
	self.m_SceneTbl[sceneName][scene._area_index] = nil
end

function CPublicAreaSceneMgr:GetCanEnterScene(player, sceneName)
	local maxPlayerNum = Scene_Common[sceneName].MaxPlayerNum
	if g_PublicAreaSceneMgr.m_IsGMSet then
		maxPlayerNum = g_PublicAreaSceneMgr.m_GM_FbNum
	end
	if self.m_SceneTbl[sceneName] then
		for _, scene in pairs(self.m_SceneTbl[sceneName]) do
			if scene:IsCanChangeIn() and scene:GetPlayerNum() < maxPlayerNum then
				return scene
			end
		end
	end
end

function CPublicAreaSceneMgr:GetTeammateScene(player, areaName)
	local teamId = player.m_Properties:GetTeamID()
	if teamId ~= 0 then
		local captainId = g_TeamMgr:GetCaptain(teamId)
		if captainId ~= player.m_uID then
			local captain = g_GetPlayerInfo(captainId)
			if IsCppBound(captain) and captain.m_AreaName == areaName then
				return captain.m_Scene
			end
		end
		
		local teamMember  = g_TeamMgr:GetMembers(teamId)
		for _, id in ipairs(teamMember) do
			if captainId ~= id and id ~= player.m_uID then
				local member = g_GetPlayerInfo(id)
				if IsCppBound(member) and member.m_AreaName == areaName then
					return member.m_Scene
				end
			end
		end
		
	end
end

function CPublicAreaSceneMgr:OnPlayerChangeArea(player, areaName, pos)
	if areaName == "" then
		return false
	end
	--print("切换区域 " ,areaName)
	local curScene = player.m_Scene
	local curSceneType = curScene.m_SceneAttr.SceneType
	local basicName 
	
	if curSceneType == 1 then
		basicName = curScene.m_SceneName
	elseif curSceneType == 25 then
		basicName = curScene.m_SceneAttr.BasicName
	else
		return false
	end
	local uniteName = basicName .. areaName
	
	if not pos then
		pos = CPos:new()
		player:GetGridPos(pos)
	end
	
	if Scene_Common[uniteName] then
		
		local teammateScene = self:GetTeammateScene(player, areaName)
		if teammateScene then
			if curScene ==  teammateScene then
				return false
			else
				MultiServerChangeScene(player.m_Conn, teammateScene.m_SceneId, g_CurServerId, pos.x, pos.y)
				return true
			end
--		else
--			g_MainSceneAreaMgr:UndateAreaPlayerInfo(curScene, player, areaName)
		end
		
		local mainScene = g_SceneMgr:GetScene(g_SceneMgr:GetSceneByName(basicName))
		if g_MainSceneAreaMgr:IsAreaFull(mainScene, areaName) then
			NewChangeScene(player.m_Conn, uniteName, pos.x, pos.y)
			return true
		else
			if curScene ==  mainScene then
				return false
			else
				MultiServerChangeScene(player.m_Conn, mainScene.m_SceneId, g_CurServerId, pos.x, pos.y)
				return true
			end
		end

	else
		if curSceneType == 25 then
			ChangeSceneByName(player.m_Conn, basicName, pos.x, pos.y)
			return true
		else
			return false
		end
	end
end


function OnDestroyPublicAreaScene(scene)
	g_PublicAreaSceneMgr:DelPublicAreaScene(scene)
end


function Gac2Gas:PublicAreaInvite(Conn, teamMateId)
	local player = Conn.m_Player
	if not IsCppBound(player) or player.m_Scene.m_SceneAttr.SceneType ~= 25  then
		--非公共地图区域本
		return
	end
	
	local teamId = player.m_Properties:GetTeamID()
	if teamId == 0 then
		return
	end
	
	--队友检测
	local teamMember  = g_TeamMgr:GetMembers(teamId)
	local isMember = false
	for _, id in ipairs(teamMember) do
		if id == teamMateId then
			isMember = true
			break
		end
	end
	if not isMember then
		return
	end
	
	
	local InvitedPlayer = g_GetPlayerInfo(teamMateId)
	-- 检测被邀请玩家是否已经在这个场景
	local scene = player.m_Scene
	if IsCppBound(InvitedPlayer) then
		if InvitedPlayer.m_Scene == scene then
			local PlayerName = InvitedPlayer.m_Properties:GetCharName()
			MsgToConn(Conn, 10008, PlayerName)
			return
		end
	end
	Gas2GacById:InviteJoinSceneMsgBox(teamMateId, scene.m_SceneId, g_CurServerId, scene.m_SceneName, player.m_Properties:GetCharName(), 25, "")
	if not scene.m_InviteData then
		scene.m_InviteData = {}
	end
	local pos = CPos:new()
	player:GetGridPos(pos)
	scene.m_InviteData[teamMateId] = pos
end

function InviteJoinPublicAreaFb(Conn, sceneId, serverId)
	Gas2GasAutoCall:AgreeInviteJoinScene(GetServerAutoConn(serverId), Conn.m_Player.m_uID, sceneId)
end

function Gas2GasDef:AgreeInviteJoinScene(Conn, charId, sceneId)
	local scene = g_SceneMgr:GetScene(sceneId)
	if not scene or scene.m_SceneAttr.SceneType ~= 25 then
		return
	end
	local pos = scene.m_InviteData and scene.m_InviteData[charId]
	if pos then
		Gas2GasAutoCall:InviteChangeScene(Conn, charId, sceneId, g_CurServerId, pos.x , pos.y)
	end
end

function Gas2GasDef:InviteChangeScene(Conn, charId, sceneId, serverId, x, y)
	local player = g_GetPlayerInfo(charId)
	if IsCppBound(player) then
		MultiServerChangeScene(player.m_Conn, sceneId, serverId, x, y)
	end
end
