gac_gas_require "item/item_info_mgr/ItemInfoMgr"

CBagSpace = class()

function CBagSpace:Size()
	return self.m_nSize
end

function CBagSpace:FindItem(BigID, SmallID)
	for i, v in ipairs(self.m_tblGrids) do
		local bid, sid = v:GetType()
		if bid == BigID and sid == SmallID then
			return i
		end
	end
	return nil
end

function CBagSpace:FindItemByItemID(ItemID)
	for i, v in ipairs(self.m_tblGrids) do
		local item = v:GetItem()
		if item and item:GetItemID() == ItemID then
			return i
		end
	end
	return nil    
end

function CBagSpace:GetItemCountByType(BigID, SmallID)
	assert(BigID and SmallID)
	local count = 0
	for i, v in ipairs(self.m_tblGrids) do
		local bid, sid = v:GetType()
		if bid == BigID and sid == SmallID then
			count = count + v:Size()
		end
	end
	return count
end

function CBagSpace:GetItemCount()
	local count = 0
	for i, v in ipairs(self.m_tblGrids) do
		count = count + v:Size()
	end
	return count
end

function CBagSpace:GetNotFreeGridCount()
	local count = 0
	for i, v in ipairs(self.m_tblGrids) do
		if v:Size() > 0 then
			count = count + 1
		end
	end
	return count
end

function CBagSpace:GetFreeGridCount()
	local count = 0
	for i, v in ipairs( self.m_tblGrids) do 
		if v:Size() == 0 then
			count = count + 1
		end
	end
	return count
end

--clinet 端不需要做判断，已经在服务器判断过了
function CBagSpace:AddItemByPos(nIndex, Item, nGridID)
	assert(nIndex > 0 and nIndex <= self.m_nSize)
	return self.m_tblGrids[nIndex]:Add(Item, nGridID)
end

--删除唯一id的物品，nItemID在 CItem中有检查，这里就不检查了 
function CBagSpace:DelUniqueByPos(nIndex,nItemID)
	assert(nIndex > 0 and nIndex <= self.m_nSize)
	return self.m_tblGrids[nIndex]:DelUnique(nItemID)
end

--删除不可叠加物品
function CBagSpace:DelNotPileByPos(nIndex)
	assert(nIndex > 0 and nIndex <= self.m_nSize)
	return self.m_tblGrids[nIndex]:DelNotPile()
end

function CBagSpace:GetGrid(nIndex)
	assert(nIndex > 0 and nIndex <= self.m_nSize)
	return self.m_tblGrids[nIndex]
end

--交换两个格子空间
function CBagSpace:ExChangeTwoGrid(nFirstIndex,nTwoIndex)
	assert(nFirstIndex > 0 and nFirstIndex <= self.m_nSize)
	assert(nTwoIndex > 0 and nTwoIndex <= self.m_nSize)
	if nFirstIndex == nTwoIndex then 
		return
	end
	self.m_tblGrids[nFirstIndex],self.m_tblGrids[nTwoIndex] = self.m_tblGrids[nTwoIndex],self.m_tblGrids[nFirstIndex]
end

function CBagSpace:ModifySpaceSize(newSize)
	if self.m_nSize == newSize then
		return
	end
	if self.m_nSize < newSize then	-- 直接扩容
		for i = self.m_nSize + 1, newSize do
			self.m_tblGrids[i] = CGrid.Init()
		end
	else	-- 重新排位
		for i = self.m_nSize, ( newSize+1 ), -1 do
			if not self:GetGrid(i):Empty() then
				for v = 1, newSize do
					if self:GetGrid(v):Empty() then 
						self.m_tblGrids[v], self.m_tblGrids[i] = self.m_tblGrids[i], self.m_tblGrids[v]
					end
				end
				self.m_tblGrids[i] = nil
			end
		end
	end
	self.m_nSize = newSize
end

--获得可以放置该物品的空间
function CBagSpace:GetWherePlace(nBigID,nIndex,nCount)
	local nPlaceCount = nCount or 1
	assert(IsNumber(nPlaceCount))
	
	local FoldLimit = g_ItemInfoMgr:GetItemInfo( nBigID,nIndex,"FoldLimit" ) or 1
	for n=1, self.m_nSize do
		if self.m_tblGrids[n]:Empty() then
			if FoldLimit < nPlaceCount then
				error("nCount must < FoldLimit")
			end
			return n
		else
			local nGridBigID,nGridIndex = self.m_tblGrids[n]:GetType()
			if nBigID == nGridBigID and nIndex == nGridIndex then
				if  nPlaceCount + self.m_tblGrids[n]:Size() <= FoldLimit  then
					return n
				end
			end
		end
	end
	return nil
end		


function CBagSpace.Init(nSize)
	assert(IsNumber(nSize) and nSize > 0)
	local NewInstance = CBagSpace:new()
	NewInstance.m_nSize = nSize
	NewInstance.m_tblGrids = {}
	for n=1, NewInstance.m_nSize do
		NewInstance.m_tblGrids[n] = CGrid.Init()
	end
	return NewInstance
end