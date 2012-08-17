
local FromPlace ={}
FromPlace.Bag =1			--从包裹中取得的
FromPlace.StatusWnd =2		--从主角属性面板取得的

CGasEquipSmeltSoulScroll = class()

local function RetGetSoulPearlFromEquip(suc, result, Conn, fromPlace, equipId,eEquipPart)
	local pearlRoomIndex 		= result["PearlRoomIndex"]
	local pearlPos				= result["PearlPos"]
	local create_soulpearl_id	= result["CreatePearlID"]
	local soulpearlType			= result["PearlType"]
	local soulpearlName			= result["PearlName"]
	local soulpearlInfo 		= result["PearlInfo"]
	if suc then
		if fromPlace == FromPlace.Bag then		--如果是从包裹中取得的装备，清除其在客户端的显示
		else
			local data = result["EquipData"]
			DelEquip(Conn, Conn.m_Player, eEquipPart, data)
		end

	end
end


local function EquipSmeltSoul(Conn,succ,Tbl,equipId,nBigID,nIndex,SmeltSoulScrollRoomIndex,SmeltSoulScrollPos,fromPlace)
	local player = Conn.m_Player
	if succ then
		local smeltSoulScrollId = Tbl["smeltSoulScrollId"]
		local flag = Tbl["Flag"]
		local result = Tbl["result"]
		local b_Flag = Tbl["b_Flag"]
		local equip_mgr = g_GetEquipMgr()
		if b_Flag then
		    local eEquipPart = Tbl["equip_part"]
			RetGetSoulPearlFromEquip(b_Flag, result, Conn, fromPlace, equipId,eEquipPart)
		end
	else 
		if Tbl and IsNumber(Tbl) then
			MsgToConn(Conn, Tbl)
		end
		return
	end
	MsgToConn(Conn,194006)
end

--@brief 装备炼化魂卷轴
function CGasEquipSmeltSoulScroll.EquipSmeltSoul(Conn,nBigID,nIndex,SmeltSoulScrollRoomIndex,SmeltSoulScrollPos,equipRoomIndex,equipPos,itemid,itemType,SmeltSoulScrollName,fromPlaceType,eEquipPart)
	local player = Conn.m_Player	
	if nil == player then
		return 
	end

	--判断是不是处于战斗状态
	if player:IsInBattleState() then
		MsgToConn(Conn,194004)
		return
	end
	--判断是不是处于缴械状态
	if player:IsInForbitUseWeaponState() then
		MsgToConn(Conn,194005)
		return
	end
	
	local data = {
								["SmeltSoulScrollRoomIndex"] = SmeltSoulScrollRoomIndex,
								["SmeltSoulScrollPos"] = SmeltSoulScrollPos,
								["equipRoomIndex"] = equipRoomIndex,
								["equipPos"] = equipPos,
								["equipitemid"] = itemid,
								["nBigID"] = nBigID,
								["nIndex"] = nIndex,
								["itemType"] = itemType,
								["SmeltSoulScrollName"] = SmeltSoulScrollName,
								["uCharId"] = player.m_uID,
								["fromPlaceType"] = fromPlaceType,
								["Class"] = Conn.m_Player:CppGetClass(),
								["eEquipPart"] = eEquipPart,
								["sceneName"] = player.m_Scene.m_SceneName
	}
	local function CallBack(succ,Tbl)
		if Tbl and IsTable(Tbl) and next(Tbl) then
			equipitemid = Tbl["equipId"]
		end
		EquipSmeltSoul(Conn,succ,Tbl,equipitemid,nBigID,nIndex,SmeltSoulScrollRoomIndex,SmeltSoulScrollPos,fromPlaceType)
	end
	CallAccountManualTrans(Conn.m_Account, "GasEquipSmeltSoulScrollDB", "EquipSmeltSoul", CallBack, data)
end
