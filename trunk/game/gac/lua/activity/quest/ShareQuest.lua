CShareQuestWnd = class (SQRDialog)

function CShareQuestWnd:Ctor()
	self:CreateFromRes("TransportHintWnd", g_GameMain)
	self.m_HintWnd = self:GetDlgChild("TransportHint")
	self.m_AcceptBtn = self:GetDlgChild("ChooseTran")
	self.m_CancelBtn = self:GetDlgChild("CancelTran")
	self.m_SharerName = nil
	self.m_QuestName = nil
end

function CShareQuestWnd:DeleteQuestShareWnd()
	self:ShowWnd(false)
	g_GameMain.m_ShareQuestWndTbl[self.m_QuestName] = nil
	for questname, wnd in pairs(g_GameMain.m_ShareQuestWndTbl) do
		return
	end
	g_GameMain.m_ShareQuestWndTbl = nil
end

function CShareQuestWnd:CheckPlayerResponse()
	if self.m_CheckResponseTick then
		UnRegisterTick(self.m_CheckResponseTick)
		self.m_CheckResponseTick = nil
		self:DeleteQuestShareWnd()
	end
end

function CShareQuestWnd:InitNotifyShareWnd(PlayerName, quest_name, teamID)
	self.m_HintWnd:SetWndText(GetStaticTextClient(1032, PlayerName, g_GetLanQuestName(quest_name)))
	self.m_HintWnd:ShowWnd(true)
	
	if self.m_CheckResponseTick then
		UnRegisterTick(self.m_CheckResponseTick)
		self.m_CheckResponseTick = nil
	end
	self.m_CheckResponseTick = RegClassTick("CheckPlayerResponse", self.CheckPlayerResponse, 20000, self)
	self.m_AcceptBtn:EnableWnd(true)
	self.m_AcceptBtn:ShowWnd(true)
	self.m_CancelBtn:EnableWnd(true)
	self.m_CancelBtn:ShowWnd(true)
	self:ShowWnd(true)
	self.m_SharerName = PlayerName
	self.m_QuestName = quest_name
	self.m_QuestTeamID = teamID
end

function CShareQuestWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == BUTTON_LCLICK) then
		if Child == self.m_AcceptBtn then
			local res, msgID = g_CheckQuestShowName(self.m_QuestName)
			if not res then
				if msgID then
					MsgClient(msgID)
				end
				return
			end
			Gac2Gas:ResponseShareQuest(g_Conn, self.m_QuestTeamID, self.m_QuestName)
			self:CheckPlayerResponse()
		elseif Child == self.m_CancelBtn then
			self:CheckPlayerResponse()
		end
	end
	return	
end

function CShareQuestWnd.UnRegisterShareQuestTick()
	if g_GameMain.m_ShareQuestWndTbl then
		for _ , ShareQuestWnd in pairs(g_GameMain.m_ShareQuestWndTbl) do
			UnRegisterTick(ShareQuestWnd.m_CheckResponseTick)
		end
	end
	g_GameMain.m_ShareQuestWndTbl = nil
end

function CShareQuestWnd.NotifyTeammateShareQuest(teamID, PlayerName, quest_name)
	if g_GameMain.m_ShareQuestWndTbl == nil then
		g_GameMain.m_ShareQuestWndTbl = {}
	end
	if not g_GameMain.m_ShareQuestWndTbl[quest_name] then
		g_GameMain.m_ShareQuestWndTbl[quest_name] = CShareQuestWnd:new(g_GameMain)
		g_GameMain.m_ShareQuestWndTbl[quest_name]:InitNotifyShareWnd(PlayerName, quest_name, teamID)
	end
end
