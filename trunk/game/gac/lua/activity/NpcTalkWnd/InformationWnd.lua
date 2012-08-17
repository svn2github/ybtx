cfg_load "scene/Position_Common"
CInformationWnd = class(SQRDialog)

function CInformationWnd:Ctor()
   self:CreateFromRes("Information",g_GameMain)
   self.m_OKBtn = self:GetDlgChild("OKBtn")
end

function CInformationWnd:OnCtrlmsg(Child, uMsgID)
	local Camp = g_MainPlayer:CppGetBirthCamp()
	local Index = Camp + 4018
	local tbl = {
		Position_Common(Index, "Description"),
		Position_Common(Index, "SceneName"),
		Position_Common(Index, "PosX"),
		Position_Common(Index, "PosY")
	}
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_OKBtn then		
			PlayerAutoTrack(tbl[1], tbl[2], tbl[3], tbl[4])
		end
	end
end

function CInformationWnd:ShowText()
	self:ShowWnd(true)
end