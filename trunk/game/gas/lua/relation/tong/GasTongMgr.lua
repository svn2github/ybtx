
CGasTongMgr = class()

function CGasTongMgr:Ctor()
	self.m_tblMembers = {}
end

function CGasTongMgr:AddMember(PlayerID,TongID)
	assert(IsNumber(PlayerID))
	assert(IsNumber(TongID))
	self.m_tblMembers[TongID] = self.m_tblMembers[TongID] or {}
	-- 判断m_tblMembers中是否已经有该角色id，有的话就不加
	if #self.m_tblMembers[TongID] == 0 then
		table.insert(self.m_tblMembers[TongID], PlayerID)
	else
		for i =1,#self.m_tblMembers[TongID] do
			if self.m_tblMembers[TongID][i] == PlayerID then
				break
			end
			if i >= #self.m_tblMembers[TongID] then
				table.insert(self.m_tblMembers[TongID], PlayerID)
			end
		end
	end
end

function CGasTongMgr:DelMember(PlayerID,TongID)
	assert(IsNumber(PlayerID))
	assert(IsNumber(TongID))
	if self.m_tblMembers[TongID] then
		for i =1,#self.m_tblMembers[TongID] do
			if self.m_tblMembers[TongID][i] == PlayerID then
				table.remove(self.m_tblMembers[TongID],i)
				break
			end
		end
	end
end

function CGasTongMgr:GetTongMsgSender(TongID)
	if self.m_tblMembers[TongID] then
		return self.m_tblMembers[TongID]["MultiMsgSender"]
	end
end

function CGasTongMgr:MemberLogin(Player, TongID)
	if self.m_tblMembers[TongID] then
		self.m_tblMembers[TongID]["MultiMsgSender"] = self.m_tblMembers[TongID]["MultiMsgSender"] or CPlayerSet:new()
		local Sender = self.m_tblMembers[TongID]["MultiMsgSender"]
		if Player and Sender then
			Sender:AddPlayer(Player)
		end
	end
end

function CGasTongMgr:MemberLogout(Player, TongID)
	if self.m_tblMembers[TongID] then
		local Sender = self.m_tblMembers[TongID]["MultiMsgSender"]
		if IsCppBound(Player) and IsCppBound(Sender) then
			Sender:DelPlayer(Player)
		end
	end
end

function CGasTongMgr:GetMembers(TongID)
	if self.m_tblMembers[TongID] then
		return self.m_tblMembers[TongID]
	else
		return {}
	end
end

function CGasTongMgr:AddTongMember(MemberId, TongID,TongName)
	local Member = g_GetPlayerInfo(MemberId)
	self:AddMember(MemberId, TongID)
	if Member and IsCppBound(Member) then
		Member:SetTongID(TongID)
		Member.m_Properties:SetTongName(TongName)
		self:MemberLogin(Member, TongID)
		Gas2Gac:UpdateHeadInfoByEntityID(Member:GetIS(0), Member:GetEntityID())
		Gas2Gac:UpdateHeadInfoByEntityID(Member.m_Conn, Member:GetEntityID())
		InitNewTongMemberPanel(Member)
	end
end

function CGasTongMgr:DelTongMember(MemberId, TongID)
	local Member = g_GetPlayerInfo(MemberId)
	if Member and IsCppBound(Member) then
		Member:SetTongID(0)
		Member.m_Properties:SetTongName("")
		self:MemberLogout(Member, TongID)
	end
	self:DelMember(MemberId, TongID)
end
