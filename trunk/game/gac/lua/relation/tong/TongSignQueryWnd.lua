CSignQueryWnd = class(SQRDialog)
CQueryListItem = class(SQRDialog)

function CSignQueryWnd:Ctor(partent,tongName, tongCamp, exploitPoint, str)
	self:CreateFromRes("RobSignQueryWnd", partent)
	self.m_InfoList		= self:GetDlgChild("InfoList")
	self.m_Title			= self:GetDlgChild("Title")
	self.m_ExitBtn      = self:GetDlgChild("ExitBtn")
	self.m_CampInfo = self:GetDlgChild("CampInfo")
	self.m_NameInfo = self:GetDlgChild("NameInfo")
	self.m_ExploitInfo = self:GetDlgChild("ExploitInfo")
	self.m_Getter = self:GetDlgChild("Getter")
	self.m_SignBtn = self:GetDlgChild("SignBtn")
	self.m_CancelBtn = self:GetDlgChild("CancelBtn")
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
end

function CSignQueryWnd:GetWnd(tongName, tongCamp, exploitPoint, str)
	if not g_GameMain.m_SignQueryWnd then
		g_GameMain.m_SignQueryWnd = CSignQueryWnd:new(g_GameMain, tongName, tongCamp, exploitPoint, str)
	end
	local wnd = g_GameMain.m_SignQueryWnd
	wnd.m_Getter:SetWndText(str)
	wnd:OpenPanel(tongName, tongCamp, exploitPoint)
	wnd:ShowWnd(true)
	return wnd
end

function CSignQueryWnd:CreateNoneWnd(str)

	if not g_GameMain.m_SignQueryWnd then
		g_GameMain.m_SignQueryWnd = CSignQueryWnd:new(g_GameMain, tongName, tongCamp, exploitPoint, str)
	end
	local wnd = g_GameMain.m_SignQueryWnd
	wnd.m_Getter:SetWndText(str)
	--wnd:OpenPanel(tongName, tongCamp, exploitPoint)
	wnd:ShowWnd(true)
	return wnd
end

function CSignQueryWnd:Destroy()
	self:ShowWnd(false)
end


function CSignQueryWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_ExitBtn) then
			self:ShowWnd(false)
		elseif Child == self.m_SignBtn then
			self:Destroy()
			Gac2Gas:JoinRobResFb(g_Conn) --判断是否能够报名	
		elseif Child == self.m_CancelBtn then
			Gac2Gas:ShowCancelWnd(g_Conn) 
			self:Destroy()
		end
	end
end

function CSignQueryWnd:InitWnd()
	if not g_GameMain.m_SignQueryWnd then
		g_GameMain.m_SignQueryWnd = CSignQueryWnd:new(g_GameMain, tongName, tongCamp, exploitPoint)
	end
	g_GameMain.m_SignQueryWnd.m_InfoList:DeleteAllItem()
	g_GameMain.m_SignQueryWnd.m_InfoList:InsertColumn(0, g_GameMain.m_SignQueryWnd.m_InfoList:GetWndOrgWidth())
	g_GameMain.m_SignQueryWnd:ShowWnd(true)
end

function CSignQueryWnd:OpenPanel(tongName, tongCamp, exploitPoint)
	self:DrawMemberList(tongName, tongCamp, exploitPoint)
end

------帮会成员列表行----------------------------------------------------------------
function CSignQueryWnd:DrawMemberList(tongName, tongCamp, exploitPoint)
	local ItemCount1 = self.m_InfoList:GetItemCount()
	self.m_InfoList:InsertItem(ItemCount1, 30) 
	local item = self.m_InfoList:GetSubItem(ItemCount1, 0)
	local infoItem = self:CreateTongListItemWnd(item)
	infoItem:SetTongMemListText(tongName, tongCamp, exploitPoint)
	item.m_Wnd = infoItem
	self:ShowWnd(true)
end


------创建帮会成员列表的行窗体
function CSignQueryWnd:CreateTongListItemWnd(parent)
	local wnd = CQueryListItem:new()
	wnd:CreateFromRes("RobSignQueryList", parent)
	wnd:ShowWnd(true)
	return wnd
end

--*************************
------list的子窗体类-------
--*************************
function CQueryListItem:OnChildCreated()
	self.m_Camp		= self:GetDlgChild("Camp")
	self.m_Name	= self:GetDlgChild("Name")
	self.m_Point	= self:GetDlgChild("Point")
end

function CQueryListItem:SetTongMemListText(tongName, tongCamp, exploitPoint)
	if tongCamp == 1 then   
		tongCamp = GetStaticTextClient(2501)
	elseif tongCamp == 2 then
		tongCamp = GetStaticTextClient(2502)
	elseif tongCamp == 3 then
		tongCamp = GetStaticTextClient(2503)
	end
	self.m_Camp:SetWndText(tongCamp)
	self.m_Name:SetWndText(tongName)
	self.m_Point:SetWndText(exploitPoint)
end


