
ChangeSceneQueue = CQueue:new()  --同时刻登录帐号处理,
ChangeSceneCharTbl = {}
ChangeSceneCharTbl.Size = 0



g_SecretKeyTbl = {}

function OnClearSecretKeyTickTbl(tick, key)
	--print("超时清除密钥:", key,  g_SecretKeyTbl[key].UserName, g_SecretKeyTbl[key].CharId)
	UnRegisterTick(tick)
	local tbl = g_SecretKeyTbl[key]
	MultiServerCancelAllAction(tbl.CharId)
	g_SecretKeyTbl[key] = nil
	SaveLogout(tbl.UserId)
end


SecretKeyLifeTime = 60 --秒

function AddSecretKeyTick(key, userName, charId, userId)
	--print("添加密钥:",key, userName, charId)
	if g_SecretKeyTbl[key] then
		UnRegisterTick(g_SecretKeyTbl[key].Tick)
		g_SecretKeyTbl[key] = nil
	end
	g_SecretKeyTbl[key] = {}
	g_SecretKeyTbl[key].UserName = userName
	g_SecretKeyTbl[key].UserId = userId
	g_SecretKeyTbl[key].CharId = charId
	g_SecretKeyTbl[key].Tick = RegisterTick("PlayerChangeInServerTick", OnClearSecretKeyTickTbl, SecretKeyLifeTime * 1000, key)
end

--使用完就删除
function UseSecretKey(key)
	local tbl = g_SecretKeyTbl[key]
	--print("使用密钥:", key,  tbl.UserName, tbl.CharId)
	if tbl then
		UnRegisterTick(tbl.Tick)
		g_SecretKeyTbl[key] = nil
		return tbl.UserName, tbl.CharId
	end
end


local function OnWaitPlayerInTick(Scene, charId)
	if not Scene.m_isWaitPlayerIn then
		return
	end
		
	Scene:Destroy()
end

local function GetLastCacheMsgString(player)
	--assert(player.m_LastCacheMsgId, "角色登录时候需要将 m_LastCacheMsgId 初始化")
	local result  = ""
	local strFormat = "[%d]=%.f," --不要小数
	for serverId, msgId in pairs(player.m_LastCacheMsgId) do
		result = result .. string.format(strFormat,serverId,msgId)
	end
	return result
end

--其他服务器有玩家要切到本服务器的场景来, 会提前通知本服, 
--将会调到这个函数,进行创建场景或判断是否能切场景.并回复原服务器
function Gas2GasDef:NotifyDestServerChangeScene(conn, charId, sceneId, x , y, isCheckCpu, allUsage,maxThreadUsage)
	--print("Gas2GasDef:NotifyDestServerChangeScene")
	local scene = g_SceneMgr:GetScene(sceneId)
	
	if isCheckCpu then
		local curAllUsage = CCpuUsageMgr_GetSystemCpuUsage(30)
		local curMaxThreadUsage = CThreadCpuUsageMgr_GetTopThreadCpuUsage(30)
		--print(curAllUsage, curMaxThreadUsage, allUsage, maxThreadUsage)
		if curAllUsage > GasConfig.AllCpuLimit or curMaxThreadUsage > GasConfig.MaxThreadCpuLimit then --当前服务器负载满了, 
			if curAllUsage >= allUsage or curMaxThreadUsage >= maxThreadUsage then --但前服务器比原来服务器更忙, 不允许却场景
				MsgToConnById(charId, 380004, scene.m_SceneName)
				return
			end
		end
	end
	
	
	if scene then
		if scene:IsCanChangeIn() then
		--回复原服可传
			Gas2GasAutoCall:RetSrcServerChangeScene(conn, charId, sceneId, x , y)
		end
		return
	end
	
	local function CallBack(IsExist, sceneName, otherArg)
		if not IsExist then
			return
		end
		scene = g_SceneMgr:GetScene(sceneId)
		if not scene then
			scene = g_SceneMgr:CreateScene(sceneName, sceneId, otherArg)
			if scene then
				scene.m_isWaitPlayerIn = true
				scene.m_PlayerChangeInTbl[charId] = true
				RegisterOnceTick(scene, "WaitPlayerInTick", OnWaitPlayerInTick, (SecretKeyLifeTime + 20) * 1000, scene, charId)
			else
				return
			end
		end
		--回复原服可传
		Gas2GasAutoCall:RetSrcServerChangeScene(conn, charId, sceneId, x , y)
	end
	local data = {}
	data["sceneId"] = sceneId
	data["serverId"] = g_CurServerId  --这个是为了断言逻辑是否有bug的, 整个流程没问题的话sceneId对应的serverId必然是g_CurServerID
	CallDbTrans("SceneMgrDB", "IsSceneExistent", CallBack, data, sceneId)
end

--回复原服务器让玩家切过来
function Gas2GasDef:RetSrcServerChangeScene(conn, charId, sceneId, x , y)
	--print(debug.traceback())
	--print("Gas2GasDef:RetSrcServerChangeScene")
	local player = g_GetPlayerInfo(charId)
	if not IsCppBound(player) or not IsCppBound(player.m_Conn) or player.m_IsChangeSceneing then
		return
	end
	if player.m_Scene.m_SceneId == sceneId then
		return
	end
	player.m_IsChangeSceneing = true
	
	local function CallBack(sceneName, arg)
		if not IsCppBound(player) or not IsCppBound(player.m_Conn) then
			player.m_IsChangeSceneing = false
			return
		end
		if sceneName then
			if conn == g_CurServerId  then
				local scene = g_SceneMgr:GetScene(sceneId)
				if scene and scene:IsCanChangeIn() then
					local fPos = CFPos:new()
					fPos.x = x * EUnits.eGridSpanForObj
					fPos.y = y * EUnits.eGridSpanForObj
					player:BeginChangeScene(scene, fPos)
--/**旧PK代码**/
--					player:SetZoneType(scene.m_SceneAttr.PkType)
				else
					player.m_IsChangeSceneing = false
				end
				return
			else
				
				local function DoChange()
				
					local Ip, Port = GetServerIporPort(arg["serverId"])
					local function CreateKeyCallback(sKey)
						if not IsCppBound(player) or not IsCppBound(player.m_Conn) then
							player.m_IsChangeSceneing = false
							ChangeSceneOver(player)
							return
						end
						
						local lastMsgIdStr = GetLastCacheMsgString(player)
						player.m_Conn.m_LogoutSetting = "Immediately"
						player.m_Conn.m_ChangeToServer = arg["serverId"]
						player.m_Conn.m_ChangeToSceneName = sceneName
						player.m_OtherServerInfo = {}
						player.m_OtherServerInfo["Ip"] = Ip
						player.m_OtherServerInfo["Port"] = Port
						player.m_OtherServerInfo["sKey"] = sKey
						player.m_OtherServerInfo["lastMsgIdStr"] = lastMsgIdStr
						ChangeSceneCancelLeaveGame(player.m_Conn)
						local function ConnExit_callback()
						end
						LeaveGame(player, player.m_AccountID, player.m_Conn.m_UserName, ConnExit_callback)
						StopUpdateOnlineTime(player.m_Conn.m_UpOnlineTimeTick)
					end
					local function CreateKeyTimeOut()
						LogErr("切场景创建密钥超时, 可能是服务器间链接断开", "目标服务器 ip:" .. Ip .. " 端口:" .. Port)
						player.m_IsChangeSceneing = false
						ChangeSceneOver(player)
					end
					local userId = player.m_Conn.m_Account:GetID()
					CallGas(conn, "CreateSecretKey", CreateKeyCallback, CreateKeyTimeOut, 20, player.m_Conn.m_UserName, charId, userId)
				
				end
--				local function test()
--					RegisterOnceTick(g_App, "test", DoChange, 10000)
--				end
--				ChangeSceneEntry(player, test)
				ChangeSceneEntry(player, DoChange)
			end
		else
			if arg then
				MsgToConn(player.m_Conn, unpack(arg))
			end
			player.m_IsChangeSceneing = false
		end
		
	end
	
	local data = {}
	data["char_id"] = charId
	data["posCur_x"] = x
	data["posCur_y"] = y
	data["scene_id"] = sceneId
	data["user_name"] = player.m_Conn.m_UserName
	data["isSameServer"] = conn == g_CurServerId
	if g_SceneMgr:IsMainScene(sceneId) then
		CallAccountAutoTrans(player.m_Conn.m_Account, "SceneMgrDB", "ChangeScene", CallBack, data)
	else
		CallAccountAutoTrans(player.m_Conn.m_Account, "SceneMgrDB", "ChangeScene", CallBack, data, sceneId)
	end
end


function ChangeSceneEntry(player, callFunc)
	--print("ChangeSceneEntry", player)
	ChangeSceneQueue:push({player, callFunc})
	TurnChangeScene()
end


function ChangeSceneOver(player)
	if ChangeSceneCharTbl[player] then
		--print("ChangeSceneOver", player)
		ChangeSceneCharTbl[player] = nil
		ChangeSceneCharTbl.Size = ChangeSceneCharTbl.Size -1
		TurnChangeScene()
	end
end

local function PassOne()
	while not ChangeSceneQueue:empty() do
		local node = ChangeSceneQueue:front()
		ChangeSceneQueue:pop()
		if IsCppBound(node[1]) and IsCppBound(node[1].m_Conn) then
			apcall(node[2])
			if ChangeSceneCharTbl[node[1]] then
				LogErr("ChangeSceneCharTbl 中出现重复 值")
			else
				ChangeSceneCharTbl[node[1]] = true
				ChangeSceneCharTbl.Size = ChangeSceneCharTbl.Size + 1
			end
			--print("成功通过角色:", node[1])
			return true
		end
		--print("成功通过角色", node[1])
	end
end

function TurnChangeScene()
	--print("当前切场景人数:", ChangeSceneCharTbl.Size, "切场景人数:", GasConfig.MaxSynChangeNum)
	while ChangeSceneCharTbl.Size < GasConfig.MaxSynChangeNum do
		if not PassOne() then
			break
		end
	end
end
