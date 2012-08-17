CAreaFbListWnd = class(SQRDialog)
local CAreaFbListWndItem = class(SQRDialog)

function CAreaFbListWnd:Ctor(parent)
	self:CreateFromRes("Fb_DareQuestList",parent)
	self.m_CloseBtn = self:GetDlgChild("Close")
	self.m_OkBtn = self:GetDlgChild("Ok")
	self.m_ListInfo = self:GetDlgChild("LisetCtrl")
	
	self.m_AreaFbNameList = {}
	
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
	self:ShowWnd(false)
end

function CAreaFbListWnd:GetWnd()
	local Wnd = g_GameMain.m_DareFbListWnd
	if not Wnd then
		Wnd = CAreaFbListWnd:new(g_GameMain)
		g_GameMain.m_DareFbListWnd = Wnd
	end
	return Wnd
end

function CAreaFbListWnd:CreateListItem(parent)
	local wnd = CAreaFbListWndItem:new()
	wnd:CreateFromRes("MatchGameSelWndListItem", parent)
	wnd:ShowWnd( true )
	return wnd
end

function CAreaFbListWnd:InsertListInfo(sceneName,QuestName)
	if sceneName then
		local ItemCount = self.m_ListInfo:GetItemCount()
		self.m_ListInfo:InsertItem(ItemCount, 25)
		local Item = self.m_ListInfo:GetSubItem(ItemCount, 0)
		Item:SetMouseWheel(true)
		local itemWnd = self:CreateListItem(Item)
		local str = sceneName.."("..g_GetLanQuestName(QuestName)..")"
		itemWnd:SetWndText(str)
		Item.m_Wnd = itemWnd
		Item.m_SceneName = sceneName
		Item.m_QuestName = QuestName
	end
end

function CAreaFbListWnd:InitAreaFbSelWnd(FbList)
	self.m_AreaFbName = nil
	self.m_QuestName = nil
	
	self:ShowFbList(FbList)
	
	self:ShowWnd(true)
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(self)
end

-- 显示副本列表
function CAreaFbListWnd:ShowFbList( FbList )
	--清除列表
	self.m_ListInfo:DeleteAllItem()
	self.m_ListInfo:InsertColumn(0, self.m_ListInfo:GetWndOrgWidth())
	
	-- 插入列表
	for i=1, #(FbList) do
		local SceneName,QuestName = FbList[i][1],FbList[i][2]
		self:InsertListInfo(SceneName,QuestName)
	end
end

function CAreaFbListWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_OkBtn then
			if self.m_AreaFbName then
				Gac2Gas:AgreedJoinDareFb(g_Conn,self.m_AreaFbName,self.m_QuestName)
			end
			self:ShowWnd(false)
		elseif Child == self.m_CloseBtn then
			self:ShowWnd(false)
		end
	elseif uMsgID == ITEM_LBUTTONCLICK or uMsgID == ITEM_SHIFTDOWN or uMsgID == ITEM_CTRLDOWN then
		if Child == self.m_ListInfo then
			local Item = self.m_ListInfo:GetSubItem(uParam1, 0)
			self.m_AreaFbName = Item.m_SceneName
			self.m_QuestName = Item.m_QuestName
		end
	end
end

function CAreaFbListWnd:RetAreaFbListBegin(SceneName, QuestName)
	table.insert(self.m_AreaFbNameList,{SceneName,QuestName})
end

function CAreaFbListWnd:RetAreaFbListEnd()
	if next(self.m_AreaFbNameList) then
		self:InitAreaFbSelWnd(self.m_AreaFbNameList)
	else
		--当前没有挑战任务副本存在！
		MsgClient(3042)
	end
	self.m_AreaFbNameList = {}
end
