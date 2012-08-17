
CItem = class()

function CItem:GetBigID()
	return self.m_nBigID
end

function CItem:GetIndex()
	return self.m_nIndex
end

function CItem:GetBindingType()
	return self.BindingType
end

function CItem:GetContext()
	return self.m_Context
end

function CItem:GetItemID()
	return self.m_nItemID
end

function CItem:SetContext(context)
	self.m_Context = context
end

function CItem:Ctor(nBigID, nIndex, nItemID, nBindingType, Context)
	if( not g_ItemInfoMgr:CheckType(nBigID,nIndex) ) then
		LogErr("不存在该物品", "nBigID:" ..nBigID .. " nIndex:" .. nIndex)
		return
	end
	self.m_nBigID		= nBigID
	self.m_nIndex		= nIndex
	self.m_nItemID		= nItemID
	self.BindingType	= nBindingType
	self.m_Context		= Context
end