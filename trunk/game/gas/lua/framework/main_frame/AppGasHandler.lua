gas_require "framework/main_frame/AppGasHandlerInc"
engine_require "server/CoreServerQuit"
gac_gas_require "reload/Reload"

--如果一些tick调用了calldbtrans，那么我们在关服之前，要注销这些tick
--防止数据库结束之后，还有tick调用数据库操作
local function UnRegisterCallDbTick()
	--清理所有的Tick
	UnRegisterTick(g_ServerTimeUpdateTick)
	g_ServerTimeUpdateTick = nil
	UnRegisterTick(g_DelNoUseItemIdTick)
	g_DelNoUseItemIdTick = nil
	UnRegisterTick(g_UpdateOnlineStatTick)
	g_UpdateOnlineStatTick = nil	
	UnRegisterTick(g_SaveServiceOnlineUserInfoTick)
	g_SaveServiceOnlineUserInfoTick = nil	
	UnRegisterTick(g_CSMarket)
	g_CSMarket = nil
	UnRegisterTick(g_ContractManufacture)
	g_ContractManufacture = nil
	if g_UserAdultCheckTick then
		UnRegisterTick(g_UserAdultCheckTick)
		g_UserAdultCheckTick = nil
	end
	if g_AutoChangeTongCaptain then
		UnRegisterTick(g_AutoChangeTongCaptain)
		g_AutoChangeTongCaptain = nil
	end
	if g_SendUserDrawBalanceInfo then
		UnRegisterTick(g_SendUserDrawBalanceInfo)
		g_SendUserDrawBalanceInfo = nil
	end
	if g_ClearUserDrawBalanceMemoryInfo then
		UnRegisterTick(g_ClearUserDrawBalanceMemoryInfo)
		g_ClearUserDrawBalanceMemoryInfo = nil
	end
	if g_SendEratingAuditTick then
		UnRegisterTick(g_SendEratingAuditTick)
		g_SendEratingAuditTick = nil
	end
	UnRegisterTick(g_TeamHPMP)
	g_TeamHPMP = nil
	
	g_BuildingMgr:ClearTick()
	
	if g_ClearArgTick then
		UnRegisterTick(g_ClearArgTick)
		g_ClearArgTick = nil
	end
	
	if GasConfig.LogMemEnabled == 1 then 
		UnRegisterTick(g_LogMemTick)
		g_MemLog:Release()
		g_LogMemTick = nil
	end

	for sKey, v in pairs(g_SecretKeyTbl) do
		UnRegisterTick(v.Tick)
		g_SecretKeyTbl[sKey] = nil
	end
	
	ClearConnOtherServerTick()
	for i,v in pairs(g_TeamMgr.m_tblMembers) do
		if v["MultiMsgSender"] then
			v["MultiMsgSender"]:Release()
		end
	end
	for i,v in pairs(g_GasTongMgr.m_tblMembers) do
		if v["MultiMsgSender"] then
			v["MultiMsgSender"]:Release()
		end
	end
	for i,v in pairs(g_GasArmyCorpsMgr.m_tblMembers) do
		if v["MultiMsgSender"] then
			v["MultiMsgSender"]:Release()
		end
	end
	for k, v in pairs(g_FriendGroupMultiMsgSenderTbl) do
	    v:Release()
	    g_FriendGroupMultiMsgSenderTbl[k] = nil
	end
	g_AllPlayerSender:Release()
	for i = 1,3 do
		CampPlayerSenderTbl[i]:Release()
		CampPlayerSenderTbl[i] = nil
	end
	
	SaveAllDelayLogoutData()
	if g_GMSendGlobalAnousmentTick then
		UnRegisterTick(g_GMSendGlobalAnousmentTick)
		g_GMSendGlobalAnousmentTick = nil
	end
	
	if g_CupStatusTick then
		UnRegisterTick(g_CupStatusTick)
	end
	if g_LoginQueueTick then
		UnRegisterTick(g_LoginQueueTick)
	end
	if g_AutoLoginTick then
		UnRegisterTick(g_AutoLoginTick)
	end
	
	if NeedFireAddExpTickTbl then
		for k, v in pairs(NeedFireAddExpTickTbl) do
			if v[1] then
				UnRegisterTick(v[1])
				NeedFireAddExpTickTbl[k] = nil
			end
		end
	end
		
	for k, v in pairs(g_CharLoginFuncFlow) do
		if v.m_LoginGameTimeTick then
			UnRegisterTick(v.m_LoginGameTimeTick)
			g_CharLoginFuncFlow[k] = nil
		end
	end

end

function AsyncServerCleanUp()
	if g_App:DoingQuit() then
		return
	end
	KickAllPlayer()
end

function EndService()
	ClearListenOfLocalAddr()
	ClearConnOfOtherServer()
	g_sleep:Wake() -- 唤醒Sleeper
	g_sleep = nil
	g_EProtocol:Shutdown()	
	g_EProtocol = nil
	g_WebServer:Shutdown()
	g_WebServer = nil
	--g_EProMsgRecv:Shutdown()
	--g_EProMsgRecv = nil
	
	local ConnMgr = g_App:GetConnMgr()
	ConnMgr:EndService()	
end

--等待异步EndService流程结束回调这个函数，执行后续操作
function OnServiceEnded()
	DestroyAllScene()
	UnRegisterCallDbTick()
	g_App:Quit()
end

function CAppGasHandler:OnBreak()
	print("OnBreak")
	
	db()
end

function CAppGasHandler:OnClose()
	print( "OnClose" )
	local filename = CBaseScriptApp_Inst():GetRootFilePath(nil) .. "/etc/gas/GasCommand.lua"
	dofile(filename)
end

function CloseGameServer(StopServerCountDown)
	local timeout = StopServerCountDown
	if timeout <= 0 then
		timeout = GasConfig.StopServerCountDown
	end
	local uCountOnClose = 0

	local function GetTime(time)
		local min,sec = 0,0
		if(time < 0) then
			return 0,0
		elseif(time < 60) then
			sec = time
		else
			min = math.floor(time/60)
			sec = time - min*60
		end
		return min,sec
	end
	
	local function EndServiceTick()	
		
		timeout = timeout -1
		
		if timeout < 0 then
			
			UnRegisterTick(g_EndServiceTick)
			g_EndServiceTick = nil
			
			if g_AppHandler.m_bDoingCleanUp then
				return
			end
			g_AppHandler.m_bDoingCleanUp = true	
			AsyncServerCleanUp()
			return
			
		end
		local min,sec = GetTime(timeout)
		if (min == 30 and sec == 0) or (min == 20 and sec == 0) or 
				(min == 10 and sec == 0) or (min == 1 and sec == 0) then
			MsgToConn(g_AllPlayerSender, 1103, min)
		elseif (min == 29 and sec >=30) or (min == 19 and sec >=30) or (min == 9 and sec >=30) then
			MsgToConn(g_AllPlayerSender, 1104, min,sec)
		elseif min == 0 then
			MsgToConn(g_AllPlayerSender, 1102, sec)
			g_App:Log("关服剩余" .. timeout .. "秒")
		elseif uCountOnClose < 30 then
			if min > 0 and sec > 0 then
				MsgToConn(g_AllPlayerSender, 1104, min,sec)
			elseif min > 0 then
				MsgToConn(g_AllPlayerSender, 1103, min)
			elseif sec > 0 then
				MsgToConn(g_AllPlayerSender, 1102, sec)
			end
		end
		uCountOnClose = uCountOnClose + 1
	end
	
	if g_EndServiceTick then
		UnRegisterTick(g_EndServiceTick)
		g_EndServiceTick = nil
	end

	g_App:Log("开始关服倒计时")
	g_EndServiceTick = RegisterTick("EndServiceTick", EndServiceTick, 1000)
end


function CAppGasHandler:OnFileChanged(data)
	
	--reload file
	local num = data:GetCount() 
	if data:GetIsLuaFile() then
		for i = 1, num do
			local name = data:GetFileName(i - 1)
			apcall(ReloadFile, name)
		end
	
		g_ScriptApp:ChangeChildVMFile(data)
	else
		for i = 1, num do
			local name = data:GetFileName(i - 1)
			ReloadSkillCfg(name)
		end
	end
end
