CConfirmSecWnd = class(SQRDialog)

function CConfirmSecWnd:Ctor(parent,activityName,ItemName, num)
	self:CreateFromRes("FbConfirmWnd",parent) 
	self.m_OkBtn = self:GetDlgChild("OkBtn")
	self.m_CancelBtn = self:GetDlgChild("CancelBtn")
	self.m_FbGameInfo = self:GetDlgChild("info")
	self.m_ItemName = ItemName
	self.m_Num = num
	self.m_activityName = activityName
--	local ItemLanInfo = g_ItemInfoMgr:GetItemLanInfo(46, ItemName, "DisplayName")
--	local actionName = g_GetFbActionNameLanStr(activityName)
--	self.m_FbGameInfo:SetWndText(GetStaticTextClient(8007, actionName, ItemLanInfo, num))
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
end

function CConfirmSecWnd:GetWnd(activityName,ItemName, num)
	if not g_GameMain.m_FbConfirmSecWnd then
		g_GameMain.m_FbConfirmSecWnd = CConfirmSecWnd:new(g_GameMain,activityName,ItemName, num)
	end
	local wnd = g_GameMain.m_FbConfirmSecWnd
	local ItemLanInfo = g_ItemInfoMgr:GetItemLanInfo(46, ItemName, "DisplayName")
	local actionName = g_GetFbActionNameLanStr(activityName)
	wnd.m_FbGameInfo:SetWndText(GetStaticTextClient(8007, actionName, ItemLanInfo, num))
	wnd:ShowWnd(true)
	return Wnd
end

function CConfirmSecWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_OkBtn then
			Gac2Gas:UseFbSecItem(g_Conn, self.m_activityName, self.m_ItemName, self.m_Num)   --减物品,次数,加buff
		end
		self:ShowWnd(false)
	end
end
