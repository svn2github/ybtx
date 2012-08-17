gas_require "framework/base_mgr/AccountMgr"

function InitGasAccountMgrTest()
	local TestTest=TestCase("test/testcase/GasAccountMgrTest")
	local Reload,Ret,Error
	
	function TestTest:setup()
	end
	
	function TestTest:TestNotHaveAccount()
		local AccountMgr = CAccountMgr:new()
		local bRet = AccountMgr:HaveAccount( "xupeng" )
		assert_equal(false,bRet,"" )
	end
	
	function TestTest:TestAccount()
		local AccountMgr = CAccountMgr:new()
		local bRet = AccountMgr:GetAccountInfo("xupeng")
		assert_equal(nil,bRet,"" )
		bRet = AccountMgr:CreateAccount( "xupeng",{"1234"} )
		assert_equal(true,bRet,"" )
		bRet = AccountMgr:CreateAccount( "xupeng",{"1234"} )
		assert_equal(false,bRet,"" )
		bRet = AccountMgr:GetAccountInfo( "xupeng")
		assert_equal("1234",bRet[1],"" )
		bRet = AccountMgr:SetAccountInfo( "xupeng",{"5678"})
		assert_equal(true,bRet,"" )
		bRet = AccountMgr:GetAccountInfo( "xupeng")
		assert_equal("5678",bRet[1],"" )
	end
	
	function TestTest:TestAccountArg()
		local AccountMgr = CAccountMgr:new()
		function func1()
			AccountMgr:CreateAccount( {},{"1234"} )
		end
		assert_error( "No Error", func1)
		
		function func2()
			AccountMgr:CreateAccount( nil,{"1234"} )
		end
		assert_error( "No Error", func2)
		
		function func3()
			AccountMgr:CreateAccount( "xupeng","1234" )
		end
		assert_error( "No Error", func3)
		
		function func4()
			AccountMgr:CreateAccount( "xupeng",nil )
		end
		assert_error( "No Error", func4)

		function func5()
			AccountMgr:HaveAccount( nil )
		end
		assert_error( "No Error", func5)
		
		function func6()
			AccountMgr:HaveAccount( 1 )
		end
		assert_error( "No Error", func6)
		
		function func7()
			AccountMgr:GetAccountInfo( 1 )
		end
		assert_error( "No Error", func7)

		function func8()
			AccountMgr:GetAccountInfo( nil )
		end
		assert_error( "No Error", func8)
		
		function func9()
			AccountMgr:SetAccountInfo( nil,{"1234"} )
		end
		assert_error( "No Error", func9)
		
		function func10()
			AccountMgr:SetAccountInfo( 1,{"1234"} )
		end
		assert_error( "No Error", func10)
		
		function func11()
			AccountMgr:SetAccountInfo( "xupeng","1234" )
		end
		assert_error( "No Error", func11)

		function func12()
			AccountMgr:SetAccountInfo( "xupeng",nil )
		end
		assert_error( "No Error", func12)
	end
	
	function TestTest:teardown()
	end
	
end