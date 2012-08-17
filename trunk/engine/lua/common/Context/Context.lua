engine_require "common/Misc/TypeCheck"

CContext = class(CTick)

--CContext构造
function CContext:Ctor( data, app )
	assert( IsTable(app) )
	
	self.m_Data = data
	self.m_App	 = app
end

--时间一到，删除该context
function CContext:OnTick()
	--print("---------->>>>>>>>>>>>>>>>CContext:OnTick()")
	self.m_App:UnRegisterTick(self)
	local id = self.m_nID
	self.m_ContextMgr.m_tblContext[id] = nil
end