--[[
	在这里做了添加和删除物品的转掉函数，命名都为ByRoomPos为后缀
	然后调用CBagSpace对应的函数，CBagSpace在找到对应的格子
	调用相应的添加和删除函数
--]]

CBagSpaceMgr = class()

function CBagSpaceMgr.Init()
	local NewInstance = CBagSpaceMgr:new()
	NewInstance.m_tblAllSpace		= {}
	NewInstance.m_tblItemID_Temp	= {}
	NewInstance.m_tblGridIDIndex	= {}
	return NewInstance
end

function CBagSpaceMgr:AddSpace(index, size)
	self.m_tblAllSpace[index] = CBagSpace.Init(size)
	return self:GetSpace(index)
end

function CBagSpaceMgr:FindItemByType(BigID, SmallID)
	for i,v in pairs(self.m_tblAllSpace) do
		local res = v:FindItem(BigID, SmallID)
		if res then
			return i, res
		end
	end
	return nil
end

--查找能叠放某个物品的格子
--如果不够放，返回nil
function CBagSpaceMgr:FindGridsForAddItem(BigID, ItemName,AddCount)
	local FoldLimit = g_ItemInfoMgr:GetItemInfo(BigID, ItemName,"FoldLimit" ) or 1
	local tbl_space = {}
	local function RetFoldSpace(RoomIndex)
			local space = self:GetSpace(RoomIndex)
			if not space then return end
			for i=1,space:Size() do
				local pos = i
				if not space.m_tblGrids[pos]:Empty() then
					local nGridBigID,nGridIndex = space.m_tblGrids[pos]:GetType()
					if BigID == nGridBigID and ItemName == nGridIndex then
						local have_size = space.m_tblGrids[pos]:Size()
						local can_add_count = 0
						if  AddCount + have_size <= FoldLimit  then
							can_add_count = AddCount
						else
							can_add_count = FoldLimit-have_size
						end
						if can_add_count > 0 then
							table.insert(tbl_space,{RoomIndex,pos,can_add_count})
							AddCount = AddCount - can_add_count
							if AddCount <= 0 then 
								return 0
							end
						end
					end
				end
			end
	end
	local function RetEmptySpace(RoomIndex)
			local space = self:GetSpace(RoomIndex)
			if not space then return end
			for i=1,space:Size() do
				local pos = i
				if space.m_tblGrids[pos]:Empty() then
					local can_add_count = 0
					if  AddCount <= FoldLimit  then
						can_add_count = AddCount
					else
						can_add_count = FoldLimit
					end
					table.insert(tbl_space,{RoomIndex,pos,can_add_count})
					AddCount = AddCount - can_add_count
					if AddCount <= 0 then 
						return 0
					end
				end
			end
	end
	local main_bag = g_StoreRoomIndex.PlayerBag
	if RetFoldSpace(main_bag) == 0 then 
		return tbl_space
	end
	local slotRoomBegin, slotRoomEnd = g_GetRoomRange( main_bag )
	for i = slotRoomBegin, slotRoomEnd do 
		if RetFoldSpace(i) == 0 then 
			return tbl_space
		end
	end
	if RetEmptySpace(main_bag) == 0 then 
		return tbl_space
	end
	for i = slotRoomBegin, slotRoomEnd do 
		if RetEmptySpace(i) == 0 then 
			return tbl_space
		end
	end
	return nil
end

function CBagSpaceMgr:FindItemBySpace(nRoomIndex, BigID, SmallID)
	local res = self:FindItemByRoom(nRoomIndex, BigID, SmallID)
	if( res ) then
		return nRoomIndex, res
	else
		local slotRoomBegin, slotRoomEnd = g_GetRoomRange( nRoomIndex )
		for i = slotRoomBegin, slotRoomEnd do --计算扩展包里面的数量
			res = self:FindItemByRoom(i, BigID, SmallID)
			if ( res ) then
				return i, res
			end
		end
	end
	return nil
end

function CBagSpaceMgr:FindItemByRoom(nRoomIndex, BigID, SmallID)
	local space = self:GetSpace(nRoomIndex)
	if( space ) then
		return space:FindItem(BigID, SmallID)
	end
	return nil
end

function CBagSpaceMgr:FindItemByTypeFromAppointedRoom(BigID, SmallID)
	local tblSimpleRoomIndex = g_SimpleRoomIndex
	for i, v in pairs(self.m_tblAllSpace) do
		
		local bFlag = true
		for j, u in ipairs(tblSimpleRoomIndex) do
			if(i == u) then bFlag = false break end
		end
		
		if(bFlag) then
			local res = v:FindItem(BigID, SmallID)
			if res then
				return i, res
			end
		end
	end
	return nil
end


function CBagSpaceMgr:FindItemRoomAndPosByItemID(nItemID)
	local tblSimpleRoomIndex = g_SimpleRoomIndex
	for i, v in pairs(self.m_tblAllSpace) do
		
		local bFlag = true
		for j, u in ipairs(tblSimpleRoomIndex) do
			if(i == u) then bFlag = false break end
		end
		
		if(bFlag) then
			local res = v:FindItemByItemID(nItemID)
			if res then
				return i, res
			end
		end
	end
	return nil
end

function CBagSpaceMgr:GetItemCountByType(BigID, SmallID)
	local TotalCount = 0
	for i,v in pairs(self.m_tblAllSpace) do
		local res = v:GetItemCountByType(BigID, SmallID)
		TotalCount = TotalCount + res
	end
	return TotalCount
end

function CBagSpaceMgr:GetItemCountBySpace(nRoomIndex, BigID, SmallID)
	local TotalCount = 0
	if BigID and SmallID then
		local space = self:GetSpace(nRoomIndex)
		TotalCount = space:GetItemCountByType(BigID, SmallID)
		local slotRoomBegin,slotRoomEnd = g_GetRoomRange( nRoomIndex )
		--下面是计算扩展包里面的数量
		for i = slotRoomBegin,slotRoomEnd  do
			local spacei = self:GetSpace(i)
			if spacei then
				TotalCount = TotalCount + spacei:GetItemCountByType(BigID, SmallID)
			end
		end
	end
	return TotalCount
end

-- 实际物品数量
function CBagSpaceMgr:GetItemCount()
	local TotalCount = 0
	for i,v in pairs(self.m_tblAllSpace) do
		local res = v:GetItemCount()
		TotalCount = TotalCount + res
	end
	return TotalCount
end

-- 总存储空间
function CBagSpaceMgr:GetTotalRoomSize()
	local TotalCount = 0
	for i,v in pairs(self.m_tblAllSpace) do
		local res = v:Size()
		TotalCount = TotalCount + res
	end
	return TotalCount 
end

function CBagSpaceMgr:SetSpaceRelateLc(index, lc)
	local space = self:GetSpace(index)
	assert(space~=nil)
	space.m_Lc = lc
end

function CBagSpaceMgr:GetSpaceRelateLc(index)
	return self:GetSpace(index).m_Lc
end

function CBagSpaceMgr:GetSpace(index)
	return self.m_tblAllSpace[index]
end

function CBagSpaceMgr:DelSpace(index)
	self.m_tblAllSpace[index] = nil
end

function CBagSpaceMgr:DelSpaceRelateLc(index)
	self:GetSpaceRelateLc(index).m_Lc = nil
end

function CBagSpaceMgr:GetGridByRoomPos(RoomIndex, Pos)
	local space = self:GetSpace(RoomIndex)
	assert(space~=nil, RoomIndex)
	return space:GetGrid(Pos)
end

--*******************************************************************************************************
--添加物品
--*******************************************************************************************************
--在某个位置上添加某物品
function CBagSpaceMgr:AddItemByRoomPos(nItemID, nRoomIndex, nPos, nBigID, nIndex, Context, nBindingType, nGridID)
	local Item = CItem:new(nBigID,nIndex,nItemID,nBindingType,Context)
	local Space = self:GetSpace(nRoomIndex)
	Space:AddItemByPos(nPos, Item, nGridID)
end

function CBagSpaceMgr:RetAddItemRoom(nRoomIndex)
	self.m_nTempRoom = nRoomIndex
end

function CBagSpaceMgr:RetAddItemToGrid(nItemID)
	table.insert(self.m_tblItemID_Temp, nItemID)
end

function CBagSpaceMgr:AfterAddItemToGrid(nPos, nType, sName)
	local nRoomIndex = self.m_nTempRoom
	self:UpdateRoomPos(self.m_nTempRoom, nPos)
	-----------（玩法组添加）添加物品通知任务相关系统----
	UpdateQuestItemCount(nType, sName, true)
	-----------------------------------------------------
	--更新快捷栏上该物品的数量
	if g_GameMain.m_MainSkillsToolBar and not g_GameMain.bCleanBagFlag then
		g_GameMain.m_MainSkillsToolBar:SetShortCutItemNum(sName)
		g_GameMain.m_MainSkillsToolBar:DelShortCutItem(sName)
	end
	--刷新烹饪制药面板显示的物品数量
	g_GameMain.m_LiveSkillProdMain.m_LiveSkillProduction:UpdateMaterialCountShow(sName)
	CRoomItemRpc.RetRefreshBag()
end

function CBagSpaceMgr:RetAddItemToGridEnd(nGridID, nPos, nType, sName, nBindingType)
	for i, nItemID in ipairs(self.m_tblItemID_Temp) do
		g_DynItemInfoMgr:CreateItemInfo(nItemID, nType, sName, nBindingType)
		self:AddItemByRoomPos(nItemID, self.m_nTempRoom, nPos, nType, sName, nil, nBindingType, nGridID)
	end
	self.m_tblGridIDIndex[nGridID] = {self.m_nTempRoom, nPos}
	self.m_tblItemID_Temp = {}
	self:AfterAddItemToGrid(nPos, nType, sName)
end

function CBagSpaceMgr:RetAddItemToGridEndEx(nGridID, nPos)
	local nType, sName = nil, nil
	for i, nItemID in ipairs(self.m_tblItemID_Temp) do
		local DynItemInfo = g_DynItemInfoMgr:GetDynItemInfo(nItemID)
		nType, sName = DynItemInfo.m_nBigID, DynItemInfo.m_nIndex
		self:AddItemByRoomPos(nItemID, self.m_nTempRoom, nPos, nType, sName, nil, DynItemInfo.BindingType, nGridID)
	end
	self.m_tblGridIDIndex[nGridID] = {self.m_nTempRoom, nPos}
	self.m_tblItemID_Temp = {}
	self:AfterAddItemToGrid(nPos, nType, sName)
end
--*******************************************************************************************************
--删除物品
--*******************************************************************************************************
--在某个位置上删除物品
function CBagSpaceMgr:DelItemByRoomPos(nRoomIndex, nPos, nItemID)
	local Space		= self:GetSpace(nRoomIndex)
	local Grid		= Space.m_tblGrids[nPos]
	local nGridID	= Grid:GetGridID()
	Space:DelUniqueByPos(nPos, nItemID)
	if( Grid:Empty() ) then
		self.m_tblGridIDIndex[nGridID] = nil
	end
end

function CBagSpaceMgr:RetDelItemFromGrid(nItemID, nRoomIndex, nPos, bDelDynInfo)
	if(bDelDynInfo) then
		g_DynItemInfoMgr:DelItemInfo(nItemID)
	end
	self:DelItemByRoomPos(nRoomIndex, nPos, nItemID)
end

--在某位置上删除不可叠加物品
function CBagSpaceMgr:DelNotPileByRoomPos(nRoomIndex, nPos)
	local Space = self:GetSpace(nRoomIndex)
	assert(Space:DelNotPileByPos(nPos))
end
	
--*******************************************************************************************************
--整理包裹相关
--*******************************************************************************************************
function CBagSpaceMgr:RetMoveItemByGridID(nGridID, nRoomIndex, nPos)
	local nSrcRoom, nSrcPos = unpack(self.m_tblGridIDIndex[nGridID])
	self:ExChangeTwoGrid(nSrcRoom, nSrcPos, nRoomIndex, nPos)
	self:UpdateRoomPos(nSrcRoom, nSrcPos)
	self:UpdateRoomPos(nRoomIndex, nPos)
	CStoreRoomWndMgr.GetMgr():SetClickedWndState(nSrcRoom, nSrcPos, nRoomIndex, nPos, true)
end

function CBagSpaceMgr:RetReplaceItemByGridIDEnd(nSrcRoom, nSrcPos, nDescRoom, nDescPos)
	CStoreRoomWndMgr.GetMgr():SetClickedWndState(nSrcRoom, nSrcPos, nDescRoom, nDescPos, true)
	g_GameMain.m_WndBagSlots.m_lcAddBagSlots:UpdateBagSlotsPos()
	--同步刷新修理装备面板的数据
	if g_GameMain.m_NewFixEquipWnd:IsShow() then
		g_GameMain.m_CEquipInRoleWnd:SetData()
		g_GameMain.m_CEquipInBagWnd:SetData()
	end
end

--交换不同的两个room空间的格子
function CBagSpaceMgr:ExChangeTwoGrid(nARoom, nAPos, nBRoom, nBPos)
	assert(self.m_tblAllSpace[nARoom])
	assert(self.m_tblAllSpace[nBRoom])
	assert(nAPos > 0 and nAPos <= self.m_tblAllSpace[nARoom]:Size())
	assert(nBPos > 0 and nBPos <= self.m_tblAllSpace[nBRoom]:Size())

	if nARoom ~= nBRoom or nAPos ~= nBPos then
		self.m_tblAllSpace[nARoom].m_tblGrids[nAPos], self.m_tblAllSpace[nBRoom].m_tblGrids[nBPos]
			= self.m_tblAllSpace[nBRoom].m_tblGrids[nBPos], self.m_tblAllSpace[nARoom].m_tblGrids[nAPos]
		local nGridIDA = self.m_tblAllSpace[nARoom].m_tblGrids[nAPos]:GetGridID()
		local nGridIDB = self.m_tblAllSpace[nBRoom].m_tblGrids[nBPos]:GetGridID()
		if(nGridIDA) then self.m_tblGridIDIndex[nGridIDA] = {nARoom, nAPos} end
		if(nGridIDB) then self.m_tblGridIDIndex[nGridIDB] = {nBRoom, nBPos} end
	end
end

--*******************************************************************************************************
--扩展包
--*******************************************************************************************************
function CBagSpaceMgr:FetchBag(nARoom, nAPos, nBRoom, nBPos, nGridID)
	self.m_tblAllSpace[nARoom].m_tblGrids[nAPos], self.m_tblAllSpace[nBRoom].m_tblGrids[nBPos]
		= self.m_tblAllSpace[nBRoom].m_tblGrids[nBPos],self.m_tblAllSpace[nARoom].m_tblGrids[nAPos]
	self.m_tblAllSpace[nBRoom].m_tblGrids[nBPos]:SetGridID(nGridID)
	self.m_tblGridIDIndex[nGridID] = {nBRoom, nBPos}
	self.m_tblItemID_Temp = {}
end

function CBagSpaceMgr:PlaceBag(nARoom, nAPos, nBRoom, nBPos)
	self.m_tblAllSpace[nARoom].m_tblGrids[nAPos], self.m_tblAllSpace[nBRoom].m_tblGrids[nBPos]
		= self.m_tblAllSpace[nBRoom].m_tblGrids[nBPos],self.m_tblAllSpace[nARoom].m_tblGrids[nAPos]
	local nGridID = self.m_tblAllSpace[nBRoom].m_tblGrids[nBPos]:GetGridID()
	self.m_tblGridIDIndex[nGridID] = nil
	self.m_tblAllSpace[nBRoom].m_tblGrids[nBPos]:SetGridID(nil)
end

function CBagSpaceMgr:FetchAndPlaceBag(nARoom, nAPos, nBRoom, nBPos)
	self.m_tblAllSpace[nARoom].m_tblGrids[nAPos], self.m_tblAllSpace[nBRoom].m_tblGrids[nBPos]
		= self.m_tblAllSpace[nBRoom].m_tblGrids[nBPos],self.m_tblAllSpace[nARoom].m_tblGrids[nAPos]
	local nGridID = self.m_tblAllSpace[nBRoom].m_tblGrids[nBPos]:GetGridID()
	self.m_tblAllSpace[nARoom].m_tblGrids[nAPos]:SetGridID(nGridID)
	self.m_tblAllSpace[nBRoom].m_tblGrids[nBPos]:SetGridID(nil)
end

--*******************************************************************************************************

--更新图标显示
function CBagSpaceMgr:UpdateRoomPos(nRoomIndex, nPos, CoolDownType, CoolDownTime)
	--是添加附背包后扩展的格子
	if g_GameMain.m_WndMainBag.m_ctItemRoom:IsInMainBagExpandByAddBag(nRoomIndex) then
		nPos = g_GameMain.m_WndMainBag.m_ctItemRoom:GetDesPos(nRoomIndex,nPos)
	end
	
	local Lc = self:GetSpaceRelateLc(nRoomIndex)
	if  Lc~= nil then
		local nX, nY = Lc:ParsePos(nPos, Lc.m_nCol) 
		Lc:UpdateWndDisPlay(nY, nX, nRoomIndex, CoolDownType, CoolDownTime)
	end
end

