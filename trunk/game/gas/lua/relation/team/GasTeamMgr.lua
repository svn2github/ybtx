--gas_require "relation/team/GasTeammatePosMgr"

CGasTeamMgr = class()

function CGasTeamMgr:Ctor()
	self.m_tblMembers = {}
	self.m_tblTeamSceneId = {}
end

--[[
功能：添加队员
参数：队员ID\队伍ID
--]]
function CGasTeamMgr:AddMember(PlayerID,TeamID)
	assert(IsNumber(PlayerID))
	assert(IsNumber(TeamID))
	self.m_tblMembers[TeamID] = self.m_tblMembers[TeamID] or {}
	self.m_tblMembers[TeamID]["MultiMsgSender"] = self.m_tblMembers[TeamID]["MultiMsgSender"] or g_App:GetMultiMsgSender()
	-- 判断m_tblMembers中是否已经有该角色id，有的话就不加
	if #self.m_tblMembers[TeamID] == 0 then
		table.insert(self.m_tblMembers[TeamID], PlayerID)
		self:SetAverageAssignOrder(PlayerID,TeamID)
	else
		for i =1,#self.m_tblMembers[TeamID] do
			if self.m_tblMembers[TeamID][i] == PlayerID then
				break
			end
			if i >= #self.m_tblMembers[TeamID] then
				table.insert(self.m_tblMembers[TeamID], PlayerID)
				self:SetAverageAssignOrder(PlayerID,TeamID)
			end
		end
	end
	local sender = self.m_tblMembers[TeamID]["MultiMsgSender"]
	local player = g_GetPlayerInfo(PlayerID)
	if player and IsCppBound(player) then
		sender:AddPipe(player.m_Conn)
		CTeamServerManager_CppAddTeamMember(TeamID, 1, player:GetEntityID())
		if not player.m_TemporaryTeam then
			player:SetTeamID(TeamID)
		end
	end
end

function CGasTeamMgr:GetTeamMsgSender(TeamID)
	if self.m_tblMembers[TeamID] then
		return self.m_tblMembers[TeamID]["MultiMsgSender"]
	end
end

function CGasTeamMgr:MemberLogout(player, teamid)
	if self.m_tblMembers[teamid] then
		local sender = self.m_tblMembers[teamid]["MultiMsgSender"]
		if player and sender then
			sender:DelPipe(player.m_Conn)
		end
	end
end

function CGasTeamMgr:SetCaptain(PlayerID,TeamID)
	self.m_tblMembers[TeamID] = self.m_tblMembers[TeamID] or {}
	self.m_tblMembers[TeamID]["Captain"] = PlayerID
end

function CGasTeamMgr:GetCaptain(TeamID)
	return self.m_tblMembers[TeamID] and self.m_tblMembers[TeamID]["Captain"]
end

function CGasTeamMgr:SetAverageAssignOrder(PlayerID,TeamID)
	assert(IsNumber(PlayerID))
	assert(IsNumber(TeamID))
	g_AverageAssignOrder[TeamID] = g_AverageAssignOrder[TeamID] or {}
	if table.getn(g_AverageAssignOrder[TeamID]) == 0 then
		table.insert(g_AverageAssignOrder[TeamID], PlayerID)
	else
		local num = table.getn(g_AverageAssignOrder[TeamID])
		for i = 1 , num do 
			if g_AverageAssignOrder[TeamID][i] == PlayerID then
				break
			end
			if i == num then
				table.insert(g_AverageAssignOrder[TeamID], PlayerID)				
			end
		end
	end
end

function CGasTeamMgr:GetMembers(TeamID)
	if self.m_tblMembers[TeamID] then
		return self.m_tblMembers[TeamID]
	else
		return {}
	end
end

function CGasTeamMgr:DelMember(TeamID,MemberID)
	local Members = self.m_tblMembers[TeamID]
	if Members then
		for i =1,#Members do
			if MemberID == Members[i] then
				table.remove(Members,i)
				local Member = g_GetPlayerInfo(MemberID)
				if Member and IsCppBound(Member) then
					self:MemberLogout(Member, TeamID)
				end
			end
		end
	end
end

function CGasTeamMgr:InitTeamInfo(team_info)
		local TeamID = team_info.m_TeamID
		local Online_Mem = team_info.m_OnlineMem
		CGasTeammatePosMgr.DestroyGetTeammatePosTick(TeamID)
		if Online_Mem == 0 then
			--在线人数为0或者队伍解散了，清空内存信息
			local MyMembers=self.m_tblMembers[TeamID]
			if MyMembers then	
				for i=1,#MyMembers do
					local player_id = MyMembers[i]
					local Player = g_GetPlayerInfo(player_id)
					if Player and IsCppBound(Player) then
						Player:SetTeamID(0)
						CTeamServerManager_CppRemoveTeamMember(TeamID, 1, Player:GetEntityID())
					end
				end		
				local Sender = MyMembers["MultiMsgSender"]				
				if Sender and IsCppBound(Sender) then
					Sender:Release()
				end
			end
			g_AverageAssignOrder[TeamID] = nil
			self.m_tblMembers[TeamID] = nil
			return
		end
		local Members,teamCap = team_info.m_AllMembers,team_info.m_TeamCap
		self.m_tblMembers[TeamID] = {}
		g_AverageAssignOrder[TeamID] = {}
		CTeamServerManager_CppDestroyTeam(TeamID,	1)
		for i = 1, #Members do
			local member_id = Members[i][1]
			self:AddMember(member_id, TeamID)
		end
		self:SetCaptain(teamCap, TeamID)
		local new_cap = g_GetPlayerInfo(teamCap)
		if new_cap and IsCppBound(new_cap) then
			CTeamServerManager_CppChangeTeamMaster(TeamID,1,new_cap:GetEntityID())
		end
end


