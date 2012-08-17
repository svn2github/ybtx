CServantQueryCallback = class(IServantQueryCallback)

function CServantQueryCallback:Ctor()
	self.m_ServantTbl = {}
end

function CServantQueryCallback:QueryServant(Obj)
	--多次QueryServant, m_ServantTbl都没清空,会累加,可能会引起bug
	self:QueryServantJob(Obj)
	return self.m_ServantTbl
end

function CServantQueryCallback:Exec(uEntityId, Servant)
	self.m_ServantTbl[uEntityId] = Servant
end
