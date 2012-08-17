gac_gas_require "framework/common/HotUpdateRequire"
engine_require "common/EngineExport"

engine_require "common/Loader/loader"
engine_require "common/Module/Module"

gac_gas_require "GacGasExport"
gac_require "GacExport"
etc_gac_require "GacConfig"
gac_gas_require "framework/common/StringSplit"

local function ParseParamFun(param)
	if param then
		local str = string.gsub(param,":","=")
		local fun = loadstring(" return { " .. str .. " }")
		if fun then
			local tbl = fun()
			return tbl
		else
			return {} 
		end
	end
end

--处理命令行信息
function g_ParseCommandline()
	local Infos = {}
	if(not g_strSelServerInfo) then
		Infos = { unpack(GacConfig.DefaultLoginConfig) }
		Infos[3] = ParseParamFun(Infos[3])
		return Infos
	end
	apcall(SplitString, g_strSelServerInfo, ";", Infos)
	Infos[3] = ParseParamFun(Infos[3])

	return Infos
end

gac_gas_require "framework/common/Random"
gac_gas_require "framework/text_filter_mgr/TextFilterMgr"
gac_require "framework/color_mgr/ColorMgr"
gac_require "framework/rpc/Gas2GacDef"	--载入gac协议实现
gac_require "framework/rpc/Db2Gac"
gac_require "message/MessageDefFun/MessageDefFun"
gac_require "entrance/change_server/ChangeServer"
gac_gas_require "activity/quest/QuestMgr"
gac_require "framework/main_frame/GlobalData"
gac_require "framework/main_frame/GameMain"
gac_require "framework/main_frame/GacEventTable"
gac_gas_require "event/Event"
gac_require "test/common/GacTestMain"
gac_require "framework/main_frame/AppGacHandler"

gac_require "framework/main_frame/OneStepLogin"
gac_require "world/area/AreaClient"
gac_gas_require "activity/quest/QuestBase"
gac_gas_require "activity/smallgame/SmallGameCfgCheck"
gac_gas_require "activity/item/LiveSkillParamsInc"
gac_gas_require "activity/quest/MercenaryLevelCfg"
gac_gas_require "character/CharacterDef"
gac_gas_require "activity/npc/NpcDef"
gac_require "framework/main_frame/GacUpdate"

LoadAllExportClass()

-- client和server的时间差异
g_nCSTimeDiff = 0

--是否选中召唤兽
g_bSelectServant = true
g_CoreSceneMgr = CBaseCoreSceneMgrClient_Inst()

function WriteToGacVarFile( sFileName, sMessage )
	WriteToVarFile( "gac/" .. sFileName , sMessage )
end

function GetServerTime()
	return os.time() - g_nCSTimeDiff
end

function SetServerTime( uServerTime )
	g_nCSTimeDiff = os.time() - uServerTime
end

function LoadHurtInfo()
	local str = GacAssembleArgs(200001,{})
	CDisplayHurt_AddHurtInfo(200001,str)
	str = GacAssembleArgs(200002,{})
	CDisplayHurt_AddHurtInfo(200002,str)
	str = GacAssembleArgs(200003,{})
	CDisplayHurt_AddHurtInfo(200003,str)
	str = GacAssembleArgs(200004,{})
	CDisplayHurt_AddHurtInfo(200004,str)
	str = GacAssembleArgs(200005,{})
	CDisplayHurt_AddHurtInfo(200005,str)
	str = GacAssembleArgs(200006,{})
	CDisplayHurt_AddHurtInfo(200006,str)
	str = GacAssembleArgs(200007,{})
	CDisplayHurt_AddHurtInfo(200007,str)
end

function GacMain()
	g_App:SetHandler(CAppGacHandler:new())
	local Infos			= g_ParseCommandline()
	local sServerName	= (not Infos[2] or "" == Infos[2]) and "" or " - " .. Infos[2]
	local sTitle = GetStaticTextClient(1) .. sServerName
	g_App:SetMainWndTitle(sTitle)
	g_LoadGeniusNamefun()
	--g_CheckProperRes()
	if( LoadSkillGac() ) then
		print( "[successful][load skill config]." )
	else
		error( "[fail][load skill config]" )
	end
	if( not CBloodTypeCfg_LoadCfg() ) then
		error( "[fail][load BloodTypeCfg config]" )
	end
	
	LoadHurtInfo()

	Main  = nil
		
	--游戏初始化
	CGameState:InitMessageBox()
	CGameState:Init()
	
	--游戏大循环
	g_App.m_re = EGameState.eToLogin

	while true do

		g_App.m_MsgBox = nil
		--游戏结束 
		if( g_App.m_re == EGameState.eExitGame ) then
			g_App:Quit()
			break
			
		--登陆
		elseif( g_App.m_re == EGameState.eToLogin ) then
			g_Login = CLogin:new()
			
			-- 如果界面隐藏，显示界面
			if g_App.m_IsGUIHide then
				OnGUIAlphaKey()
			end
			
			g_App.m_re = g_Login:main()
			
			if g_Login.m_LoginTick then
				UnRegisterTick(g_Login.m_LoginTick)
				g_Login.m_LoginTick = nil
			end
			
			g_Login = nil
			
		--选择角色
		elseif ( g_App.m_re == EGameState.eToSelectChar ) then
			g_SelectChar = CSelectCharState:new()
			g_App.m_re = g_SelectChar:main()
			g_SelectChar = nil
		--创建角色
		elseif ( g_App.m_re == EGameState.eToNewRoleGameMain ) then
			g_NewRoleEnterGame = CNewRoleEnterGame:new()

			g_App.m_re = g_NewRoleEnterGame:main()
			g_PlayerInfos = nil
			g_GameMain = nil
			
		--是否需要显示协议
		elseif (g_App.m_re == EGameState.eToAgreement) then
			g_UserAgreement = CUserAgreement:new()
			g_App.m_re = g_UserAgreement:main()
			g_UserAgreement = nil
		
		--游戏，创建角色是在主界面yeild之后进行的，在GameScene的loading完后显示出来
		elseif ( g_App.m_re == EGameState.eToGameMain ) then
			g_PlayerInfos = {}
			g_GameMain = CGameMain:new()
			RegOnceTickLifecycleObj("g_GameMain", g_GameMain)
			
			g_GameMain:OnInit()
	
			g_App.m_re = g_GameMain:main()
			g_GameMain = nil
			g_PlayerInfos = nil
		
		--未知 coroutine 返回，返回值错误
		else
			error( "error, invalid return from game state, re = ", g_App.m_re )
		end
	end
	--print("Before the GacMian end")
	CGameState:End()
	g_App:SetHandler(nil)
end


function RunGacMain()
	local co = coroutine.create(GacMain)
	coroutine.resume(co)
	return co
end

local function ClientLogMemInfo()
	local msg = GetAllMemInfo()
	g_MemLog:Write(msg)	
end

function StartUp()
	SetCheckRegisterTick(DevConfig.CheckMemLeak == 1)
	
	if GacConfig.LogMemEnabled == 1 then
		local filename = "Mem_" .. GetCurrentPID() .. ".log"
		g_MemLog = CLog_CreateLog(filename, "wb")
		g_MemLog:EnableTime(false)
		g_LogMemTick = RegisterTick("LogMemInfo", ClientLogMemInfo, GacConfig.LogMemTime)	
	end
	
	g_App.MainRoutine = RunGacMain()
	local TestCo = RunGacTest()
	local StepCo = One_Step_Quick_Login()
	CEntityClientManager_CreateInst()
	
	RegOnceTickLifecycleObj("g_App", g_App)
	
	collectgarbage("collect")
end


function CleanUp()
	if GacConfig.LogMemEnabled == 1 then
		UnRegisterTick(g_LogMemTick)
		g_MemLog:Release()
		g_LogMemTick = nil
	end

	CEntityClientManager_DestroyInst()
	UnloadSkillGac()
	CBloodTypeCfg_UnLoadCfg()
	g_App:SetHandler(nil)
	
	UnRegisterObjOnceTick(g_App)
	
	ClearLoadedCfgTbl()
	
	PrintUnRegisterTick()
end

--写入客户端游戏状态切换及角色、场景创建、销毁的log
function g_InsertTimeSeries(sEvent)
	local now = os.time()
	local sDate = string.gsub( os.date("%x", now), "/", "_" )
	local strFile = "TimeSeriesLog_" .. sDate .. ".log"
	
	--创建一个log，以append方式
	local log = CLog_CreateLog(strFile, "a")
	log:EnableTime(true)
	log:Write(sEvent)
	log:Release()
end
