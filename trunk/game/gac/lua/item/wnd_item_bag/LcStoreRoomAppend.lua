gac_require "item/wnd_item_bag/LcStoreRoom"

LcStoreRoomAppend = class(CLcStoreRoom)

--改变主背包里的空间，只要背包栏里的背包变了，无论是位置还是背包的数目，都会影响物品窗口
function AddGridsToMainBag(Wnd, SlotRoomIndex)
	--此值是用来记录每当添加一个包裹到背包栏时，主背包中开始怎天放置物品的位置
	g_GameMain.BeginPos = g_PlayerMainBag.size
	g_GameMain.m_WndMainBag.m_ctItemRoom.LCStorm = {}
	local ExpandSpace = g_GameMain.m_BagSpaceMgr:GetSpace(SlotRoomIndex)
	assert(ExpandSpace ~= nil)
	--原先：为每个包裹对应的设置一个物品窗口，将物品添加到其对应的物品窗口中
	for i=1, ExpandSpace:Size() do
		local Grid = ExpandSpace:GetGrid(i)
		if Grid:Size() ~= 0 then
			local BagIndex = Grid.m_Context["RoomIndex"]
			local BagSize = Grid.m_Context["Size"]
			local BagPos = Grid.m_Context["BagPos"]
			Wnd:PlaceBag(BagIndex,BagPos,BagSize)
			g_GameMain.BeginPos = g_GameMain.BeginPos + BagSize
		end
	end
	g_GameMain.m_WndBagSlots.m_lcAddBagSlots:UpdateBagSlotsPos()
	Wnd:PlaceBagEnd()
end

--包裹从包裹栏中取出后，主背包里的格子被删除
function DelMainBagGrid(Wnd, PosBegin, col, MainBagSize)

	local function  ParsePos(Pos,cols)
		Pos = Pos - 1
		local x = math.floor(Pos / cols)
		local y = Pos - x*cols
		return x,y
	end

	local Size = MainBagSize
	for i=PosBegin+1, Size do
		local x, y = ParsePos(i,col) 
		g_GameMain.m_WndMainBag.m_ctItemRoom:GetSubItem(x, y):ShowWnd(false)
		g_GameMain.m_WndMainBag.m_ctItemRoom:GetSubItem(x, y):EnableWnd(false)
		g_GameMain.m_WndMainBag.m_ctItemRoom._m_CDialog[i]:ShowWnd(false)
		
		--删除物品图片
		local Wnd = g_GameMain.m_WndMainBag.m_ctItemRoom:GetWnd(x,y)
		local wnd = Wnd:GetDlgChild( "BtnCD" ) 
		g_WndMouse:ClearIconWnd(wnd)
		local vWnd = g_GameMain.m_WndMainBag.m_ctItemRoom:GetSubItem( x, y )
		vWnd:SetMouseOverDescAfter("")
	end
	
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	local tbl = {IP_ENABLE, IP_DISABLE}
	for j = 1, #tbl do
		Flag.StateMask = tbl[j]
		for i=PosBegin+1, Size do
			-- 删除背景层
			local DescImage = g_GameMain.m_WndMainBag.m_ctItemRoom.LCStorm[i-g_PlayerMainBag.size]:GetWndBkImage(Flag)
			DescImage:DeleteImage(PosBegin+1-g_PlayerMainBag.size)
		end
	end

	local SpaceMgr = g_GameMain.m_BagSpaceMgr
	local Space = SpaceMgr:GetSpace(g_StoreRoomIndex.PlayerBag)
	Space:ModifySpaceSize(g_PlayerMainBag.size)
end

--附加背包巢的一些管理
--参数分别为：-1（附加背包槽），4（附加背包槽大小），6列，slotBeginPos（1）
function LcStoreRoomAppend:SetSize(RoomIndex, size, col, PosBegin)
	assert(RoomIndex and size and col and PosBegin)
	self.m_nPosBegin = PosBegin
	self.m_nRoomIndex = RoomIndex
	self.m_nCol	= col
	local row = math.ceil(size/col)
	local sub = col*row - size

	for i=1, col do
		self:InsertColumn(i-1,52)
	end

	for i=1, row do
		self:InsertItem(i-1, 52)
	end

	local ExpandSpace = g_GameMain.m_BagSpaceMgr:GetSpace(RoomIndex)
	assert(ExpandSpace ~= nil)
	
	self.mBagSlots = {}
	self.m_ayBagButton = {}

	for i=1, size do
		local item = SQRDialog:new()
		item:CreateFromResEx("Bag_ChkBtn_04", self:GetSubItem(row-1,i-1), true, true)
		item:ShowWnd( true )
		item:SetStyle(0x60000000)
		table.insert(self.mBagSlots,item)
		self.m_ayBagButton[i] =  self:GetSubItem(row-1,i-1)
		self.m_ayBagButton[i].m_ParentWnd = self
		self.m_ayBagButton[i].m_RoomIndex = RoomIndex
		self.m_ayBagButton[i].m_Row = row - 1
		self.m_ayBagButton[i].m_Col = i - 1
		self.m_ayBagButton[i]:SetDetectMouseOverWnd(true)
	end

	--原先：为每个包裹对应的设置一个物品窗口，将物品添加到其对应的物品窗口中
	for i=1, ExpandSpace:Size() do
		local bagslot = self:GetSubItem(0,i-1)
		local Grid = ExpandSpace:GetGrid(i)
		if Grid:Size() ~= 0 then
			bagslot:EnableWnd(true)
			local BagIndex = Grid.m_Context["RoomIndex"]
			local BagSize = Grid.m_Context["Size"]
			local BagPos = Grid.m_Context["BagPos"]
			self:UpdateSlot(BagPos, BagSize)
		end	 
	end
end

function LcStoreRoomAppend:GetSlots()
	return self.m_BagAddBackpacks
end

--加载包裹栏的资源
function LcStoreRoomAppend:UpdateSlot(SlotPos, BagSize)
	local Mgr = g_GameMain.m_BagSpaceMgr
	local Grid = Mgr:GetGridByRoomPos(g_AddRoomIndexClient.PlayerExpandBag, SlotPos)
	if self.m_Bags[SlotPos] == nil and BagSize ~= 0 then
		self:GetSubItem(0, SlotPos-1):EnableWnd(true)
	end
	local SmallIcon = Grid:GetIcon()
	local itemType, itemName = Grid:GetType()
	local subItem = self:GetSubItem(0, SlotPos-1)
	local WndEx = subItem:GetDlgChild( "Bag_ChkBtn_04" ):GetDlgChild("BtnCD")
	g_LoadIconFromRes(SmallIcon, WndEx, -2, IP_ENABLE, IP_CLICKDOWN)
	g_SetItemRichToolTips(subItem,itemType, itemName, 0)
end

function LcStoreRoomAppend:TakeDownBag(SlotPos)
	local subItem = self:GetSubItem(0, SlotPos-1)
	local wnd = subItem:GetDlgChild( "Bag_ChkBtn_04" ):GetDlgChild("BtnCD")
	g_WndMouse:ClearIconWnd(wnd)
	subItem:SetMouseOverDescAfter("")
	self:UpdateBagSlotsPos()
end

function LcStoreRoomAppend:AdditionalBagClose(Pos)
	assert(1<=Pos and Pos<=4)
end

--将包裹从物品栏拖动到包裹栏中的对应事件
function LcStoreRoomAppend:PlaceBag(RoomIndex, SlotPos, RoomSize)
	local Pos = SlotPos
	assert(RoomIndex and SlotPos and RoomSize)

	local Backpacks = self.m_BagAddBackpacks
	self:UpdateSlot(SlotPos, RoomSize)
	self:AfterPlaceBag(RoomIndex, RoomSize, Pos)
end

function LcStoreRoomAppend:AfterPlaceBag(RoomIndex, Size, Pos)
	assert(RoomIndex and Size and Pos)
	
	local col = g_PlayerMainBag.columns
	local Backpacks = self.m_BagAddBackpacks
	if(not Backpacks[Pos]) then
		Backpacks[Pos] = {}
	end
	
	
	Backpacks[Pos].m_nCol				= col
	Backpacks[Pos].m_nPos				= Pos
	Backpacks[Pos].m_StoreRoomIndex		= RoomIndex
	Backpacks[Pos].m_StoreRoomSize		= Size
	Backpacks[Pos].m_WndAddSlot			= self
	Backpacks[Pos].m_nPosBegin			= g_GameMain.BeginPos
	
	local SpaceMgr = g_GameMain.m_BagSpaceMgr
	local Space = SpaceMgr:GetSpace(RoomIndex)
	if Space == nil then
		SpaceMgr:AddSpace(RoomIndex, Size)
	else
		Space:ModifySpaceSize(Size)
	end
	local ctItemRoom = g_GameMain.m_WndMainBag.m_ctItemRoom
	SpaceMgr:SetSpaceRelateLc(RoomIndex, ctItemRoom)
	--当添加新的包裹后，主背包的大小也做相应的改变
	local MainBagSize = Size + g_GameMain.BeginPos
	local mainBagpace = SpaceMgr:GetSpace(g_StoreRoomIndex.PlayerBag)
	mainBagpace:ModifySpaceSize(MainBagSize)

	local size = g_GameMain.BeginPos + Size
	for i=g_GameMain.BeginPos + 1, size do
		local x, y = ctItemRoom:ParsePos(i,col)
		local wnd = ctItemRoom:GetSubItem(y,x)
		local ItemWnd = ctItemRoom._m_CDialog[i]
		table.insert(ctItemRoom.LCStorm,ItemWnd)
		wnd:EnableWnd(true)
		wnd:ShowWnd(true)
		ItemWnd:ShowWnd(true)
	end
	ctItemRoom:SetBackgroundAndItemPic(RoomIndex, g_GameMain.BeginPos + 1, size, col)
end

function LcStoreRoomAppend:GetGlobalSlotPos(Pos)
	assert(Pos)
	return self.m_nPosBegin + Pos - 1
end

function LcStoreRoomAppend:GetLocalSlotPos(Pos)
	assert(Pos - self.m_nPosBegin >= 0)
	return Pos - self.m_nPosBegin + 1
end

--向包裹栏放包裹事件
function LcStoreRoomAppend:ClickAddBackpack(row, col)
	local Pos				= col + 1
	local state, context	= g_WndMouse:GetCursorState()
	local SpaceMgr			= g_GameMain.m_BagSpaceMgr
	local Backpacks			= self.m_BagAddBackpacks
	
	if state == ECursorState.eCS_PickupItem then --手上有物品
		local srcRoom, srcRow, srcCol, Count = context[1], context[2], context[3], context[4]

		if srcRoom == self.m_nRoomIndex and srcRow == row and srcCol == col then
			g_WndMouse:ClearCursorAll()
			return
		end

		local SrcCt = SpaceMgr:GetSpaceRelateLc(srcRoom)
		local srcPos = SrcCt:GetPosByIndex(srcRow, srcCol, srcRoom)
		local Grid = SrcCt:GetGridByIndex(srcRow, srcCol, srcRoom)
		local Type, index = Grid:GetType()

		if g_ItemInfoMgr:IsPlayerBag(Type) then	--手上的物品是普通包
			if not self:CheckIsItemInUse() then
				MsgClient(12)
				return 
			end
			local addBagRoomSize		= g_ItemInfoMgr:GetItemInfo(tonumber(Type), index,"BagRoom")
			local bagSpace				= SpaceMgr:GetSpace(g_StoreRoomIndex.PlayerBag)
			local ndescPos				= self:GetGlobalSlotPos(Pos)
			local ExpandSpace			= g_GameMain.m_BagSpaceMgr:GetSpace(g_AddRoomIndexClient.PlayerExpandBag)
			local Grid					= ExpandSpace:GetGrid(ndescPos)
			local curReplacedBagSize	= 0
			if Grid:GetItem() ~= nil then   --要放置包裹的地方有其他背包
				curReplacedBagSize = Grid.m_Context["Size"]
			end
			if addBagRoomSize + (bagSpace.m_nSize - curReplacedBagSize) > g_PlayerMainBag.MaxSize then
				MessageBox(self, MsgBoxMsg(13001), MB_OKBtn)
				g_WndMouse:ClearCursorAll()
				return
			end
			if g_Client_IsSlotIndex(srcRoom) and g_Client_IsSlotIndex(self.m_nRoomIndex) then
				local nSrcPos = SrcCt:GetGlobalSlotPos(srcPos)
				Gac2Gas:Change2Bag(g_Conn,nSrcPos,ndescPos)
				g_GameMain.m_WndMainBag:SetMainItemBagState(false)
			elseif (SrcCt == g_GameMain.m_WndMainBag.m_ctItemRoom  )and self.m_nRoomIndex == g_AddRoomIndexClient.PlayerExpandBag then
				--如果将物品栏里的包裹和包裹栏里的包裹互换时，如果包裹栏里的包裹放置有物品，则不做任何响应
				if Backpacks[Pos] ~= nil then
					if SpaceMgr:GetSpace(Backpacks[Pos].m_StoreRoomIndex):GetItemCount() ~= 0 then
						return
					end
				end 
				if g_MainPlayer.m_ItemBagLock then
					MsgClient(160009)
					return
				end
				Gac2Gas:PlaceBag(g_Conn,srcRoom,srcPos,ndescPos)
				g_GameMain.m_WndMainBag:SetMainItemBagState(false)
			end
		end
		g_WndMouse:ClearCursorAll()
	end
end

--判断包裹中是否有正在使用的物品（由于使用，物品所在格子置为不可点状态）
--如果有，则包裹不可被移动
function LcStoreRoomAppend:CheckIsItemInUse()
	local SpaceMgr = g_GameMain.m_BagSpaceMgr
	local space = SpaceMgr:GetSpace(g_StoreRoomIndex.PlayerBag)
	local BagSize = space:Size()
	local ctItemRoom = g_GameMain.m_WndMainBag.m_ctItemRoom
	for i=1, BagSize do
		local col, row = ctItemRoom:ParsePos(i, g_PlayerMainBag.columns)
		local itemWnd = ctItemRoom:GetSubItem(row, col)
		if itemWnd:IsEnable() == false then
			return false
		end
	end
	return true
end

--拖动包裹栏事件
function LcStoreRoomAppend:DoByDragRoom(row, col)
	local Pos = col + 1
	local Item = self.m_ayBagButton[Pos]
	local state, context = g_WndMouse:GetCursorState()
	local SpaceMgr = g_GameMain.m_BagSpaceMgr
	if self:CheckIsItemInUse() == false then
		return
	end
	if state == ECursorState.eCS_Normal then
		local Grid = SpaceMgr:GetGridByRoomPos(self.m_nRoomIndex, Pos)
		if not Grid:Empty() then
			local goods  = self:GetSubItem(0, col):GetDlgChild("Bag_ChkBtn_04"):GetDlgChild("BtnCD") 
			g_WndMouse:SetPickupItem(Item.m_RoomIndex, Item.m_Row, Item.m_Col, num, goods, State)
		end	
	end
end

function LcStoreRoomAppend:PlaceBagEnd()
	self:ResetItem()
end

function LcStoreRoomAppend:GetAddBackpackRoomSize()
	local totalSize = 0
	for i=1, 4 do
		local bp = self.m_BagAddBackpacks[i]
		if bp ~= nil then
			totalSize = totalSize + bp.m_StoreRoomSize
		end
	end
	return totalSize
end


--此函数的参数是物品在主背包中的要放置的目的地位置，index从1开始
--用以计算放置到那个附背包中，在附背包中的具体位置，index从1开始
--返回附背包的对应索引和物品放置的附背包中的具体位置
function LcStoreRoomAppend:GetSlotIndex(CurPos)

	local Backpacks = self.m_BagAddBackpacks

	local ExpandSpace = g_GameMain.m_BagSpaceMgr:GetSpace(g_AddRoomIndexClient.PlayerExpandBag)
	assert(ExpandSpace ~= nil)
	local haveBagSlot = {}
	local j = 0
	for i=1, ExpandSpace:Size() do
		local Grid = ExpandSpace:GetGrid(i)
		if Grid:Size() ~= 0 then
			local BagIndex = Grid.m_Context["RoomIndex"]
			local BagSize = Grid.m_Context["Size"]
			j = j + 1
			haveBagSlot[j] = Backpacks[i]
		end
	end
	
	for k = 1, j do
		local maxPos = g_PlayerMainBag.size
		for l = k, 1, -1 do
			maxPos = maxPos + haveBagSlot[l].m_StoreRoomSize
		end
		if(CurPos <= maxPos) then
			local desPos = CurPos - maxPos + haveBagSlot[k].m_StoreRoomSize
			return haveBagSlot[k].m_StoreRoomIndex, desPos
		end
	end
end 

--更新包裹栏包裹的的信息 如：1\4
function LcStoreRoomAppend:UpdateBagSlotsPos()
	local Backpacks = self.m_BagAddBackpacks
	local Mgr = g_GameMain.m_BagSpaceMgr
	local ExpandSpace = Mgr:GetSpace(g_AddRoomIndexClient.PlayerExpandBag)
	assert(ExpandSpace)
	for i=1, ExpandSpace:Size() do
		local Grid = ExpandSpace:GetGrid(i)
		if Grid:Size() ~= 0 then
			local BagIndex		= Grid.m_Context["RoomIndex"]
			local BagSize		= Grid.m_Context["Size"]
			local expandSpace	= Mgr:GetSpace(BagIndex)
			local count			= expandSpace:GetNotFreeGridCount()
			local slot			= self:GetSubItem(0,i-1):GetDlgChild("Bag_ChkBtn_04")
			slot:GetDlgChild("WndNum"):SetWndText(string.format("%d/%d", count, BagSize))
		else
			local slot = self:GetSubItem(0,i-1):GetDlgChild("Bag_ChkBtn_04")
			slot:GetDlgChild("WndNum"):SetWndText("")
		end
	end
end