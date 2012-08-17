---------------------------------------------------天赋技能描述面板(学习时弹出)------------------------------------------
lan_load "skill/Lan_SkillDesc_Common"
lan_load "skill/Lan_SkillPart_Common"
cfg_load "skill/SkillPart_Common"

CTalentSkillDescWnd = class(SQRDialog)

function CTalentSkillDescWnd:Ctor()
	self:CreateFromRes("SkillDescWnd",g_GameMain)
	self.m_SkillName = self:GetDlgChild("SkillName")
	self.m_DescRichText1 = self:GetDlgChild("DescRichText1")
	self.m_DescRichText2 = self:GetDlgChild("DescRichText2")
	self.m_SkillIconBtn = self:GetDlgChild("SkillIconBtn")
end

local function CloseSkillDescWndFun(Tick,Wnd)
	if Wnd.m_CloseSkillDescWndTick then
		UnRegisterTick(Wnd.m_CloseSkillDescWndTick )
	end
	Wnd.m_CloseSkillDescWndTick = nil
	Wnd:SetTransparentObj(500, true)
end

function CTalentSkillDescWnd:OnLButtonClick( nFlags, x, y )
	self:SetTransparentObj(500, true)
end

function CTalentSkillDescWnd:SetDescInfo(SkillName) 
	if SkillName and IsString(SkillName)then 
		if SkillPart_Common(SkillName) and Lan_SkillDesc_Common(MemH64(SkillName)) then
			if self.m_CloseSkillDescWndTick then
				UnRegisterTick(self.m_CloseSkillDescWndTick)
				self.m_CloseSkillDescWndTick = nil
			end
			if g_GameMain and g_GameMain.m_LearnCommonSkillWnd:IsShow() then
				self:InserBefore(g_GameMain.m_LearnCommonSkillWnd)
			end
			if g_GameMain and g_GameMain.m_SelectSeriesWnd:IsShow() then 
				self:InserBefore(g_GameMain.m_SelectSeriesWnd)
			end
			self:AddFlashInfoByName("skillDesc")
			local DisplayName = ""
			local SmallIcon = 866
			if Lan_SkillPart_Common(MemH64(SkillName)) then
				DisplayName = Lan_SkillPart_Common(MemH64(SkillName),"DisplayName")
				SmallIcon =	SkillPart_Common(SkillName,"SkillSmallIcon")
			else
				local ClassID = g_MainPlayer:CppGetClass()
				Lan_GiftSkill_Common = loadGeniusfile(ClassID,true)
				GiftSkill_Common = loadGeniusfile(ClassID)
				if Lan_GiftSkill_Common(MemH64(SkillName)) then
					DisplayName = Lan_GiftSkill_Common(MemH64(SkillName),"DisplayName")
					SmallIcon = GiftSkill_Common(SkillName,"SkillSmallIcon")
				end		 
			end
			if SmallIcon then
				g_LoadIconFromRes(SmallIcon,self.m_SkillIconBtn,-1,IP_ENABLE, IP_CLICKDOWN)
			end
			self.m_SkillName:SetWndText(DisplayName)
			self.m_DescRichText1:SetWndText(Lan_SkillDesc_Common(MemH64(SkillName),"SkillDesc1"))
			self.m_DescRichText2:SetWndText(Lan_SkillDesc_Common(MemH64(SkillName),"SkillDesc2"))
			self:EndTransparent(false)
			self:SetTransparent(1)
			self:ShowWnd(true)
			self.m_CloseSkillDescWndTick = RegisterTick("CloseSkillDescWndTick",CloseSkillDescWndFun,10000,self)
		end
	end
end