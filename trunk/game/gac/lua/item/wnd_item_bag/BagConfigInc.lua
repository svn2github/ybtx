gac_gas_require "item/store_room_cfg/StoreRoomCfg"


--客户端专用的背包附加槽RoomIndex
g_AddRoomIndexClient = 
{
	PlayerExpandBag = -1,
	PlayerExpandDepot = -2,
}
function g_Client_GetLocalSlotPos(GlobalPos)
	assert(GlobalPos)
	if GlobalPos >= g_SlotIndex.FirstBagSlot and GlobalPos <= g_SlotIndex.LastBagSlot then
		return GlobalPos
	elseif GlobalPos >= g_SlotIndex.FirstDepotSlot and GlobalPos <= g_SlotIndex.LastDepotSlot then
		return GlobalPos - g_SlotIndex.LastBagSlot
	elseif GlobalPos >= g_SlotIndex.FirstCofferSlot and GlobalPos <= g_SlotIndex.LastCofferSlot then
		return GlobalPos - g_SlotIndex.LastDepotSlot
	end
end

--通过背包slot的来获得附栏的roomindex
function g_Client_GetSlotRoomIndexBySlot(nSlot)
	if nSlot >= g_SlotIndex.FirstBagSlot and nSlot <= g_SlotIndex.LastBagSlot then
		return g_AddRoomIndexClient.PlayerExpandBag
	elseif nSlot >= g_SlotIndex.FirstDepotSlot and nSlot <= g_SlotIndex.LastDepotSlot then
		return g_AddRoomIndexClient.PlayerExpandDepot
	end
	assert(false)
end

--判断所给的巢是否正确，只有仓库，背包有附栏，以后的佣兵另外写一个
function g_Client_IsSlotIndex( nRoom )
	if nRoom ~= g_AddRoomIndexClient.PlayerExpandBag and nRoom ~= g_AddRoomIndexClient.PlayerExpandDepot then
		return false
	else
		return true
	end
end



-- 小图标的配置 
g_ItemSmallIcon = {}
g_ItemSmallIcon.TexWidth = 256
g_ItemSmallIcon.TexHeight = 256
g_ItemSmallIcon.GridWidth = 29
g_ItemSmallIcon.GridHeight = 29