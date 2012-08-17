gac_require "relation/association/AssociationListInc"
gac_require "relation/association/AssociationFriendList"
gac_require "relation/association/AssociationGroupList"

local function SetPos(wnd, x, y)
	local Rect = CFRect:new()
	wnd:GetLogicRect(Rect)
	if(x) then
		local uWidth  = Rect.right - Rect.left
		Rect.left	= x
		Rect.right	= x+uWidth
	end
	if(y) then
		local uHeight = Rect.bottom - Rect.top
		Rect.top	= y
		Rect.bottom	= y+uHeight
	end
	wnd:SetLogicRect(Rect)
end

--***************************************************************************************
--好友列表窗体
--***************************************************************************************
function CreateAssociationListWnd(parent)
	local wnd = CAssociationListWnd:new()
	wnd:Init(parent)
	return wnd
end

function CAssociationListWnd:Init(parent)
	self.m_ListPage					= 2     --list当前页
	self.m_TotalPage				= 2
	self:CreateFromRes("AssociationList", parent)
	self.m_tblList = {}
	self.m_tblList[1] = CreateAssociationFriendListWnd(self.m_ListBase)
	self.m_tblList[2] = CreateAssociationGroupListWnd(self.m_ListBase)
	self:ShowWnd(true)
	self:ChangeListPage(1, self.m_ListPage, true)
end

function CAssociationListWnd:OnChildCreated()
	self.m_ListBase		= self:GetDlgChild("ListBase")
	self.m_UpdateBtn	= self:GetDlgChild("UpdateBtn")
	self.m_tblListBtn = {}
	for i = 1, self.m_TotalPage do
		self.m_tblListBtn[i] = self:GetDlgChild("ListBtn" .. i)
	end
end

--消息
function CAssociationListWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_UpdateBtn) then
			local nNeedTime = 60
			local UpdateOnlineFriendTick = function()
				local str = GetStaticTextClient(2009)
				if(0 == nNeedTime) then
					UnRegisterTick(self.UpdateOnlineFriendTick)
					self.UpdateOnlineFriendTick = nil
					self.m_UpdateBtn:SetWndText( str )
					self.m_UpdateBtn:EnableWnd(true)
					return
				end
				nNeedTime = nNeedTime - 1
				self.m_UpdateBtn:SetWndText( str .. "(" .. nNeedTime .. ")" )
			end
			if( not self.UpdateOnlineFriendTick ) then
				self.UpdateOnlineFriendTick = RegisterTick("UpdateOnlineFriendTick", UpdateOnlineFriendTick, 1000)
				self.m_UpdateBtn:EnableWnd(false)
				Gac2Gas:UpdateOnlineFriendInfo(g_Conn)
			end
		else
			for i = 1, self.m_TotalPage do
				if(Child == self.m_tblListBtn[i]) then
					self:ChangeListPage(i, self.m_ListPage)
					break
				end
			end
		end
	end
end

------List翻页------
function CAssociationListWnd:ChangeListPage(newPage, oldPage, bNotReset)
	local dis  = newPage - oldPage
	if(0 == dis) then return end
	self.m_ListPage = newPage
	local sign = dis/math.abs(dis)
	local tbl = self.m_tblListBtn
	local btnHeight = tbl[1]:GetWndHeight()
	self:SetPosRelatively(0, btnHeight*dis, self.m_ListBase)
	for i = newPage, oldPage+sign, -sign do
		self:SetPosRelatively(0, -sign*self.m_ListBase:GetWndHeight(), sign == 1 and tbl[i] or tbl[i-sign])
	end
	for i, v in ipairs(self.m_tblList) do
		v:ShowWnd(i == newPage)
	end
	self.m_UpdateBtn:ShowWnd(1 == newPage)
	if(not bNotReset) then
		self:ChangeList(newPage)
	end
end

------重置某窗体的位置(参数是相对于原位置的值)------
function CAssociationListWnd:SetPosRelatively(x, y, wnd)
	local Rect=CFRect:new()
	wnd:GetWndRect(Rect)
	local uWidth  = Rect.right  - Rect.left
	local uHeight = Rect.bottom - Rect.top
	Rect.left     = Rect.left + x
	Rect.top      = Rect.top  + y
	Rect.right    = Rect.left + uWidth
	Rect.bottom   = Rect.top  + uHeight
	wnd:SetWndRect(Rect)
end

function CAssociationListWnd:ChangeList(newPage)
	if(1 == newPage) then      --好友面板
		self.m_tblList[1]:SetFriendList()
	elseif(2 == newPage) then  --好友群面板
		self.m_tblList[2]:SetFriendGroupList()
	end
end