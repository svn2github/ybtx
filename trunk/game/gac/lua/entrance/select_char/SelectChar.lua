gac_require "framework/main_frame/GameState"
gac_gas_require "entrance/LoginCommFunc"

--[[
2501	The Amy Empire
2502	The Holy Church
2503	The Hughes Empire
2504	Amy
2505	Holy
2506	Hughes
--]]

CSelectCharState					= class( CGameState )
CSelectCharState.SelectCharConfig	= {}

------------			select char config		---------------------------------
CSelectCharState.SelectCharConfig.nMaxRole = 5
CSelectCharState.SelectCharConfig.nDelMaxRole = 5
CSelectCharState.SelectCharConfig.strNoSelectRole = "没有角色被选中"
CSelectCharState.SelectCharConfig.strNoRole = "该帐号没有角色"
CSelectCharState.SelectCharConfig.strRoleNumReachMax = "该帐号角色数目已经到达上限"
CSelectCharState.SelectCharConfig.strUnkownError = "未知错误"
CSelectCharState.SelectCharConfig.strGetBackOK = true

------------			rpc		---------------------------------
--返回角色列表之前，先清空存储角色想关心些tbl
function CSelectCharState.ReturnCharListBegin(rushRegisterAccountFlag)
	if g_SelectChar == nil then
		return
	end
	g_SelectChar.RushRegisterAccountFlag = rushRegisterAccountFlag
	
	g_SelectChar.m_tblRolesInfo.m_tblPlayerListGroupByActive = {{}, {}}
end


function CSelectCharState.ReturnCharList( uID, Name,uHair, uHairColor, uFace, fScale, uSex, uClass, uCamp, sTongName, uTongPos, sArmyName, uLevel, uSceneID,  
								 uArmetResID, uBodyResID, uBackResID, uShoulderResID, uArmResID, uShoeResID, uWeaponID, uOffWeaponID,uArmetPhase,uBodyPhase,uArmPhase,uShoePhase,uShoulderPhase,uWeaponPhaseu,OffWeaponPhase)
	if g_SelectChar == nil then
		return
	end
	local role = {}
	role.m_id				= uID
	role.m_name				= Name
	role.m_haircolor		= uHairColor
	role.m_sex				= uSex
	role.m_Scale 			= fScale
	role.m_level			= uLevel
	role.m_sce_id			= uSceneID
	role.m_union			= "miss"
	role.m_at				= "miss"
	role.m_Class 			= uClass
	role.m_Camp				= uCamp
	role.m_sTongName		= sTongName
	role.m_uTongPos			= uTongPos
	role.m_sArmyName		= sArmyName
	role.m_uFaceResID 		= uFace
	role.m_uHairResID 		= uHair
	role.m_uArmetResID 		= uArmetResID
	role.m_uBodyResID		= uBodyResID
	role.m_uBackResID 		= uBackResID
	role.m_uShoulderResID 	= uShoulderResID
	role.m_uArmResID 		= uArmResID
	role.m_uShoeResID 		= uShoeResID
	role.m_uWeaponID 		= uWeaponID
	role.m_uOffWeaponID 	= uOffWeaponID
	role.m_uArmetPhase		= uArmetPhase
	role.m_uBodyPhase		= uBodyPhase
	role.m_uArmPhase		= uArmPhase
	role.m_uShoePhase		= uShoePhase
	role.m_uShoulderPhase	= uShoulderPhase
	role.m_uWeaponPhase		= uWeaponPhase
	role.m_uOffWeaponPhase 	= uOffWeaponPhase
	role.m_active 			= true
	
	table.insert( g_SelectChar.m_tblRolesInfo.m_tblPlayerListGroupByActive[1], role )
end

function CSelectCharState.ReturnDelCharList( uID, Name,uHair, uHairColor, uFace, fScale, uSex, uClass, uCamp,uResTime ,uLevel, uSceneID,  
								 uArmetResID, uBodyResID, uBackResID, uShoulderResID, uArmResID, uShoeResID, uWeaponID, uOffWeaponID,uArmetPhase,uBodyPhase,uArmPhase,uShoePhase,uShoulderPhase,uWeaponPhaseu,OffWeaponPhase)
	if g_SelectChar == nil then
		return
	end
	
	local role = {}
	role.m_id				= uID
	role.m_name				= Name
	role.m_haircolor		= uHairColor
	role.m_sex				= uSex
	role.m_Scale			= fScale
	role.m_level			= uLevel
	role.m_sce_id			= uSceneID
	role.m_union			= "miss"
	role.m_at				= "miss"
	role.m_Class			= uClass
	role.m_Camp				= uCamp
	role.m_uFaceResID		= uFace
	role.m_uHairResID		= uHair
	role.m_uArmetResID		= uArmetResID
	role.m_uBodyResID		= uBodyResID
	role.m_uBackResID 		= uBackResID
	role.m_uShoulderResID	= uShoulderResID
	role.m_uArmResID		= uArmResID
	role.m_uShoeResID		= uShoeResID
	role.m_uWeaponID		= uWeaponID
	role.m_uOffWeaponID 	= uOffWeaponID
	role.m_uArmetPhase		= uArmetPhase
	role.m_uBodyPhase		= uBodyPhase
	role.m_uArmPhase		= uArmPhase
	role.m_uShoePhase		= uShoePhase
	role.m_uShoulderPhase	= uShoulderPhase
	role.m_uWeaponPhase		= uWeaponPhase
	role.m_uOffWeaponPhase 	= uOffWeaponPhase
	role.m_active			= false
	
	table.insert( g_SelectChar.m_tblRolesInfo.m_tblPlayerListGroupByActive[2], role )
end


function CSelectCharState.ReturnCharListEnd()
	if g_SelectChar == nil then
		return	
	end
	SetEvent( Event.Test.CharListReceived,true)
end

function CSelectCharState.ReturnDelCharListEnd()
	
	if g_SelectChar == nil then
		SetEvent( Event.Test.DelCharListReceived, true )
		return	
	end
	
	g_SelectChar:Update()
	g_SelectChar.m_SelectCharWnd:SelectCharByIndex(1)
	g_SelectChar:MsgControl(false, false)
	SetEvent( Event.Test.DelCharListReceived, true )
end

function CSelectCharState.DelCharEnd()
	if g_SelectChar == nil then
		return
	end
	SetEvent( Event.Test.DeleteCharEnded,true)
	g_SelectChar:MsgControl(false, false)
	if(g_SelectChar.m_SelectCharWnd.m_RenderObj) then
		g_SelectChar.m_SelectCharWnd.m_ObjWnd:DestroyRenderObject(g_SelectChar.m_SelectCharWnd.m_RenderObj)
	end
	g_SelectChar:GetRolesInfoFromGas()
end

function CSelectCharState.CompleteDelCharEnd( Connection )
	if g_SelectChar == nil then
		return
	end
	SetEvent( Event.Test.DeleteCharEnded,true)
	g_SelectChar:GetRolesInfoFromGas()
end


function CSelectCharState.DelCharErr(msgID)
	if g_SelectChar == nil then
		return
	end
	g_SelectChar:MsgControl(false, false)
	MessageBox(g_SelectChar, MsgBoxMsg(msgID), MB_BtnOK ) 
end

function CSelectCharState.GetBackRoleEnd()
	if g_SelectChar == nil then
		return
	end
	
	SetEvent( Event.Test.DeleteCharEnded,true)
	
	g_SelectChar:GetRolesInfoFromGas()
	g_SelectChar:MsgControl(false, false)
end

function CSelectCharState.GetBackRoleFail(msgID)
	if g_SelectChar == nil then
		return
	end
	g_SelectChar:MsgControl(false, false)
	MessageBox(g_SelectChar, MsgBoxMsg(msgID), MB_BtnOK)
end

function CSelectCharState.ReturnNameDiff(IsOK) --当前角色名与欲恢复的角色名一样，不能恢复
	CSelectCharState.SelectCharConfig.strGetBackOK = IsOK
	if(not IsOK) then
		MessageBox(g_SelectChar, MsgBoxMsg(11009), MB_BtnOK)
	end
	g_SelectChar:MsgControl(false, false)
end

function CSelectCharState.RetServerIpInfo(Connection, sKey, Camp, sIp, iPort)
	SecectCamp = Camp
	if "" == sIp and 0 == iPort then
		CWndSelectRole.CreateScene()
	else
		NewRoleEnterOtherServer(Connection, sKey, sIp, iPort)
	end
end

function CSelectCharState.RetAccountSucc()
	CWndSelectRole.CreateScene()
end

function CSelectCharState:GetRolesInfoFromGas()
	self.m_SelectCharWnd.m_EnterGame:EnableWnd(false)
	self.m_SelectCharWnd.m_DelRole:EnableWnd(false)
	Gac2Gas:Get_Char_List( g_Conn )			--激活的角色添加到表
	Gac2Gas:Get_DeletedChar_List( g_Conn )	--已删除角色添加到表
end	

--设置当前服务器名字
--获取进入选择角色界面网络延时
--此处有Bug，引擎中得出的时间是对的，但是在导出到lua中得到的数值不对。
--因为c++中是uint64，lua中不支持
function CSelectCharState:SetServerName()
	self.object = CTimeCheckPoint:new()
	self.object:SetCheckPoint(0)
end

function CSelectCharState:Update()
	local tbl = g_SelectChar.m_tblRolesInfo.m_tblPlayerListGroupByActive
	if( next(tbl[1]) or next(tbl[2]) ) then
		self.m_SelectCharWnd:DrawList()
		self.m_DelRoleList:DrawList()
		if( not self.m_SelectCharWnd:IsShow() ) then
			self.m_SelectCharWnd:Animation()
		end
	else
		if( not self.m_SelectCampWnd:IsShow() ) then
			Gac2Gas:GetCampInfoByNum(g_Conn,2)
			--self.m_SelectCampWnd:Animation()
		end
	end
	self:SetServerName()
end

function CSelectCharState:ShowLoadingWnd()
	self:ShowWnd(false)
	if g_App.m_Loading == nil then
		g_App.m_Loading = CLoading:new(g_App:GetRootWnd())
	else
		g_App.m_Loading:ShowWnd(true)
	end
	g_App.m_Loading:SetRandomText()
	g_App.m_Loading:SetPos(1)
	CRenderSystemClient_Inst():Refresh()
	CRenderSystemClient_Inst():Refresh()
end
	
function CSelectCharState:OnInit()
	self.m_tblRolesInfo = {}
	self.m_tblRolesInfo.m_tblPlayerListGroupByActive = {{}, {}}
	
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
	
	self.m_KeyIAccelerator = CSelectRoleIAccelerator:new()
	SQRWnd_RegisterAccelerator(self.m_KeyIAccelerator, WM_KEYDOWN)
	self.m_KeyIAccelerator.GMain = self

	self:InitGUI()
	self:GetRolesInfoFromGas() ----从服务器获得信息rpc
	
	g_SelectChar:MsgControl(false, false)
	
	if(not g_CoreScene) then
		g_CoreSceneMgr:CreateMainScene("map/dljm") --载入登陆界面的场景，并播放动画
	end
	
	self:ShowWnd(true)
end

function CSelectCharState:InitGUI()
	self.m_ScreenMask		= CScreenMask:new()
	self.m_SelectCampWnd	= CSelectCamp:new()
	self.m_SelectCharWnd	= CSelectCharWnd:new()
	self.m_wndDelRole		= CWndDelRole:new()
	self.m_DelRoleList		= CDelRoleList:new()
	RegOnceTickLifecycleObj("SelectCampWnd", self.m_SelectCampWnd)
	RegOnceTickLifecycleObj("SelectCharWnd", self.m_SelectCharWnd)
end

function CSelectCharState:MsgControl(bDisAcc, bDisMsg)
	self.m_DisMsg = bDisMsg
	self.m_DisAcc = bDisAcc
end

function CSelectCharState:OnBackToLogin()
	g_SelectChar:MsgControl(true, true)
	if g_Conn and g_Conn.ShutDown then
		g_Conn:ShutDown()
	end
	coroutine.resume( g_App.MainRoutine, EGameState.eToLogin )
end

function CSelectCharState:OnMainSceneCreated(CoreScene)
	g_CoreScene = CoreScene
	g_CoreSceneMgr:RefreshCameraDest(4, 64, 64)
	local RenderScene = g_CoreScene:GetRenderScene()
	RenderScene:PlayCameraPath(0)
	RenderScene:InitSceneRes()
end

function CSelectCharState:OnDestroyMainScene()
end

function CSelectCharState:OnMainSceneDestroyed()
	g_CoreScene = nil
end

function CSelectCharState:OnExit()
	SQRWnd_UnRegisterAccelerator(self.m_KeyIAccelerator)
	UnRegisterObjOnceTick(self.m_SelectCharWnd)
	UnRegisterObjOnceTick(self.m_SelectCampWnd)
	self.m_SelectCharWnd:DestroyObj()
	self:DestroyWnd()
end

function CSelectCharState:main()
	self:OnInit()
	local re = self:WaitMessage()
	self:OnExit()
	return re
end

