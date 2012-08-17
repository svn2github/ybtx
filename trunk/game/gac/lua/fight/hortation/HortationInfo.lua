CHortationInfoWnd = class( SQRDialog )

function CHortationInfoWnd.AddItemMsgToConn(Conn,ItemId)
	local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(ItemId)
	g_GameMain.m_BagShapeEffectWnd:ShapeEffectFun(DynInfo.m_nBigID,DynInfo.m_nIndex)
	local DisplayName = g_ItemInfoMgr:GetItemLanInfo(DynInfo.m_nBigID,DynInfo.m_nIndex,"DisplayName")
	local ItemStr = "[(" .. DisplayName .. ")" .. ItemId .. "]"
	local ColorStr = g_Tooltips:GetSuitNameColor(DynInfo,DynInfo.m_nBigID,DynInfo.m_nIndex)	
	ColorStr = string.gsub(ColorStr,"#","")
	local Info = "#W" .. GetStaticTextClient(1146) .. ModeConversion(ColorStr .. ItemStr,"#W")
	local Wnd = g_GameMain.m_CreateChatWnd.m_CChatWnd
	table.insert(Wnd.ChatInfoTbl[1] ,Info .. "#r")
	g_GameMain.m_CreateChatWnd.m_CChatWnd:ChatInfoShow()	
end

function CHortationInfoWnd.ReturnCharAddSoulMsg(Conn,soulNum)
	local Wnd = g_GameMain.m_CreateChatWnd.m_CChatWnd
	local Info = "#W" .. GetStaticTextClient(4573,soulNum) .. "#r"
	table.insert(Wnd.ChatInfoTbl[1] ,Info )
	g_GameMain.m_CreateChatWnd.m_CChatWnd:ChatInfoShow()
end

function CHortationInfoWnd.ShowSoulBottleAddSoulMsg(Conn,soulNum)
	local Wnd = g_GameMain.m_CreateChatWnd.m_CChatWnd
	local Info = "#W" .. GetStaticTextClient(4574,soulNum) .. "#r"
	table.insert(Wnd.ChatInfoTbl[1] ,Info )
	g_GameMain.m_CreateChatWnd.m_CChatWnd:ChatInfoShow()
end