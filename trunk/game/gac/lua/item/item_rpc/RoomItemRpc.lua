CRoomItemRpc = class()

--*****************************************************************************************
--初始化玩家的附背包信息
--*****************************************************************************************
function CRoomItemRpc.RetCharBagInfo(Connection,nItemID,uBagSlotIndex,nRoomIndex,nBigID,nIndex,nBindingType)
	local ROOM_INDEX = g_StoreRoomIndex
	local BagRoom = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"BagRoom")
	g_DynItemInfoMgr:CreateItemInfo(nItemID,nBigID,nIndex,nBindingType)
	local BagInfo = {Size=BagRoom,RoomIndex=nRoomIndex,BagPos=uBagSlotIndex}
	local Item = CItem:new(nBigID,nIndex,nItemID,nBindingType,BagInfo)
	g_GameMain.m_BagSpaceMgr:AddSpace(nRoomIndex,BagRoom)
	local uSlotRoom = g_Client_GetSlotRoomIndexBySlot(uBagSlotIndex)
	local Space = g_GameMain.m_BagSpaceMgr:GetSpace(uSlotRoom)
	local localPos = g_Client_GetLocalSlotPos(uBagSlotIndex)
	Space:AddItemByPos(localPos, Item)
	local Space = g_GameMain.m_BagSpaceMgr:GetSpace(g_StoreRoomIndex.PlayerBag)
	local MainBagSize = Space:Size()
	DelMainBagGrid(g_GameMain.m_WndMainBag.m_ctItemRoom,g_PlayerMainBag.size,g_PlayerMainBag.columns,MainBagSize)
	AddGridsToMainBag(g_GameMain.m_WndBagSlots.m_lcAddBagSlots, g_AddRoomIndexClient.PlayerExpandBag)
end

function CRoomItemRpc.RetCharBagInfoEnd(Connection)
	local Space = g_GameMain.m_BagSpaceMgr:GetSpace(g_StoreRoomIndex.PlayerBag)
	local MainBagSize = Space:Size()
	g_GameMain.m_WndMainBag:SetListScale(MainBagSize, g_PlayerMainBag.columns)
end

--*****************************************************************************************
--扩展包相关
--*****************************************************************************************
function CRoomItemRpc.ReturnSetBagState(Connection)
	g_GameMain.m_WndMainBag:SetMainItemBagState(true)
end

--交换两个包
function CRoomItemRpc.ReturnChange2Bag(Connection,nASlot,nBSlot)
	CStoreRoomWndMgr.GetMgr():Rpc_SwitchBagSuccess(nASlot,nBSlot)
	g_GameMain.m_WndMainBag:SetMainItemBagState(true)
end

function CRoomItemRpc.ReturnChange2BagError(Connection)
	g_GameMain.m_WndMainBag:SetMainItemBagState(true)
end

--取下包
function CRoomItemRpc.ReturnFetchBag(Connection, nSlot, nRoom, nPos, nGridID)
	CStoreRoomWndMgr.GetMgr():Rpc_OnFetchBagSuccess(nSlot, nRoom, nPos, nGridID)
	SetEvent( Event.Test.FetchBag,true, "FetchBagSuccess" )
end

function CRoomItemRpc.ReturnFetchBagError(Connection)
	g_GameMain.m_WndMainBag:SetMainItemBagState(true)
end

--放置背包
function CRoomItemRpc.ReturnPlaceBag(Connection,nRoom,nPos,nSlot,nUseRoomIndex)
	CStoreRoomWndMgr.GetMgr():Rpc_OnPlaceBagSuccess(nRoom,nPos,nSlot,nUseRoomIndex)
end

function CRoomItemRpc.ReturnPlaceBagError(Connection)
	g_GameMain.m_WndMainBag:SetMainItemBagState(true)
	SetEvent( Event.Test.PlaceBag,true, "PlaceBagError" )
end

--删除背包
function CRoomItemRpc.ReturnDelBag(Connection,nSlot)
	CStoreRoomWndMgr.GetMgr():Rpc_OnDelBagSuccess(nSlot)
	SetEvent( Event.Test.DelBag,true, "DelBagSuccess" )
end

function CRoomItemRpc.ReturnDelBagError(Connection,nError)
	SetEvent( Event.Test.DelBag,true, "DelBagError" )
end

--*****************************************************************************************
--增删物品
--*****************************************************************************************
function CRoomItemRpc.RetItemRoom(nRoomIndex)
	g_GameMain.m_BagSpaceMgr.m_nTempRoom = nRoomIndex
end

function CRoomItemRpc.RetRefreshBag()
	g_GameMain.m_WndBagSlots.m_lcAddBagSlots:UpdateBagSlotsPos()
	UpdateObjCanUseState()
end

--*****************************************************************************************
--添加物品
--*****************************************************************************************
function CRoomItemRpc.RetAddItemToGrid(nItemID)
	g_GameMain.m_BagSpaceMgr:RetAddItemToGrid(nItemID)
end

function CRoomItemRpc.RetAddItemToGridEnd(nGridID, nPos, nType, sName, nBind)
	g_GameMain.m_BagSpaceMgr:RetAddItemToGridEnd(nGridID, nPos, nType, sName, nBind)
end

function CRoomItemRpc.RetAddItemToGridEndEx(nGridID, nPos)
	g_GameMain.m_BagSpaceMgr:RetAddItemToGridEndEx(nGridID, nPos)
end

--只用在佣兵小队仓库
function CRoomItemRpc.RetAddItem(nItemID,nRoomIndex,nPos)
	local ItemInfo = g_DynItemInfoMgr:GetDynItemInfo(nItemID)
	g_GameMain.m_BagSpaceMgr:AddItemByRoomPos(nItemID,nRoomIndex,nPos,tonumber(ItemInfo.m_nBigID),ItemInfo.m_nIndex,tonumber(nItemID),nil,ItemInfo.BindingType)
	g_GameMain.m_WndMainBag.m_ctItemRoom:SetItemFlashInfo(nRoomIndex,nPos)
end

--*****************************************************************************************
--删除物品
--*****************************************************************************************
function CRoomItemRpc.RetDelItemFromGrid(nItemID, nRoomIndex, nPos, bDelDynInfo)
	local Grid = g_GameMain.m_BagSpaceMgr:GetGridByRoomPos(nRoomIndex, nPos)
	local nBigID, nIndex = Grid:GetType()
	g_GameMain.m_BagSpaceMgr:RetDelItemFromGrid(nItemID, nRoomIndex, nPos, bDelDynInfo)
	g_GameMain.m_BagSpaceMgr:UpdateRoomPos(nRoomIndex,nPos)
	g_GameMain.m_WndMainBag.m_ctItemRoom:ClearCursorAllAfterDelete(nRoomIndex,nPos)
	-------（玩法组添加）删除物品时通知任务相关系统------------
	UpdateQuestItemCount(nBigID, nIndex, true)
	-----------------------------------------------------------
	g_GameMain.m_WndMainBag.m_ctItemRoom:DelItemFlashInfo(nRoomIndex,nPos)
	--更新快捷栏上该物品的数量
	if g_GameMain.m_MainSkillsToolBar and not g_GameMain.bCleanBagFlag then
		g_GameMain.m_MainSkillsToolBar:SetShortCutItemNum(nIndex)
		g_GameMain.m_MainSkillsToolBar:DelShortCutItem(nIndex)
	end

	--生活技能面板显示的物品数量
	g_GameMain.m_LiveSkillProdMain.m_LiveSkillProduction:UpdateMaterialCountShow(nIndex)
	
	CRoomItemRpc.RetRefreshBag()
	
	--同步刷新修理装备面板的数据
	if g_GameMain.m_NewFixEquipWnd:IsShow() then
		g_GameMain.m_CEquipInRoleWnd:SetData()
		g_GameMain.m_CEquipInBagWnd:SetData()
	end
end

function CRoomItemRpc.RetDelItemError( Connection,nError)
	SetEvent( Event.Test.DelItem,true, "DelItemError" )
end

--*****************************************************************************************
--交换格子/整理物品
--*****************************************************************************************
function CRoomItemRpc.RetMoveItemByGridID(nGridID, nRoomIndex, nPos)
	g_GameMain.m_BagSpaceMgr:RetMoveItemByGridID(nGridID, nRoomIndex, nPos)
end

function CRoomItemRpc.RetReplaceItemByGridIDEnd(nSrcRoom, nSrcPos, nDescRoom, nDescPos)
	g_GameMain.m_BagSpaceMgr:RetReplaceItemByGridIDEnd(nSrcRoom, nSrcPos, nDescRoom, nDescPos)
	local Grid = g_GameMain.m_BagSpaceMgr:GetGridByRoomPos(nDescRoom, nDescPos)
	local nBigID, nIndex = Grid:GetType()
	if g_GameMain.m_MainSkillsToolBar and not g_GameMain.bCleanBagFlag then
		g_GameMain.m_MainSkillsToolBar:SetShortCutItemNum(nIndex)
		g_GameMain.m_MainSkillsToolBar:DelShortCutItem(nIndex)
	end
end

function CRoomItemRpc.RetCleanBag(Connection)
	g_GameMain.m_WndBagSlots.m_lcAddBagSlots:UpdateBagSlotsPos()
	g_GameMain.m_WndMainBag:SetMainItemBagState(true)
	g_GameMain.bCleanBagFlag = false
end

--*****************************************************************************************

function CRoomItemRpc.RetSoulPearlInfo( Connection, uItemId, uSoulNum )
	g_GameMain.m_GMConsole:SetResult("AddSoul(" .. tostring(uItemId).. "):" ..  tostring(uSoulNum))
	SetEvent( Event.Test.AddSoul,true, "AddSoulSuccess" )
end

--鼠标右键，交换物品
function CRoomItemRpc.RetQuickMoveEnd(Connection)
	SetEvent( Event.Test.QuickMove,true, "QuickMoveSuccess" )
end

function CRoomItemRpc.RetQuickMoveError(Connection,nError)
	g_WndMouse:ClearCursorAll()
	SetEvent( Event.Test.QuickMove,true, "QuickMoveError" )
end

--删除物品的GM命令错误返回
function CRoomItemRpc.RetDelItemGMError( Connection,nBigID,nIndex,sError)
	local DisplayName = g_ItemInfoMgr:GetItemLanInfo(nBigID, nIndex,"DisplayName")
	g_GameMain.m_GMConsole:SetResult("delete item type " .. tostring(nBigID) .. ",name " .. DisplayName .. "failed." .. sError)
	SetEvent( Event.Test.DelItem,true, "DelItemError" )
end

--设置事件，为gm命令删除
function CRoomItemRpc.RetDelItemAllGMEnd( Connection,nBigID,nIndex,nCount)
	local DisplayName = g_ItemInfoMgr:GetItemLanInfo(nBigID, nIndex,"DisplayName")
	g_GameMain.m_GMConsole:SetResult("delete item type " .. tostring(nBigID) .. ",name " .. DisplayName .. ",number " .. tostring(nCount))
	SetEvent( Event.Test.DelItem,true, "DelItemSuccess" )
end

function CRoomItemRpc.SetItemBindingTypeByID(Connection,nItemID,nBindingType)
	g_DynItemInfoMgr:SetItemBindingType(nItemID,nBindingType)
end

function CRoomItemRpc.RetItemTypeInfo(Connection,nItemID,nBigID,nIndex,nBindingType)
	g_DynItemInfoMgr:CreateItemInfo(nItemID,nBigID,nIndex,nBindingType)
end

function CRoomItemRpc.RetItemLeftTime(Connection,nItemID,nLeftTime)
	g_DynItemInfoMgr:AddItemTimeInfo(nItemID,nLeftTime)
end

--物品制作者
function CRoomItemRpc.RetItemMakerName(Connection,maker_name,item_id)
	g_DynItemInfoMgr:AddItemMakerName(item_id,maker_name)
end

--设置事件部分
function CRoomItemRpc.RetAddItemAllGMEnd( Connection,nBigID,nIndex,nCount)
	local DisplayName = g_ItemInfoMgr:GetItemLanInfo(nBigID, nIndex,"DisplayName")
	g_GameMain.m_GMConsole:SetResult("Additem:type " .. nBigID .. ", name '" .. DisplayName .. "', number " .. nCount .. ". succeed")
	SetEvent( Event.Test.AddItem,true, "AddItemSuccess" )
end

function CRoomItemRpc.RetAddItemGMError( Connection,nBigID,nIndex,sError)
	local DisplayName = g_ItemInfoMgr:GetItemLanInfo(nBigID, nIndex,"DisplayName")
	g_GameMain.m_GMConsole:SetResult("Additem:type " .. nBigID .. ", name '" .. DisplayName .. "'. failed:" .. sError)
end

--拆分物品
function CRoomItemRpc.RetSplitItem(nSrcRoom, nSrcPos, nItemID)
	CStoreRoomWndMgr.GetMgr():Rpc_OnSplitItemResult(nSrcRoom, nSrcPos, nItemID)
end

function CRoomItemRpc.RetSplitItemEnd(nSrcRoom, nSrcPos, nDescRoom, nDescPos)
	CStoreRoomWndMgr.GetMgr():Rpc_OnSplitItemResultEnd(nSrcRoom, nSrcPos, nDescRoom, nDescPos)
end

function CRoomItemRpc.RetSplitItemError( Connection, nError, nARoom, nAPos, nBRoom, nBPos)
	g_WndMouse:ClearCursorAll()
	CStoreRoomWndMgr.GetMgr():SetClickedWndState(nARoom, nAPos, nBRoom, nBPos, true)
	SetEvent( Event.Test.SplitItem,true, "SplitItemError" )
end

--移动物品
function CRoomItemRpc.RetMoveItem( Connection,nARoom,nAPos,nItemID,nBRoom,nBPos)
	local nIndex 
	local slotRoomBegin = g_StoreRoomIndex.SlotRoomBegin
	local slotRoomEnd = slotRoomBegin + 3
	if ((nARoom == g_StoreRoomIndex.PlayerBag) or (nARoom >= slotRoomBegin and nARoom <= slotRoomEnd)) then
		--A是背包,nIndex放在g_Rpc_OnMoveItemResult前面获取
		local Grid = g_GameMain.m_BagSpaceMgr:GetGridByRoomPos(nARoom,nAPos)
		local _,Index = Grid:GetType()
		nIndex = Index
	end
	
	CStoreRoomWndMgr.GetMgr():Rpc_OnMoveItemResult(nARoom,nAPos,nItemID,nBRoom,nBPos)
	
	if not nIndex then
		--A不是背包，B是背包，放在g_Rpc_OnMoveItemResult后面获取
		if ((nBRoom == g_StoreRoomIndex.PlayerBag) or (nBRoom >= slotRoomBegin and nBRoom <= slotRoomEnd)) then
			local Grid = g_GameMain.m_BagSpaceMgr:GetGridByRoomPos(nBRoom,nBPos)
			local _,Index = Grid:GetType()
			nIndex = Index
		end
	end
	
	--更新快捷栏上该物品的数量
	if g_GameMain.m_MainSkillsToolBar and nIndex then
		g_GameMain.m_MainSkillsToolBar:SetShortCutItemNum(nIndex)
		g_GameMain.m_MainSkillsToolBar:DelShortCutItem(nIndex)
	end
end

function CRoomItemRpc.RetMoveItemEnd( Connection,nARoom,nAPos,nBRoom,nBPos)
	g_GameMain.m_WndMainBag.m_ctItemRoom:CDItemChanged(nARoom, nAPos)
	g_GameMain.m_WndMainBag.m_ctItemRoom:CDItemChanged(nBRoom, nBPos)
	g_GameMain.m_WndMainBag.m_ctItemRoom:SetItemFlashInfo(nARoom,nAPos)
	g_GameMain.m_WndMainBag.m_ctItemRoom:SetItemFlashInfo(nBRoom,nBPos)
	CStoreRoomWndMgr.GetMgr():Rpc_OnMoveItemResultEnd(nARoom,nAPos,nBRoom,nBPos)
	SetEvent( Event.Test.MoveItem,true, "MoveItemSuccess" )
	local Grid = g_GameMain.m_BagSpaceMgr:GetGridByRoomPos(nBRoom,nBPos)
	local nBigID,nIndex = Grid:GetType()
	g_ItemInfoMgr:PlayItemSound(nBigID,nIndex)
end

function CRoomItemRpc.RetPlayerItemSound(Conn, nItemType, sItemName)
	 g_ItemInfoMgr:PlayItemSound(nItemType, sItemName)
end

--移动物品的服务端回调
function CRoomItemRpc.RetMoveReplaceItem( Connection,nARoom,nAPos,nBRoom,nBPos )
	CStoreRoomWndMgr.GetMgr():Rpc_OnReplaceItemResult(nARoom,nAPos,nBRoom,nBPos)
	SetEvent( Event.Test.ReplaceItem,true, "ReplaceItemSuccess" )
	g_GameMain.m_WndMainBag.m_ctItemRoom:CDItemChanged(nARoom, nAPos)
	g_GameMain.m_WndMainBag.m_ctItemRoom:CDItemChanged(nBRoom, nBPos)
	
	local slotRoomBegin = g_StoreRoomIndex.SlotRoomBegin
	local slotRoomEnd = slotRoomBegin + 3
	if nARoom == g_StoreRoomIndex.PlayerBag or nBRoom == g_StoreRoomIndex.PlayerBag or (nARoom >= slotRoomBegin and nARoom <= slotRoomEnd) or (nBRoom >= slotRoomBegin and nBRoom <= slotRoomEnd) then
		if g_GameMain.m_MainSkillsToolBar then
			local AGrid = g_GameMain.m_BagSpaceMgr:GetGridByRoomPos(nARoom,nAPos)
			local _,nAIndex = AGrid:GetType()
			local BGrid = g_GameMain.m_BagSpaceMgr:GetGridByRoomPos(nBRoom,nBPos)
			local _,nBIndex = BGrid:GetType()
			if nAIndex then
				g_GameMain.m_MainSkillsToolBar:SetShortCutItemNum(nAIndex)
				g_GameMain.m_MainSkillsToolBar:DelShortCutItem(nAIndex)
			end
			if nBIndex then
				g_GameMain.m_MainSkillsToolBar:SetShortCutItemNum(nBIndex)
				g_GameMain.m_MainSkillsToolBar:DelShortCutItem(nBIndex)
			end
		end
	end
	g_GameMain.m_WndMainBag.m_ctItemRoom:SetItemFlashInfo(nARoom,nAPos)
	g_GameMain.m_WndMainBag.m_ctItemRoom:SetItemFlashInfo(nBRoom,nBPos)
	
	local Grid = g_GameMain.m_BagSpaceMgr:GetGridByRoomPos(nBRoom,nBPos)
	local nBigID,nIndex = Grid:GetType()
	g_ItemInfoMgr:PlayItemSound(nBigID,nIndex)
end

function CRoomItemRpc.RetMoveItemError( Connection,nARoom, nAPos, nBRoom, nBPos)
	CStoreRoomWndMgr.GetMgr():SetClickedWndState(nARoom, nAPos, nBRoom, nBPos, true)
	SetEvent( Event.Test.MoveItem,true, "MoveItemError" )
end