CTongWar = class()
local war_box = "GasTongWarDB"
------------------------------------------------RPC函数------------------------------------------------------
--宣战请求
function CTongWar.SendChallenge(Conn, nWarZoneId, nWarZoneStationId, nMoney)
  if g_WarZoneMgr.m_CurrentChallengeCompeteState == "Close" then
  	MsgToConn(Conn, 9423)
  	return
  end
  
  local Player = Conn.m_Player
  if not Player then
  	return
  end
	local uPlayerID = Player.m_uID
	local uTongID = Player.m_Properties:GetTongID()
	if uTongID == 0 then
		MsgToConn(Conn, 9401)
		return
	end
	if nMoney < 100000 then --竞标资金少于10金
		MsgToConn(Conn, 9445)
		return
	end
	--回调函数【高速逻辑】
	local function CallBack(res, tbl)
		if IsNumber(res) then
			if tbl then
				MsgToConn(Conn, res, tbl)
			else
				MsgToConn(Conn, res)
			end
			Gas2Gac:ReturnSendChallenge(Conn,false)
			return
		end 
		if true == res then
			Gas2Gac:ReturnSendChallenge(Conn,true)
			--任务事件需求加计数
			AddVarNumForTeamQuest(Player, "完成佣兵团宣战", 1)
		end
	end
	
  local parameters = {}
	parameters["nMoney"]				= nMoney
	parameters["uPlayerID"]			= uPlayerID
	parameters["uWarZone"] = nWarZoneId
	parameters["uWarZoneStation"] = nWarZoneStationId
	parameters["PurviewName"] = "SendChallenge"
	CallAccountManualTrans(Conn.m_Account, war_box, "SendChallenge", CallBack, parameters, uTongID)
end

function CTongWar.GetTongChallengeInfoList(Conn)
  local Player = Conn.m_Player
  if not Player then
  	return
  end
	local uPlayerID = Player.m_uID
	local uTongID = Player.m_Properties:GetTongID()
	if uTongID == 0 then
		--MsgToConn(Conn, 9401)
		return
	end
	
	local parameter = {}
	parameter["uTongId"] = uTongID
	--unpack
	local function CallBack(tblRet, selfWz, selfWzs)
		Gas2Gac:ReturnGetTongChallengeInfoListBegin(Conn, selfWz, selfWzs)
		for i =1, #tblRet do
			local YBTongName = tblRet[i][1]
			local YBTongLeader = tblRet[i][2]
			local YBWarZone = tblRet[i][3]
			local YBWarZoneStation = tblRet[i][4]
			Gas2Gac:ReturnGetTongChallengeInfoList(Conn, YBTongName, YBTongLeader, YBWarZone, YBWarZoneStation)
		end
		Gas2Gac:ReturnGetTongChallengeInfoListEnd(Conn)
	end
	
	local function StationCallBack(sceneType, camp, warZoneId, stationId)
		if sceneType == 6 then
			parameter["uSceneType"] = sceneType
			parameter["uCamp"] = camp
		elseif sceneType == 7 then
			parameter["uSceneType"] = sceneType
			parameter["uCamp"] = camp
			parameter["uWarZoneId"] = warZoneId
			parameter["uStationId"] = stationId
		end
		CallAccountManualTrans(Conn.m_Account, war_box, "GetTongChallengeInfoList", CallBack, parameter, uTongID)
	end
	
	CallAccountAutoTrans(Conn.m_Account, "WarZoneDB", "GetTongStationAndCamp", StationCallBack, parameter, uTongID)
end

function CTongWar.GetTongBattleListInfo(Conn)
	local Player = Conn.m_Player
	if not Player then
		return
	end
	local TongID = Player.m_Properties:GetTongID()
	if TongID == 0 then
		MsgToConn(Conn, 9401)
		return
	end
	
	local function CallBack(tblRet)
		Gas2Gac:ReturnGetTongBattleListInfoBegin(Conn)
		for i =1, #tblRet do
			local ChlgTongName = tblRet[i][1]
			local TgtTongName = tblRet[i][2]
			local WarZoneId = tblRet[i][3]
			local StationId = tblRet[i][4]
			Gas2Gac:ReturnGetTongBattleListInfo(Conn, ChlgTongName, TgtTongName, WarZoneId, StationId)
		end
		Gas2Gac:ReturnGetTongBattleListInfoEnd(Conn)
	end
	
	local uCamp = Player:CppGetCamp()
	CallAccountAutoTrans(Conn.m_Account, "GasTongWarDB", "GetAllChallengeBattleInfo", CallBack, {["uCamp"] = uCamp}, uCamp)
end
