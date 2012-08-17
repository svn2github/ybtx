
CScriptThread.IntQuit = CScriptThread.Quit

g_ThreadApp.DbCoEndEvent = {}

function CScriptThread:Quit()

	g_ThreadApp.m_bQuit = true;
	
	if not g_DbChannelMgr:CoroutineExist() then
		self:IntQuit()
	end
	
end

