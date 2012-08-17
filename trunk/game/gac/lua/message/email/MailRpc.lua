CMailRpc = class()

function CMailRpc.RetMailListBegin(Conn)
	g_GameMain.m_EmailBox.m_MailInfo = {}
	g_GameMain.m_EmailBox.EmailWndTbl = {}
	g_GameMain.m_EmailBox.NowChooseEmailTbl = {}
	g_GameMain.m_EmailBox.NowEmail = {} 
	g_GameMain.m_EmailBox.m_SelectTokenWndTbl = {}
	g_GameMain.m_EmailBox.m_ItemCount = -1
	g_GameMain.m_EmailBox.m_EmailListBox:DeleteAllItem()
end

function CMailRpc.RetMailList(Conn,mailID,senderID,mailTitle,mailState,senderName,MailType,mail_LeftSeconds)
		local textFilter = CTextFilterMgr:new()
  	g_GameMain.m_EmailBox.m_ItemCount = g_GameMain.m_EmailBox.m_ItemCount + 1
  	local itemCount = g_GameMain.m_EmailBox.m_ItemCount
    if senderID == 0 then
    	mailTitle = textFilter:GetRealStringByMessageID(mailTitle)
    	senderName = textFilter:GetRealStringByMessageID(senderName)
    end
    local mail = {}
  	mail.ID = mailID
  	mail.senderID = senderID
   	mail.title = mailTitle
  	mail.state = mailState
  	mail.mail_LeftSeconds = mail_LeftSeconds
  	mail.senderName = senderName
  	mail.time = mailTime
  	table.insert(g_GameMain.m_EmailBox.m_MailInfo,mail)

end

function CMailRpc.RetMailListEnd(Conn)
    g_GameMain.m_EmailBox:DrawWndMailInfo()
    g_GameMain.m_EmailBox:EnableBtnFalse() 
	SetEvent(Event.Test.GetEmailListEnded,true,true)
end

function CMailRpc.RetDeleteMail(Conn,IsSuccessful)
	if (g_GameMain.m_ReceiveBox:IsShow() == true)then
	    g_GameMain.m_ReceiveBox:ShowWnd(false)
	end
	Gac2Gas:GetMailList(g_Conn)
	    
	SetEvent(Event.Test.DeleteMailEnded,true,IsSuccessful)
end

function CMailRpc.OpenMailWnd(Conn)
	EmailWnd()
end

--右键点击包裹里的邮件文本附件，查看文本信息返回函数
--参数：时间、发送者、标题、内容、类型（0为普通用户邮件，1为系统邮件）
function CMailRpc.RetCheckMailTextAttachment(Conn, sendMailTime, sender, mailTitle, mailTextAttach, senderType,strType)
	if g_GameMain.m_MailTextAttachWnd == nil then
		g_GameMain.m_MailTextAttachWnd = CMailTextAttachWnd.CreateMailTextAttachWnd(g_GameMain)
	elseif g_GameMain.m_MailTextAttachWnd:IsShow() == false then
		g_GameMain.m_MailTextAttachWnd:ShowWnd(true)
	end
	local textFilter = CTextFilterMgr:new()
	if senderType == 1 or 0 == senderType then
		sender = GetStaticTextClient(1131) .. "#r" .. sender
	else
		sender = "--" .. GetLogClient(tonumber(sender))
		mailTitle =  textFilter:GetRealStringByMessageID(mailTitle,strType)
		if senderType == 2 then
			mailTextAttach =  textFilter:GetRealStringByMessageID(mailTextAttach,strType)
		end
	end
	local sYear			= os.date("%Y", sendMailTime)
	local sMonth		= os.date("%m", sendMailTime)
	local sDay			= os.date("%d", sendMailTime)
	local sTime			= os.date("%X", sendMailTime)
	local send_time			= GetStaticTextClient(10014, sYear, sMonth, sDay, sTime)
	local wnd = g_GameMain.m_MailTextAttachWnd.m_MailText
	g_GameMain.m_MailTextAttachWnd.m_MailText:SetWndText("" .. GetStaticTextClient(1082) .. ": " .. mailTitle .. "#r" .. mailTextAttach .. "#r" .. sender .. "#r" .. send_time )
end

--从服务器返回邮件的详细信息
function CMailRpc.RetGetMailInfo(Conn,mailID,senderID,receiverID,EmailTitle,EmailText,EmailState,MoneyAtta,SenderName,MailType,strType)

	if receiverID ~= g_MainPlayer.m_Properties:GetCharID() then
		return
	end
	local EmailTextWarning = GetStaticTextClient(1129)
	if senderID == 0 then
		local textFilter = CTextFilterMgr:new()
		EmailTitle =  textFilter:GetRealStringByMessageID(EmailTitle,strType)
		SenderName =  textFilter:GetRealStringByMessageID(SenderName,strType)
		if MailType ~= 3 then
			EmailText =  textFilter:GetRealStringByMessageID(EmailText,strType)
		end
		EmailTextWarning = GetStaticTextClient(1130)
	end
	if string.len(EmailText) > 0 then
		EmailText = EmailTextWarning .. "#r" .. "#cffffff" .. EmailText
	end
	g_GameMain.m_ReceiveBox:ShowGetMailWnd(SenderName,EmailTitle,EmailText,MoneyAtta,senderID,EmailState)
	g_GameMain.m_EmailBox.senderID = senderID
	g_GameMain.m_EmailBox.SenderName = SenderName
	g_GameMain.m_EmailBox.mailID = mailID
	for i=1, # g_GameMain.m_EmailBox.m_MailInfo do
        if mailID == g_GameMain.m_EmailBox.m_MailInfo[i].ID then
	        g_GameMain.m_EmailBox.m_MailInfo[i].state = EmailState
	        break
        end
	end 
	g_GameMain.m_ReceiveBox.m_EmailGetGoodsTbl = {}
	local choosedMail = g_GameMain.m_EmailBox.EmailWndTbl.m_ChoosedMail
	if choosedMail and choosedMail.itemvalue then
		g_GameMain.m_EmailBox:CopyMail2Wnd(choosedMail.itemvalue,choosedMail)
	end
	SetEvent( Event.Test.GetEmailEnded,true,"GetEmailSuccessfully")
end

function CMailRpc.RetGetMailGoods(Conn,mailid,goods_id,emailIndex,BigID,Index,count)--EmailID, 物品id,第几个格子,物品类型名,物品名 及 物品数量
	table.insert(g_GameMain.m_ReceiveBox.m_EmailGetGoodsTbl,{emailIndex,BigID,Index,count,mailid,goods_id})
end


function CMailRpc.RetGetMailGoodsEnd(Conn,mailID)  --物品接收完成,显示
    for i=1, # g_GameMain.m_EmailBox.m_MailInfo do
        if mailID == g_GameMain.m_EmailBox.m_MailInfo[i].ID then
            if # g_GameMain.m_ReceiveBox.m_EmailGetGoodsTbl == 0 and
        		g_GameMain.m_ReceiveBox.m_AttacMoney == 0 then
        		g_GameMain.m_ReceiveBox.m_EmailGetGoodsTbl.state = 3		--查看并且没有附件
        	else	
        		g_GameMain.m_ReceiveBox.m_EmailGetGoodsTbl.state = 2		--查看未提取附件
        	end
	        g_GameMain.m_ReceiveBox:ShowGetMailGoods(mailID)
        end
    end
end


function CMailRpc.RetTakeAtachment(Conn,succeed, mailID)--提取全部附件
	if(not succeed)then  --提取附件失败
		return
	end
	g_DelWndImage(g_GameMain.m_ReceiveBox.m_Goods_01, 1, IP_ENABLE, IP_CLICKDOWN)
	g_GameMain.m_ReceiveBox.m_Goods_01:SetWndText("")
	g_GameMain.m_ReceiveBox.m_GoodsName_01:SetWndText("")
	g_GameMain.m_ReceiveBox.m_Goods_01:SetMouseOverDesc("")
	g_GameMain.m_ReceiveBox.m_Goods_01:EnableWnd(false)

	g_DelWndImage(g_GameMain.m_ReceiveBox.m_Goods_02, 1, IP_ENABLE, IP_CLICKDOWN)
	g_GameMain.m_ReceiveBox.m_Goods_02:SetWndText("")
	g_GameMain.m_ReceiveBox.m_GoodsName_02:SetWndText("")
	g_GameMain.m_ReceiveBox.m_Goods_02:SetMouseOverDesc("")
	g_GameMain.m_ReceiveBox.m_Goods_02:EnableWnd(false)

	g_DelWndImage(g_GameMain.m_ReceiveBox.m_Goods_03, 1, IP_ENABLE, IP_CLICKDOWN)
	g_GameMain.m_ReceiveBox.m_Goods_03:SetWndText("")
	g_GameMain.m_ReceiveBox.m_GoodsName_03:SetWndText("")
	g_GameMain.m_ReceiveBox.m_Goods_03:SetMouseOverDesc("")
	g_GameMain.m_ReceiveBox.m_Goods_03:EnableWnd(false)

	g_DelWndImage(g_GameMain.m_ReceiveBox.m_Goods_04, 1, IP_ENABLE, IP_CLICKDOWN)
	g_GameMain.m_ReceiveBox.m_Goods_04:SetWndText("")
	g_GameMain.m_ReceiveBox.m_GoodsName_04:SetWndText("")
	g_GameMain.m_ReceiveBox.m_Goods_04:SetMouseOverDesc("")
	g_GameMain.m_ReceiveBox.m_Goods_04:EnableWnd(false)

	g_GameMain.m_ReceiveBox.m_EmailGetGoodsTbl = {}
	for i=1, # g_GameMain.m_EmailBox.m_MailInfo do
	    if g_GameMain.m_EmailBox.m_MailInfo[i].ID == mailID then
            g_GameMain.m_EmailBox.m_MailInfo[i].state = 3	
        end
	end
end


function CMailRpc.RetTakeAtachmentByIndex(Conn,index,succeed, mailID)
	--通过玩家选择提取附件返回
	if(succeed == false)then  --提取附件失败
		return
	end
	if( index == 1)then
		g_DelWndImage(g_GameMain.m_ReceiveBox.m_Goods_01, 1, IP_ENABLE, IP_CLICKDOWN)
		g_GameMain.m_ReceiveBox.m_Goods_01:EnableWnd(false)
		g_GameMain.m_ReceiveBox.m_Goods_01:SetWndText("")
		g_GameMain.m_ReceiveBox.m_Goods_01:SetMouseOverDesc("")
		g_GameMain.m_ReceiveBox.m_GoodsName_01:SetWndText("")
	elseif(index == 2)then
		g_DelWndImage(g_GameMain.m_ReceiveBox.m_Goods_02, 1, IP_ENABLE, IP_CLICKDOWN)
		g_GameMain.m_ReceiveBox.m_Goods_02:EnableWnd(false)
		g_GameMain.m_ReceiveBox.m_Goods_02:SetWndText("")
		g_GameMain.m_ReceiveBox.m_Goods_02:SetMouseOverDesc("")
		g_GameMain.m_ReceiveBox.m_GoodsName_02:SetWndText("")
	elseif(index == 3)then
		g_DelWndImage(g_GameMain.m_ReceiveBox.m_Goods_03, 1, IP_ENABLE, IP_CLICKDOWN)
		g_GameMain.m_ReceiveBox.m_Goods_03:EnableWnd(false)
		g_GameMain.m_ReceiveBox.m_Goods_03:SetWndText("")
		g_GameMain.m_ReceiveBox.m_Goods_03:SetMouseOverDesc("")
		g_GameMain.m_ReceiveBox.m_GoodsName_03:SetWndText("")
	elseif(index == 4)then
		g_DelWndImage(g_GameMain.m_ReceiveBox.m_Goods_04, 1, IP_ENABLE, IP_CLICKDOWN)
		g_GameMain.m_ReceiveBox.m_Goods_04:EnableWnd(false)
		g_GameMain.m_ReceiveBox.m_Goods_04:SetWndText("")
		g_GameMain.m_ReceiveBox.m_Goods_04:SetMouseOverDesc("")
		g_GameMain.m_ReceiveBox.m_GoodsName_04:SetWndText("")
	elseif(index == 0)then
		g_GameMain.m_ReceiveBox.m_Money:EnableWnd(false)
		g_GameMain.m_ReceiveBox.m_Jin:SetWndText("")
		g_GameMain.m_ReceiveBox.m_Yin:SetWndText("")
		g_GameMain.m_ReceiveBox.m_Tong:SetWndText("")
		g_GameMain.m_ReceiveBox.m_AttacMoney = 0
	end
	for i =1, 5 do
		if g_GameMain.m_ReceiveBox.m_EmailGetGoodsTbl[i] ~= nil then
			if g_GameMain.m_ReceiveBox.m_EmailGetGoodsTbl[i][1] == index then 
				g_GameMain.m_ReceiveBox.m_EmailGetGoodsTbl[i] = nil
			end
		end
	end
	
	local haveNoGoods = true
	for i=1, 5 do
		if g_GameMain.m_ReceiveBox.m_EmailGetGoodsTbl[i] ~= nil then
			haveNoGoods = false
		end
	end
	if haveNoGoods and g_GameMain.m_ReceiveBox.m_AttacMoney == 0 then
	    for i=1, # g_GameMain.m_EmailBox.m_MailInfo do
            if  g_GameMain.m_EmailBox.m_MailInfo[i].ID == mailID then
	            g_GameMain.m_EmailBox.m_MailInfo[i].state = 3	
            end	        
	    end
		
	end
end

--返回发送邮件后的信息,成功或失败
function CMailRpc.RetSendEmail(Conn,IsSuccessful)
	local retMsg = IsSuccessful
	if(retMsg == true)then
		g_GameMain.m_SendBox:AddSenderList(g_GameMain.m_SendBox.m_ReceiverName:GetWndText())
		g_GameMain.m_SendBox:CleanAllGoodsInformation()
		g_GameMain.m_SendBox:ShowWnd(false)
		g_GameMain.m_WndMainBag:ShowWnd(false)
	else
		g_GameMain.m_SendBox.m_ReceiverName:SetWndText("")
		g_GameMain.m_SendBox.m_emailGoodsTbl = {}
		g_GameMain.m_SendBox.m_Jin:SetWndText("")
		g_GameMain.m_SendBox.m_Yin:SetWndText("")
		g_GameMain.m_SendBox.m_Tong:SetWndText("")
		g_GameMain.m_SendBox:CleanAllGoodsInformation()
		g_GameMain.m_SendBox.m_emailGoods = nil
		g_GameMain.m_SendBox.GoodsID = {}
		g_GameMain.m_SendBox.m_emailGoodsTbl = {}
		g_GameMain.m_SendBox.Goodsname = nil
		g_GameMain.m_SendBox.GoodsType =nil
	end
	--仍然停留在发件箱
	SetEvent( Event.Test.SendEmailEnded,true,IsSuccessful)
end


--发送物品函数
function CMailRpc.SendMailGoodsBegin(Conn)
	local strReceiverName = g_GameMain.m_SendBox.m_ReceiverName:GetWndText()
	for i =1, #(g_GameMain.m_SendBox.m_emailGoodsTbl) do
		Gac2Gas:SendMailGoods(Conn, g_GameMain.m_SendBox.m_emailGoodsTbl[i][3],g_GameMain.m_SendBox.m_emailGoodsTbl[i][1])
	end
	Gac2Gas:SendMailGoodsEnd(Conn)
	--发送完清空临时表  无论是否发送成功
	g_GameMain.m_SendBox.m_emailGoods = nil
	g_GameMain.m_SendBox.GoodsID = {}
	g_GameMain.m_SendBox.m_emailGoodsTbl = {}
	g_GameMain.m_SendBox.Goodsname = nil
	g_GameMain.m_SendBox.GoodsType =nil
end

