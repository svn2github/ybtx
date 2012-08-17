gac_require "item/depot/DepotMoneyInput"
gac_require "item/wnd_item_bag/LcStoreRoom"
CDepot = class( SQRDialog )

function CDepot:Ctor()
	self:CreateFromRes("Depot", g_GameMain)
	self:InitDepot()
	self.m_Money = 0
	g_ExcludeWndMgr:InitExcludeWnd(self, "非互斥")
end

function CDepot:OnChildCreated()
	self.m_XBtn				= self:GetDlgChild("XBtn")
	self.m_ctItemRoom		= ClassCast(CLcStoreRoom, self:GetDlgChild("ItemRoom"))
	self.m_Gold				= self:GetDlgChild("Gold")
	self.m_Silver			= self:GetDlgChild("Silver")
	self.m_Copper			= self:GetDlgChild("Copper")
	self.m_SaveMoney		= self:GetDlgChild("SaveMoney")
	self.m_GetMoney			= self:GetDlgChild("GetMoney")
	self.m_CleanDepot		= self:GetDlgChild("CleanDepot")
	self.m_DepotMoneyInput	= CreateDepotMoneyInput(self)
end

function CDepot:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == BUTTON_LCLICK)  then
		if(Child == self.m_XBtn)  then
			g_WndMouse:ClearCursorAll()
			if self.m_WndSplit then
				self.m_WndSplit:VirtualExcludeWndClosed() 
			end
			self:ShowWnd(false)
		elseif(Child == self.m_SaveMoney) then
			if g_MainPlayer.m_ItemBagLock then
				MsgClient(160023)
				return 
			end
			self.m_DepotMoneyInput:OpenPanel(1)
		elseif(Child == self.m_GetMoney) then
			if g_MainPlayer.m_ItemBagLock then
				MsgClient(160024)
				return 
			end
			self.m_DepotMoneyInput:OpenPanel(2)
		elseif(Child == self.m_CleanDepot) then
			local state = g_WndMouse:GetCursorState()
			if state == ECursorState.eCS_BreakItem then
				return
			end
			g_WndMouse:ClearCursorAll()
			if g_GameMain.m_WndMainBag.m_WndSplit then
				g_GameMain.m_WndMainBag.m_WndSplit:DestroyWnd()
			end
			Gac2Gas:CleanBag(g_Conn,g_StoreRoomIndex.PlayerDepot)
		end
	elseif(uMsgID == ITEM_LBUTTONUP or uMsgID == ITEM_LBUTTONCLICK) then
		--if self.m_lcAddBagSlots == Child then
			--self.m_lcAddBagSlots:ClickAddBackpack(uParam1,uParam2)
		if(Child == self.m_ctItemRoom) then
			self.m_ctItemRoom:DoByClickRoom(g_StoreRoomIndex.PlayerDepot, uParam1, uParam2, self)
		end
	elseif(uMsgID == ITEM_RBUTTONCLICK) then
		if(Child == self.m_ctItemRoom) then
			self.m_ctItemRoom:DoByRClick(g_StoreRoomIndex.PlayerDepot, uParam1, uParam2, self)
		end
	elseif(uMsgID == BUTTON_DRAG) then
		if(Child == self.m_ctItemRoom) then
			self.m_ctItemRoom:DoByDragRoom(g_StoreRoomIndex.PlayerDepot, uParam1, uParam2, self)
		end
	end
end

function CDepot:VirtualExcludeWndClosed()
	if self.m_WndSplit then
		self.m_WndSplit:VirtualExcludeWndClosed() 
	end
	self.m_DepotMoneyInput:ShowWnd(false)
end

--初始化仓库物品格
function CDepot:InitDepot()
	local columns, size = g_PlayerDepotMain.columns, g_PlayerDepotMain.size
	local SpaceMgr = g_GameMain.m_BagSpaceMgr
	self.m_ctItemRoom:SetSize(g_StoreRoomIndex.PlayerDepot, size, columns)
	SpaceMgr:SetSpaceRelateLc(g_StoreRoomIndex.PlayerDepot, self.m_ctItemRoom)
end

function CDepot:OpenPanel()
	Gac2Gas:GetDepotInfo(g_Conn)
end

function CDepot:ShowMoney(money)
	self.m_Money	= money
	local gold		= math.floor( money/10000 )
	local silver	= math.floor( (money - gold*10000)/100 )
	local copper	= money - gold*10000 - silver*100
	self.m_Gold:SetWndText(gold)
	self.m_Silver:SetWndText(silver)
	self.m_Copper:SetWndText(copper)
end

function Gas2Gac:ReturnGetDepotInfo(Conn, money)
	local wnd = g_GameMain.m_Depot
	wnd:ShowWnd(true)
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(wnd)
	wnd.m_ctItemRoom:UpdateRoomTipsAll(g_StoreRoomIndex.PlayerDepot)
	wnd:ShowMoney(money)
end

-----------------------------------------------------------------------------------------------------
function g_Rpc_OnOpenDepotSlot(nRet)
	g_GameMain.m_WndDepot.m_BtnStartup:EnableWnd(true)
	if nRet == 1 then
		g_GameMain.m_WndDepot.m_MsgBox = MessageBox(g_GameMain.m_WndDepot,MsgBoxMsg(17), MB_BtnOK)
	else
		g_GameMain.m_WndDepot.m_MsgBox = MessageBox(g_GameMain.m_WndDepot,MsgBoxMsg(18), MB_BtnOK)
	end
end

