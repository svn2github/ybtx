
CPutItemsToBag=class()

function CPutItemsToBag.RetItemRoom(nCharID,nRoomIndex)
	Gas2GacById:RetItemRoom(nCharID,nRoomIndex)
end

function CPutItemsToBag.RetAddItemID(nCharID,nItemID)
	Gas2GacById:RetAddItemToGrid( nCharID,nItemID)
end

function CPutItemsToBag.RetItemMakerName(nCharID,tbl_res)
	for i =1,tbl_res:GetRowNum() do
		Gas2GacById:RetItemMakerName(nCharID,tbl_res(i,2),tbl_res(i,1))
	end
end

function CPutItemsToBag.RetAddItemInfoEnd(tbl_res,nCharID)
	if not tbl_res then return end
	
	if(table.getn(tbl_res)==0) then return end

	local Type,name = tbl_res.m_nBigID,tbl_res.m_nIndex
	local nLifeTime = tbl_res.m_nLeftTime
	local binding_type = tbl_res.m_nBindType
	local item_type=tonumber(Type)
	local DynaInfo = tbl_res.m_tblDynaInfo
	for j=1,#tbl_res do
		local itemid = tbl_res[j]
		Gas2GacById:RetAddItemToGrid(nCharID,itemid)
		if g_ItemInfoMgr:IsStaticEquip(item_type) then
					local equip_mgr = g_GetEquipMgr()
					local equip = equip_mgr:GetEquipFromRet(itemid, nCharID, item_type, name, DynaInfo)
					equip:SendEquipInfo2Conn(nCharID)	
			elseif g_ItemInfoMgr:IsMailTextAttachment(item_type) then
				--邮件文本物品
				local mail_title,sender_name = DynaInfo[1],DynaInfo[2]
				Gas2GacById:RetMailTextAttachmentInfo(nCharID,itemid,mail_title,sender_name)
			elseif g_ItemInfoMgr:IsSoulPearl(item_type) then
				--魂珠
				local nASoulNum =	DynaInfo[1]
				Gas2GacById:RetSoulPearlItemInfo(nCharID,itemid,nASoulNum)
		 	elseif g_ItemInfoMgr:IsBoxitem(nBigID) then
        local boxitemOpenedFlag = DynaInfo[1]
        Gas2GacById:RetBoxitemOpened(nCharID, itemid, boxitemOpenedFlag)
			end
			Gas2GacById:RetItemLeftTime(nCharID,itemid,nLifeTime)
	end
	Gas2GacById:RetAddItemToGridEnd(nCharID,tbl_res.m_nGridID,tbl_res.m_nPos,item_type,name,binding_type)
	local Player = g_GetPlayerInfo(nCharID)
	if Player and IsCppBound(Player) then
		local Conn = Player.m_Conn
		Gas2Gac:RetPlayerItemSound(Conn,item_type,name)
	end
end

function CPutItemsToBag.RetAddItemToGridEndEx(nCharID,nGridID,nPos)
	Gas2GacById:RetAddItemToGridEndEx(nCharID,nGridID,nPos)
end

function CPutItemsToBag.RetRefreshBag(nCharID)
	Gas2GacById:RetRefreshBag(nCharID)
end

local function GetIntegralDuraValue(curDuraValue)
    local inteDuraValue = curDuraValue
    if curDuraValue >0 and curDuraValue < 1 then
        inteDuraValue = 1
    else
        inteDuraValue = tonumber(string.format("%.0f", curDuraValue))
    end
    return inteDuraValue
end

local function DestoryPet(Player, item_name, flag)
	local itemInfo = g_ItemUseInfoMgr:GetItemUseInfo(item_name)
	local petName = nil
	if itemInfo == nil then
		LogErr("ItemInfo为nil","物品名为:"..item_name)
	end
	if itemInfo ~= nil and itemInfo.Arg and itemInfo.Arg[1] then
		petName= itemInfo.Arg[1][1]
	end
	local LittlePet = Player:GetServantByType(ENpcType.ENpcType_LittlePet)
	if IsServerObjValid(LittlePet) and IsCppBound(LittlePet) then
		local name = LittlePet.m_Properties:GetCharName()
		if petName == name then 
			if flag then
				MsgToConnById(Player.m_uID, 420002)
			end
			Player:RemoveServant(LittlePet)
		end
	end
end

function CPutItemsToBag.RetInitBagItem(Conn,ItemInfo, Player)
		local BaseInfo,EquipInfo,bf_book = ItemInfo["ItemBaseInfo"],ItemInfo["equip_info"],ItemInfo["bf_book"]
		local mail_item,spearl_info = ItemInfo["mail_item"],ItemInfo["spearl_info"]
		local oremap_info = ItemInfo["oremap_info"]
		local item_maker = ItemInfo["item_maker"]
		local item_life = ItemInfo["item_life"]
		local pickore_info = ItemInfo["pickore_info"]
		local ExpAndSoulBottleInfo = ItemInfo["ExpAndSoulBottleInfo"]
		for i =1,item_maker:GetRowNum() do
			local maker_name,item_id = item_maker(i,1),item_maker(i,2)
			Gas2Gac:RetItemMakerName(Conn,maker_name,item_id)
		end
		for i =1,#item_life do
			local item_id,left_time, item_type, item_name = item_life[i][1],item_life[i][2],item_life[i][3],item_life[i][4]
			Gas2Gac:RetItemLeftTime(Conn,item_id,left_time)
			if left_time <= 0 and item_type == 40 then
				DestoryPet(Player, item_name, true)
			end
		end
		--装备动态信息
		for i =1,(EquipInfo["WeaponDynInfo"]):GetRowNum() do
			local infoi = EquipInfo["WeaponDynInfo"](i) --武器
			local inteDuraValue = GetIntegralDuraValue(infoi(11))
			Gas2Gac:RetCommonWeaponInfo(Conn,infoi(1),infoi(2),infoi(3),infoi(4),infoi(5),infoi(6),infoi(7),infoi(8),infoi(9),infoi(10),inteDuraValue, infoi(12))
		end
		for i =1,(EquipInfo["ArmorDynInfo"]):GetRowNum() do
			local infoi = EquipInfo["ArmorDynInfo"](i)	--防具
			local inteDuraValue = GetIntegralDuraValue(infoi(12))
			Gas2Gac:RetCommonArmorInfo(Conn,infoi(1),infoi(2),infoi(3),infoi(4),infoi(5),infoi(6),infoi(7),infoi(8),infoi(9),infoi(10),infoi(11),inteDuraValue, infoi(13))
		end
		for i =1,EquipInfo["ShieldDynInfo"]:GetRowNum() do
			local infoi = EquipInfo["ShieldDynInfo"](i)		--盾牌
			local inteDuraValue = GetIntegralDuraValue(infoi(12))
			Gas2Gac:RetCommonShieldInfo(Conn,infoi(1),infoi(2),infoi(3),infoi(4),infoi(5),infoi(6),infoi(7),infoi(8),infoi(9),infoi(10),infoi(11),inteDuraValue, infoi(13))
		end
		for i =1,EquipInfo["RingDynInfo"]:GetRowNum() do			--戒指
			local infoi = EquipInfo["RingDynInfo"](i)
			local inteDuraValue = GetIntegralDuraValue(infoi(10))
			Gas2Gac:RetCommonRingInfo(Conn,infoi(1),infoi(2),infoi(3),infoi(4),infoi(5),infoi(6),infoi(7),infoi(8),infoi(9),inteDuraValue, infoi(11))
		end

		for i =1, EquipInfo["EquipIntenInfo"]:GetRowNum() do
		    local infoi = EquipInfo["EquipIntenInfo"](i)
			Gas2Gac:RetEquipIntensifyInfo(Conn,infoi(1),infoi(2),infoi(3),infoi(4),infoi(5),infoi(6),infoi(7),infoi(8),infoi(9),infoi(10),infoi(11)) 
		end
		--装备强化(4~9阶段属性)信息
		for i =1,EquipInfo["EquipIntenAddInfo"]:GetRowNum() do			
			local infoi = EquipInfo["EquipIntenAddInfo"](i)
			Gas2Gac:RetEquipAddAttrInfo(Conn,infoi(1),infoi(2),infoi(3),infoi(4),infoi(5),infoi(6),infoi(7),infoi(8),infoi(9),infoi(10),infoi(11),infoi(12),infoi(13))
		end
		--装备进阶属性
		for i=1, EquipInfo["EquipAdvanceInfo"]:GetRowNum() do
            local infoi = EquipInfo["EquipAdvanceInfo"](i)
            Gas2Gac:RetEquipAdvanceInfo(Conn,infoi(1),infoi(2),infoi(3),infoi(4),infoi(5),infoi(6),infoi(7),infoi(8),infoi(9),infoi(10),infoi(11),infoi(12),infoi(13),infoi(14),infoi(15), infoi(16),infoi(17),infoi(18),infoi(19), infoi(20))
		end
		
		if #EquipInfo["EnactmentInfo"] > 0 then
			for i = 1,#EquipInfo["EnactmentInfo"] do
				local infoi = EquipInfo["EnactmentInfo"][i]
				Gas2Gac:RetEquipEnactmentInfo(Conn,infoi[1],infoi[2][1][1],infoi[2][1][2],infoi[2][2][1],infoi[2][2][2],infoi[2][3][1],infoi[2][3][2],infoi[2][4][1],infoi[2][4][2])
			end
		end
				
		local equipSuperaddInfoTbl = EquipInfo["EquipSuperaddInfo"]
		for i=1,  #equipSuperaddInfoTbl do
            Gas2Gac:RetEquipSuperaddRate(Conn, equipSuperaddInfoTbl[i][1], equipSuperaddInfoTbl[i][2], equipSuperaddInfoTbl[i][3]) 
		end
		
		--阵法书
		for j =1, bf_book:GetRowNum() do
			Gas2Gac:RetCommonBattleArrayBookInfo(Conn,bf_book(i,1),bf_book(i,2),bf_book(i,3),bf_book(i,4),bf_book(i,5),bf_book(i,6))
		end
		--邮件文本物品
		for j =1, mail_item:GetRowNum() do
			Gas2Gac:RetMailTextAttachmentInfo(Conn,mail_item(j,1),mail_item(j,2),mail_item(j,3))
		end
		--魂珠
		for j =1, spearl_info:GetRowNum() do
			Gas2Gac:RetSoulPearlItemInfo(Conn,spearl_info(j,1),spearl_info(j,2))
		end
		--矿藏图
		for j =1, oremap_info:GetRowNum() do
			local ItemId, sceneName = oremap_info(j,1),oremap_info(j,2)
			if sceneName == nil then
				sceneName = ""
			end
			Gas2Gac:RetOreMapItemInfo(Conn, ItemId, sceneName)
		end
		--采矿工具
		for j =1, pickore_info:GetRowNum() do
			local ItemId, MaxDura, CurDura = pickore_info(j,1),pickore_info(j,2),pickore_info(j,3)
			Gas2Gac:RetPickOreItemInfo(Conn, ItemId, MaxDura, CurDura)
		end
		
		--物品基本信息
		for i,v in pairs(BaseInfo) do
			local room_index = i
			local item_info = v
			Gas2Gac:RetItemRoom(Conn,room_index)
			for j=1,#item_info do
				for m =1,#item_info[j] do
					Gas2Gac:RetAddItemToGrid(Conn,item_info[j][m])
				end
				Gas2Gac:RetAddItemToGridEnd(Conn,item_info[j].m_nGridID,item_info[j].m_nPos,item_info[j].m_nBigID,item_info[j].m_sName,item_info[j].m_nBind)
			end
		end
		local opendBoxItemInfoCount  = ItemInfo["opendBoxitemInfo"]:GetRowNum()
		local pendBoxItemInfo = ItemInfo["opendBoxitemInfo"]
		for i=1, opendBoxItemInfoCount do
		    Gas2Gac:RetBoxitemOpened(Conn, pendBoxItemInfo(i,1),  true)
	  end
		
		--装备分解熟练度
		Gas2Gac:InitBreakItemExp(Conn,ItemInfo["BreakExp"])
		--经验瓶、魂瓶
		for i = 1,ExpAndSoulBottleInfo:GetRowNum() do
			local itemid ,uState,uStorageNum = ExpAndSoulBottleInfo(i,1),ExpAndSoulBottleInfo(i,2),ExpAndSoulBottleInfo(i,3)
			Gas2Gac:ReturnExpOrSoulBottleInfo(Conn,itemid ,uState,uStorageNum)
		end

end


function CPutItemsToBag.RetDelPet(nCharID,sItemName,nItemType, flag)
	if nItemType ~= 40 then
		return
	end
	local Player = g_GetPlayerInfo(nCharID)
	if not IsCppBound(Player) then
		return
	end
	DestoryPet(Player, sItemName, flag)
end
-------------------------------------------------------------------------------------

function CPutItemsToBag.RetDelItemByPos(nCharID,nItemID,nRoomIndex,nPos,bFlag)
	Gas2GacById:RetDelItemFromGrid(nCharID, nItemID, nRoomIndex, nPos,bFlag)
end

function CPutItemsToBag.ResetRoomPosByGridID(nCharID,nGridID,nRoomIndex,nPos)
	Gas2GacById:RetMoveItemByGridID(nCharID,nGridID,nRoomIndex,nPos)
end

function CPutItemsToBag.GetItemFromA2B(nCharID,AllItem,nARoom,nAPos,nBGridID,nBRoom,nBPos)
	Gas2GacById:RetItemRoom(nCharID,nBRoom)
	for i=1,#AllItem do
		Gas2GacById:RetAddItemToGrid( nCharID,AllItem[i])
	end
	Gas2GacById:RetAddItemToGridEndEx(nCharID,nBGridID,nBPos)
	for i=1,#AllItem do
		Gas2GacById:RetDelItemFromGrid(nCharID, AllItem[i],nARoom,nAPos,false)
	end
	Gas2GacById:RetRefreshBag(nCharID)
end

function CPutItemsToBag.UpdateItemBindingType(nCharID,itemInfo)
	for j =1,#itemInfo[1] do
		local nItemID = itemInfo[1][j]
		Gas2GacById:SetItemBindingTypeByID(nCharID,nItemID,2)
	end
end
-------------------------------------------------------------------------------------
