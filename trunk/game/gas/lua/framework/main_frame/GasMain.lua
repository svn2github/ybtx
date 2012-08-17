gac_gas_require "framework/common/HotUpdateRequire"
engine_require "common/EngineExport"

etc_gas_require "GasConfig"
engine_require "common/Loader/loader"
engine_require "common/Module/Module"

gac_gas_require "GacGasExport"

gas_require "GasExport"
gac_gas_require "skill/SkillCommon"
engine_require "server/cpcall"
gac_gas_require "framework/common/Random"
gac_gas_require "framework/text_filter_mgr/TextFilterMgr"
gas_require "multiserver/CMultiServerConn"
gas_require "multiserver/Gas2GasProList"
gas_require "multiserver/MultiServerConnMgr"
gas_require "multiserver/ServerMsgCommon"
gas_require "multiserver/ClientMsgMgr"
gas_require	"multiserver/DB2GasDef"
gas_require	"multiserver/DB2CallbackDef"
gas_require	"multiserver/Gas2GasDef"
gas_require	"multiserver/CallGasDef"
gas_require "multiserver/MessageRouter"
gas_require "multiserver/TransferData"
gas_require	"multiserver/CallGas"
gas_require "multiserver/PlayerChangeServerData"
gas_require "framework/rpc/Gac2GasDef"
gas_require "framework/main_frame/ConnServer"
engine_require "server/CoreCallDbTrans"
engine_require "server/OnHandleDbCallback"
gas_require "framework/base_mgr/GlobalDbTransRegister"
gas_require "framework/main_frame/GasUpdate"
gac_gas_require "activity/scene/LoadPositionCfg"
gac_gas_require "activity/quest/QuestMgr"
gac_gas_require "activity/quest/QuestBase"
--gas_require "world/npc/SpecialNpc"
gac_gas_require "character/NameColorDef"
gas_require "framework/rpc/CScriptConnMgrServerHandler"
gas_require "test/common/GasTestMain"
gas_require "world/scene_mgr/SceneMgr"
gac_gas_require "activity/scene/AreaFbSceneMgr"
gas_require "fb_game/match_game/MatchGameMgr"
gas_require "fb_game/GameCampMgr"
gas_require "world/common/CommLoadProgress"
gas_require "framework/base_mgr/AccountMgr"
gas_require "framework/main_frame/AppGasHandler"
gas_require "entrance/logout/GasLogout"
gac_gas_require "item/item_info_mgr/ItemInfoMgr"
gac_gas_require "item/store_room_cfg/StoreRoomCfg"
gas_require "item/useitem/GasUseItem"
gas_require "framework/main_frame/ViewGroupMgr"			--视野群管理器
gac_gas_require "team/TeamMgr"
gac_gas_require "framework/common/CAlarmClock"
gac_gas_require "framework/common/OnceTickMgr"
--gas_require "world/tong_area/FetchRes"
gas_require "framework/main_frame/SandBoxDef"
gas_require "framework/main_frame/GasLimit"
gac_gas_require "relation/tong/TongMgr"
gac_gas_require "activity/npc/CheckNpcDropSetting"
--gac_gas_require "activity/npc/CheckNpcFacialResponse"
gac_gas_require "activity/npc/CheckNpcAIMutexRule"
gac_gas_require "activity/npc/CheckNpcTriggerAction"
gac_gas_require "activity/Trigger/NpcTrigger"
gac_gas_require "activity/smallgame/SmallGameCfgCheck"
gas_require "world/trigger_action/obj/ObjTriggerAction"
gas_require "world/trigger_action/obj/assign_mode/AcutionAssign"
gas_require "framework/main_frame/GlobalData"
gas_require "fb_game/FbGame_Event"
gas_require "world/trigger_action/TriggerCommonFun"
gas_require "world/trap/TrapServer"
gas_require "world/npc/IntNpcNatureInfo"
gas_require "smallgames/SGMgr_Gas"
--gas_require "activity/essayquestion/EssayQuestion"
gas_require "e_protocol/EProtocol"
gas_require "e_protocol/EService"
gas_require "e_protocol/HttpClient"
gas_require "framework/web_server/WebServer"
gas_require "e_protocol/EProMsgRecv"
--gac_gas_require "activity/scene/CheckAreaFbScene"
--gac_gas_require "activity/npc/CheckRandomCreate"
gas_require "world/npc/NpcShowContent"   --yy require NPC显示脚本
gac_gas_require "areamgr/AreaMgr"
gas_require "world/area_server/AreaServer"
gas_require "world/tong_area/WarZoneMgr"

gas_require "world/tong_area/truck"
gas_require "world/tong_area/TongLoadProgress"

gas_require "world/tong_area/TongFbMgr"
gas_require "world/tong_area/TongChallengeFbMgr"
gas_require "world/tong_area/BuildingMgr"
gas_require "world/tong_area/TongMonsAttackStationMgr"
gas_require "world/common/CommWorldFun"
gac_gas_require "activity/item/LiveSkillParamsInc"
gas_require "fb_game/Drink"
gas_require "world/tong_area/Challenge"
gas_require "world/tong_area/BuildingAreaMgr"
gas_require "framework/main_frame/CApexProxyCallBackHandler"
--gas_require "world/scene_mgr/ScenePkMgr"
gas_require "world/tong_area/TongRobResourceMgr"
cfg_load "skill/BuffAndDebuff_Common"
gac_gas_require "activity/quest/MercenaryLevelCfg"
gas_require "activity/NpcTalk"
gas_require "fb_game/BossBattle"
gac_gas_require "character/CharacterDef"
gac_gas_require "activity/npc/NpcDef"
gas_require "commerce/buy_coupons/GasBuyCoupons"

LoadAllExportClass()

function WriteToGasVarFile( sFileName, sMessage )
	WriteToVarFile( "gas/" .. sFileName , sMessage )
end

g_ServerList = {} --
g_ServerAttribute = {}
g_IssueForage = {}
g_RobResMgr = CTongRobResMgr:new()

local function AddNewServer(serverId, clientIp, serverIp, clientport, serverport)
	g_ServerList[serverId] = {}
	g_ServerList[serverId].clientIp = clientIp
	g_ServerList[serverId].serverIp = serverIp
	g_ServerList[serverId].clientport = clientport
	g_ServerList[serverId].serverport = serverport
end

function GetServerIporPort(serverId)
	if g_ServerList[serverId] then
		return g_ServerList[serverId].clientIp, g_ServerList[serverId].clientport
	end
end

local function InitServerList(serverTbl)
	for _, v in pairs(serverTbl) do
		AddNewServer(v[1], v[2], v[3], v[4], v[5])
	end
	
	if not g_ServerList[g_CurServerId] then
		ShowErrorMsgAndExit(g_CurServerId .. " 号服务器未在数据库内配置")
	end
	
	local localip = g_ServerList[g_CurServerId].serverIp
	local localport = g_ServerList[g_CurServerId].serverport
	--监听本服务器端口
	CreateListenOfLocalAddr(localip,localport)
	
	for serverId, serverData in pairs(g_ServerList) do
		if serverId > g_CurServerId then
			local ip = g_ServerList[serverId].serverIp
			local port = g_ServerList[serverId].serverport
			CreateConnToTargetServer(serverId,ip,port)
		end
	end
end

local function GasStartUpAfterDB(res)
	--场景管理需要查寻数据库，所以放在这里初始化
	g_SceneMgr:Launch(res)
	--向GBS发送退出记录
	SendQuitRecord()
	--服务器重启时将表tbl_user_online中上次没正常发送的退出记录复制到表tbl_auth_logout中以发送到GBS
	CopyUnSentRecord()
	--RunGasTest(g_ConnMgrServerHander)
end


--用于记录服务器宕机时间
local function ServerTimeUpdateTick()
	CallDbTrans("GasMainDB","ServerTimeUpdateTick", nil, {["CurServerId"] = g_CurServerId})
end

local function DelNoUseItemIdTick()

	CallDbTrans("GasMainDB","DelNoUseItemId", nil,{})
end

local function UpdateOnlineStatTick()
	
	CallDbTrans("GasMainDB","UpdateOnlineStat", nil,{}, 888888)
end

local function SaveServiceOnlineUserInfoTick()
	
	CallDbTrans("GasMainDB","SaveServiceOnlineUserInfo", nil,{})
end



local function GasStartUpInit()
	g_AppHandler = CAppGasHandler:new()
	g_App:SetHandler(g_AppHandler)
	RegOnceTickLifecycleObj("g_App", g_App)
	-- 配置表全局对象
	g_ItemInfoMgr = g_ItemInfoMgr or CItemInfoMgr:new()
	
	--网络连接
	g_ConnMgr = g_App:GetConnMgr()	

	--场景管理
	g_MetaSceneMgr = CBaseMetaSceneMgrServer_Inst()
	g_CoreSceneMgr = CCoreSceneMgrServer_Inst()
	
	g_SceneMgr	 = CSceneMgr:new()
	--全局帐户管理
	g_AccountMgr = CAccountMgr:new()
	--上下文管理器
	g_ContextMgr = CContextMgr:new()
	
	--玩法阵营管理
	g_GameCampMgr = CGameCampMgr:new()
	--触发脚本
	g_TriggerScript = CTriggerScript:new()
	g_TriggerMgr = CTriggerMgr:new()
	--Sleep
	g_sleep = CSleep:new()
	--设置rpc(远程过程调用)
	
	--启动反外挂系统 Apex
	local ApexServerAddr= CAddress:new()
	ApexServerAddr:SetAddress( GasConfig.ApexProxyAddress )
	ApexServerAddr:SetPort(GasConfig.ApexProxyPort )
	CApexProxy_CreateInst()
	CApexProxy_GetInst():StartApexProxy(ApexServerAddr)
	g_ApexCallBackHandler = CApexProxyCallBackHandler:new()
	CApexProxy_GetInst():SetApexProxyCallBackHandler(g_ApexCallBackHandler)
	ApexServerAddr = nil
	--改写ShutDown函数。在断开链接之前给ApexItemServer发G消息
	local OldShutDown = CConnServer.ShutDown
	CConnServer.OldShutDown = OldShutDown
	function NewShutDown(conn, str)
		CApexProxy_GetInst():NoticeApexProxy_UserLogout(conn.m_Account:GetID(),conn.m_Account:GetName())
		OldShutDown(conn, str)
	end
	CConnServer.ShutDown = NewShutDown	
	
	CYYServer_CreateInst(CBaseScriptApp_Inst():GetRootFilePath(nil) .. "/bin/Release/")
	
	g_ConnMgrServerHander = CConnMgrServerHandler:new()
	
	local ScriptConnMgrServerHandler = CScriptConnMgrServerHandler:new()

	ScriptConnMgrServerHandler:RegistCall("gac_gas", "framework/rpc/Gas2GacProList","ISend")
	ScriptConnMgrServerHandler:RegistDef("gac_gas", "framework/rpc/Gac2GasProList","CConnServer")	
	g_ConnMgrServerHander:SetScriptHandler(ScriptConnMgrServerHandler)
	
	
	g_ConnMgr:SetHandler(g_ConnMgrServerHander)
	GetBuyCouponsWebInfo()
	
	--视野群管理器，主要处理隐身功能
	g_ViewGroupMgr = CViewGroupMgr:new()

	g_ServerTimeUpdateTick	=	RegisterTick("ServerTimeUpdateTick", ServerTimeUpdateTick,5*60*1000)
	if g_CurServerId == 1 then
		g_DelNoUseItemIdTick	=	RegisterTick("DelNoUseItemIdTick", DelNoUseItemIdTick,60*1000)
		g_UpdateOnlineStatTick = RegisterTick("UpdateOnlineStatTick", UpdateOnlineStatTick,60*1000)
		g_SaveServiceOnlineUserInfoTick = RegisterTick("SaveServiceOnlineUserInfoTick", SaveServiceOnlineUserInfoTick,300*1000)
		--注册时钟,定时刷新角色相关信息排行榜
		CSortMgr.CreateCharSortTbl()
		--注册时钟,定时刷新佣兵小队相关信息排行榜
		CSortMgr.CreateTongSortTbl()
		--注册时钟,定时刷新装备特效
		CGasFightingEvaluation.UpdateEquipEffect()
		g_CSMarket=RegisterTick("g_CSMarketTick", CSMOrderTimeTests,10*1000*60)     --判断收购和出售的物品是否到规定时间
		g_ContractManufacture=RegisterTick("g_ContractManufacture", CheckContractManufactureOrderTimeOut,10*1000*60)     --判断收购和出售的物品是否到规定时间
		CTongBasic.CheckAndChangeTongCaptain()
		CTongBasic.AutoChangeTongCaptainTick()
		CTongBasic.SettleAccountsForTongDevelopDegree()
		CTongBasic.SendAwardToTongCaptain()
		CTongNeedFireActivity.CleanTongNeedFireTick()
		CUseVIPItem.SendMailItemToVip()
		CCopyMatchGame.AlarmClock()
		UserAdultCheckTick()
		CDragonCave:DeleteDragonBuildingTask() --龙穴活动特殊建筑有效期检测tick
		CDragonCave:DragonCaveTick()  -- 设置龙穴活动计时器
		CRoomRpcMgr.g_InitSoulPearlTimeInfo()
		CRoomRpcMgr.ClearItemNumInfo()
		CTongContriMoney.ClearContributeMoneyInfoTick()
		CExchangeMoney.SendEratingAuditTick()
		CExchangeMoney.EratingAuditTick()
		CExchangeMoney.ClearWMXInfo()
		CGasToolsMall.ClearToolsmallBuyedInfo()
		CGasCompenate.AssignUserSoulAndExp()
		CTongRobResMgr.ClearAwardInfo()
		MailMgr.DelAllOvertimeMail()
		CTongRobResMgr.StartClearAwardInfo()
		CUseVIPItem.SendWelfareLevel1MailItem()
		CUseVIPItem.SendWelfareLevel2MailItem()
		CUseVIPItem.SendWelfareLevel3MailItem()
		CUseVIPItem.SendWelfareLevel4MailItem()
	end
	g_TeamHPMP = RegisterTick("g_TeamHPMP", CGasTeam.SendTeamMemberHPMP,10*1000)
	g_limit_server_tick = RegisterTick("g_limit_server_tick", limit_server_tick_func, 60 * 1000 * 1000)
	if GasConfig.SkipGBSAuth ~= 1 then
		g_SendUserDrawBalanceInfo = RegisterTick("g_SendUserDrawBalanceInfo", CExchangeMoney.SendUserDrawBalanceInfo,3*1000)
		g_ClearUserDrawBalanceMemoryInfo = RegisterTick("g_ClearUserDrawBalanceMemoryInfo", CExchangeMoney.ClearUserDrawBalanceMemoryInfo,30*1000)
	end
		
	local pid = GetCurrentPID()	
	local filename = "gms_" .. pid .. ".log"
	g_GMSLog = CLog_CreateLog(filename, "wb")
	g_GMSLog:EnableTime(true)
	
	--if GasConfig.OpenCfgSwitch == nil then
		--assert( false, "请运行AutoConfig!")
	--end
	g_AllPlayerSender = CPlayerSet:new()
	CampPlayerSenderTbl = {}
	CampPlayerSenderTbl[1] = CPlayerSet:new()
	CampPlayerSenderTbl[2] = CPlayerSet:new()
	CampPlayerSenderTbl[3] = CPlayerSet:new()
	local function global_user_num(tblUserNum)
		g_OnlineUserNum = {}
		for i=1,tblUserNum:GetRowNum() do
			g_OnlineUserNum[tblUserNum(i,2)] = tblUserNum(i,1)
		end
	end
	CallDbTrans("LogoutServerDB", "GetGlobalUserNum", global_user_num, {})

--	g_VipUserWaitTbl = {}				--存放vip用户的排队信息
	g_FriendGroupMultiMsgSenderTbl = {}
	RegMemCheckTbl("g_FriendGroupMultiMsgSenderTbl", g_FriendGroupMultiMsgSenderTbl)
	
	local function PrintTickNum()
		g_App:PrintRegisterTickNum(g_RegisteredLuaTickNum)
	end
	g_PrintTickNum = RegisterTick("PrintTickNum", PrintTickNum, 600 * 1000) 
	
	g_LoadGeniusNamefun()
	InitNpcResBaseData()		--这行必须放在LoadSkillCfg()前面
	if( LoadSkillCfg() ) then
		local Keys = BuffAndDebuff_Common:GetKeys()
		for i = 1,#(Keys) do
			CBaseStateCfgServer_SetNeedSyncByName(Keys[i])
		end
	
			--print( "[successful][load skill config]." )

	else
		error( "[fail][load skill config]" )
	end
	
	if( CppGameServerInit() == false) then
		error( "[fail][load Cpp Gas config]")
	end

	if (InitServerNpcBaseData() == false) then
		error( "[fail][Init Server Npc Base Data]")
	end
	CNpcResServerData_CheckNpcAniKeyFrameCfg()		--检查Npc攻击关键帧和全帧信息

	CEntityServerManager_CreateInst()
	
	CDragonCave:CheckDragonCaveList()  --检查龙穴活动列表
	SetBossBattleTick()	--设置Boss争夺战计时器
	
	g_ServerExpModulus = 1	--服务器经验系数
	
	--GM关闭的活动tbl
	g_ClosedActionTbl = {}

	--角色登录流程表
	g_CharLoginFuncFlow = {}
end


local function GetTime(t)
	local h = math.floor(t/3600)
	local m = math.floor(t/60) - h *60
	local c = t - 60 * math.floor(t/60)
	return h, m, c
end

--

function BackEquip()
	local function CallBack(info)
	local onetime = 100
	local begin = info["min"]
	local _max = info["max"]
	local tbl={}
	local length = math.ceil((_max - begin)/32)
	
	local function itemCallBack(index)
		coroutine.resume(tbl[index])
	end
	local function init(index, num)
		local begin = num
		local max = num + length
		local beginTime = os.time()
                local msg = "线程"..index.." 已经执行：%d 小时 %d 分钟 %d 秒  本次开始条目：%d"
                while true do
			local _min = begin
                	begin = begin + onetime
			if _min <= max then
	                        local time1 = os.time() - beginTime
				local h1, m1,c1 = GetTime(time1)
				print(string.format(msg, h1, m1, c1, _min))
				local itemArg = {}
				itemArg["MinId"] = _min
				itemArg["MaxId"] = _min + onetime
				itemArg["Index"] = index
				CallDbTrans("GMToolsDbActionDB", "TempBackData",itemCallBack, itemArg)
				coroutine.yield(index)
			else
				print("完成!!!!!!!!!!!!!!!!!!!")
                                break
			end
		end
	end

	for i = 0,32 do
		tbl[i] = coroutine.create(init)
		coroutine.resume(tbl[i], i, begin+(i*length))
	end
	end
	CallDbTrans("GMToolsDbActionDB", "TempBackData_ChangRange",CallBack, {})
end

function BuChang()
	local tbl={}

	local function itemCallBack(index)
		coroutine.resume(tbl[index])
			
	end


	local function CallBack(info)
		local n = 8
		local _max = info.item["MaxId"]
		local _min = info.item["MinId"]
		local _num = math.ceil( (_max - _min) / n) + 1
		
		
		local function init(min, max, index)	
	
			local count = 10
			local itemNum  = math.ceil ( (max - min) / count) + 1
			local itemBeginId = min
			local size = max - min
			
			local beginTime = os.time()
			local msg = "完成度：%.2f%%  已经执行：%d 小时 %d 分钟 %d 秒     剩余：%d 小时 %d 分钟 %d 秒"
			
			while true do
				
				local time1 = os.time() - beginTime
				local time2 = (max -itemBeginId) * time1 / (itemBeginId - min)
				if itemBeginId <= info.item["MaxId"] then
					local h1, m1,c1 = GetTime(time1)
					local h2, m2, c2 = GetTime(time2)
					print(string.format(msg, (itemBeginId - min) * 100/size , h1, m1, c1, h2, m2, c2 ), itemBeginId)
					local itemArg = {}
					itemArg["MinId"] = itemBeginId
					itemBeginId = itemBeginId + itemNum
					itemArg["MaxId"] = itemBeginId -1  --闭区间
					itemArg["Index"] = index
					CallDbTrans("BuChangDB", "DoItemByID", itemCallBack, itemArg)
					coroutine.yield()
				else
					print("完成!!!!!!!!!!!!!!!!!!!")
					break
				end
			
			end
		
		
		end
		
		local i = 1
		local b = _min
		
		while b <= _max do
				
			tbl[i] = coroutine.create(init)
			coroutine.resume(tbl[i], b, b + _num -1, i)
			b = b +_num
			i = i +1
		end
		
	end
		
	CallDbTrans("BuChangDB", "GetBuChangItemIDRange", CallBack, {})
end


function StartUp()
	if GasConfig.BuChangCheck == 1 then
		BuChang()
		
  		--CallDbTrans("CTongBuildingRecover", "Recover", nil, nil)
  		--CallDbTrans("GMToolsDbActionDB", "TempBackData", nil, {})
		return
	end
	print("当前lua版本:" .. _VERSION)
	if jit then
		print ("luajit状态:" .. tostring(jit.status()))
	end	
	
	SetCheckRegisterTick(DevConfig.CheckMemLeak == 1)
	
	GasStartUpInit()
	
	local function CallBack(res, warZoneRes,serverTbl,closedActionRes,auctionRes, serverAttribute)
		g_ServerAttribute = serverAttribute
		--g_CurServerId = curServerId
		g_ClosedActionTbl = closedActionRes
		InitServerList(serverTbl)
		g_WarZoneMgr:Init(warZoneRes)
		GasStartUpAfterDB(res)
		InitTongChallenge()
		CTongRobResMgr.InitRobResInfo(nil)
		CTongRobResMgr.StartUpGetRobInfo()
		CFetchRes.ResumeForage(nil)
		CRecruitMgr.ResumeRecruitState()
		ReturnAllAuctionMoney(auctionRes)
	--	ResumeData()
		CFetchRes.RetFetchInfo(nil)
		RegisterCupStatusTick()
		TestFunctionEntry()
	end
	--local GasMainDB = (g_DBTransDef["GasMainDB"])
	
--	data["ip"] = GasConfig.Address
--	data["ClientPort"] = GasConfig.Port
--	data["ServerPort"] = GasConfig.MultiServerPort
	
	--启动服务器后进行读数据库热更新数据
	HotUpdateDb()
	HotUpdateGasGac()
	
	CallDbTrans("GasMainDB", "GasStartUp", CallBack, {}, "GasStartUp" )
    --CallDbTrans("ConsignmentDB", "ReadCfgToCSMDBTable", nil, {})
	local pid = GetCurrentPID()
	local filename = "EPRT_" .. pid .. ".log"
	g_EPrtlLog = CLog_CreateLog(filename, "wb")
	g_EPrtlLog:EnableTime(true)
	
	
	local YYfilename = "YY_" .. pid .. ".log"
	g_YYLog = CLog_CreateLog(YYfilename, "wb")
	g_YYLog:EnableTime(true)

	local function InitGasConf(result)
		
		for i=1, result[1]:GetRowNum() do
			
			ERatingConfig[result[1](i,1)]= result[1](i,2)
			--print(string.format("ERatingConfig[%s]=%s",result[1](i,1),result[1](i,2)))
		end
		g_EProtocol:Connect()

		for i=1,result[2]:GetRowNum() do
			g_tblGmsValidIP	= g_tblGmsValidIP or {}
			g_tblGmsValidIP[result[2](i,1)] = true
		end
		g_ServerType = 0
		if result[3]:GetRowNum() > 0 then
			g_ServerType = result[3](1,1)
		end
		
		g_WebServer:Listen()
		--g_EProMsgRecv:Listen()
	end

	CallDbTrans("GasMainDB", "InitGasConf", InitGasConf, {})
	
	
	
	
	
	
	
	
	
	
	---------------初始化代码写在上面---------------------	
	---------------初始化代码写在上面---------------------	
	---------------初始化代码写在上面---------------------	
	---------------初始化代码写在上面---------------------	
	---------------初始化代码写在上面---------------------	
	local function StartUpFinalCallBack(result)
		-----------这个函数里不要添加代码----------------
		-----------这个函数里不要添加代码----------------
		-----------这个函数里不要添加代码----------------
		local addr= CAddress:new()
		addr:SetAddress(g_ServerList[g_CurServerId].clientIp)
		addr:SetPort(g_ServerList[g_CurServerId].clientport)
		
		--在真个启动完成之后，我们进行一次全gc	
		collectgarbage("collect")	
	
		g_ConnMgr:BeginService(addr,2*1024*1024,64*1024)
		-----------这个函数里不要添加代码----------------
		-----------这个函数里不要添加代码----------------
		-----------这个函数里不要添加代码----------------
		-----------这个函数里不要添加代码----------------
	end
	
	CallDbTrans("GasMainDB", "GasStartUpFinalize", StartUpFinalCallBack, {}, "GasStartUp" )
	
	-----------------这里不要写代码------------------------
	-----------------这里不要写代码------------------------
	-----------------这里不要写代码------------------------
	-----------------这里不要写代码------------------------
	-----------------这里不要写代码------------------------
end


function OnBeginServiceSucceeded()
	-----------这个函数里不要添加任何代码----------------
	-----------这个函数里不要添加任何代码----------------
	-----------这个函数里不要添加任何代码----------------
	-----------这个函数里不要添加任何代码----------------
	print("Game server start up Successfully") --请不要删除这个打印
	print( "服务器启动完毕。这则信息说明服务器已经打开了网络服务，如果还有数据初始化操作在后面执行，请挪到这句话的前面。" )
	print( "如果数据载入流程是首次运行时载入，这种情况是被允许的而且是鼓励的。" )
	-----------这个函数里不要添加任何代码----------------
	-----------这个函数里不要添加任何代码----------------
	-----------这个函数里不要添加任何代码----------------
	-----------这个函数里不要添加任何代码----------------
end


function OnBeginServiceFailed()
	print( "BeginService failed.")
	g_App:Quit()
end


function CleanUp()
	UnRegisterObjOnceTick(g_App)  --注销全局生命周期的一次性tick
	ClearCallGasTick()
	UnRegisterTick(g_PrintTickNum)
	g_PrintTickNum = nil
	
	if CApexProxy_GetInst()then
		CApexProxy_GetInst():StopApexProxy()
		CApexProxy_GetInst():SetApexProxyCallBackHandler(nil)
		g_ApexCallBackHandler = nil
		CApexProxy_DestroyInst()
	end
	CEntityServerManager_DestroyInst()

	CAoiDimFilter_Unload()
	UnloadSkillCfg()  -- 放最后 !!! 
	CppGameServerClear()
	--CFightSkillProf_GetProf():TurnOffFightSkillProf()	--调试用的技能统计功能
	
	--清理所有的Tick
	--如果该tick里面调用了数据库操作，请将该tick的注销放到
	--AppGasHandler里面的UnRegisterCallDbTick里面
	UnRegisterTick(g_limit_server_tick)
	g_limit_server_tick = nil
	UnRegisterFacialTick()                     --删除表情冷却TICK
	UnRegisterTick(g_AGIP_Connect_tick)
	ShutDownNotifyOtherServer()
	g_AGIP_Connect_tick = nil
	
	CTongRobResMgr.ClearRobResTick()
	ClearTongChallengeTick()
	if g_App.m_RecruitCamp then
		UnRegisterTick(g_App.m_RecruitCamp)
		g_App.m_RecruitCamp = nil
	end
	g_AlarmClock:ClearTick()
	g_WarZoneMgr:ClearTick()
	g_WarZoneMgr:RemoveWarZoneHelper()
	g_TongMonsAttackStationMgr:ClearTick()
	g_MatchGameMgr:ClearTick()
	--g_BuildingMgr:ClearTick()
	if GMBossBattleTick then
		UnRegisterTick(GMBossBattleTick)
		GMBossBattleTick = nil
	end
	ClearAllWarner()
	
	g_EPrtlLog:Release()
	g_EPrtlLog = nil
	
	g_GMSLog:Release()
	g_GMSLog = nil
	
	g_YYLog:Release()
	g_YYLog = nil
	
	g_ConnMgrServerHander:SetScriptHandler(nil)
	ScriptConnMgrServerHandler = nil
	
	g_ConnMgr:SetHandler(nil)
	g_ConnMgrServerHander = nil
	
	g_App:SetHandler(nil)
	g_AppHandler = nil
	
	CValueTable.Clear()
	ClearLoadedCfgTbl()
	
	PrintUnRegisterTick()
end

function RegisterCupStatusTick()
	local function OnTick()
		local data = {}
		data["MaxThreadUsage"] = CThreadCpuUsageMgr_GetTopThreadCpuUsage(30)
		data["AllUsage"] = CCpuUsageMgr_GetSystemCpuUsage(30)
		CallDbTrans("GasMainDB", "UpdateCupStatus", nil, data, 777777)
	end
	if not( g_ServerAttribute[g_CurServerId] and g_ServerAttribute[g_CurServerId].IsNoFbScene ) then
		OnTick() --先调用一次
		g_CupStatusTick = RegisterTick("CupStatusTick", OnTick, 15000)
	end
end


function TestFunctionEntry()
--	local function OnTestTick()
--		CallDbTrans("ClientMsgDB", "TestDb2Gac", nil, {}, "a")
--	end
--	
--	local function Once()
--		RegisterTick("test", OnTestTick, 50)
--	end
--	
--	RegisterOnceTick(g_App, "test", Once, 30000)
end
