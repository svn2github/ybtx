local build_box = "GasTongbuildingDB"
CTongBuilding = class()
-----------------------------------------RPC函数----------------------------------------
--【查询帮会建筑信息】
function CTongBuilding.GetTongBuildingList(Conn)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	local uTongID = Player.m_Properties:GetTongID()
	--回调函数【高速逻辑】
	local function CallBack(result)
			for i =1 ,result:GetRowNum() do
						Gas2Gac:ReturnGetTongBuildingList(Conn, result(i,1), result(i,2),result(i,3),
											result(i,4),result(i,5),result(i,6), result(i,9))
			end
			Gas2Gac:ReturnGetTongBuildingListEnd(Conn)
	end
	
  local parameters = {}
	parameters.uPlayerID	= uPlayerID
	CallAccountManualTrans(Conn.m_Account, build_box, "GetTongBuildingInfoRPC", CallBack, parameters, uTongID)
end

--【拆除建筑】
function CTongBuilding.RemoveBuilding(Conn, NpcID)
	local Npc = CCharacterDictator_GetCharacterByID(NpcID)
	build_id = Npc.m_BuildingId
	
	local Player = Conn.m_Player
  if not Player then return end
  if Player.m_ItemBagLock then
		MsgToConn(Conn,160042)
		return 
	end
	local uPlayerID = Player.m_uID
	local uTongID = Player.m_Properties:GetTongID()
	local function CallBack(bFlag,uMsgID,tong_id,build_name)
		if (not bFlag) then
			if IsNumber(uMsgID) then
				MsgToConn(Conn, uMsgID)
			end
			return
		end 
		if string.find(build_name,"学院") then
			CTongTech.SendTeachPropertyByServer(Conn,tong_id)
		end
		if string.find(build_name,"篝火") then
			local tblMembers = g_GasTongMgr:GetMembers(tong_id)
			for i = 1, #(tblMembers) do
				local Member = g_GetPlayerInfo(tblMembers[i])
				if Member then
					Gas2GacById:RetNeedFireActivityEnd(tblMembers[i],351009)
				end
			end
		end
	end
  local parameters = {}
	parameters.build_id	= build_id
	parameters.uPlayerID	= uPlayerID
	parameters.hpRate = Npc:CppGetHP() / Npc:CppGetMaxHP()
	CallAccountManualTrans(Conn.m_Account, build_box, "RemoveBuilding", CallBack, parameters, uTongID)
end

--【以小队等级筛选可建项目】
function CTongBuilding.GetTongLevelToFilterAvailableBuildingItems(Conn)
	local Player = Conn.m_Player
	if not Player then return end
	local uTongID = Player.m_Properties:GetTongID()
	local param = {}
	param.tongID = uTongID
	local function CallBack(res)
		Gas2Gac:FilterAvailableBuildingItems(Conn,res)
	end
	CallDbTrans("GasTongBasicDB","GetTongLevelBackToHighSpeed",CallBack,param)
end

--【制造建筑卷轴】
function CTongBuilding.CreateBuildingItem(Conn, sBuidingName,nDepot)
	local Player    = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	local uTongID   = Player.m_Properties:GetTongID()
	if nDepot < 1 or nDepot > 5 then return end	
	if not DistanceJudgeBetweenNpcAndPlayer(Conn,"建造建筑")	then
		return
	end
	if Player.m_ItemBagLock then
		MsgToConn(Conn,160039)
		return 
	end
	local function CallBack(uMsgID,tblRet)
			if IsNumber(uMsgID) then
				if tblRet then
					MsgToConn(Conn,uMsgID,tblRet)
				else
					MsgToConn(Conn,uMsgID)
				end
				return
			end
			if tblRet[1] and tblRet[1] ~= "" then
				local NeedTime = g_ItemInfoMgr:GetItemInfo(g_ItemInfoMgr:GetTongItemBigID(), tblRet[1],"NeedTime")
				if tblRet[3] > NeedTime then
					tblRet[3] = NeedTime
				end
			end
			Gas2Gac:ReturnGetCreatingBuildingitemInfo(Conn,unpack(tblRet))
	end
	
	local parameters = {}
	parameters.uPlayerID    = uPlayerID
	parameters.sBuildingName = sBuidingName
	parameters.nDepot = nDepot
	parameters.sceneName = Player.m_Scene.m_SceneName
	CallAccountManualTrans(Conn.m_Account, build_box, "CreateBuildingItemDB", CallBack, parameters, uTongID)
end

--【取消建筑模型建造】
function CTongBuilding.CancelCreateBuildingItem(Conn,nBuildID)
	local Player    = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	local uTongID   = Player.m_Properties:GetTongID()
	if not DistanceJudgeBetweenNpcAndPlayer(Conn,"建造建筑")	then
		return
	end
	if Player.m_ItemBagLock then
		MsgToConn(Conn,160039)
		return 
	end
	local function CallBack(tblRet,tblRetRes)
		if IsNumber(tblRet) then
			MsgToConn(Conn,tblRet)
			return
		end
		if tblRetRes then
			local nRes = tblRetRes.m_nRetRes
			local nMoney = tblRetRes.m_nRetMoney
			local nDropRes = tblRetRes.m_nDropRes
			if IsNumber(nMoney) and nMoney > 0 then
				Gas2Gac:TongMoneyMsgToConn(Conn,9332,nMoney)
			end
		end
		
		if IsTable(tblRet) then
			if tblRet[1] and tblRet[1] ~= "" then
				local NeedTime = g_ItemInfoMgr:GetItemInfo(g_ItemInfoMgr:GetTongItemBigID(), tblRet[1],"NeedTime")
				if tblRet[3] > NeedTime then
					tblRet[3] = NeedTime
				end
			end
			Gas2Gac:ReturnGetCreatingBuildingitemInfo(Conn,unpack(tblRet))
		end
	end
	
	local parameters = {}
	parameters.uPlayerID    = uPlayerID
	parameters.nBuildID = nBuildID
	parameters.sceneName = Player.m_Scene.m_SceneName
	CallAccountManualTrans(Conn.m_Account, build_box, "CancelMakeBuildingDB", CallBack, parameters, uTongID)
end


--【获得正在建造的建筑模型信息】
function CTongBuilding.GetCreatingBuildingItemInfo(Conn)
	local Player    = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	local uTongID   = Player.m_Properties:GetTongID()
	if not DistanceJudgeBetweenNpcAndPlayer(Conn,"建造建筑")	then
		return
	end
	local function CallBack(tblRet,tblRetRes)
		local nRes = tblRetRes.m_nRetRes
		local nMoney = tblRetRes.m_nRetMoney
		local nDropRes = tblRetRes.m_nDropRes
		if IsNumber(nRes) and nRes > 0 then
			MsgToConn(Conn,9307)
		end

		if IsNumber(nMoney) and nMoney > 0 then
			Gas2Gac:TongMoneyMsgToConn(Conn,9332,nMoney)
		end
		if tblRet[1] and tblRet[1] ~= "" then
			local NeedTime = g_ItemInfoMgr:GetItemInfo(g_ItemInfoMgr:GetTongItemBigID(), tblRet[1],"NeedTime")
			if tblRet[3] > NeedTime then
				tblRet[3] = NeedTime
			end
		end
		Gas2Gac:ReturnGetCreatingBuildingitemInfo(Conn,unpack(tblRet))
	end
	
	local parameters = {}
	parameters.uPlayerID    = uPlayerID
	parameters.sceneName = Player.m_Scene.m_SceneName
	CallAccountManualTrans(Conn.m_Account, build_box, "GetCreatingBuildingItemInfoDB", CallBack, parameters, uTongID)
end



--【获得帮会最高等级，更新帮会等级】
function CTongBuilding.UpdateBuildingLevel(Conn, NpcID)
	local Player = Conn.m_Player
	if not Player then
		return
	end
	local uPlayID = Player.m_uID
	local uTongID = Player.m_Properties:GetTongID()
	if uTongID == 0 then 
		return 
	end
	local Npc = CCharacterDictator_GetCharacterByID(NpcID)
	if not( Npc and Npc.m_BuildingId and  Npc.m_TongID == uTongID)then
		return
	end
	local function CallBack(IsSucceed, arg)
		if not IsSucceed then
			if IsNumber(arg) and IsCppBound(Conn) then
				MsgToConn(Conn, arg)
				return
			end
		end
		local newLevel = arg
		MsgToConn(Conn,9043,newLevel)
	end
	local parameters = {}
	parameters.uPlayID = uPlayID
	parameters.buildingId = Npc.m_BuildingId
	local LevUpNeedRes = g_ItemInfoMgr:GetItemInfo(g_ItemInfoMgr:GetTongItemBigID(), Npc.m_Name,"LevUpNeedRes") or 0
	local CostResource = LevUpNeedRes * 0.1
	parameters.uCostResource = CostResource
	parameters["uCurrentLevel"] = Npc:CppGetLevel()
	CallAccountManualTrans(Conn.m_Account, build_box, "UpdateBuildingLevel", CallBack, parameters, uTongID)
end

--------------------------------------------------------------
