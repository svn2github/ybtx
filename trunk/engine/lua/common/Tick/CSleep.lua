CSleep = class( CTick )

--[[
返回值：0-时间到
        1-被唤醒
--]]
function CSleep:Sleep(SleepTime, TickName)
	assert(TickName)
	self.m_TickName = TickName
	self.m_Routine = coroutine.running()
	g_App:RegisterTick(self,SleepTime)
	self:SetTickName(TickName)
	return coroutine.yield()
end

function CSleep:OnTick()
	g_App:UnRegisterTick(self)
	coroutine.resume(self.m_Routine, 0)
end

function CSleep:Wake()
	g_App:UnRegisterTick(self)
	if not self.m_Routine then
		return
	end
	if coroutine.status(self.m_Routine) == 'suspended' then
		coroutine.resume(self.m_Routine, 1)
	end
end