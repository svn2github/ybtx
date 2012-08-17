CBossBattleWnd = class(SQRDialog)
local CBossBattleInfoWnd = class(SQRDialog)

local function CreateBossBattleInfoWnd(parent)
	local Wnd = CBossBattleInfoWnd:new()
	Wnd:CreateFromRes("BossBattleInfoWnd",parent) 
	Wnd.m_Title = Wnd:GetDlgChild("Title")
	Wnd.m_Static = Wnd:GetDlgChild("Static")
	Wnd.m_Count = Wnd:GetDlgChild("Count")
	--Wnd.m_Title:SetWndText(GetStaticTextClient(9413))
	--Wnd.m_Static:SetWndText(GetStaticTextClient(9413)..":")
	return Wnd
end

function CBossBattleWnd:Ctor()
	self:CreateFromRes("Fb_PVPApplyWnd",g_GameMain) 
	self.m_CloseBtn = self:GetDlgChild("CloseBtn")
	self.m_JoinBtn = self:GetDlgChild("JoinBtn")
	self.m_CancelBtn = self:GetDlgChild("CancelBtn")
	self.m_FbGameInfo = self:GetDlgChild("FbGameInfo")
	self.m_Title = self:GetDlgChild("WndName")
	
	self.m_Title:SetWndText(GetStaticTextClient(9443))
	self.m_FbGameInfo:SetWndText(GetDescriptionDisplayName("Boss’˘∂·’Ω"))
	
	self.m_InfoWnd = CreateBossBattleInfoWnd(parent)
	
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
	self:ShowWnd(false)
end

function CBossBattleWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_JoinBtn then
			Gac2Gas:CanJoinBossBattle(g_Conn)
		elseif Child == self.m_CancelBtn then
			Gac2Gas:CancelJoinBossBattle(g_Conn)
		end
		self:ShowWnd(false)
	end
end

function CBossBattleWnd:RetEnterBossBattle(SceneName)
	local function CallBack(Wnd, BtnIndex)
		Gac2Gas:EnterBossBattle(g_Conn, BtnIndex, SceneName)
		Wnd:Close()
		g_GameMain.m_ChangeOutMsgBox = nil
	end
	if g_GameMain.m_ChangeOutMsgBox then
		g_GameMain.m_ChangeOutMsgBox:Close()
		g_GameMain.m_ChangeOutMsgBox = nil
	end
	local Msg = MsgBoxMsg(5016) .. " (%d)"
	local OkStr = GetStaticTextClient(1124)
	local CancelStr = GetStaticTextClient(1025)
	g_GameMain.m_ChangeOutMsgBox = CCountdownWnd:new(g_GameMain, Msg, 30, CallBack, {OkStr, CancelStr})
end

function CBossBattleWnd:RetLeaveBossBattle(NotKillBoss)
	local function CallBack(Wnd, BtnIndex)
		if BtnIndex == 1 then
			Gac2Gas:TimeOverLeaveBossBattle(g_Conn)
		end
		Wnd:Close()
		g_GameMain.m_ChangeOutMsgBox = nil
	end
	if g_GameMain.m_ChangeOutMsgBox then
		g_GameMain.m_ChangeOutMsgBox:Close()
		g_GameMain.m_ChangeOutMsgBox = nil
	end
	local Msg = ( (NotKillBoss and MsgBoxMsg(5018)) or MsgBoxMsg(5019) ) .. " (%d)"
	local OkStr = GetStaticTextClient(1124)
	g_GameMain.m_ChangeOutMsgBox = CCountdownWnd:new(g_GameMain, Msg, 5, CallBack, {OkStr})
end

function CBossBattleWnd:RetWaitBossBattle()
	MessageBox( g_GameMain, MsgBoxMsg(5017), MB_BtnOK)
end

function CBossBattleWnd:BossBattleKillBossMsg(SceneName, BossName, TongName, PlayerName)
	if BossName ~= "" then
		local SceneDispalyName = GetSceneDispalyName(SceneName)
		local BossDispalyName = GetNpcDisplayName(BossName)
		MsgClient(192013, SceneDispalyName, BossDispalyName, TongName, PlayerName)
	end
end

function CBossBattleWnd:BossBattleJoinFailMsg(SceneName)
	if SceneName ~= "" then
		MsgClient(192015, GetSceneDispalyName(SceneName))
	else
		MsgClient(192016)
	end
end
