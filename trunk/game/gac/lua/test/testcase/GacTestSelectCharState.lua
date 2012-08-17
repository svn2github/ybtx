gac_require "test/common/CTstLoginControler"

function InitGacTestSelectCharState()
	local testcase_SelectCharState= TestCase("TestSelectCharState")
	local controler = CTstLoginControler:new()
	
	function testcase_SelectCharState:setup()
	end

	function testcase_SelectCharState:test_GUI()	
	
		controler:Login( "test_SC", "134" )
		
		--enter game
		--wnd_select:OnCtrlmsg( wnd_select:GetDlgChild( "Btn_EnterGame" ), BUTTON_LCLICK, 0, 0 )
		--assert_equal( EGameState.eToGameMain, g_App.m_re, "[select char][gui][enter game button]" )
		
		--delete character
		if controler:GetCharacterNum() == 0 then
			controler:CreateCharacter( "what" )
		end
		
		local wnd_select = g_SelectChar.m_SelectCharWnd
		local wnd_del_role = wnd_select.m_wndDelRole
    
		wnd_select:OnCtrlmsg( wnd_select:GetDlgChild( "Btn_DelRole" ), BUTTON_LCLICK, 0, 0 )
		assert_true( wnd_del_role:IsActive(), "[sc state][del role wnd]" )
	
--验证码框未输入验证码时(未输入验证码时和输入错的验证码后给出的提示框一样，有待改进)
   
	  wnd_del_role:OnCtrlmsg( wnd_del_role:GetDlgChild( "Btn_Ok" ), BUTTON_LCLICK, 0, 0 )
	  assert_false(wnd_del_role:IsActive(),"[sc state][del role wnd][ok][not set varifyWord]")

	 -- MessageBox( wnd_del_role, MsgBoxMsg(10002), MB_BtnOK )
--验证码框输入错误的验证码时		
    local m_inputVarifyWord = wnd_del_role:GetDlgChild("Input_VarifyWord")
    m_inputVarifyWord:SetWndText("aaaa")
    wnd_del_role:OnCtrlmsg( wnd_del_role:GetDlgChild( "Btn_Ok" ), BUTTON_LCLICK, 0, 0 )
 	  assert_false(wnd_del_role:IsActive(),"[sc state][del role wnd][ok][set wrong varifyWord]")
	  --MessageBox( wnd_del_role, MsgBoxMsg(10002), MB_BtnOK )
--输入正确的验证码后
    local  m_SerVarifyWord = wnd_del_role:GetDlgChild("Wnd_VarifyWord")
    m_inputVarifyWord:SetWndText("yes")
    wnd_del_role:OnCtrlmsg( wnd_del_role:GetDlgChild( "Btn_Ok" ), BUTTON_LCLICK, 0, 0 )
    assert_false( wnd_del_role:IsActive(), "[sc state][del role wnd][ok]" )
	
		wnd_del_role:OnCtrlmsg( wnd_del_role:GetDlgChild( "Btn_Cancel" ), BUTTON_LCLICK, 0, 0 )
		assert_false( wnd_del_role:IsActive(), "[sc state][del role wnd][cancel]" )
		
		--create character
		wnd_select = g_SelectChar.m_SelectCharWnd
		wnd_select:OnCtrlmsg( wnd_select:GetDlgChild( "AiMiNewRole" ), BUTTON_LCLICK, 0, 0 )
		assert_equal( EGameState.eToNewRoleGameMain, g_App.m_re, "[sc state][new role][go]")
		
		local wnd_cr = g_NewRole.m_Wnd
		wnd_cr:OnCtrlmsg( wnd_cr:GetDlgChild( "Btn_Back" ), BUTTON_LCLICK, 0, 0 )
		assert_equal( EGameState.eToSelectChar, g_App.m_re, "[sc state][new role][back]")
		
		local r_tbl = WaitEvent(true,30000,Event.Test.CharListReceived)
		SetEvent(Event.Test.CharListReceived,false)
		assert( r_tbl[1] == EEventState.Success )
		
		controler:Back2Login()
	end
	
	function testcase_SelectCharState:test_shutdown()
	
		local Account	= "test_SC"
		local Password	= "134"
		controler:Login( Account, Password )
		
		if controler:GetCharacterNum() == 0 then
			controler:CreateCharacter( "what" )
		end
		
		controler:SelectCharacter( "what" )
		
		--弹出删除角色对话框时断开
		local wnd_select = g_SelectChar.m_SelectCharWnd
		local wnd_del_role = wnd_select.m_wndDelRole
		wnd_select:OnCtrlmsg( wnd_select:GetDlgChild( "Btn_DelRole" ), BUTTON_LCLICK, 0, 0 )
		
		controler:DisConnect()
		
		--刚登陆，角色数据还没取到时，网络断开
		local login_state = g_Login
		assert_true( g_App.m_re == EGameState.eToLogin )
	
		local Wnd_Account = SQREdit, login_state.m_LoginAccounts:GetDlgChild( "Edit_UserName" ) 
		Wnd_Account:SetWndText( Account )
	
		local Wnd_Password = SQREdit, login_state.m_LoginAccounts:GetDlgChild( "Edit_Password" ) 
		Wnd_Password:SetWndText(Password)
		login_state.m_LoginAccounts.m_editPassword:SetWndText(Password)
		login_state.m_LoginAccounts:OnCtrlmsg(login_state.m_LoginAccounts:GetDlgChild( "Btn_OK" ),BUTTON_LCLICK,0,0)
		
		SetEvent( Event.Test.LoginEnded, false )
		local r_tbl = WaitEvent( true, nil, Event.Test.LoginEnded )
		SetEvent( Event.Test.LoginEnded, false )
		assert( r_tbl[1] == EEventState.Success )
		assert_true( g_App.m_re == EGameState.eToSelectChar )
	
		controler:DisConnect()
	end
	
	function testcase_SelectCharState:test_MsgBox()
		local Account	= "test_SC_MB"
		local Password	= "134"
		local strName 	= "test_sc_MB"
		controler:Login( Account, Password )
		
		local state_select = g_SelectChar
		local wnd_select = g_SelectChar.m_SelectCharWnd
		
		local function Click_MsgBox_OKBtn()
			state_select = g_SelectChar
			assert( state_select.m_MsgBox )
			local wnd = state_select.m_MsgBox:GetDlgChild("BtnOK")
			assert( wnd )
	
			assert( getmetatable(wnd) ~= nil )
			wnd:SendMsg( WM_LBUTTONDOWN, 0, 0 )
			wnd:SendMsg( WM_LBUTTONUP, 0, 0 )
		end
		
		local function Check_MsgBox_Msg( expect_str )
			state_select = g_SelectChar
			assert( state_select.m_MsgBox )
			local wnd = state_select.m_MsgBox:GetDlgChild("WndInfo")
			assert( wnd )
			
			assert_equal( wnd:GetWndText(), expect_str )
		end
		
		--删除已有角色
		while controler:GetCharacterNum() >0 do
			controler:DelCharacter( 1 )
		end
		
		wnd_select:OnCtrlmsg( wnd_select:GetDlgChild( "Btn_DelRole" ), BUTTON_LCLICK, 0, 0 )
		Check_MsgBox_Msg( CSelectCharState.SelectCharConfig.strNoRole )
		Click_MsgBox_OKBtn()
		
	
		wnd_select:OnCtrlmsg( wnd_select:GetDlgChild( "Btn_EnterGame" ), BUTTON_LCLICK, 0, 0 )
		Check_MsgBox_Msg( CSelectCharState.SelectCharConfig.strNoRole )
		Click_MsgBox_OKBtn()
		
		--创建max个角色
		for i=1,CSelectCharState.SelectCharConfig.nMaxRole do
			controler:CreateCharacter( strName .. i )
		end
		
		wnd_select = g_SelectChar.m_SelectCharWnd
		wnd_select:OnCtrlmsg( wnd_select:GetDlgChild( "AiMiNewRole" ), BUTTON_LCLICK, 0, 0 )
		Check_MsgBox_Msg( CSelectCharState.SelectCharConfig.strRoleNumReachMax )
		Click_MsgBox_OKBtn()
		
--		--选择一个不存在的角色
--		controler:SelectCharacter( CSelectCharState.SelectCharConfig.nMaxRole +10 )
--		
--		wnd_select:OnCtrlmsg( wnd_select:GetDlgChild( "Btn_DelRole" ), BUTTON_LCLICK, 0, 0 )
--		Check_MsgBox_Msg( CSelectCharState.SelectCharConfig.strNoSelectRole )
--		Click_MsgBox_OKBtn()
--		
--		wnd_select:OnCtrlmsg( wnd_select:GetDlgChild( "Btn_EnterGame" ), BUTTON_LCLICK, 0, 0 )
--		Check_MsgBox_Msg( CSelectCharState.SelectCharConfig.strNoSelectRole )
--		Click_MsgBox_OKBtn()	
		
		--删除所有角色
		while controler:GetCharacterNum() >0 do
			controler:DelCharacter( 1 )
		end
		
		controler:Back2Login()
	end

	function testcase_SelectCharState:teardown()
	end
end
