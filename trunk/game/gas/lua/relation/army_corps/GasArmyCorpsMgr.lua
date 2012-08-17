
CGasArmyCorpsMgr = class()

function CGasArmyCorpsMgr:Ctor()
	self.m_tblMembers = {}
end

function CGasArmyCorpsMgr:AddMember(PlayerID,ArmyCorpsID)
	assert(IsNumber(PlayerID))
	assert(IsNumber(ArmyCorpsID))
	self.m_tblMembers[ArmyCorpsID] = self.m_tblMembers[ArmyCorpsID] or {}
end

function CGasArmyCorpsMgr:DelMember(PlayerID,ArmyCorpsID)
end

function CGasArmyCorpsMgr:GetArmyCorpsMsgSender(ArmyCorpsID)
	if self.m_tblMembers[ArmyCorpsID] then
		return self.m_tblMembers[ArmyCorpsID]["MultiMsgSender"]
	end
end

function CGasArmyCorpsMgr:MemberLogin(Player, ArmyCorpsID)
	if self.m_tblMembers[ArmyCorpsID] then
		self.m_tblMembers[ArmyCorpsID]["MultiMsgSender"] = self.m_tblMembers[ArmyCorpsID]["MultiMsgSender"] or CPlayerSet:new()
		local Sender = self.m_tblMembers[ArmyCorpsID]["MultiMsgSender"]
		if Player and Sender then
			Sender:AddPlayer(Player)
		end
	end
end

function CGasArmyCorpsMgr:MemberLogout(Player, ArmyCorpsID)
	if self.m_tblMembers[ArmyCorpsID] then
		local Sender = self.m_tblMembers[ArmyCorpsID]["MultiMsgSender"]
		if IsCppBound(Player) and IsCppBound(Sender) then
			Sender:DelPlayer(Player)
		end
	end
end

function CGasArmyCorpsMgr:AddArmyCorpsMember(MemberId,ArmyCorpsID,ArmyCorpsName)
	if ArmyCorpsID == 0 then
		return
	end
	self:AddMember(MemberId,ArmyCorpsID)
	local Member = g_GetPlayerInfo(MemberId)
	if Member and IsCppBound(Member) then
		self:MemberLogin(Member, ArmyCorpsID)
		--佣兵团信息更新
		Member.m_Properties:SetArmyCorpsName(ArmyCorpsName)
		Member.m_uArmyCorpsID = ArmyCorpsID
		Gas2Gac:UpdateHeadInfoByEntityID(Member:GetIS(0), Member:GetEntityID())
		Gas2Gac:UpdateHeadInfoByEntityID(Member.m_Conn, Member:GetEntityID())
	end
end

function CGasArmyCorpsMgr:DelArmyCorpsMember(MemberId)
	local Member = g_GetPlayerInfo(MemberId)
	if Member and IsCppBound(Member) and Member.m_uArmyCorpsID ~= 0 then
		self:DelMember(MemberId,Member.m_uArmyCorpsID)
		self:MemberLogout(Member, Member.m_uArmyCorpsID)
		Member.m_uArmyCorpsID = 0
		Member.m_Properties:SetArmyCorpsName("")
		Gas2Gac:UpdateHeadInfoByEntityID(Member:GetIS(0), Member:GetEntityID())
		Gas2Gac:UpdateHeadInfoByEntityID(Member.m_Conn, Member:GetEntityID())
	end
end
