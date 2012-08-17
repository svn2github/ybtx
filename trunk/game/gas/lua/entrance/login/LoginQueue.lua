
g_LoginQueue = {}	--登录游戏的队列, 服务器承载人数有上线所以要排队
g_LoginQueue.Index = {}
g_LoginQueue.Size = 0

g_LoginQueueTick = nil
g_AutoLoginTick = nil

CheckUserQueue = CQueue:new()  --同时刻登录帐号处理,
CheckingUserTbl = {}
CheckingUserTbl.Size = 0


local CanLoginNum = 0

local uTotalCount = 0
local function SaveWaitQueueCount()
	if uTotalCount == g_LoginQueue.Size then
		return
	end 
	
	uTotalCount = g_LoginQueue.Size
	local data = {["uTotalCount"] = uTotalCount}
	CallDbTrans("LoginQueueDB","SaveWaitQueueCountDB", nil,data)
end

function g_LoginQueue.SetCanLoginNum(num)
	CanLoginNum = num
	--print("SetCanLoginNum", num)
	if CanLoginNum > 0 and g_LoginQueue.Size > 0 then
		if not g_AutoLoginTick then
			g_AutoLoginTick = RegisterTick("LoginQueueTick",  g_LoginQueue.UserAutoLoginGame, 500)
		end
	else
		if g_AutoLoginTick then
			UnRegisterTick(g_AutoLoginTick)
			g_AutoLoginTick = nil
		end
	end
	 
end

local function GetLoginNum()
	CallDbTrans("LoginQueueDB", "GetServerCanLoginNum", g_LoginQueue.SetCanLoginNum, {})	
	SaveWaitQueueCount()
end

function OnLoginQueueChange()
	--print("OnLoginQueueChange", g_LoginQueue.Size)
	if g_LoginQueue.Size > 0 then
		if not g_LoginQueueTick then
			g_LoginQueueTick = RegisterTick("LoginQueueTick",  GetLoginNum, 5000)
		end
	else
		if g_LoginQueueTick then
			UnRegisterTick(g_LoginQueueTick)
			g_LoginQueueTick = nil
		end
		SaveWaitQueueCount()
	end
	
end

function g_LoginQueue.UserAutoLoginGame()
	--print("UserAutoLoginGame", CanLoginNum)
	if g_LoginQueue.Size <= 0 then
		g_LoginQueue.SetCanLoginNum(0)
		return	
	end
	for i, v in pairs(g_LoginQueue.Index) do
		for m,n in pairs(v) do
			if(n.SequenceId == 1) then
				apcall(AuthSuccess, unpack(n.Data))
				g_LoginQueue.Index[i][m] = nil
				g_LoginQueue.Size = g_LoginQueue.Size -1
			else
				n.SequenceId = n.SequenceId - 1
				Gas2Gac:ReturnUpdateWaitSequence(n.Data[2], n.SequenceId)
			end
		end
	end
	g_LoginQueue.SetCanLoginNum(CanLoginNum - 1)
	OnLoginQueueChange()
end

--@brief 取消登录排队通知其他服正在排队的玩家
function g_LoginQueue.CanceLoginGame(Connection)
	local userName = Connection.m_UserName
	local uGameId = Connection.m_nGameID
	if not g_LoginQueue.Index[uGameId] then
		return
	end
	local node = g_LoginQueue.Index[uGameId][userName]
	if not (node and node.Data[2] == Connection) then
		return false
	end
	local Conn = node.Data[2]
	if IsCppBound(Conn) then
		Gas2Gac:ReturnDelWaitingUser(Conn)
	end
	
	local cancelSequenceId = node.SequenceId
	g_LoginQueue.Index[uGameId][userName] = nil

	g_LoginQueue.Size = g_LoginQueue.Size -1
	
	for i, v in pairs(g_LoginQueue.Index) do
		for m,n in pairs(v) do
			if(n.SequenceId > cancelSequenceId) then
				n.SequenceId = n.SequenceId -1
				Gas2Gac:ReturnUpdateWaitSequence(n.Data[2], n.SequenceId)
			end
		end
	end
	OnLoginQueueChange()
	return true
end

function g_LoginQueue.QueueUp(userName, loginInfo,uGameId)
	if not g_LoginQueue.Index[uGameId] then
		g_LoginQueue.Index[uGameId] = {}
	end
	assert(g_LoginQueue.Index[uGameId][userName] == nil)
	g_LoginQueue.Size = g_LoginQueue.Size + 1
	local node = {}
	node.Data = loginInfo
	node.SequenceId = g_LoginQueue.Size
	g_LoginQueue.Index[uGameId][userName] = node
	Gas2Gac:ReturnLoginWaitingQueue(loginInfo[2],node.SequenceId)
	OnLoginQueueChange()
end

function g_LoginQueue.RepeatQueueUp(userName, newLoginInfo,uGameId)
	if not g_LoginQueue.Index[uGameId] then
		return false
	end
	local oldNode = g_LoginQueue.Index[uGameId][userName]
	if oldNode == nil then
		return false
	end
	local oldConn = oldNode.Data[2]
	if IsCppBound(oldConn) then
			oldConn.m_LogoutSetting = "Immediately"
			oldConn:OldShutDown("排队过程中被别人强行顶了")
	end

	oldNode.Data = newLoginInfo
	Gas2Gac:ReturnLoginWaitingQueue(newLoginInfo[2],oldNode.SequenceId)
	return true
end

function CheckUserEntry(conn, callFunc)
-- 	print("CheckUserEntry", conn)
	CheckUserQueue:push({conn, callFunc})
	TurnCheckUser()
end


function CheckUserOver(conn)
	if CheckingUserTbl[conn] then
		--print("CheckUserOver", conn)
		CheckingUserTbl[conn] = nil
		CheckingUserTbl.Size = CheckingUserTbl.Size -1
		TurnCheckUser()
	end
end

local function PassOne()
	while not CheckUserQueue:empty() do
		local node = CheckUserQueue:front()
		CheckUserQueue:pop()
		if IsCppBound(node[1]) and not node[1].m_Disconnect then
			apcall(node[2])
			if CheckingUserTbl[node[1]] then
				LogErr("CheckingUserTbl 中出现重复 值")
			else
				CheckingUserTbl[node[1]] = true
				CheckingUserTbl.Size = CheckingUserTbl.Size + 1
			end
			--print("成功通过链接:", node[1])
			return true
		end
		--print("失效的链接", node[1])
	end
end

function TurnCheckUser()
	--print("当前登录中帐号个数:", CheckingUserTbl.Size, "最大同时登录帐号数:", GasConfig.MaxSynLoginNum)
	while CheckingUserTbl.Size < GasConfig.MaxSynLoginNum do
		if not PassOne() then
			break
		end
	end
	--print("放行完毕==================")
	local sequenceId = 1
	local iter = CheckUserQueue._begin
	local node = nil
	while iter do
		node = iter._value
		Gas2Gac:RetLoginCheckUserQueue(node[1], sequenceId)
		sequenceId  = sequenceId + 1
		iter = iter._next
	end
end

