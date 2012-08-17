CAreaSubMenuWnd = class(SQRDialog)

--CreateSubMenu
function CAreaSubMenuWnd:Ctor(Parent, itemwidth, SubMenuNum)
	self:CreateFromRes("AreaSubMenuWnd", Parent)
	self:ShowWnd(false)
	self.m_SubSignListCtrl = self:GetDlgChild("SelectNpcSubSignListCtrl")
	
	self.m_SubSignListCtrl.m_SubItemTbl = {}
	--self.m_SubSignListCtrl:DeleteAllItem()
	self.m_SubSignListCtrl:InsertColumn(0,itemwidth)
	
	local finalwndheight = 0
	local itemheight = 0
	local ItemNum = 0
	for i, SubMenuText in pairs(g_GameMain.m_AreaInfoWnd.NpcSubMenu[SubMenuNum]) do
		self.m_SubSignListCtrl:InsertItem(i-1, 22)
		local item = self.m_SubSignListCtrl:GetSubItem(i-1, 0 )
		self.m_SubSignListCtrl.m_SubItemTbl[SubMenuText] = CAreaSelectTypeListItemWnd:new(item, SubMenuText, itemwidth, "SubMenu")
		self.m_SubSignListCtrl.m_SubItemTbl[SubMenuText]["MainMenuItem"] = Parent
		self.m_SubSignListCtrl.m_SubItemTbl[SubMenuText]:ShowWnd(true)
		
		itemheight = self.m_SubSignListCtrl.m_SubItemTbl[SubMenuText]:GetWndOrgHeight()
		self.m_SubSignListCtrl:SetItemHeight(itemheight, i)
		ItemNum = ItemNum + 1
	end
	
	-- 设置子菜单高度
	local rt = CFRect:new()
	self:GetLogicRect(rt)
	rt.bottom = rt.top + ItemNum * itemheight
	self:SetLogicRect(rt)
	self.m_SubSignListCtrl:SetLogicRect(rt)
end