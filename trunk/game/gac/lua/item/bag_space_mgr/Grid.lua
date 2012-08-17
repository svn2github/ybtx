engine_require "common/Misc/TypeCheck"

CGrid = class()

function CGrid.Init()
	local NewInstance = CGrid:new()
	NewInstance.m_bEnable = true
	NewInstance.m_Room = CList:new()
	return NewInstance
end

function CGrid:SetGridInfo(nType, sName, nBindingType, Context, nGridID)
	if( not g_ItemInfoMgr:CheckType(nType, sName) ) then
		LogErr("不存在该物品", "nType:" .. nType .. " sName:" .. sName)
		return
	end
	self.m_nGridID		= nGridID
	self.m_nType		= nType
	self.m_sName		= sName
	self.m_nBindingType	= nBindingType
	self.m_Context		= Context
end

function CGrid:Clear()
	self.m_nGridID		= nil
	self.m_nType		= nil
	self.m_sName		= nil
	self.m_nBindingType	= nil
	self.m_Context		= nil
end

function CGrid:SetGridID(nGridID)
	self.m_nGridID = nGridID
end

function CGrid:GetGridID()
	return self.m_nGridID
end

function CGrid:SetContext(Context)
	self.m_Context = Context
end

function CGrid:GetContext()
	return self.m_Context
end

function CGrid:Size()
	return self.m_Room:size()
end

function CGrid:Empty()
	return self.m_Room:empty()
end

--返回值：格子里所放的物品类型，物品的名称
function CGrid:GetType()
	if self:Empty() then
		return nil
	else
		return self.m_nType, self.m_sName, self.m_nBindingType
	end
end

function CGrid:GetItemType()
	return self.m_nType
end

function CGrid:GetItemName()
	return self.m_sName
end

function CGrid:GetBindingType()
	return self.m_nBindingType
end

function CGrid:GetFoldLimit()
	assert(not self:Empty())
	local FoldLimit = g_ItemInfoMgr:GetItemInfo(self.m_nType, self.m_sName, "FoldLimit") or 1
	return FoldLimit
end

function CGrid:GetIcon()
	if self:Empty() then
		return nil
	else
		local SmallIcon = g_ItemInfoMgr:GetItemInfo(self.m_nType, self.m_sName, "SmallIcon")
		return SmallIcon
	end
end

--得到count物品的ID
function CGrid:GetItemID(nCount)
	local item = self:GetItem()
	local tblItemInfo = {}
	local size = 0
	for v, i in self.m_Room:enum(self.m_Room:head(), self.m_Room:tail()) do
		table.insert(tblItemInfo, v:GetItemID())
		size = size + 1
		if size == nCount then
			break
		end
	end
	return tblItemInfo
end

function CGrid:Add(Item, nGridID)
	local ItemInGrid = CItemInGrid:new(Item:GetItemID(), self)
	if( self:Empty() ) then
		self:SetGridInfo(Item:GetBigID(), Item:GetIndex(), Item:GetBindingType(), Item:GetContext(), nGridID)
		self.m_Room:push_back(ItemInGrid)
		return 1
	else
		local nItemType, sItemName, nBindingType = self:GetType()
		assert(Item:GetBigID() == nItemType and Item:GetIndex() == sItemName, nItemType .. sItemName)
		if self:Size() < self:GetFoldLimit() then
			self.m_Room:push_back(ItemInGrid)
			return 1
		end
	end
	return 0
end

--获得一个物品
function CGrid:GetItem()
	if self:Empty() then
		return nil
	else
		return self.m_Room:front()
	end
end

function CGrid:CreatItemInstant()
	if self:Empty() then
		return nil
	else
		local ItemInGrid = self.m_Room:front()
		local Item = CItem:new(ItemInGrid:GetBigID(), ItemInGrid:GetIndex(), ItemInGrid:GetItemID(), ItemInGrid:GetBindingType(), ItemInGrid:GetContext())
		return Item
	end
end

--删除唯一的物品
function CGrid:DelUnique(nItemID)
	assert(IsNumber(nItemID) and nItemID > 0)
	local it = self.m_Room:head()
	local Compare = function ( a, b ) 
		if  a:GetItemID() == b then 
			return true 
		else 
			return false 
		end 
	end
	it = self.m_Room:find( nItemID, it, Compare )
	if it == self.m_Room:tail() then
		return false
	else
		self.m_Room:erase(it)
		if( self:Empty() ) then
			self:Clear()
		end
		return true
	end
end

--删除不可叠加物品
function CGrid:DelNotPile()
	assert(self:GetFoldLimit() == 1)
	return self:DelUnique(self:GetItem():GetItemID())
end

function CGrid:SetEnable(bFlag)
	self.m_bEnable = bFlag
end

function CGrid:GetEnable()
	return self.m_bEnable
end