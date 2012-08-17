cfg_load "skill/SkillPart_Common"

CTalentSkillPartWnd = class(SQRDialog)

--使鼠标添加相应的贴图
local function SetPickupSkill(Wnd)
	local state = g_WndMouse:GetCursorState()
	if(state == ECursorState.eCS_Normal) then
		local tbl = {}
			tbl.SkillName  = Wnd.m_SkillName 
			tbl.Level = Wnd.m_SkillLevel
		g_WndMouse:SetPickupSkill( tbl, Wnd )
	end
end

function CTalentSkillPartWnd:Ctor()
	self:CreateFromRes("TalentSkillPartWnd",g_GameMain.m_CommonSkillWnd)
	self.m_StaticWnd = self:GetDlgChild("StaticWnd")
	self.m_TalentNum = self:GetDlgChild("TalentNum")
	self.m_OpenTalentBtn = self:GetDlgChild("OpenTalentBtn")
	self.m_SkillBtnTbl = {}
	for i = 1 , 15 do 
		local Btn = self:GetDlgChild("TalentSkillBtn" .. i)
		Btn.m_IsLearned = false
		Btn.m_SkillLevel = 0
		g_DelWndImage(Btn,-1,IP_ENABLE,IP_CLICKDOWN )
		table.insert(self.m_SkillBtnTbl,Btn)
	end
	self.m_FlashBtnNumber = 0
	self.m_IsShow = false
end

function CTalentSkillPartWnd:SetDefaultInfo()
	for i =1 ,#(self.m_SkillBtnTbl) do
		self.m_SkillBtnTbl[i].m_IsLearned = false
		self.m_SkillBtnTbl[i].m_SkillLevel = 0
		self.m_SkillBtnTbl[i]:SetMouseOverDescAfter("")
		g_DelWndImage(self.m_SkillBtnTbl[i],-1,IP_ENABLE,IP_CLICKDOWN )
	end
end

function CTalentSkillPartWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if	uMsgID == BUTTON_DRAG then
		if Child.m_IsLearned then
			SetPickupSkill(Child)
			self:DelBtnFlash(Child)
		end
	elseif uMsgID == BUTTON_LCLICK then
		if Child == self.m_OpenTalentBtn then
			g_GameMain.m_SelectSeriesWnd:ShowFun()
			g_GameMain.m_CommonSkillWnd:ShowWnd(false)
			g_GameMain.m_SelectSeriesWnd:ShowWnd(true)
		elseif Child.m_IsLearned then
			SetPickupSkill(Child)
			self:DelBtnFlash(Child)
		end
	end
end

function CTalentSkillPartWnd:FindBtnFromSkillPartWnd()
	for j = 1,#(self.m_SkillBtnTbl) do
		if not self.m_SkillBtnTbl[j].m_IsLearned then
			return self.m_SkillBtnTbl[j]
		end
	end
end

function CTalentSkillPartWnd:SetBtnFlash(Btn)
	Btn:AddFlashInfoByName("TalentItemFlash")
	Btn.m_IsFlashBtn = true
	if self.m_FlashBtnNumber <= 0 then
		self.m_FlashBtnNumber = 0
	end
	self.m_FlashBtnNumber = self.m_FlashBtnNumber + 1
	CFunctionBar.SetToolBarSkillBtnFlash()	
end

function CTalentSkillPartWnd:DelBtnFlash(Btn)
	if Btn.m_IsFlashBtn then
		Btn:DelFlashAttention()
		Btn.m_IsFlashBtn = nil
		self.m_FlashBtnNumber = self.m_FlashBtnNumber - 1
		CFunctionBar.DelToolBarSkillBtnFlash()
	end	
end

function CTalentSkillPartWnd:ClearAllBtnFlash()
	for i = 1, #(self.m_SkillBtnTbl) do
		self.m_SkillBtnTbl[i]:DelFlashAttention()
		self.m_SkillBtnTbl[i].m_IsFlashBtn = nil 
	end
	self.m_FlashBtnNumber = 0
end

function CTalentSkillPartWnd:SetTalentSkill(SkillName,SkillLevel,IsShowFlash)
	if SkillPart_Common(SkillName) then
		local SkillPartBtn = self:FindBtnFromSkillPartWnd()
		SkillPartBtn.m_SkillName = SkillName
		SkillPartBtn.m_SkillLevel = SkillLevel
		SkillPartBtn.m_IsLearned = true
		local skill = SkillPart_Common(SkillName)
		SkillPartBtn:SetMouseOverDescAfter(g_Tooltips:GetWndSkillTips(SkillName,SkillLevel))
		if IsShowFlash then 
			self:SetBtnFlash(SkillPartBtn)				
		end
		g_RePlaceWndImage(skill("SkillSmallIcon"), SkillPartBtn, 0, IP_ENABLE, IP_CLICKDOWN)
	end	
end

function CTalentSkillPartWnd:FindTalentSkillBtn(SkillName)
	for i = 1,#(self.m_SkillBtnTbl) do
		if self.m_SkillBtnTbl[i].m_SkillName == SkillName then
			return self.m_SkillBtnTbl[i]
		end
	end	
end

--更新天赋技能栏数据
function CTalentSkillPartWnd:UpdateTalentSkillInfo(SkillName,SkillLevel)
	local TalentSkillBtn = self:FindTalentSkillBtn(SkillName)
	if TalentSkillBtn then
		TalentSkillBtn.m_SkillLevel = SkillLevel
		TalentSkillBtn:SetMouseOverDescAfter(g_Tooltips:GetWndSkillTips(SkillName,SkillLevel))
	end
end

--更新天赋技能栏上的tooltips
function CTalentSkillPartWnd:UpdateTalentSkillTips()
	for j = 1,#(self.m_SkillBtnTbl) do
		if self.m_SkillBtnTbl[j].m_IsLearned then
			self.m_SkillBtnTbl[j]:SetMouseOverDescAfter(g_Tooltips:GetWndSkillTips(self.m_SkillBtnTbl[j].m_SkillName,self.m_SkillBtnTbl[j].m_SkillLevel))
		end
	end	
end


