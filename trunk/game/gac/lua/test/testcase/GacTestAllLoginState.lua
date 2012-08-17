gac_require "test/common/CTstLoginControler"

function InitGacTestAllLoginState( )
	local testcase_AllState=TestCase("TestAllLoginState")
	
	local controler = CTstLoginControler:new()
	function testcase_AllState:setup()
	end

--	function testcase_AllState:test_Create_DelRole()
--		local strName = "test_cd23"
--		local flag_man = 1
--		local account = "test_cd"
--		local password = "134" 
--		controler:Login( account, password )
--		
--		assert_true( g_App.m_re == EGameState.eToSelectChar, "[all login state][create & del char][not in sc state]" )
--		
--		if( controler:HasCharacter(strName) ) then
--				controler:DelCharacter( strName )
--		end
--		
--		controler:CreateCharacter( strName )
--		assert_true( controler:CheckHasNamAfterCreateRole( account, password, strName ), 
--						"[all login state][create & del char][create]" )
--	
--		controler:DelCharacter(strName)
--		assert_false( controler:HasCharacter( account, password, strName ), "[all login state][create & del char][delete]" )
--		
--		controler:Back2Login()
--	end
	
	function testcase_AllState:test_Create_DelManyRoles()
	
		local strName = "test_cdm"
		local account, password = "test_cdm", "134"
		controler:Login( account, password )
	
		assert_true( g_App.m_re == EGameState.eToSelectChar, "[all login state][create & del char][not in sc state]" )
		
		while controler:GetCharacterNum() >0 do
			controler:DelCharacter( 1 )
			local r_tbl1 = controler.__WaitEvent( true, nil, Event.Test.DelCharListReceived )
			assert( r_tbl1[1] == EEventState.Success )
		end
		
		for i=1,5 do
			controler:CreateManyRoles( strName .. i )
		end
				
		for i=1,5 do
--			print("ddddddd" .. i)
			controler:DelCharacter( i )
			local r_tbl = controler.__WaitEvent( true, nil, Event.Test.DelCharListReceived )
			assert( r_tbl[1] == EEventState.Success )
		end
		
		assert_equal( 0, controler:GetCharacterNum(), "[all login state][del 5]" )
		assert_true( g_App.m_re == EGameState.eToSelectChar, "[all login state][create & del char][not in sc state]" )
		
		controler:Back2Login()
	end
	
	function testcase_AllState:test_StateChange()
		controler:Login( "test_Jump", "jump" )		-- goto select char state
		local inputNewRoleInfo = 1
		assert_true( g_App.m_re == EGameState.eToSelectChar )
		controler:GotoCreateCharacter()
		assert_true( g_App.m_re == EGameState.eToNewRoleGameMain )
		controler:CC2SC_Cancel(inputNewRoleInfo)
		assert_true( g_App.m_re == EGameState.eToSelectChar )
		controler:GotoCreateCharacter()
		assert_true( g_App.m_re == EGameState.eToNewRoleGameMain )
		controler:CC2SC_Cancel(inputNewRoleInfo)
		assert_true( g_App.m_re == EGameState.eToSelectChar )
		controler:GotoCreateCharacter()
		assert_true( g_App.m_re == EGameState.eToNewRoleGameMain )
		controler:CC2SC_Cancel(inputNewRoleInfo)
		assert_true( g_App.m_re == EGameState.eToSelectChar )
		
		controler:Back2Login()
		assert_true( g_App.m_re == EGameState.eToLogin )

--[[

   


--]]	
	
	end
	
	function testcase_AllState:test_shutdown()
		controler:Login( "shutdown", "sd" )
		
		controler:DisConnect()
		
		controler:Login( "shutdown", "sd" )
		controler:GotoCreateCharacter()
		
		controler:DisConnect()
	end
	
	function testcase_AllState:teardown()
	end
end
