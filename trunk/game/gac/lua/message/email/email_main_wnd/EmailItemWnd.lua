CEMailItemWnd = class(SQRDialog)

function CEMailItemWnd:SelectMailItem(clickChild)
    local selTokenWndTbl = g_GameMain.m_EmailBox.m_SelectTokenWndTbl
    if clickChild:GetCheck() then
   		 clickChild:SetCheck(false)
   	else
   		clickChild:SetCheck(true)
   	end
   	
   	for j =1, #(g_GameMain.m_EmailBox.NowChooseEmailTbl) do
      if self == g_GameMain.m_EmailBox.NowChooseEmailTbl[j] then
         table.remove(g_GameMain.m_EmailBox.NowEmail, j)
         table.remove(g_GameMain.m_EmailBox.NowChooseEmailTbl, j)
      end 
    end
    local check_flag = clickChild:GetCheck()
    if check_flag then
    	for i=1, # selTokenWndTbl do
        if clickChild == selTokenWndTbl[i] then 
        	if 0 == #g_GameMain.m_EmailBox.NowChooseEmailTbl then
        			table.insert(g_GameMain.m_EmailBox.NowEmail,g_GameMain.m_EmailBox.m_MailInfo[i])
            	table.insert(g_GameMain.m_EmailBox.NowChooseEmailTbl, self)
          else
        		for j=1,#g_GameMain.m_EmailBox.NowChooseEmailTbl do
        			if self == g_GameMain.m_EmailBox.NowChooseEmailTbl[j] then
        				break
        			end
        			if j == #g_GameMain.m_EmailBox.NowChooseEmailTbl then
        				table.insert(g_GameMain.m_EmailBox.NowEmail,g_GameMain.m_EmailBox.m_MailInfo[i])
            		table.insert(g_GameMain.m_EmailBox.NowChooseEmailTbl, self)
          		end
        		end
         	end
        end
       end
    end     
end

function CEMailItemWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)  --点击邮件事件
	if(uMsgID == BUTTON_LCLICK ) then
		if(Child == self:GetDlgChild("SelectToken"))then
			self:SelectMailItem(Child)      
    elseif (Child == self:GetDlgChild("ItemBtn")) then
      local choosedItem = Child.Index
    	local choosedMail = g_GameMain.m_EmailBox.m_MailInfo[choosedItem]
    	g_GameMain.m_EmailBox.EmailWndTbl.m_ChoosedMail = choosedMail
    	Gac2Gas:GetMail(g_Conn,choosedMail.ID)
   	end
   	local choosedMail = g_GameMain.m_EmailBox.EmailWndTbl.m_ChoosedMail
		if(#(g_GameMain.m_EmailBox.NowEmail) == 1 and choosedMail) or (not choosedMail and #(g_GameMain.m_EmailBox.NowEmail) > 0) then
			g_GameMain.m_EmailBox:EnableBtnTrue()
		else
			g_GameMain.m_EmailBox:EnableBtnFalse()
		end              
	end
end
