gac_require "relation/tong/TongRecommendInc"

------NPC引荐面板------

function CreateTongRecommendWnd(Parent)
	local Wnd = CTongRecommendWnd:new()
	Wnd:Init(Parent)
	return Wnd
end

function CTongRecommendWnd:Init(Parent)
	self:CreateFromRes("TongRecommend", Parent)
	g_ExcludeWndMgr:InitExcludeWnd(self, 2, false, nil, 1)
end

function CTongRecommendWnd:OnChildCreated()
	self.m_CancelBtn = self:GetDlgChild("CancelBtn")
	self.m_OkBtn     = self:GetDlgChild("OkBtn")
	self.m_NameLable = self:GetDlgChild("NameLable")
	self.m_DesText   = self:GetDlgChild("DesText")
	self.m_NameEdit  = self:GetDlgChild("NameEdit")
	self.m_XBtn      = self:GetDlgChild("XBtn")
	
	self.m_NameEdit:SetMaxTextLenght(30)
	self.m_DesText:SetMaxTextLenght(80)
end

--消息
function CTongRecommendWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_CancelBtn or Child == self.m_XBtn) then
			self:ShowWnd(false)
		elseif(Child == self.m_OkBtn) then
			self:OnRecommendJoinTong()
		end
	end
end

function CTongRecommendWnd:OpenPanel(openType)
	if( g_MainPlayer.m_Properties:GetTongID() == 0 ) then
		MsgClient(9401)
		return
	end
	self:ShowWnd(true)
	
	local str = openType and GetStaticTextClient(10005) or GetStaticTextClient(10006)
	self.m_NameLable:SetWndText(str)
	self.m_NameEdit:EnableWnd(openType)
	self.m_NameEdit:SetWndText("")
end
	
function CTongRecommendWnd:OnRecommendJoinTong()
	local name = self.m_NameEdit:GetWndText()
	local des = self.m_DesText:GetWndText()
	if("" == name) then
		MsgClient(9033)
		return
	end
	Gac2Gas:RecommendJoinTong(g_Conn, name, des)
	self:ShowWnd(false)
end

function Gas2Gac:ReseiveRecommendJoinTong(Conn, inviterId, inviterName, tongName)

end