gac_gas_require "framework/common/CMoney"

CTongProductionBase			= class()
CTongProductionCreatingIcon	= class(SQRDialog)
CTongProductionListItemBase	= class()

function CTongProductionBase:OnChildCreated()
	self.m_XBtn				= self:GetDlgChild("XBtn")
	self.m_ItemList			= self:GetDlgChild("ItemList")
	self.m_CreatingIconBtn	= self:GetDlgChild("CreatingIconBtn")
	self.m_CreatingBar		= self:GetDlgChild("CreatingBar")
	self.m_ShowAvailableCheckBtn = self:GetDlgChild("ShowAvailableCheckBtn")
	self.m_ShowAvailableText = self:GetDlgChild("ShowAvailableText")
	self.m_CreatingBar:SetProgressMode(0)
	self.labelTbl = {}
	for i = 1, 4 do
		table.insert(self.labelTbl,self:GetDlgChild("CheckBtn" .. i))
	end
	
	self.m_CreatingIcon	= self:CreateIconWnd(self.m_CreatingIconBtn)
	
	self.m_tblWaitingIconBtn	= {}
	self.m_tblWaitingIcon		= {}
	for i = 1, 6 do
		self.m_tblWaitingIconBtn[i]	= self:GetDlgChild("WaitingIconBtn" .. i)
		self.m_tblWaitingIconBtn[i]:ShowWnd(false)
		self.m_tblWaitingIcon[i]	= self:CreateIconWnd(self.m_tblWaitingIconBtn[i])
	end
end

 function CTongProductionBase:SelectLabelToDrawItem(showAvailable)
	for i,v in ipairs(self.labelTbl) do
		if v:GetCheck() then
			self:DrawItemCreateList(i,showAvailable)
		end
	end
end

function CTongProductionBase:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_XBtn) then
			self:ShowWnd(false)
		elseif ( Child == self.m_ShowAvailableCheckBtn or Child == self.m_ShowAvailableText ) then
			self.m_ShowAvailableCheckBtn:SetCheck(not self.m_ShowAvailableCheckBtn:GetCheck())
			if self.m_ShowAvailableCheckBtn:GetCheck() then				
				self:SelectLabelToDrawItem(true)
			else
				self:SelectLabelToDrawItem(false)
			end
		else
			for i,v in ipairs(self.labelTbl) do
				if Child == v then
					if v:GetCheck() then return end
					self:OnLabelGetCheck(Child)
					self:DrawItemCreateList(i,self.m_ShowAvailableCheckBtn:GetCheck())
				end
			end
		end
	elseif (uMsgID == BUTTON_RCLICK) then
		if(Child == self.m_CreatingIconBtn) then
			
			local function IsCancelCreating(g_GameMain, uButton)
				if(uButton == MB_BtnOK) then
					self:OnCancelCreateItem(Child)
				end
				g_GameMain.m_MsgBox = nil
				return true
			end
			
			g_GameMain.m_MsgBox = MessageBox( g_GameMain, MsgBoxMsg(17003), BitOr( MB_BtnOK, MB_BtnCancel), IsCancelCreating, g_GameMain)
		else
			for i = 1, 6 do
				if(Child == self.m_tblWaitingIconBtn[i]) then
					self:OnCancelCreateItem(Child)
				end
			end
		end
	end
end

function CTongProductionBase:OnLabelGetCheck(checkBtn)
	if not checkBtn:GetCheck() then checkBtn:SetCheck(true) end
	for i, v in ipairs(self.labelTbl) do
		if v ~= checkBtn and v:GetCheck() then
			v:SetCheck(false)
		end
	end
end

function CTongProductionBase:VirtualExcludeWndClosed()
	if(self.CreatingBuildingItemTick) then
		UnRegisterTick(self.CreatingBuildingItemTick)
		self.CreatingBuildingItemTick = nil
	end
end

function CTongProductionBase:UpdateInfo(tblInfo)
	local	creatingItemName, creatingItemId, creatingItemPassedTime,
			waitingItemName1, waitingItemId1, waitingItemName2, waitingItemId2, waitingItemName3, waitingItemId3,
			waitingItemName4, waitingItemId4, waitingItemName5, waitingItemId5, waitingItemName6, waitingItemId6
			= unpack(tblInfo)
	
	local tblWaitingItem =	{	{waitingItemId1, waitingItemName1}, {waitingItemId2, waitingItemName2},
								{waitingItemId3, waitingItemName3}, {waitingItemId4, waitingItemName4},
								{waitingItemId5, waitingItemName5}, {waitingItemId6, waitingItemName6}	}
	
	self:UpdateCreatingInfo(creatingItemId, creatingItemName, creatingItemPassedTime)
	
	--等待制造的物品信息
	for i = 1, 6 do
		self.m_tblWaitingIconBtn[i].itemId = tblWaitingItem[i][1]
		if(0 == tblWaitingItem[i][1]) then
			self.m_tblWaitingIconBtn[i]:ShowWnd(false)
		else
			self.m_tblWaitingIconBtn[i]:ShowWnd(true)
			self:SetWaitingIcon(tblWaitingItem[i], i)
		end
	end
end

--正在制造的物品信息
function CTongProductionBase:UpdateCreatingInfo(creatingItemId, creatingItemName, creatingItemPassedTime)
	if(self.CreatingBuildingItemTick) then
		UnRegisterTick(self.CreatingBuildingItemTick)
		self.CreatingBuildingItemTick = nil
	end
	
	if( not self:IsShow() ) then return end
	self.m_CreatingIconBtn.itemId = creatingItemId
	if("" == creatingItemName) then
		self.m_CreatingIconBtn:ShowWnd(false)
		self.m_CreatingBar:ShowWnd(false)
	else
		self.m_CreatingIconBtn:ShowWnd(true)
		self.m_CreatingBar:ShowWnd(true)
		local itemType, wholeTime = self:GetTongProdItemInfo(creatingItemName)
		
		local CreatingBuildingItemTick = function()
			if(self.m_CreatingBar:GetPos() >= wholeTime) then
				UnRegisterTick(self.CreatingBuildingItemTick)
				self.CreatingBuildingItemTick = nil
				self:OnGetCreatingItemInfo()
			else
				local newValue = self.m_CreatingBar:GetPos() + 1
				self.m_CreatingBar:SetPos(newValue)
				self.m_CreatingBar:SetWndText(newValue .. "/" .. wholeTime)
			end
		end
		
		g_LoadIconFromRes(g_ItemInfoMgr:GetItemInfo(itemType, creatingItemName,"SmallIcon"), self.m_CreatingIcon, -1, IP_ENABLE, IP_ENABLE)
		g_SetItemRichToolTips(self.m_CreatingIconBtn, itemType, creatingItemName, 0)
		self.m_CreatingBar:SetRange(wholeTime)
		self.m_CreatingBar:SetPos(creatingItemPassedTime)
		self.m_CreatingBar:SetWndText(creatingItemPassedTime .. "/" .. wholeTime)
		self.CreatingBuildingItemTick = RegisterTick("CreatingBuildingItemTick", CreatingBuildingItemTick, 1000)
	end
end

--创建小图标窗体
function CTongProductionBase:CreateIconWnd(parent)
	local wnd = CTongProductionCreatingIcon:new()
	wnd:CreateFromRes("CommonGrid", parent)
	wnd:ShowWnd( true )
	return wnd
end


--*************************
------list的子窗体类-------
--*************************
function CTongProductionListItemBase:Init(parent)
	self.m_tblInfo		= nil
	self:CreateFromRes("TongBuilldingItemCreateListItem", parent)
	self:SetStoreRoom(5)
	self:ShowWnd( true )
end

function CTongProductionListItemBase:OnChildCreated()
	self.m_CreateBtn	= self:GetDlgChild("CreateBtn")
	self.m_StoreToBtn	= self:GetDlgChild("StoreToBtn")
	self.m_Name			= self:GetDlgChild("Name")
	self.m_Icon			= self:GetDlgChild("Icon")
	
	self.m_tblNeedShow		= {}
	self.m_tblNeedShowLable	= {}
	for i = 1, 4 do
		self.m_tblNeedShow[i]		= self:GetDlgChild("NeedShow" .. i)
		self.m_tblNeedShowLable[i]	= self:GetDlgChild("NeedShowLable" .. i)
	end
end

function CTongProductionListItemBase:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_CreateBtn) then
			self:OnCreate()
		elseif(Child == self.m_StoreToBtn) then
			self:ShowStoreRoomMenu(uParam1, uParam2)
		end
	end
end

function CTongProductionListItemBase:ShowStoreRoomMenu(uParam1, uParam2)
	self.m_Menu = CMenu:new("TargetMenu", g_GameMain)
	for i = 1, 5 do
		local index = tonumber("210" .. i)
		self.m_Menu:InsertItem(GacMenuText(index), self.SetStoreRoom, nil, false, false, nil, self, i)
	end
	self.m_Menu:SetPos(uParam1, uParam2)
end

function CTongProductionListItemBase:SetStoreRoom(storeRoom)
	self.m_StoreRoom = storeRoom
	self.m_StoreToBtn:SetWndText(GacMenuText(tonumber("210" .. storeRoom)))
	if(self.m_Menu) then
		self.m_Menu:DestroyWnd()
		self.m_Menu = nil
	end
end

function CTongProductionListItemBase:SetInfo(info, itemType)
	self.m_itemType	= itemType
	self.m_tblInfo	= info
	local DisplayName	= g_ItemInfoMgr:GetItemLanInfo(itemType, info("Name"),"DisplayName")
	self.m_Name:SetWndText(DisplayName)
	
	local sTime		= g_CTimeTransMgr:FormatTime(info("NeedTime"))
	local sMoney	= g_MoneyMgr:ChangeMoneyToString(info("NeedTongMoney"))
	local tblTemp	= {	{ GetStaticTextClient(10022), info("NeedTime"), sTime },
						{ GetStaticTextClient(10023), info("NeedRes") },
						{ GetStaticTextClient(10024), info("NeedExploit") },
						{ GetStaticTextClient(10025), info("NeedTongMoney"), sMoney }	}
	
	local a = 1
	for i = 1, #tblTemp do
		self.m_tblNeedShow[i]:ShowWnd(false)
		self.m_tblNeedShowLable[i]:ShowWnd(false)
		local bFlag = ( 0 ~= tblTemp[i][2] )
		if( bFlag ) then
			self.m_tblNeedShow[a]:SetWndText( tblTemp[i][3] or tblTemp[i][2] )
			self.m_tblNeedShowLable[a]:SetWndText( tblTemp[i][1] )
			self.m_tblNeedShow[a]:ShowWnd(true)
			self.m_tblNeedShowLable[a]:ShowWnd(true)
			a = a + 1
		end
	end
	
	local smallIcon = self:GetSmallIcon(itemType, info)
	g_LoadIconFromRes(smallIcon, self.m_Icon, -1, IP_ENABLE, IP_ENABLE)
	g_SetItemRichToolTips(self.m_Icon, itemType, info("Name"), 0)
end