
local BattleArraySql = "BattleArraySql"
CGasBattleArrayBooks = class()
------------------------------------------------------------------------------------------------
--判断从客户端传来的5个阵型位置是否有任意两值相等
local function SeekPosSame(tbl, unNum)
	local tbl = tbl
	table.sort(tbl, function(a, b) return a < b end)
	for i = 1, #tbl-1 do
		if(tbl[i] > unNum) then
			if(tbl[i] == tbl[i+1]) then
				return false
			end
		end
	end
	return true
end
-------------------------------------------------------------------------------------------------
--判断从数据库中查出来的玩家ID是不是为null，为null的话用0返回
local function CheckPlayerId(PlayerId)
	if PlayerId == null then
		return 0
	end
	return PlayerId
end
----------------------------------------------------------
--【保存阵型信息具体实现】
local function SaveBattleShape(Conn,n,res,nRoomIndex,nPos)
	if n == false then
		MsgToConn(Conn, 110075)
		return
	end
	local goodsId = 0
	if IsTable(res) then
		for i=1,#res do
		--从包裹栏里删除该阵法
			goodsId = res[1]
		end
	else
		if res ~=0 and res ~= -1 then
			Gas2Gac:RetDelItemError(Conn,res)
		end
	end
	Gas2Gac:ReturnSaveBattleArrayPos(Conn, n, goodsId)
end

function CGasBattleArrayBooks.SaveBattleArrayPos(Conn,b_sName,b_uPos1,b_uPos2,b_uPos3,b_uPos4,b_uPos5,nRoomIndex,nPos)

	local PlayerId = Conn.m_Player.m_uID
	local tbl = {b_uPos1,b_uPos2,b_uPos3,b_uPos4,b_uPos5}
	local bFlag = SeekPosSame(tbl,-1)
	if not bFlag then
		return
	else
		local data = {
									["PlayerId"] = PlayerId,
									["b_sName"] = b_sName,
									["b_uPos1"] = b_uPos1,
									["b_uPos2"] = b_uPos2,
									["b_uPos3"] = b_uPos3,
									["b_uPos4"] = b_uPos4,
									["b_uPos5"] = b_uPos5,
									["nRoomIndex"] = nRoomIndex,
									["nPos"] = nPos
									}
		local function callback(n,res)
			SaveBattleShape(Conn,n,res,nRoomIndex,nPos)
		end
		--local battleArrayBooksSql = (g_DBTransDef["BattleArraySql"])
		CallAccountManualTrans(Conn.m_Account, BattleArraySql, "SaveBattleShape", callback, data)
	end
end
----------------------------------------------------------

--【保存列阵信息具体实现】
local function SaveBattleArray(Conn,ret)
		Gas2Gac:ReturnSaveBattleArrayMemPos(Conn,ret)
end

function CGasBattleArrayBooks.SaveBattleArrayMemPos(Conn,b_uId,b_sName,ab_uLoc1,ab_uLoc2,ab_uLoc3,ab_uLoc4,ab_uLoc5)
	local PlayerId = Conn.m_Player.m_uID
	local PlayerIdTbl = {}
	local tbl = {ab_uLoc1, ab_uLoc2, ab_uLoc3, ab_uLoc4, ab_uLoc5}
	for i = 1, #tbl do
		if(0 ~= tbl[i]) then
			table.insert(PlayerIdTbl,tbl[i])
		end
	end
	local bFlag = SeekPosSame(PlayerIdTbl,0)
	if not bFlag then
		return
	else
		local data = {
									["PlayerId"] = PlayerId,
									["b_uId"] = b_uId,
									["b_sName"] = b_sName,
									["ab_uLoc1"] = ab_uLoc1,
									["ab_uLoc2"] = ab_uLoc2,
									["ab_uLoc3"] = ab_uLoc3,
									["ab_uLoc4"] = ab_uLoc4,
									["ab_uLoc5"] = ab_uLoc5
									}
		local function callback(ret)
			SaveBattleArray(Conn,ret)
		end
		--local battleArrayBooksSql = (g_DBTransDef["BattleArraySql"])
		CallAccountManualTrans(Conn.m_Account, BattleArraySql, "SaveBattleArray", callback, data)
	end
end

----------------------------------------------------------
--【查询列阵信息具体实现】
local function SelectBattleArray(Conn,b_sName,battleshapeInfo,arraybattleInfo,uCapatinID)
	if(uCapatinID == Conn.m_Player.m_uID) then
		if #battleshapeInfo > 0 then
			local posString = ""
			local EntityIDIdString = ""
			for i = 1, 5 do
				local pos			= string.format("%02d,", battleshapeInfo[i+1])
				local playerID		= CheckPlayerId(arraybattleInfo[i])
				local bFlag			= (g_GetPlayerInfo(playerID) == nil)
				local entityID		= bFlag and 0 or g_GetPlayerInfo(playerID):GetEntityID()
				posString			= posString .. pos
				EntityIDIdString	= EntityIDIdString .. entityID .. (i == 5 and "" or ",")
			end
			posString = string.sub(posString, 1, -2)
			if(b_sName and posString and EntityIDIdString) then
				Conn.m_Player:CreateBattleArray(b_sName,posString,EntityIDIdString)
			else
--				print("参数为空!")
			end
		end
	end
end

function CGasBattleArrayBooks.UseBattleArraySkill(Conn,b_sName)
	local PlayerId = Conn.m_Player.m_uID
	local data = {
								["PlayerId"] = PlayerId,
								["b_sName"] = b_sName,
								}
	
	local function callback(battleshapeInfo,arraybattleInfo,uCapatinID)
		SelectBattleArray(Conn,b_sName,battleshapeInfo,arraybattleInfo,uCapatinID)
	end
	--local battleArrayBooksSql = (g_DBTransDef["BattleArraySql"])
	CallAccountManualTrans(Conn.m_Account, BattleArraySql, "SelectArrayBattleInfo", callback, data)
end
--------------------------------------------------------------------------
--[阵法还原成阵法书]
local function RevertToBattleArrayBook(Conn,succ,info)
	if(not succ) then
		Gas2Gac:ReturnRevertToBattleArrayBook(Conn, false)
		return
	end
  Gas2Gac:ReturnRevertToBattleArrayBook(Conn, true)
end

function CGasBattleArrayBooks.RevertToBattleArrayBook(Conn,b_uId,b_sName)
	local PlayerId = Conn.m_Player.m_uID
	local data = {
								["PlayerId"] = PlayerId,
								["b_sName"] = b_sName,
								["b_uId"] = b_uId
								}
	
	local function callback(succ,info)
		RevertToBattleArrayBook(Conn,succ,info)
	end
	--local battleArrayBooksSql = (g_DBTransDef["BattleArraySql"])
	CallAccountManualTrans(Conn.m_Account, BattleArraySql, "RevertToBattleArrayBook", callback, data)
end

-------------------------------------------------------------------------------------------------
local function SaveSkillBattle(Conn,result)
	if result and IsNumber(result) then
		MsgToConn(Conn, result)
	else
		Gas2Gac:ReturnSaveBattleArrayShortCutIndex(Conn, result)
	end
end
--@brief 保存阵法技能栏
function CGasBattleArrayBooks.SaveBattleArrayShortCutIndex(Conn,b_name,b_pos)
	local playerId = Conn.m_Player.m_uID
	local data = {
								["b_name"] = b_name,
								["b_pos"] = b_pos,
								["playerId"] = playerId
								}
	local function callback(result)
		SaveSkillBattle(Conn,result)
	end
	--local battleArrayBooksSql = (g_DBTransDef["BattleArraySql"])
	CallAccountManualTrans(Conn.m_Account, BattleArraySql, "SaveSkillBattle", callback, data)
end
