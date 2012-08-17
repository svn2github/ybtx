CArmyCorpsChangePosWnd 		= class(SQRDialog)

function CArmyCorpsChangePosWnd:Ctor(parent)
	self:CreateFromRes("ArmyCorpsChangePos", parent)
end

function CArmyCorpsChangePosWnd.GetWnd()
	local Wnd = g_GameMain.m_ArmyCorpsChangePosWnd
	if not Wnd then
		Wnd = CArmyCorpsChangePosWnd:new(g_GameMain)
		g_GameMain.m_ArmyCorpsChangePosWnd = Wnd
	end
	return Wnd
end

function CArmyCorpsChangePosWnd:OnChildCreated()
	self.m_Pos1				= self:GetDlgChild("PosBtn1")
	self.m_Pos2				= self:GetDlgChild("PosBtn2")
	self.m_Pos3				= self:GetDlgChild("PosBtn3")
	self.m_OKBtn			= self:GetDlgChild("OkBtn")
	self.m_CancelBtn	= self:GetDlgChild("CancelBtn")
	self.m_ShowInfo	= self:GetDlgChild("ShowInfo")
	
	self.m_ChangeToPos= 0
	self.m_uTargetId = 0
end

--消息
function CArmyCorpsChangePosWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_Pos1) then
			self.m_ChangeToPos= g_ArmyCorpsMgr:GetPosIndexByName("团长")
		elseif(Child == self.m_Pos2) then
			self.m_ChangeToPos= g_ArmyCorpsMgr:GetPosIndexByName("副团长")
		elseif(Child == self.m_Pos3) then
			self.m_ChangeToPos= g_ArmyCorpsMgr:GetPosIndexByName("团员")
		elseif(Child == self.m_OKBtn) then
			self:OnChangeTargetPos()
		elseif(Child == self.m_CancelBtn) then
			self:OnCancel()
		end
	end
end

function CArmyCorpsChangePosWnd:OnChangeTargetPos()
	if self.m_ChangeToPos == 0 then
		MsgClient(8528)
		return
	end
	Gac2Gas:ChangeArmyCorpsPos(g_Conn, self.m_uTargetId, self.m_ChangeToPos)
	self:OpenPanel(false,0,"")
end

function CArmyCorpsChangePosWnd:OnCancel()
	self:ShowWnd(false)
	self.m_ChangeToPos= 0
	self.m_uTargetId = 0
	self.m_ShowInfo:SetWndText("")
end

function CArmyCorpsChangePosWnd:OpenPanel(bFlag,uTaretId,sTargetName)
	if bFlag then
		self.m_uTargetId = uTaretId
		self.m_ShowInfo:SetWndText(GetStaticTextClient(9309, sTargetName))
		self:ShowWnd(true)
		self:SetFocus()
	else
		self:OnCancel()
	end
end
