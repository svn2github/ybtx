CAppServer.IntQuit = CAppServer.Quit


function CAppServer:Quit(bForceQuit)

	if bForceQuit == true then
		g_App:IntQuit()
		return 
	end
	
	--如果多次退出，强制退出
	if self._m_bCoreQuit then
		g_App:IntQuit()
		return
	end
	
	self._m_bCoreQuit = true

	g_App.m_bCallDbHalted = true

	local function exec()
		--print("before wait")
		local result=CCoSync.WaitFor(nil, true, self.CallDbEndEvent)
		--print("after wait")		
		g_App:IntQuit()
	end
	
	coroutine.resume(coroutine.create(exec))
end

function CAppServer:CallDbHalted()
   return self.m_bCallDbHalted
end
