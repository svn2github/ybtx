gac_gas_require "activity/npc/CheckNpcTalk"

function Gac2Gas:AddFinishNpcTalk(Conn, TalkName, NpcEntityID, IsUseTrigger)
	if not (IsCppBound(Conn) and Conn.m_Player) then
		return
	end
	local Player = Conn.m_Player
	local PlayerID = Player.m_uID
	local Scene = Player.m_Scene
	
	if not Scene.m_FinishNpcTalkTbl then
		Scene.m_FinishNpcTalkTbl = {}
	end

	if not (Scene.m_FinishNpcTalkTbl and Scene.m_FinishNpcTalkTbl[NpcEntityID])then
		Gas2Gac:RetShowFuncNpcOrObjTalkWnd(Conn)
		-- 触发事件
		if IsUseTrigger then
			local TriggerID = g_NpcTalkTbl[TalkName]["触发事件"].Param1
			local Npc = CCharacterDictator_GetCharacterByID(NpcEntityID)
			if not IsCppBound(Npc) then
				return
			end
			g_TriggerScript:RunScriptByIndex(TriggerID, Npc, Conn.m_Player)
			Scene.m_FinishNpcTalkTbl[NpcEntityID] = true
			
			-- 给场景里的所有人发消息，隐藏NPC头上的标志
			for PlayerID, Player in pairs(Scene.m_tbl_Player) do
				Gas2Gac:RetNpcTalkHeadInfo(Player.m_Conn, NpcEntityID, true)
			end
		end
	end
end

function Gac2Gas:GetNpcTalkHeadInfo(Conn, uEntityID)
	if not (IsCppBound(Conn) and Conn.m_Player) then
		return
	end
	local Player = Conn.m_Player
	local Scene = Player.m_Scene
	local IsUsed
	if not Scene.m_FinishNpcTalkTbl or not Scene.m_FinishNpcTalkTbl[uEntityID] then
		IsUsed = false
	else
		IsUsed = true
	end
	Gas2Gac:RetNpcTalkHeadInfo(Conn, uEntityID, IsUsed)
end

local function CheckFinishNpcTalk(Scene, Camp, NpcName, uEntityID)
	-- 这个NPC触发过就不再触发了
	local IsFinish = false
	if Scene.m_FinishNpcTalkTbl and Scene.m_FinishNpcTalkTbl[uEntityID] then
		IsFinish = true
	end

	if not IsFinish then
		local TalkTbl = {}
		for name,tbl in pairs(g_NpcTalkTbl) do
			if tbl["触发Npc"].Param1[Camp] == NpcName then
				table.insert( TalkTbl, name )
			end
		end
		
		for i=1, #TalkTbl do
			local TalkName = TalkTbl[i]
			if TalkName ~= "" then
				return TalkName
			end
		end
	end
	return ""
end

function ShowFuncNpcOrObjTalkWnd(Conn, uEntityID, uBuildingTongID)
	if not (IsCppBound(Conn) and Conn.m_Player) then
		return
	end
	local Player = Conn.m_Player
	local PlayerID = Player.m_uID
	local NpcObj = CCharacterDictator_GetCharacterByID(uEntityID)
	--local Npc = CEntityServerManager_GetEntityByID(uEntityID)
	if not NpcObj then
		Gas2Gac:ShowNpcTalkWnd(Conn, "", uEntityID, uBuildingTongID, true)
		return
	end
	
	local Scene = Player.m_Scene

	local TeamID = Player.m_Properties:GetTeamID()
	local tblMembers = g_TeamMgr:GetMembers(TeamID)
	
	local Camp = Player:CppGetBirthCamp()
	local NpcName = NpcObj.m_Properties:GetCharName()
	
	local TalkName = CheckFinishNpcTalk(Scene, Camp, NpcName, uEntityID)	
	
	if TalkName ~= "" then
		-- 给队伍所有人都发
		if TeamID == 0 then
			Gas2Gac:ShowNpcTalkWnd(Conn, TalkName, uEntityID, uBuildingTongID, true)
		else
			for i = 1, #(tblMembers) do
				local IsUseTrigger
				if tblMembers[i] == PlayerID then
					IsUseTrigger = true
				else
					IsUseTrigger = false
				end
				local pMember = g_GetPlayerInfo(tblMembers[i])
				if IsCppBound(pMember) and IsCppBound(pMember.m_Conn) then
					local MemberSceneName = pMember.m_Scene.m_SceneName
					if MemberSceneName == NpcObj.m_Scene.m_SceneName then
						Gas2Gac:ShowNpcTalkWnd(pMember.m_Conn, TalkName, uEntityID, uBuildingTongID, IsUseTrigger)
					end
				end
			end		
		end
	else
		Gas2Gac:ShowNpcTalkWnd(Conn, "", uEntityID, uBuildingTongID, true)
	end
end