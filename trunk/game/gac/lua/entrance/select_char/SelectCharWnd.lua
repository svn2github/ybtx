cfg_load "entrance/CampRes_Client"

CSelectCharWnd			= class(SQRDialog)
CSelectCharWndListItem	= class(SQRDialog)

function CSelectCharWnd:Ctor()
	self:CreateFromRes("Login_SelectChar", g_SelectChar)
end

function CSelectCharWnd:OnChildCreated()
	self.m_List			= self:GetDlgChild("List")
	self.m_ShowDelRole	= self:GetDlgChild("ShowDelRole")
	self.m_Back			= self:GetDlgChild("Back")
	self.m_DelRole		= self:GetDlgChild("DelRole")
	self.m_EnterGame	= self:GetDlgChild("EnterGame")
	self.m_Module		= self:GetDlgChild("Module")
	self.m_CreateRole	= self:GetDlgChild("CreateRole")
	self.m_CampShow		= self:GetDlgChild("Camp")
	self.m_NameShow		= self:GetDlgChild("Name")
	self.m_IDShow		= self:GetDlgChild("ID")
	self.m_Class		= self:GetDlgChild("Class")
	self.m_Level		= self:GetDlgChild("Level")
	self.m_Tong			= self:GetDlgChild("Tong")
	self.m_TongPos		= self:GetDlgChild("TongPos")
	self.m_ArmyCorps	= self:GetDlgChild("ArmyCorps")
	self.m_CampColor	= self:GetDlgChild("Z001")
	self.m_CampLogo		= self:GetDlgChild("Z002")
	self.m_NewIcon		= self:CreateNewIcon()
	
	self:CreateModuleWnd()
	
	self.m_EnterGame:EnableWnd(false)
	self.m_DelRole:EnableWnd(false)
	self.m_CampColor:ShowWnd(false)
	self.m_CampLogo:ShowWnd(false)
end

function CSelectCharWnd:CreateNewIcon()
	local wnd = SQRDialog:new()
	wnd:CreateFromRes("LoginNewIcon", self.m_CreateRole)
	wnd:ShowWnd(true)
	return wnd
end

function CSelectCharWnd:CreateModuleWnd()
	self.m_ObjWnd	= CRenderSystemClient_Inst():CreateObjectDialog()
	local param		= WndCreateParam:new()
	param.x			= 0
	param.y			= 20
	param.uStyle	= 0x40000000
	param.width		= self.m_Module:GetWndOrgWidth()
	param.height	= self.m_Module:GetWndOrgHeight()
	param.font_size	= 12
	param:SetParentWnd(self.m_Module)
	self.m_ObjWnd:Create( param )
	self.m_ObjWnd:SetUseSceneDepth(true)
	self.m_ObjWnd:SetStyle(0x60000000)
--	if showHead == true then
--		self.m_ObjWnd:SetViewPortrait(true)
--	end
	
	local Flag		= IMAGE_PARAM:new()
	Flag.CtrlMask	= SM_BS_BK
	Flag.StateMask	= IP_ENABLE
	local Image		= self.m_Module:GetWndBkImage( Flag )
	self.m_ObjWnd:SetWndBkImage( Flag, Image )
end

function CSelectCharWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if(g_SelectChar.m_DisMsg) then return end
	
	if( uMsgID == BUTTON_LCLICK ) then
		if(Child == self.m_EnterGame) then
			self:IsEnterGame()
		elseif(Child == self.m_Back) then
			self:BackUpAnimation(EGameState.eToLogin)
		elseif(Child == self.m_DelRole) then
			if( self:OnDeleteRole() ) then
				local nIndex = self.m_List:GetSelectItem(-1) + 1
				local role = g_SelectChar.m_tblRolesInfo.m_tblPlayerListGroupByActive[1][nIndex]
				g_SelectChar.m_wndDelRole:OnShow(role, true)
			end
		elseif(Child == self.m_ShowDelRole) then
			g_SelectChar.m_DelRoleList:ShowWnd( not g_SelectChar.m_DelRoleList:IsShow() )
		elseif(Child == self.m_CreateRole) then
			local num = #(g_SelectChar.m_tblRolesInfo.m_tblPlayerListGroupByActive[1])
			if( num >= CSelectCharState.SelectCharConfig.nMaxRole ) then
				g_SelectChar.m_MsgBox = MessageBox( g_SelectChar, MsgBoxMsg(11005), MB_BtnOK )
				return
			end
			self:BackUpAnimation()
		end
	elseif( uMsgID == ITEM_LBUTTONCLICK ) then
		if(Child == self.m_List) then
			local nIndex = self.m_List:GetSelectItem(-1) + 1
			self:ShowCharInfo(nIndex)
		end
	end
end

function CSelectCharWnd:DestroyObj()
	self.m_ObjWnd:DestroyRenderObject(self.m_RenderObj)
	CRenderSystemClient_Inst():DestroyObjectDialog(self.m_ObjWnd)
end

function CSelectCharWnd:ShowCharInfo(nIndex)
	local role = g_SelectChar.m_tblRolesInfo.m_tblPlayerListGroupByActive[1][nIndex]
	self:ShowCampInfo(role.m_Camp)
	self:SetCharInfoStr(role)
	self:DrawRoleWnd(role)
	self.m_EnterGame:EnableWnd(true)
	self.m_DelRole:EnableWnd(true)
end

function CSelectCharWnd:ShowCampInfo(nCamp)
	local campColor	= CampRes_Client(nCamp, "BarImage")
	local campLogo	= CampRes_Client(nCamp, "LogoImage")
	local Rect		= CFRect:new()
	Rect.left		= 0
	Rect.right		= 414
	Rect.top		= 0
	Rect.bottom		= 116
	self.m_CampColor:ShowWnd(true)
	self.m_CampLogo:ShowWnd(true)
	g_DynImageMgr:AddImageNeedCut(self.m_CampColor, campColor, IP_ENABLE, Rect, CFPos:new())
	g_DynImageMgr:AddImageByIP(self.m_CampLogo, campLogo, IP_ENABLE)
end

function CSelectCharWnd:SetCharInfoStr(role)
	local sCamp		= g_DisplayCommon:GetPlayerCampForShow(role.m_Camp)
	local sClass	= g_DisplayCommon:GetPlayerClassForShow(role.m_Class)
	local sPos		= g_TongPurviewMgr:GetPosName(role.m_uTongPos)
	self.m_CampShow:SetWndText(sCamp)
	self.m_NameShow:SetWndText(role.m_name)
	self.m_IDShow:SetWndText(role.m_id)
	self.m_Class:SetWndText(sClass)
	self.m_Level:SetWndText( GetStaticTextClient(8322, role.m_level) )
	self.m_Tong:SetWndText(role.m_sTongName)
	self.m_TongPos:SetWndText(sPos or "")
	self.m_ArmyCorps:SetWndText(role.m_sArmyName)
end

function CSelectCharWnd:SelectCharByIndex(nIndex)
	local tbl = g_SelectChar.m_tblRolesInfo.m_tblPlayerListGroupByActive[1]
	if( next(tbl) ) then
		self.m_List:SelectItem(nIndex - 1)
		self:ShowCharInfo(nIndex)
	end
end

function CSelectCharWnd:IsEnterGame()
	if( not next(g_SelectChar.m_tblRolesInfo.m_tblPlayerListGroupByActive[1]) ) then
		g_SelectChar.m_MsgBox = 
			MessageBox( g_SelectChar, MsgBoxMsg(11003), MB_BtnOK )
		return
	end
	if ( true ) then
		local nIndex	= self.m_List:GetSelectItem(-1) + 1
		local nCharId	= g_SelectChar.m_tblRolesInfo.m_tblPlayerListGroupByActive[1][nIndex].m_id
		Gac2Gas:PreChangePlayerName(g_Conn, nCharId)
		g_SelectChar:MsgControl(true, true)
	else
		self:OnMouseSetting(false)
	end
end

function CSelectCharWnd:OnMouseSetting(bCreate, data) --bCreate是否是创建角色
	if(bCreate) then
		g_SelectChar.m_SelectCharWnd:OnNewRole(data)
	else
		self:OnCharEnterGame()
	end
end

function CSelectCharWnd:OnCharEnterGame()
	if g_SelectChar.RushRegisterAccountFlag == 1  then
		MessageBox(g_SelectChar, MsgBoxMsg(190004), MB_BtnOK )
		return 
	end
	self:BackUpAnimation(EGameState.eToGameMain)
end

function CSelectCharWnd:OnEnterGame()
	g_SelectChar:MsgControl(true, true)
	local nIndex	= self.m_List:GetSelectItem(-1) + 1
	local nCharId	= g_SelectChar.m_tblRolesInfo.m_tblPlayerListGroupByActive[1][nIndex].m_id
	Gac2Gas:CharEnterGame( g_Conn, 0, nCharId )
	SaveLoginFlow("Gac2Gas:CharEnterGame")
	g_MyCharIdTemp = nCharId
	g_LoginFlowStepId = 0
	local function ontick()
		g_MyCharIdTemp = nil
	end
	RegisterOnceTick(g_App, "ClearMyCharIdTick", ontick, 3*60*1000)
	
	g_CoreSceneMgr:DestroyMainScene()
	g_SelectChar:ShowLoadingWnd()
	
	--------------------------------------------------------------
	SQRWnd_UnRegisterAccelerator( g_SelectChar.m_KeyIAccelerator )
	coroutine.resume( g_App.MainRoutine, EGameState.eToGameMain )
	--------------------------------------------------------------
end

--点击删除角色按钮后的事件
function CSelectCharWnd:OnDeleteRole()
	local tbl = g_SelectChar.m_tblRolesInfo.m_tblPlayerListGroupByActive
	if( #(tbl[1]) < 1 ) then
		MessageBox( g_SelectChar, MsgBoxMsg(11003), MB_BtnOK )
		return false
	end
	if( #(tbl[2]) >= CSelectCharState.SelectCharConfig.nDelMaxRole) then
		MessageBox( g_SelectChar, MsgBoxMsg(11001), MB_BtnOK )
		return false
	end
	
	local nIndex	= self.m_List:GetSelectItem(-1) + 1
	local role		= g_SelectChar.m_tblRolesInfo.m_tblPlayerListGroupByActive[1][nIndex]
	if(not role) then
		MessageBox( g_SelectChar, MsgBoxMsg(11004), MB_BtnOK )
		return false
	end
	return true
end

function CSelectCharWnd:DrawRoleWnd( role, showHead )
	if(not role) then return end
	
	local tblCustomResID = {
	role.m_uFaceResID,
	role.m_uHairResID,
	}

	local tblResID = {
	role.m_uArmetResID,
	role.m_uBodyResID,
	role.m_uBackResID,
	role.m_uShoulderResID,
	role.m_uArmResID,
	role.m_uShoeResID,
	}

	local tblEquipPhase			= {}
	tblEquipPhase["armet"]		= role.m_uArmetPhase
	tblEquipPhase["body"]		= role.m_uBodyPhase
	tblEquipPhase["arm"]		= role.m_uArmPhase
	tblEquipPhase["shoe"]		= role.m_uShoePhase
	tblEquipPhase["shoulder"]	= role.m_uShoulderPhase
	
	if(self.m_RenderObj) then
		self.m_ObjWnd:DestroyRenderObject(self.m_RenderObj)
	end
	self.m_RenderObj = self.m_ObjWnd:CreateRenderObject()
	if role.m_Class == 9 then   --要显示的是兽人战士的模型
		self.m_RenderObj:SetScale(90 * 2 /100)
	else
		self.m_RenderObj:SetScale(role.m_Scale*2 /100)
	end
	self.m_ObjWnd:AddChild( self.m_RenderObj, eLinkType.LT_UNKNOW, "")
	self.m_RenderObj:SetAsynLoadPieceSwitch(false)
	-- 注：这应只加模型 和 类似 rlm_js 一个动作包。不要加载战斗动作包
	InitPlayerCommonAni(self.m_RenderObj, role.m_Class, role.m_sex, nil)
	InitPlayerCustomRes(self.m_RenderObj, tblCustomResID, role.m_Class, role.m_sex)
	InitPlayerModel(self.m_RenderObj, tblResID, role.m_Class, role.m_sex)
	PrepareWeapon(self.m_RenderObj, role.m_uWeaponID)
	PrepareOffWeapon(self.m_RenderObj, role.m_uOffWeaponID)
	InitWeaponRender(self.m_RenderObj, role.m_uWeaponID, role.m_Class, role.m_sex)
	InitWeaponRender(self.m_RenderObj, role.m_uOffWeaponID, role.m_Class, role.m_sex)
	InitPlayerEquipPhaseFX(self.m_RenderObj,tblEquipPhase)
	self.m_RenderObj:SetDirection( CDir:new( 128 ) )
	self.m_RenderObj:SetPosition( CVector3f:new( 0, -100, 0 ) )
	self.m_RenderObj:DoAni( "stand01_w", true, -1.0 )
end

function CSelectCharWnd:Animation()
	self:ShowWnd(true)
	self:SetTransparent(0.1)
	self:SetTransparentObj(350, false)
end

function CSelectCharWnd:BackUpAnimation(toState)
	self:SetTransparentObj(400, true)
	
	------------------------------------------------------------------------------
	local function BackUpAnimationEnd()
		g_SelectChar:MsgControl(false, false)
		self:ShowWnd(false)
		if(toState == EGameState.eToLogin) then
			g_SelectChar:OnBackToLogin()
		elseif(toState == EGameState.eToGameMain) then
			g_SelectChar.m_ScreenMask:ShowWnd(true)
			g_SelectChar.m_ScreenMask:SetTransparent(0.1)
			g_SelectChar.m_ScreenMask:SetTransparentObj(800, false)
			
			-----------------------------------------------------------
			local function TransparentEnd()
				g_SelectChar:MsgControl(false, false)
				self:OnEnterGame()
			end
			g_SelectChar:MsgControl(true, true)
			RegisterOnceTick(g_SelectChar.m_SelectCharWnd, "TransparentEnd", TransparentEnd, 800)
			-----------------------------------------------------------
		else
			Gac2Gas:GetCampInfoByNum(g_Conn,2)
			--g_SelectChar.m_SelectCampWnd:Animation()
		end
	end
	g_SelectChar:MsgControl(true, true)
	RegisterOnceTick(g_SelectChar.m_SelectCharWnd, "BackUpAnimationEnd", BackUpAnimationEnd, 400)
	------------------------------------------------------------------------------
end

function CSelectCharWnd:DrawList()
	self.m_CampColor:ShowWnd(false)
	self.m_CampLogo:ShowWnd(false)
	self.m_List:DeleteAllItem()
	self.m_tblItem = {}
	self.m_List:InsertColumn(0, self.m_List:GetWorkAreaOrgWidth())
	local tbl = g_SelectChar.m_tblRolesInfo.m_tblPlayerListGroupByActive[1]
	for i, v in ipairs(tbl) do
		self.m_List:InsertItem(i-1, 50)
		local item	= self.m_List:GetSubItem(i-1, 0)
		local wnd	= self:CreateCharListItemWnd(item)
		table.insert(self.m_tblItem, wnd)
		wnd:SetInfo(v.m_name, v.m_Class, v.m_level, v.m_Camp)
	end
end

--*****************************************************************************************
--
--*****************************************************************************************
function CSelectCharWnd:CreateCharListItemWnd(parent)
	local wnd = CSelectCharWndListItem:new()
	wnd:CreateFromRes("Login_SelectCharListItem", parent)
	wnd:ShowWnd(true)
	return wnd
end

function CSelectCharWndListItem:OnChildCreated()
	self.m_Icon		= self:GetDlgChild("Icon")
	self.m_Name		= self:GetDlgChild("Name")
	self.m_Class	= self:GetDlgChild("Class")
	self.m_Level	= self:GetDlgChild("Level")
end

function CSelectCharWndListItem:SetInfo(sName, nClass, nLevel, nCamp)
	local sClass	= g_DisplayCommon:GetPlayerClassForShow(nClass)
	local icon		= g_DynImageMgr:GetClassMetalPic(nCamp, nClass)
	g_DynImageMgr:AddImageByIP(self.m_Icon, icon, IP_ENABLE)
	self.m_Name:SetWndText(sName)
	self.m_Class:SetWndText(sClass)
	self.m_Level:SetWndText(nLevel)
end
