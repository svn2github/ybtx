
local g_ItemInfoMgr		=	g_ItemInfoMgr
local LogErr = LogErr
local GetItemDynInfoBox = CreateDbBox(...)

function GetItemDynInfoBox.GetItemDynInfo(nItemID, item_type, char_id,target_id, itemIDTbl)
	local info = {}
	local EquipMgrDB = RequireDbBox("EquipMgrDB")
	local battleArrayBooksSql = RequireDbBox("BattleArraySql")
	local mail_item = RequireDbBox("MailItemDB")
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")	
	local OreMapItemDB = RequireDbBox("OreMapItemDB")	
	local PickOreItemDB = RequireDbBox("PickOreItemDB")
    if tonumber(item_type) == nil then
        LogErr("物品类型为非数字", "ItemID" .. (nItemID or "-"))
    end
	if g_ItemInfoMgr:IsStaticEquip(item_type) then		
		local equipRet = EquipMgrDB.GetEquipAllInfo( nItemID)
		info = equipRet
	elseif (g_ItemInfoMgr:IsBattleArrayBooks(item_type)) then
		--阵法书
		info = battleArrayBooksSql.SelectBattleBookByID(nItemID)
	elseif g_ItemInfoMgr:IsMailTextAttachment(item_type) then
		--邮件文本物品
		info = mail_item.GetMailTextAttachmentByID(nItemID)
	elseif g_ItemInfoMgr:IsSoulPearl(item_type) then
		local nASoulNum = g_RoomMgr.GetSoulPearlInfoByID(nItemID)
		info = {nASoulNum}		
	elseif g_ItemInfoMgr:IsOreAreaMap(item_type) then
		local sceneName = OreMapItemDB.QueryItemSceneName(nItemID)
		info = {sceneName}		
	elseif g_ItemInfoMgr:IsPickOreItem(item_type) then
		local MaxDura, CurDura = PickOreItemDB.QueryPickOreItemDura(nItemID)
		info = {MaxDura, CurDura}		
  elseif g_ItemInfoMgr:IsBoxitem(item_type) then
    local BoxitemDB = RequireDbBox("BoxitemDB")
		local openedFlag = BoxitemDB.IsBoxitemOpened(itemIDTbl, char_id)
		info = {openedFlag}
	end
	local itemBindingType = g_RoomMgr.GetItemBindingTypeByID(nItemID)
	info.ItemBindingType = itemBindingType
	local ItemMakerMgrDB = RequireDbBox("ItemMakerMgrDB")
	ItemMakerMgrDB.SendItemMakerByTblIDs(char_id,{nItemID},target_id)
	local _, item_name = g_RoomMgr.GetItemType(nItemID)
	if char_id then
		Db2CallBack("RetItemDynInfo",char_id,item_type, item_name,nItemID,info)
	end
	if  target_id  and char_id ~= target_id then
		Db2CallBack("RetItemDynInfo",target_id,item_type, item_name,nItemID,info)
	end
	return info
end

return GetItemDynInfoBox