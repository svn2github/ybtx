CFbPlayerPosMgr = class()

local GetPosTime = 3000 --毫秒
local g_FbPlayerPosTbl = {}
CFbPlayerPosMgr.g_FbNameTbl = 
{
["鲜血试炼场"] = true,
["小pvp测试"] = true,
}

-- function CFbPlayerPosMgr:GetFbPlayerPos(Conn)
function CFbPlayerPosMgr.GetFbPlayerPos(Conn)
	if not IsCppBound(Conn.m_Player) then
		return
	end
	local Player = Conn.m_Player
	
--	local GameCamp = Player:CppGetGameCamp()
--	print(GameCamp)

	local Scene = Player.m_Scene
	local SceneName = Scene.m_SceneName
	if CFbPlayerPosMgr.g_FbNameTbl[SceneName] then
		CFbPlayerPosMgr.ShowCurSceneAllPlayerPos(Conn, Scene)
	end
end

---- 显示当前地图所有玩家
--function ShowCurSceneAllPlayerPos(Scene)
--	for PlayerID, Player in pairs(Scene.m_tbl_Player) do
--		local GridPos = CPos:new()
--		Player:GetGridPos(GridPos)
--		
--		local SceneName = Player.m_Scene.m_SceneName
--		for ID, Member in pairs(Scene.m_tbl_Player) do
--			if PlayerID ~= ID then
--				Gas2GacById:SendTeammatePos(ID, PlayerID, SceneName,GridPos.x,GridPos.y)
--			end
--		end
--				
--	end
--end

local function GetFbPlayerPosTick(Tick, Scene)
	--print("Tick~~~~~~~~~~~~~~~~~~~~~~~~~")
	local SceneID = Scene.m_SceneId
	for MemberId, pMember in pairs(Scene.m_tbl_Player) do
		if IsCppBound(pMember) then
			local GridPos = CPos:new()
			pMember:GetGridPos(GridPos)
			local SceneName = pMember.m_Scene.m_SceneName
			for id, Player in pairs(g_FbPlayerPosTbl[SceneID].m_NeedSendPlayerTbl) do
				if id ~= MemberId then
					local MemberName = pMember.m_Properties:GetCharName()
					--local GameCamp = pMember:CppGetGameCamp()
					
					local GameCamp = 1
					local game, room = g_MatchGameMgr:GetRoomByScene(pMember.m_Scene)
					if room then
						GameCamp = room:GetPlayerTeamIndex(pMember)
					end
	
					--print(MemberName.." GameCamp "..GameCamp)
					Gas2Gac:SendFbPlayerPos(Player.m_Conn, MemberId, SceneName, GridPos.x, GridPos.y, MemberName, GameCamp, pMember:CppIsLive())
				end
			end
		end
	end
end

-- function CFbPlayerPosMgr:ShowCurSceneAllPlayerPos(Conn, Scene)
function CFbPlayerPosMgr.ShowCurSceneAllPlayerPos(Conn, Scene)
	local Player = Conn.m_Player
	local PlayerId = Player.m_uID
	local Scene = Player.m_Scene
	local SceneId = Scene.m_SceneId
		
	---注册Tick前预先获取一次队友的坐标
	--local tblMembers = g_TeamMgr:GetMembers(SceneId)
	--for i = 1, #(tblMembers) do
	for MemberId, pMember in pairs(Scene.m_tbl_Player) do
		if MemberId ~= PlayerId then
			if IsCppBound(pMember) then
				local GridPos = CPos:new()
				pMember:GetGridPos(GridPos)
				local SceneName = pMember.m_Scene.m_LogicSceneName
				local MemberName = pMember.m_Properties:GetCharName()
				--local GameCamp = pMember:CppGetGameCamp()
				local GameCamp = 1
				local game, room = g_MatchGameMgr:GetRoomByScene(pMember.m_Scene)
				if room then
					GameCamp = room:GetPlayerTeamIndex(pMember)
				end
					
				--print("MemberName "..MemberName.."   TeamID "..TeamID)
				Gas2Gac:SendFbPlayerPos(Conn, MemberId, SceneName, GridPos.x, GridPos.y, MemberName, GameCamp, pMember:CppIsLive())
				
--				-- 如果Player和Member在一个工会，则清除Member在Player中地图上的工会图标 
--				Gas2GacById:HideLeavedTongmatePos(PlayerId, MemberId)
			end
		end
	end
	
	if not g_FbPlayerPosTbl[SceneId] then
		g_FbPlayerPosTbl[SceneId] = {}
		local tick = RegisterTick("GetFbPlayerPosTick",GetFbPlayerPosTick,GetPosTime,Scene)
		g_FbPlayerPosTbl[SceneId].m_GetFbPlayerPosTick = tick
		g_FbPlayerPosTbl[SceneId].m_NeedSendPlayerTbl = {}
		g_FbPlayerPosTbl[SceneId].m_NeedSendPlayerTbl[PlayerId] = Player
	else
		if not g_FbPlayerPosTbl[SceneId].m_GetFbPlayerPosTick then
			local tick = RegisterTick("GetFbPlayerPosTick",GetFbPlayerPosTick,GetPosTime,SceneId)
			g_FbPlayerPosTbl[SceneId].m_GetFbPlayerPosTick = tick
		end
		g_FbPlayerPosTbl[SceneId].m_NeedSendPlayerTbl[PlayerId] = Player
	end
end

--function CFbPlayerPosMgr:DestroyGetFbPlayerPosTick(SceneID)
function CFbPlayerPosMgr.DestroyGetFbPlayerPosTick(SceneID)
	--print("DestroyGetFbPlayerPosTick")
	if g_FbPlayerPosTbl[SceneID] and 
		g_FbPlayerPosTbl[SceneID].m_GetFbPlayerPosTick then
		UnRegisterTick(g_FbPlayerPosTbl[SceneID].m_GetFbPlayerPosTick)
	end
	g_FbPlayerPosTbl[SceneID] = nil
end

-- function CFbPlayerPosMgr:PlayerStopGetFbPlayerPos(Player)
function CFbPlayerPosMgr.PlayerStopGetFbPlayerPos(Player)
	local function Init()
		local Scene = Player.m_Scene
		local SceneName = Scene.m_SceneName
		if CFbPlayerPosMgr.g_FbNameTbl[SceneName] then
			local SceneID = Scene.m_SceneId
			local PlayerId = Player.m_uID
			if g_FbPlayerPosTbl[SceneID] then
				g_FbPlayerPosTbl[SceneID].m_NeedSendPlayerTbl[PlayerId] = nil
				if next(g_FbPlayerPosTbl[SceneID].m_NeedSendPlayerTbl) == nil then
					CFbPlayerPosMgr.DestroyGetFbPlayerPosTick(SceneID)
				end
			end
		end
	end
	apcall(Init)
end

-- 隐藏下线的玩家、离开指定地图的玩家的图标
--function CFbPlayerPosMgr:StopSendLeavedFbPlayerPos(Scene, Player)
function CFbPlayerPosMgr.StopSendLeavedFbPlayerPos(Scene, Player)
	local PlayerID = Player.m_uID
	local SceneName = Scene.m_SceneName
	if CFbPlayerPosMgr.g_FbNameTbl[SceneName] then
		local SceneID = Scene.m_SceneId
		if not g_FbPlayerPosTbl[SceneID] then
			--return	
		end
		
		-- 清除Player中地图上其他玩家的图标
		Gas2Gac:StopSendFbPlayerPos(Player.m_Conn)
		
		--print("隐藏的玩家坐标  "..PlayerID.." 坐标")
	--	local tblMembers = g_GasTongMgr:GetMembers(SceneID)
	--	for i = 1, #(tblMembers) do
		--for MemberId, pMember in pairs(g_FbPlayerPosTbl[SceneID].m_NeedSendPlayerTbl) do
		for MemberId, pMember in pairs(Scene.m_tbl_Player) do
			if IsCppBound(pMember) then		
				if MemberId ~= PlayerID then
					Gas2Gac:HideLeftFbPlayerPos(pMember.m_Conn, PlayerID)	-- 通知所有在线的人隐藏PlayerID的标志
				else
					if g_FbPlayerPosTbl[SceneID] 
						and g_FbPlayerPosTbl[SceneID].m_NeedSendPlayerTbl
						and g_FbPlayerPosTbl[SceneID].m_NeedSendPlayerTbl[PlayerID] then
						g_FbPlayerPosTbl[SceneID].m_NeedSendPlayerTbl[PlayerID] = nil
						if next(g_FbPlayerPosTbl[SceneID].m_NeedSendPlayerTbl) == nil then
							CFbPlayerPosMgr.DestroyGetFbPlayerPosTick(SceneID)
						end
						--g_FbPlayerPosTbl[SceneID].m_LastSendTime[PlayerId] = nil
					end
				end
			end
		end
	end
end

-- function CFbPlayerPosMgr:StopGetFbPlayerPos(Conn)
function CFbPlayerPosMgr.StopGetFbPlayerPos(Conn)
	if not IsCppBound(Conn) or not IsCppBound(Conn.m_Player) then
		return
	end
	CFbPlayerPosMgr.PlayerStopGetFbPlayerPos(Conn.m_Player)
end

-- function CFbPlayerPosMgr:OnSceneDestroy(SceneID)
function CFbPlayerPosMgr.OnSceneDestroy(SceneID)
	CFbPlayerPosMgr.DestroyGetFbPlayerPosTick(SceneID)
	g_FbPlayerPosTbl[SceneID] = nil
end
