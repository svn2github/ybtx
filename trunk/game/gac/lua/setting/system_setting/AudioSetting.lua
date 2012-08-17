CAudioSettingWnd = class(SQRDialog)

function CAudioSettingWnd:Ctor(parent)
	self.m_tblVolumeType = {"MusicVolume", "EffectVolume", "AmbientVolume"}
	self:CreateFromRes("AudioSetting", parent)
end

function CAudioSettingWnd:OnChildCreated()
	self.m_tblVolumeSc		= {}
	self.m_tblVolumeText	= {}
	for i = 1, 3 do
		local volume	= CRenderSystemClient_Inst():GetAudioVolume(self.m_tblVolumeType[i])
		local uVolume	= math.floor(volume*100 + 0.5)
		self.m_tblVolumeText[i]	= self:GetDlgChild(self.m_tblVolumeType[i] .. "Text")
		self.m_tblVolumeSc[i]	= self:GetDlgChild(self.m_tblVolumeType[i] .. "Sc")
		self.m_tblVolumeSc[i]:SetRange(101)
		self.m_tblVolumeSc[i]:SetPage(1)
		self.m_tblVolumeSc[i]:SetPos(uVolume)
	end
	
	local open = CRenderSystemClient_Inst():GetVoiceSwitch()
	self.m_VoiceBtn = self:GetDlgChild("VoiceBtn")
	self.m_VoiceBtn:SetCheck(open)
end

function CAudioSettingWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == SCROLL_MOVE) then
		for i = 1, 3 do
			if(Child == self.m_tblVolumeSc[i]) then
				CRenderSystemClient_Inst():SetAudioVolume( self.m_tblVolumeType[i], (Child:GetPos())/100 )
				self.m_tblVolumeText[i]:SetWndText( (Child:GetPos()) )
			end
		end
	elseif (uMsgID == BUTTON_LCLICK) then
		local bFlag = self.m_VoiceBtn:GetCheck()
		CRenderSystemClient_Inst():SetVoiceSwitch(bFlag)
	end
end

function CAudioSettingWnd:GetInfo()
	for i = 1, 3 do
		local volume	= CRenderSystemClient_Inst():GetAudioVolume(self.m_tblVolumeType[i])
		local uVolume	= math.floor(volume*100 + 0.5)
		self.m_tblVolumeSc[i]:SetPos(uVolume)
		self.m_tblVolumeText[i]:SetWndText(uVolume)
	end
	local open = CRenderSystemClient_Inst():GetVoiceSwitch()
	self.m_VoiceBtn:SetCheck(open)
end

function CAudioSettingWnd:SetDefault()
	local tbl = {"music", "effect", "ambient"}
	for i = 1, 3 do
		local volume	= CSceneConfig_GetInst():GetDefaultAudioVolume(tbl[i])
		local uVolume	= math.floor(volume*100 + 0.5)
		self.m_tblVolumeSc[i]:SetPos(uVolume)
		self.m_tblVolumeText[i]:SetWndText(uVolume)
	end
	local open = CSceneConfig_GetInst():GetDefaultVoiceSwitch()
	self.m_VoiceBtn:SetCheck(open)
	CRenderSystemClient_Inst():SetVoiceSwitch(open)
end

--背景音乐音量保存
function SaveMusicVolume(musicVolume)
	local strpath = g_RootPath .. "var/gac/MusicVolume.txt"
	local fo = assert(CLuaIO_Open(strpath, "w+"))
	local data = fo:WriteString("")
	fo:Close()
	local f = assert(CLuaIO_Open(strpath, "a"))
	f:WriteString(tostring(musicVolume))
	f:Close()
end

--背景音效音量保存
function SaveEffectVolume(effectVolume)
	local strpath = g_RootPath .. "var/gac/EffectVolume.txt"
	local fo = assert(CLuaIO_Open(strpath, "w+"))
	local data = fo:WriteString("")
	fo:Close()
	local f = assert(CLuaIO_Open(strpath, "a"))
	f:WriteString(tostring(effectVolume))
	f:Close()
end

--按CTRL+M打开或者关闭背景音乐
function OpenOrCloseMusicVolume()
	local musicVolume		= CRenderSystemClient_Inst():GetAudioVolume("MusicVolume")
	if musicVolume > 0 then
		SaveMusicVolume(musicVolume)
		CRenderSystemClient_Inst():SetAudioVolume("MusicVolume", 0)
	else
		local strpath = g_RootPath .. "var/gac/MusicVolume.txt"
		local f = CLuaIO_Open(strpath, "rb")
		local data = ""
		if f then
			data = f:ReadAll()
			f:Close()
		end
		if(data == "") then
			return 
		end
		musicVolume = tonumber(data)
		CRenderSystemClient_Inst():SetAudioVolume("MusicVolume", musicVolume)
	end
end

--按CTRL+S打开或者关闭音效
function OpenOrCloseEffectVolume()
	local effectVolume		= CRenderSystemClient_Inst():GetAudioVolume("EffectVolume")
	if effectVolume > 0 then
		SaveEffectVolume(effectVolume)
		CRenderSystemClient_Inst():SetAudioVolume("EffectVolume", 0)
	else
		local strpath = g_RootPath .. "var/gac/MusicVolume.txt"
		local f = CLuaIO_Open(strpath, "rb")
		local data = ""
		if f then
			data = f:ReadAll()
			f:Close()
		end
		if(data == "") then
			return 
		end
		effectVolume = tonumber(data)
		CRenderSystemClient_Inst():SetAudioVolume("EffectVolume", effectVolume)
	end
end