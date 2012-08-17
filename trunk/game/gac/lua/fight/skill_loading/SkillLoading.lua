gac_require "fight/skill_loading/SkillLoadingInc"
gac_require "fight/skill_loading/BaseSkillLoading"

function CreateSkillLoading( Parent )
	local Wnd = CSkillLoading:new()
	Wnd:CreateFromRes( "skill_Loading", Parent )
	Wnd.m_ProgressLoad = Wnd:GetDlgChild( "Progress_Load" )
	Wnd:InitSkillLoading(Wnd.m_ProgressLoad)

	return Wnd
end

function CSkillLoading:OnSelfCastBegin( FolObj, skillname, flag ,time ,runedtime)
	self.m_Skillname = skillname
	local DisplayName = self:GetSkillDisPlayName(skillname)
	self.m_ProgressLoad:SetWndText(DisplayName)
	self:OnCastBegin( FolObj, skillname, flag ,time ,runedtime)
end

--Ò÷³ªÖÐ¶Ï
function CSkillLoading:OnSelfCastEnd()
	--»Ö¸´ÒÆ¶¯
	self:OnCastEnd()
	self:ShowWnd(false)
	g_GameMain.m_SysSetting.m_KPmap.m_dir = nil
	g_GameMain.m_SysSetting.m_KPmap:Moving()
end

function CSkillLoading:GetCastingProcessLeftTime()
	if self.m_Time == 0 then
		return 0
	else
		return self.m_Time-self.m_RunedTime
	end
end