engine_require "server/thread/ThreadCoreQuit"	
engine_require "server/thread/ThreadCoreReload"

gac_gas_require "reload/Reload"
	
CScriptThreadHandler=class (IScriptThreadHandler)


--Script线程的关闭是由引擎内部发起的，这个是引擎请求关闭线程的信号
--如果此处不调用Quit，引擎是不会强行关闭线程的
function CScriptThreadHandler:OnEndThread()
	g_ThreadApp:Quit()
end

local ReloadFileTbl = {}

function CScriptThreadHandler:OnFileChanged(data)
	local num = data:GetCount()

	for i = 1, num do
		ReloadFileTbl[#ReloadFileTbl + 1] = data:GetFileName(i - 1)
	end
	
	if g_ThreadApp:ReloadFileBegan() then
		print("正在reload file，不能再次reload")
		return
	end
	
	local function _ReloadFile()
		--尝试是否能够reload，如果不成功，yield
		g_ThreadApp:TryReloadFile()
		
		local num = #ReloadFileTbl
		for i = 1, num do
			local name = ReloadFileTbl[i]
			apcall(ReloadFile, name)
		end
		
		ReloadFileTbl = {}
		
		g_ThreadApp:EndReloadFile()
	end
	
	coroutine.resume(coroutine.create(_ReloadFile))
end
