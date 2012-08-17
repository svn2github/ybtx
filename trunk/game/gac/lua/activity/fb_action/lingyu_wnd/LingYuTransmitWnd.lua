cfg_load "fb_game/LingYuTransmit_Common"
lan_load "fb_game/Lan_LingYuTransmit_Common"

CLingYuTransmitWnd = class( SQRDialog )
local CLingYuTransmitItem = class( SQRDialog )

function CLingYuTransmitWnd:Ctor(parent)
	self:CreateFromRes("LingYuTransmitWnd", parent)
	self.m_List = self:GetDlgChild("List")
	self.m_CancelBtn = self:GetDlgChild("CancelBtn")
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
	self:ShowWnd(false)
end

function CLingYuTransmitWnd:GetWnd()
	local Wnd = g_GameMain.m_LingYuTransmitWnd
	if not Wnd then
		Wnd = CLingYuTransmitWnd:new(g_GameMain)
		g_GameMain.m_LingYuTransmitWnd = Wnd
	end
	return Wnd
end

local function CreateLingYuTransmitItem(parent)
	local Wnd = CLingYuTransmitItem:new()
	Wnd:CreateFromRes("LingYuTransmitItem", parent)
	Wnd.m_Text = Wnd:GetDlgChild("Text")
	return Wnd
end

function CLingYuTransmitWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if (uMsgID == ITEM_LBUTTONCLICK) then
		local Item = self.m_List:GetSubItem(uParam1, 0)
		Gac2Gas:EnterScopesFb(g_Conn, Item.m_SceneName)
		self:ShowWnd(false)
	end
	if (uMsgID == BUTTON_LCLICK) then
		if Child == self.m_CancelBtn then
			self:ShowWnd(false)
		end
	end
end

function CLingYuTransmitWnd:AddItem(Text, QuestName, SceneName)
	local ItemNum = self.m_List:GetItemCount()
	self.m_List:InsertItem(ItemNum, 30)
	local Item = self.m_List:GetSubItem(ItemNum, 0)
	local ItemWnd = CreateLingYuTransmitItem(Item)
	self.m_List:SetItemHeight(ItemWnd:GetWndOrgHeight(), ItemNum)
	ItemWnd.m_Text:SetWndText(Text)
	ItemWnd:ShowWnd(true)
	Item:SetMouseWheel(true)
	Item.m_SceneName = SceneName
	Item.m_QuestName = QuestName
	Item.m_Wnd = ItemWnd
end

function CLingYuTransmitWnd:ShowLingYuTransmitWnd()
	self.m_List:DeleteAllItem()
	self.m_List:InsertColumn(0, self.m_List:GetWndOrgWidth())
	local QuestTbl = {}
	local SceneTbl = {}
	for QuestName, tbl in pairs(g_GameMain.m_QuestRecordWnd.m_QuestStateTbl) do
		if tbl.m_State == QuestState.init then
			QuestTbl[QuestName] = true
		end
	end
	
	local keys = LingYuTransmit_Common:GetKeys()
	for k=1, table.getn(keys) do
		local SceneName = keys[k]
		local Text = Lan_LingYuTransmit_Common(MemH64(SceneName), "Text")
		if g_MapQuestMgr[SceneName] and Text and Text ~= "" then
			for i=1,#(g_MapQuestMgr[SceneName]) do
				local QuestName = g_MapQuestMgr[SceneName][i]
				if QuestTbl[ QuestName ] then
					self:AddItem(Text, QuestName, SceneName)
					SceneTbl[SceneName] = true
					break
				end
			end
		end
	end
	self:ShowWnd(true)
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(self)
end

--确定进副本的消息(领域副本)
function CLingYuTransmitWnd.RetIsJoinScopesFb(SceneName, PosX, PosY)

	local function IsJoinScopesFb(wnd, btnIndex)
		if btnIndex == 1 then
			Gac2Gas:AgreedJoinScopesFb(g_Conn, SceneName, PosX, PosY)
		end
		wnd:Close()
		g_GameMain.m_ChangeOutMsgBox = nil
	end
	
	if g_GameMain.m_ChangeOutMsgBox then
		g_GameMain.m_ChangeOutMsgBox:Close()
		g_GameMain.m_ChangeOutMsgBox = nil
	end
	
	local msg = MsgBoxMsg(5001, GetSceneDispalyName(SceneName))
	msg = msg.." (%d)"
	local OkStr = GetStaticTextClient(1124)
	local CancelStr = GetStaticTextClient(1025)
	g_GameMain.m_ChangeOutMsgBox = CCountdownWnd:new(g_GameMain, msg, 30, IsJoinScopesFb, {OkStr, CancelStr})
	
end
