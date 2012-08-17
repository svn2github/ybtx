engine_require "common/Misc/TypeCheck"
--½ÇÉ«»Ö¸´²âÊÔÓÃÀý  
function InitGacTestCDG()
	local TestTest= TestCase("GacTestCDG")
	local controler = CTstLoginControler:new()
	
	function TestTest:setup()
	end
	function TestTest:test_GotoCreateRoleState()
		controler:Login( "test_SS", "1" )
	end
	--´´½¨½ÇÉ«Test
	function TestTest:test_CreateCharacter()
		local strName = "test_cd000"
		--controler:GotoCreateCharacter()
		assert_true( g_App.m_re == EGameState.eToSelectChar, "[all login state][create & del char][not in sc state]" )
		
		while controler:GetCharacterNum() >0 do
			controler:DelCharacter( 1 )
		end
		for i=1,5 do
			controler:CreateCharacter( strName .. i )
		end
		assert_equal( 5, controler:GetCharacterNum(), "[all login state][create 5]" )
	end
	--É¾³ý½ÇÉ«
	function TestTest:test_DeleteCharacter()
		for i = 1, #(g_SelectChar.m_tblRolesInfo.m_tblPlayerListGroupByActive) do
			controler:DelGetBackCharacter( 1 )
			local r_tbl = controler.__WaitEvent( true, nil, Event.Test.CharListReceived )
			assert( r_tbl[1] == EEventState.Success )
			local r_tbl2 = controler.__WaitEvent( true, nil, Event.Test.DelCharListReceived )
			assert( r_tbl2[1] == EEventState.Success )   
		end
	end
	--»Ö¸´½ÇÉ«  ( ²âÊÔÖÐÎª³¹µ×É¾³ý,²»ÄÜ»Ö¸´ )
	function TestTest:test_GetBackRoleCharacter()
		for i= 1, 5 do
			local wnd_select = g_SelectChar.m_SelectCharWnd
			local wnd_roles_info = wnd_select:GetDlgChild( "ColBox_SelectRole" )
			local result = wnd_roles_info:SelectItem( 4 )                    --select Item
			local index = wnd_roles_info:GetSelectItem( -1 ) + 1
			assert( g_App.m_re == EGameState.eToSelectChar )
			assert_true( g_App.m_re == EGameState.eToSelectChar )
			wnd_select:OnCtrlmsg( wnd_select:GetDlgChild( "Btn_DelRole" ), BUTTON_LCLICK, 0, 0 )
			local wnd_del = g_SelectChar.m_SelectCharWnd.m_wndDelRole
				
			assert_true(wnd_del:IsShow(),"[TestWndIsActive]")
			assert_equal("»Ö¸´É¾³ý", wnd_del:GetDlgChild("Wnd_Title"):GetWndText(), "»Ö¸´É¾³ý????")
			wnd_del:GetDlgChild("Input_VarifyWord"):SetWndText("yes")
			wnd_del:OnCtrlmsg( wnd_del:GetDlgChild( "Btn_Ok" ), BUTTON_LCLICK, 0, 0 )
			local r_tbl2 = controler.__WaitEvent( true, nil, Event.Test.CharListReceived )
			assert( r_tbl2[1] == EEventState.Success )
			local r_tbl3 = controler.__WaitEvent( true, nil, Event.Test.DelCharListReceived )
			assert( r_tbl3[1] == EEventState.Success )
		end
		controler:Back2Login()
	end
	
	function TestTest:DeleteAllCharacter()
		local strName = "test_SS"
		controler:Login( "test_SS", "1" )
		assert_true( g_App.m_re == EGameState.eToSelectChar, "[all login state][create & del char][not in sc state]" )
		for i=1, 5 do
			controler:DelCharacter( 1 )
			local r_tbl = controler.__WaitEvent( true, nil, Event.Test.CharListReceived )
			assert( r_tbl[1] == EEventState.Success )
			local r_tbl = controler.__WaitEvent( true, nil, Event.Test.DelCharListReceived )
			assert( r_tbl[2] == EEventState.Success )
		end
		controler:Back2Login()
	end
	function TestTest:teardown()
	end
end