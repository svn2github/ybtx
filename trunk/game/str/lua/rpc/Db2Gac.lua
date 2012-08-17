local AllMsgTbl = {}  --以 coroutine 为索引

--local MsgList = {}  --使用lastId 索引的 隐式链表 (中间过程很可能是一个断的链表)
--local MsgBuffer = {} --缓冲同id 的msg, 遇到Db2GacEnd 表示完成 然后再插入 MsgList 中
--local WaitingIdTbl = {} --记录上次执行完的id


local Db2GacTimeOutTick = class(CTick)

Db2GacTimeOutTick._new = Db2GacTimeOutTick.new

function Db2GacTimeOutTick:new(co)
	local tick = Db2GacTimeOutTick:_new()
	tick.m_Co = co
	return tick
end

function Db2GacTimeOutTick:OnTick()
	g_App:UnRegisterTick(self)
	local case_data = RunningCaseTbl[self.m_Co]
	print(case_data.m_UserName .. "Db2Gac消息超时, 可能是服务器断开链接, 或超级卡,")
	db()
	ClearDb2GacTimeOutTick(self.m_Co)
end

function TestSleep(time)
	local tick = Db2GacTimeOutTick:new()
	g_App:RegisterTick(tick, time)
	WaitForMsg("SleepTimedOutMsg")
end


local function GetMsgList(co)
	local tbl = AllMsgTbl[co]
	if not tbl then
		tbl = {}
		tbl.MsgList = {}
		tbl.MsgBuffer = {}
		tbl.WaitingIdTbl = {}
		AllMsgTbl[co] = tbl
	end
	return tbl.MsgList
end

local function GetMsgBuffer(co)
	local tbl = AllMsgTbl[co]
	if not tbl then
		tbl = {}
		tbl.MsgList = {}
		tbl.MsgBuffer = {}
		tbl.WaitingIdTbl = {}
		AllMsgTbl[co] = tbl
	end
	return tbl.MsgBuffer
end

local function GetWaitingIdTbl(co)
	local tbl = AllMsgTbl[co]
	if not tbl then
		tbl = {}
		tbl.MsgList = {}
		tbl.MsgBuffer = {}
		tbl.WaitingIdTbl = {}
		AllMsgTbl[co] = tbl
	end
	return tbl.WaitingIdTbl
end


local function CheckReady(co, typeName)
	local WaitingIdTbl = GetWaitingIdTbl(co)
	local MsgList = GetMsgList(co)
	local waitId = WaitingIdTbl[typeName]  or 0
	local node = MsgList[typeName][waitId]
	if not node then
		return
	end
	
	for _, v in ipairs(node.MsgTbl) do
		local funName = v[1]
		local arg = v[2]
		--print("收到消息",funName,unpack(arg, 1, arg.n))
		local case_data = RunningCaseTbl[co]
		if Gas2Gac[funName] then
			Gas2Gac[funName](Gas2Gac, case_data.m_Conn, unpack(arg, 1, arg.n))
		end
	end
	if node.Tick then
		UnRegisterTick(node.Tick)
		node.Tick = nil
	end
	WaitingIdTbl[typeName] = waitId + 1
	MsgList[typeName][waitId] = nil
	CheckReady(co, typeName)
end

local function InsertMsg(co, typeName, msgId, node)
	local MsgList = GetMsgList(co)
	local tbl = MsgList[typeName]
	if not tbl then
		tbl = {}
		MsgList[typeName] = tbl
	end
	assert(tbl[msgId] == nil, "重复的前续消息id")
	tbl[msgId] = node
	

	node.Tick = Db2GacTimeOutTick:new(co)
	g_App:RegisterTick(node.Tick, 60000)
	
	CheckReady(co, typeName)
end


function TransactDb2Gac(conn, funName, typeName, msgId, isEnd,...)
	local arg = {...}
	arg.n = select("#", ...)
	local co = conn.m_Co
	local MsgBuffer = GetMsgBuffer(co)
	if not MsgBuffer[typeName] then
		MsgBuffer[typeName] = {}
	end
	local tbl = MsgBuffer[typeName][msgId]
	if not tbl then
		tbl = {}
		tbl.MsgTbl = {}
		MsgBuffer[typeName][msgId] = tbl
	end
	table.insert(tbl.MsgTbl, {funName, arg})
	
	if isEnd then
		InsertMsg(co, typeName, msgId, tbl)
		MsgBuffer[typeName][msgId] = nil
	end
end


local function AutoDefDb2GacFunction(funName)
	local str = [[
		function Gas2Gac:_Db2%s(conn, ...)
			TransactDb2Gac(conn, "%s", ...)
		end
	]]
	str = string.format(str, funName, funName)
	loadstring(str)()
end


--自动定义Db2Gac的包装函数
AliasDoFile("gac_gas", "framework/rpc/Gas2GacProList")

for funName in pairs(Db2GacFunList) do
	AutoDefDb2GacFunction(funName)
end


function ClearDb2GacTimeOutTick(co)
	local MsgList = GetMsgList(co)
	for _, list in pairs(MsgList) do
		for _, node in pairs(list) do
			if node.Tick then
				g_App:UnRegisterTick(node.Tick)
				node.Tick = nil
			end
		end
	end
end

function ResetDb2GacData(co)
	ClearDb2GacTimeOutTick(co)

	AllMsgTbl[co] = nil
end


function Gas2Gac:ClearDb2GacMsg(conn)
	ResetDb2GacData(conn.m_Co)
end
