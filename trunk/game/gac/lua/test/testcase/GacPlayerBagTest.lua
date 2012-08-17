gac_gas_require "item/item_info_mgr/ItemInfoMgr"
gac_require "test/common/CTstLoginControler"

function InitGacPlayerBag( TestTest )
	local controler = {}
	local WndItemBag,WndMainBag,ctItemRoom,WndMainDepot,m_lcAddBagSlotsWnd = {},{},{},{},{}
	
	function TestTest:setup()
	end
	--进入操作
	function TestTest:TestBegin()
		controler = CTstLoginControler:new()
		controler:OneStepLogin()
		
	  WndItemBag = g_GameMain.m_FunctionArea.m_WndItemBag
	
		--点开背包窗体
		g_GameMain.m_FunctionArea:OnCtrlmsg( g_GameMain.m_FunctionArea:GetDlgChild("package"), BUTTON_LCLICK, 0, 0 )
		WndMainBag = g_GameMain.m_WndMainBag
		ctItemRoom = WndMainBag.m_ctItemRoom
	  m_lcAddBagSlotsWnd = g_GameMain.m_WndBagSlots.m_lcAddBagSlots
	
		--点开仓库
		WndItemBag:OnCtrlmsg( WndItemBag.m_BtnDepot,BUTTON_LCLICK,0,0 )
		WndMainDepot = g_GameMain.m_WndDepot
		--创建背包管理对象
		BagMgr = CTstBagMgr:new(WndMainBag)
		
		MasterSkillArea = g_GameMain.m_MasterSkillArea
		MainSkillsToolBar = g_GameMain.m_MainSkillsToolBar
	end
	
	--[[
	--模拟网络延迟，选中目标后，点击了多个不同的地方
	function TestTest:TestMove2MorePlace()
	 	 Gac2Gas:GM_Execute( g_Conn, "$additem(1,1,3)" );
		 local strRet = WaitEvent(true,nil,Event.Test.AddItem)
		 SetEvent(Event.Test.AddItem,false)
		 local Mgr = g_GameMain.m_BagSpaceMgr
		 local srcPos = ctItemRoom:GetPosByIndex(0,0)
		 local descPos1 = ctItemRoom:GetPosByIndex(0,1)
		 local descPos2 = ctItemRoom:GetPosByIndex(0,2)
		 
		 local space = Mgr:GetSpace(g_StoreRoomIndex.PlayerBag)
		 assert(space~=nil)
		 local srcGrid = space:GetGrid(srcPos)
		 local descGrid1 = space:GetGrid(descPos1)
		 local descGrid2 = space:GetGrid(descPos2)
		 
		 WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 0)
		 WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 1)
		 
		 WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 2)
		 
		 strRet = WaitEvent(true,nil,Event.Test.ReplaceItem)
		 SetEvent(Event.Test.ReplaceItem,false)
		 
		 strRet = WaitEvent(true,nil,Event.Test.MoveItem)
		 SetEvent(Event.Test.MoveItem,false)
		 
		 
		 assert_equal(0, srcGrid:Size(),"")
		 assert_equal(3, descGrid1:Size(),"")
		 assert_equal(0, descGrid2:Size(),"")
		 
		 Gac2Gas:GM_Execute( g_Conn, "$delitem(1,1,3)" );
		 strRet = WaitEvent(true,nil,Event.Test.DelItem)
		 SetEvent(Event.Test.DelItem,false)
	end
	--]]
	
  --用gm命令添加删除物品
	function TestTest:TestAddDelItemGM()
		-- print("In the TestAddDelItemGM")
		 --db()
		 BagMgr:AddItem(1,"仕女图",5)
		 -- print("2")
		 local Mgr = g_GameMain.m_BagSpaceMgr
		 local srcPos,descPos = ctItemRoom:GetPosByIndex(0,0),ctItemRoom:GetPosByIndex(0,1)
		 local space = Mgr:GetSpace(g_StoreRoomIndex.PlayerBag)
		 assert(space~=nil)
		 local srcGrid = space:GetGrid(srcPos)
		 local descGrid = space:GetGrid(descPos)
		 assert_equal(3, srcGrid:Size(),"")
		 assert_equal(2, descGrid:Size(),"")
		 --print("1")
		 BagMgr:DelItem(1,"仕女图",5)
		 local srcGrid = space:GetGrid(srcPos)
		 local descGrid = space:GetGrid(descPos)
		 assert_equal(0, srcGrid:Size(),"")
		 assert_equal(0, descGrid:Size(),"")
		 --print("3")
	end
	
	--*************************快捷栏测试用例 BEGIN*************************************
	--将物品栏上的物品图标拖放到快捷栏上，然后单击左右键使用
	--[[
	function TestTest:TestDragItemOnShortcut()
		--添加一些物品
		Gac2Gas:GM_Execute( g_Conn, "$additem(1,\"仕女图\",5)" )
		local strRet = WaitEvent(true,nil,Event.Test.AddItem)
		SetEvent(Event.Test.AddItem,false)
		
		for i=1, 10 do
			WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 0)
			local state = g_WndMouse:GetCursorState()
			assert_equal(ECursorState.eCS_PickupItem, state, "")
		
			MasterSkillArea:OnCtrlmsg(MasterSkillArea.m_WndSkill, ITEM_LBUTTONCLICK, i-1, 0)
			state = g_WndMouse:GetCursorState()
			assert_equal(ECursorState.eCS_Normal, state, "")
			local tblShortcutPiece = MasterSkillArea.m_WndSkill:GetTblShortcutPiece()
			local Piece = tblShortcutPiece[i]
			local PieceState = Piece:GetState()
			assert_equal(EShortcutPieceState.eSPS_Item, PieceState, "")
			
			--点左键
			MasterSkillArea:OnCtrlmsg(MasterSkillArea.m_WndSkill, ITEM_LBUTTONCLICK, i-1, 0)
			--点右键
			Piece:OnRButtonClick(0,0,0)
		end
		
	end
	
	--将竖快捷栏上的图标移动到横快捷栏上，然后单击左右键使用
	function TestTest:TestMoveShortcut()
		for i=1, 10 do
			local tblShortcutPiece1 = MasterSkillArea.m_WndSkill:GetTblShortcutPiece()
			local Piece1 = tblShortcutPiece1[i]
			Piece1:OnDrag(0,0)
			local state = g_WndMouse:GetCursorState()
			assert_equal(ECursorState.eCS_PickupItemShortcut, state, "")
			assert_equal(EShortcutPieceState.eSPS_None, Piece1:GetState(), "")
			
			MainSkillsToolBar:OnCtrlmsg(MainSkillsToolBar.m_WndSkill, ITEM_LBUTTONCLICK, 0, i-1)
			state = g_WndMouse:GetCursorState()
			assert_equal(ECursorState.eCS_Normal, state, "")
			local tblShortcutPiece2 = MainSkillsToolBar.m_WndSkill:GetTblShortcutPiece()
			local Piece2 = tblShortcutPiece2[i]
			assert_equal(EShortcutPieceState.eSPS_Item, Piece2:GetState(), "")
			
			--点左键
			MainSkillsToolBar:OnCtrlmsg(MainSkillsToolBar.m_WndSkill, ITEM_LBUTTONCLICK, 0, i-1)
			--点右键
			Piece2:OnRButtonClick(0,0,0)
		end
	end
	
	--将快捷栏上的图标丢掉
	function TestTest:TestThrowAwayShortcut()
		for i=1, 10 do
			local tblShortcutPiece2 = MainSkillsToolBar.m_WndSkill:GetTblShortcutPiece()
			local Piece2 = tblShortcutPiece2[i]
			Piece2:OnDrag(0,0)
			local state = g_WndMouse:GetCursorState()
			assert_equal(ECursorState.eCS_PickupItemShortcut, state, "")
			assert_equal(EShortcutPieceState.eSPS_None, Piece2:GetState(), "")
			
			g_GameMain:OnLButtonUp(0,0,0)
			state = g_WndMouse:GetCursorState()
			assert_equal(ECursorState.eCS_Normal, state, "")
		end
	end
	
	--交换物品图标
	function TestTest:TestSwapShortcutIcons()
	
		Gac2Gas:GM_Execute( g_Conn, "$additem(5,\"1速剑\",3)" )
		local strRet = WaitEvent(true,nil,Event.Test.AddItem)
		SetEvent(Event.Test.AddItem,false)
		
	  --拖放第1个图标
	  WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 0)
		local state = g_WndMouse:GetCursorState()
		assert_equal(ECursorState.eCS_PickupItem, state, "")
		
		MasterSkillArea:OnCtrlmsg(MasterSkillArea.m_WndSkill, ITEM_LBUTTONCLICK, 0, 0)
		state = g_WndMouse:GetCursorState()
		assert_equal(ECursorState.eCS_Normal, state, "")
		local tblShortcutPiece = MasterSkillArea.m_WndSkill:GetTblShortcutPiece()
		local Piece = tblShortcutPiece[1]
		local PieceState = Piece:GetState()
		assert_equal(EShortcutPieceState.eSPS_Item, PieceState, "")
		local uBigID1, uIndex1, uGlobalID1, num1 = Piece:GetItemContext()
		
		--拖放第2个图标
		WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 1)
		state = g_WndMouse:GetCursorState()
		assert_equal(ECursorState.eCS_PickupItem, state, "")
		
		MasterSkillArea:OnCtrlmsg(MasterSkillArea.m_WndSkill, ITEM_LBUTTONCLICK, 1, 0)
		state = g_WndMouse:GetCursorState()
		assert_equal(ECursorState.eCS_Normal, state, "")
		Piece = tblShortcutPiece[2]
		PieceState = Piece:GetState()
		assert_equal(EShortcutPieceState.eSPS_Item, PieceState, "")
		local uBigID2, uIndex2, uGlobalID2, num2 = Piece:GetItemContext()
		
		--第1个图标和第2个图标交换
		Piece:OnDrag(0,0) --拖起第2个图标
		MasterSkillArea:OnCtrlmsg(MasterSkillArea.m_WndSkill, ITEM_LBUTTONCLICK, 0, 0) --点击第1个格子
		MasterSkillArea:OnCtrlmsg(MasterSkillArea.m_WndSkill, ITEM_LBUTTONCLICK, 1, 0) --点击第2个格子
		state = g_WndMouse:GetCursorState()
		assert_equal(ECursorState.eCS_Normal, state, "")
		
		Piece = tblShortcutPiece[1]
		local uBigID1new, uIndex1new, uGlobalID1new, num1new = Piece:GetItemContext()
		Piece = tblShortcutPiece[2]
		local uBigID2new, uIndex2new, uGlobalID2new, num2new = Piece:GetItemContext()
		
		assert_equal(uBigID1, uBigID2new, "")
		assert_equal(uIndex1, uIndex2new, "")
		assert_equal(uGlobalID1, uGlobalID2new, "")
		assert_equal(num1, num2new, "")
		
		assert_equal(uBigID2, uBigID1new, "")
		assert_equal(uIndex2, uIndex1new, "")
		assert_equal(uGlobalID2, uGlobalID1new, "")
		assert_equal(num2, num1new, "")
		
		--再从物品栏上拖动一个图标
		WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 0)
		local state, context = g_WndMouse:GetCursorState()
		
		local uBigIDCursor, uIndexCursor, uGlobalIDCursor = g_WndMouse:GetItemBigSmallGlobalID(context[1], context[2], context[3])
		local numCursor = g_GameMain.m_BagSpaceMgr:GetItemCountByType(uBigIDCursor, uIndexCursor)
		
		assert_equal(ECursorState.eCS_PickupItem, state, "")
		
		--与快捷栏上的第1个图标交换
		MasterSkillArea:OnCtrlmsg(MasterSkillArea.m_WndSkill, ITEM_LBUTTONCLICK, 0, 0) --点击第1个格子
		Piece = tblShortcutPiece[1]
		local uBigID1newnew, uIndex1newnew, uGlobalID1newnew, num1newnew = Piece:GetItemContext()
		assert_equal(uBigIDCursor, uBigID1newnew, "")
		assert_equal(uIndexCursor, uIndex1newnew, "")
		assert_equal(uGlobalIDCursor, uGlobalID1newnew, "")
		assert_equal(numCursor, num1newnew, "")
		
		state, context = g_WndMouse:GetCursorState()
		assert_equal(uBigID1new, context[1], "")
		assert_equal(uIndex1new, context[2], "")
		assert_equal(uGlobalID1new, context[3], "")
		assert_equal(num1new, context[4], "")
		
		--把鼠标上的图标丢掉
		g_GameMain:OnLButtonUp(0,0,0)
		
		--删除物品
		Gac2Gas:GM_Execute( g_Conn, "$delitem(1,\"仕女图\",5)" );
		strRet = WaitEvent(true,nil,Event.Test.DelItem)
		SetEvent(Event.Test.DelItem,false)
		
		Gac2Gas:GM_Execute( g_Conn, "$delitem(5,\"1速剑\",3)" );
		strRet = WaitEvent(true,nil,Event.Test.DelItem)
		SetEvent(Event.Test.DelItem,false)
	end
	
	--*************************快捷栏测试用例 END***************************************
	--]]
	--移动物品
	function TestTest:TestMoveToEmpty()
	-- print("In the TestMoveToEmpty")
		 Gac2Gas:GM_Execute( g_Conn, "$additem(1,\"仕女图\",5)" );
		 local strRet = WaitEvent(true,nil,Event.Test.AddItem)
		 SetEvent(Event.Test.AddItem,false)
		 
		 WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 0)
		 WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 2, 2)
		 strRet = WaitEvent(true,nil,Event.Test.ReplaceItem)
		 SetEvent(Event.Test.ReplaceItem,false)
		 
		 
		 local Mgr = g_GameMain.m_BagSpaceMgr
		 local srcPos,descPos = ctItemRoom:GetPosByIndex(0,0),ctItemRoom:GetPosByIndex(2,2)
		 local space = Mgr:GetSpace(g_StoreRoomIndex.PlayerBag)
		 assert(space~=nil)
		 local srcGrid = space:GetGrid(srcPos)
		 local descGrid = space:GetGrid(descPos)
		 assert_equal(0, srcGrid:Size(),"")
		 assert_equal(3, descGrid:Size(),"")
		
		 Gac2Gas:GM_Execute( g_Conn, "$delitem(1,\"仕女图\",5)" );
		 strRet = WaitEvent(true,nil,Event.Test.DelItem)
		 SetEvent(Event.Test.DelItem,false)
	end
	
	--合并物品
	function TestTest:TestMoveItem()
	--print("In the TestMoveItem")
		 Gac2Gas:GM_Execute( g_Conn, "$additem(1,\"仕女图\",5)" );
		 local strRet = WaitEvent(true,nil,Event.Test.AddItem)
		 SetEvent(Event.Test.AddItem,false)
		 
		 local Mgr = g_GameMain.m_BagSpaceMgr
		 local srcPos,descPos = ctItemRoom:GetPosByIndex(0,0),ctItemRoom:GetPosByIndex(0,1)
		 local space = Mgr:GetSpace(g_StoreRoomIndex.PlayerBag)
		 assert(space~=nil)
		 local srcGrid = space:GetGrid(srcPos)
		 local descGrid = space:GetGrid(descPos)
		 assert_equal(3, srcGrid:Size(),"")
		 assert_equal(2, descGrid:Size(),"")
		 
		 WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 0)
		 WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 1)
		 strRet = WaitEvent(true,nil,Event.Test.MoveItem)
		 SetEvent(Event.Test.MoveItem,false)
		 
		 assert_equal(2, srcGrid:Size(),"")
		 assert_equal(3, descGrid:Size(),"")
		
		 Gac2Gas:GM_Execute( g_Conn, "$delitem(1,\"仕女图\",5)" );
		 strRet = WaitEvent(true,nil,Event.Test.DelItem)
		 SetEvent(Event.Test.DelItem,false)
	end
	
	--交换物品
	function TestTest:TestReplaceItem()
	--print("In the TestReplaceItem")
	 	 Gac2Gas:GM_Execute( g_Conn, "$additem(1,\"仕女图\",3)" );
		 local strRet = WaitEvent(true,nil,Event.Test.AddItem)
		 SetEvent(Event.Test.AddItem,false)
		 
		 Gac2Gas:GM_Execute( g_Conn, "$additem(1,\"小雪球\",1)" );
		 local strRet = WaitEvent(true,nil,Event.Test.AddItem)
		 SetEvent(Event.Test.AddItem,false)
		 
		 local Mgr = g_GameMain.m_BagSpaceMgr
		 local srcPos,descPos = ctItemRoom:GetPosByIndex(0,0),ctItemRoom:GetPosByIndex(0,1)
		 local space = Mgr:GetSpace(g_StoreRoomIndex.PlayerBag)
		 assert(space~=nil)
		 local srcGrid = space:GetGrid(srcPos)
		 local descGrid = space:GetGrid(descPos)
		 assert_equal(3, srcGrid:Size(),"")
		 assert_equal(1, descGrid:Size(),"")
		 WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 0)
		 WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 1)
		 strRet = WaitEvent(true,nil,Event.Test.ReplaceItem)
		 SetEvent(Event.Test.ReplaceItem,false)
		 
		 --交换了里面的内容，要重新获取一下
		 srcGrid = space:GetGrid(srcPos)
		 descGrid = space:GetGrid(descPos)
		 assert_equal(1, srcGrid:Size(),"")
		 assert_equal(3, descGrid:Size(),"")
		
		 Gac2Gas:GM_Execute( g_Conn, "$delitem(1,\"小雪球\",1)" );
		 strRet = WaitEvent(true,nil,Event.Test.DelItem)
		 SetEvent(Event.Test.DelItem,false)
		 
		 Gac2Gas:GM_Execute( g_Conn, "$delitem(1,\"仕女图\",3)" );
		 strRet = WaitEvent(true,nil,Event.Test.DelItem)
		 SetEvent(Event.Test.DelItem,false)
	end

	--拆到空的位置
	function TestTest:TestSplite2EmptyItem()
	--print("In the TestSplite2EmptyItem")
		 Gac2Gas:GM_Execute( g_Conn, "$additem(1,\"仕女图\",5)" );
		 local strRet = WaitEvent(true,nil,Event.Test.AddItem)
		 SetEvent(Event.Test.AddItem,false)
		 
		 	
		 WndMainBag:OnCtrlmsg(WndMainBag.m_btnSplit,BUTTON_LCLICK, 0, 0)
		 WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 0)
		 WndMainBag.m_WndSplit:OnCtrlmsg(WndMainBag.m_WndSplit.m_BtnOK,BUTTON_LCLICK,0,0)
		 WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 2, 2)
		 strRet = WaitEvent(true,nil,Event.Test.SplitItem)
		 SetEvent(Event.Test.SplitItem,false)
		 
		 
		 local Mgr = g_GameMain.m_BagSpaceMgr
		 local srcPos,descPos = ctItemRoom:GetPosByIndex(0,0),ctItemRoom:GetPosByIndex(2,2)
		 local space = Mgr:GetSpace(g_StoreRoomIndex.PlayerBag)
		 assert(space~=nil)
		 local srcGrid = space:GetGrid(srcPos)
		 local descGrid = space:GetGrid(descPos)
		 assert_equal(2, srcGrid:Size(),"")
		 assert_equal(1, descGrid:Size(),"")
		
		 Gac2Gas:GM_Execute( g_Conn, "$delitem(1,\"仕女图\",5)" );
		 strRet = WaitEvent(true,nil,Event.Test.DelItem)
		 SetEvent(Event.Test.DelItem,false)
	end
	
	--拆到未满的位置
	function TestTest:TestSplite2NotFullItem()
	--print("In the TestSplite2NotFullItem")
		 Gac2Gas:GM_Execute( g_Conn, "$additem(1,\"仕女图\",5)" );
		 local strRet = WaitEvent(true,nil,Event.Test.AddItem)
		 SetEvent(Event.Test.AddItem,false)
		 
		 	
		 WndMainBag:OnCtrlmsg(WndMainBag.m_btnSplit,BUTTON_LCLICK, 0, 0)
		 WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 0)
		 WndMainBag.m_WndSplit:OnCtrlmsg(WndMainBag.m_WndSplit.m_BtnOK,BUTTON_LCLICK,0,0)
		 WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 1)
		 strRet = WaitEvent(true,nil,Event.Test.SplitItem)
		 SetEvent(Event.Test.SplitItem,false)
		 
		 
		 local Mgr = g_GameMain.m_BagSpaceMgr
		 local srcPos,descPos = ctItemRoom:GetPosByIndex(0,0),ctItemRoom:GetPosByIndex(0,1)
		 local space = Mgr:GetSpace(g_StoreRoomIndex.PlayerBag)
		 assert(space~=nil)
		 local srcGrid = space:GetGrid(srcPos)
		 local descGrid = space:GetGrid(descPos)
		 assert_equal(2, srcGrid:Size(),"")
		 assert_equal(3, descGrid:Size(),"")
		
		 Gac2Gas:GM_Execute( g_Conn, "$delitem(1,\"仕女图\",5)" );
		 strRet = WaitEvent(true,nil,Event.Test.DelItem)
		 SetEvent(Event.Test.DelItem,false)
	end
	
	--拆到满的位置，服务器返回错误，client不变
	function TestTest:TestSplite2FullItem()
	--print("In the TestSplite2FullItem")
		 Gac2Gas:GM_Execute( g_Conn, "$additem(1,\"仕女图\",5)" );
		 local strRet = WaitEvent(true,nil,Event.Test.AddItem)
		 SetEvent(Event.Test.AddItem,false)
		 
		 	
		 WndMainBag:OnCtrlmsg(WndMainBag.m_btnSplit,BUTTON_LCLICK, 0, 0)
		 WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 1)
		 WndMainBag.m_WndSplit:OnCtrlmsg(WndMainBag.m_WndSplit.m_BtnOK,BUTTON_LCLICK,0,0)
		 WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 0)
		 strRet = WaitEvent(true,nil,Event.Test.SplitItem)
		 SetEvent(Event.Test.SplitItem,false)
		 
		 
		 local Mgr = g_GameMain.m_BagSpaceMgr
		 local srcPos,descPos = ctItemRoom:GetPosByIndex(0,0),ctItemRoom:GetPosByIndex(0,1)
		 local space = Mgr:GetSpace(g_StoreRoomIndex.PlayerBag)
		 assert(space~=nil)
		 local srcGrid = space:GetGrid(srcPos)
		 local descGrid = space:GetGrid(descPos)
		 assert_equal(3, srcGrid:Size(),"")
		 assert_equal(2, descGrid:Size(),"")
		
		 Gac2Gas:GM_Execute( g_Conn, "$delitem(1,\"仕女图\",5)" );
		 strRet = WaitEvent(true,nil,Event.Test.DelItem)
		 SetEvent(Event.Test.DelItem,false)
	end
	
	--拆到不同的物品，服务器返回错误，client清空鼠标
	function TestTest:TestSplite2OtherItem()
	-- print("In the TestSplite2OtherItem")
		 Gac2Gas:GM_Execute( g_Conn, "$additem(1,\"仕女图\",3)" );
		 local strRet = WaitEvent(true,nil,Event.Test.AddItem)
		 SetEvent(Event.Test.AddItem,false)
		 
		 Gac2Gas:GM_Execute( g_Conn, "$additem(1,\"小雪球\",1)" );
		 local strRet = WaitEvent(true,nil,Event.Test.AddItem)
		 SetEvent(Event.Test.AddItem,false)
		 
		 local Mgr = g_GameMain.m_BagSpaceMgr
		 local srcPos,descPos = ctItemRoom:GetPosByIndex(0,0),ctItemRoom:GetPosByIndex(0,1)
		 local space = Mgr:GetSpace(g_StoreRoomIndex.PlayerBag)
		 assert(space~=nil)
		 local srcGrid = space:GetGrid(srcPos)
		 local descGrid = space:GetGrid(descPos)
		 assert_equal(3, srcGrid:Size(),"")
		 assert_equal(1, descGrid:Size(),"")
		 
		 --点击拆分按钮，选择拆分目标，确认拆分数目，放置拆分物品
		 WndMainBag:OnCtrlmsg(WndMainBag.m_btnSplit,BUTTON_LCLICK, 0, 0)
		 WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 0)
		 WndMainBag.m_WndSplit:OnCtrlmsg(WndMainBag.m_WndSplit.m_BtnOK,BUTTON_LCLICK,0,0)
		 WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 1)
		 strRet = WaitEvent(true,nil,Event.Test.SplitItem)
		 SetEvent(Event.Test.SplitItem,false)
		 
		 

		 assert_equal(3, srcGrid:Size(),"")
		 assert_equal(1, descGrid:Size(),"")
		
		 Gac2Gas:GM_Execute( g_Conn, "$delitem(1,\"小雪球\",1)" );
		 strRet = WaitEvent(true,nil,Event.Test.DelItem)
		 SetEvent(Event.Test.DelItem,false)
		 
		 Gac2Gas:GM_Execute( g_Conn, "$delitem(1,\"仕女图\",3)" );
		 strRet = WaitEvent(true,nil,Event.Test.DelItem)
		 SetEvent(Event.Test.DelItem,false)
	end
	
	--从TestPlaceBag到TestFetchBag测试用例顺序是不能调整的
	--依赖物品的放置，TestFetchBag最后一个删除掉全部的背包
	function TestTest:TestPlaceBag()
	--print("In the TestPlaceBag")
		Gac2Gas:GM_Execute( g_Conn, "$additem(2,\"大背包\",1)" );
	 	local strRet = WaitEvent(true,nil,Event.Test.AddItem)
	 	SetEvent(Event.Test.AddItem,false)
	 	WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 0)
	 	g_GameMain.m_WndBagSlots:OnCtrlmsg(m_lcAddBagSlotsWnd, ITEM_LBUTTONCLICK, 0, 0)
	 	
	 	strRet = WaitEvent(true,nil,Event.Test.PlaceBag)
		SetEvent(Event.Test.PlaceBag,false)
	
		local Mgr = g_GameMain.m_BagSpaceMgr
		local spaceMainBag = Mgr:GetSpace(g_StoreRoomIndex.PlayerBag)
		local spaceMainAddSlots = Mgr:GetSpace(g_AddRoomIndexClient.PlayerExpandBag)
		
		assert(nil~=spaceMainBag, "spaceMainBag 为nil")
		assert(nil~=spaceMainAddSlots, "spaceMainAddSlots 为nil")
		
		local srcGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 1)
		local descGrid = Mgr:GetGridByRoomPos(g_AddRoomIndexClient.PlayerExpandBag, 1)

		assert_equal(0, srcGrid:Size(),"背包没有从主背包里面移走")
		assert_equal(1, descGrid:Size(),"背包没有放到到副栏上")
	end
	
	function TestTest:TestMoveBagInSlot()
		--print("In the TestMoveBagInSlot")
		g_GameMain.m_WndBagSlots:OnCtrlmsg(m_lcAddBagSlotsWnd, BUTTON_DRAG, 0, 0)
		g_GameMain.m_WndBagSlots:OnCtrlmsg(m_lcAddBagSlotsWnd, ITEM_LBUTTONCLICK , 0, 1)
		
		strRet = WaitEvent(true,nil, Event.Test.Change2Bag)
		SetEvent( Event.Test.Change2Bag,false)
	
		g_GameMain.m_WndBagSlots:OnCtrlmsg(m_lcAddBagSlotsWnd, BUTTON_DRAG, 0, 1)
		g_GameMain.m_WndBagSlots:OnCtrlmsg(m_lcAddBagSlotsWnd, ITEM_LBUTTONCLICK, 0, 2)
		
		strRet = WaitEvent(true,nil,Event.Test.Change2Bag)
		SetEvent(Event.Test.Change2Bag,false)
		
		g_GameMain.m_WndBagSlots:OnCtrlmsg(m_lcAddBagSlotsWnd, BUTTON_DRAG, 0, 2)
		g_GameMain.m_WndBagSlots:OnCtrlmsg(m_lcAddBagSlotsWnd, ITEM_LBUTTONCLICK, 0, 3)
		
		strRet = WaitEvent(true,nil,Event.Test.Change2Bag)
		SetEvent(Event.Test.Change2Bag,false)
		
		g_GameMain.m_WndBagSlots:OnCtrlmsg(m_lcAddBagSlotsWnd, BUTTON_DRAG, 0, 3)
		g_GameMain.m_WndBagSlots:OnCtrlmsg(m_lcAddBagSlotsWnd, ITEM_LBUTTONCLICK, 0, 0)
		
		strRet = WaitEvent(true,nil,Event.Test.Change2Bag)
		SetEvent(Event.Test.Change2Bag,false)
	end
 
	function TestTest:TestChange2Bag()
		--print("In the TestChange2Bag")
		Gac2Gas:GM_Execute( g_Conn, "$additem(2,\"大背包\",1)" );
		local strRet = WaitEvent(true,nil,Event.Test.AddItem)
	 	SetEvent(Event.Test.AddItem,false)
	 	--print("test")	
		Gac2Gas:GM_Execute( g_Conn, "$additem(2,\"大背包\",1)" );
		local strRet = WaitEvent(true,nil,Event.Test.AddItem)
	 	SetEvent(Event.Test.AddItem,false)
	 	
	 	WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 0)
	 	g_GameMain.m_WndBagSlots:OnCtrlmsg(m_lcAddBagSlotsWnd, ITEM_LBUTTONCLICK, 0, 1)
	 	
	 	strRet = WaitEvent(true,nil,Event.Test.PlaceBag)
		SetEvent(Event.Test.PlaceBag,false)
		
		WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 1)
	 	g_GameMain.m_WndBagSlots:OnCtrlmsg(m_lcAddBagSlotsWnd, ITEM_LBUTTONCLICK, 0, 3)
	 	
	 	strRet = WaitEvent(true,nil,Event.Test.PlaceBag)
		SetEvent(Event.Test.PlaceBag,false)

		-- 拿起槽上的一个Bag
		g_GameMain.m_WndBagSlots:OnCtrlmsg(m_lcAddBagSlotsWnd, BUTTON_DRAG, 0, 0)
		g_GameMain.m_WndBagSlots:OnCtrlmsg(m_lcAddBagSlotsWnd, ITEM_LBUTTONCLICK, 0, 3)
		
		strRet = WaitEvent(true,nil,Event.Test.Change2Bag)
		SetEvent(Event.Test.Change2Bag,false)
		
		local Mgr = g_GameMain.m_BagSpaceMgr
		local spaceMainBag = Mgr:GetSpace(g_StoreRoomIndex.PlayerBag)
		local spaceMainAddSlots = Mgr:GetSpace(g_AddRoomIndexClient.PlayerExpandBag)
		
		assert(nil~=spaceMainBag, "spaceMainBag 为nil")
		assert(nil~=spaceMainAddSlots, "spaceMainAddSlots 为nil")
		
		
		local descGrid1 = Mgr:GetGridByRoomPos(g_AddRoomIndexClient.PlayerExpandBag, 1)
		local descGrid2 = Mgr:GetGridByRoomPos(g_AddRoomIndexClient.PlayerExpandBag, 2)
		local descGrid3 = Mgr:GetGridByRoomPos(g_AddRoomIndexClient.PlayerExpandBag, 3)
		local descGrid4 = Mgr:GetGridByRoomPos(g_AddRoomIndexClient.PlayerExpandBag, 4)

		assert_equal(1, descGrid1:Size(),"")
		assert_equal(1, descGrid2:Size(),"")
		assert_equal(0, descGrid3:Size(),"背包交换错了位置")
		assert_equal(1, descGrid4:Size(),"")
	end

	function TestTest:TestFetchBag()
		-- 拿起槽上的一个Bag
		--print("In the TestFetchBag")
		g_GameMain.m_WndBagSlots:OnCtrlmsg(m_lcAddBagSlotsWnd, BUTTON_DRAG, 0, 0)
		WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 0)
		strRet = WaitEvent(true,nil,Event.Test.FetchBag)
		SetEvent(Event.Test.FetchBag,false)
		
		g_GameMain.m_WndBagSlots:OnCtrlmsg(m_lcAddBagSlotsWnd, BUTTON_DRAG, 0, 1)
		WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 1)
		strRet = WaitEvent(true,nil,Event.Test.FetchBag)
		SetEvent(Event.Test.FetchBag,false)
		
		g_GameMain.m_WndBagSlots:OnCtrlmsg(m_lcAddBagSlotsWnd, BUTTON_DRAG, 0, 3)
		WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 3)
		strRet = WaitEvent(true,nil,Event.Test.FetchBag)
		SetEvent(Event.Test.FetchBag,false)
		
		local Mgr = g_GameMain.m_BagSpaceMgr
		local descGrid1 = Mgr:GetGridByRoomPos(g_AddRoomIndexClient.PlayerExpandBag, 1)
		local descGrid2 = Mgr:GetGridByRoomPos(g_AddRoomIndexClient.PlayerExpandBag, 2)
		local descGrid3 = Mgr:GetGridByRoomPos(g_AddRoomIndexClient.PlayerExpandBag, 3)
		local descGrid4 = Mgr:GetGridByRoomPos(g_AddRoomIndexClient.PlayerExpandBag, 4)

		assert_equal(0, descGrid1:Size(),"背包还在副栏上")
		assert_equal(0, descGrid2:Size(),"背包还在副栏上")
		assert_equal(0, descGrid3:Size(),"这里不应该有背包的")
		assert_equal(0, descGrid4:Size(),"背包还在副栏上")
		
		Gac2Gas:GM_Execute( g_Conn, "$delitem(2,\"大背包\",3)" );
		strRet = WaitEvent(true,nil,Event.Test.DelItem)
		SetEvent(Event.Test.DelItem,false)
		
	end
	--[[
	--在仓库和主背包间进行各种移动
	function TestTest:TestMoveByDepotAndMain()
	  --print("In the TestMoveByDepotAndMain")
		Gac2Gas:GM_Execute( g_Conn, "$additem(1,\"仕女图\",5)" );
		local strRet = WaitEvent(true,nil,Event.Test.AddItem)
		SetEvent(Event.Test.AddItem,false)
		--快速移动到仓库

		WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_RBUTTONCLICK, 0, 0)
		local strRet = WaitEvent(true,nil,Event.Test.QuickMove)
		SetEvent(Event.Test.QuickMove,false)
		WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_RBUTTONCLICK, 0, 1)
		strRet = WaitEvent(true,nil,Event.Test.QuickMove)
		SetEvent(Event.Test.QuickMove,false)
		
		local Mgr = g_GameMain.m_BagSpaceMgr
		local fBagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 1)
		local sBagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 2)
		assert_equal(0, fBagGrid:Size(),"")
		assert_equal(0, sBagGrid:Size(),"")
		
		local fBagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerDepot, 1)
		local sBagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerDepot, 2)
		assert_equal(3, fBagGrid:Size(),"")
		assert_equal(2, sBagGrid:Size(),"")
		
		--点击移动到主背包
		WndMainDepot:OnCtrlmsg(WndMainDepot.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 0)
		WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 0)
		strRet = WaitEvent(true,nil,Event.Test.ReplaceItem)
		SetEvent(Event.Test.ReplaceItem,false)
		 
		fBagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 1)
		assert_equal(3, fBagGrid:Size(),"")
		
		--拆分到主背包
		WndMainBag:OnCtrlmsg(WndMainBag.m_btnSplit,BUTTON_LCLICK, 0, 0)
		WndMainDepot:OnCtrlmsg(WndMainDepot.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 1)
		WndMainDepot.m_WndSplit:OnCtrlmsg(WndMainDepot.m_WndSplit.m_BtnOK,BUTTON_LCLICK,0,0)
		WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 1)
		strRet = WaitEvent(true,nil,Event.Test.SplitItem)
		SetEvent(Event.Test.SplitItem,false)
		sBagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 2)
		assert_equal(1, sBagGrid:Size(),"")
		
		--快速移动回主背包
		WndMainDepot:OnCtrlmsg(WndMainDepot.m_ctItemRoom, ITEM_RBUTTONCLICK, 0, 1)
		strRet = WaitEvent(true,nil,Event.Test.QuickMove)
		SetEvent(Event.Test.QuickMove,false)
		sBagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 2)
		assert_equal(2, sBagGrid:Size(),"")
		
		Gac2Gas:GM_Execute( g_Conn, "$delitem(1,\"仕女图\",5)" );
		strRet = WaitEvent(true,nil,Event.Test.DelItem)
		SetEvent(Event.Test.DelItem,false)
	end
	--]]
----------------------------------------------------
   --主背包和小背包之间
	function TestTest:TestMoveInAllBag()
		--创建一个新背包，并把它放到第一个格子上
		--print("In the TestMoveInAllBag")
		BagMgr:AddItem(2,'大背包',1)
		BagMgr:PlaceBag2MainBag(0,0,0,0)
		BagMgr:AddItem(1,'仕女图',5)
	 
	  local Mgr = g_GameMain.m_BagSpaceMgr
	  local lcMainBag	= Mgr:GetSpaceRelateLc(g_StoreRoomIndex.PlayerBag)
		--原先的包裹栏里是可以放物品的，现在不可以，所以这部分的测试改为将物品栏里的包裹和包裹栏里的包裹互换
	  --SmallBag:包裹栏中某个包裹对应的窗口（现为期在物品栏里产生的格子）；
	  --lcSmallBag:物品栏窗口；lcAddBagSlots:包裹栏窗口
		local SmallBag,lcSmallBag,nSmallRoomIndex = BagMgr:GetSmallBagLcAndRoomIndex(g_AddRoomIndexClient.PlayerExpandBag,1)

    BagMgr:ReplaceItem(WndMainBag,lcMainBag,0,0,SmallBag,lcSmallBag,2,4)
		
		local fBagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 1)
		local sBagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 2)
		local fExpandBagGrid = Mgr:GetGridByRoomPos(nSmallRoomIndex, 1)
		assert_equal(0, fBagGrid:Size(),"")
		assert_equal(2, sBagGrid:Size(),"")
		assert_equal(3, fExpandBagGrid:Size(),"")
		
		--移动到，（0，1）
		BagMgr:MoveItem(SmallBag,lcSmallBag,2,4,WndMainBag,lcMainBag,0,1)

		--再移动到，（0，0）
		BagMgr:ReplaceItem(SmallBag,lcSmallBag,2,4,WndMainBag,lcMainBag,0,0)
		
		fBagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 1)
		sBagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 2)
		assert_equal(2, fBagGrid:Size(),"")
		assert_equal(3, sBagGrid:Size(),"")
		--取下空包
		BagMgr:FetchBagFromMainBag(0,0)
		
		BagMgr:DelItem(1,'仕女图',5)
		BagMgr:DelItem(2,'大背包',1)
	end
	
	--删除物品
	function TestTest:TestDelItem()
		--添加一些物品
		--print("In the TestDelItem")
		Gac2Gas:GM_Execute( g_Conn, "$additem(1,\"小雪球\",5)" );
		local strRet = WaitEvent(true,nil,Event.Test.AddItem)
		SetEvent(Event.Test.AddItem,false)
		local Mgr = g_GameMain.m_BagSpaceMgr
	 	local fBagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 1)
	 	assert_equal(5, fBagGrid:Size(),"")
	 
	  --删除物品
	  local lcMainBag	= Mgr:GetSpaceRelateLc(g_StoreRoomIndex.PlayerBag)
	  WndMainBag:OnCtrlmsg(lcMainBag, ITEM_LBUTTONCLICK, 0, 0)
	 	g_GameMain:OnLButtonUp( 0, 0, 0 )
	 	
	 	local wnd = lcMainBag.m_MsgBox:GetDlgChild("BtnOK")
		wnd:SendMsg( WM_LBUTTONDOWN, 0, 0 )
		wnd:SendMsg( WM_LBUTTONUP, 0, 0 )

		local strRet = WaitEvent(true,nil,Event.Test.DelItem)
		SetEvent(Event.Test.DelItem,false)
		fBagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 1)
		assert_equal(0, fBagGrid:Size(),"")
	end
	
		--[[
	--删除空背包
	function TestTest:TestDelBagAndItem()
		--创建一个新背包，并把它放到第一个格子上
		--print("In the TestDelBagAndItem")
		Gac2Gas:GM_Execute( g_Conn, "$additem(2,\"大背包\",1)" );
	 	local strRet = WaitEvent(true,nil,Event.Test.AddItem)
	 	SetEvent(Event.Test.AddItem,false)
	 	
	 	WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 0)
	 	g_GameMain.m_WndBagSlots:OnCtrlmsg(m_lcAddBagSlotsWnd, ITEM_LBUTTONCLICK, 0, 0)
	 	
	 	strRet = WaitEvent(true,nil,Event.Test.PlaceBag)
		SetEvent(Event.Test.PlaceBag,false)
		
		--添加一些物品
		Gac2Gas:GM_Execute( g_Conn, "$additem(1,\"小雪球\",5)" );
		local strRet = WaitEvent(true,nil,Event.Test.AddItem)
		SetEvent(Event.Test.AddItem,false)
		
		--获得物品窗口
		local Mgr = g_GameMain.m_BagSpaceMgr
		local fExpandGrid = Mgr:GetGridByRoomPos(g_AddRoomIndexClient.PlayerExpandBag, 1)
		local Item = fExpandGrid:GetItem()
		local ExpandBagInfo = Item:GetContext()
		
		
		local lcAddBagSlots = g_GameMain.m_WndBagSlots.m_lcAddBagSlots
		local lcMainBag	= Mgr:GetSpaceRelateLc(g_StoreRoomIndex.PlayerBag)
		local lcSmallBag =  Mgr:GetSpaceRelateLc(ExpandBagInfo.RoomIndex)
	
		--显示附栏背包
		local Slot = lcAddBagSlots:GetSubItem(0,0)
		--Slot:SetCheck(true)
		g_GameMain.m_WndBagSlots:OnCtrlmsg(lcAddBagSlots, ITEM_LBUTTONCLICK, 0, 0)
		--点击移动到附栏背包在物品栏所产生的空间的第一个位置
		WndMainBag:OnCtrlmsg(lcMainBag, ITEM_LBUTTONCLICK, 0, 0)
		WndMainBag:OnCtrlmsg(lcMainBag, ITEM_LBUTTONCLICK, 2, 4)
		strRet = WaitEvent(true,nil,Event.Test.ReplaceItem)
		SetEvent(Event.Test.ReplaceItem,false)
		
	 	local fBagGrid = Mgr:GetGridByRoomPos(ExpandBagInfo.RoomIndex, 1)
	 	assert_equal(5, fBagGrid:Size(),"")
	 
	  --删除附栏背包
		g_GameMain.m_WndBagSlots:OnCtrlmsg(m_lcAddBagSlotsWnd, BUTTON_DRAG, 0, 0)
	 	g_GameMain:OnLButtonUp( 0, 0, 0 )
	 	local wnd = lcAddBagSlots.m_MsgBox:GetDlgChild("BtnOK")
		wnd:SendMsg( WM_LBUTTONDOWN, 0, 0 )
		wnd:SendMsg( WM_LBUTTONUP, 0, 0 )
	
		--删除失败，因为有物品
		local strRet = WaitEvent(true,nil,Event.Test.DelBag)
		SetEvent(Event.Test.DelBag,false)
		fBagGrid = Mgr:GetGridByRoomPos(g_AddRoomIndexClient.PlayerExpandBag, 1)
		assert_equal(1, fBagGrid:Size(),"")

		--删除物品
	  WndMainBag:OnCtrlmsg(lcMainBag, ITEM_LBUTTONCLICK, 2, 4)
	 	g_GameMain:OnLButtonUp( 0, 0, 0 )
	 	local wnd = lcMainBag.m_MsgBox:GetDlgChild("BtnOK")
		wnd:SendMsg( WM_LBUTTONDOWN, 0, 0 )
		wnd:SendMsg( WM_LBUTTONUP, 0, 0 )
		local strRet = WaitEvent(true,nil,Event.Test.DelItem)
		SetEvent(Event.Test.DelItem,false)
		
		fBagGrid = Mgr:GetGridByRoomPos(g_StoreRoomIndex.PlayerBag, 1)
		assert_equal(0, fBagGrid:Size(),"")
			 
		--再删除背包
		g_GameMain.m_WndBagSlots:OnCtrlmsg(m_lcAddBagSlotsWnd, BUTTON_DRAG, 0, 0)
	 	g_GameMain:OnLButtonUp( 0, 0, 0 )
	 	local wnd = lcAddBagSlots.m_MsgBox:GetDlgChild("BtnOK")
		wnd:SendMsg( WM_LBUTTONDOWN, 0, 0 )
		wnd:SendMsg( WM_LBUTTONUP, 0, 0 )
		
		--删除成功
		local strRet = WaitEvent(true,nil,Event.Test.DelBag)
		SetEvent(Event.Test.DelBag,false)
		fBagGrid = Mgr:GetGridByRoomPos(g_AddRoomIndexClient.PlayerExpandBag, 1)
		assert_equal(0, fBagGrid:Size(),"删除背包失败")
	end

	--仓库巢开启测试
	function TestTest:TestOpenDepotSlot()
		--添加一个背包，并放到仓库的巢上
		--print("lastTest")
		Gac2Gas:GM_Execute( g_Conn, "$additem(2,\"大背包\",1)" );
	 	local strRet = WaitEvent(true,nil,Event.Test.AddItem)
	 	SetEvent(Event.Test.AddItem,false)
	 	WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 0)
	 	WndMainDepot:OnCtrlmsg(WndMainDepot.m_lcAddBagSlots, ITEM_LBUTTONCLICK, 0, 0)
	 	strRet = WaitEvent(true,nil,Event.Test.PlaceBag)
		SetEvent(Event.Test.PlaceBag,false)
		assert_equal("ReplaceItemSuccess", strRet[2][1], "OpenSlot Success!")

		--获得物品窗口
		local Mgr = g_GameMain.m_BagSpaceMgr
		local lcAddBagSlots = g_GameMain.m_WndBagSlots.m_lcAddBagSlots
		local lcMainBag	= Mgr:GetSpaceRelateLc(g_StoreRoomIndex.PlayerBag)	
				
		--开启第全部的巢
		for i = 1,6 do
			WndMainDepot:OnCtrlmsg(WndMainDepot.m_BtnStartup, BUTTON_LCLICK, 0, 0)
		 	assert_equal("OpenDepotSlotSuccess", strRet[2][1], "OpenSlot Failed!")
		 	local wnd = WndMainDepot.m_MsgBox:GetDlgChild("BtnOK")
			wnd:SendMsg( WM_LBUTTONDOWN, 0, 0 )
			wnd:SendMsg( WM_LBUTTONUP, 0, 0 )
		end
	
		--在把背包放上去
		WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 0)
	 	WndMainDepot:OnCtrlmsg(WndMainDepot.m_lcAddBagSlots, ITEM_LBUTTONCLICK, 0, 0)
	 	strRet = WaitEvent(true,nil,Event.Test.PlaceBag)
		SetEvent(Event.Test.PlaceBag,false)
		
		local fExpandGrid = Mgr:GetGridByRoomPos(g_AddRoomIndexClient.PlayerExpandDepot, 1)
		local Item = fExpandGrid:GetItem()
		local ExpandBagInfo = Item:GetContext()
		fBagGrid = Mgr:GetGridByRoomPos(g_AddRoomIndexClient.PlayerExpandDepot, 1)
		assert_equal(1, fBagGrid:Size(),"")
		
		--在开启放回错误,已经开满了
		WndMainDepot:OnCtrlmsg(WndMainDepot.m_BtnStartup, BUTTON_LCLICK, 0, 0)
	 	assert_equal("OpenDepotSlotError", strRet[2][1], "OpenSlot Success!")
	 	local wnd = WndMainDepot.m_MsgBox:GetDlgChild("BtnOK")
		wnd:SendMsg( WM_LBUTTONDOWN, 0, 0 )
		wnd:SendMsg( WM_LBUTTONUP, 0, 0 )
		
		--取下背包，并删除
		g_GameMain.m_WndBagSlots:OnCtrlmsg(m_lcAddBagSlotsWnd, BUTTON_DRAG, 0, 0)
		WndMainBag:OnCtrlmsg(lcMainBag, ITEM_LBUTTONCLICK, 0, 5)
		strRet = WaitEvent(true,nil,Event.Test.FetchBag)
		SetEvent(Event.Test.FetchBag,false)
		
		Gac2Gas:GM_Execute( g_Conn, "$delitem(2,\"大背包\",1)" );
		strRet = WaitEvent(true,nil,Event.Test.DelItem)
		SetEvent(Event.Test.DelItem,false)
	end--]]
	
	--退出操作
	function TestTest:TestEnd()
		WndItemBag:OnCtrlmsg( WndItemBag.m_BtnMainBag,BUTTON_LCLICK,0,0 )
		controler:LoginOutFromGame()
	end
	
	function TestTest:teardown()
	end
end
