gas_require "world/tong_area/TongBattleCountMgrInc"
cfg_load "tong/TongKillPersonEffect_Server"
cfg_load "tong/TongContiKillPersonEffect_Server"

g_TongBattleCountMgr = CTongBattleCountMgr:new()

function CTongBattleCountMgr:Init()
	self.m_KillPersonCountTbl = {}
	self.m_ContiKillPersonCountTbl = {}
	self.m_DeathCountTbl = {}
	self.m_KillEffectTbl = {}
	self.m_AmGetExploitCountNum = 0
	self.m_SsGetExploitCountNum = 0
	self.m_XsGetExploitCountNum = 0
	for _, nKillNum in pairs(TongKillPersonEffect_Server:GetKeys()) do
		local tblEffect = TongKillPersonEffect_Server(nKillNum)
		self.m_KillEffectTbl[nKillNum] = tblEffect("EffectName")
	end
	
	self.m_ContiKillEffectTbl = {}
	for _, nContiKillNum in pairs(TongContiKillPersonEffect_Server:GetKeys()) do
		local tblEffect = TongContiKillPersonEffect_Server(nContiKillNum)
		self.m_ContiKillEffectTbl[nContiKillNum] = tblEffect("EffectName")
	end
end

g_TongBattleCountMgr:Init()

function CTongBattleCountMgr:AddKillCount(Player)
	local nPlayerID = Player.m_uID
	local Conn = Player.m_Conn
	if not IsCppBound(Conn) or not IsServerObjValid(Player) then
		return
	end
	local sPlayerName = Player.m_Properties:GetCharName()
	local scene = Player.m_Scene
	local nSceneType = scene.m_SceneAttr.SceneType
	local nSceneID = scene.m_SceneId
	local nTongID = Player.m_Properties:GetTongID()
	
	if not (nSceneType == 7 and g_WarZoneMgr.m_Warring or nSceneType == 8 and g_ChallengeCurrentState == 3) then
		return
	end
	
	--统计杀人
	if not self.m_KillPersonCountTbl[nSceneID] then
		self.m_KillPersonCountTbl[nSceneID] = {}
	end
	if not self.m_KillPersonCountTbl[nSceneID][nPlayerID] then
		self.m_KillPersonCountTbl[nSceneID][nPlayerID] = 0
	end
	self.m_KillPersonCountTbl[nSceneID][nPlayerID] = self.m_KillPersonCountTbl[nSceneID][nPlayerID] + 1
	local nKillNum = self.m_KillPersonCountTbl[nSceneID][nPlayerID]
	local sEffectName = self.m_KillEffectTbl[nKillNum]
	if sEffectName then
		if IsServerObjValid(Player) then
			Player:PlayerDoItemSkill(sEffectName, 1)
		end
		if nTongID ~= 0 then
			local function CallBack(bFlag, tblMember)
				if bFlag then
					for nIndex = 1, #tblMember do
						local nMemberCsId = tblMember[nIndex]
						if nMemberCsId ~= nPlayerID then
							MsgToConnById(nMemberCsId, 9441, sPlayerName, nKillNum)
						end
					end
				end
			end
			local parameter = {}
			parameter["uTongID"] = nTongID
			CallAccountAutoTrans(Conn.m_Account, "ChannelDB", "GetPartyOnlinePlayer", CallBack, parameter, nTongID)
		end
	end
	
	--统计连续杀人
	if not self.m_ContiKillPersonCountTbl[nSceneID] then
		self.m_ContiKillPersonCountTbl[nSceneID] = {}
	end
	if not self.m_ContiKillPersonCountTbl[nSceneID][nPlayerID] then
		self.m_ContiKillPersonCountTbl[nSceneID][nPlayerID] = 0
	end
	self.m_ContiKillPersonCountTbl[nSceneID][nPlayerID] = self.m_ContiKillPersonCountTbl[nSceneID][nPlayerID] + 1
	local nContiKillNum = self.m_ContiKillPersonCountTbl[nSceneID][nPlayerID]
	local sContiEffectName = self.m_ContiKillEffectTbl[nContiKillNum]
	if sContiEffectName then
		if IsServerObjValid(Player) then
			Player:PlayerDoItemSkill(sContiEffectName, 1)
		end
		if nTongID ~= 0 then
			local function CallBack(bFlag, tblMember)
				if bFlag then
					for nIndex = 1, #tblMember do
						local nMemberCsId = tblMember[nIndex]
						if nMemberCsId ~= nPlayerID then
							MsgToConnById(nMemberCsId, 9442, sPlayerName, nContiKillNum)
						end
					end
				end
			end
			local parameter = {}
			parameter["uTongID"] = nTongID
			CallAccountAutoTrans(Conn.m_Account, "ChannelDB", "GetPartyOnlinePlayer", CallBack, parameter, nTongID)
		end
	end
	MsgToConn(Conn, 9443, nContiKillNum)
	local nDeathCount = 0
	if self.m_DeathCountTbl[nSceneID] and self.m_DeathCountTbl[nSceneID][nPlayerID] then
		nDeathCount = self.m_DeathCountTbl[nSceneID][nPlayerID]
	end
	Gas2Gac:UpdateTongBattlePersonalCountInfo(Conn, nKillNum, nContiKillNum, nDeathCount)
end

function CTongBattleCountMgr:AddDeathCount(Player)
	local nPlayerID = Player.m_uID
	local Conn = Player.m_Conn
	if not IsCppBound(Conn) or not IsServerObjValid(Player) then
		return
	end
	local sPlayerName = Player.m_Properties:GetCharName()
	local scene = Player.m_Scene
	local nSceneType = scene.m_SceneAttr.SceneType
	local nSceneID = scene.m_SceneId
	
	if not (nSceneType == 7 and g_WarZoneMgr.m_Warring or nSceneType == 8 and g_ChallengeCurrentState == 3) then
		return
	end
	
	if not self.m_DeathCountTbl[nSceneID] then
		self.m_DeathCountTbl[nSceneID] = {}
	end
	
	if not self.m_DeathCountTbl[nSceneID][nPlayerID] then
		self.m_DeathCountTbl[nSceneID][nPlayerID] = 0
	end
	
	self.m_DeathCountTbl[nSceneID][nPlayerID] = self.m_DeathCountTbl[nSceneID][nPlayerID] + 1
	
	local nKillNum = 0
	if self.m_KillPersonCountTbl[nSceneID] and g_TongBattleCountMgr.m_KillPersonCountTbl[nSceneID][nPlayerID] then
		nKillNum = self.m_KillPersonCountTbl[nSceneID][nPlayerID]
	end
	self:ResetContiKillNum(Player)
	local nContiKillNum = 0
	if self.m_ContiKillPersonCountTbl[nSceneID] and g_TongBattleCountMgr.m_ContiKillPersonCountTbl[nSceneID][nPlayerID] then
		nContiKillNum = self.m_ContiKillPersonCountTbl[nSceneID][nPlayerID]
	end
	local nDeathCount = self.m_DeathCountTbl[nSceneID][nPlayerID]
	Gas2Gac:UpdateTongBattlePersonalCountInfo(Conn, nKillNum, nContiKillNum, nDeathCount)
end

function CTongBattleCountMgr:ResetContiKillNum(Player)
	local scene = Player.m_Scene
	local nSceneType = scene.m_SceneAttr.SceneType
	if nSceneType ~= 7 and nSceneType ~= 8 then
		return
	end
	local nSceneID = scene.m_SceneId
	local nPlayerID = Player.m_uID
	if self.m_ContiKillPersonCountTbl[nSceneID] and self.m_ContiKillPersonCountTbl[nSceneID] then
		self.m_ContiKillPersonCountTbl[nSceneID][nPlayerID] = nil
		if not next(self.m_ContiKillPersonCountTbl[nSceneID]) then
			self.m_ContiKillPersonCountTbl[nSceneID] = nil
		end
	end
end

function CTongBattleCountMgr:KillPersonCallBack(nKillerId)
	local Entity = CEntityServerManager_GetEntityByID(nKillerId)
	if Entity and IsServerObjValid(Entity) then
		if Entity.m_Properties:GetType() == ECharacterType.Player then
			self:AddKillCount(Entity)
		end
	end
end

function CTongBattleCountMgr:ResumeEffect(Player)
	local nPlayerID = Player.m_uID
	local Conn = Player.m_Conn
	if not IsCppBound(Conn) or not IsServerObjValid(Player) then
		return
	end
	local scene = Player.m_Scene
	local nSceneType = scene.m_SceneAttr.SceneType
	local nSceneID = scene.m_SceneId
	local sPlayerName = Player.m_Properties:GetCharName()
	if (nSceneType == 7 or nSceneType == 8)
		 and self.m_KillPersonCountTbl[nSceneID]
		 and self.m_KillPersonCountTbl[nSceneID][nPlayerID] then
		local nKillNum = self.m_KillPersonCountTbl[nSceneID][nPlayerID]
		if IsServerObjValid(Player) then
			if nKillNum >= 60 then
				Player:PlayerDoItemSkill(self.m_KillEffectTbl[60], 1)
			elseif nKillNum >= 35 then
				Player:PlayerDoItemSkill(self.m_KillEffectTbl[35], 1)
			elseif nKillNum >= 20 then
				Player:PlayerDoItemSkill(self.m_KillEffectTbl[20], 1)
			elseif nKillNum >= 10 then
				Player:PlayerDoItemSkill(self.m_KillEffectTbl[10], 1)
			elseif nKillNum >= 5 then
				Player:PlayerDoItemSkill(self.m_KillEffectTbl[5], 1)
			end
		end
	end
end

function CTongBattleCountMgr:ClearCountAndEffect()
	for nSceneID in pairs(self.m_KillPersonCountTbl) do
		for nPlayerID in pairs(self.m_KillPersonCountTbl[nSceneID]) do
			local Player = g_GetPlayerInfo(nPlayerID)
			self.m_KillPersonCountTbl[nSceneID][nPlayerID] = nil
			if IsServerObjValid(Player) then
				if Player:CppIsLive() then
					Player:PlayerDoPassiveSkill("国战结束清除", 1)
				else
					Player:PlayerDoPassiveSkill("国战结束尸体清除", 1)
				end
			end
		end
		self.m_KillPersonCountTbl[nSceneID] = nil
	end
	
	for nSceneID in pairs(self.m_ContiKillPersonCountTbl) do
		for nPlayerID in pairs(self.m_ContiKillPersonCountTbl[nSceneID]) do
			local Player = g_GetPlayerInfo(nPlayerID)
			self.m_ContiKillPersonCountTbl[nSceneID][nPlayerID] = nil
			if IsServerObjValid(Player) then
				if Player:CppIsLive() then
					Player:PlayerDoPassiveSkill("国战结束清除", 1)
				else
					Player:PlayerDoPassiveSkill("国战结束尸体清除", 1)
				end
			end
		end
		self.m_ContiKillPersonCountTbl[nSceneID] = nil
	end
	
	for nSceneID in pairs(self.m_DeathCountTbl) do
		for nPlayerID in pairs(self.m_DeathCountTbl[nSceneID]) do
			local Player = g_GetPlayerInfo(nPlayerID)
			self.m_DeathCountTbl[nSceneID][nPlayerID] = nil
			if IsServerObjValid(Player) then
				if Player:CppIsLive() then
					Player:PlayerDoPassiveSkill("国战结束清除", 1)
				else
					Player:PlayerDoPassiveSkill("国战结束尸体清除", 1)
				end
			end
		end
		self.m_DeathCountTbl[nSceneID] = nil
	end
	
end
