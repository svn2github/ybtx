gac_require "fight/skill_wnd/LearnedSkillShowWnd"
cfg_load "skill/SkillPart_Common"
lan_load "skill/Lan_SkillPart_Common"
cfg_load "skill/NonFightSkill_Common"

CLearnCommonSkillWnd = class(SQRDialog) 
CLearnCommonSkillItemWnd = class(SQRDialog) 

local Distance = 0
local TotalTime = 1200
local TickTime = 33

local function CreateItemWnd(Parent,IsTalent)
	local Wnd = CLearnCommonSkillItemWnd:new()
	if not IsTalent then
		Wnd:CreateFromRes("LearnCommonSkillItemWnd",Parent)
	else
		Wnd:CreateFromRes("LearnCommonTalentItemWnd",Parent)
	end
	
	Wnd.m_IconBtn = Wnd:GetDlgChild("IconBtn")
	Wnd.m_EventBtn = Wnd:GetDlgChild("EventBtn")
	Wnd.m_TipRichText = Wnd:GetDlgChild("TipRichText")
	Wnd.m_Progress_Wnd = Wnd:GetDlgChild("Progress_Wnd")
	Wnd.m_Progress_Wnd:SetProgressMode(0)
	Wnd.m_EventBtn.UpdateSkillPourSoulNum = 0
	return Wnd
end

function CLearnCommonSkillWnd:Ctor()
	self:CreateFromRes("LearnCommonSkillWnd",g_GameMain)
	self.m_CloseWnd = self:GetDlgChild("CloseWnd")
	self.m_CommonSkillItemTbl = {}
	for i = 1, 15 do
		local CommonSkillItem = CreateItemWnd(self,false)
		table.insert(self.m_CommonSkillItemTbl,CommonSkillItem)
	end
	self.m_TalentSkillItemTbl = {}
	for i = 1, 5 do
		local TalentSkillItem = CreateItemWnd(self,true)
		table.insert(self.m_TalentSkillItemTbl,TalentSkillItem)
	end
	self.m_StaticText1 = self:GetDlgChild("Static1")
	self.m_StaticText2 = self:GetDlgChild("Static2")
	self.m_MoneyRichText = self:GetDlgChild("MoneyRichText")
	self.m_SoulRichText = self:GetDlgChild("SoulRichText")
	self.m_BindMoneyRichText = self:GetDlgChild("BindMoneyRichText")
	g_ExcludeWndMgr:InitExcludeWnd(self,2)
end

function CLearnCommonSkillWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_CloseWnd then
			self:ShowWnd(false)
		end
	end
end

local function InitSkillItem(ItemWnd,SkillName)
	local LevelTbl  = g_PlayerLevelToSkillLevelTbl[SkillName]
	if not (#(LevelTbl) == 1 and tostring(LevelTbl[1][3]) == "l") then
		ItemWnd.m_AutoUpSkill = false 
	else
		ItemWnd.m_AutoUpSkill = true
	end
	ItemWnd.m_SkillName = SkillName 		-- 技能名称
	ItemWnd.m_SkillLevel = 0  				-- 技能等级
	ItemWnd.m_IsLearned = false 			-- 是否学习
	ItemWnd.m_IsInit = true   				-- 是否初始化
end

local function GetSkillName(ClassId)
	local skillNameTbl = {}
	local Keys = SkillPart_Common:GetKeys()
	for i ,p in pairs(Keys) do
		if ClassId == tonumber(SkillPart_Common(p,"ClassLimit")) and tonumber(SkillPart_Common(p,"SkillLearnType")) == 4 then
			skillNameTbl[SkillPart_Common(p,"ViewPosition")] = SkillPart_Common(p,"SkillName")
		end
	end
	return skillNameTbl
end

--初始技能面板技能条信息(技能名称、技能条位置)
function CLearnCommonSkillWnd:InitCommonSkillItem()
	local classId = g_MainPlayer:CppGetClass()
	local skillNameTbl = GetSkillName(classId)
	for Position ,SkillName in pairs (skillNameTbl) do --配置表的位置从2开始进行转换
		if self.m_CommonSkillItemTbl[Position-1] == nil then --为nil，则需要额外加加载一个对象
			local CommonSkillItem = CreateItemWnd(self,false)
			table.insert(self.m_CommonSkillItemTbl,CommonSkillItem)
		end
		if self.m_CommonSkillItemTbl[Position-1] then
			InitSkillItem(self.m_CommonSkillItemTbl[Position-1],SkillName)
		end
	end
end

function CLearnCommonSkillWnd:InitTalentSkillItem(TalentSkillName,SkillLevel)
	local AutoUpSkill = false
	local LevelTbl  = g_PlayerLevelToSkillLevelTbl[TalentSkillName]
	if not (#(LevelTbl) == 1 and tostring(LevelTbl[1][3]) == "l") then
		AutoUpSkill = false 
	else
		AutoUpSkill = true
	end		
	if AutoUpSkill == false then
		for i = 1, #(self.m_TalentSkillItemTbl) do
			if self.m_TalentSkillItemTbl[i].m_IsInit 
				and self.m_TalentSkillItemTbl[i].m_SkillName == TalentSkillName 
				and (not self.m_TalentSkillItemTbl[i].m_AutoUpSkill) then
				self.m_TalentSkillItemTbl[i].m_SkillLevel = SkillLevel
				return
			end
		end
		
		for i = 1, #(self.m_TalentSkillItemTbl) do
			if self.m_TalentSkillItemTbl[i] and (not self.m_TalentSkillItemTbl[i].m_IsInit) then
				self.m_TalentSkillItemTbl[i].m_IsInit = true
				self.m_TalentSkillItemTbl[i].m_SkillName = TalentSkillName 
				self.m_TalentSkillItemTbl[i].m_SkillLevel = SkillLevel
				self.m_TalentSkillItemTbl[i].m_AutoUpSkill = false
				self.m_TalentSkillItemTbl[i].m_IsLearned = true
				self.m_TalentSkillItemTbl[i].m_EventBtn.UpdateSkillPourSoulNum = 0
				return
			end
		end
		
		local TalentSkillItem = CreateItemWnd(self,true)
		TalentSkillItem.m_SkillName = TalentSkillName
		TalentSkillItem.m_SkillLevel = SkillLevel
		TalentSkillItem.m_IsInit = true
		TalentSkillItem.m_AutoUpSkill = false
		TalentSkillItem.m_IsLearned = true
		TalentSkillItem.m_EventBtn.UpdateSkillPourSoulNum = 0
		table.insert(self.m_TalentSkillItemTbl,TalentSkillItem)
	end
end

local function GetSkillNextLevel(SkillName,CurLevel)
	local LevelTbl  = g_PlayerLevelToSkillLevelTbl[SkillName]
	if not (#(LevelTbl) == 1 and tostring(LevelTbl[1][3]) == "l") then
		for i = 1, #(LevelTbl) do
			if LevelTbl[i][3] == CurLevel then
				return LevelTbl[i+1]
			elseif LevelTbl[i][1] <= CurLevel and LevelTbl[i][2] >= CurLevel then
				return LevelTbl[i]
			end
		end
	end
end

local function SetSkillItemShowInfo(ItemWnd)
	local money = CMoney:new()
	local SkillName = ItemWnd.m_SkillName
	local SmallIcon = SkillPart_Common(SkillName,"SkillSmallIcon")
	local SkillLevel = ItemWnd.m_SkillLevel
	if ItemWnd.m_IsLearned then
		if not ItemWnd.m_AutoUpSkill then
			ItemWnd.m_EventBtn:SetWndText(GetStaticTextClient(1132))
			local LevelTbl  = GetSkillNextLevel(SkillName,SkillLevel)
			if LevelTbl == nil or LevelTbl[1] > 80 then
				ItemWnd.m_EventBtn:SetWndText("--")
				ItemWnd.m_EventBtn:EnableWnd(false)
				ItemWnd.m_TipRichText:SetWndText(GetStaticTextClient(1143))
				ItemWnd.m_Progress_Wnd:SetWndText("")
				ItemWnd.m_Progress_Wnd:SetRange( 1 )
				ItemWnd.m_Progress_Wnd:SetPos( 0 )					
				g_LoadIconFromRes(SmallIcon, ItemWnd.m_IconBtn,-1,IP_ENABLE, IP_CLICKDOWN)	
			elseif LevelTbl then
				local ConsumeTbl = GetUpSkillConsume(SkillName,LevelTbl[1])
				local money_count = ConsumeTbl.MoneyNumber
				local soul_count = ConsumeTbl.SoulNumber
				local moneyStr = money:ChangeMoneyToString(money_count)	
				moneyStr = GetStaticTextClient(1133,LevelTbl[1]) .. moneyStr
				ItemWnd.m_TipRichText:SetWndText(moneyStr)
				if g_MainPlayer:CppGetLevel() < LevelTbl[1] then
					ItemWnd.m_EventBtn:EnableWnd(false)
					g_LoadGrayIconFromRes(SmallIcon, ItemWnd.m_IconBtn,-1,IP_ENABLE, IP_CLICKDOWN)
				else
					g_LoadIconFromRes(SmallIcon, ItemWnd.m_IconBtn,-1,IP_ENABLE, IP_CLICKDOWN)
					ItemWnd.m_EventBtn:EnableWnd(true)
				end
				ItemWnd.m_NeedMoneyNum = money_count
				ItemWnd.m_NeedSoulNum = soul_count
				local SkillPourSoulNum = ItemWnd.m_EventBtn.UpdateSkillPourSoulNum 
				ItemWnd.m_Progress_Wnd:SetRange( soul_count )
				ItemWnd.m_Progress_Wnd:SetPos( SkillPourSoulNum )
				ItemWnd.m_Progress_Wnd:SetWndText( SkillPourSoulNum .. "/" .. soul_count )
			end	
		else
			ItemWnd.m_EventBtn:SetWndText("--")
			ItemWnd.m_EventBtn:EnableWnd(false)
			ItemWnd.m_TipRichText:SetWndText(GetStaticTextClient(1143))
			ItemWnd.m_Progress_Wnd:SetWndText("")
			ItemWnd.m_Progress_Wnd:SetRange( 1 )
			ItemWnd.m_Progress_Wnd:SetPos( 0 )
			g_LoadIconFromRes(SmallIcon, ItemWnd.m_IconBtn,-1,IP_ENABLE, IP_CLICKDOWN)		
		end			
	else
		local SkillNeedLevel = SkillPart_Common(SkillName,"NeedLevel")
		if g_MainPlayer:CppGetLevel() < SkillNeedLevel then
			ItemWnd.m_EventBtn:EnableWnd(false)
			g_LoadGrayIconFromRes(SmallIcon, ItemWnd.m_IconBtn, -1,IP_ENABLE, IP_CLICKDOWN) 
		else
			ItemWnd.m_EventBtn:EnableWnd(true)
			g_LoadIconFromRes(SmallIcon, ItemWnd.m_IconBtn, -1,IP_ENABLE, IP_CLICKDOWN) 
		end		
		
		ItemWnd.m_EventBtn:SetWndText(GetStaticTextClient(1134))
		local ConsumeTbl = GetLearnSkillConsume(SkillName)
		local money_count = ConsumeTbl.MoneyNumber				
		local soul_count =  ConsumeTbl.SoulNumber				
		local moneyStr = money:ChangeMoneyToString(money_count)
		moneyStr = GetStaticTextClient(1135,SkillNeedLevel) .. moneyStr
		ItemWnd.m_TipRichText:SetWndText(moneyStr)
		ItemWnd.m_NeedMoneyNum = money_count
		ItemWnd.m_NeedSoulNum = soul_count
		local SkillPourSoulNum = ItemWnd.m_EventBtn.UpdateSkillPourSoulNum
		ItemWnd.m_Progress_Wnd:SetPos( SkillPourSoulNum )
		ItemWnd.m_Progress_Wnd:SetRange( soul_count )	
		ItemWnd.m_Progress_Wnd:SetWndText( SkillPourSoulNum .. "/" .. soul_count )	
	end
	local tips = g_Tooltips:GetWndSkillTips( SkillName,SkillLevel)
	ItemWnd.m_IconBtn:SetMouseOverDescAfter(tips)
end

function CLearnCommonSkillWnd:LayoutWnd()
	local rt1 = CFRect:new()
	local rt2 = CFRect:new()
	self.m_StaticText1:GetLogicRect(rt1)
	self.m_StaticText2:GetLogicRect(rt2)
	local ItemRt = CFRect:new()
	local TalentRT = CFRect:new()
	for i = 1, #(self.m_CommonSkillItemTbl) do
		if self.m_CommonSkillItemTbl[i].m_IsInit then
			if i == 1 or i == 2  then
				ItemRt = i == 1 and rt1 or rt2	
			else
				local rt = CFRect:new()
				self.m_CommonSkillItemTbl[i-2]:GetLogicRect(rt)
				ItemRt.left = rt.left
				ItemRt.right = rt.right
				ItemRt.top = rt.bottom + Distance
				ItemRt.bottom =  ItemRt.top + self.m_CommonSkillItemTbl[i]:GetWndOrgHeight()		
			end
			self.m_CommonSkillItemTbl[i]:SetLogicRect(ItemRt)
			if i%2 == 1 then
				TalentRT = ItemRt
			end
			self.m_CommonSkillItemTbl[i]:ShowWnd(true)
			SetSkillItemShowInfo(self.m_CommonSkillItemTbl[i])
		end		
	end

	local LastItemRt = CFRect:new() 
	LastItemRt.bottom = TalentRT.bottom
	for i = 1 ,#(self.m_TalentSkillItemTbl) do
		if self.m_TalentSkillItemTbl[i].m_IsInit and (not self.m_TalentSkillItemTbl[i].m_AutoUpSkill) then
			local TempRt = CFRect:new()
			if i == 1 then
				TempRt.left = rt1.left
				TempRt.right = rt1.right
				TempRt.top = TalentRT.bottom + Distance
				TempRt.bottom =  TempRt.top + self.m_TalentSkillItemTbl[i]:GetWndOrgHeight()
			elseif i == 2 then
				TempRt.left = rt2.left
				TempRt.right = rt2.right				
				TempRt.top = TalentRT.bottom + Distance
				TempRt.bottom =  TempRt.top + self.m_TalentSkillItemTbl[i]:GetWndOrgHeight()
				local rt = CFRect:new()
				self.m_CommonSkillItemTbl[2]:GetLogicRect(rt)
				TempRt.left = rt.left
				TempRt.right = rt.right
			else
				local rt = CFRect:new()
				self.m_TalentSkillItemTbl[i-2]:GetLogicRect(rt)
				TempRt.left = rt.left
				TempRt.right = rt.right
				TempRt.top = rt.bottom + Distance
				TempRt.bottom =  TempRt.top + self.m_TalentSkillItemTbl[i]:GetWndOrgHeight()				
			end
			LastItemRt = TempRt 			
			self.m_TalentSkillItemTbl[i]:SetLogicRect(TempRt)
			self.m_TalentSkillItemTbl[i]:ShowWnd(true)
			SetSkillItemShowInfo(self.m_TalentSkillItemTbl[i])			
		else
			self.m_TalentSkillItemTbl[i]:ShowWnd(false)
		end
	end
	local ParentRt = CFRect:new()
	self:GetLogicRect(ParentRt)
	ParentRt.bottom = LastItemRt.bottom + 40
	self:SetLogicRect(ParentRt)
	self:UpdateMoneyAndSoul()		
end

local function GetItemByName(ItemTbl,Name)
	for i =1,#(ItemTbl) do 
		if ItemTbl[i].m_SkillName == Name then
			return ItemTbl[i]
		end
	end
end

function CLearnCommonSkillWnd:UpdateBtnInfo(SkillName,SkillLevel,ActionType)
	local SkillInfo = SkillPart_Common(SkillName)
	local ItemWnd = nil
	if SkillInfo and tonumber(SkillInfo("SkillLearnType")) == 4 then
		ItemWnd = GetItemByName(self.m_CommonSkillItemTbl,SkillName) --公共升级技能
	elseif SkillInfo and tonumber(SkillInfo("SkillLearnType")) == 1 then
		ItemWnd = GetItemByName(self.m_TalentSkillItemTbl,SkillName) --天赋升级技能
	end	
	if ItemWnd then
		ItemWnd.m_EventBtn.UpdateSkillPourSoulNum = 0
		SetSkillItemShowInfo(ItemWnd)		
		if not g_GameMain.m_LearnedSkillShowWnd then
			g_GameMain.m_LearnedSkillShowWnd = CreateLearnedSkillShowWnd(g_GameMain)
		end
		if g_GameMain.m_LearnedSkillShowWnd and g_GameMain.m_LearnedSkillShowWnd:IsShow() then
			g_GameMain.m_LearnedSkillShowWnd:SetSkillBtnPicture(SkillInfo("SkillSmallIcon"),SkillName,SkillLevel,ActionType) --ActionType为0代表学习，1代表升级
		end
	end
end

function CLearnCommonSkillWnd:UpdateMoneyAndSoul()
	self.m_SoulRichText:SetWndText(GetStaticTextClient(5001) .. ":" .. g_MainPlayer.m_PlayerSoulNum)
   	local c_money = CMoney:new()
	local MoneyNumber = g_MainPlayer:GetMoney()
	local MoneyStr = c_money:ChangeMoneyToString( MoneyNumber,EGoldType.GoldCoin )
	self.m_MoneyRichText:SetWndText(GetStaticTextClient(1144) .. MoneyStr)
	local BindMoneyNumber = g_MainPlayer:GetBindMoney()
	local BindMoneyStr = c_money:ChangeMoneyToString(BindMoneyNumber,EGoldType.GoldBar)
	self.m_BindMoneyRichText:SetWndText(GetStaticTextClient(1145) .. BindMoneyStr)
end
---------------------------------------Item-------------------------------------
--使鼠标添加相应的贴图
local function SetPickupSkill(Wnd)
	local state = g_WndMouse:GetCursorState()
	if(state == ECursorState.eCS_Normal) then
		local Parent = Wnd:GetParent()
		local tbl = {}
		tbl.SkillName  = Parent.m_SkillName 
		tbl.Level = Parent.m_SkillLevel
		g_WndMouse:SetPickupSkill( tbl, Wnd )
	end
end

local function UpdateSkill(Tick, Wnd)
	if Wnd.IsShow and Wnd:IsShow() then
		if( Wnd.UpdateSkillPourSoulNum <= Wnd.UpdateSkillNeedSoulNum ) then
			local number =	math.ceil(Wnd.UpdateSkillNeedSoulNum/(TotalTime/TickTime))
			Wnd.UpdateSkillPourSoulNum = Wnd.UpdateSkillPourSoulNum + number
			if Wnd.UpdateSkillPourSoulNum >= Wnd.UpdateSkillNeedSoulNum then
				Wnd.UpdateSkillPourSoulNum = Wnd.UpdateSkillNeedSoulNum
			end
			local Parent = Wnd:GetParent()
			Wnd:EnableWnd(false)
			Parent.m_Progress_Wnd:SetRange( Wnd.UpdateSkillNeedSoulNum )
			Parent.m_Progress_Wnd:SetPos( Wnd.UpdateSkillPourSoulNum )
			Parent.m_Progress_Wnd:SetWndText( Wnd.UpdateSkillPourSoulNum .. "/" .. Wnd.UpdateSkillNeedSoulNum )			
			if( Wnd.UpdateSkillPourSoulNum == Wnd.UpdateSkillNeedSoulNum ) then
				if Wnd.m_UpdateSkillTick ~= nil then
					UnRegisterTick(Wnd.m_UpdateSkillTick)
					Wnd.m_UpdateSkillTick = nil
				end	
				if Parent.m_IsLearned then --type为0代表学习，1代表升级
					Gac2Gas:UpdateFightSkill(g_Conn,Parent.m_SkillName,Parent.m_SkillLevel,1)	
				else
					Gac2Gas:UpdateFightSkill(g_Conn,Parent.m_SkillName,0,0)
				end
				--Wnd:EnableWnd(false)			
			end		
		end
	else
		if Wnd.m_UpdateSkillTick ~= nil then
			UnRegisterTick(Wnd.m_UpdateSkillTick)
			Wnd.m_UpdateSkillTick = nil
		end			
	end
end

local function MoneyLackFun(Child,uButton)
	Child.m_MoneyMsgBox = nil
	return true
end

function CLearnCommonSkillItemWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_CLICKDOWN and Child == self.m_EventBtn then

		local money = CMoney:new()
		local Parent = Child:GetParent()
		local money_count = Parent.m_NeedMoneyNum
		local moneyStr = money:ChangeMoneyToString(money_count)
		local soul_count = Parent.m_NeedSoulNum
		
		g_GameMain.m_LearnCommonSkillWnd:ShowMsgBox(Child,moneyStr,soul_count)
		
		
	elseif uMsgID == BUTTON_DRAG and Child == self.m_IconBtn then
		local Parent = Child:GetParent()
		if Parent.m_IsLearned then
			SetPickupSkill(Child)
		end
	end
end

local function SkillLearnOrUp(context, index)
	local Child, moneyStr, soul_count = unpack(context)
	if(index == 1) then
		local money = CMoney:new()
		local Parent = Child:GetParent()
		if Parent.m_IsLearned and Parent.m_AutoUpSkill == false then 
			local money_count = Parent.m_NeedMoneyNum
			local allMoney =  tonumber(g_MainPlayer:GetBindMoney()) + tonumber(g_MainPlayer:GetMoney())
			local TipStr = ""
			--金币的判断
			if money_count > allMoney then
				local moneyStr = money:ChangeMoneyToString(money_count)		
				TipStr = MsgBoxMsg(12004,moneyStr)
				
			end
			--魂消耗判断
			local soul_count = Parent.m_NeedSoulNum
			if soul_count > g_MainPlayer.m_PlayerSoulNum then
				if TipStr == "" then
					TipStr = MsgBoxMsg(12005,soul_count)		
				else
					TipStr = TipStr .. "#r" .. MsgBoxMsg(12005,soul_count)
				end
				
			end
			if TipStr ~= "" then
				Child.m_MoneyMsgBox = MessageBox( g_GameMain, TipStr, MB_BtnOK,MoneyLackFun,Child)
				return true
			end
		
			if Child.m_UpdateSkillTick then
				UnRegisterTick(Child.m_UpdateSkillTick)
				Child.m_UpdateSkillTick = nil
				return true
			end
			Child.UpdateSkillNeedSoulNum = soul_count
			Child.m_UpdateSkillTick = RegisterTick("UpdateSkill", UpdateSkill,TickTime, Child)
		elseif  Parent.m_IsLearned == false then 
		
			local money_count = Parent.m_NeedMoneyNum
			local allMoney =  tonumber(g_MainPlayer:GetBindMoney()) + tonumber(g_MainPlayer:GetMoney())
			local moneyStr = money:ChangeMoneyToString(money_count)	
			local TipStr = ""
			--金币的判断
			if money_count > allMoney then
				TipStr = MsgBoxMsg(12004,moneyStr)
			end			
			--魂消耗判断
			local soul_count = Parent.m_NeedSoulNum
			if soul_count > g_MainPlayer.m_PlayerSoulNum then	
				if TipStr == "" then
					TipStr = MsgBoxMsg(12005,soul_count)
				else
					TipStr = TipStr .. "#r" .. MsgBoxMsg(12005,soul_count)
				end
			end
			if TipStr ~= "" then
				Child.m_MoneyMsgBox = MessageBox( g_GameMain, TipStr, MB_BtnOK,MoneyLackFun,Child)
				return true			
			end

			if Child.m_UpdateSkillTick then
				UnRegisterTick(Child.m_UpdateSkillTick)
				Child.m_UpdateSkillTick = nil
				return true
			end
			Child.UpdateSkillNeedSoulNum = soul_count
			Child.m_UpdateSkillTick = RegisterTick("UpdateSkill", UpdateSkill,TickTime, Child)
		end
				
	end
	return true
end

function CLearnCommonSkillWnd:ShowMsgBox(Child,moneyStr,soul_count)
	local Parent = Child:GetParent()
	if Parent.m_IsLearned and Parent.m_AutoUpSkill == false then
		local string = MsgBoxMsg(12014, moneyStr, soul_count)		
		MessageBox(g_GameMain, string, BitOr(MB_BtnOK,MB_BtnCancel), SkillLearnOrUp, {Child, moneyStr, soul_count})
	elseif  Parent.m_IsLearned == false then
		local string = MsgBoxMsg(12015, moneyStr, soul_count)		
		MessageBox(g_GameMain, string, BitOr(MB_BtnOK,MB_BtnCancel), SkillLearnOrUp, {Child, moneyStr, soul_count})
	end
end



--------------------------------------------------------------------------------------------------------
function CLearnCommonSkillWnd.ReturnLearnSkill(SkillName,SkillLevel)
	if g_GameMain.m_LearnSkillTbl == nil then
		g_GameMain.m_LearnSkillTbl = {}
		g_GameMain.m_LearnCommonSkillWnd:InitCommonSkillItem()
	end

	local IsLearnSkill = true
	for i = 1, #(g_GameMain.m_LearnSkillTbl) do
		if g_GameMain.m_LearnSkillTbl[i][1] == SkillName then
			g_GameMain.m_LearnSkillTbl[i][2] = SkillLevel
			IsLearnSkill = false
		end
	end
	if IsLearnSkill then
		table.insert(g_GameMain.m_LearnSkillTbl,{SkillName,SkillLevel})
	end
	
	if SkillPart_Common(SkillName) then
		if tonumber(SkillPart_Common(SkillName,"SkillLearnType")) == 4 then
			for i = 1, #(g_GameMain.m_LearnCommonSkillWnd.m_CommonSkillItemTbl) do 
				if g_GameMain.m_LearnCommonSkillWnd.m_CommonSkillItemTbl[i].m_SkillName == SkillName then
					g_GameMain.m_LearnCommonSkillWnd.m_CommonSkillItemTbl[i].m_SkillLevel = SkillLevel
					g_GameMain.m_LearnCommonSkillWnd.m_CommonSkillItemTbl[i].m_IsLearned = true
				end
			end
		elseif tonumber(SkillPart_Common(SkillName,"SkillLearnType")) == 1 then
			g_GameMain.m_LearnCommonSkillWnd:InitTalentSkillItem(SkillName,SkillLevel)
		end
		
		if PosSkillNameTbl[SkillName] then
			local SmallIcon = SkillPart_Common(SkillName,"SkillSmallIcon")
			g_GameMain.m_BeastPoseWnd:SetSkillBtnPicture(SmallIcon,SkillName,SkillLevel,PosSkillNameTbl[SkillName])
		end
	end
	
	if g_GameMain.m_NonFightSkillWnd then
		g_GameMain.m_NonFightSkillWnd:IsShowBurstSoulWnd(SkillName) --是否显示“攒怒击杀”条
	end
end

--更新技能快捷栏上的数据信息
local function UpdateToolBarTipsToGas(ToolBarWnd,SkillName,SkillLevel,WndType)
	local tblShortcutPiece = ToolBarWnd.m_WndSkill:GetTblShortcutPiece()
	for i = 1,table.maxn(tblShortcutPiece) do
		if tblShortcutPiece[i]:GetState() == EShortcutPieceState.eSPS_Skill then
			if tblShortcutPiece[i]:GetName() == SkillName then
				local tbl = {}
				tbl.SkillName = SkillName
				tbl.Level = SkillLevel
				tblShortcutPiece[i]:SetSkillContext(tbl)	
				local tips = g_Tooltips:GetWndSkillTips( SkillName,SkillLevel)			
				tblShortcutPiece[i].m_SkillBtn:SetMouseOverDescAfter(tips)		
			end
		end 
	end
end

--学习或升级技能
function CLearnCommonSkillWnd.UpdateFightSkill(SkillName,SkillLevel,ActionType)
	local SkillInfo = SkillPart_Common(SkillName)
	if SkillInfo ~= nil then
		--更新技能升级学习面板信息
		if g_GameMain.m_LearnCommonSkillWnd:IsShow() then
			g_GameMain.m_LearnCommonSkillWnd:UpdateBtnInfo(SkillName,SkillLevel,ActionType)
			g_GameMain.m_LearnCommonSkillWnd:UpdateMoneyAndSoul()
		end

		--激活(学习)和更新(升级)技能综合面板公共技能信息
		if g_GameMain.m_CommonSkillWnd and  tonumber(SkillInfo("SkillLearnType")) == 4 then
			if g_GameMain.m_CommonSkillWnd.m_InitWnd then
				g_GameMain.m_CommonSkillWnd:ActiveSkill(SkillName,SkillLevel,true)
			else   --代码结构有问题，待ExportClass时，统一优化
				if not g_GameMain.m_CommonSkillWnd.m_FlashSkillTbl then
					g_GameMain.m_CommonSkillWnd.m_FlashSkillTbl = {}
				end
				g_GameMain.m_CommonSkillWnd.m_FlashSkillTbl[SkillName] = true
			end
		end
		
		if g_GameMain.m_CreateTalentSkillPartWnd and tonumber(SkillInfo("SkillLearnType")) == 1 then 
			g_GameMain.m_CreateTalentSkillPartWnd:UpdateTalentSkillInfo(SkillName,SkillLevel)
			if g_GameMain.m_SelectSeriesWnd.m_SeriesInit then
				g_GameMain.m_SelectSeriesWnd:UpdateTalentInfo(SkillName,SkillLevel)
			end
		end
		
		if ActionType == 0 then --type为0代表学习，1代表升级 
			g_GameMain.m_ShowSkillDescWnd:SetDescInfo(SkillName,ActionType)
		end
	
		--更新技能引导面板信息
		if g_GameMain.m_LeadLeanSkillWnd then
			g_GameMain.m_LeadLeanSkillWnd:InitData()
		end
		local MainSkillWnd = 1
		local MasterSkillWnd = 2
		local AddMasterSkillWnd = 3	
		UpdateToolBarTipsToGas(g_GameMain.m_MainSkillsToolBar,SkillName,SkillLevel,MainSkillWnd)
		UpdateToolBarTipsToGas(g_GameMain.m_MasterSkillArea,SkillName,SkillLevel,MasterSkillWnd)
		UpdateToolBarTipsToGas(g_GameMain.m_AddMasterSkillArea,SkillName,SkillLevel,AddMasterSkillWnd)
		if ActionType == 1 then
			g_GameMain.m_BeastPoseWnd:UpdatePosToolBar(SkillName,SkillLevel)
			Gac2Gas:SaveShortCutEnd(g_Conn)
		end
	end
end

--学习或升级技能失败
function CLearnCommonSkillWnd.UpdateFightSkillError(Conn,SkillName)
	local Wnd = g_GameMain.m_LearnCommonSkillWnd
	local SkillInfo = SkillPart_Common(SkillName)
	if SkillInfo ~= nil then
		local ItemWnd = GetItemByName(Wnd.m_CommonSkillItemTbl,SkillName)
		if ItemWnd then
			SetSkillItemShowInfo(ItemWnd)
		end
	end
end

