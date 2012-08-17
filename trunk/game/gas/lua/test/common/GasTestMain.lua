engine_require "common/Script/lunit"
lunit.import "all"
gas_require "test/testcase/GasReloadTest"
gas_require "test/testcase/GasEventTest"
gas_require "test/testcase/GasAccountMgrTest"
gas_require "test/testcase/GasPropMgrTest"
gas_require "test/testcase/GasTextFilterMgrTest"
gas_require "test/testcase/GasControlSceneTest"
gas_require "test/testcase/GasFriendTest"

AddCheckLeakFilterObj(lunit)

--重新装载运行环境
function RunGasTest(ConnMgrHandler)		
--	local TestG = {}
--	setmetatable(TestG, {__index = _G})
--	return setfenv(GasTestMain, TestG)(ConnMgrHandler)
	
	--因为resume不能直接传参数，所以导致只能使用闭包
	--resume不能传参数的原因可能是打了coco的补丁包。只是猜测还没有确认
	function GasTestMain()
		local NeedConn = {}
		NeedConn.Count = 0
		etc_gas_require "GasTestConfig"
		--看是否要真正运行测试用例
		if GasTestConfig.Need ~= 1 then 
			return 0
		end
		
		ConnMgrHandler._OnAccepted = ConnMgrHandler.OnAccepted
	
		local co = coroutine.running()
		while NeedConn.Count < GasTestConfig.ConnCount do
			coroutine.yield()
		end
		
		function ConnMgrHandler:OnAccepted(Connection)
			ConnMgrHandler:_OnAccepted(Connection)
			NeedConn[NeedConn.Count] = Connection
			NeedConn.Count = NeedConn.Count + 1
			coroutine.resume(co)
		end
		
		ConnMgrHandler._OnDisconnect = ConnMgrHandler.OnDisconnect
		function ConnMgrHandler:OnDisconnect(Connection)
			ConnMgrHandler:_OnDisconnect(Connection)
		end
	
		RegisterTestCase()
		lunit.run()
		
		if GasTestConfig.QuitWhenDone ~= 0 then
			g_App:Quit()
		end
		return 1
	end
	
	local co = coroutine.create(GasTestMain)
	coroutine.resume(co)
end



function RegisterTestCase()

	local TestSuites=GasTestConfig.TestSuites
	for i in pairs(TestSuites) do
		TestSuites[i]()
	end

end
