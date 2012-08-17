cfg_load "item/ItemUse_Common"
gac_gas_require "activity/item/ItemUseInfoMgrInc"

function CItemUseInfoMgr:Ctor()
	local function GetNumberCol(item)
		if item and item ~= "" then
			item = tonumber(item)
		end
		return item
	end
	local function GetItemTbl(item)
		local tbl = loadstring("return" .. item)()
		return tbl
	end
	local function GetItemMultiTbl(item)
		local tbl = loadstring("return {"..item.."}")()
		return tbl
	end
	self.m_ItemUseInfo = {}
	AddCheckLeakFilterObj(self.m_ItemUseInfo)
	
	local ItemNameKeys = ItemUse_Common:GetKeys()
	for _, ItemName in pairs(ItemNameKeys) do
		local ItemUseInfo = {}
		ItemUseInfo.ItemName = ItemUse_Common(ItemName,"ItemName")
		ItemUseInfo.BigID = ItemUse_Common(ItemName,"BigID")
		ItemUseInfo.CoolDownTime = GetNumberCol(ItemUse_Common(ItemName,"CoolDownTime"))
		ItemUseInfo.UseRange = GetNumberCol(ItemUse_Common(ItemName,"UseRange"))
		ItemUseInfo.CastType = GetItemTbl(ItemUse_Common(ItemName,"CastType"))
		ItemUseInfo.ItemConsume = GetItemMultiTbl(ItemUse_Common(ItemName,"ItemConsume"))
		ItemUseInfo.GetItem = GetItemMultiTbl(ItemUse_Common(ItemName,"GetItem"))
		ItemUseInfo.InfluenceTarget = GetItemTbl(ItemUse_Common(ItemName,"InfluenceTarget"))
		ItemUseInfo.TargetStateRequire = GetItemTbl(ItemUse_Common(ItemName,"TargetStateRequire"))
		ItemUseInfo.AreaLimit = GetItemTbl(ItemUse_Common(ItemName,"AreaLimit"))
		ItemUseInfo.effect = ItemUse_Common(ItemName,"effect")
		ItemUseInfo.Arg = GetItemMultiTbl(ItemUse_Common(ItemName,"Arg"))
		ItemUseInfo.QuestRequire = GetItemTbl(ItemUse_Common(ItemName,"QuestRequire"))
		ItemUseInfo.HorseState = ItemUse_Common(ItemName,"HorseState")
		ItemUseInfo.EffectName = GetItemMultiTbl(ItemUse_Common(ItemName,"EffectName"))
		self.m_ItemUseInfo[ItemName] = ItemUseInfo
	end 
end

g_ItemUseInfoMgr = CItemUseInfoMgr:new()

function CItemUseInfoMgr:GetItemUseInfo(ItemName, ColName)
	if ColName then
		return self.m_ItemUseInfo[ItemName][ColName]
	end
	return self.m_ItemUseInfo[ItemName]
end