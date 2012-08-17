cfg_load "image_res/MailStateImage_Common"

CEmailBoxWnd = class(SQRDialog) 
--创建邮件箱窗口   
function CEmailBoxWnd:Ctor()
	self:CreateFromRes("EmailBox",g_GameMain)
	self.m_EmailListBox = self:GetDlgChild("EmailListBox")
	self:InitEmailBoxWndChild(g_GameMain)
	self.EmailWndTbl = {}            --保存listctrl的item的table
	self.NowChooseEmailTbl = {}      --保存当前选中的邮件对应的item的表
	self.NowEmail = {}               --保存当前选中的邮件信息的表
	self.m_SelectTokenWndTbl = {}
	self.m_EmailListBox:SetMouseWheel(true)
	self:SetMouseWheel(true)
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
end

--邮件箱主界面通过成员变记录窗口中的子控件
function CEmailBoxWnd:InitEmailBoxWndChild(Parent)
	self.m_NewEmail			=	self:GetDlgChild("NewEmail")
	self.m_CheckMail		=	self:GetDlgChild("CheckMail")
	self.m_DeleteEmail		=	self:GetDlgChild("DeleteEmail")
	self.m_Close			=	self:GetDlgChild("Close")
	self.m_wndmsg = CWndEmailMSG:new()                              --退出,移除群时提示信息
	self.m_wndmsg:CreateFromRes("WndMsgChk", Parent)
	self.m_NewEmail:SetMouseWheel(true)
	self.m_DeleteEmail:SetMouseWheel(true)
	self.m_CheckMail:SetMouseWheel(true)
	self.m_Close:SetMouseWheel(true)
	self:GetDlgChild("LeftTime"):SetMouseWheel(true)
	self:GetDlgChild("SenderName"):SetMouseWheel(true)
end

--点击收件箱中的“删除邮件”按钮，检查邮件的状态
function CEmailBoxWnd:DelMailCheckMailState()
    local haveUnReadOrUnTakeAttachMail = nil
    for i=1, # self.NowEmail do
        if self.NowEmail[i].state == 1 then
            haveUnReadOrUnTakeAttachMail =1
        elseif self.NowEmail[i].state == 2 then
            haveUnReadOrUnTakeAttachMail =2
        end
    end
    local Wnd_Msg = g_GameMain.m_EmailBox.m_wndmsg
    local Wnd_Info = Wnd_Msg:GetDlgChild( "Wnd_Info" )
    Wnd_Msg:SetWndText("")
		if haveUnReadOrUnTakeAttachMail == 1 then
			Wnd_Info:SetWndText(GetStaticTextClient(1076))
		elseif haveUnReadOrUnTakeAttachMail == 2 then
			self.m_MsgBox =	MessageBox( self, MsgBoxMsg(250),MB_BtnOK)
			return
		else
			Wnd_Info:SetWndText(GetStaticTextClient(1075))
		end
		Wnd_Msg:ShowWnd(true)
end

--邮件箱窗口事件响应函数
function CEmailBoxWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == BUTTON_LCLICK ) then
		if g_MainPlayer.m_ItemBagLock and Child ~= self.m_Close then
				MsgClient(160032)
				return 
		end
		if(Child == self.m_NewEmail)then
			g_GameMain.m_SendBox.m_EmailTitle:SetWndText("")
			g_GameMain.m_SendBox.m_ReceiverName:SetWndText("")
			g_GameMain.m_SendBox.m_EmailText:SetWndText("")
			g_GameMain.m_SendBox.m_Jin:SetWndText("")
			g_GameMain.m_SendBox.m_Yin:SetWndText("")
			g_GameMain.m_SendBox.m_Tong:SetWndText("")
			g_GameMain.m_SendBox:CleanAllGoodsInformation()
			g_GameMain.m_SendBox.m_SendBtn:EnableWnd(false)
			g_GameMain.m_SendBox:SetFocus()
			g_GameMain.m_SendBox:ShowWnd(true)
			g_GameMain.m_WndMainBag:ShowWnd(true)
			g_GameMain.m_WndMainBag:SetFocus()
		elseif(Child == self.m_CheckMail)then
			self:ShowGetMailBox()
		elseif(Child == self.m_DeleteEmail) then
            self:DelMailCheckMailState()
		elseif(Child == self.m_Close)then
			g_ExcludeWndMgr:CloseAllActiveWndExclude()
		end
	end
end

--创建收件箱窗口，显示被选中的邮件的详细信息
function CEmailBoxWnd:ShowGetMailBox()
    local choosedEmailTblLen = # g_GameMain.m_EmailBox.NowEmail
    if choosedEmailTblLen > 1 then
    	MsgClient(4018) --同时只能查看一封邮件
    	return
    end
    local choosedMail = g_GameMain.m_EmailBox.NowEmail[choosedEmailTblLen]
    g_GameMain.m_EmailBox.EmailWndTbl.m_ChoosedMail = choosedMail
		if choosedMail then  
			Gac2Gas:GetMail(g_Conn,choosedMail.ID)
		end
end

--删除邮件函数
function CEmailBoxWnd:DelMail()
	if #(g_GameMain.m_EmailBox.NowEmail) == 1 then
		Gac2Gas:DeleteMail(g_Conn,g_GameMain.m_EmailBox.NowEmail[1].ID)
  else
  	Gac2Gas:DeleteBatchMailBegin(g_Conn)
		for i =1,#(g_GameMain.m_EmailBox.NowEmail) do
			Gac2Gas:DeleteBatchMail(g_Conn, g_GameMain.m_EmailBox.NowEmail[i].ID)
		end
		Gac2Gas:DeleteBatchMailEnd(g_Conn)
	end
end

--从服务器获取所有mail的相关信息
function CEmailBoxWnd:GetMailsInfoFromGas()
  self.m_EmailListBox:DeleteAllItem()
  Gac2Gas:GetMailList(g_Conn)
end

function CEmailBoxWnd:EnableBtnFalse()
	g_GameMain.m_EmailBox.m_DeleteEmail:EnableWnd(false)
	g_GameMain.m_EmailBox.m_CheckMail:EnableWnd(false)
end

function CEmailBoxWnd:EnableBtnTrue()
  	g_GameMain.m_EmailBox.m_DeleteEmail:EnableWnd(true)
  	g_GameMain.m_EmailBox.m_CheckMail:EnableWnd(true)
end

--显示邮件列表中的信息
function CEmailBoxWnd:DrawWndMailInfo()
	self.EmailWndTbl = {}
	self.m_SelectTokenWndTbl = {}
	self.m_EmailListBox:InsertColumn(0, self.m_EmailListBox:GetWndOrgWidth())
	for i=1, #(self.m_MailInfo) do
		self.m_EmailListBox:InsertItem(i-1, 30)
		local embedvalue = CEMailItemWnd:new()
		embedvalue:SetMouseWheel(true)
		local item = self.m_EmailListBox:GetSubItem( i-1, 0 )
		embedvalue:CreateFromRes( "mailItem", item) 
		embedvalue:ShowWnd( true )
		item:SetMouseWheel(true)
		local itemvalue = item:GetDlgChild( "mailItem" )
		itemvalue:SetMouseWheel(true)
		self:CopyMail2Wnd(itemvalue, self.m_MailInfo[i])
		embedvalue.ItemBtn = embedvalue:GetDlgChild( "ItemBtn" )
		embedvalue.ItemBtn:SetMouseWheel(true)
		embedvalue:GetDlgChild( "SelectToken" ):SetMouseWheel(true)
		embedvalue.ItemBtn.Index = i
		self.m_MailInfo[i].itemvalue = itemvalue
		table.insert(self.EmailWndTbl, embedvalue)
	end
end
function CEmailBoxWnd:AddImageByIP(wnd, imagePath, IP)
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP
	local DefaultImage = WND_IMAGE_LIST:new()
	DefaultImage:AddImage( wnd:GetGraphic(), -1,  imagePath, nil, CFPos:new(), "0xffffffff", 0 )
	wnd:SetWndBkImage( Flag, DefaultImage )
end

function CEmailBoxWnd:SetStatePic(nState,Wnd)
	local imagePath = MailStateImage_Common(nState, "ImagePath")
	self:AddImageByIP(Wnd, imagePath, IP_ENABLE)
end

--将邮件的需显示信息显示在邮件箱中的邮件列表中
function CEmailBoxWnd:CopyMail2Wnd(item,mail)
	local mail_ID = mail.ID
	local mail_title = mail.title
	local mail_senderName = mail.senderName
	local mail_LeftSeconds = mail.mail_LeftSeconds
	local mail_state = mail.state
	local mail_senderID =   mail.senderID
	local mail_leftDays = 0
	local mail_leftDays = self:CountLeftDays(mail_LeftSeconds)
	
	self:SetStatePic(mail_state,item:GetDlgChild("ShowPic"))
	item:GetDlgChild("MailTitle"):SetWndText(mail_title)
	item:GetDlgChild("SenderName"):SetWndText(mail_senderName)
	item:GetDlgChild("LeftDays"):SetWndText(mail_leftDays)
	--测试时
	if GacTestConfig.Need == 1 then
	   SetEvent(Event.Test.GetEmailListEnded,true,true)
	   return
	end
	local selectTokenWnd = item:GetDlgChild("SelectToken")
	selectTokenWnd.ID = mail.ID
	table.insert(self.m_SelectTokenWndTbl, selectTokenWnd)
end

--计算邮件的剩余天数
function CEmailBoxWnd:CountLeftDays(mail_leftTime)
	local leftSeconds = mail_leftTime
	local function CountDays(leftSeconds)
		if leftSeconds >= 24*60*60 then
			local day = math.floor(leftSeconds/(24*60*60))
			return day .. GetStaticTextClient(1141) --'天'
		elseif leftSeconds >= 60*60 then
    	local hours = math.floor(leftSeconds/(60*60))
    	return hours .. GetStaticTextClient(1121)
    elseif leftSeconds >= 60 then
    	local minutes = math.floor(leftSeconds/(60))
    	return minutes .. GetStaticTextClient(1120)
    else
    	return leftSeconds .. GetStaticTextClient(1119)
    end
  end
  local mail_leftDays = CountDays(leftSeconds)
	return mail_leftDays
end