
CFriendsListWnd		= class(SQRDialog) 
CFriendsListItem	= class(SQRDialog)

function CFriendsListWnd:OnChildCreated()
	self.m_senderlist = self:GetDlgChild("emailsenderlist")
end

function CFriendsListWnd:InitFriendListChild()
	self:SetFriendAndRelationList()
end

--设置列表listctrl
function CFriendsListWnd:SetFriendAndRelationList()
	
	self.m_senderlist:DeleteAllItem()
	self.m_tblItemWnd = {}				--记录分组
	self.m_senderlist:InsertColumn(0, self.m_senderlist:GetWndOrgWidth())

	local tblFriendList = g_GameMain.m_AssociationBase.m_tblFriendListClass

	for i=0, #tblFriendList do
		self.m_senderlist:InsertItem(i,20)
		local item = self.m_senderlist:GetSubItem(i,0)
		local itemWnd = self:CreatFriendListItem(item,i,self)
		self.m_tblItemWnd[i+1]= itemWnd
		itemWnd:ResetItem()

	end

end

--关闭组列表
function CFriendsListWnd:CloseAll()
	for i = 1, #(self.m_tblItemWnd) do
		self.m_tblItemWnd[i]:ShowChildList(flase)
	end	
end

--好友列表listctrl的下的子list
function CFriendsListWnd:CreatFriendListItem(parent,index,fatherWnd)
	local wnd = CFriendsListItem:new(parent,index,fatherWnd)
	return wnd
end

function CFriendsListItem:Ctor(parent,index,fatherWnd)

	self.m_fatherWnd	= fatherWnd
	self.m_itemIndex	= index
	self:CreateFromRes("senderListItem", parent)
	self.m_TitleCheck	= self:GetDlgChild("TitleCheck")
	self.m_Title		= self:GetDlgChild("Title")
	self.m_ChildList	= self:GetDlgChild("ChildList")
	
	self.m_nChildItemCount = 0 --用于记录子item下的item数目
	self:ShowWnd(true)
	
end

--重设当前一级item
function CFriendsListItem:ResetItem()
	self:SetChildList()
	self:ShowChildList(false)
end

function CFriendsListItem:SetWndHeight(wnd, height)
	local Rect=CFRect:new()
	wnd:GetLogicRect(Rect)
	Rect.bottom = Rect.top + height
	wnd:SetLogicRect(Rect)
end

function CFriendsListItem:SetChildList()
	self.m_ChildList:DeleteAllItem()

	local tblFriendList = g_GameMain.m_AssociationBase.m_tblFriendListClass
	if(self.m_itemIndex <= #tblFriendList -1) then
		self:DrawFriendList(self.m_itemIndex)
	
	elseif(self.m_itemIndex == #tblFriendList) then
		self:DrawSenderList()
	end
end


function CFriendsListItem:ShowChildList(bFlag)

	local height	= bFlag and self.m_nChildItemCount*20 or 0

	self.m_TitleCheck:SetCheck(bFlag)
	self.m_ChildList:ShowWnd(bFlag)
	self.m_fatherWnd.m_senderlist:SetItemHeight(height+20, self.m_itemIndex)
end

--初始化好友列表
function CFriendsListItem:DrawFriendList(i)

	self.m_ChildList:InsertColumn(0,self.m_ChildList:GetWndOrgWidth())
	local tblFriendList = g_GameMain.m_AssociationBase.m_tblFriendListClass
	local len = #tblFriendList[i+1]
	for j=1, len do
		local friendsname =	tblFriendList[i+1][j].playerName		
		self.m_ChildList:InsertItem(number, 20)
		self.m_ChildList:GetSubItem(number, 0):SetWndText(friendsname)
	end

	local height = len*20
	self:SetWndHeight(self.m_ChildList, height)
	self:SetWndHeight(self, height+20)
	self.m_Title:SetWndText(tblFriendList[i+1].className)
	self.m_nChildItemCount = len

end

--初始化最近联系人列表
function CFriendsListItem:DrawSenderList()
	local nCharID = g_MainPlayer.m_uID
	MakeDirectoryUTF8(g_RootPath .. "var/gac/Emailsendlist")
	local strpath = g_RootPath .. "var/gac/Emailsendlist/EmailSenderList_" .. nCharID .. ".txt"
	
	self.m_Title:SetWndText(GetStaticTextClient(2012))
	local data = ""
	local f = CLuaIO_Open(strpath, "rb")
	if f then
		data = f:ReadAll()
		if(data == "") then
		else
			self.m_ChildList:InsertColumn(0,self.m_ChildList:GetWndOrgWidth())
			local number=0
			while(true) do
				if(data == "") then 
					break
				end
				local pos = string.find(data, "\n")
				if not pos then
					break
				end
				if(string.sub(data,1,pos-2) == "")then
					data = string.sub(data,pos+1,string.len(data))
					break;
				end

				local itemCount = self.m_ChildList:GetItemCount()
				self.m_ChildList:InsertItem( itemCount, 20)
				self.m_ChildList:GetSubItem( itemCount, 0 ):SetWndText(string.sub(data,1,pos-2))
				number	= number +1

				data = string.sub(data,pos+1,string.len(data))				
			end

			local height	= number*20
			self:SetWndHeight(self.m_ChildList, height)
			self:SetWndHeight(self, height+20)			
			self.m_nChildItemCount= number
			
		end
		f:Close()
	else
		local fo = CLuaIO_Open(strpath, "w+")
		if fo then
			local data = fo:WriteString("")
			fo:Close()
		end
	end
end
--好友列表窗口事件响应函数
function CFriendsListItem:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == ITEM_LBUTTONCLICK)then
		local index = self.m_ChildList:GetSelectItem( -1 )
		local wnd = self.m_ChildList:GetSubItem(index,0)
		g_GameMain.m_SendBox.m_ReceiverName:SetWndText(wnd:GetWndText())

	elseif (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_TitleCheck) then
			local bFlag = self.m_ChildList:IsShow()
			self.m_fatherWnd:CloseAll()
			self:ShowChildList(not bFlag)
		end
	
	end
end
