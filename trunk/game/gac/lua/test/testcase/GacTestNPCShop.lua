gac_require "test/common/CTstLoginControler"

function InitGacTestNPCShop()
	local test_NPCShop = TestCase("GacTestNPCShop")
	--进入操作
	function test_NPCShop:TestBegin()
		controler = CTstLoginControler:new()
		controler:OneStepLogin()
	end
	
	--NPC商店购买窗口及功能测试
	function test_NPCShop:test_NPCShopSell()
		local m_NPCShopSellWnd 	= g_GameMain.m_NPCShopSell
		local tbl 				= m_NPCShopSellWnd.m_NPCShopGoodsList       --NPC商店所卖物品列表
	  	local tblPlayerSold		= g_GameMain.m_NPCShopGoodsList        		--玩家所卖物品列表
	  	local sellGoodListWnd	= m_NPCShopSellWnd:GetDlgChild("ItemList")
		local m_NPCShopPlayerSoldWnd = g_GameMain.m_NPCShopPlayerSold
	 
		--现在是通过GM指令打开的
		Gac2Gas:GM_Execute( g_Conn, "$openpanel(3)" )
    	local strRet = WaitEvent( true, nil, Event.Test.OpenNpcSellWnd )
		SetEvent( Event.Test.OpenNpcSellWnd, false )
    
  	 	local m_MainBagWnd = g_GameMain.m_WndMainBag
  	 	assert_true( m_NPCShopSellWnd:IsShow() )
  	 	assert_false( m_NPCShopPlayerSoldWnd:IsShow() )
  	 	assert_true( m_MainBagWnd:IsShow() ) 
  	 	assert_true( m_MainBagWnd:IsShow() )
  	 	
  	 	--测试NPC商店购买窗口的关闭按钮功能
  	 	m_NPCShopSellWnd:OnCtrlmsg( m_NPCShopSellWnd:GetDlgChild("Close"), BUTTON_LCLICK, 0, 0 )
  	 	assert_false( m_NPCShopSellWnd:IsShow() )
  	 	assert_false( m_MainBagWnd:IsShow() )
  	 	m_NPCShopSellWnd:ShowWnd( true )
  	 	m_MainBagWnd:ShowWnd( true )
  	 	
  	 	local function Check_MsgBox_Msg( expect_str )
  	 	  state_m_NPCShopSellWnd = g_GameMain.m_NPCShopSell
  	 		assert( state_m_NPCShopSellWnd.m_MsgBox )
  	 		local wnd = state_m_NPCShopSellWnd.m_MsgBox:GetDlgChild("WndInfo")
  	 		assert( wnd )
  	 	
  	 		assert_equal( wnd:GetWndText(), expect_str )
  	 	end
  	 	local function Click_MsgBox_OKBtn()
  	 		state_m_NPCShopSellWnd = g_GameMain.m_NPCShopSell
  	 		assert( state_m_NPCShopSellWnd.m_MsgBox )
  	 		local wnd = state_m_NPCShopSellWnd.m_MsgBox:GetDlgChild("BtnOK")
  	 		assert( wnd )
  	 	 
  	 		assert( getmetatable(wnd) ~= nil )
  	 		wnd:SendMsg( WM_LBUTTONDOWN, 0, 0 )
  	 		wnd:SendMsg( WM_LBUTTONUP, 0, 0 )
  	 	
  	 	end  
  	 	local function Click_MsgBox_CancelBtn()
  	 		state_m_NPCShopSellWnd = g_GameMain.m_NPCShopSell
  	 		assert( state_m_NPCShopSellWnd.m_MsgBox )
  	 		local wnd = state_m_NPCShopSellWnd.m_MsgBox:GetDlgChild("BtnCancel")
  	 		assert( wnd )
  	 	
  	 		assert( getmetatable(wnd) ~= nil )
  	 		wnd:SendMsg( WM_LBUTTONDOWN, 0, 0 )
  	 		wnd:SendMsg( WM_LBUTTONUP, 0, 0 )
  	 	end 
  	 	
  	 	--没有足够金钱买所选物品时
  	 	local btn  = sellGoodListWnd:GetSubItem(0, 0)		--购买售出列表中的第一个物品
  	 	m_NPCShopSellWnd:OnCtrlmsg( btn, ITEM_RBUTTONCLICK,  0, 0 )
  	 	Check_MsgBox_Msg("金币不足，选择另种方式否？")
  	 	Click_MsgBox_CancelBtn()
  	 	
  	 	m_NPCShopSellWnd:OnCtrlmsg(  btn, ITEM_RBUTTONCLICK, 0, 0 )
  	 	Check_MsgBox_Msg("金币不足，选择另种方式否？")
  	 	Click_MsgBox_OKBtn()
  	 	Check_MsgBox_Msg( "没有足够的金币购买该物品" )
  	 	Click_MsgBox_OKBtn()
																
			SetEvent(Event.Test.AddMoneyGM, false)
  	 	--绑定金币数目不够,而流通金币足以购买所选物品时,右键选中NPC商店中的商品直接购买  	 	
  	 	Gac2Gas:GM_Execute( g_Conn, "$addbindingmoney(10000000)" )
  	 	
  	 	local strRet = WaitEvent(true,nil,Event.Test.AddBindingMoneyGM)
  	 	
																	
  	 	m_NPCShopSellWnd:OnCtrlmsg(  btn, ITEM_RBUTTONCLICK, 0, 0 )
  	 	
  	 	local strRet = WaitEvent(true, nil, Event.Test.ChangedMoneyCount)
  	 	
  	 	if strRet[1] == EEventState.Success then
  	 		if strRet[2][1] == true then
  	 			local Grid = m_MainBagWnd.m_ctItemRoom:GetGridByIndex( 0, 0, 1 )
  	 			local num = Grid:Size()
					assert_true( num == 1 )
					assert_true( g_MainPlayer.m_nBindMoney < 10000000)
				end
  	 	end		
  	 	SetEvent(Event.Test.ChangedMoneyCount, false)
  	
  		--绑定金币数目足以购买算选物品,左键选中物品拖动到包裹栏第一个格子
  		 m_NPCShopSellWnd:OnCtrlmsg( btn, ITEM_RBUTTONCLICK, 0, 0 )
  		 m_MainBagWnd:OnCtrlmsg( m_MainBagWnd.m_ctItemRoom, ITEM_LBUTTONCLICK , 0, 0 )
  		 local strRet = WaitEvent(true, nil, Event.Test.ChangedMoneyCount)
  		 if strRet[1] == EEventState.Success then
			if strRet[2][1] == true then
   		  		local Grid = m_MainBagWnd.m_ctItemRoom:GetGridByIndex( 0, 0, 1 )
   		  		local num = Grid:Size()
    	    	assert_true( num == 1)
    	    	assert_true( g_MainPlayer.m_nBindMoney < 10000000 )
			end
    	end		
    	SetEvent(Event.Test.ChangedMoneyCount, false)
    	
		 
		--批量购买测试用例
			m_NPCShopSellWnd:OnCtrlmsg( m_NPCShopSellWnd:GetDlgChild("BuyMany_Btn"), BUTTON_LCLICK,  0, 0 )
			m_NPCShopSellWnd:OnCtrlmsg( btn, ITEM_LBUTTONCLICK,  0, 0 )
    	assert_true( m_NPCShopSellWnd.m_buyBatchWnd:IsShow() )
    	
    	m_NPCShopSellWnd:OnCtrlmsg(btn, BUTTON_LCLICK,  0, 0 )
    	assert_true( m_NPCShopSellWnd.m_buyBatchWnd:IsShow() )
    	m_NPCShopSellWnd.m_buyBatchWnd:GetDlgChild( "EditMoney" ):SetWndText( 3 )
    	m_NPCShopSellWnd.m_buyBatchWnd:OnCtrlmsg( m_NPCShopSellWnd.m_buyBatchWnd:GetDlgChild("BtnOk"), BUTTON_LCLICK, 0, 0 )
    	m_MainBagWnd:OnCtrlmsg( m_MainBagWnd.m_ctItemRoom, ITEM_LBUTTONCLICK , 0, 1 )
    	
    	local strRet = WaitEvent( true, nil, Event.Test.ChangedMoneyCount )
    	if strRet[1] == EEventState.Success then
    		if strRet[2][1] == true then
    			local Grid = m_MainBagWnd.m_ctItemRoom:GetGridByIndex( 0, 1, 1)
    			local num = Grid:Size()
					assert_true( num == 1 )
				end
    	end		
		SetEvent(Event.Test.ChangedMoneyCount,false)

		--NPC窗口中"购回"按钮功能的测试
		m_NPCShopSellWnd:OnCtrlmsg(  m_NPCShopSellWnd:GetDlgChild("BuyBack"), BUTTON_LCLICK, 0, 0 )
		assert_false( m_NPCShopSellWnd:IsShow() )
		assert_true( m_NPCShopPlayerSoldWnd:IsShow() )
	end
  
  
	--NPC商店购回窗口及功能测试
	function test_NPCShop:test_NPCShopPlayerSold()
    	local tbl = g_GameMain.m_PlayerSoldGoodsList
    	--现在是通过GM指令打开的
		
  		local m_NPCShopSellWnd = g_GameMain.m_NPCShopSell
  		local m_NPCShopPlayerSoldWnd = g_GameMain.m_NPCShopPlayerSold  
  		local m_MainBagWnd = g_GameMain.m_WndMainBag
			m_NPCShopPlayerSoldWnd:OnCtrlmsg(  m_NPCShopPlayerSoldWnd:GetDlgChild("Buy"), BUTTON_LCLICK, 0, 0 )
    	assert_true( m_NPCShopSellWnd:IsShow() )
			assert_false( m_NPCShopPlayerSoldWnd:IsShow() )
    	m_NPCShopSellWnd:ShowWnd(false)
    	m_NPCShopPlayerSoldWnd:ShowWnd( true )
    	
    	--从物品栏中拖出一个物品放入NPC购回窗口,出售手中物品
    	local itemBtn  = m_NPCShopPlayerSoldWnd:GetDlgChild("Goods_01")		--购买售出列表中的第一个物品
    	m_MainBagWnd:OnCtrlmsg(m_MainBagWnd.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 2  )
    	m_NPCShopPlayerSoldWnd:OnCtrlmsg(itemBtn, BUTTON_LCLICK, 0, 0 )
    	local strRet = WaitEvent( true, nil, Event.Test.ChangedMoneyCount )

	  	SetEvent( Event.Test.ChangedMoneyCount, false )
	  	if strRet == EEventState.Success then
				if strRet[2][1] == true then
	  		  local Grid = m_MainBagWnd.m_ctItemRoom:GetGridByIndex( 0, 2, 1 )
					local num = Grid:Size()
					assert_true( num == 0 )
					assert_equal( table.maxn(tbl), 1 )
      		end
    	end
   
			--从物品栏中拖出一个物品放入NPC购回窗口中已有玩家售出物品的窗口,购回此窗口中的物品
			m_MainBagWnd:OnCtrlmsg( m_MainBagWnd.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 0  )
			m_NPCShopPlayerSoldWnd:OnCtrlmsg( itemBtn, BUTTON_LCLICK, 0, 0 )
			local strRet = WaitEvent( true, nil,  Event.Test.ChangedMoneyCount )

			SetEvent( Event.Test.ChangedMoneyCount, false )
			if strRet == EEventState.Success then
				if strRet[2][1] == true then
					local Grid = m_MainBagWnd.m_ctItemRoom:GetGridByIndex( 0, 2, 1 )
					local num = Grid:Size()
					assert_true( num == 0 )
					assert_equal( table.maxn(tbl), 0 )
				end
    	end
    
    	local itemBtn2 = m_NPCShopPlayerSoldWnd:GetDlgChild("Goods_02")		--购买售出列表中的第二个物品
   		--购回物品之后,继续出售玩家选中出售的物品
   		m_NPCShopPlayerSoldWnd:OnCtrlmsg( itemBtn2, BUTTON_RIGHTCLICKDOWN, 0, 0 )
   			
   		local strRet = WaitEvent( true, nil, Event.Test.ChangedMoneyCount )
			SetEvent( Event.Test.ChangedMoneyCount, false )
			if strRet == EEventState.Success then
				if strRet[2][1] == true then	      
					local Grid = m_MainBagWnd.m_ctItemRoom:GetGridByIndex( 0, 2, 1 )
					local num = Grid:Size()
					assert_true( num == 1 )
					assert_equal( table.maxn(tbl),1 )
				end
			end
   		 
		m_MainBagWnd:OnCtrlmsg( m_MainBagWnd.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 0  )
		m_NPCShopPlayerSoldWnd:OnCtrlmsg( itemBtn, BUTTON_LCLICK, 0, 0 )
		local strRet = WaitEvent( true, nil, Event.Test.ChangedMoneyCount )
		SetEvent( Event.Test.ChangedMoneyCount, false )
		if strRet == EEventState.Success then
			if strRet[2][1] == true then
				local Grid = m_MainBagWnd.m_ctItemRoom:GetGridByIndex( 0, 1, 1 )
				local num = Grid:Size()
				assert_true( num == 0 )
				assert_equal( table.maxn(tbl), 2)
			end
		end

    	--全部购回玩家所卖物品
    	m_NPCShopPlayerSoldWnd:OnCtrlmsg( m_NPCShopPlayerSoldWnd:GetDlgChild("BuyBackAll"), BUTTON_LCLICK, 0, 0 )
    	local strRet = WaitEvent( true, nil, Event.Test.ChangedMoneyCount )
		SetEvent( Event.Test.ChangedMoneyCount, false )
		if strRet == EEventState.Success then
			if strRet[2][1] == true then
				local Grid = m_MainBagWnd.m_ctItemRoom:GetGridByIndex( 0, 1, 1 )
				local num = Grid:Size()
				assert_true( num == 3 )
				local Grid = m_MainBagWnd.m_ctItemRoom:GetGridByIndex( 0, 2, 1 )
				local num = Grid:Size()
				assert_true( num == 3 )
				assert_equal( table.maxn(tbl), 0 )
			end
		end
    
    	--NPC商店购回窗口中的"Buy"(商人)按钮功能的测试
    	local m_BuyBtn = m_NPCShopPlayerSoldWnd:GetDlgChild("Buy")    
    	m_NPCShopPlayerSoldWnd:OnCtrlmsg( m_BuyBtn, BUTTON_LCLICK, 0, 0 )
    	assert_false( m_NPCShopPlayerSoldWnd:IsShow())
    	assert_true( m_NPCShopSellWnd:IsShow() )
    	
    	m_NPCShopSellWnd:OnCtrlmsg(  m_NPCShopSellWnd:GetDlgChild("BuyBack"), BUTTON_LCLICK, 0, 0 )
		  
		--NPC商店购回窗口的关闭按钮
		m_NPCShopPlayerSoldWnd:OnCtrlmsg( m_NPCShopPlayerSoldWnd:GetDlgChild("Close"), BUTTON_LCLICK, 0, 0 )
		assert_false( m_NPCShopPlayerSoldWnd:IsShow() )
		assert_false( m_MainBagWnd:IsShow() )
	end        
           
	--退出操作
	function test_NPCShop:TestEnd()
		controler:LoginOutFromGame()
	end
end