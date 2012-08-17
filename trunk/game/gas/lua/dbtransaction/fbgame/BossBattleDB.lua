cfg_load "fb_game/BossBattle_Server"
gac_gas_require "relation/tong/TongMgr"

--"Boss争夺战"

local g_TongMgr = g_TongMgr or CTongMgr:new()
local os = os
local event_type_tbl = event_type_tbl
local BossBattleDB = CreateDbBox(...)

function BossBattleDB.CreateBossBattleScene(data)
	local SceneMgrDB = RequireDbBox("SceneMgrDB")
	local SceneInfoTbl = {}
	for i=1, #data do
		local SceneTbl = SceneMgrDB.GetSceneFromDbByName( data[i] )
		if #SceneTbl > 1 then
			assert(false, "Boss争夺战出现重复场景")
		elseif #SceneTbl == 1 then
			local nowDate = os.date("*t", os.time())
			local sceneDate = os.date("*t", SceneTbl[1][3])
			if nowDate.day ~= sceneDate.day then
				SceneMgrDB._DeleteScene{SceneTbl[1][1]}
				local SceneId, ServerId = SceneMgrDB._CreateScene{["SceneName"] = data[i]}
				SceneInfoTbl[i] = {data[i], SceneId, ServerId}
			else
				SceneInfoTbl[i] = {data[i], SceneTbl[1][1], SceneTbl[1][2]}
			end
		else
			local SceneId, ServerId = SceneMgrDB._CreateScene{["SceneName"] = data[i]}
			SceneInfoTbl[i] = {data[i], SceneId, ServerId}
		end
	end
	return SceneInfoTbl
end

function BossBattleDB.CanJoinBossBattle(data)
	local GasTongBasicDB = RequireDbBox("GasTongBasicDB")
	local TongType = GasTongBasicDB.GetTongType(data["TongId"])
	local IsBattleTong = (TongType == g_TongMgr:GetTongTypeByName("战斗") or TongType == g_TongMgr:GetTongTypeByName("精英战斗"))
	return GasTongBasicDB.GetTongLevel(data["TongId"]), IsBattleTong
end

function BossBattleDB.BossBattleAward(data)
	if not data["KillerId"] or data["KillerId"] == 0 or data["Proffer"] == 0 then
		return {}
	end
	
	local GasTongBasicDB = RequireDbBox("GasTongBasicDB")
	local SceneMgrDB = RequireDbBox("SceneMgrDB")
	local AreaFbPointDB = RequireDbBox("AreaFbPointDB")
	
	local KillerId = data["KillerId"]
	local SceneName = data["SceneName"]
	local Proffer = data["Proffer"]
	local SceneId = data["SceneId"]
	local TongId = GasTongBasicDB.GetTongID(KillerId)
	local AwardPlayerTbl = {}
	
	if TongId~=0 then
		GasTongBasicDB.UpdateTongBoss{["AddNum"] = 1, ["TongId"] = TongId}
		GasTongBasicDB.UpdateTongCharBoss{["AddNum"] = 1, ["CharId"] = KillerId}
		
		local Members = GasTongBasicDB.GetTongOnlineCharId(TongId)
		for _, CharId in pairs(Members) do
			local CharSceneId = SceneMgrDB.GetPlayerCurScene(CharId)
			if CharSceneId == SceneId then
				AreaFbPointDB.AddAreaFbPointByType(CharId,Proffer,10,SceneName,nil,event_type_tbl["Boss争夺战奖励"])
				AwardPlayerTbl[CharId] = true
			end
		end
	else
		AreaFbPointDB.AddAreaFbPointByType(KillerId,Proffer,10,SceneName,nil,event_type_tbl["Boss争夺战奖励"])
		AwardPlayerTbl[KillerId] = true
	end
	
	return AwardPlayerTbl
end

SetDbLocalFuncType(BossBattleDB.CanJoinBossBattle)
SetDbLocalFuncType(BossBattleDB.BossBattleAward)
return BossBattleDB
