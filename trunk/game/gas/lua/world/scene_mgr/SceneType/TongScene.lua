gas_require "world/scene_mgr/SceneType/TongSceneInc"
local StationTimeModulus = {} --驻地团贡奖励系数
StationTimeModulus[1] = 100
StationTimeModulus[2] = 200
StationTimeModulus[3] = 300
StationTimeModulus[4] = 400



local npcNameTbl = {} --驻地团贡奖励系数
npcNameTbl[1] = {"第一战区艾米军资交付","第二战区艾米军资交付"}
npcNameTbl[2] = {"第一战区神圣军资交付","第二战区神圣军资交付"}
npcNameTbl[3] = {"第一战区修斯军资交付","第二战区修斯军资交付"}

local stationCampTbl = {} --驻地团贡奖励系数
stationCampTbl[111] = 1
stationCampTbl[121] = 1
stationCampTbl[122] = 1
stationCampTbl[123] = 1
stationCampTbl[211] = 2
stationCampTbl[221] = 2
stationCampTbl[222] = 2
stationCampTbl[223] = 2
stationCampTbl[311] = 3
stationCampTbl[321] = 3
stationCampTbl[322] = 3
stationCampTbl[323] = 3

function CTongScene:JoinScene( Player, sSceneName)
	return sSceneName 
end

function CTongScene:OnDestroy()
	if not self.m_IsClose then --服务器主动关闭过的帮会副本 将不在清理g_TongFbMgr, 否则可能清掉新的数据
		g_TongFbMgr:RemoveTongScene(self.m_TongId)
	end
end

function CTongScene:GetDestroyChannel()
	return self.m_SceneId, self.m_TongId
end

function CTongScene:OnCreate()
	local tongId = self.m_Process --对于帮会副本 Process 表示 帮会id
	assert(IsNumber(tongId))
	local function CallBack(resule)
		if resule then
			g_TongFbMgr:AddTongScene(tongId, self, resule)
		end
	end
	local parameter = 	{}
	parameter["uTongId"] = tongId
	parameter["isRepair"] = true
	CallDbTrans("GasTongbuildingDB", "GetTongBuildingInfo", CallBack, parameter, tongId)
end

function GetTongPos(Conn)
	if not IsCppBound(Conn) then
		return
	end
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	local nCamp = Player:CppGetCamp()
	local tongId = Player.m_Properties:GetTongID()
	if tongId  == 0 then
		return 
	end
	local function CallBack(sceneType, sceneId, serverId, sceneName, warZoneId, stationId)
		if sceneId == 0 then
			return
		end
		if IsCppBound(Conn) and IsCppBound(Player) then
			local pos
			if sceneType == 6 then
				pos = g_TongFbMgr:GetEnterPos()
			elseif sceneType == 7 then
				pos = g_WarZoneMgr:GetStationPos(stationId)
			end
			Gas2Gac:RetGetForageAutoInfo(Conn, sceneName,pos[1],pos[2])
		end
	end
	
	local data = {}
	data["tongId"] = tongId
	data["IsSetServerId"] = true
	CallAccountAutoTrans(Conn.m_Account, "WarZoneDB", "GetTongStationInfoById", CallBack, data, tongId)
end

function GetAutoSeekInfo(Conn)
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return 
	end
	local nCamp = Player:CppGetCamp()
	local tongId = Player.m_Properties:GetTongID()
	local data = {}
	data["tongId"] = tongId
	data["IsSetServerId"] = true
	local function CallBack(sceneName, sceneType, stationId, result, warZoneId)
		local pos = nil
		Gas2Gac:RetOpenAutoSeekWndBegin(Conn)
		   --任意
		for _, npcName in pairs(npcNameTbl[nCamp]) do
			Gas2Gac:RetOpenAutoSeekWnd(Conn, npcName,0)
		end
		if g_WarZoneMgr.m_CurrentTransportState == "Transporting"  then   --运输时间
			if not stationId then
				pos = g_TongFbMgr:GetEnterPos()
				if pos and sceneType and sceneName and tongId then
					Gas2Gac:RetStationSeekWnd(Conn,sceneType, 0, sceneName, pos[1], pos[2], tongId, warZoneId)
				end
			end
			if stationId then
				if sceneType then
					if sceneType == 7 then
						pos = g_WarZoneMgr:GetStationPos(stationId)
						Gas2Gac:RetStationSeekWnd(Conn,sceneType, stationId, sceneName, pos[1], pos[2], tongId, warZoneId)
					else
						pos = g_TongFbMgr:GetEnterPos()
						Gas2Gac:RetStationSeekWnd(Conn,sceneType, 0, sceneName, pos[1], pos[2], tongId, warZoneId)
					end
				end
			end
			if not result then
				return
			end
			
			for _,info in pairs(result) do
				local stationID = info[1]
				local camp = info[2]
				local wzId = info[3]
				if camp == nCamp then
					if warZoneId == wzId then
						if stationId then
							if stationId ~= stationID then
								Gas2Gac:RetOpenAutoSeekWnd(Conn, stationID, wzId)
							end
						else
							Gas2Gac:RetOpenAutoSeekWnd(Conn, stationID, wzId)
						end
					else
						Gas2Gac:RetOpenAutoSeekWnd(Conn, stationID, wzId)
					end
				end
			end
		else
			if not result then
				return
			end
			for _,info in pairs(result) do
				local stationID = info[1]
				local camp = info[2]
				local wzId = info[3]
				if camp == nCamp then
					Gas2Gac:RetOpenAutoSeekWnd(Conn, stationID, wzId)
				end
			end
		end
	end
	CallDbTrans("GasTongMarketDB", "GetStationTimeInfo", CallBack, data, tongId)
end

function GetFbStationAutoSeekInfo(Conn)
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return 
	end
	local nCamp = Player:CppGetCamp()
	local tongId = Player.m_Properties:GetTongID()
	local data = {}
	data["tongId"] = tongId
	data["IsSetServerId"] = true
	local function CallBack(sceneName)
		local pos = nil
		pos = g_TongFbMgr:GetEnterPos()
		Gas2Gac:RetFbStationAutoSeek(Conn, sceneName, pos[1], pos[2])
	end
	CallAccountAutoTrans(Conn.m_Account, "GasTongMarketDB", "GetFbStationInfo", CallBack, data, tongId)
end

function GetIssueFetchInfo(Conn)
	if not IsCppBound(Conn) then
		return 
	end
	if not IsCppBound(Conn.m_Player) then
		return 
	end
	local tongId = Conn.m_Player.m_Properties:GetTongID()
	local charId = Conn.m_Player.m_uID
	local data = {}
	data["tong_id"] = tongId
	data["charId"] = charId
	local function CallBack(result)
		if not result then
			return 
		end
		if not IsTable(result) then
			MsgToConn(Conn, result)
			return
		end
		local money = result[1]
		local line = result[2]
		local playerTbl = result[3]
		local tongName = result[4]
		local time = StationTimeModulus[line]
		g_IssueForage[tongId] = true
		Gas2Gac:RetShowIssueWnd(Conn,time)
		local num = playerTbl:GetRowNum()
		for i = 0, num - 1 do
			local a = playerTbl:GetData(i,0)
			MsgToConnById(playerTbl:GetData(i,0), 9348, tongName)
		end
	end
	CallAccountAutoTrans(Conn.m_Account, "GasTongBasicDB", "GetTongMoneyAndLine", CallBack, data, tongId)
end

function ChangeTongScene(Conn)
	local Player = Conn.m_Player
	local nCamp = Player:CppGetCamp()
	local tongId = Conn.m_Player.m_Properties:GetTongID()
	if tongId == 0 then
		return
	end
	
	local function CallBack(sceneType, sceneId, serverId, warZoneId, stationId)
		if sceneId == 0 then
			return
		end
		if IsCppBound(Conn) and IsCppBound(Player) then
			local scene = CSceneMgr:GetScene(sceneId)
			local pos
			if sceneType == 6 then
				pos = g_TongFbMgr:GetEnterPos()
			elseif sceneType == 7 then
				pos = g_WarZoneMgr:GetStationEnterPos(stationId, nCamp)
			end
			if sceneId ~= Player.m_Scene.m_SceneId then
				MultiServerChangeScene(Conn, sceneId, serverId, pos[1], pos[2])
			else
				local PixelPos = CFPos:new()
				PixelPos.x =  pos[1] * EUnits.eGridSpanForObj
				PixelPos.y =  pos[2] * EUnits.eGridSpanForObj
				Player:SetPixelPos(PixelPos)
				if not Player:CppIsLive() then
					Player:Reborn(1)
				end
			end
		end
	end
	
	local data = {}
	data["tongId"] = tongId
	data["IsSetServerId"] = true
	CallAccountAutoTrans(Conn.m_Account, "WarZoneDB", "GetTongStationInfo", CallBack, data, tongId)
	
end


function ChangeTongSceneByTrap(Conn)
	local Player = Conn.m_Player
	local nCamp = Player:CppGetCamp()
	local tongId = Conn.m_Player.m_Properties:GetTongID()
	if tongId == 0 then
		return
	end
	
	local function CallBack(sceneType, sceneId, serverId, warZoneId, stationId)
		if sceneId == 0 then
			return
		end
		if IsCppBound(Conn) and IsCppBound(Conn.m_Player) then
			local scene = CSceneMgr:GetScene(sceneId)
			local pos
			if sceneType == 6 then
				pos = g_TongFbMgr:GetEnterPos()
			elseif sceneType == 7 then
				return 
			end
			if sceneId ~= Player.m_Scene.m_SceneId then
				MultiServerChangeScene(Conn, sceneId, serverId, pos[1], pos[2])
			else
				local PixelPos = CFPos:new()
				PixelPos.x =  pos[1] * EUnits.eGridSpanForObj
				PixelPos.y =  pos[2] * EUnits.eGridSpanForObj
				Player:SetPixelPos(PixelPos)
				if not Player:CppIsLive() then
					Player:Reborn(1)
				end
			end
		end
	end
	
	local data = {}
	data["tongId"] = tongId
	data["IsSetServerId"] = true
	CallAccountAutoTrans(Conn.m_Account, "WarZoneDB", "GetTongStationInfo", CallBack, data, tongId)
	
end

--进入帮会
function Gac2Gas:EnterTongArea(Conn, npcId)
	
	local TongID = Conn.m_Player.m_Properties:GetTongID()
	if TongID == 0 then
		MsgToConn(Conn, 9401)
		return 
	end
	if not CheckAllNpcFunc(Conn.m_Player, npcId, "进入帮会") then
		return
	end

	ChangeTongScene(Conn)
	
end

--离开驻地(副本驻地和花语平原驻地)
function Gac2Gas:LeaveTongArea(Conn, npcId)
	local player = Conn.m_Player
	local TongID = player.m_Properties:GetTongID()
	if TongID == 0 then
		MsgToConn(Conn, 9401)
		return 
	end
	
	if not CheckAllNpcFunc(Conn.m_Player, npcId, "帮会驻地传送处") then
		return
	end
	
	local sceneName = player.m_MasterSceneName
	local pos = player.m_MasterScenePos
	ChangeSceneByName(Conn, sceneName, pos.x, pos.y)
end
