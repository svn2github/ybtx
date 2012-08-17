CVideoSettingWnd = class(SQRDialog)

local tblStaticTest = {
["默认"]	= GetStaticTextClient(23001),
["自定义"]	= GetStaticTextClient(23002),
["高"]		= GetStaticTextClient(23003),
["中"]		= GetStaticTextClient(23004),
["低"]		= GetStaticTextClient(23005),
["关"]		= GetStaticTextClient(23006)
}

function CVideoSettingWnd:Ctor(parent)
	self:CreateFromRes("VideoSetting", parent)
	self.m_settings = RenderSettings:new()
end

function CVideoSettingWnd:OnChildCreated()
	--比例
	self.m_WndRatioCB		= self:GetDlgChild("WndRatioCB")
	local count = 0
	local tempText = GetStaticTextClient(23007)
	local ratiostr = {"16:9", "16:10" .. tempText, "4:3", "5:4"}
	for i = 1, 4 do
		self.m_WndRatioCB:InsertItem(ratiostr[i], count, -1)
		count = count + 1
	end
	self.m_WndRatioCB:SetSel(0)
	
	--分辨率
	self.m_WndSizeCB		= self:GetDlgChild("WndSizeCB")
	local ratios = {9/16, 10/16, 3/4, 4/5}
	local maxWidthIndex = CRenderSystemClient_Inst():GetNumResWidths() - 1
	local num = 0
	for i = 0, maxWidthIndex do
		local width = CRenderSystemClient_Inst():GetResWidthAt(i)
		local height = width * ratios[1]
		self.m_WndSizeCB:InsertItem(string.format("%dx%d", width, height), num, -1)
		num = num + 1
	end
	self.m_WndSizeCB:InsertItem(tblStaticTest["自定义"], num, -1)
	self.m_WndSizeCB:SetSel(0)
	
	----------------------------------------------------------------------
	self.m_CameraSpeedScr		= self:GetDlgChild("CameraSpeedScr")
	self.m_CameraSpeedText		= self:GetDlgChild("CameraSpeedText")
	self.m_CameraSpeedScr:ShowWnd(false)
	self.m_CameraSpeedText:ShowWnd(false)
	
	self.m_CameraSpeedLable		= self:GetDlgChild("CameraSpeedLable") --这个可以在镜头滚动速度调节功能上了之后删掉
	self.m_CameraSpeedLable:ShowWnd(false)
	----------------------------------------------------------------------

	self.m_WindowedCheck		= self:GetDlgChild("WindowedCheck")
	self.m_VSyncCheck			= self:GetDlgChild("VSyncCheck")
	self.m_BloomCheck			= self:GetDlgChild("BloomCheck")
	self.m_RefractCheck			= self:GetDlgChild("RefractCheck")
	self.m_FPSLow				= self:GetDlgChild("FPSLowCheck")
	self.m_FPSMid				= self:GetDlgChild("FPSMidCheck")
	self.m_FPSHigh				= self:GetDlgChild("FPSHighCheck")
	self.m_FPSExtra				= self:GetDlgChild("FPSExtraCheck")
		
	self.m_AntiAliasingCB = self:GetDlgChild("AntiAliasingCB")
	self.m_AntiAliasingCB:InsertItem(tblStaticTest["关"], 0, -1)
	self.m_AntiAliasingCB:InsertItem(tblStaticTest["低"], 1, -1)
	self.m_AntiAliasingCB:InsertItem(tblStaticTest["高"], 2, -1)
	
	self.m_ShadowQualityCB = self:GetDlgChild("ShadowQualityCB")
	self.m_ShadowQualityCB:InsertItem(tblStaticTest["关"], 0, -1)
	self.m_ShadowQualityCB:InsertItem(tblStaticTest["低"], 1, -1)
	--self.m_ShadowQualityCB:InsertItem(tblStaticTest["中"], 2, -1)
	self.m_ShadowQualityCB:InsertItem(tblStaticTest["高"], 2, -1)
	
	self.m_TerrainQualityCB = self:GetDlgChild("TerrainQualityCB")
	self.m_TerrainQualityCB:InsertItem(tblStaticTest["低"], 0, -1)
	self.m_TerrainQualityCB:InsertItem(tblStaticTest["高"], 1, -1)
	
	self.m_WaterQualityCB = self:GetDlgChild("WaterQualityCB")
	self.m_WaterQualityCB:InsertItem(tblStaticTest["关"], 0, -1)
	self.m_WaterQualityCB:InsertItem(tblStaticTest["默认"], 1, -1)
	
	self.m_TexQualityCB = self:GetDlgChild("TextureQualityCB")
	self.m_TexQualityCB:InsertItem(tblStaticTest["低"], 0, -1)
	self.m_TexQualityCB:InsertItem(tblStaticTest["中"], 1, -1)
	self.m_TexQualityCB:InsertItem(tblStaticTest["高"], 2, -1)
	
	self.m_CameraShake		= self:GetDlgChild("CameraShake")
	self.m_EffectLevelCB	= self:GetDlgChild("EffectLevelCB")
	self.m_EffectLevelCB:InsertItem(tblStaticTest["低"], 0, -1)
	self.m_EffectLevelCB:InsertItem(tblStaticTest["中"], 1, -1)
	self.m_EffectLevelCB:InsertItem(tblStaticTest["高"], 2, -1)


	self.m_UICheck = self:GetDlgChild("UICheck")
	self.m_UIScale = self:GetDlgChild("UIScale")
	self.m_UIValue = self:GetDlgChild("UIScaleValue")
	self.m_UIScale:SetRange(65)
	self.m_UIScale:SetPage(1)
	
	self.m_MinConfigBtn		= self:GetDlgChild("MinConfigBtn")
	self.m_MaxConfigBtn		= self:GetDlgChild("MaxConfigBtn")
	self.m_NormalConfigBtn	= self:GetDlgChild("NormalConfigBtn")
	self.m_MinConfigBtn:EnableWnd(false)
	self.m_MaxConfigBtn:EnableWnd(false)
	self.m_NormalConfigBtn:EnableWnd(false)
	
end

function CVideoSettingWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == SCROLL_MOVE) then
		if(Child == self.m_UIScale) then
			self.m_UIValue:SetWndText((self.m_UIScale:GetPos()+ 68)/100)
		end
	elseif(uMsgID == ITEM_LBUTTONCLICK) then
		if(Child == self.m_UICheck) then
			self:UpdateUICheck()
		elseif(Child == self.m_WndRatioCB) then
			self:UpdateWndSizeWnd()
		end
	elseif (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_WindowedCheck) then
			--self.m_WndSizeCB:EnableWnd(self.m_WindowedCheck:GetCheck())
			self:UpdateUISize()
		end
	elseif(uMsgID == BUTTON_CLICKDOWN) then
		if(Child == self.m_UICheck) then
			self:UpdateUICheck()
		end
	end
end

function CVideoSettingWnd:GetInfo()
	CRenderSystemClient_Inst():GetRenderSettings(self.m_settings)
	local tblSettings = {}
	tblSettings.isWindowed			= self.m_settings:IsWindowed()
	tblSettings.antiAliasQuality	= self.m_settings:GetAntiAliasQuality()
	tblSettings.isVSync				= self.m_settings:IsVSync()
	tblSettings.isBloom				= CRenderSystemClient_Inst():IsBloomEnabled()
	tblSettings.isRefract			= CRenderSystemClient_Inst():IsRefractEnabled()
	tblSettings.shadowType			= CRenderSystemClient_Inst():GetShadowType()
	tblSettings.isShaderTerrain		= CRenderSystemClient_Inst():IsUseShaderTerrain()
	tblSettings.waterType			= CRenderSystemClient_Inst():GetWaterType()
	tblSettings.TexQuality			= 2 - CRenderSystemClient_Inst():GetTexQualityLevel()
	tblSettings.effectLevel			= CRenderSystemClient_Inst():GetEffectLevel()
	tblSettings.camerashake			= CRenderSystemClient_Inst():GetCameraShake()
	
	tblSettings.ratioIndex			= self:GetWindowRatioIndex()
	tblSettings.sizeIndex			= self:GetWindowSizeIndex()
	
	tblSettings.UIScale				= string.format( "%.2f", CRenderSystemClient_Inst():GetUIZoom() )
	tblSettings.RenderCyc			= CRenderSystemClient_Inst():GetRenderCyc()
	self:SetInfo(tblSettings)
end

function CVideoSettingWnd:SetDefault()
	local tblSettings = {}
	tblSettings.isWindowed				= not CSceneConfig_GetInst():GetDefaultFullScreen()
	tblSettings.antiAliasQuality	= CSceneConfig_GetInst():GetDefaultAntialias()
	tblSettings.isVSync						= CSceneConfig_GetInst():GetDefaultVSync()
	tblSettings.isBloom						= CSceneConfig_GetInst():GetDefaultBloom()
	tblSettings.isRefract					= CSceneConfig_GetInst():GetDefaultRefract()
	tblSettings.shadowType				= CSceneConfig_GetInst():GetDefaultShadowType()
	tblSettings.isShaderTerrain		= CSceneConfig_GetInst():GetDefaultShaderTile()
	tblSettings.waterType					= CSceneConfig_GetInst():GetDefaultWaterType()
	tblSettings.TexQuality				= 2 - CSceneConfig_GetInst():GetDefaultTexQuality()
	tblSettings.effectLevel				= CSceneConfig_GetInst():GetDefaultEffectLevel()
	tblSettings.camerashake			= CSceneConfig_GetInst():GetDefaultCameraShake()
	tblSettings.ratioIndex			= self:GetDefaultWindowRatioIndex()
	tblSettings.sizeIndex			= self:GetDefaultWindowSizeIndex()
	
	tblSettings.UIScale				= CSceneConfig_GetInst():GetDefaultUIScale()
	tblSettings.RenderCyc			= CAppConfigClient_Inst():GetDefaultRenderCyc()
	self:SetInfo(tblSettings)
end

function CVideoSettingWnd:ChangeSettings()
	local isWindowed		= self.m_WindowedCheck:GetCheck()
	local antiAliasQuality	= self.m_AntiAliasingCB:GetCurrentSel()*2
	local isVSync			= self.m_VSyncCheck:GetCheck()
	local shadowType		= self.m_ShadowQualityCB:GetCurrentSel()
	local isShaderTerrain = self.m_TerrainQualityCB:GetCurrentSel() == 1
	local waterType = self.m_WaterQualityCB:GetCurrentSel()
	local texQuality		= 2 - self.m_TexQualityCB:GetCurrentSel()
	local isBloom			= self.m_BloomCheck:GetCheck()
	local isRefract			= self.m_RefractCheck:GetCheck()
	local effectLevel		= self.m_EffectLevelCB:GetCurrentSel()* 0.45 + 0.1
	local camerashake		= self.m_CameraShake:GetCheck()
	local UIScale			= (self.m_UIScale:GetPos() + 68)/100
	
	if(self.m_UICheck:GetCheck()==false)then
		UIScale = 1.0
	end
	
	local wndWidth, wndHeight = self:GetSelectWndSize()
	self.m_settings:SetWidth(wndWidth)
	self.m_settings:SetHeight(wndHeight)
	self.m_settings:SetWindowed(isWindowed)
	self.m_settings:SetAntiAliasQuality(antiAliasQuality)
	self.m_settings:SetVSync(isVSync)
	CRenderSystemClient_Inst():SetShadowType(shadowType)
	CRenderSystemClient_Inst():SetUseShaderTerrain(isShaderTerrain)
	CRenderSystemClient_Inst():SetWaterType(waterType)
	CRenderSystemClient_Inst():SetBloomEnabled(isBloom)
	CRenderSystemClient_Inst():SetRefractEnabled(isRefract)
	CRenderSystemClient_Inst():SetTexQualityLevel(texQuality)
	CRenderSystemClient_Inst():SetEffectLevel(effectLevel)
	CRenderSystemClient_Inst():SetCameraShake(camerashake)
	CRenderSystemClient_Inst():SetUIZoom(UIScale)
	if self.m_FPSLow:GetCheck() then
		CRenderSystemClient_Inst():SetRenderCyc(64)
	elseif self.m_FPSMid:GetCheck() then
		CRenderSystemClient_Inst():SetRenderCyc(33)
	elseif self.m_FPSHigh:GetCheck() then
		CRenderSystemClient_Inst():SetRenderCyc(22)
	else
		CRenderSystemClient_Inst():SetRenderCyc(16)
	end
	
	CRenderSystemClient_Inst():ChangeRenderSettings(self.m_settings)
end

function CVideoSettingWnd:SetInfo(tblSettings)
	self.m_WndRatioCB:SetSel(tblSettings.ratioIndex)
	self:UpdateWndSizeWnd()
	self.m_WndSizeCB:SetSel(tblSettings.sizeIndex)
	
	self.m_WindowedCheck:SetCheck(tblSettings.isWindowed)
	self.m_ShadowQualityCB:SetSel(tblSettings.shadowType)
	self.m_TerrainQualityCB:SetSel(tblSettings.isShaderTerrain and 1 or 0)
	self.m_WaterQualityCB:SetSel(tblSettings.waterType)
	self.m_TexQualityCB:SetSel(tblSettings.TexQuality)
	self.m_AntiAliasingCB:SetSel(tblSettings.antiAliasQuality/2)
	if ( tblSettings.effectLevel < 0.25 ) then
		self.m_EffectLevelCB:SetSel(0)
	elseif( tblSettings.effectLevel < 0.75 ) then
		self.m_EffectLevelCB:SetSel(1)
	else
		self.m_EffectLevelCB:SetSel(2)
	end
	
	self.m_VSyncCheck:SetCheck(tblSettings.isVSync)
	self.m_BloomCheck:SetCheck(tblSettings.isBloom)
	self.m_RefractCheck:SetCheck(tblSettings.isRefract)
	self.m_CameraShake:SetCheck(tblSettings.camerashake)
	self.m_UICheck:SetCheck(tblSettings.UIScale~=1)
	self.m_UIScale:EnableWnd( self.m_UICheck:GetCheck() )
	self.m_UIScale:SetPos(tblSettings.UIScale*100 - 68)
	self.m_UIValue:SetWndText(tblSettings.UIScale)
	
	if tblSettings.RenderCyc>=60 then
		self.m_FPSLow:SetCheck(true)
	elseif tblSettings.RenderCyc>=30 then
		self.m_FPSMid:SetCheck(true)
	elseif tblSettings.RenderCyc>=20 then
		self.m_FPSHigh:SetCheck(true)
	else
		self.m_FPSExtra:SetCheck(true)
	end

end

function CVideoSettingWnd:UpdateWndSizeWnd()
	--清空之前填冲的数据
	local selIndex = self.m_WndSizeCB:GetCurrentSel()
	local wndWidth, wndHeight = self:GetSelectWndSize()
	local find = false
	
	self.m_WndSizeCB:RemoveAllItem()
	self.m_WndSizeCB:InsertDefaultColumn()
	--填冲
	local select = self.m_WndRatioCB:GetCurrentSel() + 1
	local ratios = {9/16, 10/16, 3/4, 4/5}
	local maxWidthIndex = CRenderSystemClient_Inst():GetNumResWidths() - 1
	local num = 0
	for i = 0, maxWidthIndex do
		local width = CRenderSystemClient_Inst():GetResWidthAt(i)
		for j = 1, 4 do
			if ( j == select ) then
				local height = width * ratios[j]
				self.m_WndSizeCB:InsertItem(string.format("%dx%d", width, height), num, -1)
				num = num + 1
				if ( wndWidth <= width ) then
					if ( find == false ) then
						selIndex = i
						find = true
					end
				end
				
			end
		end
	end
	self.m_WndSizeCB:InsertItem(tblStaticTest["自定义"], num, -1)
	
	if selIndex > maxWidthIndex then
		self.m_WndSizeCB:SetSel(0)
	elseif selIndex < 0 then
		self.m_WndSizeCB:SetSel(0)
	else
		self.m_WndSizeCB:SetSel(selIndex)
	end
end

function CVideoSettingWnd:GetWndSizeSelIdx( idx )
	local maxWidthIndex = CRenderSystemClient_Inst():GetNumResWidths() - 1
	for i = 0, maxWidthIndex do
		for j = 0, 3 do
			if ( idx == i * 4 + j ) then
				return j - 1
			end
		end
	end
end

----------------------返回选择的比率----------------------
function CVideoSettingWnd:GetWindowRatio()
	local select = self.m_WndRatioCB:GetCurrentSel()
	local ratios = {9/16, 10/16, 3/4, 4/5}
	return ratios[select + 1]
end

function CVideoSettingWnd:GetWindowRatioIdx(idx)
	return idx % 4
end

----------------------返回窗口比例------------------------
function CVideoSettingWnd:GetWinRationIdx(w, h)
	local	idx	= 0
	if (w == 0) then
		return 0
	else
		local ratios = {9/16, 10/16, 3/4, 4/5}
		local dis	= 1024
		local cur_ratio = h / w
		for i = 1, 4 do
			if ( dis > math.abs(cur_ratio - ratios[i]) ) then
				dis = math.abs(cur_ratio - ratios[i])
				idx = i
			end
		end
	end
	return idx - 1
end

---------------------返回选择的窗口大小-------------------
function CVideoSettingWnd:GetSelectWndSize()
	local selIndex = self.m_WndSizeCB:GetCurrentSel()
	local widthIndex = selIndex
	if widthIndex >= CRenderSystemClient_Inst():GetNumResWidths() then
		local rtRootWnd = CFRect:new()
		g_App:GetRootWnd():GetWndRect( rtRootWnd )
		self.m_settings:SetUseDef(true)
		return rtRootWnd:Width(), rtRootWnd:Height()
	end
	local ratio = self:GetWindowRatio()
	local wndWidth = CRenderSystemClient_Inst():GetResWidthAt(widthIndex)
	local wndHeight = wndWidth * ratio
	self.m_settings:SetUseDef(false)
	return wndWidth, wndHeight
end

function CVideoSettingWnd:GetSizeIndex(w, h)
	local ratios = {9/16, 10/16, 3/4, 4/5}
	local maxWidthIndex = CRenderSystemClient_Inst():GetNumResWidths() - 1
	
	for i = 0, maxWidthIndex do
		local width = CRenderSystemClient_Inst():GetResWidthAt(i)
		if (width - w < 1 and width - w > -1) then
			return i
		end
	end
	return maxWidthIndex + 1
end

----------------------根据大小取得选择的索引----------------------
function CVideoSettingWnd:GetWindowSizeIndex()
	local w = self.m_settings:GetWidth()
	local h = self.m_settings:GetHeight()
	return self:GetSizeIndex(w, h)
end

function CVideoSettingWnd:GetWindowRatioIndex()
	local w = self.m_settings:GetWidth()
	local h = self.m_settings:GetHeight()
	return self:GetWinRationIdx(w, h)
end

----------------------根据默认大小取得选择的索引------------------
function CVideoSettingWnd:GetDefaultWindowSizeIndex()
	local w = CAppConfigClient_Inst():GetDefaultWndWidth()
	local h = CAppConfigClient_Inst():GetDefaultWndHeight()
	return self:GetSizeIndex(w, h)
end

function CVideoSettingWnd:GetDefaultWindowRatioIndex()
	local w = CAppConfigClient_Inst():GetDefaultWndWidth()
	local h = CAppConfigClient_Inst():GetDefaultWndHeight()
	return self:GetWinRationIdx(w, h)
end

----------------------开启/关闭UI缩放-----------------------------
function CVideoSettingWnd:UpdateUICheck()
	local UIScale	= string.format( "%.2f", CRenderSystemClient_Inst():GetUIZoom() )
	if(self.m_UICheck:GetCheck())then
		self.m_UICheck:SetCheck(false)
		self.m_UIScale:EnableWnd(false)
		self.m_UIScale:SetPos(32)
		self.m_UIValue:SetWndText(1.0)
	else
		self.m_UICheck:SetCheck(true)
		self.m_UIScale:EnableWnd(true)
		self.m_UIScale:SetPos(UIScale*100 - 68)
		self.m_UIValue:SetWndText(UIScale)
	end
end

----------------------重新选择合适分辨率----------------------------
function CVideoSettingWnd:UpdateUISize()
	if( self.m_WindowedCheck:GetCheck()==false )then
		local rdsetting = RenderSettings:new()
		if( CRenderSystemClient_Inst():IsInWindowState() )then
			--由窗口进入全屏模式，重新设置当前分辨率
			local sw = CRenderSystemClient_Inst():GetScreenWidth()
			local sh = CRenderSystemClient_Inst():GetScreenHeight()
			local idx = self:GetWinRationIdx(sw, sh)
			local ratio = self:GetWindowRatio()
			if( idx < 2 )then
				--用高求宽
				sw = sh / ratio
			else
				--用宽求高
				sh = sw * ratio
			end
			--重设size
			local sizeIndex = self:GetSizeIndexMust(sw, sh)
			self.m_WndSizeCB:SetSel(sizeIndex)
		end
	end
end

--必需返回一个接近的比例
function CVideoSettingWnd:GetSizeIndexMust(w, h)
	local ratios = {9/16, 10/16, 3/4, 4/5}
	local maxWidthIndex = CRenderSystemClient_Inst():GetNumResWidths() - 1
	local idx = 0
	local dis = 5760
	for i = 0, maxWidthIndex do
		local width = CRenderSystemClient_Inst():GetResWidthAt(i)
		if (math.abs(width - w) < dis) then
			dis = math.abs(width - w)
			idx = i
		end
	end
	return idx
end

--	float	GetDefaultMinSpeed();
--	uint	GetDefaultTexHMem();
--	uint	GetDefaultTexMMen();
--	bool	GetDefaultFreeCamera();
--	float	GetDefaultMusicVolume();
--	float	GetDefaultEffectVolume();
--	float	GetDefaultAmbientVolume();
--	uint	GetDefaultLanguage();
--	uint32	GetDefaultMaxShader();
