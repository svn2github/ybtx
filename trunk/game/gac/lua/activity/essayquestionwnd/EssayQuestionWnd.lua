--gac_require "activity/essayquestionwnd/EssayQuestionWndInc"
CEssayQuestionWnd = class(SQRDialog)

cfg_load "essay_question/QuestionControl_Common"   --条件参数设置表

lan_load "essay_question/Lan_EssayQuestion_Common"
lan_load "essay_question/Lan_box_question"
lan_load "essay_question/Lan_task_question"

local m_LanQuestionStoreTbl = 
{
		["EssayQuestion_Common"] = Lan_EssayQuestion_Common,
		["box_question"] = Lan_box_question,
		["task_question"] = Lan_task_question
}

local OptionTbl = {}
local QuestionIdTbl = nil
local ShowWndTime = 1500

local function SetMediacy( partent, child)
	local rt1 = CFRect:new()
	partent:GetLogicRect(rt1)
	local rt2 = CFRect:new()
	child:GetLogicRect(rt2)
	local rt = CFRect:new()
	rt.top = rt1.top + (rt1.bottom - rt1.top) / 2 - (rt2.bottom - rt2.top) / 2
	rt.left = rt1.left + (rt1.right - rt1.left) / 2 - (rt2.right - rt2.left) / 2
	rt.bottom = rt.top + rt2.bottom - rt2.top 
	rt.right = rt.left + rt2.right - rt2.left
	child:SetLogicRect(rt)
end

function CEssayQuestionWnd:Ctor(partent)
	self:CreateFromRes("EssayQuestionWnd",partent) 
	self.m_RichQuestion = self:GetDlgChild("Question")
	self.m_QuestionNumStr = self:GetDlgChild("CQuestionNum")
	self.m_Option1Btn = self:GetDlgChild("Option1Btn")
	self.m_Option2Btn = self:GetDlgChild("Option2Btn")
	self.m_Option3Btn = self:GetDlgChild("Option3Btn")
	self.m_Option4Btn = self:GetDlgChild("Option4Btn")
	self.m_CloseBtn = self:GetDlgChild("CloseBtn")
	self.m_AnswerWnd = self:GetDlgChild("AnswerWnd")
	self.m_AnswerWnd:ShowWnd(false)
	self:ShowWnd(false)
end

function CEssayQuestionWnd.GetWnd()
	if not g_GameMain.m_EssayQuestionWnd then
		g_GameMain.m_EssayQuestionWnd = CEssayQuestionWnd:new(g_GameMain)
	end
	local wnd = g_GameMain.m_EssayQuestionWnd
	SetMediacy( wnd, wnd.m_AnswerWnd)
	wnd:ShowWnd(true)
	return wnd
end

--function CreateEssayQuestionWnd(partent)
--	local Wnd = CEssayQuestionWnd:new()
--	Wnd:CreateFromRes("EssayQuestionWnd",partent) 
--	Wnd.m_RichQuestion = Wnd:GetDlgChild("Question")
--	Wnd.m_QuestionNumStr = Wnd:GetDlgChild("CQuestionNum")
--	Wnd.m_Option1Btn = Wnd:GetDlgChild("Option1Btn")
--	Wnd.m_Option2Btn = Wnd:GetDlgChild("Option2Btn")
--	Wnd.m_Option3Btn = Wnd:GetDlgChild("Option3Btn")
--	Wnd.m_Option4Btn = Wnd:GetDlgChild("Option4Btn")
--	Wnd.m_CloseBtn = Wnd:GetDlgChild("CloseBtn")
--	Wnd.m_AnswerWnd = Wnd:GetDlgChild("AnswerWnd")
--	SetMediacy( Wnd, Wnd.m_AnswerWnd)
--	Wnd.m_AnswerWnd:ShowWnd(false)
--	return Wnd
--end

--其它答题形式接口
--"任务题库,1-4,2"
--"任务题库,1,3"
local function ExplainStr(str)
	local QuestionStoreName = nil 
	local QuestionIdBegin = 1
	local QuestionIdEnd = nil
	local QuestionCount = 1
	local s1 = string.find( str, ",", 0)
	QuestionStoreName = string.sub(str, 0, s1 - 1) or "任务题库"
	local s2 = string.find( str, ",", s1 + 1)
	local secStr = string.sub(str, s1 + 1, s2 - 1)
	local strLength = string.len(str)
	local thirdStr = string.sub( str, s2 + 1, strLength)
	QuestionCount = tonumber(thirdStr)
	s1 = string.find( secStr, "-", 0)
	if s1 ~= nil then
		s2 = string.len( secStr)
		QuestionIdBegin = tonumber( string.sub( secStr, 0, s1 - 1))
		QuestionIdEnd = tonumber( string.sub( secStr, s1 + 1, s2))
	else
		QuestionIdBegin = tonumber( secStr)	
	end
	return QuestionStoreName, QuestionIdBegin, QuestionIdEnd, QuestionCount, s1
end

--得到参数配置表中的参数设置
local function GetQuestionParameter(QuestionTypeName)
	local Param = QuestionControl_Common(QuestionTypeName)
	local QuestionStoreName = Param("QuestionStoreRoom")
	local QuestionIdBegin = tonumber(Param("QuestionBeginID"))
	local QuestionIdEnd = tonumber(Param("QuestionEndID"))
	local QuestionCountStr = Param("QuestionCount")
	local QuestionMode = tonumber(Param("QuestionMode"))
	local RandomType = tonumber(Param("RandomType"))
	local Quest_Name = Param("QuestName")
	return QuestionStoreName, QuestionIdBegin, QuestionIdEnd, QuestionCountStr, QuestionMode, RandomType, Quest_Name
end


local function RandomId(min, max)
	local ID = 0 
	ID = min + math.random()*(max - min)
	return ID
end

local function FindTblItem( item)
	for i, v in pairs( QuestionIdTbl) do 
		if item == v then
			return true
		end
	end
	return false
end

function CEssayQuestionWnd.RetShowEssayQuestionWnd(Conn, QuestionTypeName, QuestName, EntityID)
	CEssayQuestionWnd.GetWnd()
	if g_GameMain.m_EssayQuestionWnd.isInCDTime then
		MsgClient(3062)
		return
	end
	
	QuestionIdTbl = nil
	QuestionIdTbl = {}
	
	local QuestionStoreName, QuestionIdBegin, QuestionIdEnd, QuestionCountStr, QuestionMode, RandomType, Quest_Name = GetQuestionParameter(QuestionTypeName)
	if Quest_Name ~= "" or Quest_Name ~= nil then
		QuestName = Quest_Name
	end
	
	local QuestionCount = 0
	local AnswerRightNum = 1
	if QuestionCountStr ~= "" and QuestionCountStr ~= nil then
		local s1 = string.find( QuestionCountStr , ",")
		if s1 == nil or s1 == 0 then
			QuestionCount = tonumber(QuestionCountStr)
			AnswerRightNum = QuestionCount
		else
			QuestionCount = tonumber(string.sub(QuestionCountStr, 0, s1 -1))
			AnswerRightNum = tonumber(string.sub( QuestionCountStr, s1 + 1, string.len(QuestionCountStr)))	
		end
	end
	g_GameMain.m_EssayQuestionWnd.m_QuestionStoreName = QuestionStoreName
	g_GameMain.m_EssayQuestionWnd.m_QuestionMode = QuestionMode
	g_GameMain.m_EssayQuestionWnd.m_EntityId = EntityID
	if g_QuestionStoreTbl[QuestionStoreName] == nil then
		--print("QuestionControl_Common 表中"..QuestionTypeName.."题库名称 填写错误")
		return 
	end
	if QuestionIdEnd == nil or QuestionIdEnd == 0 then
		local keys = g_QuestionStoreTbl[QuestionStoreName]:GetKeys()
		QuestionIdEnd = keys[table.getn(keys)]
	end
	if QuestionIdEnd < QuestionIdBegin then
		assert(false, "起始与结束题号填写错")
	end
	if (QuestionIdEnd - QuestionIdBegin) < QuestionCount then
		QuestionCount = (QuestionIdEnd - QuestionIdBegin) + 1
	end	
	if RandomType == 1 then
		for i = 1, QuestionCount do 
			local tempID
			while 1 do 
				tempID = math.floor(RandomId(	QuestionIdBegin,QuestionIdEnd+1))
				if FindTblItem( tempID) == false then
					break
				end
			end
			QuestionIdTbl[i] = tempID
		end
	else
		for i = 1, QuestionCount do 
			QuestionIdTbl[i] = QuestionIdBegin + i - 1
		end
	end
	if g_GameMain.m_EssayQuestionWnd:InitQuestion(QuestionStoreName,1,QuestionIdTbl[1]) then
	 	g_GameMain.m_EssayQuestionWnd.m_QuestName = QuestName
	 	g_GameMain.m_EssayQuestionWnd.m_AnswerRightNum = AnswerRightNum
		g_GameMain.m_EssayQuestionWnd:ShowWnd(true)
		Gac2Gas:CreateAnswerTbl(Conn, 1) --通知服务端创建答案记录表
	end
end

function CEssayQuestionWnd:CloseEssayQuestionWnd(QuestionStoreName, QuestionId, SelectNum, ID)
	--发一个包到服务器端
	if self.QuestionId then	
		Gac2Gas:AnswerQuestion(g_Conn, QuestionStoreName, QuestionId, SelectNum, self.m_QuestName, ID, self.m_EntityId)
	end
end

function CEssayQuestionWnd:InitQuestion(QuestionStoreName, ID, QuestionId)
--		local questionTbl = EssayQuestion_Common[QuestionId]
	if not (g_QuestionStoreTbl[QuestionStoreName] and QuestionId) then
		return 
	end
	local questionTbl = m_LanQuestionStoreTbl[QuestionStoreName](QuestionId)
	if questionTbl then
		local question = questionTbl("Question")
		local questionNumStr = GetStaticTextClient(1142, ID, table.maxn(QuestionIdTbl))
		local Option1 = questionTbl("Option1")
		local Option2 = questionTbl("Option2")
		local Option3 = questionTbl("Option3")
		local Option4 = questionTbl("Option4")
		for i = 1,table.getn(OptionTbl) do
			table.remove(OptionTbl,1)
		end
		for i = 1,4 do
			local Tbl = {}
			Tbl["Option"] = questionTbl("Option"..i)
			Tbl["Index"] = i
			table.insert(OptionTbl,Tbl)
		end
--		self:SwapOption()
		
		self.m_RichQuestion:SetWndText(question)
		self.m_QuestionNumStr:SetWndText(questionNumStr)
	
--		self.m_Option1Btn:SetWndText("A.  "..Option1)
--		self.m_Option2Btn:SetWndText("B.  "..Option2)
--		self.m_Option3Btn:SetWndText("C.  "..Option3)
--		self.m_Option4Btn:SetWndText("D.  "..Option4)
		
		self.m_Option1Btn:SetWndText("A.  "..Option1)
		self.m_Option2Btn:SetWndText("B.  "..Option2)
		self.m_Option3Btn:SetWndText("C.  "..Option3)
		self.m_Option4Btn:SetWndText("D.  "..Option4)
		
		self.m_Option1Btn:EnableWnd(true)
		self.m_Option2Btn:EnableWnd(true)
		self.m_Option3Btn:EnableWnd(true)
		self.m_Option4Btn:EnableWnd(true)
		self.m_AnswerWnd:ShowWnd(false)
		
--		self.m_Option1Btn:SetWndText(OptionTbl[1]["Option"])
--		self.m_Option2Btn:SetWndText(OptionTbl[2]["Option"])
--		self.m_Option3Btn:SetWndText(OptionTbl[3]["Option"])
--		self.m_Option4Btn:SetWndText(OptionTbl[4]["Option"])		
		self.QuestionId = QuestionId
		self.ID = ID
		return true
	end
	return false
end

function CEssayQuestionWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_CLICKDOWN then
		if Child == self.m_Option1Btn and OptionTbl[1] then
			self:CloseEssayQuestionWnd(self.m_QuestionStoreName,self.QuestionId, OptionTbl[1]["Index"], self.ID)
			return
		elseif Child == self.m_Option2Btn and OptionTbl[2] then
			self:CloseEssayQuestionWnd(self.m_QuestionStoreName,self.QuestionId, OptionTbl[2]["Index"], self.ID)
			return
		elseif Child == self.m_Option3Btn and OptionTbl[3] then
			self:CloseEssayQuestionWnd(self.m_QuestionStoreName,self.QuestionId, OptionTbl[3]["Index"], self.ID)
			return
		elseif Child == self.m_Option4Btn and OptionTbl[4] then
			self:CloseEssayQuestionWnd(self.m_QuestionStoreName,self.QuestionId, OptionTbl[4]["Index"], self.ID)
			return
		elseif Child == self.m_CloseBtn then
			Gac2Gas:AnswerQuestionFinish(g_Conn,table.maxn(QuestionIdTbl),g_GameMain.m_EssayQuestionWnd.m_QuestName, self.m_QuestionMode, self.m_AnswerRightNum, self.m_EntityId)
			if self.m_AnswerWndTick ~= nil then
				UnRegisterTick(self.m_AnswerWndTick)
				self.m_AnswerWndTick = nil
			end
			self:ShowWnd(false)
			--OptionTbl = nil
		end
	end
	return
end

function CEssayQuestionWnd:TheNextQuestion(QuestionID)
	if table.maxn(QuestionIdTbl) > 1 then
		for i, id in pairs(QuestionIdTbl) do
			if id == QuestionID then
				g_GameMain.m_EssayQuestionWnd:InitQuestion( g_GameMain.m_EssayQuestionWnd.m_QuestionStoreName, i+1, QuestionIdTbl[i + 1])
				break
			end
		end
	end
end

function CEssayQuestionWnd:AnswerNextQuestion(QuestionID, ID)
	if table.maxn(QuestionIdTbl) > 1 then
		for i, id in pairs(QuestionIdTbl) do
			if id == QuestionID then
				g_GameMain.m_EssayQuestionWnd:InitQuestion( g_GameMain.m_EssayQuestionWnd.m_QuestionStoreName, ID, QuestionIdTbl[ID])
				break
			end
		end
	end	
end

--**********************答题系统废弃代码************************************************
--local function EndCDTime()
--	g_GameMain.m_EssayQuestionWnd.isInCDTime = false
--end

local function HideWindow(Tick,Wnd,bResult,QuestionID, ID)
	if g_GameMain.m_EssayQuestionWnd.m_AnswerWndTick ~= nil then
		
		UnRegisterTick(g_GameMain.m_EssayQuestionWnd.m_AnswerWndTick)
		g_GameMain.m_EssayQuestionWnd.m_AnswerWndTick = nil 
		Wnd:ShowWnd(false)
		g_GameMain.m_EssayQuestionWnd.m_Option1Btn:EnableWnd(true)
		g_GameMain.m_EssayQuestionWnd.m_Option2Btn:EnableWnd(true)
		g_GameMain.m_EssayQuestionWnd.m_Option3Btn:EnableWnd(true)
		g_GameMain.m_EssayQuestionWnd.m_Option4Btn:EnableWnd(true)
		
		--任务答题 QuestionMode == 1错误不进行下一题  QuestionMode ~= 1 对错都进行下一题
		if g_GameMain.m_EssayQuestionWnd.m_QuestionMode == 1 then
			
			if bResult == true then
				g_GameMain.m_EssayQuestionWnd:TheNextQuestion(QuestionID)
				if ID == table.maxn(QuestionIdTbl) then
					g_GameMain.m_EssayQuestionWnd:ShowWnd(false)
					Gac2Gas:AnswerQuestionFinish(g_Conn,ID,g_GameMain.m_EssayQuestionWnd.m_QuestName, g_GameMain.m_EssayQuestionWnd.m_QuestionMode, g_GameMain.m_EssayQuestionWnd.m_AnswerRightNum, g_GameMain.m_EssayQuestionWnd.m_EntityId)
				end
			else
				g_GameMain.m_EssayQuestionWnd:AnswerNextQuestion(QuestionID, ID)
				
--*****************************答题系统废弃代码********************************************----
--				if ID == table.maxn(QuestionIdTbl) then
--					Gac2Gas:AnswerQuestionFinish(g_Conn,ID,g_GameMain.m_EssayQuestionWnd.m_QuestName, g_GameMain.m_EssayQuestionWnd.m_QuestionMode, g_GameMain.m_EssayQuestionWnd.m_AnswerRightNum)
--					g_GameMain.m_EssayQuestionWnd:ShowWnd(false)
--				end
--				if g_GameMain.m_EssayQuestionWnd.m_AnswerWndTick ~= nil then
--					UnRegisterTick(g_GameMain.m_EssayQuestionWnd.m_AnswerWndTick)
--					g_GameMain.m_EssayQuestionWnd.m_AnswerWndTick = nil
--				end
				
--				g_GameMain.m_EssayQuestionWnd.isInCDTime = true
--				RegisterOnceTick(g_GameMain, "isInCDTimeTick", EndCDTime, 10000)
--************************************************************************************************
			end
		else
			g_GameMain.m_EssayQuestionWnd:TheNextQuestion(QuestionID)
			
			if ID == table.maxn(QuestionIdTbl) then
				g_GameMain.m_EssayQuestionWnd:ShowWnd(false)
				Gac2Gas:AnswerQuestionFinish(g_Conn,ID,g_GameMain.m_EssayQuestionWnd.m_QuestName, g_GameMain.m_EssayQuestionWnd.m_QuestionMode, g_GameMain.m_EssayQuestionWnd.m_AnswerRightNum, g_GameMain.m_EssayQuestionWnd.m_EntityId)
			end
		end
	end
end

local function PlayerEffect()
	g_MainPlayer:GetRenderObject():AddEffect("fx/setting/other/other/xiaoyouxi/create.efx","xiaoyouxi/create",0,nil)
end

function CEssayQuestionWnd.RetAnswerResult(QuestionID, bResult,QuestionStoreName)
	local Wnd = g_GameMain.m_EssayQuestionWnd
	if QuestionID == Wnd.QuestionId then
		local ID = nil
		for i, id in pairs(QuestionIdTbl) do
			if id == QuestionID then
				ID = i	
				break
			end
		end	
		Wnd.m_Option1Btn:EnableWnd(false)
		Wnd.m_Option2Btn:EnableWnd(false)
		Wnd.m_Option3Btn:EnableWnd(false)
		Wnd.m_Option4Btn:EnableWnd(false)	
		
		if Wnd:IsShow() then
			if bResult then
				Wnd.m_AnswerWnd:SetWndText(GetStaticTextClient(1009))
				if QuestionStoreName == "task_question" then
					PlayerEffect()
				end
			else
				Wnd.m_AnswerWnd:SetWndText(GetStaticTextClient(1010))
			end
			if Wnd.m_AnswerWndTick ~= nil then
				UnRegisterTick(Wnd.m_AnswerWndTick)
				Wnd.m_AnswerWndTick = nil
			end
			Wnd.m_AnswerWnd:ShowWnd(true)
			Wnd.m_AnswerWnd:SetFocus()
			Wnd.m_AnswerWndTick = RegisterTick("AnswerRightTick",HideWindow, ShowWndTime, Wnd.m_AnswerWnd, bResult, QuestionID, ID)
		end
	end
end


function CEssayQuestionWnd.ReturnFinishInfo(QuestName, uTargetID)
	if CQuest.CheckCanFinishQuest(QuestName) then
	--	table.insert(result, QuestName)	
		if g_QuestFinishGlobalID[QuestName]==nil then
			g_QuestFinishGlobalID[QuestName]={}
			g_QuestFinishGlobalID[QuestName].FinishGlobalID=uTargetID
		else
			g_QuestFinishGlobalID[QuestName].FinishGlobalID=uTargetID
		end
	end
end
