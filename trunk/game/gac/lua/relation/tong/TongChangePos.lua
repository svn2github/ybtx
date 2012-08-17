gac_require "relation/tong/TongChangePosInc"
cfg_load "tong/TongPurview_Common"
------修改职位面板------

function CreateTongChangePosWnd(parent)
	local wnd = CTongChangePosWnd:new()
	wnd:Init(parent)
	return wnd
end

function CTongChangePosWnd:Init(parent)
	self.m_Id		= 0
	self.m_OldPos	= 0
	self:CreateFromRes("TongChangePos", parent)
	g_ExcludeWndMgr:InitExcludeWnd(self, 3, false, nil, 1)
end

function CTongChangePosWnd:OnChildCreated()
	self.m_ShutDownBtn	= self:GetDlgChild("CancelBtn")
	self.m_Lable		= self:GetDlgChild("Lable")
	self.m_OkBtn		= self:GetDlgChild("OkBtn")
	self.m_PosList		= self:GetDlgChild("PosList")
	self.m_XBtn			= self:GetDlgChild("XBtn")
end

--消息
function CTongChangePosWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_ShutDownBtn or Child == self.m_XBtn) then
			self:ShowWnd(false)
		elseif(Child == self.m_OkBtn) then
			self:OnChangePos()
		end
	end
end

function CTongChangePosWnd:OnChangePos()
	local itemIndex = self.m_PosList:GetSelectItem(-1)+1
	local tbl = g_GameMain.m_TongMainPan.m_tblPanel[2].m_tblTongPosMem
	if(not itemIndex or itemIndex < 1 or itemIndex > #tbl) then return end
	self:ShowWnd(false)
	Gac2Gas:ChangePos(g_Conn, self.m_Id, self.m_OldPos, itemIndex)
end

function CTongChangePosWnd:OpenPanel(id, name, pos)
	self.m_Id		= id
	self.m_OldPos	= pos
	self.m_Lable:SetWndText(GetStaticTextClient(1089, name))
	self:DrawPosList()
	self:ShowWnd(true)
	self:SetFocus()
end

----------------------------------------------------------------------
function CTongChangePosWnd:DrawPosList()
	self.m_PosList:DeleteAllItem()
	self.tblPosItem = {}
	self.m_PosList:InsertColumn(0, self.m_PosList:GetWndOrgWidth())
	local tbl = g_GameMain.m_TongMainPan.m_tblPanel[2].m_tblTongPosMem
	for i = 1, #tbl do
		self.m_PosList:InsertItem(i-1, 26)
		local item = self.m_PosList:GetSubItem(i-1, 0)
		local posListItem = self:CreateTongPosListItemWnd(item)
		table.insert(self.tblPosItem, posListItem)
		posListItem:SetTongPosListText(i,tbl[i][1], tbl[i][2] or "")
	end
end

------创建帮会职位列表的行窗体
function CTongChangePosWnd:CreateTongPosListItemWnd(Parent)
	local Wnd = CTongPosListItem:new()
	Wnd:CreateFromRes("TongChangePosListItem", Parent)
	Wnd:ShowWnd(true)
	return Wnd
end

--*************************
------list的子窗体类-------
--*************************
function CTongPosListItem:OnChildCreated()
	self.m_Pos	= self:GetDlgChild("Pos")
	self.m_Name	= self:GetDlgChild("Name")
end

function CTongPosListItem:SetTongPosListText(i,pos, name)
	local tblInfo = TongPurview_Common("UpperNumber")
	local tblPos = {tblInfo("Level1"), tblInfo("Level2"), tblInfo("Level3"), tblInfo("Level4"), tblInfo("Level5")}
	if i == 1 then
		self.m_Pos:SetWndText(GetStaticTextClient(1090,pos))
	else
		self.m_Pos:SetWndText(GetStaticTextClient(1091,pos,tblPos[i]))
	end
	self.m_Name:SetWndText("")
end