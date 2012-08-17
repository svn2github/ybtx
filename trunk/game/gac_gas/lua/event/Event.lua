gac_gas_require "event/EventInc"
engine_require "common/Misc/TypeCheck"


EEventState.TimeOut = 0
EEventState.Success = 1

--[[

	由于我们重新实现了一套waitevent机制，在engine\lua\common\CoSync下面，所以得使用新的wait机制，
	但是为了兼容原先的测试代码，所以我们把WaitEvent和SetEvent重新实现。
	对于同步对象，我们使用CEvent，虽然以前使用的是query，但是以前代码query只有一个item，所以用CEvent就可以代替，
	而且CEvent能够很好的实现light设置。
--]]


function WaitEvent(bAll,nTimer,tblEvent)
	assert(not IsNil(bAll) and IsBoolean(bAll))
	assert(IsNil(nTimer) or IsNumber(nTimer))
	assert(not IsNil(tblEvent) and IsTable(tblEvent))
	assert(tblEvent:empty())
		
	if tblEvent.bLight ~= true then
		if nTimer == 0 then
			return {EEventState.TimeOut,{}}
		end
		
		local tblNode = CEvent:new(false)
		tblEvent:push(tblNode)
		
		local value = CCoSync.WaitFor(nTimer, bAll, tblNode)
	
		if value == nil then
			tblEvent:pop()
			return { EEventState.TimeOut,{} }
		else
			return { EEventState.Success,tblEvent.tblRet }
		end
		
	else
		return { EEventState.Success,tblEvent.tblRet } --如果灯是亮的直接返回成功
	end
	
	
end

function SetEvent( tblEvent, bLight, ...)
	assert(not IsNil(tblEvent) and IsTable(tblEvent))

	--设置灯是否亮要先检查
	if bLight == true then 
		tblEvent.bLight = true
		tblEvent.tblRet = { ... }
	else
		tblEvent.bLight = false
		tblEvent.tblRet = {}
		return
	end
	
	if not tblEvent:empty() then
		local tblNode = tblEvent:front()
		tblEvent:pop()
		tblNode:Set(true)
	end
end

