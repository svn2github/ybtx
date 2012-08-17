CGetItemDynInfo = class ()

function CGetItemDynInfo.RetItemDynInfo(CharID, nBigID, nIndex, nItemID, itemInfoTbl)
	local Player = g_GetPlayerInfo(CharID)
	if not (Player and IsCppBound(Player)) then return end
	local Conn = Player.m_Conn
	if g_ItemInfoMgr:IsStaticEquip(nBigID) then
		local equip_mgr = g_GetEquipMgr()
		local equip_info = itemInfoTbl
		local equip = equip_mgr:GetEquipFromRet(nItemID,CharID, tonumber(nBigID), nIndex, equip_info)
		equip:SendEquipInfo2Conn(CharID)
	elseif g_ItemInfoMgr:IsBattleArrayBooks(nBigID) then
		--阵法书
		Gas2Gac:RetCommonBattleArrayBookInfo(Conn, itemInfoTbl[1],itemInfoTbl[2],itemInfoTbl[3],itemInfoTbl[4],itemInfoTbl[5],itemInfoTbl[6])
	
	elseif g_ItemInfoMgr:IsMailTextAttachment(nBigID) then
		--邮件文本物品
		local mail_title,sender_name = itemInfoTbl[1], itemInfoTbl[2]
		Gas2Gac:RetMailTextAttachmentInfo(Conn,nItemID,mail_title,sender_name)
	
	elseif g_ItemInfoMgr:IsSoulPearl(nBigID) then
		--魂珠
		local nASoulNum = itemInfoTbl[1]
		Gas2Gac:RetSoulPearlItemInfo(Conn, nItemID, nASoulNum)
		
	elseif g_ItemInfoMgr:IsOreAreaMap(nBigID) then
		--矿藏图
		local isImmeEnter = OreAreaMap_Common(nIndex,"ImmeEnter")
		if isImmeEnter == 0 then
			local sceneName = itemInfoTbl[1]
			if sceneName then
				Gas2Gac:RetOreMapItemInfo(Conn, nItemID, sceneName)
			end
		end
	elseif g_ItemInfoMgr:IsPickOreItem(nBigID) then
		--采矿工具
		local MaxDura, CurDura = itemInfoTbl[1], itemInfoTbl[2]
		Gas2Gac:RetPickOreItemInfo(Conn, nItemID, MaxDura, CurDura)

    elseif g_ItemInfoMgr:IsBoxitem(nBigID) then
	    local boxitemOpenedFlag = 	itemInfoTbl[1]
	    Gas2Gac:RetBoxitemOpened(Conn, nItemID, boxitemOpenedFlag)
	end	
	
	--普通物品
	Gas2Gac:RetItemTypeInfo(Conn, nItemID, nBigID, nIndex, itemInfoTbl.ItemBindingType)
end