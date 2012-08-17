CCoSync = class()

local CWaitForTick = class(CTick)

function CWaitForTick:Ctor(Info)
	assert(not IsNil(Info))
	self.m_Info = Info
end


function CWaitForTick:OnTick()
	g_App:UnRegisterTick(self)
	local co=self.m_co
	coroutine.resume( co, nil )
end


local function IsSyncObject(Object)
		local class=getmetatable(Object)
		return (Object ~= nil) and	IsTable(Object) and
		( (  class == CEvent) or ( class == CSemaphore) or ( class == CMsgQueue)	)
end


--[[
	等待一个或者多个Object被设置为Enabled,直到超时。
	uWaitTime:	等待的时间长度，毫秒为单位，nil表示永久等待。
	bAll:				true表示等待所有同步对象有效，false表示等待任意一个同步对象有效
	可变参数：	一个或者多个同步对象的名称	
	返回值：    nil表示timed out, 或者{event1,event2,...}名称表示哪个event有效。
--]]

function CCoSync.WaitFor(uWaitTime,bAll,...)
	local arg = {...}
	assert(not IsNil(bAll) and IsBoolean(bAll))
	assert(IsNil(uWaitTime) or IsNumber(uWaitTime))
	local tObjects=arg
	
	if next(tObjects)==nil then
		--print("一个同步对象都没有")
		return	--一个同步对象都没有
	end
	
	--如果该同步对象对应的队列不存在，则说明该同步对象是有效的
	
	local tResultObjects={}
	
	local bNeedWait=false

	if bAll then
		
		--等待所有的同步对象		
		for i,Object in ipairs(tObjects) do
			
			if not IsSyncObject(Object) then
				error("WaitFor failed:event is invalid.")
			end
			
			if not Object:IsReady() then
				--遇到第一个无效同步对象，立即退出循环，表示必须进入等待
				bNeedWait=true
				break
			end
			
			table.insert(tResultObjects,Object)
			
		end
		
	else
		
		--等待任意一个同步对象
		bNeedWait = true
		for i,Object in ipairs(tObjects) do
			
			if not IsSyncObject(Object) then
				error("WaitFor failed:event is invalid.")
			end
			
			if Object:IsReady() then
				--遇到第一个有效同步对象，将该同步对象放入列表中
				table.insert(tResultObjects,Object)	
				
				bNeedWait = false
				
			end
		end
		
		--bNeedWait=(next(tResultObjects) ~= nil)
				
	end
	
	if not bNeedWait then
		for i,v in ipairs(tResultObjects) do
			if v._Take then
				v:_Take()
			end
		end
		
		return tResultObjects
	end
		
	--不满足等待条件，需要进入等待队列
	
	local co=coroutine.running()
		
	local Info=CWaitForCoInfo:new(bAll)
		
	--将coroutine的信息挂到所有event的等待队列中
	for i,Object in ipairs(tObjects) do
		table.insert(Info.m_tObjects,Object)
		assert( nil == Object.m_tCoroutine[co] )
		Object.m_tCoroutine[co]=Info
	end
		
	if uWaitTime ~= nil then
		--等待时间不是nil,因此要注册一个timedout的tick
		local WaitForTick = CWaitForTick:new(Info)
		WaitForTick.m_co = co
		Info.m_Tick=WaitForTick
		g_App:RegisterTick(WaitForTick,uWaitTime)
		WaitForTick:SetTickName("WaitForTick")
	end
	
	return coroutine.yield()
	
end
