gac_gas_require "activity/quest/MercenaryLevelCfg"
lan_load "quest/Lan_MercenaryLevelDirect_Common"
lan_load "message/Lan_Message_Common"
cfg_load "scene/Position_Common"

local LinkNpcBaseColor = g_ColorMgr:GetColor("自动寻路")
local LinkNpcColor = "#s"..string.sub(g_ColorMgr:GetColor("自动寻路鼠标移动到NpcObj"), 3, -1) .. "#s"..string.sub(g_ColorMgr:GetColor("自动寻路鼠标单击"), 3, -1)
local LinkSkillBaseColor = g_ColorMgr:GetColor("技能超连接颜色")
local LinkSkillColor = "#s"..string.sub(g_ColorMgr:GetColor("自动寻路鼠标移动到Skill"), 3, -1) .. "#s"..string.sub(g_ColorMgr:GetColor("自动寻路鼠标单击"), 3, -1)

CMercenaryLevelTrace = class(SQRDialog)

function g_GetMercenaryLevelLanName(Yblevel)
	local lan_index = MemH64(Yblevel.."佣兵等级佣兵等级")
	if Lan_MercenaryLevelDirect_Common(lan_index) then
		return Lan_MercenaryLevelDirect_Common(lan_index,"Describe")
	end
end

local function GetMercenaryLevelTxt(Level, Field, Subject)
	local index = MemH64(Level..Field..Subject)
	if Lan_MercenaryLevelDirect_Common(index) then
		return Lan_MercenaryLevelDirect_Common(index,"Describe")
	end
end

local function GetSentence(Describe, LinkPos, Object, Subject, Level)
	--特殊处理
	if (Subject == "气魂击" and Level < 3)
		or ((Subject=="佣兵积分" or Subject=="进阶") and Level<10)
		or (Subject == "生活技能" and Level <15) then
			return Describe
	end
	
	if Describe then
		local Sentence = ""
		if LinkPos then
			local camp = g_MainPlayer:CppGetBirthCamp()
			if Object then
				local ShowName = nil
				if Object[1] == "npc" then
					ShowName = GetNpcDisplayName(Object[2][camp])
				else
					ShowName = GetIntObjDispalyName(Object[2][camp])
				end
				local info = "{\""..Object[2][camp].."\",\""..LinkPos[camp][1].."\","..LinkPos[camp][2]..","..LinkPos[camp][3].."}"
				local str = LinkNpcBaseColor.."#u#l"..ShowName.."#i["..MemH64(info).."]#l#u#W"
				Sentence = string.gsub(Describe, "#Name#", str)
			else
				local info = "{\""..Subject.."\",\""..LinkPos[camp][1].."\","..LinkPos[camp][2]..","..LinkPos[camp][3].."}"
				Sentence = LinkNpcBaseColor.."#u#l"..Describe.."#i["..MemH64(info).."]#l#u#W"
			end
		else
			Sentence = Describe
		end
		return Sentence
	end
	return ""
end

function CMercenaryLevelTrace:InitLinkTbl()
	local Camp = g_MainPlayer:CppGetBirthCamp()
	local Index1 = Camp + 11110
	local Index2 = Camp + 4018
	self.m_LinkTbl[1] = {
		Position_Common(Index1, "Description"),
		Position_Common(Index1, "SceneName"),
		Position_Common(Index1, "PosX"),
		Position_Common(Index1, "PosY")
	}
	self.m_LinkTbl[2] = {
		Position_Common(Index2, "Description"),
		Position_Common(Index2, "SceneName"),
		Position_Common(Index2, "PosX"),
		Position_Common(Index2, "PosY")
	}
end

function CMercenaryLevelTrace:Ctor()
	self:CreateFromRes("MercenaryLevelTrace", g_GameMain)
	self.m_LogoBtn = self:GetDlgChild("LogoBtn")
	self.m_MercenaryLevel = self:GetDlgChild("MercenaryLevel")
	self.m_Static1 = self:GetDlgChild("Static1")
	self.m_Static2 = self:GetDlgChild("Static2")
	self.m_Static3 = self:GetDlgChild("Static3")
	self.m_Static4 = self:GetDlgChild("Static4")
	self.m_Static5 = self:GetDlgChild("Static5")
	self.m_Static6 = self:GetDlgChild("Static6")
	
	self.m_State = false
	self.m_MercenLevState = {}
	self.m_LinkTbl = {}
	
	self.m_MercenaryLevel:ShowWnd(false)
	self.m_LogoBtn:ShowWnd(false)
	for j=2, 6 do
		self["m_Static"..j]:ShowWnd(false)
	end
	--self.m_Static1:ShowWnd(true)
	--self:ShowWnd(true)
end

function CMercenaryLevelTrace:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
--[[
	if not IsCppBound(g_MainPlayer) then
		return
	end
	
	if not self.m_LinkTbl[1] then
		self:InitLinkTbl()
	end
	
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_LogoBtn then
			g_GameMain.m_MercenaryAssess:OpenYbAssessWnd()
		end
	end
	if (uMsgID == RICH_CLICK) then
		local str = Child:GetChooseStrAttr()
		
		if str == "返回树屋酒吧" then
			g_MainPlayer:FightSkill(str, 1)
		elseif str == "公告板" then
			local tbl = self.m_LinkTbl[1]
			PlayerAutoTrack(tbl[1], tbl[2], tbl[3], tbl[4])
		elseif str == "大雷蒙" then
			local tbl = self.m_LinkTbl[2]
			PlayerAutoTrack(tbl[1], tbl[2], tbl[3], tbl[4])
		end
		return
		
		--local LinkInfo = assert(loadstring("return "..str))()
		--if LinkInfo and g_MainPlayer.m_uMercenaryLevel<g_MercenaryLevelTbl.MaxLevel then
		--	PlayerAutoTrack(LinkInfo[1], LinkInfo[2], LinkInfo[3], LinkInfo[4])
		--end
	end
	--]]
end

function CMercenaryLevelTrace:RetUpdateMercenaryLevelTrace(MercenaryLevel,Status,Subject1,Subject2,Subject3,Count1,Count2,Count3,Challenge,Integral,Level)
	if not (IsCppBound(g_Conn) and g_MainPlayer) then
		return
	end
	
	local Wnd = g_GameMain.m_MercenaryLevelTrace
	if MercenaryLevel == g_MercenaryLevelTbl.MaxLevel then
		Wnd.m_MercenaryLevel:ShowWnd(false)
		for j=1, 6 do
			Wnd["m_Static"..j]:ShowWnd(false)
		end
		Wnd:ShowWnd(true)
		return
	end
	
	local tbl = g_MercenaryLevelTbl[MercenaryLevel]
	if Wnd.m_MercenLevState.m_Level ~= MercenaryLevel then
		Wnd.m_MercenLevState.m_Level = MercenaryLevel
		Wnd.m_MercenLevState["达成条件"] = {}
		Wnd.m_MercenLevState.m_State = Status
	elseif Wnd.m_MercenLevState.m_State ~= Status then
		local PlayerName = g_MainPlayer.m_Properties:GetCharName()
		local ShowLevelName = GetMercenaryLevelTxt(tbl.LevelName, "佣兵等级", "佣兵等级")
		MsgClient(299901+Status, PlayerName, ShowLevelName)
		Wnd.m_MercenLevState.m_State = Status
	end
	
	g_MainPlayer.m_PlayerMercenaryState = true
	g_GameMain.m_NpcHeadSignMgr:UpdateNpcAndObjInView()
	local ShowName = GetMercenaryLevelTxt(tbl.LevelName, "佣兵等级", "佣兵等级")
	local ShowSentences = {}
	local i = 1
	
	if Status == 1 then
		ShowName = ShowName..": "..GetStaticTextClient(190101)
		local function func(Subject, Count)
			local t = tbl["达成条件"][Subject]
			local Describe = GetMercenaryLevelTxt(tbl.LevelName, "达成条件", Subject)
			local str
			if Count >= t.Arg then
				str = GetStaticTextClient(190100)
			else
				str = Count.."/"..t.Arg
			end
			ShowSentences[i] = GetSentence(Describe, t.LinkPos, t.Object, Subject, Level)..": "..str
			Wnd.m_MercenLevState["达成条件"][Subject] = {Count,t.Arg}
			i = i+1
		end
		
		if tbl["达成条件"]["人物等级"] then
			func("人物等级", Level)
		end
		if tbl["达成条件"]["佣兵积分"] then
			func("佣兵积分", Integral)
		end
		if Subject1 ~= "" then
			func(Subject1, Count1)
		end
		if Subject2 ~= "" then
			func(Subject2, Count2)
		end
		if Subject3 ~= "" then
			func(Subject3, Count3)
		end
		
	elseif Status == 2 then
		ShowName = ShowName..": "..GetStaticTextClient(190102)
		local t = tbl["挑战条件"]
		local Describe = GetMercenaryLevelTxt(tbl.LevelName, "挑战条件", t.Subject)
		ShowSentences[1] = GetSentence(Describe, t.LinkPos, t.Object)
		
	elseif Status == 3 then
		ShowName = ShowName..": "..GetStaticTextClient(190103)
		local t = tbl["佣兵评定"]
		local Describe = GetMercenaryLevelTxt(tbl.LevelName, "佣兵评定", t.Subject)
		ShowSentences[1] = GetSentence(Describe, t.LinkPos, t.Object)
		
	elseif Status == 4 then
		ShowName = ShowName..": "..GetStaticTextClient(190104)
		local t = tbl["特权"]["称号"]
		local Describe = GetMercenaryLevelTxt(tbl.LevelName, "特权", "称号")
		ShowSentences[1] = GetSentence(Describe, t.LinkPos, t.Object)
	end
	
	Wnd.m_MercenaryLevel:SetWndText(ShowName)
	Wnd.m_MercenaryLevel:ShowWnd(true)
	for j=1, 6 do
		if ShowSentences[j] then
			Wnd["m_Static"..j]:SetWndText(ShowSentences[j])
			Wnd["m_Static"..j]:ShowWnd(true)
		else
			Wnd["m_Static"..j]:ShowWnd(false)
		end
	end
	
	if g_GameMain.m_MercenaryDirWnd.m_ClickBtn then
		g_GameMain.m_MercenaryAssess:OpenYbAssessWnd()
	else
		Wnd:ShowWnd(true)
	end
end

function CMercenaryLevelTrace:ShowMercenaryLevelMessage(Num)
	if not (IsCppBound(g_Conn) and g_MainPlayer) then
		return
	end
	local PlayerName = g_MainPlayer.m_Properties:GetCharName()
	if Num ~= 0 then
		local MercenaryLevel = g_MainPlayer.m_uMercenaryLevel
		if MercenaryLevel == g_MercenaryLevelTbl.MaxLevel then
			return
		end
		if Num == 6 then
			MercenaryLevel = MercenaryLevel-1
		end
		local LevelName = g_MercenaryLevelTbl[MercenaryLevel].LevelName
		local ShowLevelName = GetMercenaryLevelTxt(LevelName, "佣兵等级", "佣兵等级")
		MsgClient(299900+Num, PlayerName, ShowLevelName)
	else
		MsgClient(299900, PlayerName)
		MsgClient(299901, PlayerName)
		MsgClient(299902, PlayerName)
	end
end

function CMercenaryLevelTrace:RetCheckInMercenaryInfo()
	if g_GameMain then
		g_GameMain.m_MercenaryLevelTrace.m_IsCheckIn = true
	end
end

function SetHintMsgWndText()
	local Wnd = g_GameMain.m_MercenaryLevelTrace
	if not g_GameMain.m_MercenaryLevelTrace.m_IsCheckIn then
		local TraceLink = LinkNpcBaseColor.."#u#l"..LinkNpcColor..GetStaticTextClient(8333).."#i["..MemH64("公告板").."]#l#u#W"
		local SkillLink = LinkSkillBaseColor.."#u#l"..LinkSkillColor..GetStaticTextClient(8331).."#i["..MemH64("返回树屋酒吧").."]#l#u#W"
		local str = Lan_Message_Common(299911,"Content").."#r"
		str = string.gsub(str, "#Obj#", TraceLink)
		str = string.gsub(str, "#Skill#", SkillLink)
		Wnd.m_Static1:SetWndText( str )
	else
		local TraceLink = LinkNpcBaseColor.."#u#l"..LinkNpcColor..GetStaticTextClient(8336).."#i["..MemH64("大雷蒙").."]#l#u#W"
		local str = Lan_Message_Common(299912,"Content").."#r"
		str = string.gsub(str, "#Name#", TraceLink)
		Wnd.m_Static1:SetWndText( str )
	end
end

function RetShowHintMsgWnd()
	if not IsCppBound(g_MainPlayer) then
		return
	end
	local Wnd = g_GameMain.m_MercenaryLevelTrace
	if g_MainPlayer:CppGetLevel() >= 40 then
		Wnd:ShowWnd(false)
	else
		local IsShow = true
		for QuestName,_ in pairs(g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo) do
			if g_AllMercenaryQuestMgr[QuestName] then
				IsShow = false
			elseif g_DareQuestMgr[QuestName] then
			elseif g_ActionQuestMgr[QuestName] then
			else
				if not g_HideQuestMgr[QuestName] then
					IsShow = false
				end
			end
		end
		SetHintMsgWndText()
		Wnd:ShowWnd(IsShow)
	end
end
