CTongDepot = class()
local GasTongDepotDB = "GasTongDepotDB"
------------------------------------------RPC函数------------------------------------
function CTongDepot.GetCollectiveBagOnePageItemList(Conn, nType,nPage)
	local Player = Conn.m_Player
	if not (Player and IsCppBound(Player)) then return end
	local nCharID = Player.m_uID
	if Player.m_ItemBagLock then
		MsgToConn(Conn,160033)
		return 
	end
	if not DistanceJudgeBetweenNpcAndPlayer(Conn,"打开佣兵团仓库")	then
		return
	end
	
	local function CallBack(tblCdepotItemInfo)
		Gas2Gac:ReturnGetCollectiveBagOnePageItemListStart(Conn, nType)
		local SoulInfo = tblCdepotItemInfo.SoulpearlInfo
		if SoulInfo then
			for m =1,SoulInfo:GetRowNum() do
				Gas2GacById:RetSoulPearlItemInfo(nCharID,SoulInfo(m,1),SoulInfo(m,2))
			end
		end
		for i=1,#tblCdepotItemInfo do
			local nItemID = tblCdepotItemInfo[i][4]
			local sItemType = tonumber(tblCdepotItemInfo[i][1])
			local sItemName = tblCdepotItemInfo[i][2]
			local uBindingType = tblCdepotItemInfo[i][5]
			local nPos = tblCdepotItemInfo[i][3]
			--判断是否为装备
			if g_ItemInfoMgr:IsStaticEquip(sItemType) then
				local equip_mgr = g_GetEquipMgr()
				local equip = equip_mgr:GetEquipFromRet(nItemID, nCharID, sItemType,sItemName,tblCdepotItemInfo[i][6])
				equip:SendEquipInfo2Conn(nCharID)
			end
			Gas2Gac:RetItemTypeInfo(Conn,nItemID,sItemType,sItemName,uBindingType)
			Gas2Gac:RetAddItem( Conn,nItemID,nType,nPos)
		end
		Gas2Gac:ReturnGetCollectiveBagOnePageItemListEnd(Conn, nType,nPage)
	end
	
	local parameters = {}
	parameters.nCharID	= nCharID
	parameters.nType = nType
	parameters.nPage = nPage
	parameters.sceneName= Conn.m_Player.m_Scene.m_SceneName
	CallDbTrans(GasTongDepotDB, "GetCDepotItemByPage", CallBack, parameters, "CDepotCannel")
end
-------------------------------------------------------------
--【物品库内物品移动】
function CTongDepot.CollectiveBagMoveItemSelf(Conn, nType, nFromPage, nFromPos, nToPage, nToPos)
	local Player = Conn.m_Player
	if not (Player and IsCppBound(Player)) then return end
	local nCharID = Player.m_uID
	if Player.m_ItemBagLock then
		MsgToConn(Conn,160033)
		return 
	end
	--判断是否在对应的npc旁边
	if not DistanceJudgeBetweenNpcAndPlayer(Conn,"打开佣兵团仓库")	then
		return
	end

	local function CallBack(bFlag,item_type,name)
		if not bFlag or IsNumber(bFlag) then
			if IsNumber(bFlag) then
				MsgToConn(Conn,bFlag)
			end
			Gas2Gac:ReturnCollectiveBagMoveItemSelf(Conn, false,nType,nToPage)
			return 
		end
		Gas2Gac:RetPlayerItemSound(Conn,item_type,name)
		Gas2Gac:ReturnCollectiveBagMoveItemSelf(Conn, true,nType,nToPage)
	end
	
	local parameters = {}
	parameters.nCharID	= nCharID
	parameters.nType = nType
	parameters.nFromPage = nFromPage
	parameters.nFromPos = nFromPos
	parameters.nToPage = nToPage
	parameters.nToPos = nToPos
	CallAccountManualTrans(Conn.m_Account, GasTongDepotDB, "CollectiveBagMoveItemSelf", CallBack, parameters, "CDepotCannel")
end

--------------------------------------------------------------
--【领取】
function CTongDepot.BagAddItemFromCollectiveBag(Conn,nFromRoom, nFromPos,nFromPage,nToRoom,nToPos)
	local Player = Conn.m_Player
	if not (Player and IsCppBound(Player)) then return end
	local nCharID = Player.m_uID
	if Player.m_ItemBagLock then
		MsgToConn(Conn,160033)
		return 
	end
	if not DistanceJudgeBetweenNpcAndPlayer(Conn,"打开佣兵团仓库")	then
		return
	end
	local function CallBack(bFlag,sMsg)
		if not bFlag then
			Gas2Gac:ReturnBagAddItemFromCollectiveBag(Conn,false,nFromRoom,nFromPage)
			return 
		end
		if IsNumber(bFlag) then
			if IsString(sMsg) then
				MsgToConn(Conn,bFlag,sMsg)
			else
				MsgToConn(Conn,bFlag)
			end
			Gas2Gac:ReturnBagAddItemFromCollectiveBag(Conn,false,nFromRoom,nFromPage)
			return
		end
		MsgToConn(Conn,9032)
    Gas2Gac:ReturnBagAddItemFromCollectiveBag(Conn,true,nFromRoom,nFromPage)
	end
	
	local parameters = {}
	parameters.nCharID	= nCharID
	parameters.nFromRoom = nFromRoom
	parameters.nFromPage = nFromPage
	parameters.nFromPos = nFromPos
	parameters.nToRoom = nToRoom
	parameters.nToPos = nToPos
	CallAccountManualTrans(Conn.m_Account, GasTongDepotDB, "MoveItemFromCDepotToBag", CallBack, parameters, "CDepotCannel")
end

--【右键点击领取】
function CTongDepot.QuikMoveItemFromCDepotToBag(Conn,nFromRoom, nFromPos,nFromPage,nToStoreRoom)
	local Player = Conn.m_Player
	if not (Player and IsCppBound(Player)) then return end
	local nCharID = Player.m_uID
	if Player.m_ItemBagLock then
		MsgToConn(Conn,160033)
		return 
	end
	if not DistanceJudgeBetweenNpcAndPlayer(Conn,"打开佣兵团仓库")	then
		return
	end
	local function CallBack(bFlag,sMsg)
		if not bFlag then
			Gas2Gac:ReturnBagAddItemFromCollectiveBag(Conn,false,nFromRoom,nFromPage)
			return
		end
		if IsNumber(bFlag) then
			if sMsg then
				MsgToConn(Conn,bFlag,sMsg)
			else
				MsgToConn(Conn,bFlag)
			end
			Gas2Gac:ReturnBagAddItemFromCollectiveBag(Conn,false,nFromRoom,nFromPage)
			return
		end
    Gas2Gac:ReturnBagAddItemFromCollectiveBag(Conn,true,nFromRoom,nFromPage)
		MsgToConn(Conn,9032)
	end
		
	local parameters = {}
	parameters.nCharID	= nCharID
	parameters.nFromRoom = nFromRoom
	parameters.nFromPage = nFromPage
	parameters.nFromPos = nFromPos
	parameters.nToStoreRoom = nToStoreRoom
	CallAccountManualTrans(Conn.m_Account, GasTongDepotDB, "QuikMoveItemFromCDepotToBag", CallBack, parameters, "CDepotCannel")
end

--【捐献】
function CTongDepot.CollectiveBagAddItemFromOther(Conn, nFromRoom, nFromPos, nToRoom, nToPage, nToPos)
	local Player = Conn.m_Player
	if not (Player and IsCppBound(Player)) then return end
	local nCharID = Player.m_uID
	if Player.m_ItemBagLock then
		MsgToConn(Conn,160033)
		return 
	end
	local function CallBack(tbl_ret_res)
		if not tbl_ret_res then
			Gas2Gac:ReturnCollectiveBagAddItemFromOther(Conn,false,nToRoom, nToPage)
			return 
		end
		if IsNumber(tbl_ret_res) then
			MsgToConn(Conn,tbl_ret_res)
			Gas2Gac:ReturnCollectiveBagAddItemFromOther(Conn,false,nToRoom, nToPage)
			return
		end
		local ItemType = tbl_ret_res.ItemType
		local ItemName = tbl_ret_res.ItemName
		Gas2Gac:RetPlayerItemSound(Conn,ItemType,ItemName)
		Gas2Gac:ReturnCollectiveBagAddItemFromOther(Conn,true,nToRoom, nToPage)
	end
	
	local data = {}
	data.nCharID	= nCharID
	data.nFromRoom = nFromRoom
	data.nFromPos= nFromPos
	data.nToRoom= nToRoom
	data.nToPage= nToPage
	data.nToPos = nToPos
	data.sceneName = Player.m_Scene.m_SceneName
	CallAccountManualTrans(Conn.m_Account, GasTongDepotDB, "MoveItemFromBagToCDepotRPC", CallBack, data, "CDepotCannel")
end

-------------------------------------------------------------------------------

