gac_require "resource/productive_profession/LiveSkillDesInc"
lan_load "liveskill/Lan_LiveSkillName_Client"
local SkillCommonMgr = CLiveSkillMgr:new()

function CreateLiveSkillDesWnd(parent)
	local wnd = CLiveSkillDesWnd:new()
	wnd:CreateFromRes("LiveSkillDes", parent)
	wnd:ShowWnd(true)
	g_ExcludeWndMgr:InitExcludeWnd(wnd, 2)
	return wnd
end

function CLiveSkillDesWnd:OnChildCreated()
	self.m_XBtn			= self:GetDlgChild("XBtn")
	self.m_Title		= self:GetDlgChild("Title")
	self.m_Level		= self:GetDlgChild("Level")
	self.m_ExpScroll	= self:GetDlgChild("ExpScroll")
	self.m_Des			= self:GetDlgChild("Des")
end

function CLiveSkillDesWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_XBtn) then
			self:ShowWnd(false)
		end
	end
end

function CLiveSkillDesWnd:VirtualExcludeWndClosed()
	self:DestroyWnd()
	g_GameMain.m_LiveSkillDesWnd = nil
end

function CLiveSkillDesWnd:UpdateInfo(tblInfo)
	local sName, nLevel, nExp = unpack(tblInfo)
	local tbl = {["²É¿ó"] = GetStaticTextClient(6013), ["»¨»Ü"] = GetStaticTextClient(6012)}
	self.m_Title:SetWndText( Lan_LiveSkillName_Client(MemH64(sName), "DisplayName") )
	self.m_Level:SetWndText(GetStaticTextClient(8322,nLevel))
	self.m_Des:SetWndText(tbl[sName])
	
	local nextLevelExpNeed	= SkillCommonMgr:GetSkillInfoByName(sName, nLevel + 1,"ExperienceNeed")
	local curLevelExpNeed	= SkillCommonMgr:GetSkillInfoByName(sName, nLevel, "ExperienceNeed")
	if(nextLevelExpNeed) then
		self.m_ExpScroll:SetRange(nextLevelExpNeed - curLevelExpNeed)
		self.m_ExpScroll:SetPos(nExp - curLevelExpNeed)
		self.m_ExpScroll:SetWndText(nExp .. "/" .. nextLevelExpNeed)
	else
		self.m_ExpScroll:SetRange(1)
		self.m_ExpScroll:SetPos(1)
		self.m_ExpScroll:SetWndText(nExp .. "/" .. nExp)
	end
end