
local BoxitemDB = "BoxitemDB"

--根据从配置表或数据库得到的掉落物品信息，返回给客户端
local function RetOnePageBoxitemDropInfo(Conn, randomItemTbl, totalPageNo, curPageNo, boxitemID, bTblResultFlag )
	Gas2Gac:RetGetBoxitemInfoBegin(Conn)
	local itemInfoTblLen
	if bTblResultFlag == false then
		itemInfoTblLen = randomItemTbl:GetRowNum()
		randomItemTbl = randomItemTbl:GetTableSet()
	else
		itemInfoTblLen = # randomItemTbl
	end
	
	local Len = math.min(itemInfoTblLen, 4)
	for i=1, Len do
		local itemType, itemName, itemCount, itemIndex
		if bTblResultFlag then
			itemType, itemName, itemCount, itemBindingType, itemIndex = randomItemTbl[i][1], randomItemTbl[i][2], randomItemTbl[i][3], randomItemTbl[i][4], randomItemTbl[i][5]
		elseif bTblResultFlag == false then
			itemType, itemName, itemCount, itemIndex = randomItemTbl(i,1), randomItemTbl(i,2), randomItemTbl(i,3), randomItemTbl(i,4)
		end
		local itemtype=tonumber(ItemType)
		Gas2Gac:RetGetBoxitemInfo(Conn, itemIndex, itemType, itemName, itemCount)
	end
	Gas2Gac:RetGetBoxitemInfoEnd(Conn, totalPageNo, curPageNo, boxitemID)
end
	
--获取某个盒子物品掉落的物品信息，参数：盒子物品所在的roomindex和pos
function Gac2Gas:GetBoxitemInfo(Conn, nRoomIndex, nPos, itemType, itemName)
	if not IsCppBound(Conn.m_Player) then
		return
	end
	local data = {["RoomIndex"]=nRoomIndex, ["Pos"]=nPos, ["CharID"]=Conn.m_Player.m_uID,["CharLevel"] = Conn.m_Player:CppGetLevel(),
	["ItemType"] = itemType, ["ItemName"] = itemName}
	
	local function CallBack(suc, result)
		if suc then
			local randomItemTbl = result["RandomItemTbl"]
			local totalPageNo   = result["TotalPageNo"]
			local curPageNo		= result["CurPageNo"]
			local boxitemID		= result["BoxitemID"]
			local bTblResultFlag= result["TblResultFlag"]
			RetOnePageBoxitemDropInfo(Conn, randomItemTbl, totalPageNo, curPageNo, boxitemID, bTblResultFlag)
			Gas2Gac:RetBoxitemOpened(Conn, boxitemID,  true)
		else
			local errorMsgId = result
			MsgToConn(Conn, errorMsgId)
		end
	end
	--local entry = (g_DBTransDef["BoxitemDB"])
	CallAccountManualTrans(Conn.m_Account, BoxitemDB, "BoxitemDropInfo", CallBack, data)
end
--查询盒子状态
function Gac2Gas:SelectBoxState(Conn, nRoomIndex, nPos)
    if not IsCppBound(Conn.m_Player) then
        return 
    end
    local data = {["RoomIndex"]=nRoomIndex, ["Pos"]=nPos, ["CharID"]=Conn.m_Player.m_uID}
	
	local function CallBack(suc, result, errorMsg)
		if suc then
--			Gas2Gac:ShowBoxItemWnd(Conn,true)
			Gas2Gac:InitBoxItemWnd(Conn,true)
		else
			if IsNumber(result) then
				if errorMsg ~= nil then
					MsgToConn(Conn,result,errorMsg)
				else
					MsgToConn(Conn,result)
				end
			end
			Gas2Gac:InitBoxItemWnd(Conn,false)
		end
	end
	--local entry = (g_DBTransDef["BoxitemDB"])
	CallAccountManualTrans(Conn.m_Account, BoxitemDB, "SelectItemByID", CallBack, data)
end
local function TakeAllBoxitemDrop(Conn, boxitemID, result, charID)
	Gas2Gac:RetTakeAllBoxitemDrop(Conn, boxitemID)
end

--提取所有盒子物品掉落的物品
function Gac2Gas:TakeAllBoxitemDrop(Conn, boxitemID)
    if not IsCppBound(Conn.m_Player) then
        return 
    end
    local charID = Conn.m_Player.m_uID
	local data = {["CharID"]= charID, ["BoxitemID"]=boxitemID,
				 ["sceneName"] = Conn.m_Player.m_Scene.m_SceneName}
	
	local function CallBack(suc, result, errorMsg)
		if suc then
			TakeAllBoxitemDrop(Conn, boxitemID, result, charID)
		else
			local errorMsgId = result
			if IsNumber(errorMsgId) then
				if errorMsg ~= nil then
					MsgToConn(Conn, errorMsgId, errorMsg)
				else
					MsgToConn(Conn, errorMsgId)
				end
			end
		end
	end
	
	--local entry = (g_DBTransDef["BoxitemDB"])
	CallAccountManualTrans(Conn.m_Account, BoxitemDB, "TakeAllBoxitemDrop", CallBack, data)
end

local function TakeOneDropItemByIndex(Conn, result, curPageNo, boxitemID)
	if not IsCppBound(Conn.m_Player) then
        return 
	end
	local dropedItemLeft	= result["DropedItemLeft"]
	if dropedItemLeft then
		local randItemTbl = result["RandItemTbl"]
		local totalPageNo =	result["TotalPageNo"]
		local bTblResultFlag=  result["TblResultFlag"]
		RetOnePageBoxitemDropInfo(Conn, randItemTbl, totalPageNo, curPageNo, boxitemID, bTblResultFlag)
	else
		Gas2Gac:RetTakeAllBoxitemDrop(Conn, boxitemID)	
	end
end

function Gac2Gas:TakeOneDropItemByIndex(Conn, curPageNo, index, boxitemID)
    if not IsCppBound(Conn.m_Player) then
        return 
    end
	local data = {["CharID"]=Conn.m_Player.m_uID, ["CurPageNo"]=curPageNo,
					["Index"]= index, ["BoxitemID"]=boxitemID,
					["sceneName"] = Conn.m_Player.m_Scene.m_SceneName}
	
	local function CallBack(suc, result, errorMsg)
		if suc then
			TakeOneDropItemByIndex(Conn, result, curPageNo, boxitemID)
		else
			local errorMsgId = result
			if IsNumber(errorMsgId) then
				if errorMsg ~= nil then
					MsgToConn(Conn, errorMsgId, errorMsg)
				else
					MsgToConn(Conn, errorMsgId)
				end
			end
		end
	end	
	
	--local entry = (g_DBTransDef["BoxitemDB"])
	CallAccountManualTrans(Conn.m_Account, BoxitemDB, "TakeOneDropItemByIndex", CallBack, data)
end

--盒子物品拾取物品面板，上下翻页
function Gac2Gas:GetOnePageDropItem(Conn, boxitemID, curPageNo, totalPageNo)
    if not IsCppBound(Conn.m_Player) then
        return 
    end
	local data	= {["BoxitemID"]=boxitemID, ["CharID"] = Conn.m_Player.m_uID,
					["CurPageNo"]=curPageNo}
	local function CallBack(suc, result, errorMsg)
		if suc then
		    local bTblResultFlag = false
		    local randomItemTbl = result
			RetOnePageBoxitemDropInfo(Conn, randomItemTbl, totalPageNo, curPageNo, boxitemID, bTblResultFlag)
		else
			local errorMsgId =result
			if IsNumber(errorMsgId) then
				if errorMsg ~= nil then
					MsgToConn(Conn, errorMsgId, errorMsg)
				else
					MsgToConn(Conn, errorMsgId)
				end
			end
		end
	end
	--local entry = (g_DBTransDef["BoxitemDB"])
	CallAccountManualTrans(Conn.m_Account, BoxitemDB, "GetOnePageDropItem", CallBack, data)
end