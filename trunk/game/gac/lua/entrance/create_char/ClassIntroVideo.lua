cfg_load "video_res/ClassIntroVideo_Client"

CClassIntroVideo = class(SQRDialog)

function CClassIntroVideo:Ctor()
	self:CreateFromRes("ClassIntroVideo", g_NewRole)
end

function CClassIntroVideo:OnChildCreated()
	self.m_Video	= self:GetDlgChild("Video")
	self.m_PlayBtn	= self:GetDlgChild("PlayBtn")
end

function CClassIntroVideo:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_PlayBtn) then
			self.m_PlayBtn:ShowWnd(false)
			self.m_Video:Play()
		end
	end
end

function CClassIntroVideo:LoadAndPause(nClass)
	local sPath = ClassIntroVideo_Client(nClass, "VideoPath")
	self.m_Video:Load(sPath)
	self.m_Video:Pause()
	self.m_PlayBtn:SetFocus()
	self.m_PlayBtn:ShowWnd(true)
end

function CClassIntroVideo:Pause()
	self.m_Video:Pause()
end