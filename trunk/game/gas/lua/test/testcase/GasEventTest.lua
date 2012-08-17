gas_require "test/testcase/event_testfile/GasTestEventTable"
gac_gas_require "event/Event"

-- 异步还没想好怎么测试，目前只测试了参数检察
local function TestWait()
	local strRet = WaitEvent(true,nil,GasTestEvent.Test.TestLogin)
	assert_equal( "OK",strRet,"" )
end

--[[
local function TestWaitMoreObj()
	local One = GasTestEvent.Test.TestWaitOne
	local Two = GasTestEvent.Test.TestWaitTwo
	local Three = GasTestEvent.Test.TestWaitThree
	local strRet = WaitEvent(One,Two,Three)
	assert_equal( "OK",strRet,"" )
end
--]]
local function TestResumeOne()
	local strRet = WaitEvent(true,nil,GasTestEvent.Test.TestWaitMore1)
	assert_equal("OK",strRet,"" )
end

local function TestResumeTwo()
	local strRet = WaitEvent(true,nil,GasTestEvent.Test.TestWaitMore1)
	assert_equal("OK",strRet,"" )
end


local function TestPass()
	local strRet = WaitEvent(true,nil,GasTestEvent.Test.TestWaitMore1)
end


local function TestWait1()
	local strRet = WaitEvent(true,nil,GasTestEvent.Test.TestWaitMore1)
end

local function TestWait2()
	local strRet = WaitEvent(true,nil,GasTestEvent.Test.TestWaitMore2)
end
--[[
local function TestWait1andWait2()
	local strRet = WaitEvent(true,nil,GasTestEvent.Test.TestWaitMore2,GasTestEvent.Test.TestWaitMore1)
end
--]]

local function TestMoreWait1()
	local strRet = WaitEvent(true,nil,GasTestEvent.Test.TestLogin)
	assert_equal( "OK",strRet,"" )
end

local function TestMoreWait2()
	function func()
		local strRet = WaitEvent(true,nil,GasTestEvent.Test.TestLogin)
	end
	assert_error( "No Error", func )
end

--[[
local function TestTimerOutTrue()
	local strRet = WaitEvent(true,3000,GasTestEvent.Test.TestWaitMore1)
	assert_equal( "TimeOut",strRet,"" )
	--print("the strRet = ",strRet)
	SetEvent(GasTestEvent.Test.TestWaitMore2,true,"NO")
	SetEvent(GasTestEvent.Test.TestWaitMore2,false,"NO")
end

local function TestTimerOut()
	local co = coroutine.create(TestTimerOutTrue)
	assert_equal(true,coroutine.resume(co),"")
	WaitEvent(true,nil,GasTestEvent.Test.TestWaitMore2)
end
--]]



-- 测试testcase
function InitGasEventTest()	
	local TestTest=TestCase("test/testcase/GasEventTest")
	function TestTest:setup()
	end
--[[
	--等待单个事件
	function TestTest:TestWaitEvent()
		local co = coroutine.create(TestWait)
		assert_equal(true,coroutine.resume(co),"")
		assert_equal("suspended",coroutine.status(co))
		SetEvent(GasTestEvent.Test.TestLogin,true,"OK")
		assert_equal("dead",coroutine.status(co))
		SetEvent(GasTestEvent.Test.TestLogin,false,"OK")
	end
	
	--SetEvent的时候比waitevent早。并且第二个参数为true
	function TestTest:TestSetBefore()
		
		SetEvent(GasTestEvent.Test.TestWaitMore1,true,"OK")
		local co = coroutine.create(TestPass)
		assert_equal(true,coroutine.resume(co),"")
		assert_equal("dead",coroutine.status(co))
		SetEvent(GasTestEvent.Test.TestWaitMore1,false,"OK")
	end
--]]	
	
	--参数不能为空
	function TestTest:TestWaitNoEvent()
	
		function func1()
			WaitEvent(true,nil,GasTestEvent.Test.Teogin)
		end
		assert_error( "No Error", func1 )
		
		function func2()
			WaitEvent()
		end
		assert_error( "No Error", func2 )
		
		function func2()
			WaitEvent("haha")
		end
		assert_error( "No Error", func2 )
		
		function func3()
			SetEvent("haha")
		end
		assert_error( "No Error", func3 )
		
		function func4()
			SetEvent(GasTestEvent.Test.Teogin)
		end
		assert_error( "No Error", func4 )
		
		function func5()
			WaitEvent(true,"11",GasTestEvent.Test.TestLogin)
		end
		assert_error( "No Error", func5 )
	end

--[[	
	function TestTest:TestMoreWait()
		local co = coroutine.create(TestMoreWait1)
		assert_equal(true,coroutine.resume(co),"")
		assert_equal("suspended",coroutine.status(co))
		local co = coroutine.create(TestMoreWait2)
		assert_equal(true,coroutine.resume(co),"")
		SetEvent(GasTestEvent.Test.TestLogin,true,"OK")
		SetEvent(GasTestEvent.Test.TestLogin,false,"OK")
	end

	--测试时间超过等待时间
	function TestTest:TestTimerOut()
		TestTimeOut = false
		local co = coroutine.create(TestTimerOut)
		assert_equal(true,coroutine.resume(co),"")
	end
	
	--等待多个事件
	function TestTest:TestWaitMoreEvent()
		local co = coroutine.create(TestWaitMoreObj)
		assert_equal(true,coroutine.resume(co),"")
		assert_equal("suspended",coroutine.status(co))
		SetEvent(GasTestEvent.Test.TestWaitOne,true,"NO")
		assert_equal("suspended",coroutine.status(co))
		SetEvent(GasTestEvent.Test.TestWaitTwo,true,"NO")
		assert_equal("suspended",coroutine.status(co))
		SetEvent(GasTestEvent.Test.TestWaitThree,true,"OK")
		assert_equal("dead",coroutine.status(co))
	end
	
	--等待多个事件
	function TestTest:TestResumeMore()
		local co = coroutine.create(TestResumeOne)
		assert_equal(true,coroutine.resume(co),"")
		local co1 = coroutine.create(TestResumeTwo)
		assert_equal(true,coroutine.resume(co1),"")
		assert_equal("suspended",coroutine.status(co))
		assert_equal("suspended",coroutine.status(co1))
		SetEvent(GasTestEvent.Test.TestWaitMore1,true,"OK")
		SetEvent(GasTestEvent.Test.TestWaitMore1,false,"OK")
		assert_equal("dead",coroutine.status(co))
		assert_equal("dead",coroutine.status(co1))
	end
	
	--在两个事件有同一个coroutine的，并且他们后面有队列
	function TestTest:TestResume2Event()
		SetEvent(GasTestEvent.Test.TestWaitMore1,false,"OK")
		
		local co_wait1 = coroutine.create(TestWait1)
		assert_equal(true,coroutine.resume(co_wait1),"")
		local co_wait2 = coroutine.create(TestWait2)
		assert_equal(true,coroutine.resume(co_wait2),"")
		local co1_wait1andwait2 = coroutine.create(TestWait1andWait2)
		assert_equal(true,coroutine.resume(co1_wait1andwait2),"")
		
		assert_equal("suspended",coroutine.status(co_wait1))
		assert_equal("suspended",coroutine.status(co_wait2))
		assert_equal("suspended",coroutine.status(co1_wait1andwait2))
		
		SetEvent(GasTestEvent.Test.TestWaitMore1,true,"OK")
		assert_equal("dead",coroutine.status(co_wait1))
		assert_equal("suspended",coroutine.status(co1_wait1andwait2))
		SetEvent(GasTestEvent.Test.TestWaitMore1,false,"OK")
		
		SetEvent(GasTestEvent.Test.TestWaitMore2,true,"OK")
		assert_equal("dead",coroutine.status(co_wait2))
		assert_equal("dead",coroutine.status(co1_wait1andwait2))
		SetEvent(GasTestEvent.Test.TestWaitMore2,false,"OK")
	end
--]]
	

	function TestTest:teardown()
	end
	
end