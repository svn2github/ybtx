CMsgQueue = class(CSyncObject)

function CMsgQueue:Ctor()
	CSyncObject.Ctor(self)
	self.m_MsgQueue=CQueue:new()
end

function CMsgQueue:IsReady()
	return not self.m_MsgQueue:empty()
end

function CMsgQueue:Post( Msg )
	self.m_MsgQueue:push( Msg )
	self:_ResumeAllCo()
end

function CMsgQueue:Recv()
	local Result = self.m_MsgQueue:front()
	self.m_MsgQueue:pop()
	return Result
end

function CMsgQueue:Clear()
	self.m_MsgQueue:clear()
end