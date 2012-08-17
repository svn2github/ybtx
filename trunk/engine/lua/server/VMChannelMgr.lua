engine_require "server/DBCallbackCheck"

--从0开始计数，所以先减个一
local uAllVMNum = g_ScriptApp:GetChildVMNum() - 1
g_ChildVMNum = uAllVMNum

local VMTbl = {}

for i = 0, uAllVMNum do
	VMTbl[i] = {}
end

local Channel2VM = {}

local BLOCK = true
local UNBLOCK = false

local g_queTablePool = CQueue:new()

function GetTblFromPool()
	local table
	if g_queTablePool:empty() then
		table = {}
	else
		table = g_queTablePool:front()
		g_queTablePool:pop()
	end
	return table
end

function ReturnTblToPool(table)
	for k, v in pairs(table) do
		table[k] = nil
	end
	g_queTablePool:push(table)
end

local function MergeChannelToVM(VMNum, CallDbData)
	local MergeVM = VMTbl[VMNum]
		
	for ChannelId, v in pairs(CallDbData["channel"]) do
		local uVM = Channel2VM[ChannelId]
		local vm = VMTbl[uVM]
				
		--合并到一个vm
		if uVM ~= VMNum then
			MergeVM[ChannelId] = vm[ChannelId]
			vm[ChannelId] = nil
			Channel2VM[ChannelId] = VMNum
		end
	end
		
	return MergeVM
end

local uDispatchNum = 0

--处理所有channel都是第一次使用的情况
local function HandleChannelWithNoUsedVM(CallDbData, ...)
	--[[
		这些channel是第一次使用，我们随机分配到一个vm里面			
	--]]
	local arg = {...}
		
	local uVM = uDispatchNum
	uDispatchNum = uDispatchNum + 1
	if uDispatchNum > uAllVMNum then
		uDispatchNum = 0
	end
	
	--取得vm
	local vm = VMTbl[uVM]
	
	for k, ChannelId in ipairs(arg) do
		Channel2VM[ChannelId] = uVM
		
		--把该channel加入vm对应的queue里面去
		--因为是第一次使用该channel，所以队列也绝对是第一次创建
		vm[ChannelId] = CQueue:new()
		vm[ChannelId]:push(CallDbData)
	end
	
	--返回是否阻塞，相应的vm以及call db的数据
	return UNBLOCK, uVM, CallDbData
end

--处理所有channel中有一个vm在使用这些channel的情况
local function HandleChannelWithOneUsedVM(CallDbData, UsedVM, ...)
	--[[
		这些channel里面有可能一部分是第一次使用，而使用的则在同一个vm里面
		这个分析也比较麻烦，因为可能在该vm里面的channel已经被block了
	--]]
	local arg = {...}
		
	local uVM = 0
	for uUsedVMNum, v in pairs(UsedVM) do
		uVM = uUsedVMNum
		break
	end
	
	local bBlock = false

	local vm = VMTbl[uVM]
	
	for k, ChannelId in ipairs(arg) do
		--有可能channel是第一次加入
		if Channel2VM[ChannelId] == nil then
			Channel2VM[ChannelId] = uVM
		end
		
		if vm[ChannelId] == nil then
			vm[ChannelId] = CQueue:new()
		end
						
		--如果该vm里面的channel已经被block了
		if vm[ChannelId].bBlock == BLOCK then
			bBlock = true
			--该操作的对应的channel状态都要为block
			CallDbData["channel"][ChannelId] = BLOCK			
		end
		
		vm[ChannelId]:push(CallDbData)
	end
	
	if bBlock then
		--该vm里面的channel状态为BLOCK
		for k, ChannelId in ipairs(arg) do
			vm[ChannelId].bBlock = BLOCK		
		end
		--该vm里面的channel状态为BLOCK
		return BLOCK, 0, CallDbData
	else
		return UNBLOCK, uVM, CallDbData
	end
end

--处理所有channel中有多个vm在使用这些channel的情况
local function HandleChannelWithMoreUsedVM(CallDbData, UsedVM, uUsedNum, ...)
	--[[
		这些channel被不同的vm使用了，
	--]]
	local arg = {...}
	local uVM = 0
	local uSelectVM = {}
	for uUsedVMNum, v in pairs(UsedVM) do
		table.insert(uSelectVM, uUsedVMNum)
	end

	--随机选择一个vm
	uVM = uSelectVM[math.random(1, uUsedNum)]
	
	local BlockTbl = {}
	local uBlock = 0
	
	for k, ChannelId in ipairs(arg) do
		--如果是第一次加入，我们加入一个使用的vm里面
		if Channel2VM[ChannelId] == nil then
			Channel2VM[ChannelId] = uVM
		end
		
		local uNum = Channel2VM[ChannelId]
		local vm = VMTbl[uNum]
		
		if vm[ChannelId] == nil then
			vm[ChannelId] = CQueue:new()
		else
			--如果一个vm里面channel队列有数据，那么我们记录一下
			--每一个vm只需要记录一次就行了
			if BlockTbl[uNum] == nil then
				uBlock = uBlock + 1
			end
			BlockTbl[uNum] = true
		end
	end

	if uBlock > 1 then
		--如果有多个vm里面的channel队列都有数据，那没辙了，等待吧
		for k, ChannelId in ipairs(arg) do
			local uNum = Channel2VM[ChannelId]
			local vm = VMTbl[uNum]
							
			--如果vm的channel队列不为空，表明该操作对应的channel的状态为BLOCK了
			if not vm[ChannelId]:empty() then
				CallDbData["channel"][ChannelId] = BLOCK
			end
			
			vm[ChannelId]:push(CallDbData)	
			vm[ChannelId].bBlock = BLOCK	
		end

		return BLOCK, 0, CallDbData				
	else
		error("Request Channel 出错，在不同vm里面的频道并没有相应的队列信息")
	end
end

local function WaitForChannel(...)
	local arg = {...}
	local CallDbData = {}
	CallDbData["channel"] = {}

	local UsedVM = {}
	local uUsedNum = 0
	
	for k, ChannelId in ipairs(arg) do
		--首先找到这次操作的channel所属的vm，如果该channel对应的vm不存在，先不做处理
		local uVM = Channel2VM[ChannelId]
		if uVM then
			if UsedVM[uVM] == nil then
				uUsedNum = uUsedNum + 1
			end
			UsedVM[uVM] = true
		end
		CallDbData["channel"][ChannelId] = UNBLOCK
	end
			
	--看channel使用的vm是否是同一个vm
	if uUsedNum == 0 then
		return HandleChannelWithNoUsedVM(CallDbData, ...)		
	elseif uUsedNum == 1 then
		return HandleChannelWithOneUsedVM(CallDbData, UsedVM, ...)
	else
		return HandleChannelWithMoreUsedVM(CallDbData, UsedVM, uUsedNum, ...)
	end
end

local t_insert = table.insert
local len = string.len
local tbl_maxn = table.maxn

local tbl_blockdata = {}
local tbl_transation = {}
local b_StatisticDBTrans  = false
if GasConfig.StatisticDBTrans == 1 then
	b_StatisticDBTrans = true
end

function RequestVM(CallFunc, filename, funcname, callback, data, errfunc, ...)
	local arg = {...}
	local uSelectVM = 1

	local bBlock
	local uNum
	local CallDbData
	
	local Channels = {}
	local tbl_single = GetTblFromPool()
	
	if b_StatisticDBTrans then
		if tbl_transation[funcname] == nil then
			tbl_transation[funcname] = {0,0,{GetProcessTime()}}
		else
			t_insert(tbl_transation[funcname][3],GetProcessTime())
		end
	end

	local argn = select("#", ...)
	for i = 1, argn do
		local v = arg[i]
		if v ~= 0 and v then
			if tbl_single[v] ~= 1 then
				if type(v) == "number" then
					t_insert(Channels, v)
				elseif type(v) == "string" then
					local length = len(v)
					if length > 0 and length <= 32 then
						local hashID = String2ID(v)
						t_insert(Channels, hashID)		
					else
						LogErr("channel 超过了最大长度32","channel:"..v)
						return
					end
				else
					error("不正确的channel类型")
				end
			end
			tbl_single[v] = 1
		end
	end
	ReturnTblToPool(tbl_single)
	
	local tickIndex
	if g_bDBCallBackCheck then
		tickIndex = RegisterDBCallbackCheckTick(funcname, Channels)
	end
	
	bBlock, uNum, CallDbData = WaitForChannel(unpack(Channels))
	
	if bBlock then
		--如果block了，我们保存调用信息，等unblock之后在调用
		CallDbData["CallFunc"] = CallFunc
		CallDbData["filename"] = filename
		CallDbData["funcname"] = funcname
		CallDbData["callback"] = callback
		CallDbData["data"] 	   = data
		CallDbData["errfunc"]  = errfunc
		CallDbData["tickIndex"] = tickIndex
		CallDbData["bBlock"]   = BLOCK
		CallDbData["uBlockTime"] = GetProcessTime()
		CallDbData["Channels"] = arg
		t_insert(tbl_blockdata,CallDbData)
	else
		CallFunc(uNum, filename, funcname, callback, data, errfunc, tickIndex, unpack(Channels))
	end
end

---------------------------------------------------------------------------------

local function IsBlock(CallDbData)
	for ChannelId, bBlock in pairs(CallDbData["channel"]) do
		--如果calldb对应的channel还有被挂起的，我们继续等吧
		if bBlock == BLOCK then
			return true
		end
	end
	return false
end


local function CancelBlockForDbTrans(VMNum, CallDbData)
	if CallDbData["bBlock"] == false then
		return
	end
	
	local CallFunc = CallDbData["CallFunc"]
	local filename = CallDbData["filename"]
	local funcname = CallDbData["funcname"]
	local callback = CallDbData["callback"]
	local data     = CallDbData["data"]
	local errfunc  = CallDbData["errfunc"]
	local tickIndex = CallDbData["tickIndex"]
	
	local channels = {}

	for ChannelId, v in pairs(CallDbData["channel"]) do
		table.insert(channels, ChannelId)
	end
	
	CallFunc(VMNum, filename, funcname, callback, data, errfunc, tickIndex, unpack(channels))
	
	CallDbData["bBlock"] = false
	CallDbData["uBlockTime"] = nil
	for i=1,#tbl_blockdata do
		if tbl_blockdata[i] == CallDbData then
			table.remove(tbl_blockdata,i)
		end
	end
end

local function UnblockCallDbData(CallDbData, ChannelId, VMNum)
	if CallDbData["bBlock"] == true then
		--该操作对应的channel已经可以使用了
		CallDbData["channel"][ChannelId] = UNBLOCK
		
		--如果还有channel被block了，那么继续等吧
		if IsBlock(CallDbData) then
			return
		end		
		
		--如果calldb的所有channel都被等到了，我们就要开始处理了
		
		--使用本次回调的vm，然后把在其他vm的该channel队列移动到这个vm里面
		--这样下次在用这几个channel调用calldb的时候就很方便了，因为在同一个vm里面了
		local MergeVM = MergeChannelToVM(VMNum, CallDbData)
		
		--调用这个先前被block的db trans
		CancelBlockForDbTrans(VMNum, CallDbData)
		
		--[[
			因为这次calldb会把后来使用该channel的calldb也给block了，
			所以现在我们就要依次遍历channel的队列，把先前block的也给调用了
		--]]
			
		local ChannelQueue = MergeVM[ChannelId]	
			
		--得到该queue的第二个数据，因为第一个就是才处理的calldb
		local iter = ChannelQueue._begin._next
		while iter do
			local DbData = iter._value
			DbData["channel"][ChannelId] = UNBLOCK
			
			if IsBlock(DbData) then
				break
			end
			
			MergeChannelToVM(VMNum, DbData)
			
			CancelBlockForDbTrans(VMNum, DbData)
			
			iter = iter._next
		end

		--表明已经遍历了整个queue队列，等待的db trans都被调用了
		--那么我们把这个queue设置为unblock
		if iter == nil then
			MergeVM[ChannelId].bBlock = UNBLOCK
		end
	end
end

local s_format = string.format

--[[
	每个calldb的调用，都会有这个回调，
	当一个calldb的vm回调产生的时候，首先找到该channel对应的vm，然后找到该vm对应的该channel的queue
	弹出queue的一个元素，因为这个元素就是该操作的
	然后判断该queue的头元素，此时是下一个calldb操作的，如果这个calldb被block了，
	就要开始处理了，比较麻烦
--]]
function OnHandleVMCallback(funcname, tickIndex, ...)
	local arg = {...}
	
	if b_StatisticDBTrans then
		if tbl_transation[funcname] ~= nil then
			tbl_transation[funcname][1] = tbl_transation[funcname][1] + 1
			assert(tbl_maxn(tbl_transation[funcname][3]) > 0)
			tbl_transation[funcname][2] = tbl_transation[funcname][2] + GetProcessTime() - tbl_transation[funcname][3][1]
			table.remove(tbl_transation[funcname][3],1)
		end
	end
	
	if g_bDBCallBackCheck then
		UnRegisterDBCallbackCheckTick(tickIndex)
	end
	
	--每调用一次，引用计数减一
	g_App.CallDbEndEvent.RefNum = g_App.CallDbEndEvent.RefNum - 1
	assert(g_App.CallDbEndEvent.RefNum >= 0)
	
	for k, ChannelId in ipairs(arg) do
		assert(ChannelId)
		local uVM = Channel2VM[ChannelId]
		if not uVM or uVM > uAllVMNum or uVM < 0 then
			local msg = s_format("ChannelId %s对应的vm不存在，channel id: %s", tostring(ChannelId), table.concat(arg, " "))
			error(msg)
		end
		
		local vm = VMTbl[uVM]
		vm[ChannelId]:pop()
		
		if vm[ChannelId]:empty() then
			vm[ChannelId] = nil
			Channel2VM[ChannelId] = nil
		else
			--如果该queue没有block，那么不管了
			if vm[ChannelId].bBlock == BLOCK then
				--得到该channel下一次的数据
				local CallDbData = vm[ChannelId]:front()
				
				UnblockCallDbData(CallDbData, ChannelId, uVM)	
			end
		end
		
	end	
	

	if g_App.CallDbEndEvent.RefNum == 0 then
		g_App.CallDbEndEvent:Set(true)
	end
end

--获取每一个vm里面各个channel的个数
function GetVMChannelInfo()
	local tbl = {}
	for uVM = 0, uAllVMNum do
		local vm = VMTbl[uVM]
		t_insert(tbl, s_format("VM ID: %d\n", uVM))
		for uChannelId, ChannelQue in pairs(vm) do
			local uChannelNum = ChannelQue:size()
			t_insert(tbl, s_format("%s\t%d\n", tostring(uChannelId), uChannelNum))
		end
	end
	
	return table.concat(tbl)	
end

--将当前vm和channel的情况导出到一个文件，便于分析
function DumpVMChannelInfo()
	local str = GetVMChannelInfo()
	
	local tid = GetCurrentThreadID()	
	local filename = "VMInfo_" .. tid .. ".log"
	
	local info = CLog_CreateLog(filename, "a")
	info:Write(str)
	info:Release()
end

function GetBlockChannelList()
	local tblBlockChannel = {}
	local curTime = GetProcessTime()
	for i = 1, #tbl_blockdata do
		local blockTime = curTime - tbl_blockdata[i]["uBlockTime"]
		--为减少高并发时调用，内存消耗过大，只采集超过10秒的数据
		if blockTime > 10000 then
			table.insert(tblBlockChannel,{tbl_blockdata[i]["Channels"],tbl_blockdata[i]["funcname"], blockTime})
		end
	end
	return tblBlockChannel
end

function StartDbTranProfiler()
	b_StatisticDBTrans = true
end

function StopDbTranProfiler()
	b_StatisticDBTrans = false
	tbl_transation = {}
end

function GetDbTranProfile()
	return tbl_transation
end
