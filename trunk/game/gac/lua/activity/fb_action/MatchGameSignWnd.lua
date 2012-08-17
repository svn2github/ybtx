cfg_load "fb_game/MatchGame_Common"
CMatchGameSignWnd = class(SQRDialog)

function CMatchGameSignWnd:Ctor(parent)
	self:CreateFromRes("MatchGameSignWnd",parent) 
	self.m_OkBtn = self:GetDlgChild("OkBtn")
	self.m_CancelBtn = self:GetDlgChild("CancelBtn")
	self.m_Info = self:GetDlgChild("Info")
	self.m_XBtn = self:GetDlgChild("XBtn")
end


function CMatchGameSignWnd.GetWnd(gameName)
	if not g_GameMain.m_MatchGameSignWnd then
		g_GameMain.m_MatchGameSignWnd = CMatchGameSignWnd:new(g_GameMain)
	end
	
	local wnd = g_GameMain.m_MatchGameSignWnd
	wnd.m_GameName = gameName
	local lanName = g_GetFbActionNameLanStr(gameName)
	wnd.m_Info:SetWndText(GetStaticTextClient(370001, lanName))
	wnd:ShowWnd(true)
	return wnd
end

function CMatchGameSignWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_CancelBtn) then
			self:ShowWnd(false)
		elseif (Child == self.m_XBtn) then
			self:ShowWnd(false)
		elseif (Child == self.m_OkBtn) then
			local autoTeam = MatchGame_Common(self.m_GameName, "AutoTeamPlayerNumber")
			local switch = false
			if autoTeam and autoTeam ~= "" then
				if tonumber(autoTeam) > 0 then
					switch = true
				end
			end
			Gac2Gas:JoinMatchGame(g_Conn, self.m_GameName, switch)
			self:ShowWnd(false)
		end
	end
end
