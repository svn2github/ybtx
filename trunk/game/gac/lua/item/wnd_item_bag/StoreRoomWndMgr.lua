
CStoreRoomWndMgr = class()

function CStoreRoomWndMgr.GetMgr()
	g_GacStoreRoomMgr = g_GacStoreRoomMgr or CStoreRoomWndMgr:new()
	return g_GacStoreRoomMgr
end

--******************************************************************************
--拆分（未调用）
function CStoreRoomWndMgr:Rpc_OnSplitItemResult(srcRoom, srcPos, nItemID)
	g_GameMain.m_BagSpaceMgr:RetAddItemToGrid(nItemID)
	g_GameMain.m_BagSpaceMgr:DelItemByRoomPos(srcRoom, srcPos, nItemID)
end

--拆分结束
function CStoreRoomWndMgr:Rpc_OnSplitItemResultEnd(srcRoom, srcPos, descRoom, descPos)
	g_GameMain.m_BagSpaceMgr:UpdateRoomPos(srcRoom, srcPos)
	g_GameMain.m_BagSpaceMgr:UpdateRoomPos(descRoom, descPos)
	self:SetClickedWndState(srcRoom, srcPos, descRoom, descPos, true)
	g_GameMain.m_WndBagSlots.m_lcAddBagSlots:UpdateBagSlotsPos()
	
	local SrcGrid = g_GameMain.m_BagSpaceMgr:GetGridByRoomPos(srcRoom, srcPos)
	local _, sName = SrcGrid:GetType()
	if g_GameMain.m_MainSkillsToolBar and sName then
		g_GameMain.m_MainSkillsToolBar:SetShortCutItemNum(sName)
		g_GameMain.m_MainSkillsToolBar:DelShortCutItem(sName)
	end
end
--******************************************************************************

function CStoreRoomWndMgr:Rpc_OnReplaceItemResult(srcRoom,srcPos,descRoom,descPos)
	g_GameMain.m_BagSpaceMgr:ExChangeTwoGrid(srcRoom, srcPos, descRoom, descPos)
	g_GameMain.m_BagSpaceMgr:UpdateRoomPos(srcRoom,srcPos)
	g_GameMain.m_BagSpaceMgr:UpdateRoomPos(descRoom,descPos)
	self:SetClickedWndState(srcRoom, srcPos, descRoom, descPos, true)
	g_GameMain.m_WndBagSlots.m_lcAddBagSlots:UpdateBagSlotsPos()
end

function CStoreRoomWndMgr:Rpc_OnMoveItemResult(srcRoom,srcPos,nItemID,descRoom,descPos)
	self:Rpc_OnSplitItemResult(srcRoom,srcPos,nItemID,descRoom,descPos)
end

function CStoreRoomWndMgr:Rpc_OnMoveItemResultEnd(srcRoom,srcPos,descRoom,descPos)
	self:Rpc_OnSplitItemResultEnd(srcRoom,srcPos,descRoom,descPos)
	g_GameMain.m_WndBagSlots.m_lcAddBagSlots:UpdateBagSlotsPos()
end

--srcRoom:容器的下标索引（主背包、附背包还是其他）；srcPos：在这个容器中的第几个位置（从1开始）
--参数nSlotPos放在了背包栏的第几个位置：物理顺序
--nUseRoomIndex添加到背包栏的第几个，逻辑顺序（按添加的先后顺序)，起始为50
function CStoreRoomWndMgr:Rpc_OnPlaceBagSuccess(srcRoom, srcPos, nSlotPos, nUseRoomIndex)
	local SpaceMgr	= g_GameMain.m_BagSpaceMgr
	local nSlotRoom	= g_Client_GetSlotRoomIndexBySlot(nSlotPos)
	local SrcCt		= SpaceMgr:GetSpaceRelateLc(srcRoom)
	local DescCt	= SpaceMgr:GetSpaceRelateLc(nSlotRoom)
	local srcGrid	= SpaceMgr:GetGridByRoomPos(srcRoom,srcPos)
	local descPos	= DescCt:GetLocalSlotPos(nSlotPos)--在所在容器中的第几个位置（因为容器的开始位置不一定为1；仓库的起始位置为21）
	local descGrid	= SpaceMgr:GetGridByRoomPos(nSlotRoom,descPos)
	
	local srcType, srcIndex = srcGrid:GetType()
	
	local bFlag			= descGrid:Empty()
	local srcRoomSize	= g_ItemInfoMgr:GetItemInfo(srcType, srcIndex, "BagRoom")
	local BagInfo		= {["Size"] = srcRoomSize, ["BagPos"] = descPos}
	
	if(bFlag) then
		BagInfo["RoomIndex"] = nUseRoomIndex
		srcGrid:SetContext(BagInfo)
		SpaceMgr:PlaceBag(srcRoom, srcPos, nSlotRoom, descPos)
	else
		BagInfo["RoomIndex"] = descGrid.m_Context["RoomIndex"]
		srcGrid:SetContext(BagInfo)
		SpaceMgr:FetchAndPlaceBag(srcRoom, srcPos, nSlotRoom, descPos)
		DescCt:TakeDownBag(descPos)
		DescCt:UpdateSlot(descPos, srcRoomSize)
	end
	
	local Space = SpaceMgr:GetSpace(g_StoreRoomIndex.PlayerBag)
	local MainBagSize = Space:Size()
	DelMainBagGrid(g_GameMain.m_WndMainBag.m_ctItemRoom, g_PlayerMainBag.size, g_PlayerMainBag.columns, MainBagSize)
	AddGridsToMainBag(g_GameMain.m_WndBagSlots.m_lcAddBagSlots, g_AddRoomIndexClient.PlayerExpandBag)
	SrcCt:UpdateWndDisPlayByPos(srcPos, srcRoom)
	MainBagSize = Space:Size()
	g_GameMain.m_WndMainBag:SetListScale(MainBagSize, g_PlayerMainBag.columns)
	g_GameMain.m_WndMainBag:SetMainItemBagState(true)
	
	--更新快捷栏上该物品的数量
	if(g_GameMain.m_MainSkillsToolBar and not g_GameMain.bCleanBagFlag) then
		g_GameMain.m_MainSkillsToolBar:SetShortCutItemNum(srcIndex)
		g_GameMain.m_MainSkillsToolBar:DelShortCutItem(srcIndex)
		if(not bFlag) then
			local descItem	= descGrid:GetItem()
			local descIndex	= descItem.m_nIndex
			g_GameMain.m_MainSkillsToolBar:SetShortCutItemNum(descIndex)
		end
	end
end

function CStoreRoomWndMgr:Rpc_OnFetchBagSuccess(srcSlot, descRoom, descPos, nGridID)
	local SpaceMgr		= g_GameMain.m_BagSpaceMgr
	local srcSlotRoom	= g_Client_GetSlotRoomIndexBySlot(srcSlot)
	local SrcCt			= SpaceMgr:GetSpaceRelateLc(srcSlotRoom)
	local DescCt		= SpaceMgr:GetSpaceRelateLc(descRoom)
	local srcLocalPos	= SrcCt:GetLocalSlotPos(srcSlot)
	
	SpaceMgr:FetchBag(srcSlotRoom, srcLocalPos, descRoom, descPos, nGridID)
	SrcCt:TakeDownBag(srcLocalPos)
	
	local tbl_bag = g_GameMain.m_WndBagSlots.m_lcAddBagSlots.m_BagAddBackpacks
	tbl_bag[srcSlot] = nil
	
	local Space = SpaceMgr:GetSpace(g_StoreRoomIndex.PlayerBag)
	local MainBagSize = Space:Size()
	--SrcCt:PlaceBagEnd()
	
	DelMainBagGrid(g_GameMain.m_WndMainBag.m_ctItemRoom,g_PlayerMainBag.size,g_PlayerMainBag.columns,MainBagSize)
	AddGridsToMainBag(g_GameMain.m_WndBagSlots.m_lcAddBagSlots,g_AddRoomIndexClient.PlayerExpandBag)
	DescCt:UpdateWndDisPlayByPos(descPos, descRoom)
	
	MainBagSize = Space:Size()
	g_GameMain.m_WndMainBag:SetListScale(MainBagSize, g_PlayerMainBag.columns)
	g_GameMain.m_WndMainBag:SetMainItemBagState(true)
	
	--更新快捷栏上该物品的数量
	if(g_GameMain.m_MainSkillsToolBar and not g_GameMain.bCleanBagFlag) then
		local srcGrid = SpaceMgr:GetGridByRoomPos(descRoom, descPos)
		local srcType, srcIndex = srcGrid:GetType()
		g_GameMain.m_MainSkillsToolBar:SetShortCutItemNum(srcIndex)
		g_GameMain.m_MainSkillsToolBar:DelShortCutItem(srcIndex)
	end	
end

function CStoreRoomWndMgr:Rpc_OnDelBagSuccess(Slot)
	local SpaceMgr	= g_GameMain.m_BagSpaceMgr
	local nSlotRoom	= g_Client_GetSlotRoomIndexBySlot(Slot)
	local SrcCt		= SpaceMgr:GetSpaceRelateLc(nSlotRoom)	
	local nLocalPos	= SrcCt:GetLocalSlotPos(Slot)
	local SrcSpace = SpaceMgr:GetSpace(nSlotRoom)
	local Grid = SrcSpace:GetGrid(nLocalPos)
	Grid:DelNotPile()
	
	SrcCt:TakeDownBag(nLocalPos)
	SrcCt:UpdateSlot(nLocalPos, 0)
	SrcCt:PlaceBagEnd()
end

function CStoreRoomWndMgr:Rpc_SwitchBagSuccess(srcSlot, descSlot)
	local SpaceMgr		= g_GameMain.m_BagSpaceMgr
	local srcSlotRoom	= g_Client_GetSlotRoomIndexBySlot(srcSlot)
	local descSlotRoom	= g_Client_GetSlotRoomIndexBySlot(descSlot)
	local SrcCt			= SpaceMgr:GetSpaceRelateLc(srcSlotRoom)
	local DescCt		= SpaceMgr:GetSpaceRelateLc(descSlotRoom)
	local srcLocalPos	= SrcCt:GetLocalSlotPos(srcSlot)
	local descLocalPos	= DescCt:GetLocalSlotPos(descSlot)
	local srcGrid		= SpaceMgr:GetGridByRoomPos(srcSlotRoom, srcLocalPos)
	local descGrid		= SpaceMgr:GetGridByRoomPos(descSlotRoom, descLocalPos)

	local Space = SpaceMgr:GetSpace(g_StoreRoomIndex.PlayerBag)
	local MainBagSize = Space:Size()
	local tbl_bag = g_GameMain.m_WndBagSlots.m_lcAddBagSlots.m_BagAddBackpacks

	if not descGrid:Empty() then
		local descItem = descGrid:GetItem()
		local desccontext = descItem:GetContext()
		local descSize, descRoomIndex = desccontext.Size, desccontext.RoomIndex
		
		if srcSlotRoom == descSlotRoom and srcSlotRoom == g_AddRoomIndexClient.PlayerExpandBag then
			tbl_bag[descLocalPos],tbl_bag[srcLocalPos] = tbl_bag[srcLocalPos],tbl_bag[descLocalPos] 	
		end 

		SpaceMgr:ExChangeTwoGrid(srcSlotRoom,srcLocalPos,descSlotRoom,descLocalPos)
		srcGrid.m_Context["BagPos"], descGrid.m_Context["BagPos"] = descGrid.m_Context["BagPos"], srcGrid.m_Context["BagPos"]
		
		SrcCt:TakeDownBag(srcLocalPos)
		SrcCt:UpdateSlot(srcLocalPos,tbl_bag[srcLocalPos].m_StoreRoomSize)
		
		DescCt:TakeDownBag(descLocalPos)
		DescCt:UpdateSlot(descLocalPos,tbl_bag[descLocalPos].m_StoreRoomSize)
	else
		SpaceMgr:ExChangeTwoGrid(srcSlotRoom,srcLocalPos,descSlotRoom,descLocalPos)
		descGrid = SpaceMgr:GetGridByRoomPos(descSlotRoom, descLocalPos)
		descGrid.m_Context["BagPos"] = descLocalPos
		if srcSlotRoom == descSlotRoom and srcSlotRoom == g_AddRoomIndexClient.PlayerExpandBag then
			tbl_bag[descLocalPos] = tbl_bag[srcLocalPos]
		end
		SrcCt:TakeDownBag(srcLocalPos) 
		tbl_bag[srcLocalPos] = nil
		DescCt:UpdateSlot(descLocalPos,tbl_bag[descLocalPos].m_StoreRoomSize)
	end
	DelMainBagGrid(g_GameMain.m_WndMainBag.m_ctItemRoom,g_PlayerMainBag.size,g_PlayerMainBag.columns,MainBagSize)
	AddGridsToMainBag(g_GameMain.m_WndBagSlots.m_lcAddBagSlots,g_AddRoomIndexClient.PlayerExpandBag)
end

function CStoreRoomWndMgr:SetItemGridWndState(room,pos,state)
	assert(room and pos)
	assert("boolean" == type(state))
	local Mgr = g_GameMain.m_BagSpaceMgr
	local Lc = Mgr:GetSpaceRelateLc(room)
	if g_GameMain.m_WndMainBag.m_ctItemRoom:IsInMainBagExpandByAddBag(room) then
		pos = g_GameMain.m_WndMainBag.m_ctItemRoom:GetDesPos(room,pos)
	end
	local x, y = Lc:ParsePos(pos, Lc.m_nCol)
	Lc:SetClickedWndState(y, x, room, state)
end


function CStoreRoomWndMgr:SetClickedWndState(srcRoom,srcPos,descRoom,descPos,state)
	assert(srcRoom and srcPos and descRoom and descPos)
	assert("boolean" == type(state))
	local Mgr			= g_GameMain.m_BagSpaceMgr
	local srcLc			= Mgr:GetSpaceRelateLc(srcRoom)
	local descLc 		= Mgr:GetSpaceRelateLc(descRoom)
	local descX, descY	= 0, 0
	local srcX, srcY	= 0, 0 

	if g_GameMain.m_WndMainBag.m_ctItemRoom:IsInMainBagExpandByAddBag(descRoom) then
		descPos = g_GameMain.m_WndMainBag.m_ctItemRoom:GetDesPos(descRoom,descPos)
	end
	if g_GameMain.m_WndMainBag.m_ctItemRoom:IsInMainBagExpandByAddBag(srcRoom) then
		srcPos = g_GameMain.m_WndMainBag.m_ctItemRoom:GetDesPos(srcRoom,srcPos)
	end
	
	descX, descY	= descLc:ParsePos(descPos, descLc.m_nCol)
	srcX, srcY		= srcLc:ParsePos(srcPos, srcLc.m_nCol)
	srcLc:SetClickedWndState(srcY, srcX, srcRoom, state)
	descLc:SetClickedWndState(descY, descX, descRoom, state)
end
