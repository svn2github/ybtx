gac_require "test/common/CTstLoginControler"

function InitGacPlayerSkillPannel( TestTest )
	local controler = {}
	local WndSkillPannel = {}
	local CbtSkill = {}
	local FunctionArea = {}
	local player_controler = {}
	
	function TestTest:setup()
	end
	--进入操作
	
	function TestTest:TestBegin()
		controler = CTstLoginControler:new()
		controler:OneStepLogin()
		player_controler = CTstPlayerControler:new()
		CbtSkill = g_GameMain.m_FunctionArea.m_ChkBtnSkill
		--菜单窗口
		FunctionArea = g_GameMain.m_FunctionArea
		--浮动快捷栏
		MasterSkillArea = g_GameMain.m_MasterSkillArea 
		--主快捷栏
		MainSkillsToolBar = g_GameMain.m_MainSkillsToolBar
		--技能学习面板
		WndSkillPannel = g_GameMain.m_SkillParent.m_SkillLearnWnd 
		--点开背包窗体
		Gac2Gas:GM_Execute( g_Conn, "$deltblshortcut(" .. g_MainPlayer.m_Properties:GetCharID()
 .. ")" )
	end
	
	function TestTest:TestOpenWndAndUseSkill()
		FunctionArea:OnCtrlmsg( FunctionArea.m_ChkBtnSkill, BUTTON_LCLICK, uParam1, uParam2 )
		assert(g_GameMain.m_SkillParent:IsShow())
		WndSkillPannel:OnCtrlmsg(WndSkillPannel.m_SkillBtn[2], BUTTON_LCLICK, 0, 0)
		WndSkillPannel:OnCtrlmsg(WndSkillPannel.m_SkillLearnBtn, BUTTON_LCLICK, 0, 0)
		WndSkillPannel:OnCtrlmsg(WndSkillPannel.m_SkillBtn[3], BUTTON_LCLICK, 0, 0)
		WndSkillPannel:OnCtrlmsg(WndSkillPannel.m_SkillLearnBtn, BUTTON_LCLICK, 0, 0)	
		WndSkillPannel:OnCtrlmsg(WndSkillPannel.m_SkillBtn[4], BUTTON_LCLICK, 0, 0)
		WndSkillPannel:OnCtrlmsg(WndSkillPannel.m_SkillLearnBtn, BUTTON_LCLICK, 0, 0)			
	end
	
	--将技能面板上的技能图标拖放到快捷栏上，然后单击左右键使用
	function TestTest:TestDragItemOnShortcut()
		for i=1, 10 do
			WndSkillPannel:OnCtrlmsg(WndSkillPannel.m_SkillBtn[2], BUTTON_DRAG, 0, 0)
			local state = g_WndMouse:GetCursorState()
			assert_equal(ECursorState.eCS_PickupSkill, state, "")
			MainSkillsToolBar:OnCtrlmsg(MainSkillsToolBar.m_WndSkill, ITEM_LBUTTONCLICK, 0, i-1)
			state = g_WndMouse:GetCursorState()
			--print(i)
			assert_equal(ECursorState.eCS_Normal, state, "")
			local tblShortcutPiece = MainSkillsToolBar.m_WndSkill:GetTblShortcutPiece()
			local Piece = tblShortcutPiece[i]
			local PieceState = Piece:GetState()
			assert_equal(EShortcutPieceState.eSPS_Skill, PieceState, "")
			--点左键
			MainSkillsToolBar:OnCtrlmsg(MainSkillsToolBar.m_WndSkill, ITEM_LBUTTONCLICK, 0, i-1)
		end
	end
	
	--将主快捷栏上的图标移动到浮动捷栏上，然后单击左右键使用
	function TestTest:TestMoveShortcut()
		MainSkillsToolBar:OnCtrlmsg(MainSkillsToolBar.m_BtnPage, ITEM_LBUTTONCLICK, 0, 0)
		for i=1, 10 do
			local tblShortcutPiece1 = MainSkillsToolBar.m_WndSkill:GetTblShortcutPiece()
			local Piece1 = tblShortcutPiece1[i]
			MainSkillsToolBar.m_WndSkill:DragShortcut(Piece1)
			local state = g_WndMouse:GetCursorState()
			assert_equal(ECursorState.eCS_PickupSkill, state, "")
			assert_equal(EShortcutPieceState.eSPS_None, Piece1:GetState(), "")
			
			MasterSkillArea:OnCtrlmsg(MasterSkillArea.m_WndSkill, ITEM_LBUTTONCLICK, 0, i-1)
			state = g_WndMouse:GetCursorState()
			assert_equal(ECursorState.eCS_Normal, state, "")
			local tblShortcutPiece2 = MasterSkillArea.m_WndSkill:GetTblShortcutPiece()
			local Piece2 = tblShortcutPiece2[i]
			assert_equal(EShortcutPieceState.eSPS_Skill, Piece2:GetState(), "")
			--点左键
			MasterSkillArea:OnCtrlmsg(MasterSkillArea.m_WndSkill, ITEM_LBUTTONCLICK, 0, i-1)
		end
	end
	
	--将快捷栏上的图标丢掉
	function TestTest:TestThrowAwayShortcut()
		for i=1, 10 do
			local tblShortcutPiece2 = MasterSkillArea.m_WndSkill:GetTblShortcutPiece()
			local Piece2 = tblShortcutPiece2[i]
			MasterSkillArea.m_WndSkill:DragShortcut(Piece2)
			local state = g_WndMouse:GetCursorState()
			assert_equal(ECursorState.eCS_PickupSkill, state, "")
			assert_equal(EShortcutPieceState.eSPS_None, Piece2:GetState(), "")
			g_WndMouse:SetCursorSkillState( ECursorSkillState.eCSS_Normal )
			g_GameMain:OnLButtonUp(0,0,0)
			state = g_WndMouse:GetCursorState()
			assert_equal(ECursorState.eCS_Normal, state, "")
		end
	end
	
	--交换技能图标
	function TestTest:TestSwapShortcutIcons()
	  --拖放第1个图标
		WndSkillPannel:OnCtrlmsg(WndSkillPannel.m_SkillBtn[2], BUTTON_DRAG, 0, 0)
		local state = g_WndMouse:GetCursorState()
		assert_equal(ECursorState.eCS_PickupSkill, state, "")
		MainSkillsToolBar:OnCtrlmsg(MainSkillsToolBar.m_WndSkill, ITEM_LBUTTONCLICK, 0, 0)
		state = g_WndMouse:GetCursorState()
		assert_equal(ECursorState.eCS_Normal, state, "")
		local tblShortcutPiece = MainSkillsToolBar.m_WndSkill:GetTblShortcutPiece()
		local Piece = tblShortcutPiece[1]
		local PieceState = Piece:GetState()
		assert_equal(EShortcutPieceState.eSPS_Skill, PieceState, "")
		local fs1 = Piece:GetSkillContext()
		
		--拖放第2个图标
		WndSkillPannel:OnCtrlmsg(WndSkillPannel.m_SkillBtn[3], BUTTON_DRAG, 0, 0)
		state = g_WndMouse:GetCursorState()
		assert_equal(ECursorState.eCS_PickupSkill, state, "")
		
		MainSkillsToolBar:OnCtrlmsg(MainSkillsToolBar.m_WndSkill, ITEM_LBUTTONCLICK, 0, 1)
		state = g_WndMouse:GetCursorState()
		assert_equal(ECursorState.eCS_Normal, state, "")
		Piece = tblShortcutPiece[2]
		PieceState = Piece:GetState()
		assert_equal(EShortcutPieceState.eSPS_Skill, PieceState, "")
		local fs2 = Piece:GetSkillContext()
		
		--第1个图标和第2个图标交换
		MainSkillsToolBar.m_WndSkill:DragShortcut(Piece)--拖起第2个图标
		MainSkillsToolBar:OnCtrlmsg(MainSkillsToolBar.m_WndSkill, ITEM_LBUTTONCLICK, 0, 0) --点击第1个格子
		MainSkillsToolBar:OnCtrlmsg(MainSkillsToolBar.m_WndSkill, ITEM_LBUTTONCLICK, 0, 1) --点击第2个格子
		state = g_WndMouse:GetCursorState()
		assert_equal(ECursorState.eCS_Normal, state, "")
		
		Piece = tblShortcutPiece[1]
		local fs1new = Piece:GetSkillContext()
		Piece = tblShortcutPiece[2]
		local fs2new = Piece:GetSkillContext()
		
		assert_equal(fs1, fs2new, "")
		assert_equal(fs2, fs1new, "")
		
		--再从技能面板上拖动一个图标
		WndSkillPannel:OnCtrlmsg(WndSkillPannel.m_SkillBtn[4], BUTTON_DRAG, 0, 0)
		local state, context = g_WndMouse:GetCursorState()
		assert_equal(ECursorState.eCS_PickupSkill, state, "")
		
		--与快捷栏上的第1个图标交换
		MainSkillsToolBar:OnCtrlmsg(MainSkillsToolBar.m_WndSkill, ITEM_LBUTTONCLICK, 0, 0) --点击第1个格子
		Piece = tblShortcutPiece[1]
		local fs1newnew = Piece:GetSkillContext()
		assert_equal(context[1], fs1newnew, "")
		state, context = g_WndMouse:GetCursorState()
		assert_equal(fs1new, context[1], "")
		
		--把鼠标上的图标丢掉
		g_WndMouse:SetCursorSkillState( ECursorSkillState.eCSS_Normal )
		g_GameMain:OnLButtonUp(0,0,0)
		state = g_WndMouse:GetCursorState()
		assert_equal(ECursorState.eCS_Normal, state, "")		
	end
	
	function TestTest:TestEnd()
		FunctionArea:OnCtrlmsg( FunctionArea.m_ChkBtnSkill, BUTTON_LCLICK, uParam1, uParam2 )
		controler:LoginOutFromGame()
	end
	
	function TestTest:teardown()
	end
end