gac_require "framework/main_frame/PropRes"
CNewRoleEnterGame   = class ( CGameState )
CEnterGameInNewRoleHook		= class( IHook )

CNewRoleEnterGame.NewRoleEnterGameStateTbl = {}
CNewRoleEnterGame.NewRoleEnterGameStateTbl["NewRole"] = 1
CNewRoleEnterGame.NewRoleEnterGameStateTbl["GameMain"] = 2


function CNewRoleEnterGame.RetCreateRoleSucAtRushActivity()
    g_NewRole.m_NewRoleWnd:OnBackToSelectCamp()
end

function CNewRoleEnterGame.ReturnCreateRole(nCode)
    if g_NewRole == nil then
       return 
    end

	if nCode == 0 then
		SQRWnd_UnRegisterHook( g_NewRole.m_NewRoleWnd.m_KeyHook )
		g_App.m_CreateCharSuc = true
		g_NewRole.m_NewRoleWnd:EnterGameFromNewRoleState()
		SetEvent( Event.Test.CreateCharEnded, true,"Success" )
	else
		g_NewRole.m_NewRoleWnd.m_HaveSendCreateRoleMsgToServer = false
		MessageBox( g_NewRole, MsgBoxMsg(nCode), MB_BtnOK )
		SetEvent( Event.Test.CreateCharEnded, true,"Failed" )
	end
end

local CreateRoleGuideModel = 12

function CEnterGameInNewRoleHook:OnHook(Wnd, Msg, wParam, lParam)
	if wParam == VK_RETURN then
	    if g_NewRole.m_NewRoleWnd.m_nClass == CreateRoleGuideModel then
            return  
	    end
		if g_NewRole.m_NewRoleWnd.m_HaveSendCreateRoleMsgToServer ~= true then --如果已经向服务器发送过createRole的请求，就返回
			local roleName = g_NewRole.m_NewRoleWnd.m_editName:GetWndText()
			local textFilter = CTextFilterMgr:new()
			local bType,strRet = textFilter:CheckRoleName(roleName)
			if bType == false then
				g_NewRole.m_NewRoleWnd.m_HaveSendCreateRoleMsgToServer = false
				MessageBox( g_NewRole, MsgBoxMsg(strRet), MB_BtnOK, CallBack)
			else
			    g_NewRole.m_NewRoleWnd.m_HaveSendCreateRoleMsgToServer = true
				local campInfo = g_Conn.m_NewRoleCampInfo
				local wnd = g_NewRole.m_NewRoleWnd
				Gac2Gas:CreateRole( g_Conn, roleName, wnd.m_nHair, wnd.m_nHairColor,
                wnd.m_nFace, wnd.m_nScale, wnd.m_nSex, wnd.m_nClass, campInfo,
                wnd.m_xPos, wnd.m_yPos )
			end
		end
	end
end

function CNewRoleEnterGame:main()
	g_GameMain = CGameMain:new()
	RegOnceTickLifecycleObj("g_GameMain", g_GameMain)
	g_GameMain:OnInit() --加载GameMain的资源
	g_NewRole = CNewRoleState:new()
	g_NewRole:OnInit()  --加载newRole的资源
	self.m_CurState = CNewRoleEnterGame.NewRoleEnterGameStateTbl["NewRole"]
	re = self:WaitMessage() 
	self:OnExit()
	return re	
end

function CNewRoleEnterGame:OnLoading(fPercent)
    if g_NewRoleEnterGame.m_CurState ==  CNewRoleEnterGame.NewRoleEnterGameStateTbl["NewRole"] then
        g_NewRole:OnLoading(fPercent)
    elseif g_NewRoleEnterGame.m_CurState ==  CNewRoleEnterGame.NewRoleEnterGameStateTbl["GameMain"] then
        g_GameMain:OnLoading(fPercent)
    end
end

function CNewRoleEnterGame:OnDestroyMainScene()
    if g_NewRoleEnterGame.m_CurState ==  CNewRoleEnterGame.NewRoleEnterGameStateTbl["NewRole"]
        or g_GameMain.m_DestoryedNewRoleOrGameMainScene == nil then
        g_GameMain.m_DestoryedNewRoleOrGameMainScene = 1
        g_NewRole:OnDestroyMainScene()
    elseif g_NewRoleEnterGame.m_CurState ==  CNewRoleEnterGame.NewRoleEnterGameStateTbl["GameMain"] and
        g_GameMain.m_DestoryedNewRoleOrGameMainScene == 1 then
        if g_NewRole and g_NewRole.m_NewRoleWnd then
            g_NewRole.m_NewRoleWnd:UnRegisterEnterGameCameraTick()
        end
        g_GameMain.m_DestoryedNewRoleOrGameMainScene = 2
        g_GameMain:OnDestroyMainScene()
    end
end

function CNewRoleEnterGame:OnMainSceneCreated(CoreScene, appGacHandler)
    if g_NewRoleEnterGame.m_CurState ==  CNewRoleEnterGame.NewRoleEnterGameStateTbl["NewRole"] then
        g_NewRole:OnMainSceneCreated(CoreScene, appGacHandler)
    elseif g_NewRoleEnterGame.m_CurState ==  CNewRoleEnterGame.NewRoleEnterGameStateTbl["GameMain"] then
        g_GameMain:OnMainSceneCreated(CoreScene, appGacHandler)
    end
end

function CNewRoleEnterGame:OnMainSceneDestroyed()
    if g_NewRoleEnterGame.m_CurState ==  CNewRoleEnterGame.NewRoleEnterGameStateTbl["NewRole"] or
        g_GameMain.m_DestoryedNewRoleOrGameMainScene == 1 then
        g_NewRole.m_HaveDestroyedNewRoleMainScene = true
        g_NewRole:OnMainSceneDestroyed()
    elseif g_NewRoleEnterGame.m_CurState ==  CNewRoleEnterGame.NewRoleEnterGameStateTbl["GameMain"]
        and g_GameMain.m_DestoryedNewRoleOrGameMainScene == 2 then
        g_GameMain:OnMainSceneDestroyed()
        g_GameMain.m_DestoryedNewRoleOrGameMainScene = 1
    end
end

function CNewRoleEnterGame:OnExit()
    if g_NewRole then
       g_NewRole:OnExit()    
    end
    g_GameMain:OnExit()
    self:DestroyWnd()
end
