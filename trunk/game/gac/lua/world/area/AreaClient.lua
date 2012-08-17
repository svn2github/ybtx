cfg_load "sound/BackgroundMusic_Client"
lan_load "scene/Lan_Scene_Basic_Common"
lan_load "area/Lan_AreaInfo_Client"
lan_load "scene/Lan_Trap_Common"


g_MusicCDTbl = {}

local function ClearMusicCD(Tick, musicName)
	UnRegisterTick(g_MusicCDTbl[musicName])
	g_MusicCDTbl[musicName] = nil
	--print("音乐", musicName, "CD 结束")
	if g_App.m_CurMusicName == musicName then
		local cfgTbl = BackgroundMusic_Client(g_GameMain.m_AreaName) or BackgroundMusic_Client(musicName)
		--print("播放当前音乐")
		PlayBackgroundMusic(nil, musicName, cfgTbl)
	end
end

function PlayBackgroundMusic(Tick, musicName, cfgTbl)
	if g_MusicCDTbl[musicName] then
		--print("音乐", musicName, "CD 中")
		return
	end
	g_App.m_CurMusicName = musicName
	PlayCue(musicName)
	--print("播放背景音乐 ".. musicName)
	if g_GameMain.m_PlayBackgroundMusicTick then
		UnRegisterTick(g_GameMain.m_PlayBackgroundMusicTick)
		g_GameMain.m_PlayBackgroundMusicTick = nil
	end
	if cfgTbl then
		local playLength = cfgTbl("MusicTime") * math.random( cfgTbl("MinPlayCount"), cfgTbl("MaxPlayCount") )
		if g_GameMain.m_StopBackgroundMusicTick then
			UnRegisterTick(g_GameMain.m_StopBackgroundMusicTick)
			g_GameMain.m_StopBackgroundMusicTick = nil
		end
		g_GameMain.m_StopBackgroundMusicTick = RegisterTick("StopBackgroundMusicTick", StopBackgroundMusic, playLength * 1000, musicName, cfgTbl)
	else
		--print("背景音乐 (" ..  musicName .. ") 没有填写对应的播放规则.请检测配置表BackgroundMusic_Client")
	end	
end

function StopBackgroundMusic(Tick, musicName, cfgTbl)
	StopCue(musicName)
	--print("停止播放背景音乐 ".. musicName)
	if g_GameMain.m_StopBackgroundMusicTick then
		UnRegisterTick(g_GameMain.m_StopBackgroundMusicTick)
		g_GameMain.m_StopBackgroundMusicTick = nil
	end
	if cfgTbl then
		local stopLength = cfgTbl("WaitTime") * math.random( cfgTbl("MinWaitCount"), cfgTbl("MaxWaitCount") )
		if g_GameMain.m_PlayBackgroundMusicTick then
			UnRegisterTick(g_GameMain.m_PlayBackgroundMusicTick)
			g_GameMain.m_PlayBackgroundMusicTick = nil
		end
		g_GameMain.m_PlayBackgroundMusicTick = RegisterTick("PlayBackgroundMusicTick", PlayBackgroundMusic, stopLength * 1000, musicName, cfgTbl)
	else
		--print("背景音乐 (" ..  musicName .. ") 没有填写对应的播放规则.请检测配置表BackgroundMusic_Client")
	end
end

function PlayerMoveChangeAreaInfo()
	if not g_MainPlayer or g_GameMain.m_SceneName == nil then
		return
	end
	local Pos = {}
	local PixelPos = CFPos:new()
	g_MainPlayer:GetServerPixelPos(PixelPos)
	Pos.x = PixelPos.x / EUnits.eGridSpanForObj
	Pos.y = PixelPos.y / EUnits.eGridSpanForObj
	g_GameMain.m_AreaInfoWnd:PlayerMoveChangePlayerPosWnd()

	local AreaName = g_AreaMgr:GetSceneAreaName(g_GameMain.m_SceneName, Pos)
	
	if g_MainPlayer.m_AreaName ~= AreaName then
		local AreaMusic = g_AreaMgr:GetSceneAreaMusic(g_GameMain.m_SceneName,AreaName)

		if g_App.m_CurMusicName ~= nil and g_App.m_CurMusicName ~= AreaMusic then
			if g_GameMain.m_StopBackgroundMusicTick then
				assert(g_GameMain.m_PlayBackgroundMusicTick == nil, "播放音乐的tick 存在逻辑错误.")
				UnRegisterTick(g_GameMain.m_StopBackgroundMusicTick)
				g_GameMain.m_StopBackgroundMusicTick = nil
			elseif g_GameMain.m_PlayBackgroundMusicTick then
				assert(g_GameMain.m_StopBackgroundMusicTick == nil, "播放音乐的tick 存在逻辑错误.")
				UnRegisterTick(g_GameMain.m_PlayBackgroundMusicTick)
				g_GameMain.m_PlayBackgroundMusicTick = nil
			end
			StopCue(g_App.m_CurMusicName)
			if g_MusicCDTbl[g_App.m_CurMusicName] then
				UnRegisterTick(g_MusicCDTbl[g_App.m_CurMusicName])
			end
			g_MusicCDTbl[g_App.m_CurMusicName] = RegisterTick("ClearMusicCDTick", ClearMusicCD, 20 * 1000, g_App.m_CurMusicName)
			--print("停止播放背景音乐" .. g_App.m_CurMusicName)
			g_App.m_CurMusicName = nil
		end
		
		if AreaMusic and AreaMusic ~= "" and g_App.m_CurMusicName ~= AreaMusic then
			local cfgTbl = BackgroundMusic_Client(AreaName) or BackgroundMusic_Client(AreaMusic)
			PlayBackgroundMusic(nil, AreaMusic, cfgTbl)
		end
	end
	
	if AreaName ~= "" then 
		if AreaName ~= g_MainPlayer.m_AreaName then
			if not string.find(AreaName, "主路") then
				local AreaDislayName = GetAreaDispalyName(AreaName)
				MsgClient(3200, AreaDislayName)
			end
			if g_GameMain.m_AreaInfoWnd.m_PlayerAreaInfoTbl[AreaName] == nil
				or g_GameMain.m_AreaInfoWnd.m_PlayerAreaInfoTbl[AreaName]["IsDiscovery"] == 0 then
				Gac2Gas:PlayerChangeAreaPlace(g_Conn,Pos.x, Pos.y ,AreaName, true)
			else
				Gac2Gas:PlayerChangeAreaPlace(g_Conn,Pos.x, Pos.y,AreaName, false)
			end
			g_MainPlayer.m_AreaName = AreaName
			g_GameMain:ResetCameraRotate()
		end
--		g_GameMain.m_QuestTraceBack:ShowQuestTraceByArea(AreaName)
	elseif g_MainPlayer.m_AreaName then
		Gac2Gas:PlayerChangeAreaPlace(g_Conn, Pos.x, Pos.y,"", false)
		g_MainPlayer.m_AreaName = nil
--		g_GameMain.m_QuestTraceBack:ShowAllDoingQuestTrace()
	end

--	local ShowSceneName = g_AreaMgr:GetShowSceneName(g_GameMain.m_SceneName)
	local ShowSceneName = GetSceneDispalyName(g_GameMain.m_SceneName)
	if string.find(AreaName,"主路") then
		g_GameMain.m_NavigationMap:ShowSceneNameAndAreaName(ShowSceneName, nil)
	else
		local ShowAreaName = GetAreaDispalyName(g_MainPlayer.m_AreaName)
		g_GameMain.m_NavigationMap:ShowSceneNameAndAreaName(ShowSceneName,ShowAreaName)
	end
end

function GetSceneDispalyName(name)
	if name then
		
		if(not Lan_Scene_Basic_Common(MemH64(name))) then
			if(Scene_Common[name]) then
				print(name, Scene_Common[name].BasicName)
				name = Scene_Common[name].BasicName
			end
		end
		assert( Lan_Scene_Basic_Common(MemH64(name)), name, "不存在显示名")
		return Lan_Scene_Basic_Common(MemH64(name),"DisplayName")
	end
end

function GetAreaDispalyName(name)
	if name then
		if string.find(name,"主路") then
			name = "主路"
		end
		assert(Lan_AreaInfo_Client(MemH64(name)) ,name .. "不存在显示名")
		return Lan_AreaInfo_Client(MemH64(name), "DisplayName")
	end
end

function GetTrapDispalyName(name)
	if name then
		assert( Lan_Trap_Common(MemH64(name)), name .. "的Trap没有显示名")
		return Lan_Trap_Common(MemH64(name), "DisplayName")
	end
end
