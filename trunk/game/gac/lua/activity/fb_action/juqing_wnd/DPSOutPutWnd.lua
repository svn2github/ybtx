CDpsOutPutWnd = class( SQRDialog )

function CDpsOutPutWnd:Ctor(parent)
	self:CreateFromRes("FbAction_DPSWnd", parent)
	self.m_List = self:GetDlgChild("List")
	self.m_OkBtn = self:GetDlgChild("OKBtn")
	
	self:InitListWnd()
end

function CDpsOutPutWnd:GetWnd()
	local Wnd = g_GameMain.m_DpsOutPutWnd
	if not Wnd then
		Wnd = CDpsOutPutWnd:new(g_GameMain)
		g_GameMain.m_DpsOutPutWnd = Wnd
	end
	return Wnd
end

function CDpsOutPutWnd:InitListWnd()
	self.m_List:DeleteAllItem()
	self.m_List:InsertColumn(0, self.m_List:GetWndOrgWidth()/5)
	self.m_List:InsertColumn(1, self.m_List:GetWndOrgWidth()/5)
	self.m_List:InsertColumn(2, self.m_List:GetWndOrgWidth()/5)
	self.m_List:InsertColumn(3, self.m_List:GetWndOrgWidth()/5)
	self.m_List:InsertColumn(3, self.m_List:GetWndOrgWidth()/5)
	
	self.m_CurrState = 0
end

function CDpsOutPutWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if (uMsgID == BUTTON_LCLICK) then
		if Child == self.m_OkBtn then
			self:ShowWnd(false)
		end
	end
end

function CDpsOutPutWnd:AddItem(Name, Hurt, Heal, Coef, Point, Rate)
	if self.m_CurrState == 1 then
		self:InitListWnd()
	end
	
	local ItemNum = self.m_List:GetItemCount()
	self.m_List:InsertItem(ItemNum, 30)
	local Item = self.m_List:GetSubItem(ItemNum, 0)
	Item:SetWndText(Name)
	local Item = self.m_List:GetSubItem(ItemNum, 1)
	Item:SetWndText(Hurt)
	local Item = self.m_List:GetSubItem(ItemNum, 2)
	Item:SetWndText(Heal)
	local Item = self.m_List:GetSubItem(ItemNum, 3)
	Item:SetWndText(Coef.."%")
	local Item = self.m_List:GetSubItem(ItemNum, 4)
	local str = ""..Point
	if Point > 0 and Rate > 1 then
		str = str .. "(+" .. Point/Rate .. ")"
	end
	Item:SetWndText(str)
end

function CDpsOutPutWnd:ShowDPSWnd()
	self:ShowWnd(true)
	self.m_CurrState = 1
end
