CScopesExploration = class()

--==========================任务进领域本和剧情本 star==========================
local function FbChangeScene(Player, SceneName, x, y, IsSendMsg)
	if not IsCppBound(Player) then
		return
	end
	local SceneCommon = Scene_Common[SceneName]
	if not SceneCommon then
		return
	end
	
	local SceneType = SceneCommon.SceneType
	if not x or not y then
		x = SceneCommon.InitPosX
		y = SceneCommon.InitPosY
	end
	
	local function CallBack(isSucceed, sceneId, serverId, inviteTbl)
		if not IsCppBound(Player) then
			return
		end
		if isSucceed then
			MultiServerChangeScene(Player.m_Conn, sceneId, serverId, x , y)
			for _, teammateId in pairs(inviteTbl) do
				Gas2GacById:RetIsJoinScopesFb(teammateId, SceneName, x, y)
			end
		else
			if sceneId then
				if serverId then
					MsgToConn(Player.m_Conn, sceneId, serverId)
				else
					MsgToConn(Player.m_Conn, sceneId)
				end
			end
		end
	end
	
	local teamId = Player.m_Properties:GetTeamID()
	local data = {}
	data["charId"] = Player.m_uID
	data["teamId"] = teamId
	data["SceneName"] = SceneName
	data["InviteTeammate"] = IsSendMsg
	data["SceneType"] = SceneType
	
	CallAccountAutoTrans(Player.m_Conn.m_Account, "SceneMgrDB", "GetLingYuFbScene", CallBack, data)
end

function CScopesExploration:EnterScopesFb(Conn, SceneName)
	FbChangeScene(Conn.m_Player, SceneName, nil, nil, true)
end

function CScopesExploration:AgreedJoinScopesFb(Conn, SceneName, PosX, PosY)
	FbChangeScene(Conn.m_Player, SceneName, PosX, PosY, false)
end


--时间到后,让玩家离开副本,并删除场景
local function LeaveScopesSceneTick(Scene)
	Scene.m_State = g_FbRoomState.GameEnd
	if Scene.m_tbl_Player and next(Scene.m_tbl_Player) then
		for Id,_ in pairs(Scene.m_tbl_Player) do
			NotifyPlayerLeaveFbScene(Id, 30, 8802)-- "时间结束.")
		end
		return
	else
		Scene:Destroy()
	end
end


function CScopesExploration.IntoScene(Player)
	local Scene = Player.m_Scene
	local SceneName = Scene.m_SceneName
	
	if not Scene.m_State then
		local SceneCommon = Scene_Common[SceneName]
		local lifetime = SceneCommon.LifeCycle
		local TickTime = 10*60--无时间的就让其生存10分
		if lifetime > 0 then
			TickTime = lifetime * 60
		end
		Scene.m_State = g_FbRoomState.GameEnd
		RegisterOnceTick(Scene, "DestroySceneTick", LeaveScopesSceneTick, TickTime * 1000, Scene)
	end

	local function CallBack(result, ActionName, Type, IsTimes)
		if result and IsCppBound(Player) then
			MsgToConn(Player.m_Conn, 3515, JoinCountLimit_Server(ActionName, "Count"), IsTimes+1)
		end
	end
	
	if not Scene.m_EnteredCharTbl then
		Scene.m_EnteredCharTbl = {}
	end
	if not Scene.m_EnteredCharTbl[Player.m_uID] then
		Scene.m_EnteredCharTbl[Player.m_uID] = true
		
		local SceneCommon = Scene_Common[SceneName]
		local SceneType = SceneCommon.SceneType
		if SceneCommon.SceneType == 23 then
			local parameters = {}
			parameters["PlayerId"] = Player.m_uID
			parameters["ActivityName"] = "领域本限次"
			parameters["Type"] = 2
			CallAccountManualTrans(Player.m_Conn.m_Account, "SceneMgrDB", "EnterDareQuestFbScene", CallBack, parameters)
		end
		
	end
end
--=========================任务进领域本和剧情本 end=============================

--=========================使用物品进领域本 star ==========================
function CScopesExploration.UseItemInScene(Player, FbName, PosX, PosY)
	FbChangeScene(Player, FbName, PosX, PosY, true)
end
--=========================使用物品进领域本 end ==========================




--======================特殊领域本的功能 star=================================
--创建特殊领域副本
function CScopesExploration:CreateSpecilAreaFb(Conn, Target, SceneName)
	local Camp = Conn.m_Player:CppGetBirthCamp()
	
	local function CallBack(SceneId, ServerId)
		local TargetEntityID = Target:GetEntityID()
		Target.m_SceneId = SceneId
		Target.m_ServerId = ServerId
		Target.m_SceneName = SceneName
		Target.m_CreateSceneCamp = Camp
		--通知目标服务器去创建场景
		Gas2GasAutoCall:CreateScopesExplorationRoom(GetServerAutoConn(ServerId), SceneId, TargetEntityID)
	end
	
	local parameter = {}
	parameter["SceneName"] = SceneName
	parameter["OtherArg"] = {["m_SceneCamp"] = Camp}
	CallDbTrans("SceneMgrDB", "_CreateScene", CallBack, parameter, SceneName)
end

function Gas2GasDef:CreateScopesExplorationRoom(ServerConn, SceneId, TargetEntityID)
	local function CallBack(IsExist, SceneName, otherArg)
		if not IsExist then
			return
		end
		local scene = g_SceneMgr:GetScene(SceneId)
		if not scene then
			local scene = g_SceneMgr:CreateScene(SceneName, SceneId, otherArg)
			if scene then
				local SceneCommon = Scene_Common[SceneName]
				local lifetime = SceneCommon.LifeCycle
				local TickTime = 10*60--无时间的就让其生存10分
				if lifetime > 0 then
					TickTime = lifetime * 60
				end
				scene.m_State = g_FbRoomState.PlayerCanIn
				RegisterOnceTick(scene, "DestroySceneTick", LeaveScopesSceneTick, TickTime * 1000, scene)
			else
				return
			end
		end
		--往回发一个消息,说场景创建好了,可以进人了
		Gas2GasAutoCall:RetCanTransFromTrap(ServerConn,TargetEntityID)
	end
	
	local data = {}
	data["sceneId"] = SceneId
	data["serverId"] = g_CurServerId
	CallDbTrans("SceneMgrDB", "IsSceneExistent", CallBack, data, SceneId, SceneName)
end

function Gas2GasDef:RetCanTransFromTrap(ServerConn, TargetEntityID)
	local Target = CIntObjServer_GetIntObjServerByID(TargetEntityID)
	if Target == nil then
		return
	end
	Target.m_IsCanTransSpecilAreaFb = true
end

function CScopesExploration:IsCanEnterSpecilExplorationScene(Scene, charId, PlayerCamp)
	local SceneCamp = Scene.m_SceneCamp
	
	local PlayerInfo = {}
	if Scene.m_tbl_Player and next(Scene.m_tbl_Player) then
		for Id,Target in pairs(Scene.m_tbl_Player) do
			if IsCppBound(Target) then
				PlayerInfo[Id] = Target:CppGetBirthCamp()
			end
		end
	end
	
	local PlayerNum = {0,0}
	for id,camp in pairs(PlayerInfo) do
		if SceneCamp == camp then
			PlayerNum[1] = PlayerNum[1] + 1
		else
			PlayerNum[2] = PlayerNum[2] + 1
		end
	end
	
	if PlayerCamp == SceneCamp then
		if PlayerNum[1] >= 5 then
			--人数已经够了,不让进副本
			MsgToConnById(charId,3510)
			--LeaveFb(Player)
			return false
		end
	else
		if PlayerNum[2] >= 5 then
			--人数已经够了,不让进副本
			MsgToConnById(charId,3510)
			--LeaveFb(Player)
			return false
		end
	end
	
	return true
end

--function PlayerDeadExplorationScene(Killer, Deader)
	--玩家在特殊领域副本死后,加一个buff(为了让玩家不能再进)
	--Deader:PlayerDoFightSkillWithoutLevel("领域副本")
--end
