CUISettingWnd = class(SQRDialog)

local fl_tblIndex = {}
fl_tblIndex["玩家名称"]				= 1
fl_tblIndex["玩家称号"]				= 2
fl_tblIndex["玩家佣兵小队"]			= 3
fl_tblIndex["玩家佣兵团"]			= 4
fl_tblIndex["其他玩家名称"]			= 5
fl_tblIndex["其他玩家称号"]			= 6
fl_tblIndex["其他玩家佣兵小队"]		= 7
fl_tblIndex["其他玩家佣兵团"]		= 8
fl_tblIndex["NPC名称"]				= 9
fl_tblIndex["怪物名称"]				= 10
fl_tblIndex["显示头盔"]				= 11
fl_tblIndex["锁定快捷栏"]			= 12
fl_tblIndex["鼠标悬停显示怪物名称"]	= 13
fl_tblIndex["友方不可选中"]			= 14

function CUISettingWnd:Ctor(parent)
	self:CreateFromRes("UISetting", parent)
end

function CUISettingWnd:OnChildCreated()
	self.m_tblCBtn = {}
	for i = 1, 14 do
		self.m_tblCBtn[i] = self:GetDlgChild("CBtn" .. i)
	end
end

function CUISettingWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_tblCBtn[1]) then				--玩家名称
			local bFlag = self.m_tblCBtn[1]:GetCheck()
			self.m_tblCBtn[2]:SetCheck(bFlag)
			self.m_tblCBtn[3]:SetCheck(bFlag)
			self.m_tblCBtn[4]:SetCheck(bFlag)
		elseif(Child == self.m_tblCBtn[2] or Child == self.m_tblCBtn[3] or Child == self.m_tblCBtn[4]) then
			if(Child:GetCheck()) then
				self.m_tblCBtn[1]:SetCheck(true)
			end
		elseif(Child == self.m_tblCBtn[5]) then	--其他玩家名称
			local bFlag = self.m_tblCBtn[5]:GetCheck()
			self.m_tblCBtn[6]:SetCheck(bFlag)
			self.m_tblCBtn[7]:SetCheck(bFlag)
			self.m_tblCBtn[8]:SetCheck(bFlag)
		elseif(Child == self.m_tblCBtn[6] or Child == self.m_tblCBtn[7] or Child == self.m_tblCBtn[8]) then
			if(Child:GetCheck()) then
				self.m_tblCBtn[5]:SetCheck(true)
			end
		elseif(Child == self.m_tblCBtn[10]) then
			local bFlag = Child:GetCheck()
			self.m_tblCBtn[13]:EnableWnd(bFlag)
		end
	end
end

--设置checkbutton信息
function CUISettingWnd:SetCBtnInfo(tbl)
	for i, v in ipairs(self.m_tblCBtn) do
		v:SetCheck(tbl[i])
	end
end

function CUISettingWnd:GetSettingInfo()
	if(not g_UISettingMgr.m_tblInfo or "" == g_UISettingMgr.m_tblInfo) then
		g_UISettingMgr.m_tblInfo = g_UISettingMgr.m_tblDefault
	end
	self:SetCBtnInfo(g_UISettingMgr.m_tblInfo)
end

--设置
function CUISettingWnd:SaveUISetting()
	g_UISettingMgr.m_tblInfo = {}
	for i, v in ipairs(self.m_tblCBtn) do
		g_UISettingMgr.m_tblInfo[i] = v:GetCheck()
	end
	g_GameMain.m_CharacterInSyncMgr:UpdateCharacterInSync()
	g_GameMain.m_CharacterInSyncMgr:PlayerHeadInfoInit()
	g_GameMain.m_CharacterInSyncMgr:UpdateFriendCanSelectedInSync()
	--	HideAllPlayerFollowerEx( self.m_yincangwanjiaCBtn:GetCheck() )
	
	Gac2Gas:SaveUISetting( g_Conn, unpack(g_UISettingMgr.m_tblInfo) )
end

--恢复默认设置
function CUISettingWnd:SetDefault()
	self:SetCBtnInfo(g_UISettingMgr.m_tblDefault)
end

--显示/隐藏界面
function IsRenderGUI()
--	local bFlag = CRenderSystemClient_Inst():GetIsRenderGUI()
--	CRenderSystemClient_Inst():SetIsRenderGUI(not bFlag)
--	if not CRenderSystemClient_Inst():GetIsRenderGUI() then
--		
--	end
end

local function MoveCameraFarOrNearTick(Tick, wParam)
	if( g_MainPlayer == nil ) then
		return false
	end
	local MouseDelta = math.floor(wParam/65536)
	if MouseDelta > 32768 then
		MouseDelta = MouseDelta - 65536
	end
	RenderScene = g_CoreScene:GetRenderScene()
	local CamOffset = RenderScene:GetCameraOffset()
	CamOffset = CamOffset - MouseDelta / 1.5
	RenderScene:SetCameraOffset( CamOffset )
end

function UnRegisterMoveCameraNearTick()
	if MoveCameraNearTick then
		UnRegisterTick(MoveCameraNearTick)
		b_IsCameraMoveNear = false
	end
end

function UnRegisterMoveCameraFarTick()
	if MoveCameraFarTick then
		UnRegisterTick(MoveCameraFarTick)
		b_IsCameraMoveFar = false
	end
end

--@brief 按Home/End键拉近/拉远镜头
--@param bFlag:true -- 拉近镜头；false -- 拉远镜头
function MoveCameraNear()
	if not b_IsCameraMoveNear then
		local wParam = 7864320
		MoveCameraFarOrNearTick(nil, wParam)
		UnRegisterMoveCameraFarTick()
		MoveCameraNearTick = RegisterTick("MoveCameraNearTick", MoveCameraFarOrNearTick, 100, wParam)
		b_IsCameraMoveNear = true
	else
		UnRegisterTick(MoveCameraNearTick)
		b_IsCameraMoveNear = false
	end
end

function MoveCameraFar()
	if not b_IsCameraMoveFar then
		local wParam = 4287102976
		MoveCameraFarOrNearTick(nil, wParam)
		UnRegisterMoveCameraNearTick()
		MoveCameraFarTick = RegisterTick("MoveCameraFarTick", MoveCameraFarOrNearTick, 100, wParam)
		b_IsCameraMoveFar = true
	else
		UnRegisterTick(MoveCameraFarTick)
		b_IsCameraMoveFar = false
	end
end


local function RoundCameraTick(Tick, wParam)
	if( g_MainPlayer == nil ) then
		return false
	end
	RenderScene=g_CoreScene:GetRenderScene()
	RenderScene:SetCameraYRotate( RenderScene:GetCameraYRotate() + wParam*0.01 )
end

function UnRegisterTurnLeftCameraTick()
	if g_GameMain.m_TurnLeftCameraTick then
		UnRegisterTick(g_GameMain.m_TurnLeftCameraTick)
		g_GameMain.TurnLeftCamera = false
	end
end

function UnRegisterTurnRightCameraTick()
	if g_GameMain.m_TurnRightCameraTick then
		UnRegisterTick(g_GameMain.m_TurnRightCameraTick)
		g_GameMain.TurnRightCamera = false
	end
end

--@按Insert键左旋转镜头
function TurnCameraLeft()
	if g_GameMain.bIsForbidFreedomCamera then
		return
	end
	if not g_GameMain.TurnLeftCamera then
		local wParam = 1
		if g_GameMain.m_TurnRightCameraTick then
			UnRegisterTick(g_GameMain.m_TurnRightCameraTick)
		end
		RoundCameraTick(nil, wParam)
		g_GameMain.m_TurnLeftCameraTick = RegisterTick("TurnLeftCameraTick", RoundCameraTick, 20, wParam)
		g_GameMain.TurnLeftCamera = true
	else
		UnRegisterTick(g_GameMain.m_TurnLeftCameraTick)
		g_GameMain.m_TurnLeftCameraTick = nil
		g_GameMain.TurnLeftCamera = false
	end
end

--@按Delete键右旋转镜头
function TurnCameraRight()
	if g_GameMain.bIsForbidFreedomCamera then
		return
	end
	if not g_GameMain.TurnRightCamera then
		local wParam = -1
		if g_GameMain.m_TurnLeftCameraTick then
			UnRegisterTick(g_GameMain.m_TurnLeftCameraTick)
		end
		RoundCameraTick(nil, wParam)
		g_GameMain.m_TurnRightCameraTick = RegisterTick("TurnRightCameraTick", RoundCameraTick, 20, wParam)
		g_GameMain.TurnRightCamera = true
	else
		UnRegisterTick(g_GameMain.m_TurnRightCameraTick)
		g_GameMain.m_TurnRightCameraTick = nil
		g_GameMain.TurnRightCamera = false
	end
end


local function RoundCameraYTick(Tick, wParam)
	if( g_MainPlayer == nil ) then
		return false
	end
	RenderScene=g_CoreScene:GetRenderScene()
	RenderScene:SetCameraXRotate( RenderScene:GetCameraXRotate() + wParam*0.01 )
end

function UnRegisterTurnTopCameraTick()
	if g_GameMain.m_TurnTopCameraTick then
		UnRegisterTick(g_GameMain.m_TurnTopCameraTick)
		g_GameMain.TurnTopCamera = false
	end
end

function UnRegisterTurnBottomCameraTick()
	if g_GameMain.m_TurnBottomCameraTick then
		UnRegisterTick(g_GameMain.m_TurnBottomCameraTick)
		g_GameMain.TurnBottomCamera = false
	end
end

--@按Insert键左旋转镜头
function TurnCameraTop()
	if g_GameMain.bIsForbidFreedomCamera then
		return
	end
	if not g_GameMain.TurnTopCamera then
		local wParam = 1
		if g_GameMain.m_TurnTopCameraTick then
			UnRegisterTick(g_GameMain.m_TurnTopCameraTick)
		end
		RoundCameraYTick(nil, wParam)
		g_GameMain.m_TurnTopCameraTick = RegisterTick("TurnTopCameraTick", RoundCameraYTick, 20, wParam)
		g_GameMain.TurnTopCamera = true
	else
		UnRegisterTick(g_GameMain.m_TurnTopCameraTick)
		g_GameMain.m_TurnTopCameraTick = nil
		g_GameMain.TurnTopCamera = false
	end
end

--@按Delete键右旋转镜头
function TurnCameraBottom()
	if g_GameMain.bIsForbidFreedomCamera then
		return
	end
	if not g_GameMain.TurnBottomCamera then
		local wParam = -1
		if g_GameMain.m_TurnBottomCameraTick then
			UnRegisterTick(g_GameMain.m_TurnBottomCameraTick)
		end
		RoundCameraYTick(nil, wParam)
		g_GameMain.m_TurnBottomCameraTick = RegisterTick("TurnBottomCameraTick", RoundCameraYTick, 20, wParam)
		g_GameMain.TurnBottomCamera = true
	else
		UnRegisterTick(g_GameMain.m_TurnBottomCameraTick)
		g_GameMain.m_TurnBottomCameraTick = nil
		g_GameMain.TurnBottomCamera = false
	end
end
function ResetCameraRotate()
	if g_GameMain and g_CoreScene then
		CRenderSystemClient_Inst():SetFreeCameraEnabled(false)
		CRenderSystemClient_Inst():SetFreeCameraEnabled(true)
	end
end