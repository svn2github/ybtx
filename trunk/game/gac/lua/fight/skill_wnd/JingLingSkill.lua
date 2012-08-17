cfg_load "equip/advance/AttributeOfJingLingSkill_Common"
cfg_load "skill/SkillPart_Common"

CSkillJingLingWnd  = class(SQRDialog)

local JingLingSkillNumber = 6

--设置精灵技能信息到鼠标上
local function SetPickupJingLingSkill(Btn)
	local state = g_WndMouse:GetCursorState()
	if(state == ECursorState.eCS_Normal) then
		local tbl = {}
		tbl.SkillName  = Btn.m_SkillName 
		tbl.Level = Btn.m_SkillLevel
		g_WndMouse:SetPickupSkill( tbl, Btn )
	end
end

function CSkillJingLingWnd:Ctor()
	self:CreateFromRes("SkillWnd",g_GameMain.m_CommonSkillWnd)
end

function CSkillJingLingWnd:OnChildCreated()
	self.m_JingLingSkillBtnTbl = {}
	self.m_SelectSkillID = 0
	for i = 1, JingLingSkillNumber do
		local Btn = self:GetDlgChild("JingLingSkillBtn" .. i)
		Btn.m_SkillID = 0
		Btn.m_SkillLevel = 0
		Btn.m_SkillName = ""
		Btn.m_IsActiveSkill = false
		table.insert(self.m_JingLingSkillBtnTbl,Btn)
	end
end

function CSkillJingLingWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_DRAG  then
		if Child.m_IsActiveSkill then
			SetPickupJingLingSkill(Child)
		end
	elseif uMsgID == BUTTON_LCLICK then
		for i = 1, #(self.m_JingLingSkillBtnTbl) do
			if Child == self.m_JingLingSkillBtnTbl[i] 
				and (not Child.m_IsActiveSkill ) then
				g_WndMouse:ClearCursorAll()
				Gac2Gas:ActiveJingLingSkill(g_Conn, Child.m_SkillID)
				break
			end
		end
	end
end

function CSkillJingLingWnd:SetJingLingBtnInfo()
	local SkillAttributeTbl = AttributeOfJingLingSkill_Common
	local SkillIDTbl = g_GameMain.m_RoleStatus:GetJingLingSkillNameTbl()
	local IsJingLingActive = false
	self.m_IsShow = false
	for i = 1, JingLingSkillNumber do
		if SkillIDTbl[i] ~= nil then
			self.m_JingLingSkillBtnTbl[i].m_SkillName = SkillAttributeTbl(SkillIDTbl[i],"SkillName")
			self.m_JingLingSkillBtnTbl[i].m_SkillLevel = g_MainPlayer:CppGetLevel()		
			self.m_JingLingSkillBtnTbl[i].m_SkillID = SkillIDTbl[i]
			self.m_JingLingSkillBtnTbl[i]:SetMouseOverDescAfter(g_Tooltips:GetWndSkillTips(SkillAttributeTbl(SkillIDTbl[i],"SkillName"),g_MainPlayer:CppGetLevel()))
			local SmallIcon = SkillPart_Common(SkillAttributeTbl(SkillIDTbl[i],"SkillName"),"SkillSmallIcon")
			g_DelWndImage(self.m_JingLingSkillBtnTbl[i],-1,IP_ENABLE, IP_CLICKDOWN)
			if self.m_JingLingSkillBtnTbl[i].m_SkillID == self.m_SelectSkillID then 
				g_LoadIconFromRes(SmallIcon,self.m_JingLingSkillBtnTbl[i],-1,IP_ENABLE, IP_CLICKDOWN)
				self.m_JingLingSkillBtnTbl[i].m_IsActiveSkill = true	
				IsJingLingActive = true
			else
				g_LoadGrayIconFromRes(SmallIcon,self.m_JingLingSkillBtnTbl[i],-1,IP_ENABLE, IP_CLICKDOWN)
				self.m_JingLingSkillBtnTbl[i].m_IsActiveSkill = false
			end
			self.m_IsShow = true
		else
			self.m_JingLingSkillBtnTbl[i].m_SkillName = ""
			self.m_JingLingSkillBtnTbl[i].m_SkillLevel = 0
			self.m_JingLingSkillBtnTbl[i].m_SkillID = 0
			self.m_JingLingSkillBtnTbl[i]:SetMouseOverDescAfter("")
			g_DelWndImage(self.m_JingLingSkillBtnTbl[i],-1,IP_ENABLE, IP_CLICKDOWN)
			self.m_JingLingSkillBtnTbl[i].m_IsActiveSkill = false
		end
	end
	if not IsJingLingActive then
		self.m_SelectSkillID = 0
	end
end

function CSkillJingLingWnd.RetActiveJingLingSkill(ActiveSkillID)
	local SkillWnd = g_GameMain.m_SkillJingLingWnd
	for i = 1, JingLingSkillNumber do
		if  ActiveSkillID ~= 0 and SkillWnd.m_JingLingSkillBtnTbl[i].m_SkillID == ActiveSkillID then
			local SmallIcon = SkillPart_Common(SkillWnd.m_JingLingSkillBtnTbl[i].m_SkillName,"SkillSmallIcon")
			SkillWnd.m_JingLingSkillBtnTbl[i].m_IsActiveSkill = true
			g_DelWndImage(SkillWnd.m_JingLingSkillBtnTbl[i],-1,IP_ENABLE, IP_CLICKDOWN)
			g_LoadIconFromRes(SmallIcon,SkillWnd.m_JingLingSkillBtnTbl[i],-1,IP_ENABLE, IP_CLICKDOWN)
		elseif SkillWnd.m_JingLingSkillBtnTbl[i].m_SkillID ~= 0 then
			local SmallIcon = SkillPart_Common(SkillWnd.m_JingLingSkillBtnTbl[i].m_SkillName,"SkillSmallIcon")
			SkillWnd.m_JingLingSkillBtnTbl[i].m_IsActiveSkill = false	
			g_DelWndImage(SkillWnd.m_JingLingSkillBtnTbl[i],-1,IP_ENABLE, IP_CLICKDOWN)
			g_LoadGrayIconFromRes(SmallIcon,SkillWnd.m_JingLingSkillBtnTbl[i],-1,IP_ENABLE, IP_CLICKDOWN)		
		end
	end

	if SkillWnd.m_SelectSkillID ~= 0 and SkillWnd.m_SelectSkillID ~= ActiveSkillID then
		--清楚技能快捷栏上的数据信息
		local SkillTbl = {}
		local SkillAttributeTbl = AttributeOfJingLingSkill_Common
		table.insert(SkillTbl,SkillAttributeTbl(SkillWnd.m_SelectSkillID,"SkillName"))
		
		CNpcSkillLearnWnd.ClearToolBarTalentSkill(SkillTbl)
		Gac2Gas:SaveShortCutEnd(g_Conn)
	end
	SkillWnd.m_SelectSkillID = ActiveSkillID
end