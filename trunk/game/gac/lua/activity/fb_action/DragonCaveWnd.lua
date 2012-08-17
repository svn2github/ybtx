CDragonCaveWnd = class(SQRDialog)

function CDragonCaveWnd:Ctor()
	self:CreateFromRes("DragonCaveWnd", g_GameMain)
	self.m_Text = self:GetDlgChild("DisplayText")
	self.m_Enter = self:GetDlgChild("EnterBtn")
	self.m_Cancel = self:GetDlgChild("CancelBtn")
	self.m_Close = self:GetDlgChild("CloseBtn")
	self.m_Enter:SetWndText(GetStaticTextClient(380001))
	self.m_Cancel:SetWndText(GetStaticTextClient(380002))
	self.m_Text:SetWndText(GetDescriptionDisplayName("ÁúÑ¨»î¶¯"))
	
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(self)
	
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)

end

function CDragonCaveWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_Cancel or Child == self.m_Close then
			self:ShowWnd(false)
		elseif Child == self.m_Enter then 
			local TargetNpcId = self.m_TargetNpcId
			local Npc = CCharacterFollower_GetCharacterByID(TargetNpcId)
			local NpcName = Npc.m_Properties:GetCharName()
			Gac2Gas:EnterDragonCave(g_Conn, NpcName)
			self:ShowWnd(false)
		end	
	end
end		

function CDragonCaveWnd:RetEnterDragonCave(Conn, SceneName)
	local function CallBack(Wnd, BtnIndex)
		if BtnIndex == 1 then
			Gac2Gas:IntoDragonCave(g_Conn, SceneName)
		end
		Wnd:Close()
		g_GameMain.m_ChangeOutMsgBox = nil
	end
	if g_GameMain.m_ChangeOutMsgBox then
		g_GameMain.m_ChangeOutMsgBox:Close()
		g_GameMain.m_ChangeOutMsgBox = nil
	end
	local Msg = MsgBoxMsg(210001).." (%d)"
	local OkStr = GetStaticTextClient(380001)
	local CancelStr = GetStaticTextClient(380002)
	g_GameMain.m_ChangeOutMsgBox = CCountdownWnd:new(g_GameMain, Msg, 10, CallBack, {OkStr, CancelStr})
end

function CDragonCaveWnd:SendMsgToLocalSence(Conn)
	local function CallBack(Wnd, BtnIndex)
		if BtnIndex == 1 then
			Gac2Gas:LeaveDragonCave(g_Conn)
		end
		Wnd:Close()
		g_GameMain.m_ChangeOutMsgBox = nil
	end
	if g_GameMain.m_ChangeOutMsgBox then
		g_GameMain.m_ChangeOutMsgBox:Close()
		g_GameMain.m_ChangeOutMsgBox = nil
	end
	local Msg = MsgBoxMsg(210002).." (%d)"
	local OkStr = GetStaticTextClient(380003)
	g_GameMain.m_ChangeOutMsgBox = CCountdownWnd:new(g_GameMain, Msg, 10, CallBack, {OkStr})
end

function CDragonCaveWnd:SendMsgToOtherSence(Conn)
	local function CallBack(Wnd, BtnIndex)
		if BtnIndex == 1 then
			Gac2Gas:LeaveDragonCave(g_Conn)
		end
		Wnd:Close()
		g_GameMain.m_ChangeOutMsgBox = nil
	end
	if g_GameMain.m_ChangeOutMsgBox then
		g_GameMain.m_ChangeOutMsgBox:Close()
		g_GameMain.m_ChangeOutMsgBox = nil
	end
	local Msg = MsgBoxMsg(210003).." (%d)"
	local OkStr = GetStaticTextClient(380003)
	g_GameMain.m_ChangeOutMsgBox = CCountdownWnd:new(g_GameMain, Msg, 10, CallBack, {OkStr})
end

function CDragonCaveWnd:SendMsgToEverySence(Conn)
	local function CallBack(Wnd, BtnIndex)
		if BtnIndex == 1 then
			Gac2Gas:LeaveDragonCave(g_Conn)
		end
		Wnd:Close()
		g_GameMain.m_ChangeOutMsgBox = nil
	end
	if g_GameMain.m_ChangeOutMsgBox then
		g_GameMain.m_ChangeOutMsgBox:Close()
		g_GameMain.m_ChangeOutMsgBox = nil
	end
	local Msg = MsgBoxMsg(210004).." (%d)"
	local OkStr = GetStaticTextClient(380003)
	g_GameMain.m_ChangeOutMsgBox = CCountdownWnd:new(g_GameMain, Msg, 10, CallBack, {OkStr})
end
