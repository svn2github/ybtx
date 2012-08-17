gac_require "resource/productive_profession/LiveSkillProdMainInc"
gac_require "resource/productive_profession/LiveSkillProdPracticed"
lan_load "liveskill/Lan_LiveSkillName_Client"

gac_require "resource/productive_profession/LiveSkillCookBase"

function CreateLiveSkillProdMainWnd(parent)
	local wnd = CLiveSkillProdMainWnd:new()
	wnd:Init(parent)
	g_ExcludeWndMgr:InitExcludeWnd(wnd, 2)
	return wnd
end

function CLiveSkillProdMainWnd:Init(parent)
	self.m_sType = ""
	self:CreateFromRes("LiveSkillProdMain", parent)
	self.m_LiveSkillProduction		= CreateLiveSkillCookBaseWnd(self.m_Static)
	self.m_LiveSkillProdPracticed	= CreateLiveSkillProdPracticedWnd(self.m_Static) --熟练度面板
	self:ChangePageTo(1)
end

function CLiveSkillProdMainWnd:OnChildCreated()
	self.m_XBtn			= self:GetDlgChild("XBtn")
	self.m_LevelLable	= self:GetDlgChild("LevelLable")
	self.m_EXPProgress	= self:GetDlgChild("EXPProgress")
	self.m_Static		= self:GetDlgChild("Static")
	self.m_EXPProgress:SetProgressMode(0)
	self.m_tblCheckBtn = {}
	for i = 1, 2 do
		self.m_tblCheckBtn[i] = self:GetDlgChild("CheckBtn" .. i)
	end
end

--消息
function CLiveSkillProdMainWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_XBtn) then
			self:OpenWnd(false)
		else
			for i, v in ipairs(self.m_tblCheckBtn) do
				if(Child == v) then
					self:ChangePage()
					break
				end
			end
		end
	end
end

function CLiveSkillProdMainWnd:ChangePageTo(nPage)
	self.m_tblCheckBtn[nPage]:SetCheck(true)
	self:ChangePage()
end

function CLiveSkillProdMainWnd:ChangePage()
	self.m_LiveSkillProduction:ShowWnd(self.m_tblCheckBtn[1]:GetCheck())
	self.m_LiveSkillProdPracticed:OpenWnd(self.m_tblCheckBtn[2]:GetCheck())
end

function CLiveSkillProdMainWnd:OpenWnd(bFlag, tblSkillInfo)
	if(bFlag) then
		self:UpdateInfo(tblSkillInfo)
		self.m_LiveSkillProduction:ChangeListPage(1, self.m_LiveSkillProduction.m_ListPage)
		self.m_LiveSkillProduction:OnClearWnd()
		self:ChangePageTo(1)
	end
	self:ShowWnd(bFlag)
end

function CLiveSkillProdMainWnd:UpdateInfo(tblSkillInfo)
	self.m_sType, self.m_nLevel, self.m_nEXP = unpack(tblSkillInfo)
	self:UpdateSkillNameShow()
	self:UpdateExpBar(tblSkillInfo)
	
	local nMaxPracticedValue = g_LiveSkillMgr:GetSkillInfoByName(self.m_sType, self.m_nLevel, "UpperSpecialize")
	self.m_LiveSkillProduction:UpdateInfo(tblSkillInfo)
	local nMaxPracticedValue = g_LiveSkillMgr:GetSkillInfoByName(self.m_sType, self.m_nLevel, "UpperSpecialize")
	self.m_LiveSkillProdPracticed:OnUpgreadProgress(nMaxPracticedValue)
end

function CLiveSkillProdMainWnd:UpdateExpBar(tblSkillInfo)
	_, _, self.m_nEXP = unpack(tblSkillInfo)
	
	local nextLevelExpNeed	= g_LiveSkillMgr:GetSkillInfoByName(self.m_sType, self.m_nLevel + 1, "ExperienceNeed")
	local curLevelExpNeed	= g_LiveSkillMgr:GetSkillInfoByName(self.m_sType, self.m_nLevel, "ExperienceNeed")
	if(nextLevelExpNeed) then
		self.m_EXPProgress:SetRange(nextLevelExpNeed - curLevelExpNeed)
		self.m_EXPProgress:SetPos(self.m_nEXP - curLevelExpNeed)
		self.m_EXPProgress:SetWndText(self.m_nEXP .. "/" .. nextLevelExpNeed)
	else
		self.m_EXPProgress:SetRange(1)
		self.m_EXPProgress:SetPos(1)
		self.m_EXPProgress:SetWndText(self.m_nEXP .. "/" .. self.m_nEXP)
	end
end

function CLiveSkillProdMainWnd:UpdateSkillNameShow()
	local sSkillDisplayName = Lan_LiveSkillName_Client(MemH64(self.m_sType), "DisplayName")
	self.m_LevelLable:SetWndText(GetStaticTextClient(8327, sSkillDisplayName, self.m_nLevel))
	self.m_tblCheckBtn[1]:SetWndText(sSkillDisplayName)
	local bFlag = "烹饪" ~= self.m_sType and "制药" ~= self.m_sType
	self.m_tblCheckBtn[1]:ShowWnd(bFlag)
	self.m_tblCheckBtn[2]:ShowWnd(bFlag)
end