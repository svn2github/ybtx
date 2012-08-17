CNewRoleGuideEndWnd = class (SQRDialog)

function CNewRoleGuideEndWnd:Ctor()
    self:CreateFromRes("CreateRoleGuideEnd", g_NewRole)
end

function CNewRoleGuideEndWnd:OnChildCreated()
    self.m_CloseBtn =   self:GetDlgChild("CloseBtn")
    self.m_ClassDesc =   self:GetDlgChild("ClassDesc")
    self.m_EndBtn  =   self:GetDlgChild("EndBtn")
end

function CNewRoleGuideEndWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2)
    if uMsgID == BUTTON_LCLICK then
        if Child ==  self.m_CloseBtn or
            Child == self.m_EndBtn then
            self:ShowWnd(false)
            g_NewRole.m_NewRoleWnd:OnBackToSelectClass()           
        end
    end
end

function CNewRoleGuideEndWnd:SetFinishedClassQuestionAndAnswer(classDesc)
    self.m_ClassDesc:SetWndText(classDesc)
end