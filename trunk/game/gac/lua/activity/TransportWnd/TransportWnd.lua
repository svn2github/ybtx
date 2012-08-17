CTransportWnd = class(SQRDialog)

function CTransportWnd:Ctor(parent)
	self:CreateFromRes("TransportHintWnd",parent)
	self.m_HintWnd = self:GetDlgChild("TransportHint")
	self.m_TransportBtn = self:GetDlgChild("ChooseTran")
	self.m_CancelBtn = self:GetDlgChild("CancelTran")
	self.m_CallerName = nil
end

function CTransportWnd:GetWnd()
	local Wnd = g_GameMain.m_TransportHintWnd
	if not Wnd then
		Wnd = CTransportWnd:new(g_GameMain)
		g_GameMain.m_TransportHintWnd = Wnd
	end
	return Wnd
end

function CTransportWnd:CheckPlayerIsInFight(Tick, starttime)
	if os.time()-starttime>10 then
		self:ShowWnd(false)
		UnRegisterTick(self.m_CheckIsInFightTick)
		self.m_CheckIsInFightTick = nil
		return
	end
	if g_MainPlayer:CppGetCtrlState(EFighterCtrlState.eFCS_InBattle) then
		self.m_TransportBtn:EnableWnd(false)
	else
		self.m_TransportBtn:EnableWnd(true)
	end
end

function CTransportWnd:InitTransportHintWnd(CallerName)
	self.m_HintWnd:SetWndText(GetStaticTextClient(1040, CallerName))
	self.m_HintWnd:ShowWnd(true)
	if g_MainPlayer:CppGetCtrlState(EFighterCtrlState.eFCS_InBattle) then
		self.m_TransportBtn:EnableWnd(false)
	else
		self.m_TransportBtn:EnableWnd(true)
	end
	if self.m_CheckIsInFightTick then
		UnRegisterTick(self.m_CheckIsInFightTick)
		self.m_CheckIsInFightTick = nil
	end
	local starttime = os.time()
	self.m_CheckIsInFightTick = RegClassTick("CheckPlayerIsInFight", self.CheckPlayerIsInFight, 500, self, starttime)
	self.m_TransportBtn:ShowWnd(true)
	self.m_CancelBtn:EnableWnd(true)
	self.m_CancelBtn:ShowWnd(true)
	self:ShowWnd(true)
	self.m_CallerName = CallerName
end

function CTransportWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == BUTTON_LCLICK) then
		if Child == self.m_TransportBtn then
			Gac2Gas:ResponseCallRequest(g_Conn, self.m_CallerName, true)
			UnRegisterTick(self.m_CheckIsInFightTick)
			self.m_CheckIsInFightTick = nil
			self:ShowWnd(false)
		elseif Child == self.m_CancelBtn then
			Gac2Gas:ResponseCallRequest(g_Conn, self.m_CallerName, false)
			UnRegisterTick(self.m_CheckIsInFightTick)
			self.m_CheckIsInFightTick = nil
			self:ShowWnd(false)
		end
	end
end
