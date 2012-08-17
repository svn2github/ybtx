gac_require "test/common/CTstLoginControler"

function _WaitEvent(time, Eventname)
	WaitEvent(true,time,Eventname)
	SetEvent(Eventname,false)
end
function InitGacTestEquipToSoul()
  local test_EquipToSoul = TestCase("TestEquipToSoul")
  function test_EquipToSoul:setup()
	end	
	--进入操作
	function test_EquipToSoul:TestBegin()
		controler = CTstLoginControler:new()
		controler:OneStepLogin()
		
		WndItemBag = g_GameMain.m_FunctionArea.m_WndItemBag
		--点开背包窗体
		--WndItemBag.m_BtnMainBag:SetCheck(true)
		g_GameMain.m_FunctionArea:OnCtrlmsg( g_GameMain.m_FunctionArea:GetDlgChild("package"), BUTTON_LCLICK, 0, 0 )
		
		WndItemBag:OnCtrlmsg( WndItemBag.m_BtnMainBag,BUTTON_LCLICK,0,0 )
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
	
	--用gm命令添加删除物品
	function test_EquipToSoul:TestAddDelItemGM()

		BagMgr:AddItem(20,"10级单手剑改造书",6)
		BagMgr:AddItem(5,"1速剑",6)
		BagMgr:AddSoul(2)
		
		local Mgr = g_GameMain.m_BagSpaceMgr
		local srcPos,descPos = ctItemRoom:GetPosByIndex(0,0),ctItemRoom:GetPosByIndex(0,1)
		local space = Mgr:GetSpace(g_StoreRoomIndex.PlayerBag)
		assert(space~=nil)
		local srcGrid = space:GetGrid(srcPos)
		local descGrid = space:GetGrid(descPos)
		assert_equal(3, srcGrid:Size(),"")
		assert_equal(2, descGrid:Size(),"")
	end
	--将装备注入魂
	function test_EquipToSoul:test_SoulToEquip()
		local SoulAddEquip = g_GameMain.m_mergePearlEquip
		SoulAddEquip:ShowWnd(true)
		SoulAddEquip:SetFocus()
		SoulAddEquip:OnCtrlmsg(SoulAddEquip.m_CloseBtn, BUTTON_LCLICK, 0, 0)			--测试关闭按钮
		assert_false(SoulAddEquip:IsShow())
		SoulAddEquip:ShowWnd(true)
		SoulAddEquip:SetFocus()
		WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 1, 0)
		SoulAddEquip:OnCtrlmsg(SoulAddEquip.m_EquipBtn, BUTTON_LCLICK, 0, 0)   			--放进装备
		WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 2, 0)
		SoulAddEquip:OnCtrlmsg(SoulAddEquip.m_SoulPearlBtn, BUTTON_LCLICK, 0, 0)   --放进魂
		SoulAddEquip:OnCtrlmsg(SoulAddEquip.m_OKBtn, BUTTON_LCLICK, 0, 0)   			--合成
		_WaitEvent(nil, Event.Test.SoulInEquip)
		--SoulAddEquip:ShowWnd(false)
	end
	
	--将装备炼化
	function test_EquipToSoul:TestPutInEquip()
		local EquipToSoul = g_GameMain.m_EquipToSoul
		EquipToSoul:ShowWnd(true)
		EquipToSoul:SetFocus()			
		WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 0)
		EquipToSoul:OnCtrlmsg(EquipToSoul.m_EquipBtn, BUTTON_LCLICK, 0, 0) --放进非装备
		if EquipToSoul.m_MsgBox ~=nil then
			local wnd = EquipToSoul.m_MsgBox:GetDlgChild("BtnOK")
			wnd:SendMsg( WM_LBUTTONUP, 0, 0 )
		end
		WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 0)
		WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 1, 0)
		EquipToSoul:OnCtrlmsg(EquipToSoul.m_EquipBtn, BUTTON_LCLICK, 0, 0) --放进装备 
		EquipToSoul:OnCtrlmsg(EquipToSoul.m_GetSoulBtn, BUTTON_LCLICK, 0, 0) --炼化 
		_WaitEvent(nil, Event.Test.AddSoul)
	end
	function test_EquipToSoul:test_EquipRebuild()
	end
	function test_EquipToSoul:teardown()
	end
end