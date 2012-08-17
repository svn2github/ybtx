local TestSleepTick = class(CTick)

TestSleepTick._new = TestSleepTick.new

function TestSleepTick:new()
	local tick = TestSleepTick:_new()
	tick.m_Co = coroutine.running()
	return tick
end

function TestSleepTick:OnTick()
	g_App:UnRegisterTick(self)
	PostMsg(self.m_Co, "SleepTimedOutMsg")
end

function TestSleep(time)
	local tick = TestSleepTick:new()
	g_App:RegisterTick(tick, time)
	WaitForMsg("SleepTimedOutMsg")
end
