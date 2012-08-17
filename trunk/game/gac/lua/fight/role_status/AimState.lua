--[[
应该在进游戏之前就把目标属性面板创建出来
同时把创建人物渲染资源，当要显示目标属性面板时
再目标的模型信息传递过来显示
--]]

CAimStatus    = class( SQRDialog )

function CAimStatus:Ctor()
	self:CreateFromRes( "AimStateWnd", g_GameMain.m_AimPlayerManageWnd )
	self.m_nDir = 129
	InitPropertyValue(self)
	local Rt = CFRect:new()
	g_GameMain.m_AimPlayerManageWnd.m_ActionCheckBtnTbl[1]:GetLogicRect(Rt)
	Rt.top = Rt.bottom + 3
	Rt.bottom = Rt.top + self:GetWndOrgHeight()
	Rt.right = Rt.left +  self:GetWndOrgWidth()
	self:SetLogicRect(Rt)
end

function CAimStatus:OnChildCreated()
	self:OnCreateEquipWnd()
	CreatePlayerPropertyWnd(self)
	CreateGuiCommWnd(self)
	self:OnCreateRoleModel()
end

function CAimStatus:OnCreateEquipWnd()
    local initColorIconFlag = false
	OnCreateEquipWnd(self, initColorIconFlag)
end

function CAimStatus:OnCreateRoleModel()
	OnCreateRoleModel(self)
end

function CAimStatus:OnDestroy()
	OnDestroy(self)
end

--删除角色的渲染模型部分
function CAimStatus:DelWndRolePiece( EquipPart )
	DelWndRolePiece( self,EquipPart )
end

--更新角色的信息 同步角色的属性
function CAimStatus:UpdateAllRoleInfo()
	UpdateAllRoleInfo(self)
end
--绘制角色
function CAimStatus:DrawRole()
	if self.m_Target == nil or not IsCppBound(self.m_Target) then
		return
	end
	if self.m_bLoadPiece == nil then
		local tblCustomResID = {
				self.m_Target.m_Properties:GetFaceResID(),
				self.m_Target.m_Properties:GetHairResID(),
		}
		
		local tblResID = {
				g_GetResIDByEquipPart(EEquipPart.eHead,self.m_Target),
				g_GetResIDByEquipPart(EEquipPart.eWear,self.m_Target),
				g_GetResIDByEquipPart(EEquipPart.eHand,self.m_Target),
				g_GetResIDByEquipPart(EEquipPart.eShoe,self.m_Target),
				g_GetResIDByEquipPart(EEquipPart.eShoulder,self.m_Target),
				g_GetResIDByEquipPart(EEquipPart.eBack,self.m_Target) 
		}
		
		local tblEquipPhase = {}
		tblEquipPhase["armet"] = g_GetEquipPhaseByEquipPart(EEquipPart.eHead,self.m_Target)
		tblEquipPhase["body"] = g_GetEquipPhaseByEquipPart(EEquipPart.eWear,self.m_Target)
		tblEquipPhase["arm"] = g_GetEquipPhaseByEquipPart(EEquipPart.eHand,self.m_Target)
		tblEquipPhase["shoe"]= g_GetEquipPhaseByEquipPart(EEquipPart.eShoe,self.m_Target)
		tblEquipPhase["shoulder"] = g_GetEquipPhaseByEquipPart(EEquipPart.eShoulder,self.m_Target)
		
		self.m_WndBody.RenderObj:ClearAddSke()
		self.m_WndBody.RenderObj:ClearAllEfx()
		self.m_WndBody.RenderObj:RemoveAllPiece()	
		self.m_WndBody.RenderObj:Delframework()
		
		InitPlayerCommonAni(self.m_WndBody.RenderObj, self.m_Target:CppGetClass(), self.m_Target.m_Properties:GetSex(), nil)
		local WeaponItemID = self.m_Target.m_Properties:GetWeaponIndexID()
		if WeaponItemID and WeaponItemID ~= 0 then
			local itemtype,itemname = g_ItemInfoMgr:GetStaticWeaponBigID(),g_CParseWeaponTbl[WeaponItemID]
			local EquipType = g_ItemInfoMgr:GetItemInfo(itemtype,itemname,"EquipType")
			InitPlayerFinalAni(self.m_WndBody.RenderObj, self.m_Target:CppGetClass(), self.m_Target.m_Properties:GetSex(), EquipType)
		end	
		InitPlayerCustomRes(self.m_WndBody.RenderObj,tblCustomResID,self.m_Target:CppGetClass(),self.m_Target.m_Properties:GetSex())
		InitPlayerModel(self.m_WndBody.RenderObj,tblResID,self.m_Target:CppGetClass(),self.m_Target.m_Properties:GetSex())
		PrepareWeapon(self.m_WndBody.RenderObj, g_GetResIDByEquipPart(EEquipPart.eWeapon,self.m_Target))
		PrepareOffWeapon(self.m_WndBody.RenderObj, g_GetResIDByEquipPart(EEquipPart.eAssociateWeapon,self.m_Target))
		InitWeaponRender(self.m_WndBody.RenderObj, g_GetResIDByEquipPart(EEquipPart.eWeapon,self.m_Target),self.m_Target:CppGetClass(),self.m_Target.m_Properties:GetSex())
		InitWeaponRender(self.m_WndBody.RenderObj, g_GetResIDByEquipPart(EEquipPart.eAssociateWeapon,self.m_Target),self.m_Target:CppGetClass(),self.m_Target.m_Properties:GetSex())
		InitPlayerEquipPhaseFX(self.m_WndBody.RenderObj,tblEquipPhase)
		self.m_bLoadPiece = true
	end
	self.m_nHasDraw = true	
	self.m_WndBody.RenderObj:SetDirection(CDir:new(self.m_nDir))	
	if IsCppBound(self.m_Target) then
		if self.m_Target:CppGetClass() == 9 then
			self.m_WndBody.RenderObj:SetScale(1.8)
			self.m_WndBody.RenderObj:SetPosition( CVector3f:new( 0, -135, 0 ) )
		else
			local Sex = self.m_Target.m_Properties:GetSex()
			if Sex == 1 then
				self.m_WndBody.RenderObj:SetScale(2.4)
			else
				self.m_WndBody.RenderObj:SetScale(2.5)
			end
			self.m_WndBody.RenderObj:SetPosition( CVector3f:new( 0, -140, 0 ) )
		end
	end
end

function CAimStatus:SetAni()
	if self.m_Target:CppGetCtrlState(EFighterCtrlState.eFCS_InDrawWeaponMode) then
		self.m_WndBody.RenderObj:DoAni( "stand01_y", true, -1.0 )
	else
		self.m_WndBody.RenderObj:DoAni( "stand01_w", true, -1.0 )	
	end
end

--模型的左旋转
local function DrawRoleLeftTick(Tick, RoleStatus )

	if RoleStatus.m_nDir + 5 > 255 then
		RoleStatus.m_nDir = RoleStatus.m_nDir + 5 - 255
	else
		RoleStatus.m_nDir = RoleStatus.m_nDir + 5
	end
	if RoleStatus.m_BtnChangeLeft:IsHeld() then
		RoleStatus:DrawRole()
	else
		if RoleStatus.m_nHasDraw == false then
			RoleStatus:DrawRole()
		end
		UnRegisterTick(RoleStatus.m_DrawTick)
		RoleStatus.m_DrawTick = nil
	end
end

--模型的右旋转
local function DrawRoleRightTick(Tick, RoleStatus )
	if RoleStatus.m_nDir - 5 > 0 then
		RoleStatus.m_nDir = 255 - (5 - RoleStatus.m_nDir)
	else
		RoleStatus.m_nDir = RoleStatus.m_nDir - 5
	end
	if RoleStatus.m_BtnChangeRight:IsHeld() then
		RoleStatus:DrawRole()
	else
		if RoleStatus.m_nHasDraw == false then
			RoleStatus:DrawRole()
		end
		UnRegisterTick(RoleStatus.m_DrawTick)
		RoleStatus.m_DrawTick = nil
	end
end

--清除所有目标装备信息
function CAimStatus:ClearAllInfo()
	for i = 1, nAllPartNum do 
		if self.Part[i].m_Info ~= nil then		 
			g_DynItemInfoMgr:DelItemInfo(self.Part[i].m_Info:GetItemID())
			self.Part[i].m_Info = nil
			g_DelWndImage(self.Part[i].iconbtn, 0, IP_ENABLE, IP_MOUSEOVER)
			self.Part[i].iconbtn:SetMouseOverDescAfter("")
		end
	end
end

--响应鼠标事件
function CAimStatus:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_CLICKDOWN  and (self.m_BtnChangeLeft == Child or self.m_BtnChangeRight == Child) then
		if self.m_BtnChangeLeft == Child and self.m_DrawTick == nil then
			self.m_nHasDraw = false
			self.m_DrawTick = RegisterTick("DrawRoleLeftTick", DrawRoleLeftTick,33,self)
		elseif self.m_BtnChangeRight == Child and self.m_DrawTick == nil then
			self.m_nHasDraw = false
			self.m_DrawTick = RegisterTick("DrawRoleRightTick", DrawRoleRightTick,33,self)
		end
	elseif uMsgID == BUTTON_LCLICK then
		if g_GameMain.m_SysSetting.m_KPmap:KeyIsExist(VK_SHIFT) then
			DoShiftDownState(self,Child) 
		elseif self.m_XBtn == Child  then
			self:OnClosePanel()
		elseif self.m_BtnChangeLeft == Child then
			if self.m_DrawTick ~= nil then
				if self.m_nHasDraw == false then
					DrawRoleLeftTick(nil, self )
				end
			end
		elseif self.m_BtnChangeRight == Child then
			if self.m_DrawTick ~= nil then
				if self.m_nHasDraw == false then
					DrawRoleRightTick(nil, self )
				end
			end
		elseif self.m_WndBodyShowBtn == Child then
			local flag = not (self.m_PlayerPropertyWndTbl[1]:IsShow())
			for i = 1,4 do
				self.m_PlayerPropertyWndTbl[i]:ShowWnd(flag)
			end
			self.m_DituStatic:ShowWnd(flag)
		elseif(Child == self.m_PropertyChangeBtn) then
			if self.m_PlayerPropertyWndTbl[1]:IsShow() then
				self.m_PlayerPropertyWndTbl[1]:ShowWnd(false)
				self.m_PlayerPropertyWndTbl[2]:ShowWnd(true)
				self.m_PropertyChangeBtn:InserBefore(self.m_PlayerPropertyWndTbl[2])
			else
				self.m_PlayerPropertyWndTbl[1]:ShowWnd(true)
				self.m_PlayerPropertyWndTbl[2]:ShowWnd(false)
				self.m_PropertyChangeBtn:InserBefore(self.m_PlayerPropertyWndTbl[1])
			end
		elseif Child == self.m_FightingBtn then
			self:ShowWnd(false)
			g_GameMain.m_AimFightingEvaluationWnd:ShowWnd(true)
			g_GameMain.m_AimPlayerManageWnd.m_ActionCheckBtnTbl[2]:SetCheck(true)
		end
	end
end

function CAimStatus:OnClosePanel()
		if(self.m_CoreObj_ID ~= nil)then
			g_PlayerInfos[self.m_CoreObj_ID] = nil
		end
		if self.m_DrawTick ~= nil then
			UnRegisterTick(self.m_DrawTick)
			self.m_DrawTick = nil
		end
		if self.m_Target ~= nil then
			local viewinfoMgr = CFighterViewInfoMgr.GetWnd()
			viewinfoMgr:DelViewInfoByID(self.m_Target:GetEntityID())
			self.m_Target = nil
		end
end

function CAimStatus:VirtualExcludeWndClosed()
	self:OnClosePanel()
end

function CAimStatus:AddImage(Wnd,strTexture)
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP_ENABLE	
	local DefaultImage = WND_IMAGE_LIST:new()
	DefaultImage:AddImage(Wnd:GetGraphic(), -1,  strTexture, nil, CFPos:new(), "0xffffffff", 15 ) 	
	Wnd:SetWndBkImage( Flag, DefaultImage )
end

--设置目标属性面板的ToolTips和图标
function CAimStatus:UpdateAimWndTips()
	for i = 1, nAllPartNum do
		if g_GameMain.m_AimStatusWnd.Part[i].m_Info ~= nil  then
			local Item = g_GameMain.m_AimStatusWnd.Part[i].m_Info
			local nBigID,nIndex,nItemID = Item:GetBigID(),Item:GetIndex(),Item:GetItemID()
			local SmallIcon = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"SmallIcon")
			local TmpWnd = g_GameMain.m_AimStatusWnd.Part[i].iconbtn
			TmpWnd.m_IsAimStatus = true
			g_SetWndMultiToolTips(TmpWnd,nBigID,nIndex,nItemID)
			g_LoadIconFromRes(SmallIcon, TmpWnd, -2,  IP_ENABLE, IP_MOUSEOVER)
		end
	end
end

function CAimStatus:SetShowNeedInfo()
	if self.m_Target then
		local tongName = self.m_Target.m_Properties:GetTongName()
		SetShowNeedInfoCommFun(self,self.m_Target,{""},tongName)
	end
end

--RPC					--5			--Q			--1002
function CAimStatus.RetAimEquipInfo(nBigID, nIndex, nItemID, eEquipPart,nBindingType)
	local Item = CItem:new(nBigID,nIndex,nItemID,nBindingType)
	local TmpWnd = {}
	assert(IsNumber(eEquipPart))
	g_DynItemInfoMgr:CreateItemInfo(nItemID,nBigID,nIndex,nBindingType)

	for i = 1, nAllPartNum do
		if eEquipPart == g_GameMain.m_AimStatusWnd.Part[i].PartType then
			g_GameMain.m_AimStatusWnd.Part[i].m_Info = Item
			TmpWnd = g_GameMain.m_AimStatusWnd.Part[i].iconbtn	
			break
		end
	end
end

function CAimStatus.RetAimInfoEnd(CharID)
	if g_GameMain.m_AimStatusWnd.m_Target == nil or g_GameMain.m_AimStatusWnd.m_Target.m_uID ~= CharID then
		return
	end
	
	if nil == g_GameMain.m_AimStatusWnd.m_Target.m_Properties.m_StaticSyncRootVariant then
		return
	end
	
	g_GameMain.m_AimStatusWnd:UpdateAimWndTips()
	g_GameMain.m_AimStatusWnd.m_bLoadPiece = nil
	g_GameMain.m_AimStatusWnd:DrawRole()
	g_GameMain.m_AimStatusWnd:SetAni()
	g_GameMain.m_AimStatusWnd:UpdateAllRoleInfo()
	g_GameMain.m_AimStatusWnd:SetShowNeedInfo()
	
	if not g_GameMain.m_AimPlayerManageWnd:IsShow() then
		if g_GameMain.m_AimPlayerManageWnd.m_ActionCheckBtnTbl[1]:GetCheck() then
			if not g_GameMain.m_AimStatusWnd:IsShow() then
				g_GameMain.m_AimStatusWnd:ShowWnd(true)
			end
		elseif g_GameMain.m_AimPlayerManageWnd.m_ActionCheckBtnTbl[2]:GetCheck() then
			if not g_GameMain.m_AimFightingEvaluationWnd then   				
				g_GameMain.m_AimFightingEvaluationWnd:ShowWnd(true)
			end
		end
		local TotalPoint = g_GameMain.m_AimFightingEvaluationWnd.m_uTotalPoint
		g_GameMain.m_AimPlayerManageWnd.m_PlayerNameStatic:SetWndText(g_GameMain.m_AimStatusWnd.m_Target.m_Properties:GetCharName())
		g_GameMain.m_AimPlayerManageWnd.m_ActionCheckBtnTbl[3]:ShowWnd(false)
		g_GameMain.m_AimPlayerManageWnd:ShowWnd(true)
	end
end

function CAimStatus.ReturnAimPlayerSoulNum( soulValue)
	local Wnd = g_GameMain.m_AimStatusWnd
	Wnd.m_SoulValue:SetWndText(soulValue)
end
