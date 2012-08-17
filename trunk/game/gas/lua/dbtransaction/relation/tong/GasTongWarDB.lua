gac_gas_require "relation/tong/TongMgr"
gac_gas_require "war/WarZoneBasic"
local LogErr = LogErr
local g_TongMgr = CTongMgr:new()
local WarZoneBasic = CWarZoneBasic:new()
local StmtOperater = {}	
local event_type_tbl = event_type_tbl
------------------------------------
--闭包开始
local CTongWarBox = CreateDbBox(...)
local CHALLENGE_STATE =
	{
		["Challenging"] = 1,
		["ChallengeSucc"] = 2,
	}
local CHALLENGE_BATTLE_STATE =
	{
		["eChlgList"] =1,
		["eReady"] = 2,
		["ePlay"] = 3,
		["eClose"] =4,
	}
local WarZoneStation = {}
WarZoneStation[1] = 111
WarZoneStation[2] = 121
WarZoneStation[3] = 122
WarZoneStation[4] = 123
WarZoneStation[5] = 211
WarZoneStation[6] = 221
WarZoneStation[7] = 222
WarZoneStation[8] = 223
WarZoneStation[9] = 311
WarZoneStation[10] = 321
WarZoneStation[11] = 322
WarZoneStation[12] = 323

--竞标小队等级限制
local BID_LEVEL_SELF_LINE_2 = 5
local BID_LEVEL_SELF_LINE_1 = 6
local BID_LEVEL_ENEMY_LINE_1 = 8
local BID_LEVEL_ENEMY_LINE_2 = 10

--------------------------------------------------------------------------------
local StmtDef = {
    	"_GetAllTongCapInfo",
    	[[ 
    		select 	T.t_uId,T.t_sName,C.c_sName,W.wz_uId,W.wzs_uIndex
    		from 		tbl_tong as T,tbl_member_tong as M,
    						tbl_char as C,tbl_war_zone_station as W
    		where 	
    				C.cs_uId = M.cs_uId
    		and M.t_uId = T.t_uId
    		and W.t_uId = T.t_uId
    		and M.mt_sPosition = ?
    		and T.t_uCamp = (select t_uCamp from tbl_tong,tbl_member_tong 
    											where tbl_tong.t_uId = tbl_member_tong.t_uId 
    											and tbl_member_tong.cs_uId = ?)
    		group by T.t_uId
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetTongNameAndLeader",
    	[[ 
    		select 	T.t_sName,C.c_sName
    		from 		tbl_tong as T,tbl_member_tong as M,
    						tbl_char as C
    		where
    				C.cs_uId = M.cs_uId
    		and M.t_uId = T.t_uId
    		and T.t_uId = ?
    		and M.mt_sPosition = ?
    		group by T.t_uId
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetCountActiveTong",
    	[[ 
    		select count(*) from tbl_tong_challenge where tc_nActiveTongID = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_AddChlgInfo",
    	[[ 
    		insert into tbl_tong_challenge(tc_nActiveTongID,tc_nPassiveWzId,tc_nPassiveWzsIndex,tc_nChlgMoney,tc_nState,tc_nTime,tc_nActiveCsId) values(?,?,?,?,?,now(),?)
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_UpdateChallengeInfo",
    	[[ 
    		update tbl_tong_challenge set tc_nState = ? where tc_nActiveTongID = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetTongChallengeState",
    	[[ 
    		select tc_nState from tbl_tong_challenge where tc_nActiveTongID = ?
    	]]
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_ClearTongChallengeInfo",
    	[[ 
    		delete from tbl_tong_challenge
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetAllTgtStationInfo",
    	[[ 
    		select tc_nPassiveWzId,tc_nPassiveWzsIndex from tbl_tong_challenge
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_SelectChallengeInfoByState",
    	[[ 
    		select tc_nActiveTongID,tc_nChlgMoney,tc_nActiveCsId from tbl_tong_challenge
    		where tc_nState = ?
    	]]
}
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetChlgOwner",
    	[[ 
    		select tc_nActiveTongID,tc_nActiveCsId from tbl_tong_challenge
    		where
    				tc_nPassiveWzId = ?
    		and tc_nPassiveWzsIndex = ?
    		order by tc_nChlgMoney desc, tc_nTime asc
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetTongChallengeBattleCount",
    	[[ 
    		select count(*) from tbl_tong_challenge_battle
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_AddChallengeBattle",
    	[[ 
    		insert into tbl_tong_challenge_battle(tcb_nActiveTongID, tcb_nActiveCsId, tcb_nPassiveTongID, tcb_nPassiveWzId, tcb_nPassiveWzsIndex, tcb_nSceneId, tcb_nServerId, tcb_nState, tcb_nCamp) values(?,?,?,?,?,?,?,?,?)
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_UpdateChallengeBattleState",
    	[[ 
    		update tbl_tong_challenge_battle set tcb_nState = ?
    		where tcb_nServerId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_UpdateTongChallengeBattleState",
    	[[ 
    		update tbl_tong_challenge_battle set tcb_nState = ?
    		where tcb_nActiveTongID = ?
    		and tcb_nPassiveTongID = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_UpdateTongChallengeBattleSceneId",
    	[[ 
    		update tbl_tong_challenge_battle set tcb_nSceneId = ?
    		where tcb_nActiveTongID = ?
    		and tcb_nPassiveTongID = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetChallengeBattleInfoByServerId",
    	[[ 
    		select tcb_nActiveTongID,tcb_nActiveCsId, tcb_nPassiveTongID, tcb_nSceneId from tbl_tong_challenge_battle
    		where tcb_nServerId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetTongChallengeBattleInfo",
    	[[ 
    		select
    			tcb_nActiveTongID, tcb_nPassiveTongID, tcb_nPassiveWzId, tcb_nPassiveWzsIndex, tcb_nSceneId, tcb_nServerId, tcb_nState, tcb_nCamp 
    		from
    			tbl_tong_challenge_battle
    		where
    			tcb_nActiveTongID = ? or tcb_nPassiveTongID = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetAllChallengeBattleInfo",
    	[[ 
    		select tcb_nActiveTongID, tcb_nPassiveTongID, tcb_nPassiveWzId, tcb_nPassiveWzsIndex from tbl_tong_challenge_battle
    		where tcb_nCamp = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetChallengeBattleTongNameByServerId",
    	[[ 
    		select tcb_nActiveTongID, tcb_nPassiveTongID from tbl_tong_challenge_battle
    		where tcb_nServerId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_ClearChallengeBattleInfoByServerId",
    	[[ 
    		delete from tbl_tong_challenge_battle
    		where tcb_nServerId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_DeleteTongChallengeBattleInfo",
    	[[ 
    		delete from tbl_tong_challenge_battle where
    		tcb_nActiveTongID = ?
    		and tcb_nPassiveTongID = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = 
{
	"GetCharOnlineServer",
	[[
		select 
			co_uOnServerId
		from
			tbl_char_online
		where
			cs_uId = ?
	]]
}
DefineSql ( StmtDef, StmtOperater )

--------------------------------------------RPC调用相关-------------------------------------------------
--【所有前线战场的团队信息】
function CTongWarBox.GetTongChallengeInfoList(parameter)
	local uTongId = parameter["uTongId"]
  local uSceneType = parameter["uSceneType"]
	local uCamp = parameter["uCamp"]
	local uWarZoneId = parameter["uWarZoneId"]
	local uStationId = parameter["uStationId"]
 	
 	local WarZoneDB = RequireDbBox("WarZoneDB")
 	local uTongLine = WarZoneDB.GetTongRelativeLine(uTongId)
 	
 	local res = {}
 	if uSceneType == 6 then --如果挑战者驻地在副本中
 		local WarZoneNum = WarZoneDB.GetWarZoneCount()
 		for wzId = 1, WarZoneNum do
 			CTongWarBox.GetTongChlgInfoListByWzId(wzId, uStationId, uCamp, uTongLine, WarZoneDB, res)
 		end
 	else --如果挑战者驻地在花语平原
 		CTongWarBox.GetTongChlgInfoListByWzId(uWarZoneId, uStationId, uCamp, uTongLine, WarZoneDB, res)
 	end
 	local selfWz
 	if not uWarZoneId then
 		selfWz = 0
 	else
 		selfWz = uWarZoneId
 	end
 	local selfWzs
 	if not uStationId then
 		selfWzs = 0
 	else
 		selfWzs = uStationId
 	end
	return res, selfWz, selfWzs
end

function CTongWarBox.GetTongChlgInfoListByWzId(wzId, uWzsId, uCamp, uTongLine, WarZoneDB, res)
	local result
	local data = {}
	data["WarZoneId"] = wzId
	local campInfoTbl = WarZoneDB.GetStationCampInfo(data)
 	local tongInfoTbl = WarZoneDB.GetStationTongInfo(data)
 	local uWzsCamp
 	for _, wzsId in pairs(WarZoneStation) do
 		if not uWzsId or uWzsId and uWzsId ~= wzsId then
	 		local IsOccupyed = tongInfoTbl[wzsId]
	 		if IsOccupyed then -- 有帮会占领此驻地
	 			uWzsCamp = campInfoTbl[wzsId]
	 		else
	 			uWzsCamp = WarZoneBasic:GetStationDefaultCamp(wzsId)
	 		end
	 		
	 		if uCamp == uWzsCamp then --相同阵营才能挑战
	 			local stationLine = -1
	 			if IsOccupyed then -- 有帮会占领此驻地
	 				stationLine = WarZoneDB.GetTongRelativeLine(IsOccupyed)
	 			else
	 				stationLine = 3 - WarZoneBasic:GetStationWarLine(wzsId)
	 			end
	 			if stationLine == uTongLine + 1 then --只能挑战前一线驻地
	 				if IsOccupyed then --如果有帮会占领此驻地
	 					result = StmtOperater._GetTongNameAndLeader:ExecSql('s[32]s[18]',IsOccupyed,g_TongMgr:GetPosIndexByName("团长"))
	 					if result then
	 						table.insert(res, {result:GetData(0,0), result:GetData(0,1), wzId, wzsId})
	 						result:Release()
	 					else
	 						table.insert(res, {"", "", wzId, wzsId})
	 					end
	 				else
	 					table.insert(res, {"", "", wzId, wzsId})
	 				end
	 			end
	 		end
	 	end
 	end
end

--【帮会宣战】
function CTongWarBox.SendChallenge(parameters)
	local nMoney 					= parameters["nMoney"]
	local uPlayerID 			= parameters["uPlayerID"]
	local uWarZone				= parameters["uWarZone"]
	local uWarZoneStation	= parameters["uWarZoneStation"]
	local PurviewName			= parameters["PurviewName"]
	
	local tong_box = RequireDbBox("GasTongBasicDB")
	if not tong_box.JudgePurview(uPlayerID, PurviewName) then
		return 9006
	end
	local uTongID = tong_box.GetTongID(uPlayerID)
	
	local CTongBasicBox = RequireDbBox("GasTongBasicDB")
	local tongType = CTongBasicBox.GetTongType(uTongID)
	if tongType ~= g_TongMgr:GetTongTypeByName("精英战斗") then
		return 9168
	end
	
	local Count = StmtOperater._GetCountActiveTong:ExecSql('n',uTongID)
	if Count:GetNumber(0,0) > 0 then
		--不能多次竞标
		return 9187
	end
	if nMoney > 0 then
		if not tong_box.UpdateTongMoney({["uTongID"]=uTongID,["uMoney"]= -nMoney,["nEventType"] = event_type_tbl["佣兵小队宣战"]}) then
			CancelTran()
		--资金不足
		return 9181
		end
	end
		
	-- 竞标小队等级限制：我方1线大于等于6级，敌方1线大于等于7级，敌方2线大于等于8级
	local camp = tong_box.GetTongCampByIDRpc({["TongID"] = uTongID})
	local level = tong_box.GetTongLevel(uTongID)
	local stationLine = WarZoneBasic:GetStationWarLine(uWarZoneStation)
	local stationCamp = WarZoneBasic:GetStationDefaultCamp(uWarZoneStation)
	if camp == stationCamp then
		if stationLine == 1 and level < BID_LEVEL_SELF_LINE_1 then
			return 9288, BID_LEVEL_SELF_LINE_1
		end
	else
		if (stationLine == 1 and level < BID_LEVEL_ENEMY_LINE_1) then
			return 9288, BID_LEVEL_ENEMY_LINE_1
		end
		if (stationLine == 2 and level < BID_LEVEL_ENEMY_LINE_2) then
			return 9288, BID_LEVEL_ENEMY_LINE_2
		end
	end

	StmtOperater._AddChlgInfo:ExecSql('',uTongID,uWarZone,uWarZoneStation,nMoney,CHALLENGE_STATE["Challenging"],uPlayerID)
	
	--写LOG
	local WarZoneDB = RequireDbBox("WarZoneDB")
	local uDfnderTongID = WarZoneDB.GetStationOwnerTong(uWarZone, uWarZoneStation)
	local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.TongChallengeStartLog(uPlayerID, uTongID, uDfnderTongID, uWarZone, uWarZoneStation)

	return true
end

function CTongWarBox.ClearTongChallengeInfo()
	StmtOperater._ClearTongChallengeInfo:ExecSql('')
	return (g_DbChannelMgr:LastAffectedRowNum() > 0)
end

function CTongWarBox.GetOnlineServerByCharId(nCharId)
	local res = StmtOperater.GetCharOnlineServer:ExecStat(nCharId)
	if res:GetRowNum() > 0 then
		return true, res:GetData(0, 0)
	else
		return false
	end
end

function CTongWarBox.ReturnCompeteMoneyToFailer()
	local nState = CHALLENGE_STATE["Challenging"]
	local resTbl = {}
	local result = StmtOperater._SelectChallengeInfoByState:ExecStat(nState)
	local GasTongBasicDB = RequireDbBox("GasTongBasicDB")
	
	local nActiveTongId, nChlgMoney, nActiveCsId
	for i = 0, result:GetRowNum() - 1 do
		nActiveTongId, nChlgMoney, nActiveCsId = result:GetData(i, 0), result:GetData(i, 1), result:GetData(i, 2)
		local bRet = GasTongBasicDB.UpdateTongMoney({["uTongID"] = nActiveTongId, ["uMoney"] = nChlgMoney,["nEventType"] = event_type_tbl["佣兵小队宣战竞标失败"]})
		if not bRet then
			CancelTran()
			return false
		end
		local flag, nServerId = CTongWarBox.GetOnlineServerByCharId(nActiveCsId)
		if flag then
			table.insert(resTbl, {nActiveCsId, nServerId})
		end
	end
	return true, resTbl
end

function CTongWarBox.ReturnMoneyAndClearChallengeInfo(parameter)
	local Flag, resTbl = CTongWarBox.ReturnCompeteMoneyToFailer()
	if not Flag then
		return false
	end
	Flag = CTongWarBox.ClearTongChallengeInfo()
	return Flag, resTbl
end

function CTongWarBox.UpdateSelfServerBattleStateAndSendMsg(parameter)
	CTongWarBox.UpdateChallengeBattleStateByServerId(parameter)
	local serverId = parameter["uServerId"]
	local GasTongBasicDB = RequireDbBox("GasTongBasicDB")
	local resTbl = {}
	local result = StmtOperater._GetChallengeBattleTongNameByServerId:ExecStat(serverId)
	
	for index = 0, result:GetRowNum() - 1 do
		local chlgerTongId = result:GetData(index, 0)
		local tgtTongId = result:GetData(index, 1)
		local Flag, memberSet = GasTongBasicDB.GetTongOnlineMemberInfo({["uTongID"] = chlgerTongId})
		
		if Flag then
			table.insert(resTbl, memberSet)
		end
		
		Flag, memberSet = GasTongBasicDB.GetTongOnlineMemberInfo({["uTongID"] = tgtTongId})
		
		if Flag then
			table.insert(resTbl, memberSet)
		end
	end
	
	return resTbl
end

function CTongWarBox.AddWinLogAndSendMsg(parameter)
	local uTongId = parameter["uTongID"]
	local GasTongBasicDB = RequireDbBox("GasTongBasicDB")
	local ex = RequireDbBox("Exchanger") 
	local sMsg = "2023"
	local CTongBasicBox = RequireDbBox("GasTongBasicDB")
 	if not CTongBasicBox.AddTongLogs(uTongId,sMsg, g_TongMgr:GetLogType("战争")) then
   	return false
  end
	return GasTongBasicDB.GetTongOnlineMemberInfo(parameter)
end

function CTongWarBox.ChallengeEndUpdateHonor(parameter)
	local ChlgerTongId = parameter["uChlgTongId"]
	local tgtTongId = parameter["uTgtTongId"]
	local uChlgerHonor = parameter["uChlgerHonor"]
	local uTgtHonor = parameter["uTgtHonor"]
	
	local GasTongBasicDB = RequireDbBox("GasTongBasicDB")
	GasTongBasicDB.UpdateHonor({["uTongID"] = ChlgerTongId, ["uHonor"] = uChlgerHonor, ["nEventType"] = 129})
	
	if tgtTongId ~= 0 then
		GasTongBasicDB.UpdateHonor({["uTongID"] = tgtTongId, ["uHonor"] = uTgtHonor, ["nEventType"] = 129})
	end
	
	return true
end

function CTongWarBox.UpdateHonorAndBattleState(parameter)
	local ChlgerTongId = parameter["uChlgerTongId"]
	local tgtTongId = parameter["uTgtTongId"]
	local ChlgerHonor = parameter["uChlgerHonor"]
	local tgtHonor	= parameter["uTgtHonor"]
	local nState = parameter["uState"]
	
	local GasTongBasicDB = RequireDbBox("GasTongBasicDB")
	GasTongBasicDB.UpdateHonor({["uTongID"] = ChlgerTongId, ["uHonor"] = ChlgerHonor, ["nEventType"] = 130})
	
	if tgtTongId ~= 0 then
		GasTongBasicDB.UpdateHonor({["uTongID"] = tgtTongId, ["uHonor"] = tgtHonor, ["nEventType"] = 130})
	end
	
	StmtOperater._UpdateTongChallengeBattleState:ExecStat(nState, ChlgerTongId, tgtTongId)
end

--这个是为生成对战表用的,按相对战线保存
function CTongWarBox.GetAllLineChallengeInfo()
	local chlgTbl = {}
	local result = StmtOperater._GetAllTgtStationInfo:ExecSql('nn')
	local data = {}
	local WarZoneDB = RequireDbBox("WarZoneDB")
	local tongInfoTbl = WarZoneDB.GetAllStationTongInfo(data)
	
	for index = 0, result:GetRowNum() -1 do
		local tgtWzId = result:GetData(index, 0)
		local tgtWzsId = result:GetData(index, 1)
		local OccupyedTong = tongInfoTbl[tgtWzId][tgtWzsId]
		local stationLine
		if OccupyedTong then
			stationLine = WarZoneDB.GetTongRelativeLine(OccupyedTong)
		else
			stationLine = 3 - WarZoneBasic:GetStationWarLine(tgtWzsId)
		end
		
		if not chlgTbl[stationLine] then
			chlgTbl[stationLine] = {}
		end
		if not chlgTbl[stationLine][tgtWzId] then
			chlgTbl[stationLine][tgtWzId]	= {}
		end
		if OccupyedTong then
			chlgTbl[stationLine][tgtWzId][tgtWzsId] = OccupyedTong
		else
			chlgTbl[stationLine][tgtWzId][tgtWzsId] = 0
		end
	end
	result:Release()
	return chlgTbl
end

function CTongWarBox.MakeChallengeList(parameter)
	local resTbl = {}
	local chlgTbl = CTongWarBox.GetAllLineChallengeInfo()
	local	WzsIsChlged = {}
	local	TongIsChlg = {}
	local	ChallengeTbl = {}
	for line = 1, 4 do
		local tbl = chlgTbl[line]
		if tbl then
			for wzId, wzsTbl in pairs(tbl) do
				if not WzsIsChlged[wzId] then
					WzsIsChlged[wzId] = {}
				end
				for wzsId, OccupyedTong in pairs(wzsTbl) do
					if not WzsIsChlged[wzId][wzsId] and OccupyedTong ~= 0 and not TongIsChlg[OccupyedTong] or
						 not WzsIsChlged[wzId][wzsId] and OccupyedTong == 0 then
						local parameter = {}
						parameter["tgtWzId"] = wzId
						parameter["tgtWzsId"] = wzsId
						parameter["tgtTongId"] = OccupyedTong
						parameter["TongIsChlg"] = TongIsChlg
						local Flag, ChlgerTongId, ChlgerWzId, ChlgerWzsId, tgtTongId, tgtWzId, tgtWzsId, camp, ChlgerCsId = CTongWarBox.GetChlgOwner(parameter)
						
						if Flag then
							TongIsChlg[ChlgerTongId] = 0
							TongIsChlg[tgtTongId] = 0
							if not WzsIsChlged[tgtWzId][tgtWzsId] then
								WzsIsChlged[tgtWzId][tgtWzsId] = 0
							end
							if ChlgerWzId and ChlgerWzsId then
								WzsIsChlged[ChlgerWzId][ChlgerWzsId] = 0
							end
							
							local SceneMgrDB = RequireDbBox("SceneMgrDB")
							local data = {}
							data["SceneName"] = "帮会挑战本"
							data["Proc"] = 0
							local otherArg = {}
							otherArg["uChlgerTongId"] = ChlgerTongId
							otherArg["uTgtWarZoneId"] = tgtWzId
							otherArg["uTgtStationId"] = tgtWzsId
							otherArg["uTgtTongId"] = tgtTongId
							otherArg["uCamp"] = camp
							data["OtherArg"] = otherArg
							
							local sceneId, serverId = SceneMgrDB._CreateScene(data)
							
							if not sceneId then
								LogErr("帮会挑战错误", "创建场景不成功, sceneId:" .. sceneId .. ", serverId:" .. serverId)
								CancelTran()
								return false
							end
							local data = {}
							data["uChlgerTongId"] = ChlgerTongId
							data["uTgtTongId"] = tgtTongId
							data["uTgtWzId"] = tgtWzId
							data["uTgtWzsId"] = tgtWzsId
							data["uSceneId"] = sceneId
							data["serverId"] = serverId
							data["uChlgerCsId"] = ChlgerCsId
							data["uCamp"] = camp
							local nFlag = CTongWarBox.AddChallengeBattle(data)
							if not nFlag then
								CancelTran()
								return false
							end
							local flag, nServerId = CTongWarBox.GetOnlineServerByCharId(ChlgerCsId)
							if flag then
								table.insert(resTbl, {ChlgerCsId, nServerId})
							end
						end
					end
				end
			end
		end
	end
	
	return true, resTbl
end

function CTongWarBox.GetChlgOwner(parameter)
	local tgtWzId = parameter["tgtWzId"]
	local tgtWzsId = parameter["tgtWzsId"]
	local tgtTongId = parameter["tgtTongId"]
	local TongIsChlgTbl = parameter["TongIsChlg"]
	
	local WarZoneDB = RequireDbBox("WarZoneDB")
	local result = StmtOperater._GetChlgOwner:ExecSql('nn', tgtWzId, tgtWzsId)
	local ChlgerTongId, ChlgerWzId, ChlgerWzsId, ChlgerCsId
	
	for index = 0, result:GetRowNum() -1 do
		ChlgerTongId = result:GetData(index,0)
		ChlgerCsId = result:GetData(index,1)
		ChlgerWzId, ChlgerWzsId = WarZoneDB.GetTongWzAndWzs(ChlgerTongId)
		if not TongIsChlgTbl[ChlgerTongId] then
			break
		end
	end
	if TongIsChlgTbl[ChlgerTongId] then
		return false
	end
	
	result:Release()
	
	local camp
	if ChlgerTongId then
		local GasTongBasicDB = RequireDbBox("GasTongBasicDB")
		camp = GasTongBasicDB.GetTongCampByIDRpc({["TongID"] = ChlgerTongId})
	end
	return true, ChlgerTongId, ChlgerWzId, ChlgerWzsId, tgtTongId, tgtWzId, tgtWzsId, camp, ChlgerCsId
end

function CTongWarBox.GetTongChallengeBattleCount(parameter)
	local result = StmtOperater._GetTongChallengeBattleCount:ExecSql("n")	
	local Count = result:GetData(0, 0)
	result:Release()
	if Count > 0 then
		local resTbl = {}
		local battleInfo = CTongWarBox.GetChallengeBattleInfoByServerId(parameter)
		for index = 1, #battleInfo do
			local chlgerTongId, chlgerCsId, tgtTongId, sceneId, serverId = battleInfo[index][1], battleInfo[index][2], battleInfo[index][3], battleInfo[index][4], battleInfo[index][5]
			local flag, nServerId = CTongWarBox.GetOnlineServerByCharId(chlgerCsId)
			if flag then
				table.insert(resTbl, {chlgerCsId, nServerId})
			end
			
			local SceneMgrDB = RequireDbBox("SceneMgrDB")
			local isExist = SceneMgrDB.IsSceneExistent({["sceneId"] = sceneId, ["serverId"] = serverId})
			if not isExist then
				local sceneId, serverId = SceneMgrDB._CreateScene({["SceneName"] = "帮会挑战本", ["Proc"] = 0})
				if not sceneId then
					LogErr("帮会挑战错误", "CreateScene失败")
					CancelTran()
					return false
				end
				local nFlag = CTongWarBox.UpdateTongChallengeBattleSceneId({["uChlgerTongId"] = chlgerTongId, ["uTgtTongId"] = tgtTongId, ["uSceneId"] = sceneId})
				if not nFlag then
					LogErr("帮会挑战错误", "UpdateTongChallengeBattleSceneId失败")
					CancelTran()
					return false
				end
			end
		end
		return true, resTbl
	else
		local Flag, resTbl = CTongWarBox.MakeChallengeList()
		return Flag, resTbl
	end
end


function CTongWarBox.AddChallengeBattle(parameter)
	local chlgerTongId = parameter["uChlgerTongId"]
	local tgtTongId = parameter["uTgtTongId"]
	local tgtWzId	=	parameter["uTgtWzId"]
	local tgtWzsId = parameter["uTgtWzsId"]
	local sceneId = parameter["uSceneId"]
	local serverId = parameter["serverId"]
	local state = CHALLENGE_BATTLE_STATE["eChlgList"]
	local chlgerCsId = parameter["uChlgerCsId"]
	local camp = parameter["uCamp"]
	StmtOperater._AddChallengeBattle:ExecSql("", chlgerTongId, chlgerCsId, tgtTongId, tgtWzId, tgtWzsId, sceneId, serverId, state, camp)
	
	if g_DbChannelMgr:LastAffectedRowNum() == 0 then
		LogErr("帮会挑战错误", "插入对战表不成功")
		return false
	end
	
	StmtOperater._UpdateChallengeInfo:ExecSql("", CHALLENGE_STATE["ChallengeSucc"], chlgerTongId)
	if g_DbChannelMgr:LastAffectedRowNum() == 0 then
		LogErr("帮会挑战错误", "更新竞标表状态不成功, TongID=" .. chlgerTongId)
		CancelTran()
		return false
	end
	return true
end

function CTongWarBox.GetChallengeBattleInfoByServerId(parameter)
	local serverId = parameter["uServerId"]
	local result = StmtOperater._GetChallengeBattleInfoByServerId:ExecSql("nnnn", serverId)
	
	local resTbl = {}
	for i = 0, result:GetRowNum() - 1 do
		table.insert(resTbl, {result:GetData(i, 0), result:GetData(i, 1), result:GetData(i, 2), result:GetData(i, 3), serverId}) 
	end
	
	result:Release()
	return resTbl
end

function CTongWarBox.GetTongChallengeBattleInfo(parameter)
	local uTongID = parameter["uTongID"]
	local result = StmtOperater._GetTongChallengeBattleInfo:ExecSql("nnnnnnnn", uTongID, uTongID)
	if result:GetRowNum() > 0 then
		local uPlayerID = parameter["uPlayerID"]
		local GasTongBasicDB = RequireDbBox("GasTongBasicDB")	
		if not GasTongBasicDB.JudgePurview(uPlayerID, "Campaigner") then
			return false, 9400
		end
	
		local chlgerTongId, tgtTongId, tgtWarZoneId, tgtStationId, sceneId, serverId, state, camp = result:GetData(0, 0), result:GetData(0, 1), result:GetData(0, 2), result:GetData(0, 3), result:GetData(0, 4), result:GetData(0, 5), result:GetData(0, 6), result:GetData(0, 7)
		result:Release()
		return true, 0, chlgerTongId, tgtTongId, tgtWarZoneId, tgtStationId, sceneId, serverId, state, camp
	else
		return false, 9413
	end
end

function CTongWarBox.UpdateChallengeBattleStateByServerId(parameter)
	local state = parameter["uState"]
	local serverId = parameter["uServerId"]
	StmtOperater._UpdateChallengeBattleState:ExecSql("", state, serverId)
	return (g_DbChannelMgr:LastAffectedRowNum() > 0)
end

function CTongWarBox.UpdateTongChallengeBattleSceneId(parameter)
	local chlgerTongId = parameter["uChlgerTongId"]
	local tgtTongId = parameter["uTgtTongId"]
	local sceneId = parameter["uSceneId"]
	StmtOperater._UpdateTongChallengeBattleSceneId:ExecSql("", sceneId, chlgerTongId, tgtTongId)
	return (g_DbChannelMgr:LastAffectedRowNum() > 0)
end

function CTongWarBox.GetAllChallengeBattleInfo(parameter)
	local nCamp = parameter["uCamp"]
	local result = StmtOperater._GetAllChallengeBattleInfo:ExecSql("nnnn", nCamp)
	local resTbl = {}
	local GasTongBasicDB = RequireDbBox("GasTongBasicDB")
	
	for i = 0, result:GetRowNum() - 1 do
		local chlgerTongId = result:GetData(i, 0)
		local tgtTongId = result:GetData(i ,1)
		local tgtWzId = result:GetData(i, 2)
		local tgtWzsId = result:GetData(i ,3)
		
		local data = {}
		data["ChlgerTongId"] = chlgerTongId
		data["TgtTongId"] = tgtTongId
		
		local Flag, chlgerTongName, tgtTongName = GasTongBasicDB.GetChlgerNameAndTgtName(data)
		table.insert(resTbl, {chlgerTongName, tgtTongName, tgtWzId, tgtWzsId})
	end
	
	result:Release()
	return resTbl
end

function CTongWarBox.ClearSelfServerChallengeBattleInfo(parameter)
	local serverId = parameter["uServerId"]
	StmtOperater._ClearChallengeBattleInfoByServerId:ExecSql("", serverId)
	return (g_DbChannelMgr:LastAffectedRowNum() > 0)
end

function CTongWarBox.DeleteTongChallengeBattleInfo(parameter)
	local chlgerTongId = parameter["uChlgerTongId"]
	local tgtTongId = parameter["uTgtTongId"]
	StmtOperater._DeleteTongChallengeBattleInfo:ExecSql("", chlgerTongId, tgtTongId)
	return (g_DbChannelMgr:LastAffectedRowNum() >0)
end

function CTongWarBox.IsSendChallenge(PlayerId)
	local tong_box = RequireDbBox("GasTongBasicDB")
	local TongId = tong_box.GetTongID(PlayerId)
	local Count = StmtOperater._GetCountActiveTong:ExecSql('n',TongId)
	return Count:GetNumber(0,0) > 0
end
---------------------------------
return CTongWarBox
