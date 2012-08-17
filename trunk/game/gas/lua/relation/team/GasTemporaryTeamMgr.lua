
CTemporaryTeam = class()

function CTemporaryTeam:Ctor()
	self.m_tblTeam = {}
	self.m_TeamID = 0
end

function CTemporaryTeam:AddMember(MemberID,TempTeamID)
	local Member = g_GetPlayerInfo(MemberID)
	self.m_tblTeam[TempTeamID] = self.m_tblTeam[TempTeamID] or {}
	if Member and IsCppBound(Member) and Member.m_TemporaryTeam then
		local old_team = Member.m_TemporaryTeam
		local new_team = self.m_tblTeam[TempTeamID].TeamID or ""
		LogErr("不能同时加入多个临时队伍", "MemberID:" .. MemberID .. "old_team:" .. old_team .. "new_team:" .. new_team .. "new_temp_teamid:" .. TempTeamID)
		if new_team ~= old_team then
			--加入新队伍时，先判断是否已经在其他临时队伍，在的话先删除
			for i,v in pairs(self.m_tblTeam) do
				for j = #self.m_tblTeam[i],1,-1 do
					if self.m_tblTeam[i][j] == MemberID then
						table.remove(self.m_tblTeam[i],j)
					end
				end
			end
		else
			--如果重复加入同一队伍，直接返回
			return
		end
	end
	table.insert(self.m_tblTeam[TempTeamID],MemberID)
	local temp_mem = self.m_tblTeam[TempTeamID]
	local TeamID = temp_mem.TeamID
	if TeamID then
		g_TeamMgr:AddMember(MemberID,TeamID)
		self:SendTeamInfo(TeamID)
		local data = {}
		data.team_id = TeamID
		data.member_id = {MemberID}
		CallDbTrans("GasTeamDB", "AddTempTeamlog", nil,data)
	end
end

function CTemporaryTeam:SetCaptain(MemberID,TempTeamID)
	self.m_tblTeam[TempTeamID] = self.m_tblTeam[TempTeamID] or {}
	table.insert(self.m_tblTeam[TempTeamID],MemberID)
	self.m_tblTeam[TempTeamID].m_CaptainID = MemberID
	local temp_mem = self.m_tblTeam[TempTeamID]
	local TeamID = temp_mem.TeamID
	if TeamID then
		g_TeamMgr:SetCaptain(MemberID,TeamID)
		self:SendTeamInfo(TeamID)
	end
end

function CTemporaryTeam:CreateTeam()
	local function CallBack(TeamID,data)
		if TeamID > 0 then
			local tblMembers = {}
			local tblTeamInfo = {}
			self.m_tblTeam[data.m_TeamID] = self.m_tblTeam[data.m_TeamID] or {}
			local tblCharID = self.m_tblTeam[data.m_TeamID] or {}
			self.m_tblTeam[data.m_TeamID].TeamID = TeamID
			local add_log_id = {}
			for i =1,#tblCharID do
				local PlayerID = tblCharID[i]
				table.insert(tblMembers,{PlayerID})
				local Player = g_GetPlayerInfo(PlayerID)
				if Player and IsCppBound(Player) then
					if Player.m_TemporaryTeam then
						local now_team = Player.m_TemporaryTeam
						LogErr("不能同时加入多个临时队伍","now_team:" .. now_team .. "new_team:" .. TeamID)
					end
					Player.m_TemporaryTeam = TeamID
				end
				if not Player.m_TemporaryTeam then
					table.insert(add_log_id,PlayerID)
				end
			end
			tblTeamInfo.m_TeamID = TeamID
			tblTeamInfo.m_AllMembers = tblMembers
			tblTeamInfo.m_TeamCap = self.m_tblTeam[data.m_TeamID].m_CaptainID
			tblTeamInfo.m_OnlineMem = #tblCharID
			g_TeamMgr:InitTeamInfo(tblTeamInfo)
			self:SendTeamInfo(TeamID)
			local data = {}
			data.team_id = TeamID
			data.member_id = add_log_id
			CallDbTrans("GasTeamDB", "AddTempTeamlog",nil,data)
		end
	end
	self.m_TeamID = self.m_TeamID +1
	local data = {}
	data.m_TeamID = self.m_TeamID
	CallDbTrans("GasTeamDB", "GetTeamStaticID", CallBack,data)
	return self.m_TeamID
end

function CTemporaryTeam:DelMember(MemberID,TempTeamID)
	self.m_tblTeam[TempTeamID] = self.m_tblTeam[TempTeamID] or {}
	local temp_mem = self.m_tblTeam[TempTeamID]
	local TeamID = temp_mem.TeamID
	for i = #self.m_tblTeam[TempTeamID],1,-1 do
		local PlayerID = self.m_tblTeam[TempTeamID][i]
		if PlayerID == MemberID then
			table.remove(self.m_tblTeam[TempTeamID],i)
		end
	end
	if #self.m_tblTeam[TempTeamID] == 0 then
		self.m_tblTeam[TempTeamID] = nil
	end
	local Member = g_GetPlayerInfo(MemberID)
	if Member and IsCppBound(Member) then
		Member.m_TemporaryTeam = nil
	end
	if TeamID then
		g_TeamMgr:DelMember(TeamID,MemberID)
		self:SendTeamInfo(TeamID)
	end
	CGasTeam.GetTeamMembers(MemberID)
end

function CTemporaryTeam:BreakTeam(TempTeamID)
	local temp_mem = self.m_tblTeam[TempTeamID] or {}
	local TeamID = temp_mem.TeamID
	self.m_tblTeam[TempTeamID] = nil
	if TeamID then
		local Members = g_TeamMgr:GetMembers(TeamID)
		if Members then
			local tblTeamInfo = {}
			tblTeamInfo.m_TeamID = TeamID
			tblTeamInfo.m_AllMembers = {}
			tblTeamInfo.m_TeamCap = 0
			tblTeamInfo.m_OnlineMem = 0
			g_TeamMgr:InitTeamInfo(tblTeamInfo)
			for i=1,#Members do
				local MemberID = Members[i]
				local Member = g_GetPlayerInfo(MemberID)
				if Member and IsCppBound(Member) then
						Member.m_TemporaryTeam = nil
				end
				CGasTeam.GetTeamMembers(MemberID)
			end
		end
	end
end

function CTemporaryTeam:SendTeamInfo(TeamID)
	local tblCharID = g_TeamMgr:GetMembers(TeamID)
	if tblCharID then
		for i =1,#tblCharID do
			local PlayerID = tblCharID[i]
			local Player = g_GetPlayerInfo(PlayerID)
			if Player and IsCppBound(Player) then
				Player.m_TemporaryTeam = TeamID
				Player:SetTeamID(TeamID)
				CTeamServerManager_CppAddTeamMember(TeamID, 1, Player:GetEntityID())
			end
			Gas2GacById:ReturnGetTeamMemberBegin(PlayerID)
			for j = 1, #tblCharID do
					local char_id = tblCharID[j]
					if char_id ~= PlayerID then
						local char = g_GetPlayerInfo(char_id)
						if char and IsCppBound(char) then
							local name = char.m_Properties:GetCharName()
							local bOnline = true
							local entity_id = char:GetEntityID()
							local class,sex,level = char:CppGetClass(),char.m_Properties:GetSex(),char:CppGetLevel()
							Gas2GacById:ReturnGetTeamMember(PlayerID,char_id,name,bOnline,entity_id,class,sex,level)
						end
					end
			end
			Gas2GacById:ReturnGetTeamMemberEnd(PlayerID,g_TeamMgr:GetCaptain(TeamID),#tblCharID)
		end
	end
end

function CTemporaryTeam:InitTemporaryTeamInfo(MemberID)
	local Member = g_GetPlayerInfo(MemberID)
	if Member and IsCppBound(Member) and (not Member.m_TemporaryTeam) then
		local TeamID  = nil
		for i,v in pairs(self.m_tblTeam) do
			local TempTeamID = i
			local TeamIDi = nil
			self.m_tblTeam[TempTeamID] = self.m_tblTeam[TempTeamID] or {}
			for j =1,#self.m_tblTeam[TempTeamID] do
				if self.m_tblTeam[TempTeamID][j] == MemberID then
					if not self.m_tblTeam[TempTeamID].TeamID then return end
					if # self.m_tblTeam[TempTeamID] > 1 then
						TeamIDi = self.m_tblTeam[TempTeamID].TeamID
						break
					end
				end
			end
			if TeamIDi then
				TeamID = TeamIDi
				break
			end
		end
		if TeamID then
			if Member and IsCppBound(Member) then
				Member.m_TemporaryTeam = TeamID
				Member:SetTeamID(TeamID)
				CTeamServerManager_CppAddTeamMember(TeamID, 1, Member:GetEntityID())
				CGasTeam.GetTeamMembers(MemberID)
			end
		end
	end
end



