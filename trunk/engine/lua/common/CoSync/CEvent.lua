engine_require "common/Misc/TypeCheck"

CEvent = class(CSyncObject)

function CEvent:Ctor(_bSignaled)
	CSyncObject.Ctor(self)
	if IsBoolean(_bSignaled) then
		self.m_bSignaled=_bSignaled
	else
		self.m_bSignaled=false
	end
end

function CEvent:IsReady()
	return self.m_bSignaled
end

--[[
	bEnable：	将Object设置为Enable或者Disabled
	如果Object被设置为Enabled,则唤醒所有等待该Object的，并且满足其他等待条件的coroutine.
--]]

function CEvent:Set( bEnabled )
	assert( IsBoolean( bEnabled ) )
	
	if bEnabled then
		
		--设置同步对象为Enabled
		if self.m_bSignaled then
			--同步对象本来就处于Enabled状态，那么什么也不用做
			return
		end
		
		--将同步对象至为enabled
		self.m_bSignaled = true
		
		self:_ResumeAllCo()
		
	else
		
		--设置同步对象为Disabled
		self.m_bSignaled=false
		
	end
end
