
g_bDBCallBackCheck = true

local OverTime = 60000

local tblDBCallbackCheckTick = {}

-- 加详细LOG帮助分析，类似自动执行GM指令ShowDBTranList，为防止重复大量不可控输出LOG加以下限制
local tblChannelDetailLog = {}		--已输出过详细LOG的channel
local DetailLogMax = 5				--最高输出详细LOG次数,暂定5次应该够分析了
local DetailLogCount = 0			--输出详细LOG次数计数

local CallbackTick = class(CTick)
CallbackTick._new = CallbackTick.new

function CallbackTick:OnTick()
	return self.m_TickFunc(self, unpack(self.m_TickArg))
end

local function ChannelTable2String(channels)
	local strChannels = ""
	if channels then
		local size = #channels
		for i = 1, size do
			if i == size then
				strChannels = strChannels..channels[i]
			else
				strChannels = strChannels..channels[i]..","
			end
		end
	end
	return strChannels
end

local function MakeBlockDetailLog(funcname, channels)
	DetailLogCount = DetailLogCount + 1
	if DetailLogCount > DetailLogMax then
		return false
	end
	if channels then
		local size = #channels
		for i = 1, size do
			if tblChannelDetailLog[channels[i]] then
				return false
			else
				tblChannelDetailLog[channels[i]] = true	
			end
		end
	end

	local strBlockDetailLog = ""
	--BlockWhere(BW), Time(T), Count(C), Channel(Ch), FuncName(F)
	local tblBlockChannel = GetBlockChannelList()
	for i=1, #tblBlockChannel do
		local strChannels = ChannelTable2String(tblBlockChannel[i][1])
		strBlockDetailLog = strBlockDetailLog .. "BW:VM, T:"..tblBlockChannel[i][3]..", Ch:"..strChannels..", F:"..tblBlockChannel[i][2] .. "\n"
	end
	
	local ChildVMCount = 0
	local TblWaitingLocalChannel = {}
	local TblWaitingConnection = {}
	local TblWaitingGlobalChannel = {}
	local TblExecutingSql = {}
	
	local function CallBack(WaitingLocalChannel, WaitingConnection, WaitingGlobalChannel, ExecutingSql)	
		for i = 1, #WaitingLocalChannel do
			table.insert(TblWaitingLocalChannel, WaitingLocalChannel[i])
		end
		for i = 1, #WaitingConnection do
			table.insert(TblWaitingConnection, WaitingConnection[i])
		end
		for i = 1, #WaitingGlobalChannel do
			table.insert(TblWaitingGlobalChannel, WaitingGlobalChannel[i])
		end
		for i = 1, #ExecutingSql do
			table.insert(TblExecutingSql, ExecutingSql[i])
		end	

		if ChildVMCount == g_ChildVMNum then
			for i = 1, #TblWaitingLocalChannel do
				local strChannels = ChannelTable2String(TblWaitingLocalChannel[i][3])
				strBlockDetailLog = strBlockDetailLog .. "BW:LocalChannel, T:"..TblWaitingLocalChannel[i][2]..", C:"..TblWaitingLocalChannel[i][4]..", Ch:"..strChannels..", F:"..TblWaitingLocalChannel[i][1] .. "\n"
			end
			for i = 1, #TblWaitingConnection do
				local strChannels = ChannelTable2String(TblWaitingConnection[i][3])
				strBlockDetailLog = strBlockDetailLog .. "BW:Connection, T:"..TblWaitingConnection[i][2]..", C:"..TblWaitingConnection[i][4]..", Ch:"..strChannels..", F:"..TblWaitingConnection[i][1] .. "\n"
			end		
			for i = 1, #TblWaitingGlobalChannel do
				local strChannels = ChannelTable2String(TblWaitingGlobalChannel[i][3])
				strBlockDetailLog = strBlockDetailLog .. "BW:GlobalChannel, T:"..TblWaitingGlobalChannel[i][2]..", C:"..TblWaitingGlobalChannel[i][4]..", Ch:"..strChannels..", F:"..TblWaitingGlobalChannel[i][1] .. "\n"
			end
			for i = 1, #TblExecutingSql do
				local strChannels = ChannelTable2String(TblExecutingSql[i][3])
				strBlockDetailLog = strBlockDetailLog .. "BW:ExecutingSql, T:"..TblExecutingSql[i][2]..", C:"..TblExecutingSql[i][4]..", Ch:"..strChannels..", F:"..TblExecutingSql[i][1] .. "\n"
			end
			
			local strChannels = ChannelTable2String(channels)
			local type = string.format("[严重错误] DBCallback 超时, 详细List")
			local msg = string.format("%s, %s", funcname, strChannels)
			strBlockDetailLog = msg .. "\n" .. strBlockDetailLog
			LogErr(type, strBlockDetailLog)
		else
			ChildVMCount = ChildVMCount + 1
		end
	end
	for i = 0, g_ChildVMNum do
		CallSpecialVMDbTrans(i, "GMDB", "GetDbTransInfo", CallBack, nil, {})
	end
	return true
end

local function DBCallbackOvertime(Tick, ...)
	local arg = {...}
	local funcname = arg[1]
	local channels = arg[2]
	local strBlockDetailLog = MakeBlockDetailLog(funcname, channels)
	if strBlockDetailLog == false then
		local strChannels = ChannelTable2String(channels)
		local type = string.format("[严重错误] DBCallback 超时")
		local msg = string.format("%s, %s", funcname, strChannels)
		LogErr(type, msg)
	end
	local tickIndex = tostring(Tick)
	UnRegisterDBCallbackCheckTick(tickIndex)
end

local g_queTickPool = CQueue:new()

local function GetTickFromPool()
	local tick
	if g_queTickPool:empty() then
		tick = CallbackTick:_new()	
	else
		tick = g_queTickPool:front()
		g_queTickPool:pop()
	end
	return tick
end

local function ReturnTickToPool(tick)
	g_queTickPool:push(tick)
end

function RegisterDBCallbackCheckTick( ... )
	local arg = {...}
	local tick = GetTickFromPool()
	tick.m_TickFunc = DBCallbackOvertime
	tick.m_TickArg = arg	
	g_App:RegisterTick(tick, OverTime)
	tick:SetTickName("DBCallbackCheck")
	local tickIndex = tostring(tick)
	tblDBCallbackCheckTick[tickIndex] = tick
--	print("+++ RegisterDBCallbackCheckTick:", tickIndex, arg[1])
	return tickIndex
end

function UnRegisterDBCallbackCheckTick( tickIndex )
	if tickIndex then
--		print("--- UnRegisterDBCallbackCheckTick:", tickIndex, funcname)
		local tick = tblDBCallbackCheckTick[tickIndex]
		if tick then		-- 注意：有可能回调时已经超时，在超时处理中已经删除
			tick.m_TickFunc = nil
			tick.m_TickArg = nil
			g_App:UnRegisterTick( tick )
			ReturnTickToPool( tick )
			tblDBCallbackCheckTick[tickIndex] = nil
		end
--	else
--		print("XXX UnRegisterDBCallbackCheckTick:", tickIndex, funcname)
	end
end
