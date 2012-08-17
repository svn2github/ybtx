--gac_require "relation/tong/TongSellResWndInc"
CTongSellForageWnd = class(SQRDialog)

------帮会出售粮草面板------
function CTongSellForageWnd:Ctor(parent,flag)
	self:CreateFromRes("TongSellForageWnd", parent)
	self.m_Title = self:GetDlgChild("Title")
	self.m_Introduce = self:GetDlgChild("Introduce")
	self.m_SoldInfo = self:GetDlgChild("SoldInfo")
	self.m_SoldNum = self:GetDlgChild("SoldNum")
	self.m_GetInfo = self:GetDlgChild("GetInfo")
	self.m_GetNum = self:GetDlgChild("GetNum")
	self.m_ConfirmBtn = self:GetDlgChild("ConfirmBtn")
	self.m_CancelBtn = self:GetDlgChild("CancelBtn")
	self.m_XBtn = self:GetDlgChild("XBtn")
	self.m_Introduce:SetWndText(GetStaticTextClient(320003))
	g_ExcludeWndMgr:InitExcludeWnd(self, 2, false, nil, 1)
end

function CTongSellForageWnd.GetWnd(nNum, nProffer, tongId)
	if not g_GameMain.m_TongSellRes then
		g_GameMain.m_TongSellRes = CTongSellForageWnd:new(g_GameMain)
	end
	local wnd = g_GameMain.m_TongSellRes 
	wnd.m_SoldNum:SetWndText(nNum)
	wnd.m_GetNum:SetWndText(nProffer)
	wnd.m_TongID = tongId
	wnd:ShowWnd(true)
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(wnd)
end


--消息
function CTongSellForageWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_CancelBtn or Child == self.m_XBtn then
			self:ShowWnd(false)
		elseif(Child == self.m_ConfirmBtn) then
			Gac2Gas:SellTongResource(g_Conn, self.m_TongID)
			self:ShowWnd(false)
		end
	end
end
