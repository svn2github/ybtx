CDelRoleList		= class(CSelectCharWnd)
CDelRoleListItem	= class(SQRDialog)

function CDelRoleList:Ctor()
	self:CreateFromRes("Login_DelCharList", g_SelectChar.m_SelectCharWnd)
end

function CDelRoleList:OnChildCreated()
	self.m_Live	= self:GetDlgChild("Live")
	self.m_List	= self:GetDlgChild("List")
	self.m_Live:EnableWnd(false)
end

function CDelRoleList:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		if(Child == self.m_Live) then
			self:OnMakeRoleAlive()
		end
	elseif( uMsgID == ITEM_LBUTTONCLICK ) then
		if(Child == self.m_List) then
			self.m_Live:EnableWnd(true)
		end
	end
end

--点击恢复角色按钮后的事件
function CDelRoleList:OnGetBackRole()
	local tbl = g_SelectChar.m_tblRolesInfo.m_tblPlayerListGroupByActive
	if( #(tbl[2]) < 1 ) then
		MessageBox( g_SelectChar, MsgBoxMsg(11003), MB_BtnOK )
		return false
	end
	if( #(tbl[1]) >= CSelectCharState.SelectCharConfig.nMaxRole ) then --如果没有空位并且有5个以上的已删除角色时
		MessageBox( g_SelectChar, MsgBoxMsg(11005), MB_BtnOK ) --提示当前创建的角色过多,请删除至少一个角色后继续
		return false
	end
	
	local nIndex	= self.m_List:GetSelectItem(-1) + 1
	local role		= g_SelectChar.m_tblRolesInfo.m_tblPlayerListGroupByActive[2][nIndex]
	if(not role) then
		MessageBox( g_SelectChar, MsgBoxMsg(11004), MB_BtnOK )
		return false
	end
	return true
end

function CDelRoleList:OnMakeRoleAlive()
	if self:OnGetBackRole() then --恢复
		local nIndex = self.m_List:GetSelectItem(-1) + 1
		local role = g_SelectChar.m_tblRolesInfo.m_tblPlayerListGroupByActive[2][nIndex]
		g_SelectChar.m_wndDelRole:OnShow(role, false)
	end
end

function CDelRoleList:DrawList()
	self.m_Live:EnableWnd(false)
	self.m_List:DeleteAllItem()
	self.m_tblItem = {}
	self.m_List:InsertColumn(0, self.m_List:GetWorkAreaOrgWidth())
	local tbl = g_SelectChar.m_tblRolesInfo.m_tblPlayerListGroupByActive[2]
	for i, v in ipairs(tbl) do
		self.m_List:InsertItem(i-1, 50)
		local item = self.m_List:GetSubItem(i-1, 0)
		local wnd = self:CreateCharListItemWnd(item)
		table.insert(self.m_tblItem, wnd)
		wnd:SetInfo(v.m_name, v.m_Class, v.m_level, v.m_Camp)
	end
end

-------------------------------------------------------------------------------------------
function CDelRoleList:CreateCharListItemWnd(parent)
	local wnd = CDelRoleListItem:new()
	wnd:CreateFromRes("Login_SelectCharListItem", parent)
	wnd:ShowWnd(true)
	return wnd
end

function CDelRoleListItem:OnChildCreated()
	self.m_Icon		= self:GetDlgChild("Icon")
	self.m_Name		= self:GetDlgChild("Name")
	self.m_Class	= self:GetDlgChild("Class")
	self.m_Level	= self:GetDlgChild("Level")
end

function CDelRoleListItem:SetInfo(sName, nClass, nLevel, nCamp)
	local sClass	= g_DisplayCommon:GetPlayerClassForShow(nClass)
	local icon		= g_DynImageMgr:GetClassMetalPic(nCamp, nClass)
	g_DynImageMgr:AddImageByIP(self.m_Icon, icon, IP_ENABLE)
	self.m_Name:SetWndText(sName)
	self.m_Class:SetWndText(sClass)
	self.m_Level:SetWndText( GetStaticTextClient(8322, nLevel) )
end