gac_require "activity/MercenaryMonsterFbWnd/MercenaryMonsterFbWndInc"

function CreateMercenaryMonsterFbWnd(parent)
	local Wnd = CMercenaryMonsterFbWnd:new()
	Wnd:CreateFromRes("Fb_PVPApplyWnd",parent) 
	Wnd.m_CloseBtn = Wnd:GetDlgChild("CloseBtn")
	Wnd.m_JoinBtn = Wnd:GetDlgChild("JoinBtn")
	Wnd.m_CancelBtn = Wnd:GetDlgChild("CancelBtn")
	Wnd.m_FbGameInfo = Wnd:GetDlgChild("FbGameInfo")
	Wnd.m_Title = Wnd:GetDlgChild("WndName")
	
	Wnd.m_Title:SetWndText(GetStaticTextClient(9413))
	Wnd.m_FbGameInfo:SetWndText(GetDescriptionDisplayName("”∂±¯Õ≈À¢π÷±æΩÈ…‹"))
		
	g_ExcludeWndMgr:InitExcludeWnd(Wnd, 1)
	Wnd:ShowWnd(false)
	return Wnd
end

function Gas2Gac:RetShowMercenaryMonsterFbWnd(Conn)
	local Wnd = g_GameMain.m_MercenaryMonsterFbWnd
	Wnd:ShowWnd(true)
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(Wnd)
end

function CMercenaryMonsterFbWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_JoinBtn then
			Gac2Gas:EnterGeneralMercenaryMonsterFb(g_Conn)
		end
		self:ShowWnd(false)
	end
end


function Gas2Gac:RetIsJoinMercenaryMonsterFb(Connection, SceneName, LeaderTongId)
	local function IsJoinMercenaryMonsterFb(wnd, btnIndex)
		if btnIndex == 1 then
			Gac2Gas:AgreedJoinMercenaryMonsterFb(g_Conn, SceneName, LeaderTongId)
		end
		wnd:Close()
		g_GameMain.m_ChangeOutMsgBox = nil
	end
	
	if g_GameMain.m_ChangeOutMsgBox then
		g_GameMain.m_ChangeOutMsgBox:Close()
		g_GameMain.m_ChangeOutMsgBox = nil
	end
	
	local msg = MsgBoxMsg(5001, GetSceneDispalyName(SceneName))
	msg = msg.." (%d)"
	local OkStr = GetStaticTextClient(1124)
	g_GameMain.m_ChangeOutMsgBox = CCountdownWnd:new(g_GameMain, msg, 30, IsJoinMercenaryMonsterFb, {OkStr})
end