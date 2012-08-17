

function CScriptThread:ReloadFileBegan()
	return self.m_bReloadFileBegan
end

function CScriptThread:TryReloadFile()
	self.m_bReloadFileBegan = true
	self.m_DbCo = {}
	
	if g_DbChannelMgr:CoroutineExist() then
		self.m_ReloadCo = coroutine.running()
		coroutine.yield()
	end
end

function CScriptThread:EndReloadFile()
	self.m_bReloadFileBegan = false
	self:UnBlockAllDbQuery()
end

function CScriptThread:BlockDbQuery(co)
	table.insert(self.m_DbCo, co)
	
	coroutine.yield()
end

function CScriptThread:UnBlockAllDbQuery()
	local tbl_co = self.m_DbCo

	for i = 1, #tbl_co do
		coroutine.resume(tbl_co[i])
	end
	
	self.m_DbCo = {}
	
end

function CScriptThread:ResumeReloadFile()
	coroutine.resume(self.m_ReloadCo) 
end

