
ThreadCoreServer={}
p=ThreadCoreServer


local function Run( FileName )
	ThreadCoreServer=nil
	Run=nil

	--创建Server主对象
	g_ThreadApp = CScriptThread_Inst()
	
	local lServer=g_ThreadApp
	
	require "server/thread/ThreadCoreQuit"	
	
	local CScriptThreadHandler=class (IScriptThreadHandler)

	function CScriptThreadHandler:OnEndThread()
		lServer:Quit()
	end
	
	local handler = CScriptThreadHandler:new()
	lServer:SetHandler(handler);
	
	require(FileName)
	
	StartUp()
	
	lServer:Run()
	
	CleanUp()  
	
	lServer:SetHandler(nil)
	
	g_ThreadApp=nil
	

end

ThreadCoreServer=
{
	Run = Run
}

