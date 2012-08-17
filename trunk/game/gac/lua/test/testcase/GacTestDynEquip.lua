gac_require "test/common/CTstLoginControler"
gac_require "test/common/CTstMakeMgr"

function InitGacTestDynEquip(TestTest)
	local controler = {}
	local WndItemBag,WndMainBag = {},{}
	
	function TestTest:setup()
	end
	
	--进入操作
	function TestTest:TestBegin()
		controler = CTstLoginControler:new()
		controler:OneStepLoginEx( "DynEquip_Test", "DynEquip" )
		g_GameMain.m_FunctionArea:OnCtrlmsg( g_GameMain.m_FunctionArea:GetDlgChild("package"), BUTTON_LCLICK, 0, 0 )
		
		WndMainBag = g_GameMain.m_WndMainBag 
		--点开人物属性面板,等待数据信息
		g_GameMain.m_FunctionArea:OnCtrlmsg( g_GameMain.m_FunctionArea.m_ChkBtnProp,BUTTON_LCLICK,0,0 )
		RoleStatus = g_GameMain.m_RoleStatus 
		
		--创建背包管理对象
		BagMgr = CTstBagMgr:new(WndMainBag)
		--人物信息管理
		--PlayInfoMgr = CTstPlayerInfoMgr:new()
		--打开铁匠面板
		--MakeEquipMgr = CTstMakeMgr:new()
		--MakeEquipMgr:OpenFuntion()
	end
	
	--[[
	function TestTest:TestOpenSmallBag()
		--创建一个新背包，并把它放到第一个格子上
		--print("In the TestOpenSmallBag")
		BagMgr:AddItem(2,"花背包",1)
		BagMgr:PlaceBag2MainBag(0,0,0,0)
	end
	
	
	--删除初始化装备
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
	--]]
	
	--静态装备
	function TestTest:TestUseStatic()
		BagMgr:AddItem(5,"1速剑",1)
		BagMgr:AddItem(6,"衣服",1)
		BagMgr:AddItem(6,"帽子",1)
		BagMgr:AddItem(6,"护手",1)
		BagMgr:AddItem(6,"鞋子",1)
		--BagMgr:AddItem(7,"初始盾牌",1)
		BagMgr:AddItem(8,"我的戒指",1)	--戒指
		BagMgr:AddItem(8,"我的戒指",1)	--戒指
		local Mgr = g_GameMain.m_BagSpaceMgr
	
		BagMgr:UseItemInMainBag(0,0)
		local BagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 1)
		local nBigID,nIndex = BagGrid:GetType()
		assert_equal(nil, nBigID, '')

		BagMgr:UseItemInMainBag(0,1)
		local BagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 2)
		local nBigID,nIndex = BagGrid:GetType()
		assert_equal(nil, nBigID, '')

		BagMgr:UseItemInMainBag(0,2)
		local BagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 3)
		local nBigID,nIndex = BagGrid:GetType()
		assert_equal(nil, nBigID, '')
		
		BagMgr:UseItemInMainBag(0,3)
		local BagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 4)
		local nBigID,nIndex = BagGrid:GetType()
		assert_equal(nil, nBigID, '')
		
		BagMgr:UseItemInMainBag(0,4)
		local BagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 5)
		local nBigID,nIndex = BagGrid:GetType()
		assert_equal(nil, nBigID, '')
		
		BagMgr:UseItemInMainBag(0,5)
		local BagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 6)
		local nBigID,nIndex = BagGrid:GetType()
		assert_equal(nil, nBigID, '')
		
		BagMgr:UseItemInMainBag(1,0)
		local BagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 7)
		local nBigID,nIndex = BagGrid:GetType()
		assert_equal(nil, nBigID, '')
		
--		BagMgr:UseItemInMainBag(1,1)
--		local BagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 8)
--		local nBigID,nIndex = BagGrid:GetType()
--		assert_equal(nil, nBigID, '')
		
	end
	

	function TestTest:TestReLogin()
		controler:LoginOutFromGame()
		controler:OneStepReLoginEx( "DynEquip_Test", "DynEquip" )
		--重新打开背包
		WndItemBag = g_GameMain.m_FunctionArea.m_WndItemBag
		--点开背包窗体
		g_GameMain.m_FunctionArea:OnCtrlmsg( g_GameMain.m_FunctionArea:GetDlgChild("package"), BUTTON_LCLICK, 0, 0 )
		WndMainBag = g_GameMain.m_WndMainBag
		BagMgr = CTstBagMgr:new(WndMainBag)
		--点开人物属性面板,等待数据信息
		SetEvent(Event.Test.PlayerInfo,false) --因为换装备也会setevent，所有先清除掉
		g_GameMain.m_FunctionArea:OnCtrlmsg( g_GameMain.m_FunctionArea.m_ChkBtnProp,BUTTON_LCLICK,0,0 )
		RoleStatus = g_GameMain.m_RoleStatus
	end
	
	--先卸载装备再穿上装备
	function TestTest:TestFetchThenEquip()
		local Mgr = g_GameMain.m_BagSpaceMgr
		--武器
		RoleStatus:OnCtrlmsg(RoleStatus.Part[12].iconbtn,BUTTON_LCLICK,0,0)
		BagMgr:MainBagClicked(0,0)
		WaitEvent(true,nil,Event.Test.FetchEquip)
		SetEvent( Event.Test.FetchEquip,false, "" )
		local BagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 1)
		local nBigID,nIndex = BagGrid:GetType()
		assert_equal(5, nBigID,"")
		BagMgr:UseItemInMainBag(0,0)
		assert_equal(0, BagGrid:Size(),"")

		--取左戒指
		RoleStatus:OnCtrlmsg(RoleStatus.Part[4].iconbtn ,BUTTON_LCLICK,0,0)
		BagMgr:MainBagClicked(0,0)
		WaitEvent(true,nil,Event.Test.FetchEquip)
		SetEvent( Event.Test.FetchEquip,false, "" )
		nBigID,nIndex = BagGrid:GetType()
		assert_equal(8, nBigID,"")
		BagMgr:UseItemInMainBag(0,0)
		assert_equal(0, BagGrid:Size(),"")

		--取右戒指
		RoleStatus:OnCtrlmsg(RoleStatus.Part[5].iconbtn ,BUTTON_LCLICK,0,0)
		BagMgr:MainBagClicked(0,0)
		WaitEvent(true,nil,Event.Test.FetchEquip)
		SetEvent( Event.Test.FetchEquip,false, "" )
		nBigID,nIndex = BagGrid:GetType()
		assert_equal(8, nBigID,"")
		BagMgr:UseItemInMainBag(0,0)
		assert_equal(0, BagGrid:Size(),"")

		--盾牌
--		RoleStatus:OnCtrlmsg(RoleStatus.m_WndAssociateWeapon,BUTTON_LCLICK,0,0)
--		BagMgr:MainBagClicked(0,0)
--		WaitEvent(true,nil,Event.Test.FetchEquip)
--		SetEvent( Event.Test.FetchEquip,false, "" )
--		nBigID,nIndex = BagGrid:GetType()
--		assert_equal(7, nBigID,"")
--		BagMgr:UseItemInMainBag(0,0)
--		assert_equal(0, BagGrid:Size(),"")
		--头盔
		RoleStatus:OnCtrlmsg(RoleStatus.Part[7].iconbtn ,BUTTON_LCLICK,0,0)
	 	BagMgr:MainBagClicked(0,0)
		WaitEvent(true,nil,Event.Test.FetchEquip)
		SetEvent( Event.Test.FetchEquip,false, "" )
		local BagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 1)
		nBigID,nIndex = BagGrid:GetType()
		assert_equal(6, nBigID,"")
		BagMgr:UseItemInMainBag(0,0)
		assert_equal(0, BagGrid:Size(),"")
		--衣服
		RoleStatus:OnCtrlmsg(RoleStatus.Part[9].iconbtn ,BUTTON_LCLICK,0,0)
		BagMgr:MainBagClicked(0,0)
		WaitEvent(true,nil,Event.Test.FetchEquip)
		SetEvent( Event.Test.FetchEquip,false, "" )
		nBigID,nIndex = BagGrid:GetType()
		assert_equal(6, nBigID,"")
		BagMgr:UseItemInMainBag(0,0)
		assert_equal(0, BagGrid:Size(),"")
		--护腕
		RoleStatus:OnCtrlmsg(RoleStatus.Part[3].iconbtn ,BUTTON_LCLICK,0,0)
		BagMgr:MainBagClicked(0,0)
		WaitEvent(true,nil,Event.Test.FetchEquip)
		SetEvent( Event.Test.FetchEquip,false, "" )
		nBigID,nIndex = BagGrid:GetType()
		assert_equal(6, nBigID,"")
		BagMgr:UseItemInMainBag(0,0)
		assert_equal(0, BagGrid:Size(),"")
		--鞋子
		RoleStatus:OnCtrlmsg(RoleStatus.Part[11].iconbtn ,BUTTON_LCLICK,0,0)
		BagMgr:MainBagClicked(0,0)
		WaitEvent(true,nil,Event.Test.FetchEquip)
		SetEvent( Event.Test.FetchEquip,false, "" )
		nBigID,nIndex = BagGrid:GetType()
		assert_equal(6, nBigID,"")
		BagMgr:UseItemInMainBag(0,0)
		assert_equal(0, BagGrid:Size(),"")
	end
	--删除装备
	function TestTest:TestDelEquip()
		--删除武器
		RoleStatus:OnCtrlmsg(RoleStatus.Part[12].iconbtn,BUTTON_LCLICK,0,0)
	 	g_GameMain:OnLButtonUp( 0, 0, 0 )
	 	local wnd = RoleStatus.Part[12].iconbtn.m_MsgBox:GetDlgChild("BtnOK")
		wnd:SendMsg( WM_LBUTTONDOWN, 0, 0 )
		wnd:SendMsg( WM_LBUTTONUP, 0, 0 )
		WaitEvent(true,nil,Event.Test.FetchEquip)
		SetEvent( Event.Test.FetchEquip,false, "" )
		assert_equal(nil,RoleStatus.Part[12].iconbtn.m_Info,"")
		--删除左右戒指
		RoleStatus:OnCtrlmsg(RoleStatus.Part[4].iconbtn,BUTTON_LCLICK,0,0)
	 	g_GameMain:OnLButtonUp( 0, 0, 0 )
	 	local wnd = RoleStatus.Part[4].iconbtn.m_MsgBox:GetDlgChild("BtnOK")
		wnd:SendMsg( WM_LBUTTONDOWN, 0, 0 )
		wnd:SendMsg( WM_LBUTTONUP, 0, 0 )
		WaitEvent(true,nil,Event.Test.FetchEquip)
		SetEvent( Event.Test.FetchEquip,false, "" )
		assert_equal(nil, RoleStatus.Part[4].iconbtn.m_Info,"")
		
		RoleStatus:OnCtrlmsg(RoleStatus.Part[5].iconbtn,BUTTON_LCLICK,0,0)
	 	g_GameMain:OnLButtonUp( 0, 0, 0 )
	 	wnd = RoleStatus.Part[5].iconbtn.m_MsgBox:GetDlgChild("BtnOK")
		wnd:SendMsg( WM_LBUTTONDOWN, 0, 0 )
		wnd:SendMsg( WM_LBUTTONUP, 0, 0 )
		WaitEvent(true,nil,Event.Test.FetchEquip)
		SetEvent( Event.Test.FetchEquip,false, "" )
		assert_equal(nil, RoleStatus.Part[5].iconbtn.m_Info,"")

		--删除暗器
		--
--		RoleStatus:OnCtrlmsg(RoleStatus.m_WndAssociateWeapon,BUTTON_LCLICK,0,0)
--	 	g_GameMain:OnLButtonUp( 0, 0, 0 )
--	 	local wnd = RoleStatus.m_WndAssociateWeapon.m_MsgBox:GetDlgChild("BtnOK")
--		wnd:SendMsg( WM_LBUTTONDOWN, 0, 0 )
--		wnd:SendMsg( WM_LBUTTONUP, 0, 0 )
--		WaitEvent(true,nil,Event.Test.FetchEquip)
--		SetEvent( Event.Test.FetchEquip,false, "" )
--		assert_equal(nil, RoleStatus.m_WndAssociateWeapon.m_Info,"")
		--
		--头盔
		RoleStatus:OnCtrlmsg(RoleStatus.Part[7].iconbtn,BUTTON_LCLICK,0,0)
	 	g_GameMain:OnLButtonUp( 0, 0, 0 )
	 	local wnd = RoleStatus.Part[7].iconbtn.m_MsgBox:GetDlgChild("BtnOK")
		wnd:SendMsg( WM_LBUTTONDOWN, 0, 0 )
		wnd:SendMsg( WM_LBUTTONUP, 0, 0 )
		WaitEvent(true,nil,Event.Test.FetchEquip)
		SetEvent( Event.Test.FetchEquip,false, "" )
		assert_equal(nil, RoleStatus.Part[7].iconbtn.m_Info,"")
		--衣服
		RoleStatus:OnCtrlmsg(RoleStatus.Part[9].iconbtn,BUTTON_LCLICK,0,0)
	 	g_GameMain:OnLButtonUp( 0, 0, 0 )
	 	local wnd = RoleStatus.Part[9].iconbtn.m_MsgBox:GetDlgChild("BtnOK")
		wnd:SendMsg( WM_LBUTTONDOWN, 0, 0 )
		wnd:SendMsg( WM_LBUTTONUP, 0, 0 )
		WaitEvent(true,nil,Event.Test.FetchEquip)
		SetEvent( Event.Test.FetchEquip,false, "" )
		assert_equal(nil, RoleStatus.Part[9].iconbtn.m_Info,"")
		--护腕
		RoleStatus:OnCtrlmsg(RoleStatus.Part[3].iconbtn,BUTTON_LCLICK,0,0)
	 	g_GameMain:OnLButtonUp( 0, 0, 0 )
	 	local wnd = RoleStatus.Part[3].iconbtn.m_MsgBox:GetDlgChild("BtnOK")
		wnd:SendMsg( WM_LBUTTONDOWN, 0, 0 )
		wnd:SendMsg( WM_LBUTTONUP, 0, 0 )
		WaitEvent(true,nil,Event.Test.FetchEquip)
		SetEvent( Event.Test.FetchEquip,false, "" )
		assert_equal(nil, RoleStatus.Part[3].iconbtn.m_Info,"")
		--删除鞋子
		RoleStatus:OnCtrlmsg(RoleStatus.Part[11].iconbtn,BUTTON_LCLICK,0,0)
		g_GameMain:OnLButtonUp( 0, 0, 0 )
	 	local wnd = RoleStatus.Part[11].iconbtn.m_MsgBox:GetDlgChild("BtnOK")
		wnd:SendMsg( WM_LBUTTONDOWN, 0, 0 )
		wnd:SendMsg( WM_LBUTTONUP, 0, 0 )
		WaitEvent(true,nil,Event.Test.FetchEquip)
		SetEvent( Event.Test.FetchEquip,false, "" )
		assert_equal(nil, RoleStatus.Part[11].iconbtn.m_Info,"")
	end
	
	function TestTest:TestEnd()
		controler:LoginOutFromGame()
	end
	function TestTest:teardown()
	end
end