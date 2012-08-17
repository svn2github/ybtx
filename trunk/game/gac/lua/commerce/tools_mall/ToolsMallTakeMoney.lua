CToolsMallTakeMoneyWnd = class ( SQRDialog )

local function CreateToolsMallTakeMoney(wnd)
    wnd:CreateFromRes("ToolsMallTakeMoney", g_GameMain)
    g_ExcludeWndMgr:InitExcludeWnd(wnd, "·Ç»¥³â")
    wnd:InitChild()
end

function CToolsMallTakeMoneyWnd:Ctor()
    CreateToolsMallTakeMoney(self)
end

function CToolsMallTakeMoneyWnd:InitChild()
    self.m_CloseBtn     =   self:GetDlgChild("CloseBtn")
    self.m_PreBtn       =   self:GetDlgChild("PreBtn")
    self.m_LastBtn      =   self:GetDlgChild("LastBtn")
    self.m_OkBtn        =   self:GetDlgChild("OkBtn")
    self.m_CancelBtn    =   self:GetDlgChild("CancelBtn")
    self.m_Text         =   self:GetDlgChild("Text")
    self.m_MoneyCountBtn=   self:GetDlgChild("MoneyCount")
    self.m_MoneyCount   =   0
    self:SetStyle(0x00040000)
end


function CToolsMallTakeMoneyWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
    if (uMsgID == BUTTON_LCLICK) then
        if (Child == self.m_CloseBtn) or 
            (Child == self.m_CancelBtn) then
            self:ShowWnd(false)            
        
        elseif (Child == self.m_PreBtn) then
            self.m_MoneyCount = self.m_MoneyCount - 1
            if self.m_MoneyCount < 0 then
               self.m_MoneyCount  = 0 
            end
            self.m_MoneyCountBtn:SetWndText(self.m_MoneyCount)
            
        elseif (Child == self.m_LastBtn) then
            self.m_MoneyCount = self.m_MoneyCount + 1
            if self.m_MoneyCount > g_GameMain.m_ToolsMallWnd.m_MaxYongBingBi then
                self.m_MoneyCount = g_GameMain.m_ToolsMallWnd.m_MaxYongBingBi
            end
            self.m_MoneyCountBtn:SetWndText(self.m_MoneyCount)
        
        elseif (Child == self.m_OkBtn) then
            if self.m_MoneyCount == 0 then
                MessageBox(self, MsgBoxMsg(303), MB_BtnOK)
                return
            elseif self.m_MoneyCount > g_GameMain.m_ToolsMallWnd.m_MaxYongBingBi then
                MessageBox(self, MsgBoxMsg(304), MB_BtnOK)
                return
            else
                local function callback(Context,Button)
                    if Button == MB_BtnOK then
                        self:ShowWnd(false)  
                        Gac2Gas:RequestTakeYongBingBi(g_Conn, self.m_MoneyCount)
                    end
                    return true
                end
                MessageBox( self,  MsgBoxMsg(305, self.m_MoneyCount), BitOr( MB_BtnOK, MB_BtnCancel),callback)
                
            end
        end
    
    elseif (uMsgID == WND_NOTIFY ) then
        if (WM_IME_CHAR == uParam1 or WM_CHAR == uParam1) then
            if Child == self.m_MoneyCountBtn then
                local inputMoney = g_GameMain.m_ToolsMallBuyWnd:GetInputCount(self.m_MoneyCountBtn, g_GameMain.m_ToolsMallWnd.m_MaxYongBingBi)
                self.m_MoneyCountBtn:SetWndText(inputMoney)  
                self.m_MoneyCount = inputMoney      
            end
        end
    end
end
