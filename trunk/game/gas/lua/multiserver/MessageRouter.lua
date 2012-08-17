--可以跨服向客服端发消息,
--对于Gas2GacProList 里注册的rpc函数, 如果需要用到跨服发消息的去 Gas2GasProList 中注册
Gas2GacById = {}


--初始这个值为了避免单个服务器重启后Id归零出错(只要保证msgId是递增的就行)
--100年内 os.time() < 5.0e9  所以 os.time() * 10000 < 5.0e13  而lua在不超过1.0e14都能精确表示整数
--每秒钟的消息不会超过10000条,故理论上是不会有问题的 ^_^


--增加一个起始时间,使能表达的时间更远,支持的消息频率更高(增至10万/秒)
local StartDate = {year = 2010, month = 1, day =1}
local MsgSequenceId = (os.time() - os.time(StartDate)) * 100000
PlayerInServerCache = {}
PlayerMessageList = {}

local CharOnlineTbl = {}
local CharNameOnlineTbl = {}

local function InitPlayerLastMsgIdTbl(player, lastMsgIdStr)
	if lastMsgIdStr == nil or lastMsgIdStr == "" then
		local temp = {}
		for serverId in pairs(g_ServerList) do
			temp[serverId] = 0
		end
		player.m_LastCacheMsgId = temp 
	else
		player.m_LastCacheMsgId = loadstring("return {" .. lastMsgIdStr .. "}")()
	end
end

local function RemoveReceivedMsg(charMsgQueue, lastMsgId)
	--删除接收过的消息
	while charMsgQueue._begin do
		if charMsgQueue._begin._value[4] <=  lastMsgId then
			charMsgQueue:pop()
		else
			break
		end
	end
end



local function SendCurServerMsg(player)
	local charId = player.m_uID
	local charMsgQueue = PlayerMessageList[charId]
	if not charMsgQueue then
		return
	end
	local lastMsgIdTbl = player.m_LastCacheMsgId
	RemoveReceivedMsg(charMsgQueue, lastMsgIdTbl[g_CurServerId])
	
	local iter = charMsgQueue._begin
	while iter do
		local v = iter._value
		--print("本服自己取走消息 " ,v[4], unpack(v[2], 1, v[3]))
		Gas2Gac[v[1]](Gas2Gac, player.m_Conn, unpack(v[2], 1, v[3]))
		lastMsgIdTbl[g_CurServerId] = v[4]
		iter = iter._next
	end
	
	PlayerMessageList[charId] = nil
end

function PlayerLoginGetMessage(player, ChangeType, lastMsgIdStr)
	InitPlayerLastMsgIdTbl(player, lastMsgIdStr)
	local lastMsgIdTbl = player.m_LastCacheMsgId
	local charId = player.m_uID
	if ChangeType == 1 then --切场景登录,	去其他服务器拿消息
		--print"登录取消息----------------------------开始 "
		for serverId, otherConn in pairs(g_OtherServerConnList) do
			Gas2GasCall:GetCacheMsg(otherConn, g_CurServerId, charId, lastMsgIdTbl[serverId])
		end
		--处理自己服的消息
		SendCurServerMsg(player)

		--print"登录取消息----------------------------结束"
		--切场景只删本服消息
		Gas2GasAutoCall:ClearCacheMsg(GetServerAutoConn(g_CurServerId), charId)
	else
		for serverId in pairs(g_ServerList) do
			Gas2GasAutoCall:ClearCacheMsg(GetServerAutoConn(serverId), charId)
		end
	end
end

local function InsertMessage(charId, msgId, funName, ...)
	--print("InsertMessage", msgId)
	local charMsgQueue = PlayerMessageList[charId]
	if not charMsgQueue then
		charMsgQueue = CQueue:new()
		charMsgQueue.LastSendCount = 0
		PlayerMessageList[charId] = charMsgQueue
	end
	local now = os.time()
	local n = select("#", ...)
	charMsgQueue:push({funName,{...}, n, msgId, now})
	if charMsgQueue:size() - charMsgQueue.LastSendCount >= 2 and now - charMsgQueue:front()[5] < 10 then
		--print("不需要通知来取" , msgId)
		return false
	end
	return true
end

--处理接到的需发给客服端的消息()
function ReceiveServerToClientMsg(funName, Conn, charId, serverId, msgId, ...)
	local player = g_GetPlayerInfo(charId)
	if not (IsCppBound(player) and IsCppBound(player.m_Conn) ) or (player.m_Conn.m_ChangeToServer ~= nil)  then
		return
	end
	if msgId <= player.m_LastCacheMsgId[serverId] then
		return
	end
	--print("收到消息 " , msgId,  ...)
	Gas2Gac[funName](Gas2Gac, player.m_Conn, ...)
	player.m_LastCacheMsgId[serverId] = msgId
end

function Gas2GasDef:ClearCacheMsg(Conn, charId)
	--print("清空消息!!!!")
	PlayerMessageList[charId] = nil
end

function Gas2GasDef:GetCacheMsg(Conn, serverId, charId, lastMsgId)
	--print ("服务器 " .. serverId .. " 来取 " .. lastMsgId .. " 以上的消息")
	PlayerInServerCache[charId] = serverId --更新下玩家位置
	local charMsgQueue = PlayerMessageList[charId]
	if not charMsgQueue then
		return
	end
	
	RemoveReceivedMsg(charMsgQueue, lastMsgId)
	
	local iter = charMsgQueue._begin
	while iter do
		local v = iter._value
		--print("服务器 " .. serverId  .. "取走消息  " .. v[4])
		Gas2GasAutoCall[v[1]](Gas2GasAutoCall, Conn, charId, g_CurServerId, v[4], unpack(v[2], 1, v[3]))
		iter = iter._next
	end
	charMsgQueue.LastSendCount = charMsgQueue:size()
	--print("取走" ,charMsgQueue:size(), "条消息")
end


function Gas2GasDef:NotifyPlayerGetMsg(Conn, isSrcMsg, srcServerId, charId)
	--print ("Gas2GasDef:NotifyPlayerGetMsg ", srcServerId)
	local player = g_GetPlayerInfo(charId)
	if IsCppBound(player) and IsCppBound(player.m_Conn) then
		if isSrcMsg then
			--print("cache命中")
		end
		if player.m_Conn.m_ChangeToServer == nil then
			--print("本服角色存在,去服务器 " .. srcServerId .. " 取 " .. player.m_LastCacheMsgId[srcServerId] .. " 以上的消息")
			Gas2GasCall:GetCacheMsg(GetServerConn(srcServerId), g_CurServerId, charId, player.m_LastCacheMsgId[srcServerId])
		end
		return
	end
	if isSrcMsg then --原消息将转发给其他服
		--print("cache未命中, 转发其他服")
		for id, otherConn in pairs(g_OtherServerConnList) do
			if id ~= srcServerId then
				Gas2GasCall:NotifyPlayerGetMsg(otherConn, false, srcServerId, charId)
			end
		end
	end
end



function Gas2GacByIdAllFunDo(funName, charId, ...)

	if not IsNumber(charId) or charId == 0 then
		LogErr("调用Gas2GacById:" .. funName .. "   参数charId 不正确",  "charId = " .. tostring(charId))
		return
	end
	--print ("调用 Gas2GacById 发消息", funName)
	local player = g_GetPlayerInfo(charId)
	if IsCppBound(player) and IsCppBound(player.m_Conn) and player.m_Conn.m_ChangeToServer == nil then
		--print("角色在本服直接发", MsgSequenceId ," 消息:", ...)
		if PlayerMessageList[charId] then
			SendCurServerMsg(player)
			PlayerMessageList[charId] = nil
		end
		local ret, msg = apcall(Gas2Gac[funName],Gas2Gac, player.m_Conn, ...)
		if not ret then
			LogErr("调用Gas2GasById:" .. funName .. "  出错" .. tostring(msg))
		end
		return
	end 
	
	if not CharOnlineTbl[charId] then --离线角色不处理消息
		return
	end
	--角色不在 放到消息列表中,等待角色所在的服务器来取
	local mustNotify = InsertMessage(charId, MsgSequenceId, funName, ...)
	
	if mustNotify then
		--通知玩家过来取消息
		local cacheServerId = PlayerInServerCache[charId]
		if cacheServerId and cacheServerId ~= g_CurServerId then
			--print("角色不在本服 向服务器 " .. cacheServerId .. "发来取消息通知  msdId:", MsgSequenceId)
			Gas2GasCall:NotifyPlayerGetMsg(g_OtherServerConnList[cacheServerId], true, g_CurServerId, charId)
		else
			--print("角色不在本服    也没有角色所在服务器的cache, 直接广播  msdId:", MsgSequenceId)
			for id, otherConn in pairs(g_OtherServerConnList) do
				Gas2GasCall:NotifyPlayerGetMsg(otherConn, false, g_CurServerId, charId)
			end
		end
	end
	MsgSequenceId = MsgSequenceId + 1
end

local function AutoDefGas2GasFun(funName)
	local str = 
	[[
		function Gas2GasDef:%s(serverConn, charId, serverId, msgId, ...)
			--print("收到自动定义的消息")
			ReceiveServerToClientMsg("%s", serverConn, charId, serverId, msgId, ...)
		end 
	]]
	str = string.format(str, funName, funName)
	loadstring(str)()
end

local function AutoDefGas2GacByIdFun(funName)
	local str = 
	[[
		function Gas2GacById:%s(charId, ...)
			--print("收到自动定义的消息")
			Gas2GacByIdAllFunDo("%s", charId, ...)
		end 
	]]
	str = string.format(str, funName, funName)
	loadstring(str)()
end

--定义需要跨服向客服端发的Rpc函数 的服务器间通信函数
local function DefCrossServerToGacFun ()
	for funName in pairs(CrossServerToGacList) do
		AutoDefGas2GasFun(funName)
		AutoDefGas2GacByIdFun(funName)
	end
end


DefCrossServerToGacFun()



function Db2Gas:CharLogout(data)
	local charId = data[1]
	local charName = data[2]
	CharOnlineTbl[charId] = nil
	CharNameOnlineTbl[charName] = nil
	
	PlayerInServerCache[charId] = nil
	PlayerMessageList[charId] = nil
	ClearPlayerLastServerData(charId)
end

function Db2Gas:CharLogin(data)
	local charId = data[1]
	local charName = data[2]
	CharOnlineTbl[charId] = true
	CharNameOnlineTbl[charName] = true
end

local function ResetOnlineInfo(onlineInfo)
	CharOnlineTbl = {}
	CharNameOnlineTbl = {}
	for i = 0, onlineInfo:GetRowNum() -1 do
		CharOnlineTbl[onlineInfo:GetData(i,0)] = true
		CharNameOnlineTbl[onlineInfo:GetData(i,1)] = true
	end
end

function Db2Gas:UpdateOnlineInfo(data)
	local function CallBack(onlineInfo)
		ResetOnlineInfo(onlineInfo)
	end
	CallDbTrans("MultiServerDB", "GetOnlineInfo", CallBack, {})
end

function DbCallBack:SetOnlineInfo(onlineInfo)
	ResetOnlineInfo(onlineInfo)
end



--只能用于预判断.   处理重要逻辑要以数据库为准
function CheckPlayerIsOnline(charId)
	return CharOnlineTbl[charId]
end

function CheckPlayerIsOnlineByName(charName)
	return CharNameOnlineTbl[charName]
end