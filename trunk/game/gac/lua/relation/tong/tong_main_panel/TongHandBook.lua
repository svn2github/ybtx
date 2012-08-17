lan_load "tong/Lan_TongHandBook_Client"

CTongHandBookWnd		= class(SQRDialog)	--导出类
CTongHandBookListItem	= class(SQRDialog)

function CTongHandBookWnd.CreateTongHandBookWnd(parent)
	local wnd = CTongHandBookWnd:new()
	wnd:Init(parent)
	return wnd
end

function CTongHandBookWnd:Init(parent)
	self:CreateFromRes("TongHandBook", parent)
	self:InitList()
end

function CTongHandBookWnd:OnChildCreated()
	self.m_TypeList	= self:GetDlgChild("TypeList")
	self.m_Des		= self:GetDlgChild("Des")
end

--消息
function CTongHandBookWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == ITEM_LBUTTONCLICK) then
		if(Child == self.m_TypeList) then
			self:ShowDescription(uParam1 + 1)
		end
	end
end

function CTongHandBookWnd:OpenPanel(bFlag)
	self:ShowWnd(bFlag)
end

function CTongHandBookWnd:ShowDescription(nIndex)
	local sDescription	= Lan_TongHandBook_Client(nIndex, "Description")
	self.m_Des:SetWndText(sDescription)
end

function CTongHandBookWnd:InitList()
	self:DrawList()
	self.m_TypeList:SelectItem(0)
	self:ShowDescription(1)
end

function CTongHandBookWnd:DrawList()
	self.m_tblItemWnd = {}
	self.m_TypeList:DeleteAllItem()
	self.m_TypeList:InsertColumn(0, self.m_TypeList:GetWndOrgWidth())
	for i, v in ipairs( Lan_TongHandBook_Client:GetKeys() ) do
		self.m_TypeList:InsertItem(i-1, 30)
		local item		= self.m_TypeList:GetSubItem(i-1, 0)
		local itemWnd	= self:CreateTongHandBookListItem(item)
		local sType		= Lan_TongHandBook_Client(v, "DisplayType")
		table.insert(self.m_tblItemWnd, itemWnd)
		itemWnd:SetWndText(sType)
	end
end

--------------------------------------------------
function CTongHandBookWnd:CreateTongHandBookListItem(parent)
	local wnd = CTongHandBookListItem:new()
	wnd:CreateFromRes("TongHandBookListItem", parent)
	wnd:ShowWnd(true)
	return wnd
end