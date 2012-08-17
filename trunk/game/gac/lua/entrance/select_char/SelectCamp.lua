lan_load "entrance/Lan_CampRes_Client"
cfg_load "entrance/CampRes_Client"
gac_require "framework/texture_mgr/DynImageMgr"

CSelectCamp = class(SQRDialog)
local CLoginRecruitCamp = class(SQRDialog)

function CSelectCamp:CreateLoginRecruitWnd(parent, num)
	local wnd = CLoginRecruitCamp:new()
	wnd:CreateFromRes("LoginRecruitCamp", parent)
	wnd.m_Text = wnd:GetDlgChild("Text")
	return wnd
end

function CSelectCamp:SetRecruitWndInfo(wnd,num)
	if g_App.m_RecruitInfo[num] == 0 then 
		wnd:ShowWnd(false)
	elseif g_App.m_RecruitInfo[num] == 1 then 
		local imageId = CampRes_Client(num, "CommonText")
		local strTex = g_ImageMgr:GetImagePath(imageId)
		g_DynImageMgr:AddImageByIP(wnd.m_Text, strTex , IP_ENABLE)
		g_DynImageMgr:AddImageByIP(wnd.m_Text, strTex , IP_DISABLE)
		wnd:ShowWnd(true)
	elseif g_App.m_RecruitInfo[num] == 2 then  
		local imageId = CampRes_Client(num, "GoldText")
		local strTex = g_ImageMgr:GetImagePath(imageId)
		g_DynImageMgr:AddImageByIP(wnd.m_Text, strTex , IP_ENABLE)
		g_DynImageMgr:AddImageByIP(wnd.m_Text, strTex , IP_DISABLE)
		wnd:ShowWnd(true)
	end
end

function CSelectCamp:Ctor()
	self:CreateFromRes("LoginSelectCamp", g_SelectChar)
end

function CSelectCamp:OnChildCreated()
	self.m_tblCampRadio	= {}
	self.m_tblRoleIcon	= {}
	self.m_tblRecruit	= {}
	for i = 1, 3 do
		self.m_tblCampRadio[i] = self:GetDlgChild("Radio" .. i)
		--if g_App.m_RecruitInfo[i] ~= 0 then
		local itemWnd = self:CreateLoginRecruitWnd(self.m_tblCampRadio[i], i)
		self:SetRecruitWndInfo(itemWnd, i)
		table.insert(self.m_tblRecruit, itemWnd)
		--end
		self.m_tblRoleIcon[i] = {}
		for j = 1, 4 do
			local str = string.format("%s%d%d", "I", i, j)
			self.m_tblRoleIcon[i][j] = self:GetDlgChild(str)
		end
	end

	self.m_OkBtn			= self:GetDlgChild("OkBtn")
	self.m_BackBtn			= CLoginBackBtn:new(g_SelectChar)
	self.m_ChooseCampLabel	= self:GetDlgChild("ChooseCampLabel")
	self.m_Description		= self:GetDlgChild("Description")
	self.m_Mask				= self:GetDlgChild("Mask")
	self.m_RecruitInfo = self:GetDlgChild("RecruitInfo")
	self.m_tblBack = {}
	self.m_tblBack[1]		= self:GetDlgChild("Back1")
	self.m_tblBack[2]		= self:GetDlgChild("Back2")
	
	self.m_Description:SetCutWnd(self.m_Mask)
	self.m_OkBtn:ShowWnd(false)
	self.m_RecruitInfo:ShowWnd(false)
end

function CSelectCamp:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if(g_SelectChar.m_DisMsg) then return end
	
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_OkBtn) then
			self:BackUpAnimation(EGameState.eToNewRoleGameMain)
		else
			for i, v in ipairs(self.m_tblCampRadio) do
				if(Child == v) then
					self.m_ChooseCampLabel:ShowWnd(false)
					self.m_OkBtn:ShowWnd(true)
					self:DescriptionAnimation()
				end
			end
		end
	end
end

function CSelectCamp:OnDestroy()
	if(self.Tick_RadioAnimation) then
		UnRegisterTick(self.Tick_RadioAnimation)
		self.Tick_RadioAnimation = nil
	end
	if(self.Tick_DescriptionAnimationEnd) then
		UnRegisterTick(self.Tick_DescriptionAnimationEnd)
		self.Tick_DescriptionAnimationEnd = nil
	end
	if(self.Tick_ShowOkBtn) then
		UnRegisterTick(self.Tick_ShowOkBtn)
		self.Tick_ShowOkBtn = nil
	end
end

function CSelectCamp:Animation()
	self:ShowWnd(true)
	self:SetTransparentObj(1, false)
	for i, v in ipairs(self.m_tblCampRadio) do
		v:SetCheck(false)
		v:ShowWnd(false)
	end
	for i, v in ipairs(self.m_tblRoleIcon) do
		for j, w in ipairs(v) do
			w:ShowWnd(false)
		end
	end
	self.m_BackBtn:ShowWnd(false)
	self.m_ChooseCampLabel:ShowWnd(false)
	self.m_Description:ShowWnd(false)
	self.m_OkBtn:ShowWnd(false)
	self.m_tblBack[1]:ShowWnd(false)
	self.m_tblBack[2]:ShowWnd(false)
	self:BackBarAnimation()
end

function CSelectCamp:BackBarAnimation()
	local zoom = self:GetRootZoomSize()
	
	for i, v in ipairs(self.m_tblBack) do
		local RectEnd	= CFRect:new()
		local RectStart	= CFRect:new()
		v:GetWndRect(RectEnd)
		RectStart.top		= RectEnd.top - 20*zoom
		RectStart.bottom	= RectEnd.bottom - 20*zoom
		RectStart.left		= RectEnd.left
		RectStart.right		= RectEnd.right
		v:ShowWnd(true)
		v:SetTransparent(0.1)
		v:SetTransparentObj(350, false)
		v:SetShapeEffect(350, RectStart, RectEnd)
	end
	
	------------------------------------------------------------------------------
	local function BackBarAnimationEnd()
		g_SelectChar:MsgControl(false, false)
		self:RadioAnimation()
	end
	g_SelectChar:MsgControl(true, true)
	RegisterOnceTick(g_SelectChar.m_SelectCampWnd, "BackBarAnimationEnd", BackBarAnimationEnd, 400)
	------------------------------------------------------------------------------
end

function CSelectCamp:RadioAnimation()
	local zoom = self:GetRootZoomSize()
	
	local tblRectRadio = {{}, {}, {}}
	for i, v in ipairs(self.m_tblCampRadio) do
		local RectEnd	= CFRect:new()
		local RectStart	= CFRect:new()
		v:GetWndRect(RectEnd)
		tblRectRadio[i][1]	= RectEnd
		RectStart.top		= RectEnd.top - 20*zoom
		RectStart.bottom	= RectEnd.bottom - 20*zoom
		RectStart.left		= RectEnd.left
		RectStart.right		= RectEnd.right
		tblRectRadio[i][2]	= RectStart
	end
	
	local tbl = {3, 2, 1}
	local times = 1
	
	local function RadioAnimation()
		local nIndex = tbl[times]
		local wnd = self.m_tblCampRadio[nIndex]
		wnd:ShowWnd(true)
		wnd:EnableWnd(false)
		wnd:SetTransparent(0.1)
		wnd:SetTransparentObj(400, false)
		self.m_tblCampRadio[nIndex]:SetShapeEffect(400, tblRectRadio[nIndex][2], tblRectRadio[nIndex][1])
		if( times == #(tbl) ) then
			g_SelectChar:MsgControl(false, false)
			UnRegisterTick(self.Tick_RadioAnimation)
			self.Tick_RadioAnimation = nil
			
			------------------------------------------------------------------------------
			local function RadioAnimationEnd()
				g_SelectChar:MsgControl(false, false)
				self:ShowRoleIcon()
				for i, v in ipairs(self.m_tblCampRadio) do
					v:EnableWnd(true)
				end
			end
			g_SelectChar:MsgControl(true, true)
			RegisterOnceTick(g_SelectChar.m_SelectCampWnd, "RadioAnimationEnd", RadioAnimationEnd, 500)
			------------------------------------------------------------------------------
		end
		times = times + 1
	end
	
	------------------------------------------------------------------------------
	if(self.Tick_RadioAnimation) then
		UnRegisterTick(self.Tick_RadioAnimation)
		self.Tick_RadioAnimation = nil
	end
	g_SelectChar:MsgControl(true, true)
	self.Tick_RadioAnimation = RegisterTick("RadioAnimation", RadioAnimation, 200)
	------------------------------------------------------------------------------
end

function CSelectCamp:DescriptionAnimation()
	local nCamp = self:GetCheckCamp()
	self.m_Description:SetWndText(Lan_CampRes_Client(nCamp, "CampDescription"))
	if g_App.m_RecruitInfo[nCamp] == 1 then
		self.m_RecruitInfo:SetWndText(Lan_CampRes_Client(nCamp, "CommonDescription"))
		self.m_RecruitInfo:ShowWnd(true)
	elseif g_App.m_RecruitInfo[nCamp] == 2 then
		self.m_RecruitInfo:SetWndText(Lan_CampRes_Client(nCamp, "GoldDescription"))
		self.m_RecruitInfo:ShowWnd(true)
	else
		self.m_RecruitInfo:ShowWnd(false)
	end
	self.m_OkBtn:ShowWnd(false)
	local zoom = self:GetRootZoomSize()
	local RectEnd	= CFRect:new()
	local RectStart	= CFRect:new()
	self.m_Description:GetWndRect(RectEnd)
	RectStart.top		= RectEnd.top - 70*zoom
	RectStart.bottom	= RectEnd.bottom - 70*zoom
	RectStart.left		= RectEnd.left
	RectStart.right		= RectEnd.right
	self.m_Mask:SetWndRect(RectStart)
	self.m_Mask:SetShapeEffect(500, RectStart, RectEnd)
	self.m_Description:ShowWnd(true)
	
	------------------------------------------------------------------------------
	local function DescriptionAnimationEnd()
		UnRegisterTick(self.Tick_DescriptionAnimationEnd)
		self.Tick_DescriptionAnimationEnd = nil
		self.m_Mask:SetWndRect(RectEnd)
		
		-----------------------------------------------
		local function ShowOkBtn()
			UnRegisterTick(self.Tick_ShowOkBtn)
			self.Tick_ShowOkBtn = nil
			self.m_OkBtn:ShowWnd(true)
			self.m_OkBtn:SetTransparent(0.1)
			self.m_OkBtn:SetTransparentObj(200, false)
		end
		
		if(self.Tick_ShowOkBtn) then
			UnRegisterTick(self.Tick_ShowOkBtn)
			self.Tick_ShowOkBtn = nil
		end
		self.Tick_ShowOkBtn = RegisterTick("ShowOkBtn", ShowOkBtn, 200)
		-----------------------------------------------
	end
	
	if(self.Tick_DescriptionAnimationEnd) then
		UnRegisterTick(self.Tick_DescriptionAnimationEnd)
		self.Tick_DescriptionAnimationEnd = nil
	end
	self.Tick_DescriptionAnimationEnd = RegisterTick("DescriptionAnimationEnd", DescriptionAnimationEnd, 500)
	------------------------------------------------------------------------------
end

function CSelectCamp:BackUpAnimation(toState)
	local zoom = self:GetRootZoomSize()
	local RectEnd		= CFRect:new()
	local RectStart		= CFRect:new()
	self:GetWndRect(RectEnd)
	RectStart.top		= RectEnd.top - 20*zoom
	RectStart.bottom	= RectEnd.bottom - 20*zoom
	RectStart.left		= RectEnd.left
	RectStart.right		= RectEnd.right
	self:SetTransparentObj(600, true)
	self:SetShapeEffect(600, RectEnd, RectStart)
	
	------------------------------------------------------------------------------
	local function BackUpAnimationEnd()
		g_SelectChar:MsgControl(false, false)
		self:ShowWnd(false)
		if(toState == EGameState.eToNewRoleGameMain) then
			g_SelectChar.m_ScreenMask:ShowWnd(true)
			g_SelectChar.m_ScreenMask:SetTransparent(0.1)
			g_SelectChar.m_ScreenMask:SetTransparentObj(800, false)
			
			-----------------------------------------------------------
			local function TransparentEnd()
				g_SelectChar:MsgControl(false, false)
				local nCamp = self:GetCheckCamp()
				self:GotoSelectCharScene(nCamp)
			end
			g_SelectChar:MsgControl(true, true)
			RegisterOnceTick(g_SelectChar.m_SelectCharWnd, "TransparentEnd", TransparentEnd, 800)
			-----------------------------------------------------------
		elseif(toState == EGameState.eToLogin) then
			g_SelectChar:OnBackToLogin()
		else
			g_SelectChar.m_SelectCharWnd:Animation()
		end
	end
	g_SelectChar:MsgControl(true, true)
	RegisterOnceTick(g_SelectChar.m_SelectCampWnd, "BackUpAnimationEnd", BackUpAnimationEnd, 800)
	------------------------------------------------------------------------------
end

function CSelectCamp:GetCheckCamp()
	for i, v in ipairs(self.m_tblCampRadio) do
		if( v:GetCheck() ) then
			return i
		end
	end
end

function CSelectCamp:ShowRoleIcon()
	local tblRole = g_SelectChar.m_tblRolesInfo.m_tblPlayerListGroupByActive[1]
	local tblTemp ={0, 0, 0}
	for i, v in ipairs(tblRole) do
		local nCamp		= v.m_Camp
		tblTemp[nCamp]	= tblTemp[nCamp] + 1
		local iconWnd	= self.m_tblRoleIcon[nCamp][tblTemp[nCamp]]
		local icon		= g_DynImageMgr:GetClassMetalWithoutCampPic(nCamp, v.m_Class)
		local sToolTips	= string.format("%s %s", v.m_name, GetStaticTextClient(8322, v.m_level))
		g_DynImageMgr:AddImageByIP(iconWnd, icon, IP_ENABLE)
		iconWnd:ShowWnd(true)
		iconWnd:SetMouseOverDescAfter(sToolTips)
		iconWnd:SetTransparent(0.1)
		iconWnd:SetTransparentObj(2000, false)
	end
	self.m_ChooseCampLabel:ShowWnd(true)
	self.m_ChooseCampLabel:SetTransparent(0.1)
	self.m_ChooseCampLabel:SetTransparentObj(1000, false)
	
	self.m_BackBtn:ShowWnd(true)
	self.m_BackBtn:SetTransparent(0.1)
	self.m_BackBtn:SetTransparentObj(1000, false)
end

function CSelectCamp:GotoSelectCharScene(nCamp)
	SQRWnd_UnRegisterAccelerator( g_SelectChar.m_KeyIAccelerator )
	g_CoreSceneMgr:DestroyMainScene()
	g_SelectChar:ShowLoadingWnd()
	g_Conn.m_NewRoleCampInfo = nCamp
	local SceneName = SceneMapDifCamp_Common(nCamp, "SceneMap")
	local MapRes = Scene_Common[SceneName].MapFile
	coroutine.resume(g_App.MainRoutine, EGameState.eToNewRoleGameMain)
	g_CoreSceneMgr:CreateMainScene(MapRes)
end

function CSelectCamp.ReturnRecruitInfo(flag1, flag2, flag3)
	if not g_App.m_RecruitInfo then
		g_App.m_RecruitInfo = {}
	end
	g_App.m_RecruitInfo = {
		[1] = flag1,
		[2] = flag2,
		[3] = flag3,
	}
end

function CSelectCamp:RetCreateChild(camp, campInfo)
	local wnd = g_SelectChar.m_SelectCampWnd.m_tblRecruit[camp]
	g_SelectChar.m_SelectCampWnd.m_RecruitInfo:ShowWnd(false)
	if campInfo == 0 then 
		wnd:ShowWnd(false)
	elseif campInfo == 1 then 
		local imageId = CampRes_Client(camp, "CommonText")
		local strTex = g_ImageMgr:GetImagePath(imageId)
		g_DynImageMgr:AddImageByIP(wnd.m_Text, strTex , IP_ENABLE)
		g_DynImageMgr:AddImageByIP(wnd.m_Text, strTex , IP_DISABLE)
		wnd:ShowWnd(true)
	elseif campInfo == 2 then  
		local imageId = CampRes_Client(camp, "GoldText")
		local strTex = g_ImageMgr:GetImagePath(imageId)
		g_DynImageMgr:AddImageByIP(wnd.m_Text, strTex , IP_ENABLE)
		g_DynImageMgr:AddImageByIP(wnd.m_Text, strTex , IP_DISABLE)
		wnd:ShowWnd(true)
	end
	return wnd
end

function CSelectCamp.RetReturnRecruitInfo(flag1, flag2, flag3, returnFlag)
	if not g_App.m_RecruitInfo then
		g_App.m_RecruitInfo = {}
	end
	g_App.m_RecruitInfo = {
		[1] = flag1,
		[2] = flag2,
		[3] = flag3,
	}
	
	if returnFlag == 1 then
		g_NewRole.m_NewRoleWnd:OnBackToSelectCamp()
	else
		for camp, campInfo in pairs(g_App.m_RecruitInfo) do
			g_SelectChar.m_SelectCampWnd:RetCreateChild(camp, campInfo)
		end
		g_SelectChar.m_SelectCampWnd:Animation()
	end
end
