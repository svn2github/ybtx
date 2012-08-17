
CSyncObject = class()

function CSyncObject:Ctor()
	self.m_tCoroutine={}
end

function CSyncObject:_ResumeAllCo()

	local tCoroutine=self.m_tCoroutine
		
	for co,Info in pairs(tCoroutine) do
			
		local value = Info:ResumeIfReady( self, co )
		
		if not self:IsReady() then
			break;
		end
			
	end

end