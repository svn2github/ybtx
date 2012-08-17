cfg_load "tong/TongPrivilegeTrack_Common"
cfg_load "tong/TongPrivilege_Common"
lan_load "tong/Lan_TongPrivilege_Common"

CTongPrivilegeWnd		= class(SQRDialog)	--导出类
CTongPrivilegeListItem	= class(SQRDialog)

function CTongPrivilegeWnd.CreateTongPrivilegeWnd(parent)
	local wnd = CTongPrivilegeWnd:new()
	wnd:Init(parent)
	return wnd
end

function CTongPrivilegeWnd:Init(parent)
	self.m_bFirstOpen = true
	self.m_tblTongLog = {}
	self:CreateFromRes("TongPrivilege", parent)
end

function CTongPrivilegeWnd:OnChildCreated()
	self.m_List = self:GetDlgChild("List")
end

--消息
function CTongPrivilegeWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )

end

function CTongPrivilegeWnd:OpenPanel(bFlag)
	if(self.m_bFirstOpen and bFlag) then
		self:DrawList()
		self.m_bFirstOpen = false
	end
	self:ShowWnd(bFlag)
end

----------------------------------------------------------------
function CTongPrivilegeWnd:DrawList()
	self.m_List:DeleteAllItem()
	self.tblItemWnd = {}
	self.m_List:InsertColumn(0, self.m_List:GetWndOrgWidth())
	for i, v in ipairs( Lan_TongPrivilege_Common:GetKeys() ) do
		self.m_List:InsertItem(i-1, 30)
		local item		= self.m_List:GetSubItem(i-1, 0)
		local itemWnd	= self:CreateTongPrivilegeItemWnd(item)
		table.insert(self.tblItemWnd, itemWnd)
		
		itemWnd:SetInfo(v,
						Lan_TongPrivilege_Common(v, "DisplayName"),
						Lan_TongPrivilege_Common(v, "DisplayLevel"),
						Lan_TongPrivilege_Common(v, "DisplayTime"),
						Lan_TongPrivilege_Common(v, "DisplayExpend"),
						Lan_TongPrivilege_Common(v, "DisplayAward"),
						Lan_TongPrivilege_Common(v, "DisplayExecutive")
		)
	end
end

------创建日志列表的行窗体
function CTongPrivilegeWnd:CreateTongPrivilegeItemWnd(parent)
	local wnd = CTongPrivilegeListItem:new()
	wnd:CreateFromRes("TongPrivilegeListItem", parent)
	wnd:ShowWnd(true)
	return wnd
end

--*************************
------list的子窗体类-------
--*************************
function CTongPrivilegeListItem:OnChildCreated()
	self.m_Name			= self:GetDlgChild("Name")
	self.m_Level		= self:GetDlgChild("Level")
	self.m_Time			= self:GetDlgChild("Time")
	self.m_Expend		= self:GetDlgChild("Expend")
	self.m_Award		= self:GetDlgChild("Award")
	self.m_Executive	= self:GetDlgChild("Executive")
end

function CTongPrivilegeListItem:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == RICH_CLICK ) then
		if(Child == self.m_Executive) then
			local value = self.m_Executive:GetChooseStrAttr()
			local SceneName = TongPrivilegeTrack_Common(value, "SceneName")
			local PosX = TongPrivilegeTrack_Common(value, "PosX")
			local PosY = TongPrivilegeTrack_Common(value, "PosY")
			local Mark = TongPrivilegeTrack_Common(value, "Mark")
			PlayerAutoTrack(Mark, SceneName, PosX, PosY)
		end
	end
end

function CTongPrivilegeListItem:SetInfo(nIndex, sName, nLevel, sTime, sExpend, sAward, sExecutive)
	self.m_Name:SetWndText(sName)
	self.m_Level:SetWndText(nLevel)
	self.m_Time:SetWndText(sTime)
	self.m_Expend:SetWndText(sExpend)
	self.m_Award:SetWndText(sAward)
	self.m_Executive:SetWndText(self:GetLinkString(nIndex, sExecutive))
end

function CTongPrivilegeListItem:GetLinkString(nIndex, sExecutive)
	local str = sExecutive
	local sLink = TongPrivilege_Common(nIndex, "Link")
	if(sLink and "" ~= sLink) then
		if( string.find(sLink, "#camp#") ) then
			local nCamp = g_MainPlayer:CppGetBirthCamp()
			sLink = string.gsub(sLink, "#camp#", nCamp)
		end
		str = AutoTrackColorStr.."#u#l"..sExecutive.."#i[".. MemH64(sLink) .."]#l#u#W"
	end
	return str
end