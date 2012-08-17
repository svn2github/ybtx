local GasEquipIdentifyDB = "GasEquipIdentifyDB"
CGasEquipIdentify = class()

local function ArmorOrJewelryIdentify(Conn,result,Tbl,equipitemid,nBigID,nIndex,scrollRoomIndex,scrollPos)
	local player = Conn.m_Player
	if result then
		local EquipRet_New =  Tbl["EquipRet_New"]
		local EquipRet_Old =	Tbl["EquipRet_Old"] 
		local scrollId = 	Tbl["ScrollId"]
		local flag = Tbl["Flag"]
		local equip_mgr = g_GetEquipMgr()
		local new_equip,old_equip		
        local newEquipInfoTbl = EquipRet_New
        local oldEquipInfoTbl = EquipRet_Old
        local equipPart = Tbl["Part"]
		if flag and equipPart ~= 0 then
			old_equip = equip_mgr:GetEquipFromRet(equipitemid, player.m_uID, tonumber(nBigID), nIndex,oldEquipInfoTbl)
			old_equip:DisableBasePropty(player,equipPart, "ArmorOrJewelryIdentify")
		end
			
		new_equip = equip_mgr:GetEquipFromRet(equipitemid, player.m_uID, tonumber(nBigID), nIndex,newEquipInfoTbl)
		new_equip:SendEquipInfo2Conn(player.m_uID)
		if flag and equipPart ~= 0 then
			new_equip:EnableBasePropty(player,equipPart, "ArmorOrJewelryIdentify")
		end	
	end
	Gas2Gac:ReturnIdentifyResult(Conn,result)
	
end

--防具鉴定
function CGasEquipIdentify.ArmorOrJewelryIdentify(Conn,armorBaseValue,nBigID,nIndex,identify_equipTrend,identify_equipTrendValue,scrollRoomIndex,scrollPos,equipRoomIndex,equipPos,equipitemid)
	local player = Conn.m_Player
	if nil == player then
		return 
	end
	--判断是不是处于战斗状态
	if player:IsInBattleState() then
		MsgToConn(Conn,170009)
		return
	end
	--判断是不是处于缴械状态
	if player:IsInForbitUseWeaponState() then
		MsgToConn(Conn,170010)
		return
	end
	
	local equipType = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"EquipType")
	local equipPart = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"EquipPart")
	local attrNameTbl = GetEquipOutputAttrName(nBigID,nIndex)
	
	local data = {
								["attrNameTbl"] = attrNameTbl,
								["identify_equipTrend"] = identify_equipTrend,
								["armorBaseValue"] = armorBaseValue,
								["equipType"] = equipType,
								["equipPart"] = equipPart,
								["scrollRoomIndex"] = scrollRoomIndex,
								["scrollPos"] = scrollPos,
								["equipRoomIndex"] = equipRoomIndex,
								["equipPos"] = equipPos,
								["equipitemid"] = equipitemid,
								["identify_equipTrendValue"] = identify_equipTrendValue,
								["uCharId"] = player.m_uID,
								["sceneName"] = player.m_Scene.m_SceneName
	}
	
	local function CallBack(result,Tbl)
		if Tbl and next(Tbl) then
			equipitemid = Tbl["equipId"]
		end
		ArmorOrJewelryIdentify(Conn,result,Tbl,equipitemid,nBigID,nIndex,scrollRoomIndex,scrollPos)
	end
	CallAccountManualTrans(Conn.m_Account, GasEquipIdentifyDB, "ArmorOrJewelryIdentify", CallBack, data)
end
-----------------------------------------------------------------------------------------------------
--武器鉴定
local function WeaponIdentify(Conn,result,Tbl,equipitemid,nBigID,nIndex,scrollRoomIndex,scrollPos)
	local player = Conn.m_Player
	if result then
		local EquipRet_New =  Tbl["EquipRet_New"]
		local EquipRet_Old =	Tbl["EquipRet_Old"] 
		local scrollId = 	Tbl["ScrollId"]
		local flag = Tbl["Flag"]
		local equip_mgr = g_GetEquipMgr()
		local new_equip,old_equip		
		local equipPart = Tbl["Part"]
		
		if flag and equipPart ~= 0 then
			old_equip = equip_mgr:GetEquipFromRet(equipitemid, player.m_uID, tonumber(nBigID), nIndex,EquipRet_Old)
			old_equip:DisableBasePropty(player,equipPart, "WeaponIdentify")
		end
			
		new_equip = equip_mgr:GetEquipFromRet(equipitemid, player.m_uID, tonumber(nBigID), nIndex,EquipRet_New)
		new_equip:SendEquipInfo2Conn(player.m_uID)
		if flag and equipPart ~= 0 then
			new_equip:EnableBasePropty(player,equipPart, "WeaponIdentify")
		end	
	end
	Gas2Gac:ReturnIdentifyResult(Conn,result)
end

--武器鉴定
function CGasEquipIdentify.WeaponIdentify(Conn,curLevel,nBigID,nIndex,identify_equipTrend,identify_equipTrendValue,scrollRoomIndex,scrollPos,equipRoomIndex,equipPos,itemid,equipPart)
	local player = Conn.m_Player
	if nil == player then
		return 
	end

	--判断是不是处于战斗状态
	if player:IsInBattleState() then
		MsgToConn(Conn,170009)
		return
	end
	--判断是不是处于缴械状态
	if player:IsInForbitUseWeaponState() then
		MsgToConn(Conn,170010)
		return
	end
	
	local equipType = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"EquipType")

	local data = {
								["curLevel"] = curLevel,
								["identify_equipTrend"] = identify_equipTrend,
								["equipType"] = equipType,
								["equipPart"] = equipPart,
								["scrollRoomIndex"] = scrollRoomIndex,
								["scrollPos"] = scrollPos,
								["equipRoomIndex"] = equipRoomIndex,
								["equipPos"] = equipPos,
								["equipitemid"] = itemid,
								["identify_equipTrendValue"] = identify_equipTrendValue,
								["uCharId"] = player.m_uID,
								["sceneName"] = player.m_Scene.m_SceneName
	}
	local function CallBack(result,Tbl)
		if Tbl and next(Tbl) then
			itemid = Tbl["equipId"]
		end
		WeaponIdentify(Conn,result,Tbl,itemid,nBigID,nIndex,scrollRoomIndex,scrollPos)
	end
	CallAccountManualTrans(Conn.m_Account, GasEquipIdentifyDB, "WeaponIdentify", CallBack, data)
end
