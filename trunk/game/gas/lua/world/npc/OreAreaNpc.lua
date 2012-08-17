cfg_load "npc/Npc_Common"
cfg_load "liveskill/OreMonster_Common"
cfg_load "liveskill/OreMapPosition_Common"

local CreateGoldBoxRate = 25
--local CreateGoldBoxName = "øÛ∂¥±¶œ‰"
--local CreateOreBossName = "‘∂π≈“Ï ﬁ"
--local CreateSceneBossName = "øÒ±©µƒµÿ¡È"

local function CreateNpcPos(NpcPosTbl, posInUseTbl)
	if table.maxn(NpcPosTbl) <= table.maxn(posInUseTbl) then
		return 1
	end
	local bContinue = true
	local n
	while bContinue do
		n = math.random(table.maxn(NpcPosTbl))
		bContinue = false
		for i,v in pairs(posInUseTbl) do
			if v == n then
				bContinue = true
			end
		end
	end
	return n
end

local function RandomPosByScene(SceneName)
	local Info = OreMapPosition_Common(SceneName)
	if Info and Info("BossPosition") ~= "" then
		local BossPosition = GetCfgTransformValue(false, "OreMapPosition_Common", SceneName, "BossPosition")
		local n = math.random(table.maxn(BossPosition))
		return GetOreMapPosition(BossPosition[n])
	end
end

local function GetBossMonsterName(SceneName)
	local Info = OreMapPosition_Common(SceneName)
	if Info then
		return Info("BossName")
	end
end

function CreateOreAreaNpc(Scene)
	local SceneLevel = Scene.m_SceneLevel
	local NpcPosTbl = GetCfgTransformValue(false, "OreMapPosition_Common", Scene.m_SceneName, "PosTable")
	local ObjPosTbl = GetCfgTransformValue(false, "OreMapPosition_Common", Scene.m_SceneName, "GoldBoxPosition")
	local posInUseTbl = {}
	--À¢NPC
	if OreMonster_Common(SceneLevel) and OreMonster_Common(SceneLevel, "MonsterType") then
		local monsterType = GetCfgTransformValue(false, "OreMonster_Common", SceneLevel, "MonsterType")
		local monsterAmount = OreMonster_Common(SceneLevel, "Amount")
		if Scene.m_SceneName == "Ãÿ ‚øÛ∂¥" then
			monsterAmount = OreMonster_Common(SceneLevel, "SpAmount")
		end
		Scene.m_OreMonsterTbl = {}
		Scene.m_OreMonsterTbl.m_NpcCount = 0
		for i = 1 , monsterAmount do
			if table.maxn(monsterType) == 0  then
				return
			end
			local n = math.random(table.maxn(monsterType))
			local CreateNpcName = monsterType[n]
			local MaxLevel = Scene.m_PlayerLevel or 1
--			print("SceneLevel:"..SceneLevel) --testest
--			print("PlayersLevel:"..PlayersLevel) --testest
--			print("PlayerNum:"..PlayerNum)
--			print("MaxLevel:"..MaxLevel) --testest
			local CreateNpcLevel = math.random( OreMonster_Common(SceneLevel, "MinLevel"), MaxLevel)
--			print(CreateNpcLevel)
			if CreateNpcLevel > OreMonster_Common(SceneLevel, "MaxLevel") then
				CreateNpcLevel = OreMonster_Common(SceneLevel, "MaxLevel")
			end
			local CreatePos = CPos:new()
			n = CreateNpcPos(NpcPosTbl, posInUseTbl)
			CreatePos.x, CreatePos.y = GetOreMapPosition(NpcPosTbl[n], Scene)
			posInUseTbl[i] = n
--			print(CreateNpcName.."::"..CreateNpcLevel.."::"..CreatePos.x..","..CreatePos.y)
			
			CreateNpcName = CreateNpcLevel.."º∂"..CreateNpcName
			CreateNpcLevel = g_NpcBornMgr:GetNpcBornLevel(CreateNpcName)
			
			local fPos = CFPos:new( CreatePos.x * EUnits.eGridSpan, CreatePos.y * EUnits.eGridSpan )
			local CreateNpc, logMsg = g_NpcServerMgr:CreateServerNpc(CreateNpcName, CreateNpcLevel, Scene, fPos)
			if IsServerObjValid(CreateNpc) then
				CreateNpc:SetAndSyncActionDir(math.random(0, 255))
				Scene.m_OreMonsterTbl[CreateNpc:GetEntityID()] = true
				Scene.m_OreMonsterTbl.m_NpcCount = Scene.m_OreMonsterTbl.m_NpcCount + 1
			else
				LogErr("¥¥Ω®Npc ß∞‹", logMsg)
			end
		end
--		for i = 1, table.maxn(ObjPosTbl) do
--			local n = math.random(1, 100)
--			if n <= CreateGoldBoxRate then
--				local Pos = CPos:new()
--				Pos.x,Pos.y = GetOreMapPosition(ObjPosTbl[i], Scene)
--				local Obj = CreateIntObj(Scene,Pos,CreateGoldBoxName)
--			end
--		end

		--À¢±¶œ‰
		if OreMonster_Common(SceneLevel, "GoldBox") then
			local index = 1
			local GoldBox = GetCfgTransformValue(true, "OreMonster_Common", SceneLevel, "GoldBox")
			for i, v in pairs(GoldBox) do
				for j = 1, v[2] do
					local n = math.random(1, 1000)
					if v[3] == nil or n <= (v[3] * 1000) then 
						if not IsNumber(ObjPosTbl[index]) then
							break
						end
						local Pos = CPos:new()
						Pos.x,Pos.y = GetOreMapPosition(ObjPosTbl[index], Scene)
						local Obj = CreateIntObj(Scene,Pos,v[1])
					end
					index = index + 1
				end
			end
		end
	end
end

local function CreateOreBoss(Scene)
	local CreateNpcName = GetBossMonsterName(Scene.m_SceneName)
--	local CreateNpcLevel = 30
	local CreatePos = CPos:new()
--	if OreMapPosition_Common[Scene.m_SceneName] 
--		and OreMapPosition_Common[Scene.m_SceneName].BossPosition 
--		and OreMapPosition_Common[Scene.m_SceneName].BossPosition ~= "" then
--		local BossPos = OreMapPosition_Common[Scene.m_SceneName].BossPosition
--		CreatePos.x, CreatePos.y = BossPos[1], BossPos[2]
--	else
		CreatePos.x, CreatePos.y = RandomPosByScene(Scene.m_SceneName)
--	end
	if CreateNpcName == nil then
		return
	end
	MsgToConn(Scene.m_CoreScene, 10010)
	local fPos = CFPos:new( CreatePos.x * EUnits.eGridSpan, CreatePos.y * EUnits.eGridSpan )
	local CreateBoss = g_NpcServerMgr:CreateServerNpc(CreateNpcName,CreateNpcLevel,Scene,fPos)
end

function CreateOreBossInScene(Scene, PlayerName)
	local CreateSceneId = Scene.m_CreateSceneId
	local serverId = Scene.m_CreateServerId
	Gas2GasAutoCall:CreateOreBossInScene(GetServerAutoConn(serverId), CreateSceneId, g_CurServerId, PlayerName, Scene.m_SceneId)
end

function Gas2GasDef:SendOreBossMsg(Conn, SceneId, PlayerName, SceneName, AreaName)
	local Scene = g_SceneMgr:GetScene(SceneId)
	if not Scene then
		return
	end
	MsgToConn(Scene.m_CoreScene, 10009, PlayerName, SceneName, AreaName)
	Gas2Gac:SendPlayerBossMsg(Scene.m_CoreScene, 6016, PlayerName, SceneName, AreaName)
end
	
function Gas2GasDef:CreateOreBossInScene(Conn, SceneId, serverId, PlayerName, ReSceneId)
	local CreateScene = g_SceneMgr:GetScene(SceneId)
	if not CreateScene then
		return
	end
	local CreateNpcName = GetBossMonsterName(CreateScene.m_SceneName)
--	local CreateNpcLevel = 30
	local CreatePos = CPos:new()
	CreatePos.x, CreatePos.y = RandomPosByScene(CreateScene.m_SceneName)
	if CreateNpcName == nil then
		return
	end
	local AreaName = g_AreaMgr:GetSceneAreaName(CreateScene.m_SceneName, CreatePos)
	Gas2GasAutoCall:SendOreBossMsg(GetServerAutoConn(serverId), ReSceneId, PlayerName, CreateScene.m_SceneName, AreaName)
	MsgToConn(CreateScene.m_CoreScene, 10009, PlayerName, CreateScene.m_SceneName, AreaName)
	Gas2Gac:SendPlayerBossMsg(CreateScene.m_CoreScene, 6016, PlayerName, CreateScene.m_SceneName, AreaName)
	local fPos = CFPos:new( CreatePos.x * EUnits.eGridSpan, CreatePos.y * EUnits.eGridSpan )
	local CreateBoss = g_NpcServerMgr:CreateServerNpc(CreateNpcName,CreateNpcLevel,CreateScene,fPos)
end

local function CreateOreObj(Npc)
	local pos = CPos:new()
	Npc:GetGridPos(pos)
	local CreateScene = Npc.m_Scene 
	local CreateName = "øÛ∂¥ª˙πÿ"
	local Obj = CreateIntObj(CreateScene,pos,CreateName,false)  --¥¥Ω®Obj
	if Obj then
		MsgToConn(CreateScene.m_CoreScene, 10012)
		g_IntObjServerMgr:IntObjChangeState(Obj, 1)
	end
end

function CheckOreMonster(Npc)  --øÛ∂¥ø™πÿ
	local Scene = Npc.m_Scene
	if Scene.m_OreMonsterTbl then
		if Scene.m_OreMonsterTbl[Npc:GetEntityID()] then
			Scene.m_OreMonsterTbl[Npc:GetEntityID()] = nil
			Scene.m_OreMonsterTbl.m_NpcCount = Scene.m_OreMonsterTbl.m_NpcCount - 1
			if Scene.m_OreMonsterTbl.m_NpcCount <= 0 then
				CreateOreObj(Npc)
--				local Obj = GetOnlyIntObj(Scene.m_SceneId, "øÛ∂¥ª˙πÿ")
--				if Obj then
--					MsgToConn(Scene.m_CoreScene, 10012)
--					g_IntObjServerMgr:IntObjChangeState(Obj, 1)
--				end
				if Scene.m_OreAreaBoss then
					CreateOreBoss(Scene)
				end
				Scene.m_OreMonsterTbl = nil
			else
				if Scene.m_OreMonsterTbl.m_NpcCount < 5 or (Scene.m_OreMonsterTbl.m_NpcCount%5) == 0 then
					MsgToConn(Scene.m_CoreScene, 10019, Scene.m_OreMonsterTbl.m_NpcCount)
				end
			end
		end
	end
end
