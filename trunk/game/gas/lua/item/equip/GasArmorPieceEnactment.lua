CGasArmorPieceEnactment = class()

local function ArmorPieceEnactment(Conn,result,Tbl,equipitemid,nBigID,nIndex,armorPieceRoomIndex,armorPiecePos)
	local player = Conn.m_Player
	if not IsCppBound(player) then
		return
	end
	if result then
		local EquipRet_New =  Tbl["EquipRet_New"]
		local EquipRet_Old =	Tbl["EquipRet_Old"] 
		local armorPieceId = 	Tbl["armorPieceId"]
		local flag = Tbl["Flag"]
		local cost = Tbl["msgId"]
		local equip_mgr = g_GetEquipMgr()
		local new_equip,old_equip
		local equipPart = Tbl["equip_part"]
		local newEquipInfoTbl = EquipRet_New
		local oldEquipInfoTbl = EquipRet_Old
		if flag and equipPart ~= 0 then
			old_equip = equip_mgr:GetEquipFromRet(equipitemid, player.m_uID, tonumber(nBigID), nIndex,oldEquipInfoTbl)
			old_equip:DisableEquipPieceEnactment(player,equipPart, "ArmorPieceEnactment")
		end
			
		new_equip = equip_mgr:GetEquipFromRet(equipitemid, player.m_uID, tonumber(nBigID), nIndex,newEquipInfoTbl)
		new_equip:SendEquipInfo2Conn(player.m_uID)
		if flag and equipPart ~= 0 then
			new_equip:EnableEquipPieceEnactment(player,equipPart, "ArmorPieceEnactment")
			CGasFightingEvaluation.UpdateFightingEvaluationInfo(player)
		end	
	else 
		if Tbl and IsNumber(Tbl) then
			MsgToConn(Conn, Tbl)
		end
	end
	Gas2Gac:ReturnArmorPieceEnactment(Conn,result)
end

--@brief 护甲片设定
function CGasArmorPieceEnactment.ArmorPieceEnactment(Conn,nBigID,nIndex,armorPieceRoomIndex,armorPiecePos,equipRoomIndex,equipPos,itemid,update_index,itemType,ArmorPieceName)
	local player = Conn.m_Player	
	if nil == player then
		return 
	end

	--判断是不是处于战斗状态
	if player:IsInBattleState() then
		MsgToConn(Conn,171007)
		return
	end
	--判断是不是处于缴械状态
	if player:IsInForbitUseWeaponState() then
		MsgToConn(Conn,171008)
		return
	end
	
	local data = {
								["armorPieceRoomIndex"] = armorPieceRoomIndex,
								["armorPiecePos"] = armorPiecePos,
								["equipRoomIndex"] = equipRoomIndex,
								["equipPos"] = equipPos,
								["equipitemid"] = itemid,
								["nBigID"] = nBigID,
								["nIndex"] = nIndex,
								["update_index"] = update_index,
								["itemType"] = itemType,
								["ArmorPieceName"] = ArmorPieceName,
								["uCharId"] = player.m_uID,
								["sceneName"] = player.m_Scene.m_SceneName
	}
	local function CallBack(result,Tbl)
		if Tbl and IsTable(Tbl) and next(Tbl) then
			equipitemid = Tbl["equipId"]
		end
		ArmorPieceEnactment(Conn,result,Tbl,equipitemid,nBigID,nIndex,armorPieceRoomIndex,armorPiecePos)
	end
	CallAccountManualTrans(Conn.m_Account, "GasArmorPieceEnactmentDB", "ArmorPieceEnactment", CallBack, data)
end
