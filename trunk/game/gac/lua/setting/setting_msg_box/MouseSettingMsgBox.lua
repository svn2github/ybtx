gac_require "setting/setting_msg_box/MouseSettingMsgBoxInc"
gac_gas_require "setting/SettingMgr"

function CreateMouseSettingMsgBox(parent)
	local wnd = CMouseSettingMsgBox:new()
	wnd:Init(parent)
	return wnd
end

function CMouseSettingMsgBox:Init(parent)
	self:CreateFromRes("MouseSettingMsgBox", parent)
end

function CMouseSettingMsgBox:OnChildCreated()
	self.m_XBtn				= self:GetDlgChild("XBtn")
	self.m_OkBtn			= self:GetDlgChild("OkBtn")
	self.m_ShowAgainCheck	= self:GetDlgChild("ShowAgainCheck")
	
	self.m_tblCheck = {}
	for i = 1, 2 do
		self.m_tblCheck[i] = self:GetDlgChild("Check" .. i)
	end
	
	self.m_tblDescription = {}
	for i = 1, 2 do
		self.m_tblDescription[i] = {}
		for j = 1, 6 do
			self.m_tblDescription[i][j] = self:GetDlgChild( string.format("Description%d%d", i, j) )
			self.m_tblDescription[i][j]:InserBefore(self.m_tblCheck[i])
		end
	end
	
	self.m_OkBtn:EnableWnd(false)
end

function CMouseSettingMsgBox:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_XBtn) then
			self:ShowWnd(false)
		elseif(Child == self.m_OkBtn) then
			local nOptionType	= self:GetOptionType()
			local bShowAgain	= not self.m_ShowAgainCheck:GetCheck()
			if(self.m_bCreate) then
				g_SelectChar.m_SelectCharWnd:OnNewRole(self.m_EnterData)
				g_tblMouseCtrlForClient = {nOptionType, bShowAgain}
			else
				Gac2Gas:ChangeMouseCtrlBeforeGame(g_Conn, self.m_EnterData, nOptionType, bShowAgain)
			end
		else
			for i, v in ipairs(self.m_tblCheck) do
				if(Child == v) then
					self.m_OkBtn:EnableWnd(true)
					break
				end
			end
		end
	end
end

function CMouseSettingMsgBox:OpenPanel(bCreate, data)
	self.m_bCreate		= bCreate
	self.m_EnterData	= data
	for i, v in ipairs(self.m_tblCheck) do
		v:SetCheck(false)
	end
	self.m_OkBtn:EnableWnd(false)
	self:ShowWnd(true)
end

function CMouseSettingMsgBox:GetOptionType()
	for i, v in ipairs(self.m_tblCheck) do
		if( v:GetCheck() ) then
			return i
		end
	end
end

function Gas2Gac:ReturnChangeMouseCtrlBeforeGame(Conn, result)
	if(not g_SelectChar) then return end --网络卡的时候会导致玩家多次点击进入游戏在这里判断一下比直接让按钮不可用更好一些
	g_SelectChar.m_SelectCharWnd:OnCharEnterGame()
end