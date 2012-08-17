CNewRoleGuideWnd = class (SQRDialog)

local AnswerNumOne   = 1
local AnswerNumTwo   = 2
local AnswerNumThree = 3
local MaxQuestioNum  = 10
local ClassQuestionAndAnswerCountTbl = {}
ClassQuestionAndAnswerCountTbl["DaJian"]    = 0
ClassQuestionAndAnswerCountTbl["MoJian"]    = 0
ClassQuestionAndAnswerCountTbl["ShouRen"]   = 0
ClassQuestionAndAnswerCountTbl["FaShi"]     = 0
ClassQuestionAndAnswerCountTbl["JiSi"]      = 0
ClassQuestionAndAnswerCountTbl["XieMo"]     = 0

local ClassDescMapClassID = {}
ClassDescMapClassID["DaJian"]   =   EClass.eCL_Warrior
ClassDescMapClassID["MoJian"]   =   EClass.eCL_MagicWarrior
ClassDescMapClassID["ShouRen"]  =   EClass.eCL_OrcWarrior
ClassDescMapClassID["FaShi"]    =   EClass.eCL_NatureElf
ClassDescMapClassID["JiSi"]     =   EClass.eCL_Priest
ClassDescMapClassID["XieMo"]    =   EClass.eCL_EvilElf

function CNewRoleGuideWnd:Ctor()
    self:CreateFromRes("CreateRoleGuide", g_NewRole)
    g_ExcludeWndMgr:InitExcludeWnd(self, 1)
end


function CNewRoleGuideWnd:OnChildCreated()
    self.m_CloseBtn =   self:GetDlgChild("CloseBtn")
    self.m_StartBtn =   self:GetDlgChild("StartBtn")
    self.m_QuitBtn  =   self:GetDlgChild("QuitBtn")
    self.m_ThirdChoice=   self:GetDlgChild("ThirdChoice")
    self.m_Desc     =   self:GetDlgChild("Desc")
    self.m_ThirdChoice:ShowWnd(false)
    
    self.m_CurQuestionNum = 0
end

function CNewRoleGuideWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
    if uMsgID == BUTTON_LCLICK then
        if Child ==  self.m_CloseBtn then
            self:ShowWnd(false)
            g_NewRole.m_NewRoleWnd:OnBackToSelectClass()
                    
        elseif Child == self.m_QuitBtn then
            if self.m_CurQuestionNum >= 1 then
                self:DoClassGuidingTest(AnswerNumTwo)  
            else
                self:ShowWnd(false)
                g_NewRole.m_NewRoleWnd:OnBackToSelectClass()
            end
            
        elseif Child == self.m_StartBtn then 
            if self.m_CurQuestionNum == 0 then
                self:InitCreateRoleQAndA()  
                self:ResetClassQuestionAndAnswerCountTbl()
            end
            self:DoClassGuidingTest(AnswerNumOne)  
             
  
        elseif  Child == self.m_ThirdChoice  then
            self:DoClassGuidingTest(AnswerNumThree)     
        end
    end
end

function CNewRoleGuideWnd:InitCreateRoleQAndA()
    local keys = Lan_CreateRoleGuide_Client:GetKeys()
    self.questionAndAnswerInfoTbl = {}
    local questionNums = # keys
    for i=1, questionNums do
        local index = CreateRoleGuide_Client(i, "Index")
        if self.questionAndAnswerInfoTbl[index] == nil then
            self.questionAndAnswerInfoTbl[index] =  {}
            self.questionAndAnswerInfoTbl[index]["Question"] = Lan_CreateRoleGuide_Client(i, "DisplayQuestion")
            self.questionAndAnswerInfoTbl[index]["AnswersTbl"] = {}
        end
        local answer = Lan_CreateRoleGuide_Client(i, "DisplayAnswer")
        local node = {}
        node["Answer"] = answer
        node["Count"]  = {["DaJian"]= CreateRoleGuide_Client(i, "DaJian"),
        ["MoJian"]= CreateRoleGuide_Client(i, "MoJian"),
        ["ShouRen"]= CreateRoleGuide_Client(i, "ShouRen"),
        ["FaShi"]= CreateRoleGuide_Client(i, "FaShi"),
        ["JiSi"]= CreateRoleGuide_Client(i, "JiSi"),
        ["XieMo"]= CreateRoleGuide_Client(i, "XieMo")}
        
        table.insert(self.questionAndAnswerInfoTbl[index]["AnswersTbl"], node)
    end
end

function CNewRoleGuideWnd:CountPlayerClassChoice(index)
    local classCountTbl = self.questionAndAnswerInfoTbl[self.m_CurQuestionNum]["AnswersTbl"][index]["Count"]
    for i, v in pairs (classCountTbl) do
        ClassQuestionAndAnswerCountTbl[i] = ClassQuestionAndAnswerCountTbl[i] + v
    end
end

function CNewRoleGuideWnd:DoClassGuidingTest(index)
    if self.m_CurQuestionNum > 0 then
        self:CountPlayerClassChoice(index)
    end
    
    self.m_CurQuestionNum = self.m_CurQuestionNum + 1
    if self.m_CurQuestionNum > MaxQuestioNum then
        self:FinishedTheGuideQestionAndAnswer()
        return 
    end
    local question   =  self.questionAndAnswerInfoTbl[self.m_CurQuestionNum]["Question"]
    local answersTbl =  self.questionAndAnswerInfoTbl[self.m_CurQuestionNum]["AnswersTbl"]
    self.m_Desc:SetWndText(question)
    
    local answersTblLen = # answersTbl
    if answersTblLen == 3 then
        self.m_ThirdChoice:ShowWnd(true)
    else
        self.m_ThirdChoice:ShowWnd(false)
    end
    local questionWndTbl = {self.m_StartBtn, self.m_QuitBtn, self.m_ThirdChoice}
    for i, v in pairs (answersTbl) do
        local answer = v["Answer"]
        questionWndTbl[i]:SetWndText(answer)
    end
end


function CNewRoleGuideWnd:FinishedTheGuideQestionAndAnswer()
    local MaxCountClass = "DaJian"
    for i, v in pairs (ClassQuestionAndAnswerCountTbl) do
        if ClassQuestionAndAnswerCountTbl[MaxCountClass] < v then
            MaxCountClass = i  
        end
    end
    self:ShowWnd(false)
    local classID = ClassDescMapClassID[MaxCountClass]
    local displayClass = g_GameMain.m_DisplayCommonObj:GetPlayerClassForShow(classID)
    g_NewRole.m_NewRoleGuidEndWnd = CNewRoleGuideEndWnd:new()
    g_NewRole.m_NewRoleGuidEndWnd:ShowWnd(true)
    g_NewRole.m_NewRoleGuidEndWnd:SetFinishedClassQuestionAndAnswer(displayClass)
end

function CNewRoleGuideWnd:ResetClassQuestionAndAnswerCountTbl()
    ClassQuestionAndAnswerCountTbl["DaJian"]    = 0
    ClassQuestionAndAnswerCountTbl["MoJian"]    = 0
    ClassQuestionAndAnswerCountTbl["ShouRen"]   = 0
    ClassQuestionAndAnswerCountTbl["FaShi"]     = 0
    ClassQuestionAndAnswerCountTbl["JiSi"]      = 0
    ClassQuestionAndAnswerCountTbl["XieMo"]     = 0
end