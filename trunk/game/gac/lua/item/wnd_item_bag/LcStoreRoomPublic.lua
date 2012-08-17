gac_require "information/tooltips/ToolTips"
gac_require "framework/texture_mgr/TextureMgr"

LcStoreRoomPublic = class(CLcStoreRoom)
-------------------------------------------------------------------
------删除所有物品------
function LcStoreRoomPublic:DeleteAllGridItem()
	if(self.m_nRow and self.m_nCol) then
		for i = 0, self.m_nRow-1 do
			for j = 0, self.m_nCol-1 do
				local grid = self:GetGridByIndex(i, j, self.m_nRoomIndex)
				local tbl  = grid:GetItemID(grid:Size())
				if(#tbl > 0) then
					for k = 1, #tbl do
						grid:DelUnique(tbl[k])
					end
				end
			end
		end
	end
end

------从格子上拾取物品------
function LcStoreRoomPublic:SetPickupItemFromCollectiveBag(nRoomIndex, row, col, num, State, page)
	local grid = self:GetGridByIndex(row, col, nRoomIndex)
	local wnd  = self._m_CDialog[row*self.m_nCol + col + 1]
	local vWnd = self:GetSubItem(row, col)
	vWnd:SetMouseOverDescAfter("")
	g_WndMouse:SetPickupItemFromCollectiveBag(nRoomIndex, row, col, num, wnd.m_BtnCD, State, page)
end

--右键点击物品栏
function LcStoreRoomPublic:DoByRClick(nRoomIndex,uParam1,uParam2,WndFarther)
	local state, context = g_WndMouse:GetCursorState()
	local curPos  = self:GetPosByIndex(uParam1,uParam2)
	local page = WndFarther.m_Page
	if(state == ECursorState.eCS_Normal and g_GameMain.m_WndMainBag:IsShow()) then
		Gac2Gas:QuikMoveItemFromCDepotToBag(g_Conn,nRoomIndex, curPos,page,g_StoreRoomIndex.PlayerBag)
	end
end

------左键点击物品栏，坐标是从(0,0)开始的------
function LcStoreRoomPublic:DoByClickRoom(nRoomIndex, uParam1, uParam2, wndFather)
	local state, context = g_WndMouse:GetCursorState()
	local page = wndFather.m_Page
	local fromRoom, fromRow, fromCol, count, fromPage
	if(nil ~= context and #context > 0) then
		fromRoom, fromRow, fromCol, count, fromPage = context[1], context[2],context[3], context[4], context[5]
	end
	
	local curPos  = self:GetPosByIndex(uParam1,uParam2)
	local curGrid = self:GetGridByIndex(uParam1, uParam2, nRoomIndex)
	local curSize = curGrid:Size()
	if(state == ECursorState.eCS_Normal) then          --鼠标上没有东西
		if(wndFather == g_GameMain.m_TongDepot) then   --如果点击的是帮会仓库
			if not self:GetClickWndItem(curPos, nRoomIndex)  then
				self:SetPickupItemFromCollectiveBag(nRoomIndex, uParam1, uParam2, _, ECursorState.eCS_PickupItemFromTongDepot, page)
			end
		end
	elseif(state == ECursorState.eCS_PickupItem) then  --鼠标上是从包裹类型的地方取出的物品
		if(wndFather == g_GameMain.m_TongDepot) then   --如果点击的是帮会仓库
			local srcCt  = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc(fromRoom)
			local srcPos = srcCt:GetPosByIndex(fromRow,fromCol)
			if(srcPos > g_PlayerMainBag.size) then     --鼠标上的物品是从背包的扩展空间里取出的
				fromRoom,srcPos = g_GameMain.m_WndBagSlots.m_lcAddBagSlots:GetSlotIndex(srcPos)
			end
			
			if g_Client_IsSlotIndex(fromRoom) then --是从附加包裹栏或者附加仓库里取出的物品
				--local nSrcPos = srcCt:GetGlobalSlotPos(srcPos)
				--Gac2Gas:CollectiveBagAddItemFromOther(g_Conn, nSrcPos, nRoomIndex, curPos)
			else
				Gac2Gas:CollectiveBagAddItemFromOther(g_Conn, fromRoom, srcPos, nRoomIndex, page ,curPos)
				--g_SetClickedWndState(fromRoom, srcPos, 6, curPos, false)
			end
			g_WndMouse:ClearCursorAll()
		end
	elseif(state == ECursorState.eCS_PickupItemFromTongDepot) then --鼠标上是从帮会仓库取出的物品
		if(wndFather == g_GameMain.m_TongDepot) then               --如果点击的是帮会仓库
			local fromPos = self:GetPosByIndex(fromRow, fromCol)
			if(fromRow == uParam1 and fromCol == uParam2 and fromPage == page) then     --如果拾取位置和要放置的位置相同
				self:SetClickedWndState(uParam1, uParam2, nRoomIndex, true)
				g_WndMouse:ClearCursorAll()
			else
				Gac2Gas:CollectiveBagMoveItemSelf(g_Conn, nRoomIndex, fromPage, fromPos, page, curPos)
			end
		end
	elseif(state == ECursorState.eCS_PickupItemFromNPCShop) then   --鼠标上是从NPC商店获取的物品
	elseif(state == ECursorState.eCS_PickupEquip) then             --鼠标上是装备
	elseif(state == ECursorState.eCS_Split and curSize > 1) then
	end
end

function LcStoreRoomPublic:DoByDragRoom(nRoomIndex, uParam1, uParam2, WndFarther)
	local page				= WndFarther.m_Page
	local state, context	= g_WndMouse:GetCursorState()
	local CurGrid			= self:GetGridByIndex(uParam1, uParam2, nRoomIndex)
	local CurPos			= self:GetPosByIndex(uParam1, uParam2, nRoomIndex)

	local Cursize = CurGrid:Size()
	if 0 == Cursize then
		return
	end
	if state == ECursorState.eCS_Normal then --手上没有物品
		if not self:GetClickWndItem(CurPos, nRoomIndex)  then
			self:SetPickupItemFromCollectiveBag(nRoomIndex, uParam1, uParam2, _, ECursorState.eCS_PickupItemFromTongDepot, page)
		end
	end
end

--------------------------------------------------------------------------------------------------------------

------返回得到集体包裹某页物品信息开始-------
function Gas2Gac:ReturnGetCollectiveBagOnePageItemListStart(Conn, type)
	if(type == g_StoreRoomIndex.TongDepot) then --帮会仓库
		g_GameMain.m_TongDepot.m_TongItemRoom:DeleteAllGridItem()
	end
end

------返回得到集体包裹某页物品信息结束-------
function Gas2Gac:ReturnGetCollectiveBagOnePageItemListEnd(Conn, type, page)
	if(type == g_StoreRoomIndex.TongDepot) then --帮会仓库
		g_GameMain.m_TongDepot:Draw()
	end
end

------返回集体包裹在自身内部的移动操作------
function Gas2Gac:ReturnCollectiveBagMoveItemSelf(Conn, bFlag, type, page)
	if(type == g_StoreRoomIndex.TongDepot) then 
		g_GameMain.m_TongDepot:GetTongDepotOnePageItemList(page)
	end
	if(bFlag) then g_WndMouse:ClearCursorAll() end
end

------返回从其它来源向集体包裹添加物品------
function Gas2Gac:ReturnCollectiveBagAddItemFromOther(Conn, bFlag, type, page)
	if(type == g_StoreRoomIndex.TongDepot) then 
		g_GameMain.m_TongDepot:GetTongDepotOnePageItemList(page)
	end
	if(bFlag) then g_WndMouse:ClearCursorAll() end
end

------返回从集体包裹像其它处添加物品------
function Gas2Gac:ReturnBagAddItemFromCollectiveBag(Conn, bFlag, type, page)
	if(type == g_StoreRoomIndex.TongDepot) then 
		g_GameMain.m_TongDepot:GetTongDepotOnePageItemList(page)
	end
	g_WndMouse:ClearCursorAll()
	if(not bFlag) then MsgClient(9164) end
end