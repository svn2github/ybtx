CToolsMallShowYongBingBiWnd = class ( SQRDialog )

local function CreateToolsMallShowYongBingBi(wnd)
    wnd:CreateFromRes("ToolsMallShowYongBingBi", g_GameMain)
    wnd:InitChild()
    g_ExcludeWndMgr:InitExcludeWnd(wnd, "非互斥")
end

function CToolsMallShowYongBingBiWnd:Ctor()
    CreateToolsMallShowYongBingBi(self)
end

function CToolsMallShowYongBingBiWnd:InitChild()
    self.m_Close    =   self:GetDlgChild("Close")
    self.m_Cancel   =   self:GetDlgChild("Cancel")
    self.m_AddMoney =   self:GetDlgChild("AddMoney")
end

function CToolsMallShowYongBingBiWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
   if (uMsgID == BUTTON_LCLICK) then
        if (Child == self.m_Close or Child == self.m_Cancel) then
            self:ShowWnd(false)
        
        elseif (Child == self.m_AddMoney) then
            --打开充值网页
        end
    end
end