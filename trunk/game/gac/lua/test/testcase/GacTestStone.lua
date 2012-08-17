gac_require "test/common/CTstLoginControler"
engine_require "common/Misc/TypeCheck"
--魂镶嵌宝石 测试用例  
function InitGacTestStone()
	local TestStone= TestCase("GacTestStone")
	controler = CTstLoginControler:new()
	
	function TestStone:setup()
	end
	function TestStone:TestBegin()
		controler:OneStepLogin()
	end
	
	--打孔功能
	function TestStone:test_dakong()
		local StoneWnd = CStone.GetWnd()
		StoneWnd:ShowWnd(true) --打开宝石面板
		StoneWnd:OnCtrlmsg(StoneWnd:GetDlgChild("1"),BUTTON_LCLICK,0,0)
		 
		Gac2Gas:GM_Execute( g_Conn, "$additem(15,\"一级金刚钻\",1)" )
		local str = WaitEvent(true,nil,Event.Test.AddItem)
		SetEvent(Event.Test.AddItem,false)
		Gac2Gas:GM_Execute( g_Conn, "$additem(15,\"二级金刚钻\",1)" )
		local str1 = WaitEvent(true,nil,Event.Test.AddItem)
		SetEvent(Event.Test.AddItem,false)
		Gac2Gas:GM_Execute( g_Conn, "$additem(15,\"三级金刚钻\",1)" )
		local str2 = WaitEvent(true,nil,Event.Test.AddItem)
		SetEvent(Event.Test.AddItem,false)
		--[[  --因为开第四个孔有等级限制,先不测
		Gac2Gas:GM_Execute( g_Conn, "$additem(15,\"四级金刚钻\",1)" )
		local str3 = WaitEvent(true,nil,Event.Test.AddItem)
		SetEvent(Event.Test.AddItem,false)
		--]]
		--点开背包窗体
		g_GameMain.m_FunctionArea:OnCtrlmsg( g_GameMain.m_FunctionArea.m_ChkBtnBag,BUTTON_LCLICK,0,0 )
		assert_true(StoneWnd.StonePartUsing:IsShow(),"[PartStoneISShow]")
		WndMainBag = g_GameMain.m_WndMainBag
		 
		WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 0)
		StoneWnd.StonePartUsing:OnCtrlmsg(StoneWnd.StonePartUsing:GetDlgChild("1"),BUTTON_LCLICK,0,0)
		StoneWnd.StonePartUsing:OnCtrlmsg(StoneWnd.StonePartUsing:GetDlgChild("dakong"),BUTTON_LCLICK,0,0)
		local strRet = WaitEvent(true,nil,Event.Test.OpenHole) --打孔的返回
		local allHolestateRet = WaitEvent(true,nil,Event.Test.SendAllHoleInfoEnd)--重新获取所有孔的状态
		SetEvent(Event.Test.OpenHole,false)
		SetEvent(Event.Test.SendAllHoleInfoEnd,false)
		
		WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 1)
		StoneWnd.StonePartUsing:OnCtrlmsg(StoneWnd.StonePartUsing:GetDlgChild("2"),BUTTON_LCLICK,0,0)
		StoneWnd.StonePartUsing:OnCtrlmsg(StoneWnd.StonePartUsing:GetDlgChild("dakong"),BUTTON_LCLICK,0,0)
		local strRet1 = WaitEvent(true,nil,Event.Test.OpenHole) --打孔的返回
		local allHolestateRet = WaitEvent(true,nil,Event.Test.SendAllHoleInfoEnd)--重新获取所有孔的状态
		SetEvent(Event.Test.OpenHole,false)
		SetEvent(Event.Test.SendAllHoleInfoEnd,false)
		
		WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 2)
		StoneWnd.StonePartUsing:OnCtrlmsg(StoneWnd.StonePartUsing:GetDlgChild("3"),BUTTON_LCLICK,0,0)
		StoneWnd.StonePartUsing:OnCtrlmsg(StoneWnd.StonePartUsing:GetDlgChild("dakong"),BUTTON_LCLICK,0,0)
		local strRet2 = WaitEvent(true,nil,Event.Test.OpenHole) --打孔的返回
		local allHolestateRet = WaitEvent(true,nil,Event.Test.SendAllHoleInfoEnd)--重新获取所有孔的状态
		SetEvent(Event.Test.OpenHole,false)
		SetEvent(Event.Test.SendAllHoleInfoEnd,false)
		
		--[[  --等级不够
		WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 3)
		StoneWnd.StonePartUsing:OnCtrlmsg(StoneWnd.StonePartUsing:GetDlgChild("4"),BUTTON_LCLICK,0,0)
		StoneWnd.StonePartUsing:OnCtrlmsg(StoneWnd.StonePartUsing:GetDlgChild("dakong"),BUTTON_LCLICK,0,0)
		local strRet3 = WaitEvent(true,nil,Event.Test.OpenHole) --打孔的返回
		local allHolestateRet = WaitEvent(true,nil,Event.Test.SendAllHoleInfoEnd)--重新获取所有孔的状态
		SetEvent(Event.Test.OpenHole,false)
		SetEvent(Event.Test.SendAllHoleInfoEnd,false)
		--]]
	end
	
	--镶嵌功能
	function TestStone:test_xiangqian()
		local StoneWnd = CStone.GetWnd()
		StoneWnd:OnCtrlmsg(StoneWnd:GetDlgChild("1"),BUTTON_LCLICK,0,0)
		Gac2Gas:GM_Execute( g_Conn, "$additem(14,\"一级吉祥宝石\",1)" )
		local str = WaitEvent(true,nil,Event.Test.AddItem)
		SetEvent(Event.Test.AddItem,false)
		Gac2Gas:GM_Execute( g_Conn, "$additem(14,\"一级吉祥宝石\",1)" )
		local str1 = WaitEvent(true,nil,Event.Test.AddItem)
		SetEvent(Event.Test.AddItem,false)
		Gac2Gas:GM_Execute( g_Conn, "$additem(14,\"一级吉祥宝石\",1)" )
		local str2 = WaitEvent(true,nil,Event.Test.AddItem)
		SetEvent(Event.Test.AddItem,false)

		assert_true(StoneWnd.StonePartUsing:IsShow(),"[PartStoneISShow]")
		WndMainBag = g_GameMain.m_WndMainBag
		 --镶嵌第一个孔
		WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 0)
		StoneWnd.StonePartUsing:OnCtrlmsg(StoneWnd.StonePartUsing:GetDlgChild("1"),BUTTON_LCLICK,0,0)
		StoneWnd.StonePartUsing:OnCtrlmsg(StoneWnd.StonePartUsing:GetDlgChild("xiangqian"),BUTTON_LCLICK,0,0)
		local strRet = WaitEvent(true,nil,Event.Test.InlayStone) --镶嵌的返回
		local allHolestateRet = WaitEvent(true,nil,Event.Test.SendAllHoleInfoEnd)--重新获取所有孔的状态
		SetEvent(Event.Test.OpenHole,false)
		SetEvent(Event.Test.SendAllHoleInfoEnd,false)
		--镶嵌第二个孔
		WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 0)
		StoneWnd.StonePartUsing:OnCtrlmsg(StoneWnd.StonePartUsing:GetDlgChild("2"),BUTTON_LCLICK,0,0)
		StoneWnd.StonePartUsing:OnCtrlmsg(StoneWnd.StonePartUsing:GetDlgChild("xiangqian"),BUTTON_LCLICK,0,0)
		local strRet1 = WaitEvent(true,nil,Event.Test.InlayStone) --镶嵌的返回
		local allHolestateRet = WaitEvent(true,nil,Event.Test.SendAllHoleInfoEnd)--重新获取所有孔的状态
		SetEvent(Event.Test.OpenHole,false)
		SetEvent(Event.Test.SendAllHoleInfoEnd,false)
		--镶嵌第三个孔
		WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 0)
		StoneWnd.StonePartUsing:OnCtrlmsg(StoneWnd.StonePartUsing:GetDlgChild("3"),BUTTON_LCLICK,0,0)
		StoneWnd.StonePartUsing:OnCtrlmsg(StoneWnd.StonePartUsing:GetDlgChild("xiangqian"),BUTTON_LCLICK,0,0)
		local strRet1 = WaitEvent(true,nil,Event.Test.InlayStone) --镶嵌的返回
		local allHolestateRet = WaitEvent(true,nil,Event.Test.SendAllHoleInfoEnd)--重新获取所有孔的状态
		SetEvent(Event.Test.OpenHole,false)
		SetEvent(Event.Test.SendAllHoleInfoEnd,false)
	end
	
	--摘除功能
	function TestStone:test_zhaichu()
		local StoneWnd = CStone.GetWnd()
		StoneWnd:OnCtrlmsg(StoneWnd:GetDlgChild("1"),BUTTON_LCLICK,0,0)
		StoneWnd.StonePartUsing:OnCtrlmsg(StoneWnd.StonePartUsing:GetDlgChild("1"),BUTTON_LCLICK,0,0)
		StoneWnd.StonePartUsing:OnCtrlmsg(StoneWnd.StonePartUsing:GetDlgChild("zhaichu"),BUTTON_LCLICK,0,0)
		assert_true(StoneWnd.m_MSG:IsShow(),"[MSGWndISShow]")
		StoneWnd.m_MSG:OnCtrlmsg(StoneWnd.m_MSG:GetDlgChild("BtnOK"),BUTTON_LCLICK,0,0)
		local strRet = WaitEvent(true,nil,Event.Test.RemovalStone) --摘除的返回
		local allHolestateRet = WaitEvent(true,nil,Event.Test.SendAllHoleInfoEnd)--重新获取所有孔的状态
		SetEvent(Event.Test.RemovalStone,false)
		SetEvent(Event.Test.SendAllHoleInfoEnd,false)
		StoneWnd:ShowWnd(false) --关闭宝石面板
	end
	
	--打孔材料合成功能
	function TestStone:test_dakongMaterialcompound()
--		local StoneCompoundWnd = CStoneCompound.GetWnd()
--		StoneCompoundWnd:ShowWnd(true)  --打开合成面板
--		Gac2Gas:GM_Execute( g_Conn, "$additem(15,\"一级金刚钻\",12)" )
--		local str = WaitEvent(true,nil,Event.Test.AddItem)
--		SetEvent(Event.Test.AddItem,false)
--
--		assert_true(StoneCompoundWnd:IsShow(),"[MaterialCompoundISShow]")
--		WndMainBag = g_GameMain.m_WndMainBag
--		 --合成
--		WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 1) --刚摘除的宝石在包裹第一个位置
--		StoneCompoundWnd:OnCtrlmsg(StoneCompoundWnd:GetDlgChild("lowstone"),BUTTON_LCLICK,0,0)
--		StoneCompoundWnd:OnCtrlmsg(StoneCompoundWnd:GetDlgChild("compound"),BUTTON_LCLICK,0,0)
--		local strRet = WaitEvent(true,nil,Event.Test.ReturnSynthesisItemEnd) --合成的返回
--		SetEvent(Event.Test.ReturnSynthesisItemEnd,false)
--		StoneCompoundWnd:OnCtrlmsg(StoneCompoundWnd:GetDlgChild("lowstone"),BUTTON_LCLICK,0,0)
	end
	
	--宝石合成功能
	function TestStone:test_stonecompound()
--		local StoneCompoundWnd = CStoneCompound.GetWnd()
--		Gac2Gas:GM_Execute( g_Conn, "$additem(14,\"一级吉祥宝石\",16)" )
--		local str = WaitEvent(true,nil,Event.Test.AddItem)
--		SetEvent(Event.Test.AddItem,false)
--
--		assert_true(StoneCompoundWnd:IsShow(),"[StoneCompoundISShow]")
--		WndMainBag = g_GameMain.m_WndMainBag
--		 --合成
--		WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 0)
--		StoneCompoundWnd:OnCtrlmsg(StoneCompoundWnd:GetDlgChild("lowstone"),BUTTON_LCLICK,0,0)
--		StoneCompoundWnd:OnCtrlmsg(StoneCompoundWnd:GetDlgChild("batchcompound"),BUTTON_LCLICK,0,0)
--		local strRet = WaitEvent(true,nil,Event.Test.ReturnSynthesisItemEnd) --合成的返回
--		SetEvent(Event.Test.ReturnSynthesisItemEnd,false)
--		StoneCompoundWnd:OnCtrlmsg(StoneCompoundWnd:GetDlgChild("lowstone"),BUTTON_LCLICK,0,0)
--		StoneCompoundWnd:ShowWnd(false) --关闭合成面板
	end
	
	--白宝石鉴定
	--[[
	function TestStone:test_whiteStone()
		local Whitestone = CWhiteStone.GetWnd()
		Whitestone:ShowWnd(true) --打开白宝石鉴定面板
		Gac2Gas:GM_Execute( g_Conn, "$additem(18,\"白宝石\",20)" )
		local str = WaitEvent(true,nil,Event.Test.AddItem)
		SetEvent(Event.Test.AddItem,false)
		assert_true(Whitestone:IsShow(),"[WhiteStoneISShow]")
		WndMainBag = g_GameMain.m_WndMainBag
		 --鉴定
		for i = 1, 20 do
			WndMainBag:OnCtrlmsg(WndMainBag.m_ctItemRoom, ITEM_LBUTTONCLICK, 0, 4)
			Whitestone:OnCtrlmsg(Whitestone:GetDlgChild("Fromstone"),BUTTON_LCLICK,0,0)
			Whitestone:OnCtrlmsg(Whitestone:GetDlgChild("jianding"),BUTTON_LCLICK,0,0)
			local strRet = WaitEvent(true,nil,Event.Test.RetTakeAppraisedStone) --鉴定的返回
			SetEvent(Event.Test.RetTakeAppraisedStone,false)
			Whitestone:OnCtrlmsg(Whitestone:GetDlgChild("jianding"),BUTTON_LCLICK,0,0)
			
			local strRet = WaitEvent(true,nil,Event.Test.RetTakeAppraisedStone) --鉴定的返回
			SetEvent(Event.Test.RetTakeAppraisedStone,false)
			--提取鉴定得到的宝石 
			Whitestone:OnCtrlmsg(Whitestone:GetDlgChild("Tostone"),BUTTON_LCLICK,0,0)
			local strRet1 = WaitEvent(true,nil,Event.Test.RetTakeAppraisedStone) --提取的返回
			SetEvent(Event.Test.RetTakeAppraisedStone,false)
			--Whitestone:OnCtrlmsg(Whitestone:GetDlgChild("Fromstone"),BUTTON_LCLICK,0,0)
		end
		Whitestone:ShowWnd(false)  --关闭白宝石鉴定面板
	end
	--]]
	function TestStone:TestEnd()
		controler:LoginOutFromGame()
	end
	
	function TestStone:teardown()
	end
end
