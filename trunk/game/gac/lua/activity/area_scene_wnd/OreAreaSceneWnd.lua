COreSceneMSG = class (SQRDialog)
--矿洞副本

--矿藏图使用提示
function COreSceneMSG.OreMapInfoShow(SceneName, x, y)
	local str = GetStaticTextClient(1118,GetSceneDispalyName(SceneName),x,y)
	MessageBox( g_GameMain, str, MB_BtnOK)
end

function COreSceneMSG.JoinAreaSceneMsgBox(SceneId, serverId, SceneName, SceneLevel)
	local function IsJoinAreaSceneFb(wnd, btnIndex)
		if btnIndex == 1 then
			Gac2Gas:JoinOreScene(g_Conn, SceneId, serverId, SceneName)
		end
		wnd:Close()
		g_GameMain.m_ChangeOutMsgBox = nil
	end
	
	if g_GameMain.m_ChangeOutMsgBox then
		g_GameMain.m_ChangeOutMsgBox:Close()
		g_GameMain.m_ChangeOutMsgBox = nil
	end
	
	local msg = MsgBoxMsg(5002)
	msg = msg.." (%d)"
	local OkStr = GetStaticTextClient(1124)
	local CancelStr = GetStaticTextClient(1025)
	g_GameMain.m_ChangeOutMsgBox = CCountdownWnd:new(g_GameMain, msg, 20, IsJoinAreaSceneFb, {OkStr,CancelStr})
end

--是否进入副本提示(主)
function COreSceneMSG.CreateAreaSceneMsgBox(SceneName)
	local function IsCreateScene(g_GameMain,uButton)
		if uButton == MB_BtnOK then
			Gac2Gas:CreateAreaScene(g_Conn, SceneName)
		end	
		g_GameMain.m_CreateAreaSceneMsgBox = nil	
		return true
	end
	local str
--	if SceneLevel and type(SceneLevel) == "number" and SceneLevel ~= 0 then
--		str = "是否进入"..SceneLevel.."级"..SceneName.."副本"
--	else
--		str = "是否进入"..SceneName.."副本"
--	end
	str = MsgBoxMsg(5002)
--	local Wnd = CAreaFbSelWnd:new()
	g_GameMain.m_CreateAreaSceneMsgBox = MessageBox( g_GameMain, str, BitOr( MB_BtnOK, MB_BtnCancel),IsCreateScene,g_GameMain)
end

function COreSceneMSG.TeamJoinSceneMsgBox(SceneId, serverId, SceneName, SceneLevel)
	local function IsJoinScene(g_GameMain,uButton)
		if uButton == MB_BtnOK then
			Gac2Gas:TeamJoinScene(g_Conn, SceneId, serverId, SceneName, SceneLevel)
		else
			Gac2Gas:JoinOreScene(g_Conn, SceneId, serverId, SceneName)
		end
		g_GameMain.m_TeamJoinSceneMsgBox = nil
		return true
	end
	local str = MsgBoxMsg(5003)
	g_GameMain.m_TeamJoinSceneMsgBox = MessageBox( g_GameMain, str, BitOr( MB_BtnOK, MB_BtnCancel),IsJoinScene,g_GameMain)
end

function COreSceneMSG:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		if g_GameMain.m_IsExitSceneMsgBox.m_BtnOK == Child then 
			Gac2Gas:JoinOreNextScene(g_Conn)
		elseif g_GameMain.m_IsExitSceneMsgBox.m_BtnCancel == Child then
			Gac2Gas:LeaveAreaScene(g_Conn)
		end
		UnRegisterTick(g_GameMain.m_IsExitSceneMsgBox.m_CountDownTick)
		g_GameMain.m_IsExitSceneMsgBox.m_CountDownTick = nil
		g_GameMain.m_IsExitSceneMsgBox:DestroyWnd()
	end
end

function COreSceneMSG.IsExitSceneMsgBox(IsShow)
	if g_GameMain.m_IsExitSceneMsgBox and g_GameMain.m_IsExitSceneMsgBox.m_CountDownTick then
		return
	end
	local function SetWndInfo(time)
		if IsShow then
			g_GameMain.m_IsExitSceneMsgBox.m_WndInfo:SetWndText(GetStaticTextClient(6017, time))
		else
			g_GameMain.m_IsExitSceneMsgBox.m_WndInfo:SetWndText(GetStaticTextClient(6018, time))
		end
	end
	local function ExitGameCountDownTick()
		local leftTime = g_GameMain.m_IsExitSceneMsgBox.m_CountDownNum - 1
		g_GameMain.m_IsExitSceneMsgBox.m_CountDownNum = leftTime
		if leftTime > 0 then
			SetWndInfo(leftTime)
		else
			UnRegisterTick(g_GameMain.m_IsExitSceneMsgBox.m_CountDownTick)
			g_GameMain.m_IsExitSceneMsgBox.m_CountDownTick = nil
			g_GameMain.m_IsExitSceneMsgBox:DestroyWnd()
			if IsShow then
				Gac2Gas:JoinOreNextScene(g_Conn)
			else
				Gac2Gas:LeaveAreaScene(g_Conn)
			end
		end
	end	
 	g_GameMain.m_IsExitSceneMsgBox = COreSceneMSG:new()                              --退出时提示信息
	g_GameMain.m_IsExitSceneMsgBox:CreateFromRes("WndMsgChk", g_GameMain)
	g_GameMain.m_IsExitSceneMsgBox:ShowWnd( true )
	g_GameMain.m_IsExitSceneMsgBox.m_WndInfo = g_GameMain.m_IsExitSceneMsgBox:GetDlgChild("Wnd_Info")
	g_GameMain.m_IsExitSceneMsgBox.m_CountDownNum = 20
	SetWndInfo(20)
	g_GameMain.m_IsExitSceneMsgBox.m_BtnOK = g_GameMain.m_IsExitSceneMsgBox:GetDlgChild("Btn_Ok")	
	g_GameMain.m_IsExitSceneMsgBox.m_BtnOK:SetWndText(GetStaticTextClient(1005))
	g_GameMain.m_IsExitSceneMsgBox.m_BtnCancel = g_GameMain.m_IsExitSceneMsgBox:GetDlgChild("Btn_Back")	
	g_GameMain.m_IsExitSceneMsgBox.m_BtnCancel:SetWndText(GetStaticTextClient(1006))
	if not IsShow then
		g_GameMain.m_IsExitSceneMsgBox.m_BtnOK:ShowWnd(false)
		local ChildWnd = g_GameMain.m_IsExitSceneMsgBox.m_BtnCancel
		local Rect=CFRect:new()
		ChildWnd:GetWndRect(Rect)
		Rect.right = Rect.right - 120
		Rect.left	= Rect.left - 120
		ChildWnd:SetWndRect(Rect)
		ChildWnd:SetFocus()
	end
	g_GameMain.m_IsExitSceneMsgBox.m_CountDownTick = RegisterTick( "ExitSceneTick", ExitGameCountDownTick, 1000)	
--	g_GameMain.m_IsExitSceneMsgBox:ShowWnd(false)	
end

--是否邀请该玩家进入副本？需要消耗1个疾传石。
function COreSceneMSG.SendInviteJoinSceneMsgBox(CharID)
	local function IsJoinScene(g_GameMain,uButton)
		if uButton == MB_BtnOK then
			Gac2Gas:SendInviteJoinSceneMsg(g_Conn, CharID)
		end
		g_GameMain.m_JoinAreaSceneMsgBox = nil
		return true
	end
	if g_GameMain.m_JoinAreaSceneMsgBox then
		return
	end
	local str = MsgBoxMsg(5014)
	g_GameMain.m_JoinAreaSceneMsgBox = MessageBox( g_GameMain, str, BitOr( MB_BtnOK, MB_BtnCancel),IsJoinScene,g_GameMain)
end

--是否接受邀请进入副本提示
function COreSceneMSG.InviteJoinSceneMsgBox(SceneId, ServerId, SceneName, PlayerName, SceneType, OtherParam)
	local function IsJoinScene(g_GameMain,uButton)
		if uButton == MB_BtnOK then
			if SceneType == 12 then
				Gac2Gas:JoinOreScene(g_Conn, SceneId, ServerId, SceneName)
			else
				Gac2Gas:InviteChangeScene(g_Conn, SceneId, ServerId, SceneName, SceneType, OtherParam)
			end
		end
		g_GameMain.m_JoinAreaSceneMsgBox = nil
		return true
	end
	if g_GameMain.m_JoinAreaSceneMsgBox then
		return
	end
	if g_GameMain.m_ChangeOutMsgBox then
		if g_GameMain.m_ChangeOutMsgBox.m_Time ~= 0 then
			return
		end
	end
	local str = MsgBoxMsg(5013,PlayerName)
	g_GameMain.m_JoinAreaSceneMsgBox = MessageBox( g_GameMain, str, BitOr( MB_BtnOK, MB_BtnCancel),IsJoinScene,g_GameMain)
end
