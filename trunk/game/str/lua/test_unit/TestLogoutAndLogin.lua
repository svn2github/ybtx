TestLogOutAndLoginCase = TestCase("TestLogoutAndLogin")

function TestLogOutAndLoginCase:test_LogoutAndLogIn(case_data)
	assert(case_data.m_Conn)
	local user_name = case_data.m_UserName
	print(user_name,"LogoutAndLogIn1")
	--断开连接再登录
	ReConnect(case_data, 2000, ConnAddr)
	Login(case_data, 0)
	print(user_name,"LogoutAndLogIn2")
end
