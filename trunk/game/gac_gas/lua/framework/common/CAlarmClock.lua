gac_gas_require "framework/common/CAlarmClockInc"

--实现这个类是为了方便处理较长的固定时间点定时器任务. 如(每天 3点,  每周 1, 3, 5 晚8点 等等之类的时间)
g_AlarmClock = CAlarmClock:new()

g_AlarmClock.m_TaskTbl = {} --需要执行的任务表
g_AlarmClock.m_NextTaskTbl = {} -- 下次需要执行的任务
g_AlarmClock.m_NextTime = nil -- 下次需要执行的任务的时间
g_AlarmClock.m_TaskIndex = 0


local function DiffTime(wday1, time1, wday2, time2)
	if wday1 > wday2 or (wday1 == wday2 and time1 >= time2) then
		wday2 = wday2 + 7
	end
	return (wday2 - wday1) * 24 * 60 * 60 + (time2 - time1)
end


--从类似 "9:24" 的字符传中获取相对当天 0:00 点经过的秒数
local function GetOffsetSecond(strTime)
	if strTime == nil then
		return nil
	end
	local index1 = string.find(strTime, ":")
	assert(index1, "时间格式错误")
	local index2 = string.find(strTime, ":", index1 + 1) or 0
	local hour = tonumber(string.sub(strTime, 1, index1 - 1))
	local min = tonumber(string.sub(strTime, index1 + 1, index2 -1))
	local sec = 0
	if index2 ~= 0 then
		sec = tonumber(string.sub(strTime, index2 + 1, -1))
	end
	assert(hour and min and sec, "时间格式错误")
	return (hour * 60 + min) * 60  + sec
end

--定时的时间基类
local CTaskDate = class() 

function CTaskDate:NextTime()
end

--按周重复时间类
local CWeekRepeatDate = class(CTaskDate)

function CWeekRepeatDate:Ctor(date)
	self.wday = {}
	if date.wday and #date.wday ~= 0 then
		for _, w in pairs(date.wday) do
			assert (w >= 1 and w <= 7, "星期填写错误" )
			local wday = (w == 7 and 1 ) or w + 1 -- 星期天换成 1, 星期1 换成 2 ....
			table.insert(self.wday, wday)
		end
	else
		self.wday = {1,2,3,4,5,6,7}    --为了处理不分支,使用同一个for循环
	end

	self.offset = {}
	for _, s in pairs(date.time) do 
		table.insert(self.offset, GetOffsetSecond(s))
	end 
end

function CWeekRepeatDate:NextTime()
	self.cur_time = self.next_time
	if not self.cur_time then
		self.cur_time = os.time()
	end
	local date = os.date("*t", self.cur_time)
	local curOffset = (date.hour * 60 + date.min) * 60  + date.sec
	local tempDisSec
	local nearTime
	for _ , wday in pairs(self.wday) do
		for _, offset in pairs(self.offset) do
			tempDisSec = DiffTime(date.wday, curOffset, wday, offset)
			if nearTime == nil or tempDisSec < nearTime then
				nearTime = tempDisSec
			end
		end
	end
	self.next_time = self.cur_time + nearTime
	
	--这样返回是为了 纠正时间
	return  self.next_time - os.time()
end


local CTask = class()

function CTask:Ctor(taskName, date, fun, count, arg, argn, index)
	self.name = taskName
	self.fun = fun
	self.count = count
	self.arg = arg
	self.argn = argn
	self.index = index
	if date.type == nil or date.type ==  1 then --为了兼容原来的格式, 类型为nil 的默认是1 类型
		self.task_date = CWeekRepeatDate:new(date)
	else
		assert(false, "目前就支持1类新的 时间格式")
	end
end

function CTask:OnTaskTick()
	UnRegisterTick(self.tick)
	self.tick = nil
	self.fun(unpack(self.arg, 1, self.argn))
	
	if self.count then
		self.count = self.count -1
		if self.count <= 0 then
			g_AlarmClock:RemoveTask(self.index)
			return
		end
	end
	
	local time = self.task_date:NextTime()
	self.tick = RegClassTick("AlarmClockTike", self.OnTaskTick, time * 1000, self)
end

function CTask:Start()
	if self.tick then
		return
	end
	local time = self.task_date:NextTime()
	self.tick = RegClassTick("AlarmClockTike", self.OnTaskTick, time * 1000, self)
end

function CTask:Close()
	if self.tick then
		UnRegisterTick(self.tick)
		self.tick = nil
	end
end




--参数date格式
--date["time"] = {}  24小时制  必填,至少1个  格式例子: {"1:50", "23:00:05"}  hour 和 min 必须有 秒可有可无
--date["wday"] = {}  可填多个,不填代或为nil 表每天 也等价于{1,2,3,4,5,6,7}  7代表星期天
--date["day"] = {}   目前没有支持
--date["month"] = {} 目前没有支持
function CAlarmClock:AddTask(taskName, date, fun, count, ...)
	local arg = {...}
	local argn = select("#", ...)
	assert(IsFunction(fun) and date and date.time and (not count or count > 0), "CAlarmClock:AddTask 参数错误")

	local index = self.m_TaskIndex
	self.m_TaskIndex = self.m_TaskIndex + 1
	self.m_TaskTbl[index] = CTask:new(taskName, date, fun, count, arg, argn, index)
	self.m_TaskTbl[index]:Start()
	return index
end


function CAlarmClock:RemoveTask(index)
	self.m_TaskTbl[index]:Close()
	self.m_TaskTbl[index] = nil
end

function CAlarmClock:ClearTick()
	for i, task in pairs(self.m_TaskTbl) do
		task:Close()
		self.m_TaskTbl[i] = nil
	end
end


