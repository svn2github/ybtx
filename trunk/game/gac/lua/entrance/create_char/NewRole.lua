cfg_load "player/NewRoleClassModle_Client"
lan_load "player/Lan_NewRoleClassModle_Client"
lan_load "player/Lan_NewRoleProperty_Common"
cfg_load "player/NewRoleProperty_Common"
gac_gas_require "framework/text_filter_mgr/TextFilterMgr"
gac_gas_require "setting/SettingMgr"

CNewRoleState		= class( CGameState ) 



-------------------------------NewRoleState---------------------------------------
function CNewRoleState:OnInit()
    local nWidth = g_App:GetRootWnd():GetWndWidth()
	local nHeight = g_App:GetRootWnd():GetWndHeight()
	
	local param = WndCreateParam:new()
	param.uStyle = 0x40000000
	param.width = 1024
	param.height = 768
	param:SetParentWnd(g_App:GetRootWnd())
	param.font_size = 12
	self:Create( param )
	self:SetLockFlag( BitOr( LOCK_LEFT, LOCK_RIGHT, LOCK_TOP, LOCK_BOTTOM ) )
	local Image = WND_IMAGE_LIST:new()
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP_ENABLE
	self:SetWndBkImage( Flag, Image )
	
	---------------------------------
	InitCameraX = SceneMapDifCamp_Common(g_Conn.m_NewRoleCampInfo, "InitCameraX") * 64
	InitCameraY = SceneMapDifCamp_Common(g_Conn.m_NewRoleCampInfo, "InitCameraY") * 64
	
	self.m_NewRoleWnd = CNewRoleWnd:new()
	self.m_NewRoleWnd:CreateFromRes("Login_NewRole", self)
	self.m_NewRoleWnd:ShowWnd( true )
	self.m_NewRoleWnd:InitChild()
	
	self.m_ClassIntroVideo		= CClassIntroVideo:new()
	
	self:ShowWnd(false)
end

function CNewRoleState:OnExit()
    self.m_NewRoleWnd:UnRegisterNewRoleTick()
    self.m_NewRoleWnd:DestroyWnd()
    self:DestroyWnd()
    g_NewRole = nil
end

function CNewRoleState:OnLoading(fPercent)
	if( fPercent >= 100  )then
		g_App.m_Loading:ShowWnd( false )
		g_NewRole:ShowWnd( true )
		if g_App.m_CurMusicName and g_App.m_CurMusicName ~= "" then
			StopCue(g_App.m_CurMusicName)
		end
    PlayCue("music19")
    g_App.m_CurMusicName = "music19"
		g_NewRole.m_NewRoleWnd:ShowChild( false )
		g_NewRole.m_NewRoleWnd.m_IsFallingCamera = false
		local RenderScene=g_CoreScene:GetRenderScene()
		RenderScene:SetDistSpeed(9999999999)
		RenderScene:SetXZDist(0)
		SetEvent(Event.Test.FinishedLoading, true)
	else    
		g_App.m_Loading:ShowWnd( true )
		g_NewRole:ShowWnd( false )
		g_App.m_Loading:SetPos( fPercent )
		CRenderSystemClient_Inst():Refresh()
	end
end

function CNewRoleState:OnMainSceneCreated(CoreScene, appGacHandler)
	g_CoreScene = CoreScene
	local speed = 4
	local xpos = SceneMapDifCamp_Common(g_Conn.m_NewRoleCampInfo, "InitCameraX") * 64 
	local ypos = SceneMapDifCamp_Common(g_Conn.m_NewRoleCampInfo, "InitCameraY") * 64
	
	local RenderScene=g_CoreScene:GetRenderScene()
	local newRoleCfgInfoTbl = SceneMapDifCamp_Common(g_Conn.m_NewRoleCampInfo)
	CRenderSystemClient_Inst():SetCameraNoChange(true)
	RenderScene:SetDist(newRoleCfgInfoTbl("MinDist"), newRoleCfgInfoTbl("MaxDist"))
	RenderScene:SetFov(newRoleCfgInfoTbl("MinFov"), newRoleCfgInfoTbl("MaxFov"))
	RenderScene:SetRadian(newRoleCfgInfoTbl("MinRadian"), newRoleCfgInfoTbl("MaxRadian") )
	g_CoreSceneMgr:RefreshCameraDest(speed, xpos, ypos)
	RenderScene:SetCameraOffset( newRoleCfgInfoTbl("MaxDist")*64 )
    g_NewRole.m_NewRoleWnd:CreateClassPerformer()	--创建代表不同职业的九个npc
	RenderScene:InitSceneRes()
end


function CNewRoleState:OnDestroyMainScene()
    SQRWnd_UnRegisterHook( g_NewRole.m_NewRoleWnd.m_KeyHook )
    g_NewRole.m_NewRoleWnd:UnRegisterFallCameraTick()
    g_NewRole.m_NewRoleWnd:UnRegisterRaiseCameraTick()
    CRenderSystemClient_Inst():SetCameraNoChange(false)
	CRenderSystemClient_Inst():InitCameraInfo()
    --self.m_NewRoleWnd:UnRegisterNewRoleTick()
    
    CEffectLoader_Inst():Clear()
end

function CNewRoleState:OnMainSceneDestroyed()
    g_CoreScene = nil
    if self.m_NewRoleWnd.m_RaiseCameraInGameTick == nil and --已经输完用户名，并且摄像机拉到最远处了
        g_NewRoleEnterGame.m_CurState ==  CNewRoleEnterGame.NewRoleEnterGameStateTbl["GameMain"] then
        g_NewRole:OnExit()
    end    
end

----------------------------------------------------
--双击主角模型
function CNewRoleState:OnLButtonDblClk(nFlags, x, y )
    local wnd = self.m_NewRoleWnd
	if ( g_CoreScene and 
	    g_NewRoleEnterGame.m_CurState ==  CNewRoleEnterGame.NewRoleEnterGameStateTbl["NewRole"] ) then 
        if ( wnd.m_BackToSelectClass:IsShow() and 
             wnd.m_IsFallingCamera ~= true   and 
             wnd.m_IsRaisingCamera ~= true  ) then
            wnd:IsDBLClickRenderObj()
		end
	end
end

--鼠标移动
function CNewRoleState:OnMouseMove(nFlags, x, y)
    local wnd = self.m_NewRoleWnd
	if( g_CoreScene and 
	    g_NewRoleEnterGame.m_CurState ==  CNewRoleEnterGame.NewRoleEnterGameStateTbl["NewRole"] ) then 
		if ( wnd.m_BackToSelectCamp:IsShow() and
		     wnd.m_IsFallingCamera ~= true  and
			 wnd.m_IsRaisingCamera ~= true) then
            wnd:IsMouseMoveOnRenderObj()
		end
	end
end

--左键点击
function CNewRoleState:OnLButtonClick( nFlags, x, y ) 
    local wnd = self.m_NewRoleWnd
	if ( g_CoreScene and 
	    g_NewRoleEnterGame.m_CurState ==  CNewRoleEnterGame.NewRoleEnterGameStateTbl["NewRole"] ) then 
		if ( wnd.m_BackToSelectCamp:IsShow() and
		     wnd.m_IsFallingCamera ~= true and
		     wnd.m_IsRaisingCamera ~= true ) then
			wnd:IsSelectRenderObj()
		end
	end
end

--右键点击
function CNewRoleState:OnRButtonClick( nFlags, x, y )
    local wnd = self.m_NewRoleWnd
	if ( g_CoreScene and 
	    g_NewRoleEnterGame.m_CurState ==  CNewRoleEnterGame.NewRoleEnterGameStateTbl["NewRole"] ) then
		if wnd.m_IsRaisingCamera == true or
		    wnd.m_HaveSendCreateRoleMsgToServer == true then	--摄像头在拉动过程中
			return
		end
		if wnd.m_BackToSelectClass:IsShow() then
			wnd:OnBackToSelectClass()
		end
	end
end

