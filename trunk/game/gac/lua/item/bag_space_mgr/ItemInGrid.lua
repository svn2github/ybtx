CItemInGrid = class()

function CItemInGrid:Ctor(nItemID, Grid)
	self.m_nItemID	= nItemID
	self.m_Grid		= Grid
end

function CItemInGrid:GetItemID()
	return self.m_nItemID
end

function CItemInGrid:GetGrid()
	return self.m_Grid
end

function CItemInGrid:GetBigID()
	return self.m_Grid.m_nType
end

function CItemInGrid:GetIndex()
	return self.m_Grid.m_sName
end

function CItemInGrid:GetBindingType()
	return self.m_Grid.m_nBindingType
end

function CItemInGrid:GetContext()
	return self.m_Grid.m_Context
end