gas_require "log_mgr/CLogMgr"
local OperateRoomRpcDB = "OperateRoomRpcDB"

CRoomRpcMgr = class()

--【根据位置删除物品】
local function DelItemByPos( Connection, nRoomIndex,nPos,nCount,delType)
	local Player = Connection.m_Player
	if not (Player and IsCppBound(Player) and IsCppBound(Connection)) then return end
	local nCharID = Player.m_uID
	
	local function CallBack(result,ItemName,DecItemVarTbl)
		if not result then
			return
		end
		CRoleQuest.DecreQuestItem_DB_Ret(Player,ItemName, DecItemVarTbl[2],DecItemVarTbl[3])
		if not IsTable(result) then
			Gas2GacById:RetDelItemError(nCharID, result)
		end
	end
	
	local data = {}
	data.nCharID	= nCharID
	data.nRoomIndex = nRoomIndex
	data.nPos = nPos
	data.nCount = nCount
	data.sceneName = Player.m_Scene.m_SceneName
	data.delType = delType
	CallAccountManualTrans(Connection.m_Account, OperateRoomRpcDB, "DelItemByPos", CallBack, data)
end
----------------------------------------------------------------------------------------------------

--【拆分物品】
function CRoomRpcMgr.SplitItem( Connection, nARoom,nAPos,nBRoom,nBPos,nCount)
	local Player = Connection.m_Player
	if not (Player and IsCppBound(Player) and IsCppBound(Connection)) then 
		Gas2Gac:RetSplitItemError(Connection,res,nARoom,nAPos,nBRoom,nBPos)
		return 
	end
	if Player.m_ItemBagLock then
		MsgToConn(Connection,160011)
		Gas2Gac:RetSplitItemError(Connection,res,nARoom,nAPos,nBRoom,nBPos)
		return
	end
	
	local nCharID = Player.m_uID
	
	local function CallBack(res,itemInfo)
		if not res then return end
		if IsTable(res) then
			Gas2GacById:RetSplitItemEnd(nCharID,nARoom,nAPos,nBRoom,nBPos)
		else
			Gas2GacById:RetSplitItemError(nCharID,res,nARoom,nAPos,nBRoom,nBPos)
		end
	end
	
	local data = {}
	data.nCharID	= nCharID
	data.nARoom = nARoom
	data.nAPos = nAPos
	data.nBRoom = nBRoom
	data.nBPos = nBPos
	data.nCount = nCount
	local nRes = CallAccountManualTrans(Connection.m_Account, OperateRoomRpcDB, "SplitItem", CallBack, data)
	if 1 == nRes then
		Gas2GacById:RetSplitItemError(nCharID,res,nARoom,nAPos,nBRoom,nBPos)
	end
end
-----------------------------------------------------------------------------------------------------

--【移动物品】
function CRoomRpcMgr.MoveItem( Connection, nARoom, nAPos, nBRoom, nBPos)
	local Player = Connection.m_Player
	if not (Player and IsCppBound(Player) and IsCppBound(Connection)) then
		Gas2Gac:RetMoveItemError(Connection,nARoom,nAPos,nBRoom,nBPos)
		return
	end
	local nCharID = Player.m_uID
	local function CallBack(result)
		if #result == 0 then
			Gas2GacById:RetMoveItemError(nCharID,nARoom,nAPos,nBRoom,nBPos)
			return
		end
		local res = result[1]
		local type = result[2]
		if IsTable(res) then
			if type == "Replace" then
				Gas2GacById:RetReplaceItemByGridIDEnd(nCharID,nARoom,nAPos,nBRoom,nBPos)
			else
				Gas2GacById:RetSplitItemEnd(nCharID,nARoom,nAPos,nBRoom,nBPos)
			end
		else
			Gas2GacById:RetMoveItemError(nCharID,nARoom,nAPos,nBRoom,nBPos)
		end
	end
	
	local data = {}
	data.nCharID	= nCharID
	data.nARoom = nARoom
	data.nAPos = nAPos
	data.nBRoom = nBRoom
	data.nBPos = nBPos
	local nRes = CallAccountManualTrans(Connection.m_Account, OperateRoomRpcDB, "MoveItem", CallBack, data)
	if 1 == nRes then
		Gas2GacById:RetMoveItemError(nCharID,nARoom,nAPos,nBRoom,nBPos)
	end
end
-----------------------------------------------------------------------------------------------------

--【放置背包,如果有包的话并且能装下物品就进行包交换】
function CRoomRpcMgr.PlaceBag(Connection,nRoom,nPos,nSlot)
	local Player = Connection.m_Player
	if not (Player and IsCppBound(Player) and IsCppBound(Connection)) then return end
	if Player.m_ItemBagLock then
		Gas2GacById:ReturnPlaceBagError(nCharID)
		MsgToConn(Connection,160009)
		return
	end
	local nCharID = Player.m_uID
	local nCharLevel = Player:CppGetLevel()
	local function CallBack(res)
		if IsTable(res) then
			Gas2GacById:ReturnPlaceBag(nCharID,unpack(res))
		else
			Gas2GacById:ReturnPlaceBagError(nCharID)
		end
	end
	
	local parameters = {}
	parameters["nCharID"]	= nCharID
	parameters["nRoom"] = nRoom
	parameters["nPos"] = nPos
	parameters["nSlot"] = nSlot
	parameters["nCharLevel"] = nCharLevel
	local nRes = CallAccountManualTrans(Connection.m_Account, OperateRoomRpcDB, "UseBag", CallBack, parameters)
	if 1 == nRes then
		Gas2GacById:ReturnPlaceBagError(nCharID)
	end
end
------------------------------------------------------------------------------------------------------

--【把包拿下】
function CRoomRpcMgr.FetchBag(Connection,nSlot,nRoom,nPos)
	local Player = Connection.m_Player
	if not (Player and IsCppBound(Player) and IsCppBound(Connection)) then return end
	if Player.m_ItemBagLock then
		Gas2GacById:ReturnFetchBagError(nCharID)
		MsgToConn(Connection,160026)
		return
	end
	local nCharID = Player.m_uID
	
	local function CallBack(res)
		if IsTable(res) then
			Gas2GacById:ReturnFetchBag(nCharID,unpack(res))
		else
			Gas2GacById:ReturnFetchBagError(nCharID)
		end
	end
	
	local parameters = {}
	parameters["nCharID"]	= nCharID
	parameters["nRoom"] = nRoom
	parameters["nPos"] = nPos
	parameters["nSlot"] = nSlot
	local nRes = CallAccountManualTrans(Connection.m_Account, OperateRoomRpcDB, "FetchBag", CallBack, parameters)
	if 1 == nRes then
		Gas2GacById:ReturnFetchBagError(nCharID)
	end
end
------------------------------------------------------------------------------------------------------
--【删除背包】
function CRoomRpcMgr.DelBag(Connection,nSlot)
	local Player = Connection.m_Player
	if not (Player and IsCppBound(Player) and IsCppBound(Connection)) then return end
	local nCharID = Player.m_uID
	
	local function CallBack(res)
		if IsTable(res) then
			Gas2GacById:ReturnDelBag(nCharID,res[1])
		else
			Gas2GacById:ReturnDelBagError(nCharID,res)
		end
	end
	
	local parameters = {}
	parameters["nCharID"]	= nCharID
	parameters["nSlot"] = nSlot
	CallAccountManualTrans(Connection.m_Account, OperateRoomRpcDB, "DelBag", CallBack, parameters)
end
------------------------------------------------------------------------------------------------
--【交换背包】
function CRoomRpcMgr.Change2Bag(Connection,nASlot,nBSlot)
	local Player = Connection.m_Player
	if not (Player and IsCppBound(Player) and IsCppBound(Connection)) then
		Gas2Gac:ReturnChange2BagError(Connection)
		return
	end
	local nCharID = Player.m_uID
	
	local function CallBack(res)
		if IsTable(res) then
			Gas2GacById:ReturnChange2Bag(nCharID,unpack(res))
		else
			Gas2GacById:ReturnChange2BagError(nCharID)
		end
	end
	
	local data = {}
	data.nCharID	= nCharID
	data.nASlot = nASlot
	data.nBSlot = nBSlot
	local nRes = CallAccountManualTrans(Connection.m_Account, OperateRoomRpcDB, "Change2Bag", CallBack, data)
	if 1 == nRes then
		Gas2GacById:ReturnChange2BagError(nCharID)
	end
end

-------------------------------------------------------------------------------------------------------
--【通过右键来移动物品】
function CRoomRpcMgr.QuickMove(Connection,nARoom,nAPos,nStoreMain)
	local Player = Connection.m_Player
	if not (Player and IsCppBound(Player) and IsCppBound(Connection)) then return end
	local nCharID = Player.m_uID
	
	local function CallBack(res)
			if not IsTable(res) then
				Gas2GacById:RetQuickMoveError(nCharID,res)
			end
	end
	
	local data = {}
	data.nCharID	= nCharID
	data.nARoom = nARoom
	data.nAPos = nAPos
	data.nStoreMain = nStoreMain
	CallAccountManualTrans(Connection.m_Account, OperateRoomRpcDB, "QuickMoveItem", CallBack, data)
end
--------------------------------------------------------------------------------------------------
--包裹整理
function CRoomRpcMgr.CleanBag(Connection,nStoreMain)
	local Player = Connection.m_Player
	if not (Player and IsCppBound(Player) and IsCppBound(Connection)) then
		Gas2Gac:ReturnSetBagState(Connection)
		return
	end
	
	local nCharID = Player.m_uID
	
	local function CallBack(RetRes)
		if not IsTable(RetRes) then return end
		if IsTable(RetRes) then
			local tbl_old_res = RetRes.tbl_old_res or {}
			local tbl_new_res = RetRes.tbl_new_res or {}
			for m=1,#tbl_new_res do
				Gas2GacById:RetItemRoom(nCharID,tbl_new_res[m].new_pos[1])
				for i= 1,#tbl_new_res[m] do
					Gas2GacById:RetAddItemToGrid( nCharID,tbl_new_res[m][i])
				end
				Gas2GacById:RetAddItemToGridEndEx(nCharID,tbl_new_res[m].grid_id,tbl_new_res[m].new_pos[2])
			end
			for n=1,#tbl_old_res do
				for i= 1,#tbl_old_res[n] do
					Gas2GacById:RetDelItemFromGrid(nCharID,tbl_old_res[n][i],tbl_old_res[n].old_pos[1],tbl_old_res[n].old_pos[2],false)
				end
			end
			local tblChangedPos = RetRes.tbl_changed_pos
			for i=1,2 do
				local tblChangedPosi = tblChangedPos[i] or {}
				for m =1,#tblChangedPosi do
					Gas2GacById:RetMoveItemByGridID(nCharID,tblChangedPosi[m][1],tblChangedPosi[m][2],tblChangedPosi[m][3])
				end
			end
		else
			Gas2GacById:ReturnSetBagState(nCharID)
			return
		end
		Gas2GacById:RetCleanBag(nCharID)
	end
	
	local parameters = {}
	parameters["nCharID"]	= nCharID
	parameters["nStoreMain"]		= nStoreMain
	local nRes = CallAccountManualTrans(Connection.m_Account, OperateRoomRpcDB, "CleanBag", CallBack, parameters)
	if 1 == nRes then
		Gas2GacById:ReturnSetBagState(nCharID)
	end
end
---------------------------------------------------------------------------------------------
--物品使用后被删除
function CRoomRpcMgr.OnUseDelItemByPos(Connection, nRoomIndex,nPos,nCount)
	local Player = Connection.m_Player
	if not (Player and IsCppBound(Player) and IsCppBound(Connection)) then return end	
	if Player.m_ItemBagLock then
		MsgToConn(Connection,160009)
		return 
	end
	DelItemByPos( Connection, nRoomIndex,nPos,nCount,event_type_tbl["物品使用"])
end
--------------------------------------------------------------------------------------------------
--左键点起删除物品
function CRoomRpcMgr.OnLButtonUpDelItemByPos(Connection, nRoomIndex,nPos,nCount)
	local Player = Connection.m_Player
	if not (Player and IsCppBound(Player) and IsCppBound(Connection)) then return end	
	if Player.m_ItemBagLock then
		MsgToConn(Connection,160013)
		return 
	end
	DelItemByPos( Connection, nRoomIndex,nPos,nCount,event_type_tbl["左键点击删除"])
end

function CRoomRpcMgr.DelWithTimeOutItem(Connection, nItemID,nItemType,sItemName)
	local Player = Connection.m_Player
	if not (Player and IsCppBound(Player) and IsCppBound(Connection)) then return end	
	local nCharID = Player.m_uID
	
	local function CallBack(result)
		if not result then
			return
		end
		local nLeftTime = result.m_LeftTime
		local nEquipPart = result.equip_part

		if nLeftTime and nLeftTime ~= 0 then
			Gas2GacById:RetItemLeftTime(nCharID,nItemID,nLeftTime)
		else
			if nEquipPart then
				local equip_info = result.equip_info
				local equip_mgr = g_GetEquipMgr()
				local old_equip = equip_mgr:GetEquipFromRet(nItemID,nCharID,nItemType,sItemName,equip_info)
				if ((not equip_info.m_tblTimeOut[2]) and equip_info.m_tblTimeOut[1] > 0) then
					old_equip:DisablePropty(Player,nEquipPart, "Gac2Gas:DelWithTimeOutItem")
				end
			end
			local itemInfo = g_ItemUseInfoMgr:GetItemUseInfo(sItemName)
			local petName = nil
			if itemInfo ~= nil and itemInfo.Arg and itemInfo.Arg[1] then
				petName= itemInfo.Arg[1][1]
			end
			if not IsCppBound(Player) then
				return
			end
			local LittlePet = Player:GetServantByType(ENpcType.ENpcType_LittlePet)
			if IsServerObjValid(LittlePet) and IsCppBound(LittlePet) then
				local name = LittlePet.m_Properties:GetCharName()
				if petName == name then 
					MsgToConnById(Player.m_uID, 420002)
					Player:RemoveServant(LittlePet)
				end
			end
		end
	end

	
	local params = {}
	params.nCharID	= nCharID
	params.nItemID	= nItemID
	params.nItemType = nItemType
	params.sItemName	= sItemName
	CallAccountManualTrans(Connection.m_Account, OperateRoomRpcDB, "DelWithTimeOutItem", CallBack, params)
end

--定时重置魂珠经验球使用次数
function CRoomRpcMgr.g_InitSoulPearlTimeInfo()
	local function CallBack(nLimitType)
		local data = {}
		data.nLimitType = nLimitType
		if not g_App:CallDbHalted() then
			CallDbTrans(OperateRoomRpcDB,"ResetSoulPearlTime", nil,data)
		end
	end
	cfg_load "item/SoulPearlLimitType_Common"
	local keys = SoulPearlLimitType_Common:GetKeys()
	for i,key in pairs(keys) do
		local p = SoulPearlLimitType_Common(key)
		local nLimitType = p("LimitType")
		local time_info = loadstring("return " .. p("ResetTime"))()
		g_AlarmClock:AddTask("SoulPearlTime", time_info, CallBack, nil,nLimitType)
	end
end


function CRoomRpcMgr.ClearItemNumInfo()
	local function CallBack()
		if not g_App:CallDbHalted() then
			CallDbTrans(OperateRoomRpcDB,"ClearItemAmountInfo", nil,{})
		end
	end
	g_AlarmClock:AddTask("ClearItemAmountInfo", {wday = {1,2,3,4,5,6,7},time = {"00:00"}}, CallBack)
end




