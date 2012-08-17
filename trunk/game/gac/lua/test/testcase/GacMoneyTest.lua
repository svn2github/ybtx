gac_require "test/common/CTstLoginControler"
gac_require "test/common/CTstBagMgr"
gac_require "test/common/CTstPlayerInfoMgr"

function InitGacPlayerMoney( TestTest )
	local controler = {}
	local WndItemBag,WndMainBag = {},{}
	
	local function CheckCommonAttr(nNum)
		assert_equal(nNum, PlayInfoMgr:GetAddAgility(),"")
		assert_equal(nNum, PlayInfoMgr:GetAddIntellect(),"")
		assert_equal(nNum, PlayInfoMgr:GetAddSpirit(),"")
		assert_equal(nNum, PlayInfoMgr:GetAddStamina(),"")
		assert_equal(nNum, PlayInfoMgr:GetAddStrength(),"")
	end
	
	function TestTest:setup()
	end
	
	--进入操作
	function TestTest:TestBegin()
		controler = CTstLoginControler:new()
		controler:OneStepLoginEx( "ItemUse_Test", "itemuse" )
		WndItemBag = g_GameMain.m_FunctionArea.m_WndItemBag
		--点开背包窗体
		g_GameMain.m_FunctionArea:OnCtrlmsg( g_GameMain.m_FunctionArea.m_ChkBtnBag,BUTTON_LCLICK,0,0 )
		WndMainBag = g_GameMain.m_WndMainBag
		--点开人物属性面板,等待数据信息
		SetEvent(Event.Test.PlayerInfo,false)
		g_GameMain.m_FunctionArea:OnCtrlmsg( g_GameMain.m_FunctionArea.m_ChkBtnProp,BUTTON_LCLICK,0,0 )
		--local strRet = WaitEvent(true,nil,Event.Test.PlayerInfo)
		--gas\lua\player\ServerPlayer.lua  918    返回被注释掉了  原因???
		SetEvent(Event.Test.PlayerInfo,false)
		RoleStatus = g_GameMain.m_RoleStatus
		--创建背包管理对象
		BagMgr = CTstBagMgr:new(WndMainBag)
		--人物信息管理
		PlayInfoMgr = CTstPlayerInfoMgr:new()
		
	end
	
	
	function TestTest:TestAddMoney()
		SetEvent(Event.Test.AddMoneyGM,false)
		Gac2Gas:GM_Execute( g_Conn, "$addmoney(10101)" );
		local strRet = WaitEvent(true,nil,Event.Test.AddMoneyGM)
		SetEvent(Event.Test.AddMoneyGM,false)
		 
		assert_equal(10101,g_MainPlayer:GetMoney(),"Money Error")
		assert_equal(tostring(1), WndMainBag.m_WndGold:GetWndText() ,"Gold Wnd Display Error")
		assert_equal(tostring(01), WndMainBag.m_WndArgent:GetWndText() ,"Argent Wnd Display Error")
		assert_equal(tostring(01), WndMainBag.m_WndCopper:GetWndText() ,"Copper Wnd Display Error")
		
		Gac2Gas:GM_Execute( g_Conn, "$addmoney(-10101)" );
		strRet = WaitEvent(true,nil,Event.Test.AddMoneyGM)
		SetEvent(Event.Test.AddMoneyGM,false)
		assert_equal(0,g_MainPlayer:GetMoney(),"Money Error")
	end
	
	function TestTest:TestAddMoneyError()
		Gac2Gas:GM_Execute( g_Conn, "$addmoney(-100000)" );
		local strRet = WaitEvent(true,nil,Event.Test.AddMoneyGM)
		SetEvent(Event.Test.AddMoneyGM,false)
		assert_equal( strRet[2][1] , "AddMoneyError" , "AddMoneyError,But Not Error")
	end
	
	function TestTest:TestAddTicket()
		Gac2Gas:GM_Execute( g_Conn, "$addticket(10101)" );
		local strRet = WaitEvent(true,nil,Event.Test.AddTicketGM)
		SetEvent(Event.Test.AddTicketGM,false)
		assert_equal(10101, g_MainPlayer:GetTicket(),"Ticket Error")
		assert_equal(tostring(10101), WndMainBag.m_WndTicket:GetWndText() ,"Ticket Wnd Display Error")
		
		Gac2Gas:GM_Execute( g_Conn, "$addticket(-10101)" );
		local strRet = WaitEvent(true,nil,Event.Test.AddTicketGM)
		SetEvent(Event.Test.AddTicketGM,false)
		assert_equal(0,g_MainPlayer:GetTicket(),"Ticket Error")
	end
	
	function TestTest:TestAddTicketError()
		Gac2Gas:GM_Execute( g_Conn, "$addticket(-100000)" );
		local strRet = WaitEvent(true,nil,Event.Test.AddTicketGM)
		SetEvent(Event.Test.AddTicketGM,false)
		assert_equal( strRet[2][1] , "AddTicketError" , "AddTicketError,But Not Error")
	end
	
	function TestTest:TestAddBindingMoney()
		Gac2Gas:GM_Execute( g_Conn, "$addbindingmoney(10101)" );
		local strRet = WaitEvent(true,nil,Event.Test.AddBindingMoneyGM)
		SetEvent(Event.Test.AddBindingMoneyGM,false)
		assert_equal(10101,g_MainPlayer:GetBindMoney(),"BindingMoney Error")
		
		Gac2Gas:GM_Execute( g_Conn, "$addbindingmoney(-10101)" );
		local strRet = WaitEvent(true,nil,Event.Test.AddBindingMoneyGM)
		SetEvent(Event.Test.AddBindingMoneyGM,false)
		assert_equal(0,g_MainPlayer:GetBindMoney(),"BindingMoney Error")
	end
	
	function TestTest:TestAddBindingMoneyError()
		Gac2Gas:GM_Execute( g_Conn, "$addbindingmoney(-100000)" );
		local strRet = WaitEvent(true,nil,Event.Test.AddBindingMoneyGM)
		SetEvent(Event.Test.AddBindingMoneyGM,false)
		assert_equal( strRet[2][1] , "AddBindingMoneyError" , "AddBindingMoneyError,But Not Error")
	end
	
	function TestTest:TestAddBindingTicket()
		Gac2Gas:GM_Execute( g_Conn, "$addbindingticket(10101)" );
		local strRet = WaitEvent(true,nil,Event.Test.AddBindingTicketGM)
		SetEvent(Event.Test.AddBindingTicketGM,false)
		assert_equal(10101,g_MainPlayer:GetBindTicket(),"BindingMoney Error")
		
		Gac2Gas:GM_Execute( g_Conn, "$addbindingticket(-10101)" );
		local strRet = WaitEvent(true,nil,Event.Test.AddBindingTicketGM)
		SetEvent(Event.Test.AddBindingTicketGM,false)
		assert_equal(0,g_MainPlayer:GetBindTicket(),"BindingMoney Error")
	end
	
	function TestTest:TestAddBindingTicketError()
		Gac2Gas:GM_Execute( g_Conn, "$addbindingticket(-100000)" );
		local strRet = WaitEvent(true,nil,Event.Test.AddBindingTicketGM)
		SetEvent(Event.Test.AddBindingTicketGM,false)
		assert_equal( strRet[2][1] , "AddBindingTicketError" , "AddBindingTicketError,But Not Error")
	end
	--货币流通
	function TestTest:TestChangeMoney()
		Gac2Gas:GM_Execute( g_Conn, "$openpanel(10)" )
	  
		--先增加四种钱
		Gac2Gas:GM_Execute( g_Conn, "$addticket(1000)" )
		WaitEvent(true,nil,Event.Test.AddTicketGM)
	
		Gac2Gas:GM_Execute( g_Conn, "$addbindingticket(1000)" )
		WaitEvent(true,nil,Event.Test.AddBindingTicketGM)
		
		Gac2Gas:GM_Execute( g_Conn, "$addbindingmoney(1000)" )
		WaitEvent(true,nil,Event.Test.AddBindingMoneyGM)
		
		Gac2Gas:GM_Execute( g_Conn, "$addmoney(1000)" )
		WaitEvent(true,nil,Event.Test.AddMoneyGM)

		--金币兑换绑定金币
		g_GameMain.m_Agiotage.MoneyToMoney:SetWndText("100")
		g_GameMain.m_Agiotage:OnCtrlmsg(g_GameMain.m_Agiotage.TranTo ,BUTTON_LCLICK,0, 0)
    
		local strRet = WaitEvent( true, nil, Event.Test.OpenChangeMoneyWnd )
		SetEvent( Event.Test.OpenChangeMoneyWnd, false )

		assert_equal(900,g_MainPlayer:GetMoney(),"MoneyToMoney Error")
		assert_equal(1100,g_MainPlayer:GetBindMoney(),"MoneyToMoney Error")
	 
			--佣兵币兑换金币
		g_GameMain.m_Agiotage.TickToMoney:SetWndText("100")
		g_GameMain.m_Agiotage:OnCtrlmsg(g_GameMain.m_Agiotage.TranTo ,BUTTON_LCLICK,0, 0)
		local strRet = WaitEvent( true, nil, Event.Test.OpenChangeMoneyWnd )
		SetEvent( Event.Test.OpenChangeMoneyWnd, false )
		assert_equal(900,g_MainPlayer:GetTicket(),"TickToMoney Error")
		assert_equal(1000,g_MainPlayer:GetMoney(),"TickToMoney Error")
		
			--佣兵币兑换绑定的佣兵卷
		g_GameMain.m_Agiotage.TickToTick:SetWndText("100")
		g_GameMain.m_Agiotage:OnCtrlmsg(g_GameMain.m_Agiotage.TranTo ,BUTTON_LCLICK,0, 0)
		local strRet = WaitEvent( true, nil, Event.Test.OpenChangeMoneyWnd )
		SetEvent( Event.Test.OpenChangeMoneyWnd, false )
		assert_equal(800,g_MainPlayer:GetTicket(),"TickToMoney Error")
		assert_equal(1100,g_MainPlayer:GetBindTicket(),"TickToMoney Error")
	end
	
	--[[
	function TestTest:TestDelInitEquip()
		RoleStatus:OnCtrlmsg(RoleStatus.m_WndWear,BUTTON_LCLICK,0,0)
	 	g_GameMain:OnLButtonUp( 0, 0, 0 )
	 	local wnd = RoleStatus.m_WndWear.m_MsgBox:GetDlgChild("BtnOK")
		wnd:SendMsg( WM_LBUTTONDOWN, 0, 0 )
		wnd:SendMsg( WM_LBUTTONUP, 0, 0 )
		WaitEvent(true,nil,Event.Test.FetchEquip)
		SetEvent( Event.Test.FetchEquip,false, "" )
		assert_equal(nil, RoleStatus.m_WndWear.m_Info,"")
		
		RoleStatus:OnCtrlmsg(RoleStatus.m_WndWeapon,BUTTON_LCLICK,0,0)
	 	g_GameMain:OnLButtonUp( 0, 0, 0 )
	 	local wnd = RoleStatus.m_WndWeapon.m_MsgBox:GetDlgChild("BtnOK")
		wnd:SendMsg( WM_LBUTTONDOWN, 0, 0 )
		wnd:SendMsg( WM_LBUTTONUP, 0, 0 )
		WaitEvent(true,nil,Event.Test.FetchEquip)
		SetEvent( Event.Test.FetchEquip,false, "" )
		assert_equal(nil, RoleStatus.m_WndWeapon.m_Info,"")
	end
	
	--武器测试,数字下一步进行测试
	function TestTest:TestUseStaticWeapon()
		BagMgr:AddItem(5,"1速剑",1)
		BagMgr:AddItem(5,"2速剑",1)
		BagMgr:AddItem(5,"1速杖",1)
		BagMgr:AddItem(5,"2速杖",1)
		
		BagMgr:UseItemInMainBag(0,0)
		
		local Mgr = g_GameMain.m_BagSpaceMgr
		local fBagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 1)
		assert_equal(0, fBagGrid:Size(),"")
		CheckCommonAttr(1)
		
		--如果通过了本地的client的验证，那么会在本地弹出对话框
		WndMainBag.m_ctItemRoom.m_MsgBox = nil
		BagMgr:UseItemInMainBag(0,1)
		assert_equal(nil, WndMainBag.m_ctItemRoom.m_MsgBox,"")
		
		WndMainBag.m_ctItemRoom.m_MsgBox = nil
		BagMgr:UseItemInMainBag(0,2)
		assert_equal(nil, WndMainBag.m_ctItemRoom.m_MsgBox,"")
		
		--替换成功
		BagMgr:UseItemInMainBag(0,3)
		fBagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 4)
		local nBigID,nIndex = fBagGrid:GetType()
		assert_equal(1, fBagGrid:Size(),"")
		assert_equal(5, nBigID,"")
		assert_equal("1速剑", nIndex,"")
		CheckCommonAttr(2)
		
		BagMgr:DelItem(5,"1速剑",1)
		BagMgr:DelItem(5,"2速剑",1)
		BagMgr:DelItem(5,"1速杖",1)
	end
	
	--装备测试，数字和初始化后面在测试
	function TestTest:TestUseStaticArmor()
		BagMgr:AddItem(6,"衣服",1)
		BagMgr:AddItem(6,"帽子",1)
		BagMgr:AddItem(6,"护手",1)
		BagMgr:AddItem(6,"鞋子",1)
		BagMgr:AddItem(6,"好衣服",1)
		BagMgr:AddItem(6,"好帽子",1)
		BagMgr:AddItem(6,"好护手",1)
		BagMgr:AddItem(6,"好鞋子",1)
		--未使用装备的时候，防御为0
		assert_equal(0, PlayInfoMgr:GetAddShield(),"")
		BagMgr:UseItemInMainBag(0,0)
		local Mgr = g_GameMain.m_BagSpaceMgr
		local fBagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 1)
		assert_equal(0, fBagGrid:Size(),"")
		CheckCommonAttr(3)
		--assert_equal(16, PlayInfoMgr:GetAddShield(),"")--10+2+4 = 16
		
		--如果通过了本地的client的验证，那么会在本地弹出对话框
		BagMgr:UseItemInMainBag(0,1)
		fBagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 2)
		assert_equal(0, fBagGrid:Size(),"")
		CheckCommonAttr(4)
		--assert_equal(37, PlayInfoMgr:GetAddShield(),"")--15+2+4 = 21
		
		BagMgr:UseItemInMainBag(0,2)
		fBagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 3)
		assert_equal(0, fBagGrid:Size(),"")
		CheckCommonAttr(5)
		--assert_equal(58, PlayInfoMgr:GetAddShield(),"")--15+2+4 = 21
		
		BagMgr:UseItemInMainBag(0,3)
		fBagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 4)
		assert_equal(0, fBagGrid:Size(),"")
		CheckCommonAttr(6)
		--assert_equal(79, PlayInfoMgr:GetAddShield(),"")--15+2+4 = 21
		--替换
		BagMgr:UseItemInMainBag(0,4)
		fBagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 5)
		local nBigID,nIndex = fBagGrid:GetType()
		assert_equal(1, fBagGrid:Size(),"")
		assert_equal(6, nBigID,"")
		assert_equal("衣服", nIndex,"")
		
		WndMainBag.m_ctItemRoom.m_MsgBox = nil
		BagMgr:UseItemInMainBag(0,5)
		assert_equal(nil, WndMainBag.m_ctItemRoom.m_MsgBox,"")
		
		WndMainBag.m_ctItemRoom.m_MsgBox = nil
		BagMgr:UseItemInMainBag(0,6)
		assert_equal(nil, WndMainBag.m_ctItemRoom.m_MsgBox,"")
		
		WndMainBag.m_ctItemRoom.m_MsgBox = nil
		BagMgr:UseItemInMainBag(0,7)
		assert_equal(nil, WndMainBag.m_ctItemRoom.m_MsgBox,"")
		--替换装备属性提高
		CheckCommonAttr(7)
		--assert_equal(90, PlayInfoMgr:GetAddShield(),"")--15+4+8-10-2-4= 11
		
		BagMgr:DelItem(6,"衣服",1)
		BagMgr:DelItem(6,"好帽子",1)
		BagMgr:DelItem(6,"好护手",1)
		BagMgr:DelItem(6,"好鞋子",1)
	end
	
	
	--静态暗器测试
	function TestTest:TestUseStaticShield()
		BagMgr:AddItem(7,"暗器1",1)
		BagMgr:AddItem(7,"暗器2",1)
		BagMgr:AddItem(7,"暗器3",1)
		BagMgr:AddItem(7,"暗器4",1)
		
		BagMgr:UseItemInMainBag(0,0)
		local Mgr = g_GameMain.m_BagSpaceMgr
		local fBagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 1)
		assert_equal(0, fBagGrid:Size(),"")
		CheckCommonAttr(8)
		
		--如果通过了本地的client的验证，那么会在本地弹出对话框
		WndMainBag.m_ctItemRoom.m_MsgBox = nil
		BagMgr:UseItemInMainBag(0,1)
		assert_equal(nil, WndMainBag.m_ctItemRoom.m_MsgBox,"")
		
		--如果通过了本地的client的验证，那么会在本地弹出对话框
		WndMainBag.m_ctItemRoom.m_MsgBox = nil
		BagMgr:UseItemInMainBag(0,2)
		assert_equal(nil, WndMainBag.m_ctItemRoom.m_MsgBox,"")
		
		BagMgr:UseItemInMainBag(0,3)
		fBagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 4)
		local nBigID,nIndex = fBagGrid:GetType()
		assert_equal(1, fBagGrid:Size(),"")
		assert_equal(7, nBigID,"")
		assert_equal("暗器1", nIndex,"")
		
		CheckCommonAttr(9)
		
		BagMgr:DelItem(7,"暗器1",1)
		BagMgr:DelItem(7,"暗器2",1)
		BagMgr:DelItem(7,"暗器3",1)
	end

	--饰品的使用
	function TestTest:TestUseStaticJewelry()
		BagMgr:AddItem(8,"我的戒指",2)
		BagMgr:AddItem(8,"我的戒指2",1)
		BagMgr:AddItem(8,"项链",1)
		BagMgr:AddItem(8,"饰品",1)
		
		BagMgr:UseItemInMainBag(0,0)
		local Mgr = g_GameMain.m_BagSpaceMgr
		fBagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 1)
		assert_equal(0, fBagGrid:Size(),"")
		CheckCommonAttr(9)
		
		BagMgr:UseItemInMainBag(0,1)
		fBagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 2)
		assert_equal(0, fBagGrid:Size(),"")
		
		CheckCommonAttr(9)
		--衣服加了一
		
		--assert_equal(5, PlayInfoMgr:GetPoisonDefence(),"")
		--assert_equal(5, PlayInfoMgr:GetFireDefence(),"")
		--assert_equal(5, PlayInfoMgr:GetIceDefence(),"")
		--assert_equal(5, PlayInfoMgr:GetThunderDefence(),"")
		
		--戒指替换
		BagMgr:UseItemInMainBag(0,2)
		fBagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 3)
		local nBigID,nIndex = fBagGrid:GetType()
		assert_equal(1, fBagGrid:Size(),"")
		assert_equal(8, nBigID,"")
		assert_equal("我的戒指", nIndex,"")
		
		CheckCommonAttr(10)
		
		--assert_equal(5, PlayInfoMgr:GetPoisonDefence(),"")
		--assert_equal(5, PlayInfoMgr:GetFireDefence(),"")
		--assert_equal(5, PlayInfoMgr:GetIceDefence(),"")
		--assert_equal(5, PlayInfoMgr:GetThunderDefence(),"")
		
		--项链
		BagMgr:UseItemInMainBag(0,3)
		fBagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 4)
		assert_equal(0, fBagGrid:Size(),"")
		CheckCommonAttr(10)
		
		BagMgr:UseItemInMainBag(0,4)
		fBagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 5)
		assert_equal(0, fBagGrid:Size(),"")
		--饰品
		CheckCommonAttr(11)
		
		--assert_equal(9, PlayInfoMgr:GetPoisonDefence(),"")
		--assert_equal(9, PlayInfoMgr:GetFireDefence(),"")
		--assert_equal(9, PlayInfoMgr:GetIceDefence(),"")
		--assert_equal(9, PlayInfoMgr:GetThunderDefence(),"")
		
		BagMgr:DelItem(8,"我的戒指",1)
	end

	--可以检查人物的属性是否正确
	function TestTest:TestEquipLoginInAndLoginOut()
		controler:LoginOutFromGame()
		controler:OneStepReLoginEx( "ItemUse_Test", "itemuse" )
		--重新打开背包
		WndItemBag = g_GameMain.m_FunctionArea.m_WndItemBag
		--点开背包窗体
		g_GameMain.m_FunctionArea:OnCtrlmsg( g_GameMain.m_FunctionArea.m_ChkBtnBag,BUTTON_LCLICK,0,0 )
		WndMainBag = g_GameMain.m_WndMainBag
		
		BagMgr = CTstBagMgr:new(WndMainBag)
		--点开人物属性面板,等待数据信息
		SetEvent(Event.Test.PlayerInfo,false) --因为换装备也会setevent，所有先清除掉
		g_GameMain.m_FunctionArea:OnCtrlmsg( g_GameMain.m_FunctionArea.m_ChkBtnProp,BUTTON_LCLICK,0,0 )
		local strRet = WaitEvent(true,nil,Event.Test.PlayerInfo)
		SetEvent(Event.Test.PlayerInfo,false)
		RoleStatus = g_GameMain.m_RoleStatus
	end

	--重新登录后检查人物的数字是否正确
	function TestTest:TestReLoginCheck()
		CheckCommonAttr(11)
		
		--assert_equal(9, PlayInfoMgr:GetPoisonDefence(),"")
		--assert_equal(9, PlayInfoMgr:GetFireDefence(),"")
		--assert_equal(9, PlayInfoMgr:GetIceDefence(),"")
		--assert_equal(9, PlayInfoMgr:GetThunderDefence(),"")
		
	end
	
	--后面的测试是依赖前面装备上去的进行测试的
	function TestTest:TestFetchWeaponAndReplaceWeapon()

		BagMgr:AddItem(5,"1速剑",1)
		--先点击装备栏，在点击物品栏。交换装备，并且属性减1
		RoleStatus:OnCtrlmsg(RoleStatus.m_WndWeapon,BUTTON_LCLICK,0,0)
		BagMgr:MainBagClicked(0,0)
		WaitEvent(true,nil,Event.Test.UseItem)
		SetEvent( Event.Test.UseItem,false, "" )
		
		local Mgr = g_GameMain.m_BagSpaceMgr
		local BagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 1)
		local nBigID,nIndex = BagGrid:GetType()
		assert_equal(1, BagGrid:Size(),"")
		assert_equal(5, nBigID,"")
		assert_equal("2速杖", nIndex,"")
		CheckCommonAttr(10)
		
		--先点物品栏，在点装备栏.属性加1
		BagMgr:MainBagClicked(0,0)
		RoleStatus:OnCtrlmsg(RoleStatus.m_WndWeapon,BUTTON_LCLICK,0,0)
		WaitEvent(true,nil,Event.Test.UseItem)
		SetEvent( Event.Test.UseItem,false, "" )
		
		BagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 1)
		nBigID,nIndex = BagGrid:GetType()
		assert_equal(1, BagGrid:Size(),"")
		assert_equal(5, nBigID,"")
		assert_equal("1速剑", nIndex,"")
		CheckCommonAttr(11)
		
		--卸载装备
		RoleStatus:OnCtrlmsg(RoleStatus.m_WndWeapon,BUTTON_LCLICK,0,0)
		BagMgr:MainBagClicked(0,1)
		WaitEvent(true,nil,Event.Test.FetchEquip)
		SetEvent( Event.Test.FetchEquip,false, "" )
		
		BagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 2)
		nBigID,nIndex = BagGrid:GetType()
		assert_equal(1, BagGrid:Size(),"")
		assert_equal(5, nBigID,"")
		assert_equal("2速杖", nIndex,"")
		CheckCommonAttr(9)
		
		BagMgr:DelItem(5,"1速剑",1)
		BagMgr:DelItem(5,"2速杖",1)
	end
	
	function TestTest:TestFetchRingAndReplaceRing()
		--取下左戒指
		RoleStatus:OnCtrlmsg(RoleStatus.m_WndRingLeft,BUTTON_LCLICK,0,0)
		BagMgr:MainBagClicked(0,0)
		WaitEvent(true,nil,Event.Test.FetchEquip)
		SetEvent( Event.Test.FetchEquip,false, "" )
		
		local Mgr = g_GameMain.m_BagSpaceMgr
		local BagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 1)
		local nBigID,nIndex = BagGrid:GetType()
		assert_equal(1, BagGrid:Size(),"")
		assert_equal(8, nBigID,"")
		assert_equal("我的戒指", nIndex,"")
		CheckCommonAttr(9)
		
		--assert_equal(7, PlayInfoMgr:GetPoisonDefence(),"")
		--assert_equal(7, PlayInfoMgr:GetFireDefence(),"")
		--assert_equal(7, PlayInfoMgr:GetIceDefence(),"")
		--assert_equal(7, PlayInfoMgr:GetThunderDefence(),"")
		
		
		--替换2次右戒指
		RoleStatus:OnCtrlmsg(RoleStatus.m_WndRingRight,BUTTON_LCLICK,0,0)
		BagMgr:MainBagClicked(0,0)
		WaitEvent(true,nil,Event.Test.ReplaceRing)
		SetEvent( Event.Test.ReplaceRing,false, "" )
		CheckCommonAttr(8)
		
		BagMgr:MainBagClicked(0,0)
		RoleStatus:OnCtrlmsg(RoleStatus.m_WndRingRight,BUTTON_LCLICK,0,0)
		WaitEvent(true,nil,Event.Test.ReplaceRing)
		SetEvent( Event.Test.ReplaceRing,false, "" )
		
		BagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 1)
		nBigID,nIndex = BagGrid:GetType()
		assert_equal(1, BagGrid:Size(),"")
		assert_equal(8, nBigID,"")
		assert_equal("我的戒指", nIndex,"")
		CheckCommonAttr(9)
		
		--在把戒指放回去
		BagMgr:MainBagClicked(0,0)
		RoleStatus:OnCtrlmsg(RoleStatus.m_WndRingLeft,BUTTON_LCLICK,0,0)
		WaitEvent(true,nil,Event.Test.ReplaceRing)
		SetEvent( Event.Test.ReplaceRing,false, "" )
		CheckCommonAttr(9)
		
		--assert_equal(9, PlayInfoMgr:GetPoisonDefence(),"")
		--assert_equal(9, PlayInfoMgr:GetFireDefence(),"")
		--assert_equal(9, PlayInfoMgr:GetIceDefence(),"")
		--assert_equal(9, PlayInfoMgr:GetThunderDefence(),"")
		
		
		--交换两个戒指
		RoleStatus:OnCtrlmsg(RoleStatus.m_WndRingLeft,BUTTON_LCLICK,0,0)
		RoleStatus:OnCtrlmsg(RoleStatus.m_WndRingRight,BUTTON_LCLICK,0,0)
		WaitEvent(true,nil,Event.Test.Switch2Ring)
		SetEvent( Event.Test.Switch2Ring,false, "" )
		assert_equal("我的戒指2", RoleStatus.m_WndRingLeft.m_Info.m_nIndex,"")
		assert_equal("我的戒指", RoleStatus.m_WndRingRight.m_Info.m_nIndex,"")
	end
	--交换暗器，并删除暗器
	function TestTest:TestFetchAssociateWeaponAndDel()
		BagMgr:AddItem(7,"暗器1",1)
		--先点击装备栏，在点击物品栏。
		RoleStatus:OnCtrlmsg(RoleStatus.m_WndAssociateWeapon,BUTTON_LCLICK,0,0)
		BagMgr:MainBagClicked(0,0)
		WaitEvent(true,nil,Event.Test.UseItem)
		SetEvent( Event.Test.UseItem,false, "" )
		BagMgr:DelItem(7,"暗器4",1)
		
		--删除装备
	  RoleStatus:OnCtrlmsg(RoleStatus.m_WndAssociateWeapon,BUTTON_LCLICK,0,0)
	 	g_GameMain:OnLButtonUp( 0, 0, 0 )

	 	local wnd = RoleStatus.m_WndAssociateWeapon.m_MsgBox:GetDlgChild("BtnOK")
		wnd:SendMsg( WM_LBUTTONDOWN, 0, 0 )
		wnd:SendMsg( WM_LBUTTONUP, 0, 0 )
		WaitEvent(true,nil,Event.Test.FetchEquip)
		SetEvent( Event.Test.FetchEquip,false, "" )
		assert_equal(nil, RoleStatus.m_WndAssociateWeapon.m_Info,"")
		CheckCommonAttr(7)
	end
	
	--交换饰品，并且删除
	function TestTest:TestFetchStaticJewelryDel()
		BagMgr:AddItem(8,"项链",1)
		BagMgr:AddItem(8,"饰品",1)
		--需要类型匹配才能交换
		RoleStatus:OnCtrlmsg(RoleStatus.m_WndNecklace,BUTTON_LCLICK,0,0)
		BagMgr:MainBagClicked(0,0)
		WaitEvent(true,nil,Event.Test.UseItem)
		SetEvent( Event.Test.UseItem,false, "" )
		
		RoleStatus:OnCtrlmsg(RoleStatus.m_WndAccouterment,BUTTON_LCLICK,0,0)
		BagMgr:MainBagClicked(0,1)
		WaitEvent(true,nil,Event.Test.UseItem)
		SetEvent( Event.Test.UseItem,false, "" )
		
		BagMgr:DelItem(8,"项链",1)
		BagMgr:DelItem(8,"饰品",1)
		
		RoleStatus:OnCtrlmsg(RoleStatus.m_WndNecklace,BUTTON_LCLICK,0,0)
	 	g_GameMain:OnLButtonUp( 0, 0, 0 )
	 	local wnd = RoleStatus.m_WndNecklace.m_MsgBox:GetDlgChild("BtnOK")
		wnd:SendMsg( WM_LBUTTONDOWN, 0, 0 )
		wnd:SendMsg( WM_LBUTTONUP, 0, 0 )
		WaitEvent(true,nil,Event.Test.FetchEquip)
		SetEvent( Event.Test.FetchEquip,false, "" )
		assert_equal(nil, RoleStatus.m_WndNecklace.m_Info,"")
		CheckCommonAttr(7)
		
		RoleStatus:OnCtrlmsg(RoleStatus.m_WndAccouterment,BUTTON_LCLICK,0,0)
	 	g_GameMain:OnLButtonUp( 0, 0, 0 )
	 	local wnd = RoleStatus.m_WndAccouterment.m_MsgBox:GetDlgChild("BtnOK")
		wnd:SendMsg( WM_LBUTTONDOWN, 0, 0 )
		wnd:SendMsg( WM_LBUTTONUP, 0, 0 )
		WaitEvent(true,nil,Event.Test.FetchEquip)
		SetEvent( Event.Test.FetchEquip,false, "" )
		assert_equal(nil, RoleStatus.m_WndAccouterment.m_Info,"")
		CheckCommonAttr(6)
	end
	--交换防具，并删除
	function TestTest:TestFetchStaticArmorDel()
		BagMgr:AddItem(6,"帽子",1)
		BagMgr:AddItem(6,"护手",1)
		BagMgr:AddItem(6,"鞋子",1)
		BagMgr:AddItem(6,"好衣服",1)
		--往别的地方单击也会进行交换
		BagMgr:MainBagClicked(0,0)
		RoleStatus:OnCtrlmsg(RoleStatus.m_WndNecklace,BUTTON_LCLICK,0,0)
		WaitEvent(true,nil,Event.Test.UseItem)
		SetEvent( Event.Test.UseItem,false, "" )
		
		BagMgr:MainBagClicked(0,1)
		RoleStatus:OnCtrlmsg(RoleStatus.m_WndNecklace,BUTTON_LCLICK,0,0)
		WaitEvent(true,nil,Event.Test.UseItem)
		SetEvent( Event.Test.UseItem,false, "" )
		
		BagMgr:MainBagClicked(0,2)
		RoleStatus:OnCtrlmsg(RoleStatus.m_WndNecklace,BUTTON_LCLICK,0,0)
		WaitEvent(true,nil,Event.Test.UseItem)
		SetEvent( Event.Test.UseItem,false, "" )
		
		BagMgr:MainBagClicked(0,3)
		RoleStatus:OnCtrlmsg(RoleStatus.m_WndNecklace,BUTTON_LCLICK,0,0)
		WaitEvent(true,nil,Event.Test.UseItem)
		SetEvent( Event.Test.UseItem,false, "" )
		BagMgr:DelItem(6,"帽子",1)
		BagMgr:DelItem(6,"护手",1)
		BagMgr:DelItem(6,"鞋子",1)
		BagMgr:DelItem(6,"好衣服",1)
		
		RoleStatus:OnCtrlmsg(RoleStatus.m_WndHead,BUTTON_LCLICK,0,0)
	 	g_GameMain:OnLButtonUp( 0, 0, 0 )
	 	local wnd = RoleStatus.m_WndHead.m_MsgBox:GetDlgChild("BtnOK")
		wnd:SendMsg( WM_LBUTTONDOWN, 0, 0 )
		wnd:SendMsg( WM_LBUTTONUP, 0, 0 )
		WaitEvent(true,nil,Event.Test.FetchEquip)
		SetEvent( Event.Test.FetchEquip,false, "" )
		assert_equal(nil, RoleStatus.m_WndHead.m_Info,"")
		
		RoleStatus:OnCtrlmsg(RoleStatus.m_WndWear,BUTTON_LCLICK,0,0)
	 	g_GameMain:OnLButtonUp( 0, 0, 0 )
	 	local wnd = RoleStatus.m_WndWear.m_MsgBox:GetDlgChild("BtnOK")
		wnd:SendMsg( WM_LBUTTONDOWN, 0, 0 )
		wnd:SendMsg( WM_LBUTTONUP, 0, 0 )
		WaitEvent(true,nil,Event.Test.FetchEquip)
		SetEvent( Event.Test.FetchEquip,false, "" )
		assert_equal(nil, RoleStatus.m_WndWear.m_Info,"")
		
		RoleStatus:OnCtrlmsg(RoleStatus.m_WndCuff,BUTTON_LCLICK,0,0)
	 	g_GameMain:OnLButtonUp( 0, 0, 0 )
	 	local wnd = RoleStatus.m_WndCuff.m_MsgBox:GetDlgChild("BtnOK")
		wnd:SendMsg( WM_LBUTTONDOWN, 0, 0 )
		wnd:SendMsg( WM_LBUTTONUP, 0, 0 )
		WaitEvent(true,nil,Event.Test.FetchEquip)
		SetEvent( Event.Test.FetchEquip,false, "" )
		assert_equal(nil, RoleStatus.m_WndCuff.m_Info,"")
		
		RoleStatus:OnCtrlmsg(RoleStatus.m_WndShoe,BUTTON_LCLICK,0,0)
	 	g_GameMain:OnLButtonUp( 0, 0, 0 )
	 	local wnd = RoleStatus.m_WndShoe.m_MsgBox:GetDlgChild("BtnOK")
		wnd:SendMsg( WM_LBUTTONDOWN, 0, 0 )
		wnd:SendMsg( WM_LBUTTONUP, 0, 0 )
		WaitEvent(true,nil,Event.Test.FetchEquip)
		SetEvent( Event.Test.FetchEquip,false, "" )
		assert_equal(nil, RoleStatus.m_WndShoe.m_Info,"")
		
		assert_equal(0, PlayInfoMgr:GetAddShield(),"")
	end
	--]]
	--[[function TestTest:TestUseDrug()
		BagMgr:AddItem(3,10026,1)
		BagMgr:AddItem(3,20007,1)
		BagMgr:AddItem(3,20007,1)
		
		Gac2Gas:GM_Execute( g_Conn, "$AddLevel(25)" )
		local strRet = WaitEvent(true,nil,Event.Test.SetLevel)
		SetEvent(Event.Test.SetLevel,false)
	
		WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_RBUTTONCLICK, 0, 0)
		WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_RBUTTONCLICK, 0, 1)
		WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_RBUTTONCLICK, 0, 2)
		local strRet = WaitEvent(true,nil,Event.Test.UseItem)
		SetEvent(Event.Test.UseItem,false)
		
		local Mgr = g_GameMain.m_BagSpaceMgr
		local fBagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 1)
		assert_equal(0, fBagGrid:Size(),"")
		
		fBagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 2)
		assert_equal(1, fBagGrid:Size(),"")
		
		fBagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 3)
		assert_equal(1, fBagGrid:Size(),"")
		
		BagMgr:DelItem(3,20007,2)
	end--]]
	
	--退出操作
	function TestTest:TestEnd()
		WndItemBag:OnCtrlmsg( WndItemBag.m_BtnMainBag,BUTTON_LCLICK,0,0 )
		controler:LoginOutFromGame()
	end
	
	function TestTest:teardown()
	end
end