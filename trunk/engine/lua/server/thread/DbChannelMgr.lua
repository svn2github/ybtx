engine_require "common/Queue/Queue"
engine_require "server/thread/DbChannelMgrInc"
engine_require "server/thread/StmtObj"
engine_require "server/thread/DbResultSet"

local g_bOpenSqlProfiler  = false
if GasConfig.StatisticDBCalls == 1 then
	g_bOpenSqlProfiler = true
end
local g_tblSqlProfilerData = {}
local g_uSqlProfileAllTime = 0

local g_DbContextQueue = CQueue:new()
CDbContext.__new = CDbContext.new

local function GetDbContext(CurCoInfo)
	local DbContext
	if g_DbContextQueue:empty() then
		DbContext = CDbContext:__new()
	else
		DbContext = g_DbContextQueue:front()
		g_DbContextQueue:pop()
	end
	DbContext.m_Co = CurCoInfo.m_Co
	
	return DbContext
end

local function ReturnDbContext(DbContext)
	DbContext.m_Co = nil
	DbContext:Clear()
	g_DbContextQueue:push(DbContext)
end

-- 给DbThreadsMgr的回调
function CDbChannelMgr:OnSomeQueryJobDone()

	while true do
	
		local DbContext = self._m_ThreadsMgr:GetQueryResult()
		
		if DbContext == nil then
			return
		end

		local error = DbContext:GetReport()
		local report = error:ToString()

		local ResultSetValue = DbContext:GetValueTable()
		
		local co= DbContext.m_Co
		
		assert( self._m_ThreadsMgr:PopQueryResult() )
		
		ReturnDbContext(DbContext)
		coroutine.resume(co, report, ResultSetValue)
	end
	
end

function CDbChannelMgr:_GetChannelQueue(sChannelID)
	if nil == self._m_AllChannels[sChannelID] then
		self._m_AllChannels[sChannelID] = CQueue:new()
	end
	return self._m_AllChannels[sChannelID]
end


function CDbChannelMgr:_RequestLocalChannel(CurCoInfo, ...)
--考虑query Coroutine等待两个相同的channel
	local arg = {...}
	local Channels={}
	local WaitingChannels={}
	local funcname = CurCoInfo.m_sFuncname
	local blockChannels = {}
	
	CurCoInfo.m_Channels				=	Channels
	CurCoInfo.m_WaitingChannels			=	WaitingChannels

	if nil ~= arg[1] then
		local tbl_single = GetTblFromPool()
		for i,v in ipairs(arg) do
			if tbl_single[v] ~= 1 then
				Channels[i] = v
			end
			tbl_single[v] = 1
		end
		ReturnTblToPool(tbl_single)
		
		for i,v in pairs(Channels) do
			local queChannel = self:_GetChannelQueue(v)
			queChannel:push(CurCoInfo)

			if 1 ~= queChannel:size() then
				WaitingChannels[v]=true

				if not self._m_WaitingLocalChannel then
					self._m_WaitingLocalChannel = {}
				end		
				if not self._m_WaitingLocalChannel[funcname] then
					self._m_WaitingLocalChannel[funcname] = {}
				end
				if not self._m_WaitingLocalChannel[funcname][v] then
					self._m_WaitingLocalChannel[funcname][v] = {}
					self._m_WaitingLocalChannel[funcname][v].m_RequestTime = GetProcessTime()
					self._m_WaitingLocalChannel[funcname][v].m_Channels = Channels
					self._m_WaitingLocalChannel[funcname][v].m_Count = 0
				end
				blockChannels[v] = true
				self._m_WaitingLocalChannel[funcname][v].m_Count = self._m_WaitingLocalChannel[funcname][v].m_Count + 1
			end
		end
	end

	if next(WaitingChannels) ~= nil then
		coroutine.yield()
	end
	
	if self._m_WaitingLocalChannel then
		if self._m_WaitingLocalChannel[funcname] then
			for v, _ in pairs(blockChannels) do
				self._m_WaitingLocalChannel[funcname][v].m_Count = self._m_WaitingLocalChannel[funcname][v].m_Count - 1
				if self._m_WaitingLocalChannel[funcname][v].m_Count == 0 then
					self._m_WaitingLocalChannel[funcname][v] = nil
					
					if next(self._m_WaitingLocalChannel[funcname]) == nil then
						self._m_WaitingLocalChannel[funcname] = nil
					end
				end
			end
		end
	end
end

function CDbChannelMgr:_ReleaseLocalChannel(CurCoInfo)

	local Channels = CurCoInfo.m_Channels
	local OcChannels = Channels
		
	--将该coroutine占用的channel全部从队列中清除
	for i,v in pairs(Channels) do
		local queChannel = self:_GetChannelQueue(v)
		queChannel:pop()
	end
	
	--本Coroutine不再占用任何channels
	Channels={}
	
	for i,v in pairs(OcChannels) do
		local queChannel = self:_GetChannelQueue(v)
		if queChannel:empty() then
			--该频道的等待队列为空，说明没有任何coroutine在等待之，因此把该channel彻底删除
			self._m_AllChannels[v] = nil
			--break
		else
			local WaitChannelCoInfo = queChannel:front()
			local co = WaitChannelCoInfo.m_Co
			local WaitingChannels = WaitChannelCoInfo.m_WaitingChannels
		
			WaitingChannels[v] = nil
			if next(WaitingChannels) == nil	then
				--该coroutine的等待频道集合为空，说明该coroutine已经获得了所有他需要的频道，应该让他继续执行
				coroutine.resume( co )
			end	
		end		
	end
end

-- 获得空闲的Connection
function CDbChannelMgr:_RequestConnection(CurCoInfo)
	local funcname = CurCoInfo.m_sFuncname
	local channel = CurCoInfo.m_Channels[1]
	if channel == nil then
		channel = 0
	end
	if self._m_queIdleConn:empty() then
		self._m_queWaitingConnCo:push(CurCoInfo)
		
		if not self._m_WaitingConnection then
			self._m_WaitingConnection = {}
		end		
		if not self._m_WaitingConnection[funcname] then
			self._m_WaitingConnection[funcname] = {}
		end
		if not self._m_WaitingConnection[funcname][channel] then
			self._m_WaitingConnection[funcname][channel] = {}
			self._m_WaitingConnection[funcname][channel].m_RequestTime = GetProcessTime()
			self._m_WaitingConnection[funcname][channel].m_Channels = CurCoInfo.m_Channels
			self._m_WaitingConnection[funcname][channel].m_Count = 0
		end
		self._m_WaitingConnection[funcname][channel].m_Count = self._m_WaitingConnection[funcname][channel].m_Count + 1
			
		coroutine.yield()
	end
	
	if self._m_WaitingConnection then
		if self._m_WaitingConnection[funcname] then
			self._m_WaitingConnection[funcname][channel].m_Count = self._m_WaitingConnection[funcname][channel].m_Count - 1
			if self._m_WaitingConnection[funcname][channel].m_Count == 0 then
				self._m_WaitingConnection[funcname][channel] = nil
				
				if next(self._m_WaitingConnection[funcname]) == nil then
					self._m_WaitingConnection[funcname] = nil
				end
			end
		end
	end
	
	local Conn = self._m_queIdleConn:front()
	assert(Conn)
	self._m_queIdleConn:pop()
	CurCoInfo.m_Conn = Conn
end

-- 释放已经使用完毕的Connection
function CDbChannelMgr:_ReleaseConnection(CurCoInfo)

	local Conn = CurCoInfo.m_Conn
	
	CurCoInfo.m_Conn=nil
	
	self._m_queIdleConn:push(Conn)
	
	if self._m_queIdleConn:size() > self._m_DbConnNum then
	   error("执行db的coroutine的数量大于线程的数量了")
	end

	if self._m_queWaitingConnCo:empty() then
		return
	end
	
	local WaitingConnCoInfo = self._m_queWaitingConnCo:front()	
	self._m_queWaitingConnCo:pop()
	
	coroutine.resume( WaitingConnCoInfo.m_Co )
end

local _GetResultSize = CValueTable.GetBufferSize
local _GetRowNum = CValueTable.GetRowNum
local _GetColNum = CValueTable.GetColNum
local _s_format = string.format
local _t_concat = table.concat

function CDbChannelMgr:CheckStmtResultSize(ResultSet, sStmtName, sStmtSql, ...)
	local MaxSize = self._m_MaxDBResultSize
	local ResultSize = _GetResultSize(ResultSet)
	if ResultSize > MaxSize then
		local nRow = _GetRowNum(ResultSet)
		local nCol = _GetColNum(ResultSet)
		local arg = {...}
		local nNum = select("#", ...)
		local err_type = _s_format("StatExec查询的结果集过大，大于%d，StmtName: %s", MaxSize, sStmtName)
		local err_msg 
		if next(arg) then
			for i = 1, nNum do
				if arg[i] == nil then
					arg[i] = "nil"
				end
			end
			err_msg = _s_format("StmtSql: %s\nrow: %d, col: %d，实际大小: %d, 传入参数: %s", sStmtSql, nRow, nCol, ResultSize, _t_concat(arg, "\t"))
		else
			err_msg = _s_format("StmtSql: %s\nrow: %d, col: %d，实际大小: %d", sStmtSql, nRow, nCol, ResultSize)
		end
		
		LogErr(err_type, err_msg)
	end
end

-- 执行Statement
function CDbChannelMgr:StatementExecute(uStatIndex, ...)
	local StmtObj = self._m_ayStatement[uStatIndex]
	local sStmtName = StmtObj._m_StmtName
	if g_bOpenSqlProfiler then
		if g_tblSqlProfilerData[sStmtName] == nil then
			g_tblSqlProfilerData[sStmtName] = {}
			g_tblSqlProfilerData[sStmtName].m_uCount = 0
			g_tblSqlProfilerData[sStmtName].m_uAllTime = 0
			g_tblSqlProfilerData[sStmtName].m_uLastExecTime = 0
		end
		
		g_tblSqlProfilerData[sStmtName].m_uLastExecTime = GetProcessTime()
	end

	local arg = {...}

	local CurCoInfo = self._m_AllCoInfo[coroutine.running()]
	
	if CurCoInfo == nil then
		error ("Statement must being executed in a query coroutine.")
	end
	
	local pStat = CurCoInfo.m_Conn.m_ayStat[uStatIndex]
	
	local Context = GetDbContext(CurCoInfo)

	if #arg > pStat:GetParamNum() then
		error("传入的参数个数大于实际需要的参数个数")
	end

	self._m_ThreadsMgr:AddQueryStatementJob(pStat, Context, ...)
		
	local vReport,vResultSet
	vReport, vResultSet = coroutine.yield()
	
	if g_bOpenSqlProfiler and g_tblSqlProfilerData[sStmtName] then
		g_tblSqlProfilerData[sStmtName].m_uCount = g_tblSqlProfilerData[sStmtName].m_uCount + 1
		local uPassTime = GetProcessTime() - g_tblSqlProfilerData[sStmtName].m_uLastExecTime
		g_tblSqlProfilerData[sStmtName].m_uAllTime = g_tblSqlProfilerData[sStmtName].m_uAllTime + uPassTime
		g_uSqlProfileAllTime = g_uSqlProfileAllTime + uPassTime
	end
	
	if string.len(vReport)~= 0 then
		if vReport == "MySQL server has gone away" then
			ShowErrorMsgAndExit("MySQL server has gone away,process exit.\n")
		elseif vReport == "Commands out of sync; you can't run this command now" then
			ShowErrorMsgAndExit("Commands out of sync; you can't run this command now, process exit.\n")
		end
		error (vReport)
	end
	
	if vResultSet ~= nil then
		local sStmtSql = StmtObj._m_StmtSql
		self:CheckStmtResultSize(vResultSet, sStmtName, sStmtSql, ...)	
		
		local ResultSet = CDbResultSet:new()
		ResultSet:SetValueTable(vResultSet)
		return vReport, ResultSet
	end
	
	return vReport, nil 
end


-- 批量执行Statement，因为是批量执行，所以无法返回结果
function CDbChannelMgr:BatchStatementExec(uStatIndex, uExecTime, ...)
	local CurCoInfo = self._m_AllCoInfo[coroutine.running()]
	
	if CurCoInfo == nil then
		error ("Procedure must being executed in a query coroutine.")
	end
	
	local pStat = CurCoInfo.m_Conn.m_ayStat[uStatIndex]
	
	local Context = GetDbContext(CurCoInfo)

	self._m_ThreadsMgr:AddQueryBatchStatementJob( pStat, Context, uExecTime, ...)
		
	local vReport, vResultSet
	vReport, vResultSet = coroutine.yield()
	
	assert( vResultSet == nil )
	
	return vReport
end

local _StatementProcedure = CDbChannelMgr.StatementProcedure



function CDbChannelMgr:CheckTextResultSize(ResultSet, sText)
	local MaxSize = self._m_MaxDBResultSize
	local ResultSize = _GetResultSize(ResultSet)
	if ResultSize > MaxSize then
		local nRow = _GetRowNum(ResultSet)
		local nCol = _GetColNum(ResultSet)
		local err_type = _s_format("TextExec查询的结果集过大，大于%d，查询语句: %s", MaxSize, sText)
		local err_msg = _s_format("row: %d, col: %d，实际大小: %d", nRow, nCol, ResultSize)
	
		LogErr(err_type, err_msg)
	end
end

function CDbChannelMgr:TextExecute(sSql)

	if g_bOpenSqlProfiler then
		if g_tblSqlProfilerData[sSql] == nil then
			g_tblSqlProfilerData[sSql] = {}
			g_tblSqlProfilerData[sSql].m_uCount = 0
			g_tblSqlProfilerData[sSql].m_uAllTime = 0
			g_tblSqlProfilerData[sSql].m_uLastExecTime = 0
		end
		
		g_tblSqlProfilerData[sSql].m_uLastExecTime = GetProcessTime()
	end
	
	local CurCoInfo = self:GetCurCoInfo()
	
	local Context = GetDbContext(CurCoInfo)
	
	self._m_ThreadsMgr:AddQueryTextJob(CurCoInfo.m_Conn.m_DbConn,sSql,string.len(sSql),Context)
	
	local vReport = {} vTextResult = {}
	vReport, vTextResult = coroutine.yield()
	
	if g_bOpenSqlProfiler and g_tblSqlProfilerData[sSql] then
		g_tblSqlProfilerData[sSql].m_uCount = g_tblSqlProfilerData[sSql].m_uCount + 1
		local uPassTime = GetProcessTime() - g_tblSqlProfilerData[sSql].m_uLastExecTime
		g_tblSqlProfilerData[sSql].m_uAllTime = g_tblSqlProfilerData[sSql].m_uAllTime + uPassTime
		g_uSqlProfileAllTime = g_uSqlProfileAllTime + uPassTime
	end
	
	if string.len(vReport) ~= 0 then
		error (vReport)
	end
	
	if vTextResult ~= nil then
		self:CheckTextResultSize(vTextResult, sSql)	
		local ResultSet = CDbResultSet:new()
		ResultSet:SetValueTable(vTextResult)
		return vReport, ResultSet
	end
	return vReport, nil
end
local sStartTranSql		= "start transaction"
local sEndTranSql			= "commit"
local sCancelTranSql	= "rollback"

function CDbChannelMgr:StartTran()
	--local CurCoInfo = self:GetCurCoInfo()
	--self:TextExecute(sStartTranSql,"")
end

--[[
	commit和cancel还是需要等待的，不能只发job就不管了
	不然可能会造成在一个connection上面执行了，但是未commit之前，
	在另一个connection上面执行同样的操作了，可能会造成问题
--]]

function CDbChannelMgr:EndTran()
	local CurCoInfo = self._m_AllCoInfo[coroutine.running()]
	local Context = GetDbContext(CurCoInfo)
	self._m_ThreadsMgr:AddCommitTransJob(CurCoInfo.m_Conn.m_DbConn, Context)
	coroutine.yield()
end


function CDbChannelMgr:CancelTran()
	local CurCoInfo = self._m_AllCoInfo[coroutine.running()]
	local Context = GetDbContext(CurCoInfo)
	self._m_ThreadsMgr:AddRollbackTransJob(CurCoInfo.m_Conn.m_DbConn, Context)
	coroutine.yield()
end

function CDbChannelMgr:SetSavepoint()
	local info = debug.getinfo(2)
	local len = string.len(info.short_src)
	local tmp = string.reverse(info.short_src)
	local pos = string.find(tmp, "/")
	local last_find_index = len - pos + 1
	local name = string.sub(info.short_src, last_find_index + 1, len - 4) .. info.currentline
	local CurCoInfo = self._m_AllCoInfo[coroutine.running()]
	local Context = GetDbContext(CurCoInfo)
	self._m_ThreadsMgr:AddSetSavepointJob(CurCoInfo.m_Conn.m_DbConn, Context, name)
	coroutine.yield()
	return name
end

function CDbChannelMgr:RollbackTransTo(name)
	local CurCoInfo = self._m_AllCoInfo[coroutine.running()]
	local Context = GetDbContext(CurCoInfo)
	self._m_ThreadsMgr:AddRollbackTransToJob(CurCoInfo.m_Conn.m_DbConn, Context, name)
	coroutine.yield()
end

function CDbChannelMgr:_InitCoData()
	local co=coroutine.running()
	local CoInfo={m_Co=co}			--刚创建的，m_Conn、m_Channls和m_WaitingChannels都为空
	self._m_AllCoInfo[co]=CoInfo
end

function CDbChannelMgr:_UnitCoData()
	self._m_AllCoInfo[coroutine.running()]=nil
end

function CDbChannelMgr:GetCurCoInfo()
	return self._m_AllCoInfo[coroutine.running()]
end

function CDbChannelMgr:_IncActiveCoNum(CurCoInfo)
	self._m_uActiveCoNum = self._m_uActiveCoNum + 1
end

function CDbChannelMgr:_DecActiveCoNum(CurCoInfo)

	self._m_uActiveCoNum = self._m_uActiveCoNum - 1
	assert(self._m_uActiveCoNum >= 0)
end

-- 传入的函数，以及频道
function CDbChannelMgr:CreateQueryCoroutine(func, funcname, ...)
	assert(IsFunction(func))
	local arg = {...}
	function exec()		
		local co = coroutine.running()
	
		--if g_ThreadApp:ReloadFileBegan() then
		--	g_ThreadApp:BlockDbQuery(co)		
		--end
	
		self._m_QueryCoTable[co] = true
		self:_InitCoData()

		local CurCoInfo =  self._m_AllCoInfo[co]
		CurCoInfo.m_sFuncname = funcname
		self:_RequestLocalChannel(CurCoInfo, unpack(arg))
		self:_RequestConnection(CurCoInfo)
				
		self:_IncActiveCoNum(CurCoInfo)

		--错误处理都在func里面进行了
		func()
				
		self:_DecActiveCoNum(CurCoInfo)
		
		self:_ReleaseConnection(CurCoInfo)
		self:_ReleaseLocalChannel(CurCoInfo)
		
		self:_UnitCoData()
		
		self._m_QueryCoTable[co] = nil
		
		--如果有元素，表明还存在未执行的coroutine	
		
		--if g_ThreadApp:ReloadFileBegan() then
		--	if not self:CoroutineExist() then
		--		g_ThreadApp:ResumeReloadFile()
		--	end
		--end
		
		if not g_ThreadApp.m_bQuit then
			return			
		end
		
		if self:CoroutineExist() then
			print( "剩余" .. #(self._m_QueryCoTable) .. "个QueryCoroutine" )
			return
		end
		
		g_ThreadApp:IntQuit()
		
	end
	
	local co = coroutine.running()
	if co and self._m_QueryCoTable[co] then
		error([[Is Already in a query coroutine,
			中文解释：QueryCoroutine只能由主循环创建，
				不能出现QueryCoroutine里面创建QueryCoroutine的情况
			]])
	end
		
	coroutine.resume(coroutine.create(exec, 0))
end


function CDbChannelMgr:CoroutineExist()
	return next( self._m_QueryCoTable ) ~= nil
end


function CDbChannelMgr:LastInsertId()
	local CurCoInfo = self._m_AllCoInfo[coroutine.running()]
	
	if nil == CurCoInfo then
		return
	end
		
	if nil == CurCoInfo.m_Conn then
		return
	end
	
	return CurCoInfo.m_Conn.m_DbConn:LastInsertId()
end

function CDbChannelMgr:LastAffectedRowNum()
	local CurCoInfo = self._m_AllCoInfo[coroutine.running()]
	
	if nil == CurCoInfo then
		return
	end
		
	if nil == CurCoInfo.m_Conn then
		return
	end
	
	return CurCoInfo.m_Conn.m_DbConn:LastAffectedRowNum()
end
	
	
--------------------------------------------------------------------	

local stat_mt = {}
local stat_index = {}

stat_mt.__index = function(tbl, index)
	local sText = stat_index[index]
	local Conn = rawget(tbl, "Conn")
	local uLength = string.len(sText)
	
	local Stat = Conn:CreateStatementWithSize(sText,uLength)
	if Stat == nil then
		ShowErrorMsgAndExit("CreateStatementWithSize failed,process exit.\n")
	end
	
	rawset(tbl, index, Stat)
	
	return Stat
end

local tbl_maxn = table.maxn
local function _GetIdleStatIndex(sStmtSql)
	assert(sStmtSql)
	local num = tbl_maxn(stat_index) + 1
	
	for i = 1, num do
		if stat_index[i] == nil then
			stat_index[i] = sStmtSql
			return i
		end 
	end
end

function CDbChannelMgr:_ReleaseStatement(uIndex)
	if self._m_queIdleConn:size() ~= self._m_DbConnNum then
		error("删除statement的时候必须没有任何trans在执行")
	end
	
	local iter = self._m_queIdleConn._begin
	local MAX
	
	if iter == nil then
		return
	end
	 
	while iter do
		MAX = tbl_maxn(iter._value.m_ayStat)
	
		if uIndex >= 1 and uIndex <= MAX then
			local ay = rawget(iter._value.m_ayStat, uIndex)	
			if ay then
				ay:Release()
				rawset(iter._value.m_ayStat, uIndex, nil)
			end			
		end
			
		iter = iter._next
	end
	
	stat_index[uIndex] = nil
end


-- 创建ConnectionMgr
function CDbChannelMgr:_CreateConnectionMgr(uNum, sHost, uPort, sUser, sPassword, sDatabase)
	self._m_DbConnMgr = g_ThreadApp:GetDbConnectionMgrByHost(uNum, sHost, uPort, sUser, sPassword, sDatabase)
	for i=1, uNum do
		local Conn = { m_DbConn = self._m_DbConnMgr:GetConnection(i-1), m_ayStat = {}}
		setmetatable(Conn.m_ayStat, stat_mt)
		
		rawset(Conn.m_ayStat, "Conn", Conn.m_DbConn)
		Conn.m_DbConn:SetIsolationLevel(Isolation_Level.READ_COMMITTED)
		Conn.m_DbConn:SetAutoCommit(false)
		self._m_queIdleConn:push(Conn)
		
		Conn.m_DbConn:SetCharacterSet("latin1")
	end
	
	self._m_DbConnNum = uNum
end

-- 创建ThreadsMgr
function CDbChannelMgr:_CreateThreadsMgr(uNum)
	self._m_ThreadsMgr = g_ThreadApp:GetDbThreadMgr(uNum, self)
end

local _Release = IDbThreadScriptHandler.Release
function CDbChannelMgr:Release()

	g_ThreadApp:ReleaseDbThreadsMgr()
	
	for k, v in pairs(self._m_ayStatement) do
		v:Release()
	end
	
	g_ThreadApp:ReleaseDbConnectionMgr(self._m_DbConnMgr)
	
	_Release(self)
end


CDbChannelMgr.__new = CDbChannelMgr.new


function CDbChannelMgr:new(ConnNum,IP,Port,UserName,Password,DatabaseName,MaxDBResultSize)

	local NewInstance = CDbChannelMgr:__new()
	
	NewInstance._m_queWaitingConnCo		= CQueue:new()			-- 正在等待获取空闲connection的coroutine
	NewInstance._m_queIdleConn				= CQueue:new()		-- 空闲connection队列
	NewInstance._m_ThreadsMgr					= {}
	NewInstance._m_DbConnMgr					= {}
	NewInstance._m_AllChannels				= {}				-- 频道table
	NewInstance._m_ayStatement				= {}				-- 创建的Statement
	NewInstance._m_AllCoInfo					= {}			-- 当前Coroutine的信息 {m_Conn,m_Channels,m_WaitingChannels}
	NewInstance._m_queWaitingReloadCo = {}						-- 等待Reload的coroutine的队列
	NewInstance._m_CoReload						= {}			-- 调用reload功能的coroutine
	NewInstance._m_uActiveCoNum				= 0					-- 执行中的Coroutine的数量
	NewInstance._m_bReloading					= false
	NewInstance._m_QueryCoTable        		= {}
	NewInstance._m_DbConnNum 				= 0
	NewInstance._m_MaxDBResultSize			= MaxDBResultSize
	
	--数据库创建失败，我们直接退出程序了
	local ret, msg = pcall(CDbChannelMgr._CreateConnectionMgr, 
		   NewInstance, ConnNum,IP,Port,UserName,Password,DatabaseName)
	if not ret then
		ShowErrorMsgAndExit(msg .. "\n数据库创建Connection Mgr失败")
	end	
	
	ret, msg = pcall(CDbChannelMgr._CreateThreadsMgr, NewInstance, ConnNum)
	if not ret then
		ShowErrorMsgAndExit(msg .. "\n数据库创建Thread Mgr失败")
	end
		
	return NewInstance
end



function CDbChannelMgr:CreateStatement(ArgTable,HolderTable)
	if HolderTable == nil then
		return false
	end
	
	local sStmtName=ArgTable[1]
	local sStmtSql=ArgTable[2]
	
	assert(sStmtName)
	assert(sStmtSql)

	local uIndex = 0

	if HolderTable[sStmtName] ~= nil then
		local msg = "在不是reload状态下面不允许定义相同的statement " .. sStmtName .. " " .. sStmtSql
		error(msg)
		
		uIndex = HolderTable[sStmtName]._m_uIndex
		self._m_ayStatement[uIndex] = nil
		HolderTable[sStmtName]:Release()
	else
		HolderTable[sStmtName]=CStmtObj:new()
	end	
		
	uIndex = _GetIdleStatIndex(sStmtSql)
	HolderTable[sStmtName]._m_uIndex = uIndex
	HolderTable[sStmtName]._m_Mgr = self
	HolderTable[sStmtName]._m_StmtName = sStmtName
	HolderTable[sStmtName]._m_StmtSql = sStmtSql
	
	self._m_ayStatement[uIndex] = HolderTable[sStmtName]
	
	return true
end

function CDbChannelMgr:GetWaitingLocalChannel()
	local tblWaitingLocalChannel = {}
	if not self._m_WaitingLocalChannel then
		return tblWaitingLocalChannel
	end
	local curTime = GetProcessTime()
	for name, tbl in pairs(self._m_WaitingLocalChannel) do
		if tbl then
			for i, data in pairs(tbl) do
				local waitingTime = curTime - data.m_RequestTime
				--为减少高并发时，内存消耗过大，只采集大于10秒的数据
				if waitingTime > 10000 then
					table.insert(tblWaitingLocalChannel, {name, waitingTime, data.m_Channels, data.m_Count})
				end
			end
		end
	end
	return tblWaitingLocalChannel
end

function CDbChannelMgr:GetWaitingConnection()
	local tblWaitingConnection = {}
	if not self._m_WaitingConnection then
		return tblWaitingConnection
	end
	local curTime = GetProcessTime()
	for name, tbl in pairs(self._m_WaitingConnection) do
		if tbl then
			for i, data in pairs(tbl) do
				local waitingTime = curTime - data.m_RequestTime
				if waitingTime > 10000 then
					table.insert(tblWaitingConnection, {name, waitingTime, data.m_Channels, data.m_Count})
				end
			end
		end
	end
	return tblWaitingConnection
end

function CDbChannelMgr:GetWaitingGlobalChannel()
	local tblWaitingGlobalChannel = {}
	if not self._m_WaitingGlobalChannel then
		return tblWaitingGlobalChannel
	end
	local curTime = GetProcessTime()
	for name, data in pairs(self._m_WaitingGlobalChannel) do
		local waitingTime = curTime - data.m_RequestTime
		if waitingTime > 10000 then
			table.insert(tblWaitingGlobalChannel, {name, waitingTime, data.m_Channels, data.m_Count})
		end
	end
	return tblWaitingGlobalChannel
end

function CDbChannelMgr:GetExecutingSql()
	local tblExecutingSql = {}
	if not self._m_ExecutingSql then
		return tblExecutingSql
	end
	local curTime = GetProcessTime()
	for name, data in pairs(self._m_ExecutingSql) do
		local waitingTime = curTime - data.m_RequestTime
		if waitingTime > 10000 then
			table.insert(tblExecutingSql, {name, waitingTime, data.m_Channels, data.m_Count})
		end
	end
	return tblExecutingSql
end

function CDbChannelMgr:EnableSqlProfiler(open)
	g_bOpenSqlProfiler = open
	if g_bOpenSqlProfiler == false then
		g_tblSqlProfilerData = {}
		g_uSqlProfileAllTime = 0
	end
end

function CDbChannelMgr:GetDbSqlProfilerData()	
	return g_tblSqlProfilerData, g_uSqlProfileAllTime
end
