gas_require "item/item_bag/BreakItemMgrInc"
cfg_load "item/ItemBreak_Common"
function CBreakItemMgr:Ctor()
	self.m_ItemBreakComm = ItemBreak_Common
end

function CBreakItemMgr:CanBreak(ItemName,ItemType)
	local bFlag = false
	local keys = self.m_ItemBreakComm:GetKeys()
	for i, key in pairs(keys) do
		local p = self.m_ItemBreakComm(key)
		if (ItemName == p("ItemName") and ItemType == p("ItemType")) then
			return true,p
		end
	end
	return bFlag
end


function CBreakItemMgr:GetBreakProducts(ItemName,ItemType)
	local tbl = {}
	local keys = self.m_ItemBreakComm:GetKeys()
	for i, key in pairs(keys) do
		local p = self.m_ItemBreakComm(key)
		if (ItemName == p("ItemName") and ItemType == p("ItemType")) then
			local sProductInfo = p("ProductInfo")
			local ProductInfo = loadstring(" return { " .. sProductInfo .. " }")()
			local nTotalProp = 0
			for m =1,#ProductInfo do
				nTotalProp = nTotalProp + ProductInfo[m][3]
			end
			local nRandNum = math.random(nTotalProp)
			local nSum = 0
			for j =1,#ProductInfo do
				local ProductName,ProductType,Prop,MinNum,MaxNum = unpack(ProductInfo[j])
				if Prop > 0 then
					if nRandNum > nSum and nRandNum < (nSum + Prop) then
						local nNum = math.random(MinNum,MaxNum)
						if nNum > 0 then
							table.insert(tbl,{ProductName,ProductType,nNum})
							break
						end
					else
						nSum = nSum + Prop
				end
					end
			end
			break
		end
	end
	return tbl
end

