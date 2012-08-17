engine_require "common/Misc/TypeCheck"

function InitGacTestCreateRole( )
	local TestTest= TestCase("TestCreateRole")
	local controler = CTstLoginControler:new()
	
	function TestTest:setup()
	end
	
	function TestTest:test_GotoCreateRoleState()
		controler:Login( "test_cc", "134" )
		controler:GoToInputNewRoleInfo()
	end
	
	--测试角色名是否合法(4-12个英文字符，或2-6个汉字)
	function TestTest:TestCheckRoleName()
		assert_true( EGameState.eToNewRoleGameMain == g_App.m_re )
		assert_equal(false, CheckRoleName("123"), "")
		assert_equal(false, CheckRoleName("吴"), "")
		assert_equal(true, CheckRoleName("1234"), "")
		assert_equal(true, CheckRoleName("吴俊"), "")
		assert_equal(true, CheckRoleName("123456789012"), "")
		assert_equal(true, CheckRoleName("吴俊真是好人"), "")
		assert_equal(true, CheckRoleName("1234567890123"), "")
		assert_equal(true, CheckRoleName("吴俊真是好人啊"), "")
		assert_equal(true, CheckRoleName("吴gg"), "")
		assert_equal(false, CheckRoleName("吴g"), "")	
		assert_equal(true, CheckRoleName("吴俊是好人!!"), "")
		assert_equal(true, CheckRoleName("吴俊是好人!!!"), "")
	end

	--按选择发型右按钮
	function TestTest:TestClickHairStyleRight()
		assert_true( EGameState.eToNewRoleGameMain == g_App.m_re )
		local i
		if(g_GameMain.m_NewRole.m_nSex == 1) then
			for i = 1, table.getn(g_GameMain.m_NewRole.m_HairStyle)-1, 1 do
				g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole.m_btnStyleRight,BUTTON_LCLICK,0,0)
				assert_equal(i+1, g_GameMain.m_NewRole.m_nHairStyleSelIdx[1], "")
	  	end
			for i = 0, table.getn(g_GameMain.m_NewRole.m_HairStyle)-1, 1 do
				g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole.m_btnStyleRight,BUTTON_LCLICK,0,0)
				assert_equal(i+1, g_GameMain.m_NewRole.m_nHairStyleSelIdx[1], "")
	  	end
		else
			for i = 1, table.getn(g_GameMain.m_NewRole.m_WomanHairStyle)-1, 1 do
				g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole.m_btnStyleRight,BUTTON_LCLICK,0,0)
				assert_equal(i+1, g_GameMain.m_NewRole.m_nHairStyleSelIdx[2], "")
	  	end
			for i = 0, table.getn(g_GameMain.m_NewRole.m_WomanHairStyle)-1, 1 do
				g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole.m_btnStyleRight,BUTTON_LCLICK,0,0)
				assert_equal(i+1, g_GameMain.m_NewRole.m_nHairStyleSelIdx[2], "")
	  	end
		end
	end
	--按选择发型左按钮
	function TestTest:TestClickHairStyleLeft()
		assert_true( EGameState.eToNewRoleGameMain == g_App.m_re )
		local i
		if(g_GameMain.m_NewRole.m_nSex == 1) then
			for i = g_GameMain.m_NewRole.m_nHairStyleSelIdx[1]-1, 1, -1 do
				g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole.m_btnStyleLeft,BUTTON_LCLICK,0,0)
				assert_equal(i, g_GameMain.m_NewRole.m_nHairStyleSelIdx[1], "")
	  	end
			g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole.m_btnStyleLeft,BUTTON_LCLICK,0,0)
			assert_equal(table.getn(g_GameMain.m_NewRole.m_HairStyle), g_GameMain.m_NewRole.m_nHairStyleSelIdx[1], "")
		else
			for i = g_GameMain.m_NewRole.m_nHairStyleSelIdx[2]-1, 1, -1 do
				g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole.m_btnStyleLeft,BUTTON_LCLICK,0,0)
				assert_equal(i, g_GameMain.m_NewRole.m_nHairStyleSelIdx[2], "")
	  	end
			g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole.m_btnStyleLeft,BUTTON_LCLICK,0,0)
			assert_equal(table.getn(g_GameMain.m_NewRole.m_WomanHairStyle), g_GameMain.m_NewRole.m_nHairStyleSelIdx[2], "")
		end
	end
	--按选择发色右按钮
	function TestTest:TestClickHairColorRight()
		assert_true( EGameState.eToNewRoleGameMain == g_App.m_re )
		local i
		if(g_GameMain.m_NewRole.m_nSex == 1) then
			for i = 1, table.getn(g_GameMain.m_NewRole.m_ManHairColor)-1, 1 do
				g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole.m_btnColorRight,BUTTON_LCLICK,0,0)
				assert_equal(i+1, g_GameMain.m_NewRole.m_nHairColorSelIdx[1], "")
	  	end
			for i = 0, table.getn(g_GameMain.m_NewRole.m_ManHairColor)-1, 1 do
				g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole.m_btnColorRight,BUTTON_LCLICK,0,0)
				assert_equal(i+1, g_GameMain.m_NewRole.m_nHairColorSelIdx[1], "")
	  	end
		else
			for i = 1, table.getn(g_GameMain.m_NewRole.m_WomanHairColor)-1, 1 do
				g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole.m_btnColorRight,BUTTON_LCLICK,0,0)
				assert_equal(i+1, g_GameMain.m_NewRole.m_nHairColorSelIdx[2], "")
	  	end
			for i = 0, table.getn(g_GameMain.m_NewRole.m_WomanHairColor)-1, 1 do
				g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole.m_btnColorRight,BUTTON_LCLICK,0,0)
				assert_equal(i+1, g_GameMain.m_NewRole.m_nHairColorSelIdx[2], "")
	  	end
		end	
	end
	--按选择发色左按钮
	function TestTest:TestClickHairColorLeft()
		assert_true( EGameState.eToNewRoleGameMain == g_App.m_re )
		local i
		if(g_GameMain.m_NewRole.m_nSex == 1) then
			for i = g_GameMain.m_NewRole.m_nHairColorSelIdx[1]-1, 1, -1 do
				g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole.m_btnColorLeft,BUTTON_LCLICK,0,0)
				assert_equal(i, g_GameMain.m_NewRole.m_nHairColorSelIdx[1], "")
	  	end
			g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole.m_btnColorLeft,BUTTON_LCLICK,0,0)
			assert_equal(table.getn(g_GameMain.m_NewRole.m_ManHairColor), g_GameMain.m_NewRole.m_nHairColorSelIdx[1], "")
		else
			for i = g_GameMain.m_NewRole.m_nHairColorSelIdx[2]-1, 1, -1 do
				g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole.m_btnColorLeft,BUTTON_LCLICK,0,0)
				assert_equal(i, g_GameMain.m_NewRole.m_nHairColorSelIdx[2], "")
	  	end
			g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole.m_btnColorLeft,BUTTON_LCLICK,0,0)
			assert_equal(table.getn(g_GameMain.m_NewRole.m_WomanHairColor), g_GameMain.m_NewRole.m_nHairColorSelIdx[2], "")
		end
	end
	--按选择脸型右按钮
	function TestTest:TestClickFaceRight()
		assert_true( EGameState.eToNewRoleGameMain == g_App.m_re )
		local i
		if(g_GameMain.m_NewRole.m_nSex == 1) then
			for i = 1, table.getn(g_GameMain.m_NewRole.m_Face)-1, 1 do
				g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole.m_btnFaceRight,BUTTON_LCLICK,0,0)
				assert_equal(i+1, g_GameMain.m_NewRole.m_nFaceSelIdx[1], "")
	  	end
			for i = 0, table.getn(g_GameMain.m_NewRole.m_Face)-1, 1 do
				g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole.m_btnFaceRight,BUTTON_LCLICK,0,0)
				assert_equal(i+1, g_GameMain.m_NewRole.m_nFaceSelIdx[1], "")
	  	end
		else
			for i = 1, table.getn(g_GameMain.m_NewRole.m_WomanFace)-1, 1 do
				g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole.m_btnFaceRight,BUTTON_LCLICK,0,0)
				assert_equal(i+1, g_GameMain.m_NewRole.m_nFaceSelIdx[2], "")
	  	end
			for i = 0, table.getn(g_GameMain.m_NewRole.m_WomanFace)-1, 1 do
				g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole.m_btnFaceRight,BUTTON_LCLICK,0,0)
				assert_equal(i+1, g_GameMain.m_NewRole.m_nFaceSelIdx[2], "")
	  	end
		end	
	end
	--按选择脸型左按钮
	function TestTest:TestClickFaceLeft()
		assert_true( EGameState.eToNewRoleGameMain == g_App.m_re )
		local i
		if(g_GameMain.m_NewRole.m_nSex == 1) then
			for i = g_GameMain.m_NewRole.m_nFaceSelIdx[1]-1, 1, -1 do
				g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole.m_btnFaceLeft,BUTTON_LCLICK,0,0)
				assert_equal(i, g_GameMain.m_NewRole.m_nFaceSelIdx[1], "")
	  	end
			g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole.m_btnFaceLeft,BUTTON_LCLICK,0,0)
			assert_equal(table.getn(g_GameMain.m_NewRole.m_Face), g_GameMain.m_NewRole.m_nFaceSelIdx[1], "")
		else
			for i = g_GameMain.m_NewRole.m_nFaceSelIdx[2]-1, 1, -1 do
				g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole.m_btnFaceLeft,BUTTON_LCLICK,0,0)
				assert_equal(i, g_GameMain.m_NewRole.m_nFaceSelIdx[2], "")
	  	end
			g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole.m_btnFaceLeft,BUTTON_LCLICK,0,0)
			assert_equal(table.getn(g_GameMain.m_NewRole.m_WomanFace), g_GameMain.m_NewRole.m_nFaceSelIdx[2], "")
		end
	end
	--按选择身材比例右按钮
	function TestTest:TestClickBodyRight()
		assert_true( EGameState.eToNewRoleGameMain == g_App.m_re )
		local i
		if(g_GameMain.m_NewRole.m_nSex == 1) then
			for i = 1, table.getn(g_GameMain.m_NewRole.m_Body)-1, 1 do
				g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole.m_btnBodyRight,BUTTON_LCLICK,0,0)
				assert_equal(i+1, g_GameMain.m_NewRole.m_nBodySelIdx[1], "")
	  	end
			for i = 0, table.getn(g_GameMain.m_NewRole.m_Body)-1, 1 do
				g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole.m_btnBodyRight,BUTTON_LCLICK,0,0)
				assert_equal(i+1, g_GameMain.m_NewRole.m_nBodySelIdx[1], "")
	  	end
		else
			for i = 1, table.getn(g_GameMain.m_NewRole.m_Body)-1, 1 do
				g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole.m_btnBodyRight,BUTTON_LCLICK,0,0)
				assert_equal(i+1, g_GameMain.m_NewRole.m_nBodySelIdx[2], "")
	  	end
			for i = 0, table.getn(g_GameMain.m_NewRole.m_Body)-1, 1 do
				g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole.m_btnBodyRight,BUTTON_LCLICK,0,0)
				assert_equal(i+1, g_GameMain.m_NewRole.m_nBodySelIdx[2], "")
	  	end
		end	
	end
	--按选择身材比例左按钮
	function TestTest:TestClickBodyLeft()
		assert_true( EGameState.eToNewRoleGameMain == g_App.m_re )
		local i
		if(g_GameMain.m_NewRole.m_nSex == 1) then
			for i = g_GameMain.m_NewRole.m_nBodySelIdx[1]-1, 1, -1 do
				g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole.m_btnBodyLeft,BUTTON_LCLICK,0,0)
				assert_equal(i, g_GameMain.m_NewRole.m_nBodySelIdx[1], "")
	  	end
			g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole.m_btnBodyLeft,BUTTON_LCLICK,0,0)
			assert_equal(table.getn(g_GameMain.m_NewRole.m_Body), g_GameMain.m_NewRole.m_nBodySelIdx[1], "")
		else
			for i = g_GameMain.m_NewRole.m_nBodySelIdx[2]-1, 1, -1 do
				g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole.m_btnBodyLeft,BUTTON_LCLICK,0,0)
				assert_equal(i, g_GameMain.m_NewRole.m_nBodySelIdx[2], "")
	  	end
			g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole.m_btnBodyLeft,BUTTON_LCLICK,0,0)
			assert_equal(table.getn(g_GameMain.m_NewRole.m_Body), g_GameMain.m_NewRole.m_nBodySelIdx[2], "")
		end
	end
	

  --[[--测试选择天赋
  function TestTest:TestSelectInherence()
  		assert_true( EGameState.eToNewRoleGameMain == g_App.m_re )
  		g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole:GetDlgChild( "Chk_Genius01" ),BUTTON_LCLICK,0,0)
		assert_equal(1, g_GameMain.m_NewRole.m_nInherence, "")
		g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole:GetDlgChild( "Chk_Genius02" ),BUTTON_LCLICK,0,0)
		assert_equal(2, g_GameMain.m_NewRole.m_nInherence, "")
		g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole:GetDlgChild( "Chk_Genius03" ),BUTTON_LCLICK,0,0)
		assert_equal(3, g_GameMain.m_NewRole.m_nInherence, "")
		g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole:GetDlgChild( "Chk_Genius04" ),BUTTON_LCLICK,0,0)
		assert_equal(4, g_GameMain.m_NewRole.m_nInherence, "")
		g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole:GetDlgChild( "Chk_Genius05" ),BUTTON_LCLICK,0,0)
		assert_equal(5, g_GameMain.m_NewRole.m_nInherence, "")
		g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole:GetDlgChild( "Chk_Genius06" ),BUTTON_LCLICK,0,0)
		assert_equal(6, g_GameMain.m_NewRole.m_nInherence, "")
		g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole:GetDlgChild( "Chk_Genius07" ),BUTTON_LCLICK,0,0)
		assert_equal(7, g_GameMain.m_NewRole.m_nInherence, "")
		g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole:GetDlgChild( "Chk_Genius08" ),BUTTON_LCLICK,0,0)
		assert_equal(8, g_GameMain.m_NewRole.m_nInherence, "")
		g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole:GetDlgChild( "Chk_Genius09" ),BUTTON_LCLICK,0,0)
		assert_equal(9, g_GameMain.m_NewRole.m_nInherence, "")
		g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole:GetDlgChild( "Chk_Genius10" ),BUTTON_LCLICK,0,0)
		assert_equal(10, g_GameMain.m_NewRole.m_nInherence, "")
		g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole:GetDlgChild( "Chk_Genius11" ),BUTTON_LCLICK,0,0)
		assert_equal(11, g_GameMain.m_NewRole.m_nInherence, "")
		g_GameMain.m_NewRole:OnCtrlmsg(g_GameMain.m_NewRole:GetDlgChild( "Chk_Genius12" ),BUTTON_LCLICK,0,0)
		assert_equal(12, g_GameMain.m_NewRole.m_nInherence, "")
	end
	--]]
	function TestTest:test_ShutDown()
		assert_true( EGameState.eToNewRoleGameMain == g_App.m_re )
		local selectClass = 2
		controler:CC2SC_Cancel(selectClass)
		assert_true( g_App.m_re == EGameState.eToSelectChar )
		
		local name = "shutdown"
		
		if( controler:HasCharacter(name) ) then
				controler:DelCharacter( name )
		end

		controler:GoToInputNewRoleInfo()
    	
		--create character
		local wnd_cr = g_GameMain.m_NewRole
		wnd_cr:GetDlgChild( "Edit_Name" ):SetWndText( name )

		wnd_cr:OnCtrlmsg( wnd_cr:GetDlgChild( "Btn_OK" ), BUTTON_LCLICK, 0, 0 )
		
		controler:DisConnect()
	end
	
	function TestTest:teardown()
	end
end

