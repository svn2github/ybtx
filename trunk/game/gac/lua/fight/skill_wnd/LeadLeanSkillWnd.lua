cfg_load "skill/SkillPart_Common"
cfg_load "npc/SkillLearnNpcPosition_Common"

CLeadLeanSkillWnd = class(SQRDialog) 

local function GetNpcPositionByCampIDTbl()
	local CampIDToNpcPosition = {}
	CampIDToNpcPosition[1] = {}
	CampIDToNpcPosition[2] = {}
	CampIDToNpcPosition[3] = {}
    
    local Keys = SkillLearnNpcPosition_Common:GetKeys()
	for i, v in pairs (Keys) do
		local RowInfo = SkillLearnNpcPosition_Common(v)
		table.insert(
						CampIDToNpcPosition[RowInfo("CampID")],
						{
							RowInfo("NpcName"),
							RowInfo("SceneName"),
							RowInfo("PosX"),
							RowInfo("PosY"),
						}
					)
	end
	return CampIDToNpcPosition
end

local IconNumber = 8
function CLeadLeanSkillWnd:Ctor()
	self:CreateFromRes("LeadLeanSkillWnd",g_GameMain)
	self.m_SkillBtnTbl 	= {}
	self.m_BaseMapTbl 	= {}
	for i = 1,IconNumber do 
		local ChildBtn 		= self:GetDlgChild("SkillBtn" .. i)
		table.insert(self.m_SkillBtnTbl,ChildBtn)
		local BaseMapBtn 	= self:GetDlgChild("BaseMap" .. i )
		table.insert(self.m_BaseMapTbl,BaseMapBtn)
	end
	self.m_LinkRichTest1 	= self:GetDlgChild("LinkRichTest1")
	self.m_LinkRichTest2 	= self:GetDlgChild("LinkRichTest2")
	self.m_CloseBtn 		= self:GetDlgChild("CloseBtn")
	self.m_CampIDToNpcPositionTbl = GetNpcPositionByCampIDTbl()
end

function CLeadLeanSkillWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if uMsgID  == RICH_CLICK then
		if Child == self.m_LinkRichTest1 or Child == self.m_LinkRichTest2 then
			local CampID = g_MainPlayer:CppGetBirthCamp()
			local PointList = self.m_CampIDToNpcPositionTbl[CampID]
			PlayerAutoTrackNearestPoint(PointList)
		end
	elseif uMsgID == BUTTON_LCLICK then
		if Child == self.m_CloseBtn then
			self:ShowWnd(false)
			local Rt = CFRect:new()
			self.m_CloseBtn:GetLogicRect(Rt)
			g_GameMain.m_LeadLeanSkillShowWnd:SetLogicRect(Rt)
			g_GameMain.m_LeadLeanSkillShowWnd:ShowWnd(true)
		end
	end
end

function CLeadLeanSkillWnd:GetPositionList(nCamp)
	return self.m_CampIDToNpcPositionTbl[nCamp]
end

function CLeadLeanSkillWnd:RemoveTalentSkill()
	if g_GameMain.m_LearnSkillTbl then
		local number = #(g_GameMain.m_LearnSkillTbl)
		for i = number , 1 , -1 do 
			if SkillPart_Common(g_GameMain.m_LearnSkillTbl[i][1]) 
				and tonumber(SkillPart_Common(g_GameMain.m_LearnSkillTbl[i][1],"SkillLearnType")) == 1 then
				table.remove(g_GameMain.m_LearnSkillTbl,i)
			end
		end	
	end
end

local function GetSkillLearnedTblAndNoLearnTbl()
	local LearnedSkillTbl = {}
	local NoLearnSkillTbl = {}
	if g_GameMain.m_LearnSkillTbl then
		local CurLevel 	= g_MainPlayer:CppGetLevel()
		local nClass 	= g_MainPlayer:CppGetClass()
		local Keys 		= SkillPart_Common:GetKeys()	
		for i ,p in pairs(Keys) do
			if nClass == SkillPart_Common(p,"ClassLimit")
				and SkillPart_Common(p,"NeedLevel") <= CurLevel then
				local IsLearnSkill = false
				local LearnSkillIndex = 1
				for j = 1,#(g_GameMain.m_LearnSkillTbl) do
					if g_GameMain.m_LearnSkillTbl[j][1] == p then
						IsLearnSkill = true
						LearnSkillIndex = j
						break
					end
				end
				
				if IsLearnSkill then
					local LevelTbl  = g_PlayerLevelToSkillLevelTbl[p]
					if not (#(LevelTbl) == 1 and tostring(LevelTbl[1][3]) == "l") then
						table.insert(LearnedSkillTbl,g_GameMain.m_LearnSkillTbl[LearnSkillIndex])
					end
				elseif tonumber(SkillPart_Common(p,"SkillLearnType")) == 4 then
					table.insert(NoLearnSkillTbl,p)
				end	
			end								
		end
	end	
	return LearnedSkillTbl,NoLearnSkillTbl
end

function CLeadLeanSkillWnd:InitData()
	local LearnedSkillTbl,NoLearnSkillTbl = GetSkillLearnedTblAndNoLearnTbl()
	if #(NoLearnSkillTbl) > 0 then
		for i = 1,4 do
			if NoLearnSkillTbl[i] then
				local ItemInfo = SkillPart_Common(NoLearnSkillTbl[i])
				self.m_SkillBtnTbl[i]:ShowWnd(true)
				self.m_BaseMapTbl[i]:ShowWnd(true)
				g_LoadIconFromRes(ItemInfo("SkillSmallIcon"), self.m_SkillBtnTbl[i],-1,IP_ENABLE, IP_CLICKDOWN)
				local tips = g_Tooltips:GetWndSkillTips(NoLearnSkillTbl[i],0)
				self.m_SkillBtnTbl[i]:SetMouseOverDescAfter(tips)
			else
				self.m_SkillBtnTbl[i]:ShowWnd(false)
				self.m_BaseMapTbl[i]:ShowWnd(false)
			end
		end
	else
		for i = 1, 4 do 
			self.m_SkillBtnTbl[i]:ShowWnd(false)
			self.m_BaseMapTbl[i]:ShowWnd(false)
		end
	end
	
	if #(NoLearnSkillTbl) > 0 then 
		local str = "#u" ..  GetStaticTextClient(1148) .. "#u"
		self.m_LinkRichTest1:SetWndText(str)
		self.m_LinkRichTest1:ShowWnd(true)
	else
		self.m_LinkRichTest1:ShowWnd(false)
	end
	
	local TempIndex = 0
	local PlayerLevel = g_MainPlayer:CppGetLevel()
	if #(LearnedSkillTbl) > 0 then
		for i = 1,#(LearnedSkillTbl) do
			local SkillNeedLevel,SkillIndex =  GetSkillLevel(LearnedSkillTbl[i][1],PlayerLevel)
			if LearnedSkillTbl[i][2] < SkillNeedLevel then
				if LearnedSkillTbl[i]  then
					if TempIndex < 4 then
						TempIndex = TempIndex + 1
					else
						break
					end
					local ItemInfo = SkillPart_Common(LearnedSkillTbl[i][1])
					self.m_SkillBtnTbl[4 + TempIndex]:ShowWnd(true)
					self.m_BaseMapTbl[4 + TempIndex]:ShowWnd(true)
					g_LoadIconFromRes(ItemInfo("SkillSmallIcon"), self.m_SkillBtnTbl[4 + TempIndex],-1,IP_ENABLE, IP_CLICKDOWN)
					local tips = g_Tooltips:GetWndSkillTips(LearnedSkillTbl[i][1],LearnedSkillTbl[i][2])
					self.m_SkillBtnTbl[4 + TempIndex]:SetMouseOverDescAfter(tips)
				end
			end
		end	
	end
	
	for i =  TempIndex + 5 , 8 do 
		self.m_SkillBtnTbl[i]:ShowWnd(false)
		self.m_BaseMapTbl[i]:ShowWnd(false)
	end
	
	if TempIndex > 0 then
		local str = "#u" ..  GetStaticTextClient(1149) .. "#u"
		self.m_LinkRichTest2:SetWndText( str )
		self.m_LinkRichTest2:ShowWnd(true)
	else
		self.m_LinkRichTest2:ShowWnd(false)
	end
		
	if #(NoLearnSkillTbl) > 0 or TempIndex > 0 then
		if not g_GameMain.m_LeadLeanSkillShowWnd:IsShow() then
			self:ShowWnd(true)
		end
	else
		self:ShowWnd(false)
		g_GameMain.m_LeadLeanSkillShowWnd:ShowWnd(false)
	end
end