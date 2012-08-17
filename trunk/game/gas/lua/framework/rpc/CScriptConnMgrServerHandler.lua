gac_gas_require "framework/rpc/ConnMgrHandler"
gas_require "world/player/CreateServerPlayer"
gas_require "entrance/logout/GasLogout"
gac_gas_require "framework/common/LogoutSetting"
gas_require "framework/main_frame/AppGasHandler"
cfg_load "scene/SceneProperty_Server"
cfg_load "player/RebornPos_Common"
gas_require "log_mgr/CLogMgr"

--CConnMgrServerHandler = class(IConnMgrServerHandler)
CScriptConnMgrServerHandler = class(IScriptConnMgrServerHandler, CConnMgrHandler)

function CScriptConnMgrServerHandler:OnError( Connection, sError )
	print( "Connection " , Connection , " has error " .. sError )
end

function CScriptConnMgrServerHandler:OnAccepted(Connection)

end

local function DelFriendGroupMsgPipe(friendGroupIdSet,Conn)
	if friendGroupIdSet:GetRowNum() > 0 then
		local friendGroupIdTbl = friendGroupIdSet:GetTableSet()
		for i = 1,friendGroupIdSet:GetRowNum() do
			if g_FriendGroupMultiMsgSenderTbl[friendGroupIdTbl(i,1)] then
				g_FriendGroupMultiMsgSenderTbl[friendGroupIdTbl(i,1)]:DelPipe(Conn)
			end
		end
	end
end


local function SetPlayerLogoutData(player, username,AccountID, LeaveGameCallback)
	local function force_logout(msg)
		local uCharID = player.m_uID
		local sCharName = player.m_Properties:GetCharName()
		g_DelPlayerInfo(uCharID, sCharName)
		g_AllConnection[uCharID] = nil
		apcall(player.Destroy, player)
		LogErr("[强制删除角色]出现这个错导致角色无法下线: " .. msg, "charId:" .. uCharID .. " 所在场景:" .. player.m_Scene.m_SceneName)
	end
	
	local function get_save_data()
		local data = {}
		local StateVec,StoredObjVec = player:GetStateTbl(1)
		data["char_id"]		= player.m_uID
		data["playerOnTimeTotal"] = math.abs(os.time() - player.m_PlayerOnTimeBegin)
		data["lastServerTime"] = player.m_PlayerOnTimeBegin
		if player:CppIsLive() or player.m_Scene.m_SceneAttr.RebornType == 8 then
			data["char_hp"] 	= player:CppGetHP()
			data["char_mp"]	= player:CppGetMP()
		elseif not player:CppIsLive() and player.m_Scene.m_SceneAttr.RebornType == 11 then
			data["char_hp"] 	= player:CppGetMaxHP() * 0.3  
			data["char_mp"]	= player:CppGetMaxHP() * 0.3
		else
			data["char_hp"] 	= CStaticAttribMgr_CalcBasicMaxHealthPoint(player:CppGetClass(),150)
			data["char_mp"] 	= CStaticAttribMgr_CalcBasicMaxManaPoint(player:CppGetClass(),150)
		end
		data["StateData"]		= StateVec
		data["StateStoreObjData"]	= StoredObjVec
		data["skillCoolDownData"]= player:GetSkillCoolDownTimeTbl()
		data["AureMagicData"]= player:GetAureMagicDataTbl()
		local bChangeScene = player.m_Conn.m_ChangeToServer ~= nil
		local NewSceneName = player.m_Conn.m_ChangeToSceneName
		data["ServantData"] = player:GetServantDataTbl()
		if bChangeScene and Scene_Basic_Common_Shell(NewSceneName) and Scene_Basic_Common_Shell(NewSceneName,"ClearServant") == 1 then
			data["ServantData"] = {}
		end
		local Truck = player:GetServantByType(ENpcType.ENpcType_Truck)
		if IsServerObjValid(Truck) then
			data["TruckName"] = Truck.m_Properties:GetCharName()
			data["Resource"] = Truck.m_TruckInfo:GetResource()
		end
		local posCur = CFPos:new()
		player:GetPixelPos( posCur )
		data["scene_type"]		= player.m_Scene.m_SceneAttr.SceneType
		data["posCur_x"]		= posCur.x / EUnits.eGridSpanForObj
		data["posCur_y"] 		= posCur.y / EUnits.eGridSpanForObj
		
		if not player:CppIsLive() and player.m_Scene.m_SceneAttr.RebornType == 11 then
			local SceneName = player.m_Scene.m_SceneName
			local nCamp =  player:CppGetCamp()
			local info = RebornPos_Common(SceneName, tostring(nCamp))
			if info then
				local Keys = RebornPos_Common:GetKeys(SceneName, tostring(nCamp))
				for _,p in pairs(Keys) do
					if info(p, "DefaultReborn") == 0 then
						data["posCur_x"]		= info(p, "PosX")
						data["posCur_y"] 		= info(p, "PosY")
					end
				end
			end
		end
		
		if(data["posCur_x"] < 0 or data["posCur_x"] > 1000 or
			 data["posCur_y"] < 0 or data["posCur_y"] > 1000) then
			local msg = "通过 GetPixelPos 得到的坐标为 (" .. posCur.x .." ,".. posCur.y .. ") 要存进数据库的坐标为 (" .. data["posCur_x"] .." ,".. data["posCur_y"] .. ")"
			LogErr("角色坐标异常", msg)
		end
		
		data["scene_id"]		= player.m_Scene.m_SceneId
		data["scene_name"]		= player.m_Scene.m_SceneName
		data["scene_logic_name"] = player.m_Scene:GetLogicSceneName()
		local gridPos = {}
		gridPos.x = math.floor(data["posCur_x"])
		gridPos.y = math.floor(data["posCur_y"])
		data["area_name"] = g_AreaMgr:GetSceneAreaName(data["scene_name"], gridPos)
		data["is_server_shutdown"] = g_bServerShuttingDown
		data["tbl_HighStoneID"] = player.tbl_HighStoneID
		data["killNpcCount"] = player.m_KillNpcCount
		data["killPlayerCount"] = player.m_KillPlayerCount
		data["serverId"] = g_CurServerId
		data["playerOnTimeTotal"] = math.abs(os.time() - player.m_PlayerOnTimeBegin)
		data["lastServerTime"] = player.m_PlayerOnTimeBegin
		data["PkSwitch"] = player.m_SwitchState
		data["ChangeToServer"] = player.m_Conn.m_ChangeToServer
		data["BurstSoulTime"] = player:GetBurstSoulTimes()
		data["SpecialItemCoolDown"] = player.m_SpecialItemCoolDown
		player.m_KillNpcCount = 0 
		player.m_KillPlayerCount = 0 
		player.m_PlayerOnTimeBegin = os.time()
		data["soulCount"] = player.m_uKillDropSoulCount
		data["uSoulBottleValue"] = player.m_uAddSoulBottleValue or 0
		
		data["addSoulType"] = 104
		player.m_uKillDropSoulCount = 0
		data["KillNpcExpCount"] = player.m_KillNpcExp
		player.m_KillNpcExp = 0
		player.m_uAddSoulBottleValue = 0
		data["uTotalPoint"] = player.m_uFightingTotalPoint
		return data
	end
	
	local function Servant_apcall()
		player:ClearServantType(ENpcType.ENpcType_MonsterCard)
		player:ClearServantType(ENpcType.ENpcType_Totem)
	end
	apcall(Servant_apcall)
	
	local ret1, saveData  = apcall(get_save_data)
	if not ret1 then
		force_logout(saveData)
		return
	end
	
	local dataId = nil
	if saveData["ChangeToServer"] then
		dataId = GetDelayDataId()
	end
	
	
	local function Init()
		local get_data = {}
		get_data["char_id"]		= player.m_uID
		get_data["scene_name"] = player.m_Scene.m_SceneName
		get_data["scene_type"] = player.m_Scene.m_SceneAttr.SceneType
		get_data["ChangeToServer"] = saveData["ChangeToServer"]
		get_data["DelayDataId"] = dataId
		
		local data = {}
		data["GetLogoutData"] = get_data
		data["Immediately"] = GetImmediatelySaveData(saveData)
		if not saveData["ChangeToServer"] then
			data["Delay"] = GetDelaySaveData(saveData)
		end
		
		local function callback(result)
			if saveData["ChangeToServer"] then
				SavePlayerLogoutData(saveData, username, AccountID, dataId)
			end
			g_AllDisabledPlayer[player.m_uID] = nil
			if IsCppBound(g_AllPlayerSender) and player then
				g_AllPlayerSender:DelPlayer(player)
			end
			if player and IsCppBound(player) and CampPlayerSenderTbl[player:CppGetBirthCamp()] then
				CampPlayerSenderTbl[player:CppGetBirthCamp()]:DelPlayer(player)
			end
			apcall(DelFriendGroupMsgPipe, result["friendGroupIdTbl"], player.m_Conn)
			apcall(DestroyServerPlayer, player, result, player.m_Conn.m_ChangeToServer ~= nil)
			player.m_bCanCallTrans = nil

			if LeaveGameCallback then
				LeaveGameCallback()
			end
		end

		local function err_callback()
			force_logout("调用CharacterMediatorDB.LogoutSave 出错")
		end
		
		if g_SceneMgr:IsMainScene(saveData["scene_id"]) then
			PCallDbTrans("CharacterMediatorDB", "LogoutSave", callback, err_callback, data, username, AccountID)
		else
			PCallDbTrans("CharacterMediatorDB", "LogoutSave", callback, err_callback, data, username, AccountID,saveData["scene_id"])
		end
	end
	

	apcall(player.UnRegisterPlayerTick, player)

	local ret2, msg = apcall(Init)
	if not ret2 then
		force_logout(msg)
	end
end

function LeaveGame(player, AccountID, username, callback)
	SaveCharLoginFuncFlow(player.m_uID,"LeaveGame")
	local function Init()
		if (not player) or (not IsCppBound(player)) then
			return
		end
		if player.m_PrepareReuse then
			LogErr("在角色处于 准备被重新使用的状态下 调用了 LeaveGame")
			return
		end
		if player.m_Conn.m_ChangeToServer ~= nil then
			apcall(g_PlayerLeaveSceneFb,player)
		else
			apcall(g_PlayerOffLineSceneFb,player)
		end
		SetPlayerLogoutData(player, username, AccountID, callback)
	end
	apcall(Init)
end


function DeleteAccountInfo(Conn)
	if Conn.m_Account.shutdowntick then
		UnRegisterTick(Conn.m_Account.shutdowntick)
		Conn.m_Account.shutdowntick = nil
	end
	local AccountID = Conn.m_Account:GetID()
	Conn.m_Account:Clear()
	g_AccountMgr:DeleteAccount(Conn.m_nGameID, Conn.m_UserName )
	
	CApexProxy_GetInst():NoticeApexProxy_UserLogout(AccountID, Conn.m_UserName)
	if Conn.m_ChangeToServer == nil then
		SaveLogout(AccountID)
	end
	--Conn:LogOnOffLineMsg("user:" .. AccountID .. "\tlua里面下线了")

	local event = Conn.m_Account.m_Event
	event:Set(true)
	Conn.m_Account = nil
end

local function DelayLogout(player, delay, AccountID, username, callback)
	-- delay = 600
	player.m_UserName = username
	g_AllDisabledPlayer[player.m_uID] = player
	
	local function TickCallBack(Tick)
		UnRegisterTick(Tick)
		player.m_LeaveTick = nil
		LeaveGame(player, AccountID, username, callback)
	end
	player.m_LeaveTick = RegisterTick("ConnLeave", TickCallBack, delay*1000)
	player.m_bDisabled = true
end

function Gas2GasDef:LogoutForAdultCheck(Conn, uCharID)
	local player = g_GetPlayerInfo(uCharID)
	if player and IsCppBound(player) then
		local Connection = player.m_Conn
		DelayLogout(player, 20, Connection.m_Account:GetID(), Connection.m_UserName)
		Gas2Gac:ReceiveAntiIndulgenceExitGame(Connection)
	end
end

function LogoutForAdultCheck(uCharID)
	for server_id in pairs(g_ServerList) do
		Gas2GasAutoCall:LogoutForAdultCheck(GetServerAutoConn(server_id),uCharID)
	end
end
--断线
function CScriptConnMgrServerHandler:OnDisconnect(Conn)
	Conn.m_Disconnect = true
	CancelLoginWaiting(Conn)
	CheckUserOver(Conn)
	StopUpdateOnlineTime(Conn.m_UpOnlineTimeTick)
	Conn.m_UpOnlineTimeTick = nil
	if(Conn.m_nID ~= nil) then
		g_ContextMgr:DelContext(Conn.m_nID)
		Conn.m_nID = nil
	end
	if not Conn.m_Account or g_bServerShuttingDown then
		return
	end
		
	local AccountID = Conn.m_Account:GetID()
	DeleteAccountInfo(Conn)

	local player = Conn.m_Player
	if (not player )then
		return
	end
	
	if player.m_Conn.m_ChangeToServer == nil then
		--清理报名的玩家数据库信息
		MultiServerCancelAllAction(player.m_uID)
	end
	
	if IsCppBound(player) and  player.m_Properties:GetTeamID() ~= 0 then
		g_TeamMgr:MemberLogout(player,player.m_Properties:GetTeamID())
	end
	if IsCppBound(player)  and player.m_Properties:GetTongID() ~= 0 then
		g_GasTongMgr:MemberLogout(player,player.m_Properties:GetTongID())
	end
	if IsCppBound(g_AllPlayerSender) then
		g_AllPlayerSender:DelPlayer(player)
	end
	if CampPlayerSenderTbl[player:CppGetBirthCamp()] then
		CampPlayerSenderTbl[player:CppGetBirthCamp()]:DelPlayer(player)
	end
			
	if g_CountDownTime[Conn.m_LogoutSetting] == 0 then
		player.m_bDBTransBlock = false
		player.m_bDisabled = false
		player:DisableDirectorMoving(false)
	
		g_AllConnection[player.m_uID] = nil
		DelayLogout(player, 0, AccountID, Conn.m_UserName)
		return
	end
	
	if player.m_LeaveTick or player.m_bDisabled then
		return
	end
	
	g_AllConnection[player.m_uID] = nil
	DelayLogout(player, g_CountDownTime[Conn.m_LogoutSetting], 
				AccountID, Conn.m_UserName)
end

--客户端断开连接握手
function Gac2Gas:NotifyClientLeave(Conn, LeftTime, uToState)
	if LeftTime > 20 or LeftTime < 0 then
		LeftTime = 20
	end

	local AccountID = Conn.m_Account:GetID()
	local player = Conn.m_Player
	local function callback()
		if Conn and Conn.ShutDown then
			Conn:ShutDown("客户端断开连接")
		end
	end
	if player then
		player:DisableDirectorMoving(true)
		player.m_bDBTransBlock = true
		DelayLogout(Conn.m_Player, LeftTime, AccountID, Conn.m_UserName, callback)
	end
	Gas2Gac:ReturnNotifyClientLeave(Conn,uToState)
end

function Gac2Gas:NotifyToCharSelect(Conn, LeftTime)
	if LeftTime > 20 or LeftTime < 0 then
		LeftTime = 20
	end
	CRecruitMgr.GetRecruitInfo(Conn)
	local player = Conn.m_Player
	local uid = Conn.m_Account:GetID()
	local function LeaveGameCallBack()
		if (not Conn) or (not Conn.m_Account) then
			return
		end
		--Conn:LogOnOffLineMsg("user:" .. uid .. "\tlua里帐号换角色了")
		Gas2Gac:ChangeToStateNow(Conn, 1)
	end
	if player then
		player:DisableDirectorMoving(true)
		player.m_bDBTransBlock = true
		DelayLogout(Conn.m_Player, LeftTime+1, Conn.m_Account:GetID(), Conn.m_UserName, LeaveGameCallBack)
	end
	Gas2Gac:ReturnNotifyClientLeave(Conn,1)	
end


local function CancelLeaveGame(Conn)
	local player = Conn.m_Player
	if player then
		UnRegisterTick(player.m_LeaveTick)
		player.m_LeaveTick = nil
		player.m_bDBTransBlock = false
		player.m_bDisabled = false
		player:DisableDirectorMoving(false)
		return
	end
end

function ChangeSceneCancelLeaveGame(Conn)
    local player = Conn.m_Player
    if player then
        if player.m_LeaveTick then
            CancelLeaveGame(Conn)
            Gas2Gac:RetCancelLeaveGame(Conn)
        end
    end
end

function Gac2Gas:CancelReadLeftTime(Conn)
    CancelLeaveGame(Conn)
end

--[[

function Gac2Gas:Exit2CharList(Conn)
	local player = Conn.m_Player
	if not player.m_LeaveTick then
		return
	end
	UnRegisterTick(player.m_LeaveTick)
	player.m_LeaveTick = nil
	--player:SetConnection( nil )
	local function LeaveGameCallBack()
		Gas2Gac:ChangeToStateNow(Conn, 1)
	end
	LeaveGame(player, Conn.m_Account:GetID(), Conn.m_UserName, LeaveGameCallBack)
end

--]]

function CScriptConnMgrServerHandler:OnBeginServiceSucceeded()
	OnBeginServiceSucceeded()
end

function CScriptConnMgrServerHandler:OnBeginServiceFailed(szErrMsg)
	print(szErrMsg .. " 服务器将会关闭 !")
	AsyncServerCleanUp()
end

function CScriptConnMgrServerHandler:OnServiceEnded()
	OnServiceEnded()
end

function CScriptConnMgrServerHandler:OnCheat(Connection)
	local userName = Connection.m_UserName
	local player = Connection.m_Player
	if not player then
		return
	end
	local charId = player.m_uID
	local sceneName = player.m_Scene.m_SceneName
	
	OnCheat(userName,charId,sceneName)
end
