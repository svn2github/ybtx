cfg_load "skill/SkillPart_Common"

CCommonSkillWnd = class(SQRDialog)

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

local function GetSkillName(ClassId)
	local skillNameTbl = {}
	local Keys = SkillPart_Common:GetKeys()
	for i ,p in pairs(Keys) do
		local RowInfo = SkillPart_Common(p)
		if ClassId == tonumber(RowInfo("ClassLimit")) and tonumber(RowInfo("SkillLearnType")) == 4 then
			skillNameTbl[tonumber(RowInfo("ViewPosition"))] = RowInfo("SkillName")
		end
	end
	return skillNameTbl
end

function CCommonSkillWnd:Ctor()
	self:CreateFromRes("CommonSkillWnd",g_GameMain)
	self.m_CloseBtn = self:GetDlgChild("CloseBtn")
	self.m_SkillBtnTbl = {}
	for i = 1,15 do 
		local SkillBtn = self:GetDlgChild("SkillBtn" .. i)
		SkillBtn:ShowWnd(false)
		SkillBtn.m_SkillName = ""
		SkillBtn.m_SkillSmallIcon = 0
		SkillBtn.m_IsLearned = false
		table.insert(self.m_SkillBtnTbl,SkillBtn)
	end
	self.m_FlashBtnNumber = 0
	g_ExcludeWndMgr:InitExcludeWnd(self, 2)
end

function CCommonSkillWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_DRAG then
		if Child.m_IsLearned then
			SetPickupSkill(Child)
			self:DelBtnFlash(Child)
		end
	elseif uMsgID == BUTTON_LCLICK then
		if Child == self.m_CloseBtn then
			g_GameMain.m_CommonSkillWnd:ClearAllBtnFlash()
			g_GameMain.m_NonFightSkillWnd:ClearAllBtnFlash()
			g_GameMain.m_CreateTalentSkillPartWnd:ClearAllBtnFlash()	
			CFunctionBar.DelToolBarSkillBtnFlash()				
			self:ShowWnd(false)
		end
		if Child.m_IsLearned then
			SetPickupSkill(Child)
			self:DelBtnFlash(Child)
		end		
	end
end

--初始化公共技能
function CCommonSkillWnd:InitSkillBtn()
	local classId = g_MainPlayer:CppGetClass()
	local skillNameTbl = GetSkillName(classId)
	for i ,p in pairs (skillNameTbl) do
		self.m_SkillBtnTbl[i].m_SkillName = p
		self.m_SkillBtnTbl[i]:ShowWnd(true)
		local skill = SkillPart_Common(self.m_SkillBtnTbl[i].m_SkillName)
		self.m_SkillBtnTbl[i].m_SkillSmallIcon = skill("SkillSmallIcon")
		self.m_SkillBtnTbl[i].m_IsLearned = false
		local LevelTbl = g_PlayerLevelToSkillLevelTbl[self.m_SkillBtnTbl[i].m_SkillName]
		self.m_SkillBtnTbl[i]:SetMouseOverDescAfter(g_Tooltips:GetWndSkillTips(self.m_SkillBtnTbl[i].m_SkillName,0))
		g_LoadGrayIconFromRes(skill("SkillSmallIcon"), self.m_SkillBtnTbl[i], -2, IP_ENABLE, IP_CLICKDOWN)
		if self.m_FlashSkillTbl and self.m_FlashSkillTbl[p] then
			self:SetBtnFlash(self.m_SkillBtnTbl[i])
		end
	end
	self.m_FlashSkillTbl = nil
end

function CCommonSkillWnd:FindSkillBtn(SkillName)
	for i = 1,#(self.m_SkillBtnTbl) do
		if self.m_SkillBtnTbl[i].m_SkillName == SkillName then
			return self.m_SkillBtnTbl[i]
		end
	end	
end

function CCommonSkillWnd:SetBtnFlash(Btn)
	Btn:AddFlashInfoByName("TalentItemFlash")
	Btn.m_IsFlashBtn = true
	if self.m_FlashBtnNumber <= 0 then
		self.m_FlashBtnNumber = 0
	end
	self.m_FlashBtnNumber = self.m_FlashBtnNumber + 1
	CFunctionBar.SetToolBarSkillBtnFlash()	
end

function CCommonSkillWnd:DelBtnFlash(Btn)
	if Btn.m_IsFlashBtn then
		Btn:DelFlashAttention()
		Btn.m_IsFlashBtn = nil
		self.m_FlashBtnNumber = self.m_FlashBtnNumber - 1
		CFunctionBar.DelToolBarSkillBtnFlash()
	end	
end

function CCommonSkillWnd:ClearAllBtnFlash()
	for i = 1, #(self.m_SkillBtnTbl) do 
		self.m_SkillBtnTbl[i]:DelFlashAttention()
		self.m_SkillBtnTbl[i].m_IsFlashBtn = nil
	end
	self.m_FlashBtnNumber = 0
end

--激活公共技能
function CCommonSkillWnd:ActiveSkill(SkillName,SkillLevel,IsShowFlash)
	local btn = self:FindSkillBtn(SkillName)
	assert(btn,SkillName .. "技能在SkillPart_Common配置表不存在。职业:" .. g_MainPlayer:CppGetClass())
	if not btn.m_IsLearned and IsShowFlash then
		self:SetBtnFlash(btn)
	end
	btn.m_IsLearned = true
	btn.m_SkillLevel = SkillLevel
	btn:SetMouseOverDescAfter(g_Tooltips:GetWndSkillTips(SkillName,SkillLevel))
	g_RePlaceWndImage(btn.m_SkillSmallIcon, btn, 0, IP_ENABLE, IP_CLICKDOWN)
end

--更新公共技能
function CCommonSkillWnd:UpdateCommonSkillTips()
	for i = 1,#(self.m_SkillBtnTbl) do
		if self.m_SkillBtnTbl[i] and self.m_SkillBtnTbl[i].m_IsLearned then
			local SkillLevel = self.m_SkillBtnTbl[i].m_SkillLevel
			local SkillName = self.m_SkillBtnTbl[i].m_SkillName
			if  SkillLevel and  SkillName then
				 self.m_SkillBtnTbl[i]:SetMouseOverDescAfter(g_Tooltips:GetWndSkillTips(SkillName,SkillLevel))
			end
		end
	end
end
-----------------------------------------------------------------------------------
--初始化普通攻击
--给普通技能确定技能名称
function CCommonSkillWnd:CommonlySkill()
	local Btn = self.m_SkillBtnTbl[1]
	local Icon,SkillShowName
	if g_GameMain.m_RoleStatus.Part[12].m_Info ~= nil then
		local Wnd = g_GameMain.m_RoleStatus.Part[12].m_Info
		Btn.m_SkillName  = g_GameMain.CommonlySkillName
		Btn.m_SkillLevel = 1
		local SmallIcon = g_ItemInfoMgr:GetItemInfo(Wnd:GetBigID(),Wnd:GetIndex(),"SmallIcon")
		Btn.m_SkillSmallIcon = SmallIcon
	else
		Btn.m_SkillName = g_GameMain.CommonlySkillName
		Btn.m_SkillLevel = 1
		Btn.m_SkillSmallIcon = 866
	end
	Btn.m_IsLearned = true
	
	g_LoadIconFromRes(Btn.m_SkillSmallIcon, Btn, -2, IP_ENABLE, IP_CLICKDOWN)
	Btn:SetMouseOverDescAfter(g_Tooltips:GetWndSkillTips(Btn.m_SkillName,g_MainPlayer:CppGetLevel()))
	Btn:ShowWnd(true)
end
---------------------------------------------------------------------------------------
--初始化普通攻击技能
function CCommonSkillWnd.ReCommonlySkill(CommonlySkillName)
	g_GameMain.CommonlySkillName = CommonlySkillName
	local AttackSpeed1,EquipType,DynInfo
	local WeaponItem = g_GameMain.m_RoleStatus.Part[12].m_Info
	if WeaponItem then
		DynInfo = g_DynItemInfoMgr:GetDynItemInfo(WeaponItem:GetItemID())
	end
	if WeaponItem and DynInfo and DynInfo.CurDuraValue > 0 then
		if DynInfo.BaseLevel <= g_MainPlayer:CppGetLevel() then
			AttackSpeed1 = g_ItemInfoMgr:GetItemInfo(WeaponItem:GetBigID(),WeaponItem:GetIndex(),"AttackSpeed")
			EquipType = g_ItemInfoMgr:GetItemInfo(WeaponItem:GetBigID(),WeaponItem:GetIndex(),"EquipType")
			g_MainPlayer:SetMHNACfg(CommonlySkillName,AttackSpeed1,EquipType,WeaponDPSFloor_Common(EquipType,"AttackType"))
		else
			g_MainPlayer:SetMHNACfg(CommonlySkillName,2,"","")
		end
	else
		AttackSpeed1 = 2
		EquipType = ""
		g_MainPlayer:SetMHNACfg(CommonlySkillName,AttackSpeed1,EquipType,"")
	end
	
	if g_GameMain.m_CommonSkillWnd ~= nil then
		g_GameMain.m_CommonSkillWnd:CommonlySkill()
	end
end

function CCommonSkillWnd.RetAddFightSkillStart()
	if g_GameMain.m_CommonSkillWnd ~= nil then
		g_GameMain.m_CommonSkillWnd:InitSkillBtn()
	end
end

