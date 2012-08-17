engine_require "common/Misc/TypeCheck"
gac_gas_require "framework/common/UserNameCheck"


function InitGacTestLogin()
	local TestTest=TestCase("TestLogin")
	function TestTest:setup()
	end
	
	--测试用户名的所有（由字母a～z(不区分大小写)、数字0～9、点、减号或下划线组成）
	function TestTest:TestCheckUserNameAll()
--		print("int the TestCheckUserNameAll")
		assert_equal(true, CheckUserName("123Aa.-_b"), "")
		assert_equal(false, CheckUserName("123*123"), "")
		assert_equal(false, CheckUserName("123~123"),"")
		assert_equal(false, CheckUserName("123#123"),"")
		assert_equal(false, CheckUserName("123好123"),"")
		assert_equal(false, CheckUserName("天人之恋"),"")
	end
	
	--测试用户名的开头和结尾（只能是数字或字母）
	function TestTest:TestCheckUserNameBeginEnd()
--		print("int the TestCheckUserNameBeginEnd")
		assert_equal(true, CheckUserName("1abcdef2"),"")
		assert_equal(true, CheckUserName("a12_34b"),"")
		assert_equal(true, CheckUserName("a123.123"),"")
		assert_equal(true, CheckUserName("123-123G"),"")
		assert_equal(false, CheckUserName(".123123"),"")
		assert_equal(false, CheckUserName("-123123"),"")
		assert_equal(false, CheckUserName("_123123"),"")
		assert_equal(false, CheckUserName("123123."),"")
		assert_equal(false, CheckUserName("123123-"),"")
		assert_equal(false, CheckUserName("123123_"),"")
		assert_equal(false, CheckUserName(".123123-"),"")	
	end
	
	--测试用户名的长度（必须为4~18个）
	function TestTest:TestCheckUserNameLength()
		--print("int the TestCheckUserNameLength")
		assert_equal(false, CheckUserName("123"), "")
		assert_equal(true, CheckUserName("1234"), "")
		assert_equal(true, CheckUserName("123456789011121314"), "")
		assert_equal(false, CheckUserName("1234567890111213141"), "")
	end
	
	--测试“确定”按钮的使能状态
	function TestTest:TestBtnOKStatus()
		assert_equal(false, g_Login.m_LoginAccounts.m_btnOK:IsEnable(), "")
	end
	--[[
	--用户名不合法导致登录失败
	function TestTest:TestLoginFailUserNameInValid()
		--print("in the TestLoginFailUserNameInValid")
		g_Login.m_LoginAccounts.m_editPassword:SetWndText("111")
		g_Login.m_LoginAccounts.m_editUserName:SetWndText("111")
		g_Login.m_LoginAccounts:OnCtrlmsg(g_Login.m_LoginAccounts:GetDlgChild( "Btn_OK" ),BUTTON_LCLICK,0,0)
		assert_equal( EGameState.eToLogin, g_App.m_re, "" )
	end
	--]]
	--没输密码导致登录失败
	function TestTest:TestLoginFailNoPassword()
		--print("in the TestLoginFailNoPassword")
		g_Login.m_LoginAccounts.m_editUserName:SetWndText("1111")
		g_Login.m_LoginAccounts:OnCtrlmsg(g_Login.m_LoginAccounts:GetDlgChild( "Btn_OK" ),BUTTON_LCLICK,0,0)
		assert_equal( EGameState.eToLogin, g_App.m_re, "" )
	end
	
--[[
	--连接RCP服务器成功后RCP服务器断开
	function TestTest:TestConnRCPDisConn()
		
		--print("in the TestConnRCPDisConn")
		g_Login.m_LoginAccounts.m_editPassword:SetWndText("111")
		
		g_Login.m_LoginAccounts.m_editUserName:SetWndText("1111")
		
		g_Login.m_LoginAccounts:OnCtrlmsg(g_Login.m_LoginAccounts:GetDlgChild( "Btn_OK" ),BUTTON_LCLICK,0,0)
		
		local strRet = WaitEvent(true,30000,Event.Test.RCPBegan)
		SetEvent(Event.Test.RCPBegan,false)
		if strRet[1] == EEventState.Success then
			if strRet[2][1] == "RCPConnSuccess" then
				g_Login.m_Socket:ShutDown()
				g_Login.m_Socket:OnDisconnected(nil,nil)
				strRet = WaitEvent(true,30000,Event.Test.RCPRecv)
				SetEvent(Event.Test.RCPRecv,false)
				if strRet[1] == EEventState.Success then
					assert_equal("RCPRecvFailure", strRet[2][1], "")
				else
					assert_fail("RCP Recv Failure TimeOut")
				end
			else
				assert_fail("RCP Connect Failure")
			end
		else
			assert_fail("RCP Began Failure TimeOut")
		end	
	end
	--]]
	--登录成功
	function TestTest:TestLoginSuccess()
		--print("in the TestLoginSuccess")
		
		g_Login.m_LoginAccounts.m_editPassword:SetWndText("justfortest2007")
		g_Login.m_LoginAccounts.m_editUserName:SetWndText("wujun_cn2007")
		g_Login.m_LoginAccounts:OnCtrlmsg(g_Login.m_LoginAccounts.m_btnOK,BUTTON_LCLICK,0,0)
		local strRet = WaitEvent(true,30000,Event.Test.LoginEnded)
		SetEvent(Event.Test.LoginEnded,false)
		if strRet[1] == EEventState.Success then
			
			if strRet[2][1] == "LoginSuccess" then
				
				if( g_App.m_re == EGameState.eToAgreement ) then
					g_UserAgreement.m_UserAgreementWnd:OnCtrlmsg(g_UserAgreement.m_UserAgreementWnd:GetDlgChild( "Btn_Agree" ),BUTTON_LCLICK,0,0)	
				end
				
				strRet = WaitEvent(true,30000,Event.Test.CharListReceived)
				SetEvent(Event.Test.CharListReceived,false)
				
				if strRet[1] == EEventState.Success then
					g_SelectChar.m_SelectCharWnd:OnCtrlmsg( g_SelectChar.m_SelectCharWnd:GetDlgChild( "Btn_Back" ), BUTTON_LCLICK, 0, 0 )
					local strRet = WaitEvent( true, nil, Event.Test.BackToLoginFinished )
					assert(strRet[1] == EEventState.Success)
					SetEvent( Event.Test.BackToLoginFinished, false )
					assert_equal( EGameState.eToLogin, g_App.m_re, "" )
				else
					assert_fail( "Login Failed SelectFail TimeOut" )
				end
				
			else
				assert_fail( "Login Failed Passwd error!" )
			end
			
		else
			assert_fail( "Login Failed Connect TimeOut" )
		end
		
	end
	
	function TestTest:teardown()
	end

end
