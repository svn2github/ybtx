cfg_load "npc/NpcShowTask_Common"
CSpecialNpc = class()

local function SpecialNpcLimitCount(Npc, Name, limitCount)
	if limitCount == 0 then
		return true
	end
	if not Npc.m_SpecialCreat then
		Npc.m_SpecialCreat = {}
	end
	if not Npc.m_SpecialCreat[Name] then
		Npc.m_SpecialCreat[Name] = 0
	end
	if Npc.m_SpecialCreat[Name] >= limitCount then
		return false
	end
	Npc.m_SpecialCreat[Name] = Npc.m_SpecialCreat[Name] + 1
	return true
end

local function RetTransInfo(Player, EntityID)
	local Npc = CCharacterDictator_GetCharacterByID(EntityID)
	if not IsServerObjValid(Npc) then
		return
	end
	if not IsCppBound(Player) then
		return
	end
	local npcName = Npc.m_Properties:GetCharName()
	local NpcShowTaskCommon = NpcShowTask_Common(npcName)
	local NpcShowTaskIndex = NpcShowTaskCommon:GetKeys()
	for i, _ in pairs(NpcShowTaskIndex) do
		local str = i..""
		local Arg = NpcShowTaskCommon(str, "Arg")
		local TriggerType = NpcShowTaskCommon(str, "TriggerType")
		local index = npcName .. TriggerType
		for j,i in pairs(Transport_Server:GetKeys(Arg, "NPC")) do
			local TransInfo = Transport_Server(Arg, "NPC", i.."")
			local level = TransInfo("LeastLev")
 			if Player:CppGetLevel() < TransInfo("LeastLev") then
 				return true
			end 
		end
	end
end


function CSpecialNpc.RemitSpecScene(Conn, EntityID, Name)

	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	local Npc = CCharacterDictator_GetCharacterByID(EntityID)
	if not IsServerObjValid(Npc) then
		return
	end
	local npcName = Npc.m_Properties:GetCharName()
	
	local CampNameTbl = 
	
	{
		"艾米帝国",
		"神圣教廷",
		"修斯帝国"
	}
	
--	local NpcShowTaskCommon = NpcShowTask_Common(npcName) 
--	local NpcShowTaskIndex = NpcShowTaskCommon:GetKeys()
	local NpcShowTaskCommonKey = NpcShowTask_Common:GetKeys()
	for j, index in pairs(NpcShowTaskCommonKey) do
		if index == npcName then
			local NpcShowTaskCommon = NpcShowTask_Common(npcName) 
			local NpcShowTaskIndex = NpcShowTaskCommon:GetKeys()
			for i, _ in pairs(NpcShowTaskIndex) do
				local str = i..""
				local triggerType = NpcShowTaskCommon(str, "TriggerType")
				local taskType = NpcShowTaskCommon(str, "TaskType")
				if triggerType == Name and taskType == "Transmit" then
					local arg = NpcShowTaskCommon(str, "Arg")
					local limitCount = NpcShowTaskCommon(str, "LimitCount")
					if Transport_Server(arg) then
						for _, i in pairs(Transport_Server:GetKeys(arg, "NPC")) do
							local TransInfo = Transport_Server(arg, "NPC", i.."")
							if (TransInfo("BeginScene") == "" or Player.m_Scene.m_SceneName == TransInfo("BeginScene")) then
							  if (TransInfo("Camp") ~= 0 and TransInfo("Camp") ~= Player:CppGetBirthCamp()) then
							  	MsgToConn(Conn, 196003)
									return
							  end
							  if Player:CppGetLevel() < TransInfo("LeastLev") then
									MsgToConn(Conn, 196002)
									return
								end 
								if SpecialNpcLimitCount(Npc, Name, limitCount) then
									local RealName = TransInfo("EndScene")
									local posx = TransInfo("PosX")
									local posy = TransInfo("PosY")
									if not Scene_Common[RealName] then
										RealName = CampNameTbl[Player:CppGetCamp()]..RealName
									end
									if Player.m_Scene.m_SceneName == RealName then
										local pos = CPos:new()
										pos.x = posx
										pos.y = posy
										Player:SetGridPosByTransport(pos)
									else
										NewChangeScene(Conn, RealName, posx, posy)
									end
								else
									MsgToConn(Conn,196001)
								end
								break;
							end
						end
					end
				end
			end
		end
	end
end

function CSpecialNpc.RenoSpecNpc(Conn, EntityID, Name)
	if not Conn.m_Player then
		return 
	end
	local Scene = Conn.m_Player.m_Scene
	local PlayerTbl = Scene.m_tbl_Player
	local SceneId = Scene.m_SceneID
	local Npc = CCharacterDictator_GetCharacterByID(EntityID)
	if not IsServerObjValid(Npc) then
		return
	end
	local npcName = Npc.m_Properties:GetCharName()
	
	local NpcShowTaskCommonKey = NpcShowTask_Common:GetKeys()
	for j, index in pairs(NpcShowTaskCommonKey) do
		if index == npcName then
			local NpcShowTaskCommon = NpcShowTask_Common(npcName) 
			local NpcShowTaskIndex = NpcShowTaskCommon:GetKeys()
				for i, _ in pairs(NpcShowTaskIndex) do
				local str = i..""
				local triggerType = NpcShowTaskCommon(str, "TriggerType")
				local taskType = NpcShowTaskCommon(str, "TaskType")
				local limitCount = NpcShowTaskCommon(str, "LimitCount")
				if triggerType == Name and taskType == "Renovate" then
					if SpecialNpcLimitCount(Npc, Name, limitCount) then
						local NpcName =	NpcShowTaskCommon(str, "Arg")
						local NpcSceneName = NpcShowTaskCommon(str, "SceneName")
						local NpcPosx = NpcShowTaskCommon(str, "PosX")
						local NpcPosy = NpcShowTaskCommon(str, "PosY")
						--创建Npc
						if Scene.m_SceneName == NpcSceneName then
							local Pos = CPos:new()
							Pos.x = NpcPosx
							Pos.y = NpcPosy
							local fPos = CFPos:new( Pos.x * EUnits.eGridSpan, Pos.y * EUnits.eGridSpan )	
							local NewNpc = g_NpcServerMgr:CreateServerNpc(NpcName, NpcLevel, Scene, fPos)
							Npc.m_Flag = true
							for playerId, _ in pairs(PlayerTbl) do
								local char = g_GetPlayerInfo(playerId)
								if not IsCppBound(char) then
									return
								end
								if not IsCppBound(char.m_Conn) then
									return
								end
								Gas2Gac:RetNpcHeadInfo(char.m_Conn,EntityID)
							end
						else
							CfgLogErr("NpcShowTask_Common配置表脚本填写错误!","SceneName("..NpcSceneName..")与NpcName所在场景("..Scene.m_SceneName..")不符!")
							return
						end
					else
						MsgToConn(Conn,196001)
					end
				end
			end
		end
	end
end

function CSpecialNpc.GetPicInfo(Conn,EntityID)
	local Npc = CCharacterDictator_GetCharacterByID(EntityID)
	if not IsServerObjValid(Npc) then
		return
	end
	Gas2Gac:RetPicInfo(Conn, EntityID, Npc.m_Flag)
end

function CSpecialNpc.CanTransport(Conn, CostMoney, Name, EntityID, isBind)
	if not IsCppBound(Conn) then
		return
	end
	if not IsCppBound(Conn.m_Player) then
		return
	end
	
	local data = {}
	data["isBind"] = isBind
	data["money"] = -CostMoney
	data["playerId"] = Conn.m_Player.m_uID
	if RetTransInfo(Conn.m_Player, EntityID) then
		MsgToConn(Conn, 196002)
		return
	end
	local function CallBack(flag, uMsgID)
		if not IsCppBound(Conn) then
			return
		end
		if flag then
			CSpecialNpc.RemitSpecScene(Conn, EntityID, Name)
		else
			MsgToConn(Conn, uMsgID)
		end
	end
	CallAccountManualTrans(Conn.m_Account, "MoneyMgrDB", "CostMoneyByTrans", CallBack, data)
end
