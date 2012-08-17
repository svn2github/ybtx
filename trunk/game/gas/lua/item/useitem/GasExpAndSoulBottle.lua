CExpOrSoulStorage = class ()
cfg_load "item/Exp_Soul_Bottle_Common"

local ExpOrSoulStorageDB = "ExpOrSoulStorageDB"
local uActivationType		= 1 	--激活
local uCloseType				= 2		--关闭
-----------------------------------------------------------------------
--@brief 经验瓶、魂瓶激活
function CExpOrSoulStorage.OpenExpOrSoulBottle(Conn,uRoomIndex, uPos)
	local data = {["uRoomIndex"]= uRoomIndex, ["uPos"]= uPos, ["uCharID"]= Conn.m_Player.m_uID
							,["SoulBottleInfo"] = Conn.m_Player.m_SoulBottleInfo,
							["uSoulBottleValue"] = Conn.m_Player.m_uAddSoulBottleValue,
							["uLevel"] = Conn.m_Player:CppGetLevel()}
	Conn.m_Player.m_uAddSoulBottleValue = 0										
	local function CallBack(succ,result,bottle_type,sItemName)
		if succ then
			local player = Conn.m_Player
			if result["uActivationItemId"] and result["uCloseItemId"] then
				MsgToConn(Conn,182009)
				Gas2Gac:ReturnBottleState(Conn,result["uActivationItemId"],uActivationType)
				Gas2Gac:ReturnBottleState(Conn,result["uCloseItemId"],uCloseType)
			elseif result["uActivationItemId"] then
				MsgToConn(Conn,182004)
				Gas2Gac:ReturnBottleState(Conn,result["uActivationItemId"],uActivationType)
			elseif result["uCloseItemId"] then
				MsgToConn(Conn,182008)
				Gas2Gac:ReturnBottleState(Conn,result["uCloseItemId"],uCloseType)
			end	
			if result["uCloseItemId"] then
				if bottle_type == 1 then
					player.m_SoulBottleInfo = nil
				else
					player.m_ExpBottleInfo = nil
				end	
			end
			if result["uActivationItemId"] then
				if bottle_type == 1 then
					player.m_SoulBottleValue = result["uActivationNum"]
					player.m_SoulBottleInfo = {result["uActivationItemId"],sItemName}
				else
					player.m_ExpBottleInfo = {result["uActivationItemId"],sItemName}
				end	
			end
		else
			MsgToConn(Conn,182005)
		end
	end
			
	CallAccountManualTrans(Conn.m_Account, ExpOrSoulStorageDB, "OpenExpOrSoulBottle", CallBack, data)
end
-------------------------------------------------------------------------
--@brief 魂瓶存储魂值
function CExpOrSoulStorage.StorageSoulFuc(Player,uAddSoulNum)
	local uLeftSoulCount = uAddSoulNum
	if Player.m_SoulBottleInfo then
		if not Player.m_uAddSoulBottleValue then
			Player.m_uAddSoulBottleValue = 0
		end
		local tbl = Player.m_SoulBottleInfo
		local sItemName = tbl[2]
		local uStorageNum = Exp_Soul_Bottle_Common(sItemName,"StorageNum")
		if Player.m_SoulBottleValue + uAddSoulNum < uStorageNum then
			Player.m_uAddSoulBottleValue = Player.m_uAddSoulBottleValue + uAddSoulNum
			Player.m_SoulBottleValue = Player.m_SoulBottleValue + uAddSoulNum
			Gas2Gac:ReturnExpOrSoulBottleInfo(Player.m_Conn,tbl[1],uActivationType,Player.m_SoulBottleValue)
			Gas2Gac:ShowSoulBottleAddSoulMsg(Player.m_Conn,uAddSoulNum)
			return 0
		elseif Player.m_SoulBottleValue + uAddSoulNum == uStorageNum then
			Player.m_uAddSoulBottleValue = Player.m_uAddSoulBottleValue + uAddSoulNum
			uLeftSoulCount = 0
		else
			Player.m_uAddSoulBottleValue = Player.m_uAddSoulBottleValue + (uStorageNum - Player.m_SoulBottleValue)
			uLeftSoulCount = uAddSoulNum - (uStorageNum - Player.m_SoulBottleValue)
		end
		local function CallBack(succ,result,sItemName,nRoomIndex, nPos)
			if succ then
				Player.m_SoulBottleValue = nil
				Player.m_SoulBottleInfo = nil
				Player.m_uAddSoulBottleValue = nil
				MsgToConn(Player.m_Conn,182010)
			end
		end
			
		local param = {["uSoulBottleId"] = tbl[1],["uSoulBottleValue"] = Player.m_uAddSoulBottleValue,
											["PlayerId"] = Player.m_uID,["StorageNum"] = uStorageNum	,["bottleName"] = tbl[2]}
											
		CallAccountManualTrans(Player.m_Conn.m_Account, ExpOrSoulStorageDB, "StorageSoulValue", CallBack, param)
		return uLeftSoulCount
	end
	return uLeftSoulCount
end
-------------------------------------------------------------------------
--@brief 经验瓶存储经验
function CExpOrSoulStorage.StorageExpFuc(Player,uAddSoulNum)
	local uLeftSoulCount = uAddSoulNum
	if Player.m_SoulBottleInfo then
		if not Player.m_uAddSoulBottleValue then
			Player.m_uAddSoulBottleValue = 0
		end
		local tbl = Player.m_SoulBottleInfo
		local sItemName = tbl[2]
		local uStorageNum = Exp_Soul_Bottle_Common(sItemName,"StorageNum")
		if Player.m_SoulBottleValue + uAddSoulNum < uStorageNum then
			Player.m_uAddSoulBottleValue = Player.m_uAddSoulBottleValue + uAddSoulNum
			Player.m_SoulBottleValue = Player.m_SoulBottleValue + uAddSoulNum
			Gas2Gac:ReturnExpOrSoulBottleInfo(Player.m_Conn,tbl[1],uActivationType,Player.m_SoulBottleValue)
			return 0
		elseif Player.m_SoulBottleValue + uAddSoulNum == uStorageNum then
			Player.m_uAddSoulBottleValue = Player.m_uAddSoulBottleValue + uAddSoulNum
			uLeftSoulCount = 0
		else
			Player.m_uAddSoulBottleValue = Player.m_uAddSoulBottleValue + (uStorageNum - Player.m_SoulBottleValue)
			uLeftSoulCount = uAddSoulNum - (uStorageNum - Player.m_SoulBottleValue)
		end
		local function CallBack(succ,result,sItemName,nRoomIndex, nPos)
			if succ then
				Player.m_SoulBottleValue = nil
				Player.m_SoulBottleInfo = nil
				Player.m_uAddSoulBottleValue = nil
			end
		end
			
		local param = {["uSoulBottleId"] = tbl[1],["uSoulBottleValue"] = Player.m_uAddSoulBottleValue,
											["PlayerId"] = Player.m_uID,["StorageNum"] = uStorageNum	,["bottleName"] = tbl[2]}
											
		CallAccountManualTrans(Player.m_Conn.m_Account, ExpOrSoulStorageDB, "StorageSoulValue", CallBack, param)
		return uLeftSoulCount
	end
	return uLeftSoulCount
end