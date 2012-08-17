engine_require "common/Misc/TypeCheck"

CSemaphore = class(CSyncObject)

function CSemaphore:Ctor( _uCount )
	CSyncObject.Ctor(self)
	if IsNumber(_uCount) and _uCount>=0 then
		self.m_uCount=_uCount
	else
		self.m_uCount=0
	end
end


function CSemaphore:IsReady()
	return self.m_uCount>0
end


function CSemaphore:_Take( co )
	self.m_uCount = self.m_uCount - 1
end


function CSemaphore:Put( _uCount )
	assert( IsNumber(_uCount) )
	assert( _uCount>=0 )
	self.m_uCount = self.m_uCount + _uCount
	self:_ResumeAllCo()
end