CObjShowContent = class(SQRDialog)
cfg_load "int_obj/IntObj_Common" 

function CObjShowContent:Ctor(parent)
	self:CreateFromRes("WndShowTips",parent) 
	self.m_ObjName = self:GetDlgChild("WndObjName")
	self.m_CloseButton = self:GetDlgChild("WndTipsClose")
	self.m_TipsContext = self:GetDlgChild("WndTipsContext")
	--self.m_UpDown = self:GetDlgChild("WndUpDown")
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
	self:ShowWnd(false)
end

function CObjShowContent:GetWnd()
	local Wnd = g_GameMain.m_ShowContentWnd
	if not Wnd then
		Wnd = CObjShowContent:new(g_GameMain)
		g_GameMain.m_ShowContentWnd = Wnd
	end
	return Wnd
end

function CObjShowContent:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == BUTTON_LCLICK) then
		if ( Child == self.m_CloseButton) then
			--self:ShowWnd(false)
			self:ShowWnd(false)
			self.m_IsInOpened = nil
		end
	end
end

function CObjShowContent:InitShowContent(ObjName,ShowContext)
	self.m_ObjName:SetWndText(GetIntObjDispalyName(ObjName))
	ShowContext = g_QuestInfoTextTransfer(ShowContext)
	self.m_TipsContext:SetWndText(ShowContext)
	self.m_IsInOpened = true
	self:ShowWnd(true)
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(self)
end

function CObjShowContent:RetObjShowContentWnd(GlobalID)	
	local ShowObjClient = CIntObjClient_GetIntObjByID(GlobalID)
	if ShowObjClient == nil then
		return
	end

	local ObjName = ShowObjClient.m_Properties:GetCharName()
	if IntObj_Common(ObjName) ~= nil then
		local ShowContentID =g_ObjActionArgTbl[ObjName][1]
		local ShowContent = GetStaticTextClient(ShowContentID)
		self:InitShowContent(ObjName,ShowContent)	
	end
end
