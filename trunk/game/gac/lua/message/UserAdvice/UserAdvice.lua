gac_require "message/UserAdvice/UserAdviceInc"
gac_gas_require "framework/text_filter_mgr/TextFilterMgr"
cfg_load "gm/UserAdvice_Common"
lan_load "gm/Lan_UserAdvice_Common"



------此窗口分为  提问 , 和 查询 两个状态  
-- m_Type  为  "Ask" 或 "Query"

CUserAdviceWnd = class(SQRDialog) 

function CUserAdviceWnd.CreateUserAdviceWnd(Parent)
	local Wnd = CUserAdviceWnd:new()
	Wnd:CreateFromRes( "WndUserAdvice", Parent )
	return Wnd
end

function CUserAdviceWnd:SetQuestionType(questionTypeId)
	self.m_TypeOption:RemoveAllItem()
	self.m_TypeOption:InsertDefaultColumn()
	local typeOption =  GetCfgTransformValue(true, "Lan_UserAdvice_Common", questionTypeId, "typeOption")
	for i, v in ipairs(typeOption) do
		self.m_TypeOption:InsertItem(v, i, -1)
	end
	self.m_TypeOption:SetWndText(GetStaticTextClient(7040))
	self.m_TypeOption:EnableWnd(true)
end

function CUserAdviceWnd:OnChildCreated()
	self.m_BtnClose = self:GetDlgChild("BtnClose")
	self.m_QuestionType = self:GetDlgChild("QuestionType")
	self.m_TypeOption = self:GetDlgChild("TypeOption")
	self.m_Content = self:GetDlgChild("Content")
	self.m_BtnCommit = self:GetDlgChild("BtnCommit")
	self.m_QuestionList = self:GetDlgChild("QuestionList")
	self.m_BtnChange = self:GetDlgChild("BtnChange")
	self.m_Caption = self:GetDlgChild("Caption")
	self.m_AskContent = self:GetDlgChild("AskContent")
	self.m_AnswerContent = self:GetDlgChild("AnswerContent")
end

function CUserAdviceWnd:InitAsk()
	self.m_Type = "Ask"
	self.m_Caption:SetWndText(GetStaticTextClient(7046))
	self.m_BtnChange:SetWndText(GetStaticTextClient(7048))
	
	self.m_Content:ShowWnd(true)
	self.m_BtnCommit:ShowWnd(true)
	self.m_QuestionType:ShowWnd(true)
	self.m_TypeOption:ShowWnd(true)
	self.m_QuestionList:ShowWnd(false)
	self.m_AskContent:ShowWnd(false)
	self.m_AnswerContent:ShowWnd(false)
	
--	self.m_QuestionList:DeleteAllItem()
--	self.m_QuestionList:InsertColumn(0, self.m_QuestionList:GetWndOrgWidth())
--	local i = 0
--	for _, questionTypeId in pairs(Lan_UserAdvice_Common:GetKeys()) do
--		self.m_QuestionList:InsertItem(i, 25)
--		local item = self.m_QuestionList:GetSubItem(i, 0)
--		local questionType = Lan_UserAdvice_Common(questionTypeId, "questionType")
--		item:SetWndText(questionType)
--		item.m_QuestionTypeId = questionTypeId
--		i = i + 1
--	end
	
	self.m_QuestionType:RemoveAllItem()
	self.m_QuestionType:InsertDefaultColumn()
	local i = 0
	for _, questionTypeId in pairs(Lan_UserAdvice_Common:GetKeys()) do
		local questionType = Lan_UserAdvice_Common(questionTypeId, "questionType")
		self.m_QuestionType:InsertItem(questionType, i, -1)
		local item = self.m_QuestionType:GetItem(i)
		item.m_QuestionTypeId = questionTypeId
		i = i + 1
	end
	self.m_QuestionType:SetWndText(GetStaticTextClient(7038))
	self.m_TypeOption:SetWndText(GetStaticTextClient(7040))
	self.m_TypeOption:EnableWnd(false)
end

function CUserAdviceWnd:InitQuery()
	self.m_Type = "Query"
	self.m_Caption:SetWndText(GetStaticTextClient(7047))
	self.m_BtnChange:SetWndText(GetStaticTextClient(7049))
	
	self.m_Content:ShowWnd(false)
	self.m_BtnCommit:ShowWnd(false)
	self.m_TypeOption:ShowWnd(false)
	self.m_QuestionType:ShowWnd(false)
	self.m_QuestionList:ShowWnd(true)
	self.m_AskContent:ShowWnd(true)
	self.m_AnswerContent:ShowWnd(true)
	self.m_AskContent:SetWndText("")
	self.m_AnswerContent:SetWndText("")
	
	self.m_QuestionList:DeleteAllItem()
	self.m_QuestionList:InsertColumn(0, self.m_QuestionList:GetWndOrgWidth())
	
	Gac2Gas:QueryQuestionList(g_Conn)
end

local function GetDateTblFormStr(str)
	local date = {}
	date.year = tonumber( string.sub(str, 1, 4))
	date.month = tonumber( string.sub(str, 6, 7))
	date.day = tonumber( string.sub(str, 9, 10))
	date.hour = tonumber( string.sub(str, 12, 13))
	date.min = tonumber( string.sub(str, 15, 16))
	date.sec = tonumber( string.sub(str, 18, 19))
	return os.time(date)
end

function CUserAdviceWnd:AddQuestion(iter)
	local askTime = GetDateTblFormStr(iter:Get("askTime"))
	local index = self.m_QuestionList:GetItemCount()
	for i = 0, self.m_QuestionList:GetItemCount()-1 do
		local tempItem = self.m_QuestionList:GetSubItem(i, 0)
		if tempItem.m_AskTime < askTime then
			index = i
			break
		end
	end

	local name = iter:GetSiblingName()
	self.m_QuestionList:InsertItem(index, 25)
	local item = self.m_QuestionList:GetSubItem(index, 0)
	item.m_QuestionTypeId = tonumber(iter:Get("questionTypeId"))
	item.m_TypeOptionId = tonumber(iter:Get("typeOptionId"))
	item.m_GameAccount = iter:Get("gameAccount")
	item.m_GameRole = iter:Get("gameRole")
	item.m_GameServer = iter:Get("gameServer")
	item.m_QuestionText = iter:Get("questionText")
	item.m_AnswerText = iter:Get("answerText") or ""
	item.m_AskTimeStr = iter:Get("askTime")
	item.m_AnswerTime = iter:Get("answerTime")
	
	item.m_AskTime = askTime
	
	item:SetWndText(item.m_AskTimeStr)
end

function CUserAdviceWnd:InitQuestionList(content)
	if self.m_Type ~= "Query" then
		return
	end
	self.m_QuestionList:DeleteAllItem()
	self.m_QuestionList:InsertColumn(0, self.m_QuestionList:GetWndOrgWidth())
	
	local xml = CXmlConfig:new()
	xml:LoadConfigFormString("linekong",content)
	local iter = CXmlSiblingElemIter:new()
	xml:GetIter_2(iter, "questions", "question")
	while not iter:IsEnd() do
		self:AddQuestion(iter)
		iter:MoveNext()
	end
	
end

function CUserAdviceWnd:OpenWnd()
	self:InitAsk()
	self:ShowWnd(true)
end

function CUserAdviceWnd:ChangeType()
	if self.m_Type == "Ask" then
		self:InitQuery()
	else
		self:InitAsk()
	end
end


function CUserAdviceWnd:OnClickQuestionType(index)
	local item = self.m_QuestionType:GetItem(index)
	if item.m_QuestionTypeId then
		self:SetQuestionType(item.m_QuestionTypeId)
	end
end

function CUserAdviceWnd:OnClickQuestion(index)
	local item = self.m_QuestionList:GetSubItem(index, 0)
	local questionType = Lan_UserAdvice_Common(item.m_QuestionTypeId, "questionType")
	local typeOptionTbl =  GetCfgTransformValue(true, "Lan_UserAdvice_Common", item.m_QuestionTypeId, "typeOption")
	local typeOption = typeOptionTbl[item.m_TypeOptionId]
	local askContent = GetStaticTextClient(7100, questionType, typeOption, item.m_AskTimeStr, item.m_QuestionText)
	self.m_AskContent:SetWndText(askContent)
	
	local answerContent = ""
	if item.m_AnswerText and item.m_AnswerText ~= "" then
		answerContent = GetStaticTextClient(7101, item.m_AnswerTime, item.m_AnswerText)
	else
		answerContent = GetStaticTextClient(7102)
	end
	self.m_AnswerContent:SetWndText(answerContent)
end

function CUserAdviceWnd:CommitQuestion()
	if self.m_QuestionType:GetCurrentSel() == -1 then
		g_GameMain.m_MsgBox = MessageBox(g_GameMain,GetStaticTextClient(7038) , MB_BtnOK, CallBack, g_GameMain)
		return
	end
	if self.m_TypeOption:GetCurrentSel() == -1 then
		g_GameMain.m_MsgBox = MessageBox(g_GameMain,GetStaticTextClient(7040) , MB_BtnOK, CallBack, g_GameMain)
		return
	end
	
	local now = os.time()
	if self.m_LastCommitTime and now - self.m_LastCommitTime < 10 then
		g_GameMain.m_MsgBox = MessageBox(g_GameMain,GetStaticTextClient(7039) , MB_BtnOK, CallBack, g_GameMain)
		return 
	end
	
	local item = self.m_QuestionType:GetItem(self.m_QuestionType:GetCurrentSel())
	local questionTypeId = item.m_QuestionTypeId
	local typeOptionId = self.m_TypeOption:GetCurrentSel() + 1
	
	local textFltMgr = CTextFilterMgr:new()
	local questionText = self.m_Content:GetWndText()
	questionText = textFltMgr:RemoveTab1(questionText)
	if string.len(questionText) > 360 then
		g_GameMain.m_MsgBox = MessageBox(g_GameMain,GetStaticTextClient(7050) , MB_BtnOK, CallBack, g_GameMain)
		return
	end
	if string.len(questionText) == 0 then
		g_GameMain.m_MsgBox = MessageBox(g_GameMain,GetStaticTextClient(7043) , MB_BtnOK, CallBack, g_GameMain)
		return
	end
	
	Gac2Gas:SendUserAdviceInfo(g_Conn, questionText, questionTypeId, typeOptionId)
	self.m_Content:SetWndText("")
	self.m_LastCommitTime = now
end

local function CallBack(GameWnd, uButton)	
	if g_GameMain.m_UserAdviceWnd and g_GameMain.m_UserAdviceWnd:IsShow() then
		g_GameMain.m_UserAdviceWnd.m_Content:SetWndText("")	
	end
	return true
end




function CUserAdviceWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_BtnClose) then
			self:ShowWnd(false)
		elseif (Child == self.m_BtnCommit) then
			self:CommitQuestion()
		elseif (Child == self.m_BtnChange) then
			self:ChangeType()
		end
		
	elseif(uMsgID == ITEM_LBUTTONCLICK or uMsgID == ITEM_SHIFTDOWN or uMsgID == ITEM_CTRLDOWN) then
		if (Child == self.m_QuestionList) then
			if self.m_Type == "Query" then
				local index = self.m_QuestionList:GetSelectItem(-1)
				self:OnClickQuestion(index)
			end
		elseif (Child == self.m_QuestionType) then
			if self.m_Type == "Ask" then
				local index = self.m_QuestionType:GetCurrentSel()
				self:OnClickQuestionType(index)
			end
		end
	end
end


