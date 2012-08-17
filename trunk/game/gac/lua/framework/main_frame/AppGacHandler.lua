gac_require "framework/main_frame/AppGacHandlerInc"
gac_require "framework/main_frame/MainFrameInc"
gac_gas_require "reload/Reload"

function CAppGacHandler:OnBreak()
	db()
end

--根据游戏当前所处的不同状态，通过此函数CAppGacHandler回调不同的函数
function CAppGacHandler:GetExactDoingInCurGameState()
	local curStateWnd
	if(EGameState.eToLogin == g_App.m_re) then
		curStateWnd = g_Login
	elseif(EGameState.eToAgreement == g_App.m_re) then
		curStateWnd = g_UserAgreement
	elseif(EGameState.eToSelectChar == g_App.m_re) then
		curStateWnd = g_SelectChar
	elseif(EGameState.eToNewRoleGameMain == g_App.m_re) then
		curStateWnd = g_NewRoleEnterGame
	elseif(EGameState.eToGameMain == g_App.m_re) then
		curStateWnd = g_GameMain
	end 
	if(curStateWnd) then
		return curStateWnd.OnLoading, curStateWnd.OnMainSceneCreated, curStateWnd.OnDestroyMainScene, curStateWnd.OnMainSceneDestroyed
	else
		return nil
	end
end

function CAppGacHandler:OnCloseMainWnd()
	
	if (nil ~= g_Login) then
		if g_Login.m_LoginWaitingQueue then
			UnRegisterTick( g_Login.m_LoginWaitingQueue.m_LoadTime )
		end
	end
	
	if (g_GameMain) then
		
		g_GameMain:ExitToState( EGameState.eExitGame )
	
	elseif (g_Conn and g_Conn.ShutDown) then
	
		g_Conn:ShutDown()
		coroutine.resume( g_App.MainRoutine, EGameState.eExitGame )
	
	else
	
		coroutine.resume( g_App.MainRoutine, EGameState.eExitGame )
	
	end
end

				
function CAppGacHandler:OnMainSceneCreated(CoreScene)
--	print("OnMainSceneCreated")
	local _, OnMainSceneCreated = self:GetExactDoingInCurGameState()
	if OnMainSceneCreated ~= nil then
	    OnMainSceneCreated(_, CoreScene, self)
	end
end
					
function CAppGacHandler:OnDestroyMainScene()
--	print("OnDestroyMainScene~~~")
	local _, _,  DestroyMainScene = self:GetExactDoingInCurGameState()
	if DestroyMainScene ~= nil then
	    DestroyMainScene()
	end
end

function CAppGacHandler:OnMainSceneDestroyed()
--	print("OnMainSceneDestroyed.")
	SetEvent( Event.Test.SceneCreated , false )
	SetEvent( Event.Test.SceneDestroied , true )
	
	local _, _, _, OnMainSceneDestroyed = self:GetExactDoingInCurGameState()
	if OnMainSceneDestroyed ~= nil then
	    OnMainSceneDestroyed()
	else
	     g_CoreScene = nil 
	end
end

function CAppGacHandler:OnSelectPosTick()
	if( g_MainPlayer and IsCppBound(g_MainPlayer) )then
		g_MainPlayer:UpdateSelectPos()
		--g_GameMain.m_NavigationMap:SetPlayerPos()
	end
end

function CAppGacHandler:OnLoading( fPercent )
    local OnLoading = self:GetExactDoingInCurGameState()
    if OnLoading ~= nil then
        OnLoading(self, fPercent)
    end
end

function CAppGacHandler:OnLoadingStatusChanged(nIndex)
	g_App.m_Loading:SetProcessStatusInfo(nIndex)
end

function CAppGacHandler:OnFileChanged(data)
	
	--reload file
	local num = data:GetCount() 
	if data:GetIsLuaFile() then
		for i = 1, num do
			local name = data:GetFileName(i - 1)
			apcall(ReloadFile, name)
		end
	else
		for i = 1, num do
			local name = data:GetFileName(i - 1)
			ReloadSkillCfg(name)
		end
	end
	
		
end
