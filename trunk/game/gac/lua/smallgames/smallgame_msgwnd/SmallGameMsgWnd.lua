gac_require "smallgames/smallgame_msgwnd/SmallGameMsgWndInc"
lan_load "smallgame/Lan_SmallGame_Common"

function CreateSmallGameMsgWnd(parent)
	local Wnd = CSmallGameMsgWnd:new()
	Wnd:CreateFromRes("SmallGameMsgWnd",parent)
	Wnd:SetStyle(0x80000000)--设置成模态窗口
	Wnd.m_WndTitle = Wnd:GetDlgChild("TitleWnd")
	Wnd.m_OkBtn = Wnd:GetDlgChild("OkBtn")
	Wnd.m_CancelBtn = Wnd:GetDlgChild("CloseBtn")
	Wnd.m_InfoShow = Wnd:GetDlgChild("InfoShow")
	
	g_ExcludeWndMgr:InitExcludeWnd(Wnd, 1)
	Wnd:ShowWnd(false)
	
	return Wnd
end

function CSmallGameMsgWnd:InitSmallGameMsgWnd(GameName,TargetID)
	self.m_SamellGameName = GameName
	self.m_TargetId = TargetID
	self.m_IsAgree = nil
	
	self.m_WndTitle:SetWndText(Lan_SmallGame_Common(MemH64(GameName), "ShowName"))
	local str = "#c392206"..Lan_SmallGame_Common(MemH64(GameName), "GameInfo")
	self.m_InfoShow:SetWndText(str)
	
	local GameCommon = SmallGame_Common(GameName)
	
	if GameCommon then
		local strTexture = GameCommon("MsgWndTexFile")
		if strTexture and strTexture ~= "" then
			self:AddImage(strTexture)
		end
	end
	
	self:ShowWnd(true)
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(self)
end

function CSmallGameMsgWnd:AddImage(strTexture)
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP_ENABLE	
	local DefaultImage = WND_IMAGE_LIST:new()
	DefaultImage:AddImage(self:GetGraphic(), -1,  strTexture, nil, CFPos:new(), "0xffffffff", 15 ) 	
	self:SetWndBkImage( Flag, DefaultImage )
end

function CSmallGameMsgWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		
		if Child == self.m_OkBtn then
			self.m_IsAgree = true
			self:ShowWnd(false)
		elseif Child == self.m_CancelBtn then
			self:ShowWnd(false)
		end

	end
end

function CSmallGameMsgWnd:VirtualExcludeWndClosed()
	if self.m_IsAgree then
		Gac2Gas:AgreeBeginSmallGame(g_Conn,self.m_SamellGameName,self.m_TargetId)
	else
		Gac2Gas:AgreeBeginSmallGame(g_Conn,"",self.m_TargetId)
	end
end

function Gas2Gac:CloseSmallGameMsgWnd(Conn)
	if IsFunction(g_GameMain.m_SmallGemeMsgWnd.IsShow) then
		g_GameMain.m_SmallGemeMsgWnd:ShowWnd(false)
	end
end
