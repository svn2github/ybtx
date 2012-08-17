gac_require "fight/skill_wnd/LearnedSkillShowWndInc"

function CreateLearnedSkillShowWnd(Parent)
	local wnd = CLearnedSkillShowWnd:new()
	wnd:CreateFromRes("LearnedSkillShowWnd",Parent)
	wnd:ShowWnd(true)
	return wnd
end 


function CLearnedSkillShowWnd:OnChildCreated()
	self.m_LearnedSkillBtnTbl = {}
	self.m_BgBtnTbl = {}
	for i = 1, 6 do
		local Btn = self:GetDlgChild("SkillPictureBtn" .. i)
		Btn.m_HavePicture = false
		Btn.m_Index = i
		table.insert(self.m_LearnedSkillBtnTbl,Btn)
		Btn:ShowWnd(false)
		local bgBtn = self:GetDlgChild("SkillBtn" .. i)
		bgBtn:ShowWnd(false)
		table.insert(self.m_BgBtnTbl,bgBtn)
	end
end

local function SetPickupSkill(wnd)
	local state = g_WndMouse:GetCursorState()
	if (state == ECursorState.eCS_Normal) then
		local tbl = {}
		tbl.SkillName  = wnd.m_SkillName 
		tbl.Level = wnd.m_SkillLevel
		g_WndMouse:SetPickupSkill( tbl, wnd )
	end
end


function CLearnedSkillShowWnd:OnCtrlmsg(Child,uMsgID,uParam1,uParam2)
	if uMsgID == BUTTON_LCLICK or uMsgID == BUTTON_DRAG then
		SetPickupSkill(Child)
		Child.m_HavePicture = false
		Child.m_SkillName = nil
		self.m_BgBtnTbl[Child.m_Index]:ShowWnd(false)
		Child:ShowWnd(false)
	end 
end

function CLearnedSkillShowWnd:ClearAllGenius()
	for i = 1,6 do
		if self.m_LearnedSkillBtnTbl[i]:IsShow() then
			local skillName = self.m_LearnedSkillBtnTbl[i].m_SkillName
			local uSkillLearnType = SkillPart_Common(skillName,"SkillLearnType")
			if uSkillLearnType == 1 then
				self.m_BgBtnTbl[i]:ShowWnd(false)
				self.m_LearnedSkillBtnTbl[i].m_HavePicture = false
				self.m_LearnedSkillBtnTbl[i]:ShowWnd(false)
				self.m_LearnedSkillBtnTbl[i].m_SkillName = nil
			end
		end
	end
end

function CLearnedSkillShowWnd:SetSkillBtnPicture(skillIcon,skillName,skillLevel,uFlag)
	local IsShow = SkillPart_Common(skillName,"IsShow")
	if tonumber(IsShow) == 1 then
		return 
	end
	for i = 1, 6 do
		if self.m_LearnedSkillBtnTbl[i].m_SkillName == skillName then
			self.m_LearnedSkillBtnTbl[i].m_SkillLevel = skillLevel
			g_LoadIconFromRes(skillIcon, self.m_LearnedSkillBtnTbl[i],-1,IP_ENABLE, IP_CLICKDOWN)
			self.m_LearnedSkillBtnTbl[i]:SetMouseOverDescAfter(g_Tooltips:GetWndSkillTips(skillName,skillLevel))
			return
		end
	end

	if PosSkillNameTbl[skillName] then
		g_GameMain.m_BeastPoseWnd:SetSkillBtnPicture(skillIcon,skillName,skillLevel,PosSkillNameTbl[skillName])
	end
	
	if uFlag == 1 then --为0代表学习，1代表升级
		return
	end		
	
	if not self.m_LearnedSkillBtnTbl[1].m_HavePicture then
		self.m_LearnedSkillBtnTbl[1]:ShowWnd(true)
		self.m_BgBtnTbl[1]:ShowWnd(true)
		self.m_LearnedSkillBtnTbl[1].m_SkillName = skillName
		self.m_LearnedSkillBtnTbl[1].m_SkillLevel = skillLevel
		self.m_LearnedSkillBtnTbl[1].m_HavePicture = true
		self.m_LearnedSkillBtnTbl[2].m_HavePicture = false
		g_LoadIconFromRes(skillIcon, self.m_LearnedSkillBtnTbl[1],-1,IP_ENABLE, IP_CLICKDOWN)
		self.m_LearnedSkillBtnTbl[1]:SetMouseOverDescAfter(g_Tooltips:GetWndSkillTips(skillName,skillLevel))
	elseif not self.m_LearnedSkillBtnTbl[2].m_HavePicture then
		self.m_LearnedSkillBtnTbl[2]:ShowWnd(true)
		self.m_BgBtnTbl[2]:ShowWnd(true)
		self.m_LearnedSkillBtnTbl[2].m_SkillName = skillName
		self.m_LearnedSkillBtnTbl[2].m_SkillLevel = skillLevel
		self.m_LearnedSkillBtnTbl[2].m_HavePicture = true
		self.m_LearnedSkillBtnTbl[3].m_HavePicture = false
		g_LoadIconFromRes(skillIcon, self.m_LearnedSkillBtnTbl[2],-1,IP_ENABLE, IP_CLICKDOWN)
		self.m_LearnedSkillBtnTbl[2]:SetMouseOverDescAfter(g_Tooltips:GetWndSkillTips(skillName,skillLevel))
	elseif not self.m_LearnedSkillBtnTbl[3].m_HavePicture then
		self.m_LearnedSkillBtnTbl[3]:ShowWnd(true)
		self.m_BgBtnTbl[3]:ShowWnd(true)
		self.m_LearnedSkillBtnTbl[3].m_SkillName = skillName
		self.m_LearnedSkillBtnTbl[3].m_SkillLevel = skillLevel
		self.m_LearnedSkillBtnTbl[3].m_HavePicture = true
		self.m_LearnedSkillBtnTbl[4].m_HavePicture = false
		g_LoadIconFromRes(skillIcon, self.m_LearnedSkillBtnTbl[3],-1,IP_ENABLE, IP_CLICKDOWN)
		self.m_LearnedSkillBtnTbl[3]:SetMouseOverDescAfter(g_Tooltips:GetWndSkillTips(skillName,skillLevel))
	elseif not self.m_LearnedSkillBtnTbl[4].m_HavePicture then
		self.m_LearnedSkillBtnTbl[4]:ShowWnd(true)
		self.m_BgBtnTbl[4]:ShowWnd(true)
		self.m_LearnedSkillBtnTbl[4].m_SkillName = skillName
		self.m_LearnedSkillBtnTbl[4].m_SkillLevel = skillLevel
		self.m_LearnedSkillBtnTbl[4].m_HavePicture = true
		self.m_LearnedSkillBtnTbl[5].m_HavePicture = false
		g_LoadIconFromRes(skillIcon, self.m_LearnedSkillBtnTbl[4],-1,IP_ENABLE, IP_CLICKDOWN)
		self.m_LearnedSkillBtnTbl[4]:SetMouseOverDescAfter(g_Tooltips:GetWndSkillTips(skillName,skillLevel))
	elseif not self.m_LearnedSkillBtnTbl[5].m_HavePicture then
		self.m_LearnedSkillBtnTbl[5]:ShowWnd(true)
		self.m_BgBtnTbl[5]:ShowWnd(true)
		self.m_LearnedSkillBtnTbl[5].m_SkillName = skillName
		self.m_LearnedSkillBtnTbl[5].m_SkillLevel = skillLevel
		self.m_LearnedSkillBtnTbl[5].m_HavePicture = true
		self.m_LearnedSkillBtnTbl[6].m_HavePicture = false
		g_LoadIconFromRes(skillIcon, self.m_LearnedSkillBtnTbl[5],-1,IP_ENABLE, IP_CLICKDOWN)
		self.m_LearnedSkillBtnTbl[5]:SetMouseOverDescAfter(g_Tooltips:GetWndSkillTips(skillName,skillLevel))
	elseif not self.m_LearnedSkillBtnTbl[6].m_HavePicture then
		self.m_LearnedSkillBtnTbl[6]:ShowWnd(true)
		self.m_BgBtnTbl[6]:ShowWnd(true)
		self.m_LearnedSkillBtnTbl[6].m_SkillName = skillName
		self.m_LearnedSkillBtnTbl[6].m_SkillLevel = skillLevel
		self.m_LearnedSkillBtnTbl[6].m_HavePicture = true
		self.m_LearnedSkillBtnTbl[1].m_HavePicture = false
		g_LoadIconFromRes(skillIcon, self.m_LearnedSkillBtnTbl[6],-1,IP_ENABLE, IP_CLICKDOWN)
		self.m_LearnedSkillBtnTbl[6]:SetMouseOverDescAfter(g_Tooltips:GetWndSkillTips(skillName,skillLevel))
	end
end