--[[
	CResumeTick
	为了防止堆栈混乱，我们在obj准备好resume之前，创建一个tick，通过该tick来进行resume
--]]

CWaitForCoInfo = class()

local CResumeTick = class(CTick)

function CResumeTick:Ctor(co, tResultObj)
	assert(not IsNil(co))
	assert(not IsNil(tResultObj))
	
	self.m_co = co
	self.m_tResultObj = tResultObj
end

function CResumeTick:OnTick()
	g_App:UnRegisterTick(self)
	local co=self.m_co
	local tResultObj = self.m_tResultObj
	
	for k, v in ipairs (tResultObj) do
		v.m_tCoroutine[co] = nil
	end
	
	coroutine.resume( co, tResultObj )
end

function CWaitForCoInfo:Ctor(bWaitAll)
	self.m_co = coroutine.running()
	self.m_tObjects = {}
	self.m_bWaitAll=bWaitAll
end


function CWaitForCoInfo:ResumeIfReady( Obj, Co )
	
	local tResultObj={}

	if self.m_bWaitAll then
		
		--需要等待所有event,因此遍历这个coroutine当前正在等待的所有event,确认他们都是enabled状态
		for i,OtherObj in ipairs(self.m_tObjects) do
					
			if not OtherObj:IsReady() then
				--遇到第一个Not Ready同步对象，说明该coroutine还在等待其他未激活的Object,因此不需要唤醒该coroutine
				return 
			end
			
			--OtherObj已经被激活，放入结果队列
			table.insert( tResultObj, OtherObj )
			
		end
		
	else
		--等待至少一个Object被设置为enabled,因为当前Object正在被设置为enabled,所以该coroutine肯定要被唤醒
		--这里不需要再遍历所有的Object,除了当前Object之外，其他Object肯定是Not Ready状态。	
		table.insert( tResultObj, Obj )
		
	end
	
	if Obj._Take then
		Obj:_Take()
	end
	
	if self.m_Tick then
		g_App:UnRegisterTick(self.m_Tick)
	end
	
	local ResumeTick = CResumeTick:new(Co, tResultObj)
	g_App:RegisterTick(ResumeTick, 0)
	ResumeTick:SetTickName("ResumeTick")
	
	--coroutine.resume(Co, tResultObj)
	
end