str_require "test_unit/TestUnit"
str_require "test_unit/TestScript"

RunningCaseTbl = {}
setmetatable(RunningCaseTbl, {__mode = "k"})

--在这里我们设置test case默认的setup和teardown函数
--如果我们的testcase没有定义自己的，那么我们就默认使用这两个
--一般这两个函数就是为登录，登出使用
DefaultSetUp = DefaultLoginFlow
DefaultTearDown = DefaultLogoutFlow

local EndNum = 0

function TestRun(index,CurConnNum)
	local co = coroutine.running()
	
	---------------------------------------------------
	
	local test_case = g_TestCaseFun[index]
	
	local case_data = {}
	RunningCaseTbl[co] = case_data
	local user_name = BaseUserName .. CurConnNum
	case_data.m_UserName = user_name
	
	local ret = SetUpTestCase(test_case, case_data)
	if not ret then 
		return 
	end
	
	--run the test 
	while true do
		case_data.LoginTime = GetProcessTime()
		local ret = RunTestCase(test_case, case_data)	
		print("UserName:" .. case_data.m_UserName, "Loop Time:"..(GetProcessTime() - case_data.LoginTime))
		if not ret then	
			break
		end
	end
	
	TearDownTestCase(test_case, case_data)
	
	EndNum = EndNum + 1
	print(user_name, EndNum)
	
	---------------------------------------------------
	RunningCaseTbl[coroutine.running()] = nil
	
	if g_App.m_bQuit then
		if not next(RunningCaseTbl) then
			g_App:Quit()			
		end
	end
end

