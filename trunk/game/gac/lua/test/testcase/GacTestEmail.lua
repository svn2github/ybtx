gac_require "test/common/CTstLoginControler"
engine_require "common/Misc/TypeCheck"
function InitGacTestEmail()
 	local test_Email=TestCase("GacTestEmail")
	--进入操作
	function test_Email:TestBegin()
		controler = CTstLoginControler:new()
		controler:OneStepLoginEx("123456","丁丁")
		controler:LoginOutFromGame()		
		controler:OneStepLogin()
	end
	
	--邮件系统主界面
	function test_Email:TestEmailBox()
    	local m_EmailBox = g_GameMain.m_EmailBox	
  		local m_EmailListBox = m_EmailBox:GetDlgChild("EmailListBox")
    
   		Gac2Gas:GM_Execute( g_Conn, "$openpanel(5)" )
    	local strRet = WaitEvent( true, nil, Event.Test.OpenEmailWnd )
		SetEvent( Event.Test.OpenEmailWnd, false )
      
    	assert_true(m_EmailBox:IsShow())
    	assert_equal(m_EmailListBox:GetWndText(),"","[Doesn'tHaveEmail]")
    	assert_false(m_EmailBox:GetDlgChild("CheckMail"):IsEnable(),"[NoMailBtnDisable]")
    	assert_false(m_EmailBox:GetDlgChild("TakeAttachment"):IsEnable(),"[NoMailBtnDisable]")
    	assert_false(m_EmailBox:GetDlgChild("DeleteEmail"):IsEnable(),"[NoMailBtnDisable]")
    
    	--[[m_EmailBox:OnCtrlmsg(m_EmailBox:GetDlgChild("TakeAttachment") ,BUTTON_LCLICK,0, 0)
    	--检验提取附件是否正确、待完成
    	--]]
    	--批量删除邮件  < 在有邮件的情况下 ,删除全部邮件 >
    	local strRet = WaitEvent(true, nil, Event.Test.GetEmailListEnded) --邮件列表接收完成
   		for i=1,m_EmailListBox:GetItemCount() do--删除所有附件
   			local itemvalue = m_EmailListBox:GetSubItem( i-1, 0 ):GetDlgChild( "mailItem" )
   			m_EmailBox:OnCtrlmsg(itemvalue:GetDlgChild("SelectToken") ,BUTTON_LCLICK,0, 0)
   		end
   		if(m_EmailListBox:GetItemCount()>0) then
   			m_EmailBox:OnCtrlmsg(m_EmailBox:GetDlgChild("DeleteEmail") ,BUTTON_LCLICK,0, 0)
   		end
       	m_EmailBox:OnCtrlmsg(m_EmailBox:GetDlgChild("Close") ,BUTTON_LCLICK,0, 0)
       	assert_false(m_EmailBox:IsShow())
	end
   	
	--发件箱客户端测试用例	
  	function test_Email:TestSendEmail()
  	
    	local m_EmailBox = g_GameMain.m_EmailBox
		local m_SendBox = g_GameMain.m_SendBox
    	
    	m_EmailBox:OnCtrlmsg(m_EmailBox:GetDlgChild("NewEmail") ,BUTTON_LCLICK,0, 0)
    	
    	local m_ReceiverName = m_SendBox.m_ReceiverName
    	local m_EmailTitle = m_SendBox.m_EmailTitle
    	local m_Tong = m_SendBox.m_Tong
    	local m_SendBtn    = m_SendBox.m_SendBtn
    	local m_EmailText  = m_SendBox.m_EmailText
    	local m_CancelBtn  = m_SendBox:GetDlgChild("Cancel")
    	
    	local function Check_MsgBox_Msg( expect_str )
    	  	state_sendbox = g_GameMain.m_SendBox
    		assert( state_sendbox.m_MsgBox )
    		local wnd = state_sendbox.m_MsgBox:GetDlgChild("WndInfo")
    		assert( wnd )
    	
    		assert_equal( wnd:GetWndText(), expect_str )
    	end
    	local function Click_MsgBox_OKBtn()
    		state_sendbox = g_GameMain.m_SendBox
    		assert( state_sendbox.m_MsgBox )
    		local wnd = state_sendbox.m_MsgBox:GetDlgChild("BtnOK")
    		assert( wnd )
     
    		assert( getmetatable(wnd) ~= nil )
    		wnd:SendMsg( WM_LBUTTONDOWN, 0, 0 )
    		wnd:SendMsg( WM_LBUTTONUP, 0, 0 )

   		end  
    	local function Click_MsgBox_CancelBtn()
    		state_sendbox = g_GameMain.m_SendBox
    		assert( state_sendbox.m_MsgBox )
    		local wnd = state_sendbox.m_MsgBox:GetDlgChild("BtnCancel")
    		assert( wnd )
        	
    		assert( getmetatable(wnd) ~= nil )
    		wnd:SendMsg( WM_LBUTTONDOWN, 0, 0 )
    		wnd:SendMsg( WM_LBUTTONUP, 0, 0 )
   		end 
    
    	m_EmailBox:ShowWnd(true)
    	assert_true(m_SendBox:IsShow(),"[SendEmailBoxIsShow]")
    	m_SendBox:OnCtrlmsg(m_SendBox:GetDlgChild("Cancel") ,BUTTON_LCLICK,0, 0)
    	assert_false(m_SendBox:IsShow(),"[SendEmailBoxNotShow]")
    	
    	
    	m_SendBox:ShowWnd(true)
    	
    	--测试收件人和邮件主题必须同时不为空
    	m_EmailTitle:SetWndText("")
    	m_ReceiverName:SetWndText("fjsaf")
    	m_SendBox:OnCtrlmsg(m_SendBox.m_ReceiverName and m_SendBox.m_EmailTitle,WND_NOTIFY,WM_IME_CHAR or WM_CHAR , 0)
    	assert_false(m_SendBtn:IsEnable(),"[EmailTitleCanNotBeFree]")
    	
    	m_ReceiverName:SetWndText("")
    	m_EmailTitle:SetWndText("fjsaf")
    	assert_false(m_SendBtn:IsEnable(),"[EmailTitleCanNotBeFree]")
    	
    	m_ReceiverName:SetWndText("fafadsfew")
    	m_EmailTitle:SetWndText("的说法放大")
    	m_EmailText:SetWndText("11111111")
    	m_SendBox:OnCtrlmsg(m_SendBox.m_ReceiverName and m_SendBox.m_EmailTitle,WND_NOTIFY,WM_IME_CHAR or WM_CHAR , 0)
    	assert_true(m_SendBtn:IsEnable(),"[SendBtnEnableTrue]")  
    	
    	--收件人不能为自己
    	m_ReceiverName:SetWndText(g_MainPlayer.m_Properties:GetCharName())
    	m_EmailTitle:SetWndText("的法定")
    	m_SendBox:OnCtrlmsg(m_SendBox.m_ReceiverName and m_SendBox.m_EmailTitle,WND_NOTIFY,WM_IME_CHAR or WM_CHAR , 0)
    	m_SendBox:OnCtrlmsg(m_SendBox:GetDlgChild("Send") ,BUTTON_LCLICK,0, 0)
    	Check_MsgBox_Msg("收件人不能是自己")
    	Click_MsgBox_OKBtn()
    	assert_true(m_SendBox:IsShow(),"[ReceiverCannotBeOneself]")
    	 
		-------------------------------------------------------------
		--发送信件成功后
		--需要邮资 所以需要先加钱
		Gac2Gas:GM_Execute( g_Conn, "$addmoney(1000)" );
		local strRet = WaitEvent(true,nil,Event.Test.AddMoneyGM)
		SetEvent(Event.Test.AddMoneyGM,false)
		m_ReceiverName:SetWndText("丁丁")
		m_EmailTitle:SetWndText("的法定")
		m_EmailText:SetWndText("text")
		m_SendBox:OnCtrlmsg(m_SendBox:GetDlgChild("Send") ,BUTTON_LCLICK,0, 0)
		--Check_MsgBox_Msg("确定要发送".. "12"   .. "数目的金币吗？")
		--Click_MsgBox_OKBtn()
		local strRet = WaitEvent(true,nil,Event.Test.SendEmailEnded)
		SetEvent(Event.Test.SendEmailEnded,false)
		if strRet[1] == EEventState.Success then
			assert_true(strRet[2][1])
			if strRet[2][1] == true then
				--print("发送成功")
				assert_false(m_SendBox:IsShow(),"[SendSuccessfully]")
			end
		end

		m_SendBox:ShowWnd(true)
		--收件人不存在，导致信件发送失败
		m_ReceiverName:SetWndText("wwwww")
		m_EmailTitle:SetWndText("的法定")
		m_Tong:SetWndText("12")
		m_SendBox:OnCtrlmsg(m_SendBox.m_MoneyAmount ,WND_NOTIFY,WM_IME_CHAR or WM_CHAR , 0)
    	
		m_SendBox:OnCtrlmsg(m_SendBox:GetDlgChild("Send") ,BUTTON_LCLICK,0, 0)
		Check_MsgBox_Msg("确定要发送金银12铜 吗？")
		Click_MsgBox_CancelBtn()
		assert_true(m_SendBox:IsShow(),"[CancelSendingEmail]")
    	
		SetEvent(Event.Test.MsgToClient,false)
		m_SendBox:OnCtrlmsg(m_SendBox:GetDlgChild("Send") ,BUTTON_LCLICK,0, 0)
		--Click_MsgBox_OKBtn()
    	
		local strRet = WaitEvent(true,nil,Event.Test.MsgToClient)
		SetEvent(Event.Test.MsgToClient,false)
		if strRet[1] == EEventState.Success then
			assert_equal("收件人wwwww不存在", strRet[2][1])
		else
			assert_fail("MessageToConn Failed TimeOut")
		end
		m_SendBox:ShowWnd(true)
		local strRet = WaitEvent(true,nil,Event.Test.SendEmailEnded)
		SetEvent(Event.Test.SendEmailEnded,false)
		if strRet[1] == EEventState.Success then
			assert_false(strRet[2][1])
			if strRet[2][1] == false then
				assert_true(m_SendBox:IsShow(),"[SendFailed]")
			end
		 end
		assert_true(m_SendBox:IsShow(),"[SendBoxShowTrue]")
		m_SendBox:ShowWnd(false)
		m_EmailBox:ShowWnd(false)
	end
  

	--收件箱客户端测试用例 
	function test_Email:TestReceiveEmail()
		controler:LoginOutFromGame()
		controler:OneStepLoginEx("123456","丁丁")
		    
    	local m_EmailBox = g_GameMain.m_EmailBox	
  		--local g_CGameHook = CGameAccelerator:new()
  		local m_ReceiveBox = g_GameMain.m_ReceiveBox
  		local m_SendBox = g_GameMain.m_SendBox
  		local m_EmailListBox = m_EmailBox:GetDlgChild("EmailListBox")
    	
		m_EmailListBox:SelectItem(0) 
    	Gac2Gas:GM_Execute( g_Conn, "$openpanel(5)" )
    	local strRet = WaitEvent( true, nil, Event.Test.OpenEmailWnd )
		SetEvent( Event.Test.OpenEmailWnd, false )
			
    	assert_true(m_EmailBox:IsShow())
   		-- local itemWnd = m_EmailBox:GetDlgChild("EmailListBox"):GetSubItem( 0,0 )
    	local strRet = WaitEvent(true,nil,Event.Test.GetEmailListEnded)
		SetEvent(Event.Test.GetEmailListEnded,false)
		
		--print("不为空:" .. itemWnd:GetWndText())
		if strRet[1] == EEventState.Success then
			assert_true(strRet[2][1])
			if strRet[2][1] == true then
				--print("获取邮件列表成功")
				--assert_true(g_GameMain.m_EmailBox:GetDlgChild("EmailListBox"):GetWndText() ~= "","[HasEmail]")
			end
		end
    	
    	m_EmailBox:OnCtrlmsg("",ITEM_LBUTTONCLICK,0,0)
    	local strRet = WaitEvent(true,nil,Event.Test.GetEmailListEnded)
    	local itemvalue = m_EmailListBox:GetSubItem( 0, 0 ):GetDlgChild( "mailItem" )
    	itemvalue:OnCtrlmsg(itemvalue:GetDlgChild("SelectToken") ,BUTTON_LCLICK,0, 0)
    	
    	assert_true(m_EmailBox:GetDlgChild("CheckMail"):IsEnable(),"[HasMailBtnEnable]")
    	assert_true(m_EmailBox:GetDlgChild("TakeAttachment"):IsEnable(),"[HasMailBtnEnable]")
    	assert_true(m_EmailBox:GetDlgChild("DeleteEmail"):IsEnable(),"[HasMailBtnEnable]")
		m_EmailBox:OnCtrlmsg(m_EmailBox:GetDlgChild("CheckMail") ,BUTTON_LCLICK,0, 0)
		-- Gac2Gas:GetMail(g_Conn,choosedMail.ID)
    	
    	local strGetRet = WaitEvent(true,nil,Event.Test.GetEmailEnded)
    	SetEvent(Event.Test.GetEmailEnded,false)
    	local m_senderName = m_ReceiveBox:GetDlgChild("SenderName"):GetWndText()
		local m_emailTitle = m_ReceiveBox:GetDlgChild("EmailTitle"):GetWndText()
    	if strGetRet[1] == EEventState.Success then
			if strGetRet[2][1] == "GetEmailSuccessfully" then
		       assert_true(m_ReceiveBox:IsShow())
		       assert_false(m_senderName == "")
		       assert_false(m_emailTitle == "")
			end
		end
    	
		--收件箱已打开，对收件箱窗口做测试
		m_ReceiveBox:OnCtrlmsg(m_ReceiveBox:GetDlgChild("Reply"),BUTTON_LCLICK,0,0)
		assert_false(m_ReceiveBox:IsShow())
		assert_true(m_SendBox:IsShow())
		assert_equal(m_SendBox:GetDlgChild("ReceiverName"):GetWndText(),m_senderName)
		assert_equal(m_SendBox:GetDlgChild("EmailTitle"):GetWndText(),"Re:" .. m_emailTitle)
		m_SendBox:ShowWnd(false)
		
		m_EmailListBox:SelectItem(0) 
		      
    	m_EmailBox:OnCtrlmsg("",ITEM_LBUTTONCLICK,0,0)
    	assert_true(m_EmailBox:GetDlgChild("CheckMail"):IsEnable(),"[HasMailBtnEnable]")
    	assert_true(m_EmailBox:GetDlgChild("TakeAttachment"):IsEnable(),"[HasMailBtnEnable]")
    	assert_true(m_EmailBox:GetDlgChild("DeleteEmail"):IsEnable(),"[HasMailBtnEnable]")
    	
		m_EmailBox:OnCtrlmsg(m_EmailBox:GetDlgChild("CheckMail") ,BUTTON_LCLICK,0, 0)
		local strGetRet = WaitEvent(true,nil,Event.Test.GetEmailEnded)
    	SetEvent(Event.Test.GetEmailEnded,false)
		assert_true(m_ReceiveBox:IsShow())
		  
		  
		m_ReceiveBox:OnCtrlmsg(m_ReceiveBox:GetDlgChild("DeleteEmail"),BUTTON_LCLICK,0,0)
    	
		local strDelRet = WaitEvent(true,nil,Event.Test.DeleteMailEnded)
		SetEvent(Event.Test.DeleteMailEnded,false)
		
		if strDelRet[1] == EEventState.Success then
			assert_true(strRet[2][1])
			if strDelRet[2][1] == 1 then
				--print("删除邮件成功")
				assert_false(m_ReceiveBox:IsShow(),"[DeleteSuccessfully]")
				assert_false(m_EmailBox:GetDlgChild("CheckMail"):IsEnable())
			end
		end
		 		  			
		m_EmailBox:ShowWnd(false)																					
	end   
           
	--退出操作
	function test_Email:TestEnd()
		controler:LoginOutFromGame()
	end
	
	function test_Email:teardown()
	end

end
