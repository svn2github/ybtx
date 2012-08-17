gac_gas_require "item/store_room_cfg/StoreRoomCfg"
gac_require "item/wnd_item_bag/BagConfigInc"
gac_require "information/tooltips/ToolTips"
gac_require "item/wnd_item_bag/GacUseItem"
gac_require "item/wnd_item_bag/LcStoreRoomAppend"
gac_require "item/wnd_item_bag/LcStoreRoom"

CWndItemMainBag	= class( SQRDialog )

function CWndItemMainBag:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	
	if( uMsgID == BUTTON_LCLICK ) then
		if( Child == self.m_btnSplit ) then
			if g_MainPlayer.m_ItemBagLock then
				MsgClient(160011)
				return
			end
			local state = g_WndMouse:GetCursorState()
			if state == ECursorState.eCS_Normal then
				g_WndMouse:SetCursorState(ECursorState.eCS_Split)
			elseif state == ECursorState.eCS_Split then
				g_WndMouse:SetCursorState(ECursorState.eCS_Normal)
			end
		elseif( Child == self.m_BtnCleanBag ) then
			if g_GameMain.m_WndBagSlots.m_lcAddBagSlots:CheckIsItemInUse() == false then
				MsgClient(841)
				return
			end
			local state = g_WndMouse:GetCursorState()
			if state == ECursorState.eCS_BreakItem then
				return
			end
			g_GameMain.bCleanBagFlag = true
			g_WndMouse:ClearCursorAll()
			self:SetMainItemBagState(false)
			if self.m_WndSplit then
				self.m_WndSplit:DestroyWnd()
			end
			Gac2Gas:CleanBag(g_Conn,g_StoreRoomIndex.PlayerBag)--整理包裹
		elseif( Child == self.m_BtnBreakItem ) then
			local arg = g_ParseCommandline()[3]
			if arg and arg.gastype == 1 then
				local str = GetStaticTextClient(1400)
				g_GameMain.m_MsgBox = MessageBox(g_GameMain,str , MB_BtnOK, CallBack, g_GameMain)
				return
			end
			if g_MainPlayer.m_ItemBagLock then
				MsgClient(160010)
				return
			end
			g_WndMouse:ClearCursorAll()
			g_WndMouse:SetBreakItemState()
		elseif( Child == self.m_BtnAgiotage ) then
			g_GameMain.m_Agiotage:OpenWnd()
		elseif( Child == self.m_BtnExit ) then
			if g_WndMouse:GetCursorState() == ECursorState.eCS_BreakItem then
				g_WndMouse:ClearCursorSpecialState()
			end
			self:ShowWnd(not self:IsShow())
		elseif( Child == self.m_ChkBtnLock ) then
			if self.m_ChkBtnLock:GetCheck() then
				self:SetStyle( BitOr( self:GetStyle(), WS_CHILD ) )
			else
				self:SetStyle( BitAnd( self:GetStyle(), 0 ) )
			end
		elseif( Child == self.m_ChkBindingMoney ) then
		    if ( g_GameMain.m_NPCShopSell:IsShow() or g_GameMain.m_NPCShopPlayerSold:IsShow() ) and 
		        g_GameMain.m_NPCShopSell.m_LiuTongShopFlag == 1 or 
		        g_GameMain.m_EquipSuperaddWnd:IsShow() then --是流通商店
		        self.m_ChkMoney:SetCheck(true)
		        MsgClient(6121)
                return		          
		    end
			if g_GameMain.m_PlayersTradeWnd == nil or
				g_GameMain.m_PlayersTradeWnd:IsShow() == false then
				Gac2Gas:SetMoneyType(g_Conn, 2)
			else
				self.m_ChkMoney:SetCheck(true)
			end
		elseif( Child == self.m_ChkMoney ) then
            if ( g_GameMain.m_NPCShopSell:IsShow() or g_GameMain.m_NPCShopPlayerSold:IsShow() ) and 
		        g_GameMain.m_NPCShopSell.m_LiuTongShopFlag == 0 and
		        g_GameMain.m_NPCShopSell.m_CommonShopTipFlag == 1 then --是通用商店
		        local wnd = NpcShopWndMsgSettingLiuTongBtn:new()
		        wnd:ShowWnd(true)
		    else
		        Gac2Gas:SetMoneyType(g_Conn, 1)
		    end
		     
			
		elseif( Child == self.m_UpBtn ) then
			g_GameMain.m_RoleStatus:OnUpEquip(1) --参数1是在包裹中炼化
		elseif Child == self.m_OpenIntensifyBtn then
			if g_MainPlayer.m_ItemBagLock then
				MsgClient(160012)
				return
			end
			g_GameMain.m_EquipUpIntensifyWnd:OpenNoInitWnd()	
		elseif Child == self.m_WithOneShop then
		    g_GameMain.m_NPCShopSell:NPCShopSellWnd()
		elseif Child == self.m_BuyCouponsBtn then
            CBuyCouponsWnd.OpenBuyCouponsWnd()
		end
	elseif( uMsgID == BUTTON_CLICKUP ) then
		if( Child == self.m_UpBtn ) then
			g_GameMain.m_RoleStatus:OnUpEquip(1) --参数1是在包裹中炼化
		end
	end
	
	if( Child == self.m_ctItemRoom) then
		local nRoomIndex = nil
		local pos = uParam1*g_PlayerMainBag.columns + uParam2 +1
		if pos > g_PlayerMainBag.size  then
			nRoomIndex = g_GameMain.m_WndBagSlots.m_lcAddBagSlots:GetSlotIndex(pos)
		else
			nRoomIndex = g_StoreRoomIndex.PlayerBag
		end
		if (uMsgID == ITEM_SHIFTDOWN or
			uMsgID == ITEM_LBUTTONUP or 
			uMsgID == ITEM_LBUTTONCLICK or 
			uMsgID == ITEM_RBUTTONCLICK or 
			uMsgID == BUTTON_DRAG or
			uMsgID == ITEM_CTRLDOWN ) 
			and (not self:GetMainItemBagState()) then
				MsgClient(160001)
				return
		end
		if uMsgID == ITEM_SHIFTDOWN then
			self.m_ctItemRoom:DoShiftDownRoom(nRoomIndex, uParam1, uParam2, self)
		elseif uMsgID == ITEM_LBUTTONUP or uMsgID == ITEM_LBUTTONCLICK then
			self.m_ctItemRoom:DoByClickRoom(nRoomIndex, uParam1, uParam2, self)
		elseif uMsgID == ITEM_RBUTTONCLICK then
			self.m_ctItemRoom:DoByRClick(nRoomIndex, uParam1, uParam2, self)
		elseif uMsgID == BUTTON_DRAG then
			self.m_ctItemRoom:DoByDragRoom(nRoomIndex, uParam1, uParam2, self)
		elseif uMsgID == ITEM_CTRLDOWN then
			self.m_ctItemRoom:DoCtrlDownRoom(nRoomIndex, uParam1, uParam2, self)
		end
	end
end

function CWndItemMainBag:VirtualExcludeWndClosed()
	g_WndMouse:ClearCursorAll()
	self.m_ctItemRoom:ClearPickupItem()
	if self.m_WndSplit then
		self.m_WndSplit:VirtualExcludeWndClosed() 
	end
end

function CWndItemMainBag:InitMainBackPack()
	-- 初始化主包
	local ROOM_INDEX = g_StoreRoomIndex
	local ct = self.m_ctItemRoom
	ct:SetSize(ROOM_INDEX.PlayerBag,g_PlayerMainBag.size,g_PlayerMainBag.columns)

	local SpaceMgr = g_GameMain.m_BagSpaceMgr
	SpaceMgr:SetSpaceRelateLc(ROOM_INDEX.PlayerBag, self.m_ctItemRoom)
end

function CWndItemMainBag:UpdateRoomDis()
	local Mgr = g_GameMain.m_BagSpaceMgr
	local Space = Mgr:GetSpace(g_StoreRoomIndex.PlayerBag)
	--local count = Space:GetItemCount()
	local count = Space:GetNotFreeGridCount()
	local ExpandSpace = Mgr:GetSpace(g_AddRoomIndexClient.PlayerExpandBag)
	assert(ExpandSpace ~= nil)
	for i=1, ExpandSpace:Size() do
		local Grid = ExpandSpace:GetGrid(i)
		if Grid:Size() ~= 0 then
			local BagIndex = Grid.m_Context["RoomIndex"]			
			local expandSpace = Mgr:GetSpace(BagIndex)
			if expandSpace ~= nil then
				--count = expandSpace:GetItemCount() + count
				count = expandSpace:GetNotFreeGridCount() + count
			end
		end
	end

	local totalcount = g_GameMain.m_WndBagSlots.m_lcAddBagSlots:GetAddBackpackRoomSize() + g_PlayerMainBag.size
	self.m_WndRoomSize:SetWndText(string.format("%d/%d", count,totalcount ))
end

function CWndItemMainBag:OnChildCreated()
	self.m_ctItemRoom		= ClassCast(CLcStoreRoom, self:GetDlgChild("WndMasterList"))
	self.m_btnSplit			= self:GetDlgChild("BtnSplit")
	self.m_BtnCleanBag		= self:GetDlgChild("BtnCleanBag")
	self.m_BtnBreakItem		= self:GetDlgChild("BtnBreakItem")
	self.m_BtnAgiotage		= self:GetDlgChild("BtnAgiotage")
	self.m_BtnExit			= self:GetDlgChild("BtnExit")
	self.m_ChkBtnLock		= self:GetDlgChild("ChkBtnLock")
	self.m_WndGold			= self:GetDlgChild("WndGold")
	self.m_WndArgent		= self:GetDlgChild("WndArgent")
	self.m_WndCopper		= self:GetDlgChild("WndCopper")
	self.m_WndBindingCopper	= self:GetDlgChild("WndBindingCopper")
	self.m_WndBindingArgent	= self:GetDlgChild("WndBindingArgent")
	self.m_WndBindingGold	= self:GetDlgChild("WndBindingGold")
	self.m_WndRoomSize		= self:GetDlgChild("WndNumber")
	self.m_ChkBindingMoney	= self:GetDlgChild("CBtnBindingMoney")
	self.m_ChkMoney			= self:GetDlgChild("CBtnMoney")
	self.m_UpBtn			= self:GetDlgChild("UpBtn")
	self.m_OpenIntensifyBtn	= self:GetDlgChild("OpenIntensifyBtn")
	self.m_SoulValue		= self:GetDlgChild("SoulValue")
	self.m_UpIconWnd		= self:CreateUpIconWnd(self.m_UpBtn)
	self.m_PosStatic 		= self:GetDlgChild("PosStatic")
	
	self.m_UpBtn:ShowWnd(false)
	self.m_ChkMoney:SetCheck(true)
	self.m_ctItemRoom:SetDetectMouseOverWnd(true)
	
	g_WndMouse:ClearIconWnd(self.m_UpIconWnd)
	g_GameMain.m_WndBagSlots = CWndBagSlots:new(self.m_PosStatic)
	self:InitMainBackPack()
	self.m_IsBagCanUse = true
	self.m_WithOneShop = self:GetDlgChild("WithOneShop")
	self.m_BuyCouponsBtn = self:GetDlgChild("BuyCouponsBtn")
end

--包裹是否处于可用状态
function CWndItemMainBag:GetMainItemBagState()
	return self.m_IsBagCanUse
end

--设置包裹状态
function CWndItemMainBag:SetMainItemBagState(state)
	self.m_IsBagCanUse = state
end

CUpIcon	= class( SQRDialog )
--创建强化框小图标窗体
function CWndItemMainBag:CreateUpIconWnd(parent)
	local wnd = CUpIcon:new()
	wnd:CreateFromRes("CommonGrid", parent)
	wnd:ShowWnd( true )
	return wnd
end

function CWndItemMainBag:UpDateSoulValue(total_soul)
	self.m_SoulValue:SetWndText(GetStaticTextClient(5001) .. ":" .. total_soul)
end

function CWndItemMainBag:InitMoneyInMainBag()
	--流通金币
	self.m_WndGold:SetWndText(tostring(g_MainPlayer:GetGold())) 
	self.m_WndArgent:SetWndText(tostring(g_MainPlayer:GetArgent())) 
	self.m_WndCopper:SetWndText(tostring(g_MainPlayer:GetCopper())) 
	--绑定金币
	self.m_WndBindingGold:SetWndText(tostring(g_MainPlayer:GetBindingGold())) 
	self.m_WndBindingArgent:SetWndText(tostring(g_MainPlayer:GetBindingArgent())) 
	self.m_WndBindingCopper:SetWndText(tostring(g_MainPlayer:GetBindingCopper())) 
end

function CWndItemMainBag:UpdateItemLeftTimeInBagTick()
	local bagWnd		= g_GameMain.m_WndMainBag
	local roleStatusWnd	= g_GameMain.m_RoleStatus
	local tbl = g_DynItemInfoMgr.m_tblGlobalItemInfo
	for i, v in pairs(tbl) do
		local itemID = i
		if v.m_nLifeTime and v.m_nLifeTime > 0 then
			v.m_nLifeTime = v.m_nLifeTime - 1		--1秒刷新一次
			if v.m_nLifeTime <= 0 then
				v.m_nLifeTime = 0
				Gac2Gas:DelWithTimeOutItem(g_Conn,itemID,v.m_nBigID,v.m_nIndex)
				bagWnd.m_ctItemRoom:UpdateWndDisPlayByItemID(true, itemID)
				roleStatusWnd:UpdateLifeEquipByItemID(itemID)
			end
			if( bagWnd:IsShow() ) then
				bagWnd.m_ctItemRoom:UpdateBagTips(true, itemID)
			end
			if( roleStatusWnd:IsShow() ) then
				roleStatusWnd:UpdateLifeEquipTipsByItemID(itemID)
			end
		end
	end
end

--初始化包裹、附背包栏、仓库等容器
function CWndItemMainBag:InitBagRoom( Parent )
	-- 主角的背包空间初始化
	local ROOM_INDEX = g_StoreRoomIndex
	local size,columns = g_PlayerMainBag.size,g_PlayerMainBag.columns

	Parent.m_BagSpaceMgr = CBagSpaceMgr.Init()
	-- 增加主空间，增加主背包的附属背包栏。每个角色都具有
	Parent.m_BagSpaceMgr:AddSpace(ROOM_INDEX.PlayerBag,size,columns)
	size,columns = g_MainBagExpand.size,g_MainBagExpand.columns
	Parent.m_BagSpaceMgr:AddSpace(g_AddRoomIndexClient.PlayerExpandBag,size,columns)
	
	--添加仓库的背包和附属背包栏
	size,columns = g_PlayerDepotMain.size,g_PlayerDepotMain.columns
	Parent.m_BagSpaceMgr:AddSpace(ROOM_INDEX.PlayerDepot,size,columns)
	size,columns = g_MainDepotExpand.size,g_MainDepotExpand.columns
	Parent.m_BagSpaceMgr:AddSpace(g_AddRoomIndexClient.PlayerExpandDepot,size,columns)

	--添加帮会仓库
	size,columns = g_TongDepotMain.size,g_TongDepotMain.columns
	Parent.m_BagSpaceMgr:AddSpace(ROOM_INDEX.TongDepot,size,columns)
	self.m_UpdateItemLeftTimeInBagTick = RegClassTick("UpdateItemLeftTimeInBagTick", self.UpdateItemLeftTimeInBagTick, 1000, self)
end

--初始化包裹格子
function CWndItemMainBag:InitBagGrids()
	if g_GameMain.m_WndMainBag == nil then
		g_GameMain.m_WndMainBag = CWndItemMainBag:new()
		g_GameMain.m_WndMainBag:CreateFromRes("BagMasterOpened",g_GameMain)
		g_GameMain.m_WndMainBag.m_WndFarther = g_GameMain.m_MainSkillsToolBar.m_WndItemBag
		g_ExcludeWndMgr:InitExcludeWnd(g_GameMain.m_WndMainBag, "非互斥")
	end
end

function CWndItemMainBag:SetWndScale(addition)
	local rect=CFRect:new()
	self:GetLogicRect(rect)
	rect.bottom = rect.bottom  + addition
	self:SetLogicRect(rect)
end

function CWndItemMainBag:SetListScale(size, col)
	local row = math.ceil(size/col)
	local rect=CFRect:new()
	self.m_ctItemRoom:GetLogicRect(rect)
	
	local tmpBottom = rect.bottom
	rect.bottom = rect.top + (row*g_GridSize)
	self.m_ctItemRoom:SetLogicRect(rect)
	
	local addition = rect.bottom - tmpBottom
	self:SetWndScale(addition)
end

---------------------------------------------------------------------------------------
