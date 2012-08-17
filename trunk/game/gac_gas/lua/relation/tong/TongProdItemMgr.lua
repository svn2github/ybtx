gac_gas_require "relation/tong/TongProdItemMgrInc"
cfg_load "item/TongProdItem_Common"

function CTongProdItemInfoMgr:Ctor()
	self.tblItemInfo = TongProdItem_Common
end

function CTongProdItemInfoMgr:GetTableByType(nType)
	local tbl = {}
	local ItemNames = self.tblItemInfo:GetKeys()
	for i=1,#ItemNames do
		local ItemName = ItemNames[i]
		local p =  self.tblItemInfo(ItemName)
		if (nType == p("Type")) then
			table.insert(tbl, p)
		end
	end
	return tbl
end

function CTongProdItemInfoMgr:GetCanBeCreateBuilding()
	local tbl = {}
	local ItemNames = self.tblItemInfo:GetKeys()
	for i=1,#ItemNames do
		local ItemName = ItemNames[i]
		local p = self.tblItemInfo(ItemName)
		if (	not string.find(ItemName, "´ó±¾Óª")	and
				1 == p("Type") and p("SubType") ~= 0 and ( g_MainPlayer:CppGetBirthCamp() == p("Camp") or 0 == p("Camp") ) ) then
			local subType = p("SubType")
			if not tbl[subType] then tbl[subType] = {} end
			table.insert(tbl[subType], p)
		end
	end
	return tbl
end

function CTongProdItemInfoMgr:GetBuildNameByCamp(nCamp,sName)
	local ItemNames = self.tblItemInfo:GetKeys()
	for i=1,#ItemNames do
		local ItemName = ItemNames[i]
		if (self.tblItemInfo(ItemName,"Camp") == nCamp) and string.find(ItemName,sName) then
			return ItemName
		end
	end
end

function CTongProdItemInfoMgr:GetCampByBuildName(sName)
	local ItemNames = self.tblItemInfo:GetKeys()
	for i=1,#ItemNames do
		local ItemName = ItemNames[i]
		if ItemName == sName then
			return self.tblItemInfo(ItemName,"Camp")
		end
	end
end