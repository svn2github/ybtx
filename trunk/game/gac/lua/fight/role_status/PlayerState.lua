gac_gas_require "item/Equip/EquipCommonFunc"
cfg_load "equip/MoneyConsumedFixingEquip_Common"

CPlayerStatus = class( SQRDialog )
CWndBagUpIcon	= class( SQRDialog )

function CPlayerStatus:Ctor()
	self:Init(g_GameMain.m_PlayerManageWnd)
end

function CPlayerStatus:Init(parent)
	self.m_UpSpace			= 0   --炼化框位置:0属性面板,1包裹
	self.m_fromBagItemWnd	= nil --记录被强化装备的来源窗体
	self.m_nDir				= 128
	self.m_PieceSkillNameToNumberTbl = {}
	self:CreateFromRes("PlayerStateWnd", parent)
	self.m_UpSpaceIconWnd	= self.m_UpIconWnd --炼化框图标窗体
	InitPropertyValue(self)
	local Rt = CFRect:new()
	parent.m_ActionCheckBtnTbl[1]:GetLogicRect(Rt)
	Rt.top = Rt.bottom + 3
	Rt.bottom = Rt.top + self:GetWndOrgHeight()
	Rt.right = Rt.left +  self:GetWndOrgWidth()
	self:SetLogicRect(Rt)
end

function CPlayerStatus:OnChildCreated()
	self.m_UpBoxLable			= self:GetDlgChild("UpBoxLable")
	self.m_UpBtn				= self:GetDlgChild("UpBtn")
	self.m_UpIconWnd 			= self:CreateUpIconWnd(self.m_UpBtn)
	self.m_UpBtn:ShowWnd(false)
	g_WndMouse:ClearIconWnd(self.m_UpIconWnd)
	self:OnCreateEquipWnd(self)
	CreatePlayerPropertyWnd(self)
	CreateGuiCommWnd(self)
	self:OnCreateRoleModel()
end

--创建强化框小图标窗体
function CPlayerStatus:CreateUpIconWnd(parent)
	local wnd = CWndBagUpIcon:new()
	wnd:CreateFromRes("CommonGrid", parent)
	wnd:ShowWnd( true )
	return wnd
end

function CPlayerStatus:OnCreateEquipWnd()
    local initColorIconFlag = true
	OnCreateEquipWnd(self, initColorIconFlag)
end

function CPlayerStatus:OnCreateRoleModel()
	OnCreateRoleModel(self)
end

function CPlayerStatus:OnDestroy()
	OnDestroy(self)
end

--删除角色的渲染模型部分
function CPlayerStatus:DelWndRolePiece( EquipPart )
	DelWndRolePiece( self,EquipPart )
end

--更新角色的信息 同步角色的属性
function CPlayerStatus:UpdateAllRoleInfo()
	if self:IsShow() then
		UpdateAllRoleInfo(self)
	end
end

--绘制角色
function CPlayerStatus:DrawRole()
	if not self:IsShow() then
		return
	end
	if self.m_bLoadPiece == nil then
		local tblCustomResID = {
				g_MainPlayer.m_Properties:GetFaceResID(),
				g_MainPlayer.m_Properties:GetHairResID(),
		}
		
		local tblResID = {
				g_GetResIDByEquipPart(EEquipPart.eHead,g_MainPlayer),
				g_GetResIDByEquipPart(EEquipPart.eWear,g_MainPlayer),
				g_GetResIDByEquipPart(EEquipPart.eHand,g_MainPlayer),
				g_GetResIDByEquipPart(EEquipPart.eShoe,g_MainPlayer),
				g_GetResIDByEquipPart(EEquipPart.eShoulder,g_MainPlayer),
				g_GetResIDByEquipPart(EEquipPart.eBack,g_MainPlayer)
		}
		
		local tblEquipPhase = {}
		tblEquipPhase["armet"] = g_GetEquipPhaseByEquipPart(EEquipPart.eHead,g_MainPlayer)
		tblEquipPhase["body"]= g_GetEquipPhaseByEquipPart(EEquipPart.eWear,g_MainPlayer)
		tblEquipPhase["arm"] = g_GetEquipPhaseByEquipPart(EEquipPart.eHand,g_MainPlayer)
		tblEquipPhase["shoe"] = g_GetEquipPhaseByEquipPart(EEquipPart.eShoe,g_MainPlayer)
		tblEquipPhase["shoulder"] = g_GetEquipPhaseByEquipPart(EEquipPart.eShoulder,g_MainPlayer)

		self.m_WndBody.RenderObj:ClearAddSke()
		self.m_WndBody.RenderObj:ClearAllEfx()
		self.m_WndBody.RenderObj:RemoveAllPiece()

		InitPlayerCommonAni(self.m_WndBody.RenderObj, g_MainPlayer:CppGetClass(), g_MainPlayer.m_Properties:GetSex(), nil)
		local WeaponItemID = g_MainPlayer.m_Properties:GetWeaponIndexID()
		if WeaponItemID and WeaponItemID ~= 0 then
			local itemtype,itemname = g_ItemInfoMgr:GetStaticWeaponBigID(),g_CParseWeaponTbl[WeaponItemID]
			local EquipType = g_ItemInfoMgr:GetItemInfo(itemtype,itemname,"EquipType")
			InitPlayerFinalAni(self.m_WndBody.RenderObj, g_MainPlayer:CppGetClass(), g_MainPlayer.m_Properties:GetSex(), EquipType)
		end
		InitPlayerCustomRes(self.m_WndBody.RenderObj,tblCustomResID,g_MainPlayer:CppGetClass(),g_MainPlayer.m_Properties:GetSex())
		InitPlayerModel(self.m_WndBody.RenderObj,tblResID,g_MainPlayer:CppGetClass(),g_MainPlayer.m_Properties:GetSex())
		PrepareWeapon(self.m_WndBody.RenderObj, g_GetResIDByEquipPart(EEquipPart.eWeapon,g_MainPlayer))
		PrepareOffWeapon(self.m_WndBody.RenderObj, g_GetResIDByEquipPart(EEquipPart.eAssociateWeapon,g_MainPlayer))
		InitWeaponRender(self.m_WndBody.RenderObj, g_GetResIDByEquipPart(EEquipPart.eWeapon,g_MainPlayer),g_MainPlayer:CppGetClass(),g_MainPlayer.m_Properties:GetSex())
		InitWeaponRender(self.m_WndBody.RenderObj, g_GetResIDByEquipPart(EEquipPart.eAssociateWeapon,g_MainPlayer),g_MainPlayer:CppGetClass(),g_MainPlayer.m_Properties:GetSex())
		InitPlayerEquipPhaseFX(self.m_WndBody.RenderObj,tblEquipPhase)
		self.m_bLoadPiece = true
	end
	self.m_nHasDraw = true
	self.m_WndBody.RenderObj:SetDirection(CDir:new(self.m_nDir))
	if g_MainPlayer:CppGetClass() == 9 then
		self.m_WndBody.RenderObj:SetScale(1.8)
		self.m_WndBody.RenderObj:SetPosition( CVector3f:new( 0, -135, 0 ) )
	else
		local Sex = g_MainPlayer.m_Properties:GetSex()
		if Sex == 1 then
			self.m_WndBody.RenderObj:SetScale(2.4)
		else
			self.m_WndBody.RenderObj:SetScale(2.5)
		end
		self.m_WndBody.RenderObj:SetPosition( CVector3f:new( 0, -140, 0 ) )
	end
end
  

--模型的左旋转
local function DrawRoleLeftTick(Tick, RoleStatus )
	local dir = RoleStatus.m_nDir - 5
	RoleStatus.m_nDir = (dir > 0 and 255 or 0) + dir
	RoleStatus:OnDrawRole(RoleStatus.m_BtnChangeLeft)
end

--模型的右旋转
local function DrawRoleRightTick(Tick, RoleStatus )
	local dir = RoleStatus.m_nDir + 5
	RoleStatus.m_nDir = (dir > 255 and -255 or 0) + dir
	RoleStatus:OnDrawRole(RoleStatus.m_BtnChangeRight)
end

function CPlayerStatus:OnDrawRole(btnChange)
	if(btnChange:IsHeld()) then
		self:DrawRole()
	else
		if(self.m_nHasDraw == false) then
			self:DrawRole()
		end
		UnRegisterTick(self.m_DrawTick)
		self.m_DrawTick = nil
	end
end

function CPlayerStatus:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_CLICKDOWN then
		if Child == self.m_BtnChangeLeft and self.m_DrawTick == nil then
			self.m_nHasDraw = false
			self.m_DrawTick = RegisterTick("DrawRoleLeftTick", DrawRoleLeftTick,33,self)
		elseif Child == self.m_BtnChangeRight and self.m_DrawTick == nil then
			self.m_nHasDraw = false
			self.m_DrawTick = RegisterTick("DrawRoleRightTick", DrawRoleRightTick,33,self)
		end	
	elseif uMsgID == BUTTON_DRAG then
		self:ClickOrDragBtn(Child,false)
	elseif uMsgID == BUTTON_LCLICK then
		if g_GameMain.m_SysSetting.m_KPmap:KeyIsExist(VK_SHIFT) then
			DoShiftDownState(self,Child)
		elseif(Child == self.m_XBtn) then
			self:ShowPanel(false)
			local state = g_WndMouse:GetCursorState()
			if state == ECursorState.eCS_RenewEquip then
				g_WndMouse:ClearCursorSpecialState()
			end
		elseif(Child == self.m_BtnChangeLeft) then
			if self.m_DrawTick ~= nil then
				if self.m_nHasDraw == false then
					DrawRoleLeftTick(nil, self )
				end
			end
		elseif(Child == self.m_BtnChangeRight) then
			if self.m_DrawTick ~= nil then
				if self.m_nHasDraw == false then
					DrawRoleRightTick(nil, self )
				end
			end
		elseif(Child == self.m_WndBodyShowBtn) then			
			local flag = not (self.m_PlayerPropertyWndTbl[1]:IsShow())
			for i = 1,4 do
				self.m_PlayerPropertyWndTbl[i]:ShowWnd(flag)
			end
			self.m_DituStatic:ShowWnd(flag)
		elseif(Child == self.m_UpBtn) then
			self:OnUpEquip(0)
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
			g_GameMain.m_FightingEvaluationWnd:ShowWnd(true)
			g_GameMain.m_PlayerManageWnd.m_ActionCheckBtnTbl[2]:SetCheck(true)		
		else
			self:ClickOrDragBtn(Child, true)
		end
	elseif(uMsgID == BUTTON_CLICKUP) then
		if(Child == self.m_UpBtn) then
			self:OnUpEquip(0)
		else
			self:ClickOrDragBtn(Child, true)
		end
	elseif uMsgID == BUTTON_RIGHTCLICKDOWN then
		if(not g_MainPlayer ) then
			local nLoading = g_App.m_Loading:IsShow() and 1 or 0
			local str = string.format("是否Loading:%d,游戏状态:%d", nLoading, g_App.m_re)
			LogErr("逻辑组:g_MainPlay不存在(属性面板)", str)
			return
		end
		if g_MainPlayer:IsInBattleState() then
			g_WndMouse:ClearCursorAll()
			MsgClient(1024)
			return
		end
		if g_MainPlayer.m_ItemBagLock then
			MsgClient(160014)
			return
		end		
		self:RClickFuc(Child)
	end
end

function CPlayerStatus:OnUpEquip(upSpace) --upSpace升级强化位置:0是属性面板中, 1是背包中 ,2是显示图片
	local state, context = g_WndMouse:GetCursorState()

	local tblInfo = {}
	if( state == ECursorState.eCS_PickupItem ) then --包裹里的物品
		local roomIndex, row, col, num, iconWnd, bagListItemWnd = unpack(context)
		local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc(roomIndex)
		local grid  = SrcCt:GetGridByIndex(row, col, roomIndex)
		local itemType, itemName, bindingType = grid:GetType()
		if( g_ItemInfoMgr:IsStaticEquip(itemType) ) then --如果是装备
			
			local isCanBeUp = self:IsCanBeUp(itemType, itemName)
			if(not isCanBeUp) then 
				g_WndMouse:ClearCursorAll()
				return MsgClient(1027) 
			end
			
			self:ClearUpIconWnd()
			self.m_UpSpace = upSpace
			if 0 == upSpace then
				self.m_UpSpaceIconWnd = self.m_UpIconWnd
			elseif 1 == upSpace then
				self.m_UpSpaceIconWnd = g_GameMain.m_WndMainBag.m_UpIconWnd
			elseif 2 == upSpace then
				self.m_UpSpaceIconWnd = g_GameMain.m_EquipUpIntensifyWnd.m_EquipIcon
			end
			
			local itmeId = grid:GetItem():GetItemID()
			local item = CItem:new(itemType, itemName, itmeId, bindingType)
			tblInfo.m_Info        = item
			tblInfo.PartType      = 0
			tblInfo.iconbtn       = bagListItemWnd
			self.iconWnd = iconWnd
			self.m_fromBagItemWnd = bagListItemWnd
			self.m_fromBagItemWnd:EnableWnd(false)
		else
			return MsgClient(1026)
		end
		self:OnUpEquipAfter(tblInfo)
	elseif( state == ECursorState.eCS_PickupEquip ) then --身上的装备
		tblInfo.m_Info, tblInfo.PartType, tblInfo.iconbtn = unpack(context)
		local itemType, itemName = tblInfo.m_Info:GetBigID(), tblInfo.m_Info:GetIndex()
		
		local isCanBeUp = self:IsCanBeUp(itemType, itemName)
		if(not isCanBeUp) then 
			g_WndMouse:ClearCursorAll()
			return MsgClient(1027) 
		end
		
		self:ClearUpIconWnd()
		self.m_UpSpace = upSpace
		if 0 == upSpace then
			self.m_UpSpaceIconWnd = self.m_UpIconWnd
		elseif 1 == upSpace then
			self.m_UpSpaceIconWnd =  g_GameMain.m_WndMainBag.m_UpIconWnd
		elseif 2 == upSpace then
			self.m_UpSpaceIconWnd =  g_GameMain.m_EquipUpIntensifyWnd.m_EquipIcon
		end
		self:OnUpEquipAfter(tblInfo)
	elseif (state == ECursorState.eCS_Normal ) then 
		g_GameMain.m_EquipUpIntensifyWnd:OpenNoInitWnd()	
	end
end

function CPlayerStatus:IsCanBeUp(itemType, itemName)
	local AdvancedQuality = g_ItemInfoMgr:GetItemInfo(itemType, itemName,"AdvancedQuality")
	if tonumber(AdvancedQuality) > 0 then
		return true
	else
		return false
	end
end

function CPlayerStatus:OnUpEquipAfter(tblInfo)
	g_GameMain.m_EquipUpIntensifyWnd:InitData(tblInfo)
	self:ReSetUpBoxIcon(tblInfo.m_Info)
	g_WndMouse:ClearCursorAll()
end

function CPlayerStatus:ReSetUpBoxIcon(info) --self.m_UpSpace升级强化位置:0是属性面板中, 1是背包中
	local nBigID, nIndex, nItemID = info:GetBigID(), info:GetIndex(), info:GetItemID()
	local SmallIcon	= g_ItemInfoMgr:GetItemInfo(nBigID, nIndex,"SmallIcon")
	g_LoadIconFromRes(SmallIcon, self.m_UpSpaceIconWnd, -1, IP_ENABLE, IP_ENABLE)
end

function CPlayerStatus:ClickOrDragBtn(Child,BCilk)
	for i = 1, nAllPartNum do
		if(Child == self.Part[i].iconbtn)then
			if BCilk then 
				self:EquipWndClicked(self.Part[i].iconbtn,self.Part[i].m_Info,self.Part[i].PartType)
			else
				local State,Context = g_WndMouse:GetCursorState()
				if State == ECursorState.eCS_Normal then
					if self.Part[i].m_Info ~= nil then
						g_WndMouse:SetPickupEquip(self.Part[i].m_Info,self.Part[i].PartType,self.Part[i].iconbtn)
					end
				end
			end
			break
		end
	end
end

function CPlayerStatus:RClickFuc(Child)
	for i = 1, nAllPartNum do
		if(Child == self.Part[i].iconbtn)then
			local EquipInfo = self.Part[i].m_Info
			local State,Context = g_WndMouse:GetCursorState()
			if State == ECursorState.eCS_Normal then --手上没有东西
				if( not EquipInfo ) then return end
				local eEquipPart = self.Part[i].PartType
				Gac2Gas:RClickFetchEquipByPart(g_Conn, eEquipPart)
			end
			break
		end
	end
end

function CPlayerStatus:CheckMoneyEnoughForFixingEquip(equipBaseLevel, eEquipPart, equipID)
    local needMoney = MoneyConsumedFixingEquip_Common(equipBaseLevel,"MoneyConsumed")
	local function callback(Context,Button)
		Context.m_MsgBox = nil
		return true
	end
	if needMoney > g_MainPlayer.m_nMoney + g_MainPlayer.m_nBindMoney then
		self.m_MsgBox = MessageBox(g_GameMain, MsgBoxMsg(1018), MB_BtnOK,callback,self)
	else
		Gac2Gas:RenewEquipDuraValue(g_Conn, eEquipPart, equipID, choosedPayType)
	end
		
end

function CPlayerStatus:RenewClickedEquip(EquipInfo, eEquipPart, Context)
    local equipID   =   EquipInfo:GetItemID()
    if eEquipPart ~= nil and equipID ~= nil then
        local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(equipID)
        if tonumber(DynInfo.CurDuraValue) == tonumber(DynInfo.MaxDuraValue) then 
            MessageBox(self, MsgBoxMsg(1021), MB_BtnOK, callback) 
	        return 
        end
        local needMoney = MoneyConsumedFixingEquip_Common(DynInfo.BaseLevel,"MoneyConsumed")
        local c_money = CMoney:new()
        local moneyNeedToFix = c_money:ChangeMoneyToString(needMoney)
        local function callback(Context,Button)
            if Button == MB_BtnOK then
                if g_MainPlayer.m_ItemBagLock then
                    MsgClient(160045)
                    return
                end
                self:CheckMoneyEnoughForFixingEquip(DynInfo.BaseLevel, eEquipPart, equipID)
            end
            return true
        end
        MessageBox(self, MsgBoxMsg(1019, moneyNeedToFix), BitOr( MB_BtnOK, MB_BtnCancel), callback)           
    end

end

function CPlayerStatus:UseEquipIntenBackItem(bigID, index, equipID, nRoomIndex, curPos, eEquipPart, Context)
	--判断是否处于战斗状态下。
	if g_MainPlayer:IsInBattleState() then
		MsgClient(171007)
		return
	end
	--判断是否处于缴械状态下。
	if g_MainPlayer:IsInForbitUseWeaponState() then
		MsgClient(171008)
		return
	end
	if g_ItemInfoMgr:IsStaticEquip(bigID) then --判断是不是装备
	    local intenBackItemType, intenBackItemName, roomIndex, nPos = Context[2], Context[3], Context[4], Context[5]
        Gac2Gas:UseEquipIntenBackItem(g_Conn, equipID, eEquipPart, nRoomIndex, curPos, roomIndex, nPos, intenBackItemType, intenBackItemName )
	end
	g_WndMouse:ClearCursorSpecialState()
end

--响应点击装备按钮
function CPlayerStatus:EquipWndClicked( EquipWnd, EquipInfo, eEquipPart)
	local State,Context = g_WndMouse:GetCursorState()
	if State == ECursorState.eCS_Normal then --手上没有东西
		if( not EquipInfo ) then return end
		g_WndMouse:SetPickupEquip(EquipInfo, eEquipPart, EquipWnd)
	elseif State == ECursorState.eCS_PickupEquip then --手上是身上的装备
		self:ExChangeEquip(Context, EquipInfo, EquipWnd, eEquipPart)
	elseif State == ECursorState.eCS_PickupItem then --手上是从包裹来的装备
		self:ButOnFromBag(Context, EquipWnd, eEquipPart, EquipInfo)
	elseif State == ECursorState.eCS_EquipIdentify then --鉴定装备
		if( not EquipInfo ) then return end
		self:OnEquipIdentify(EquipInfo,eEquipPart, Context)
	elseif State == ECursorState.eCS_ArmorPieceEnactment  then  --护甲片设定
		if( not EquipInfo ) then return end
		self:OnArmorPieceEnactment(EquipInfo, Context)
	elseif State == ECursorState.eCS_EquipSmeltSoul then --装备炼化魂
		if( not EquipInfo ) then return end
		self:OnEquipSmeltSoul(EquipInfo, Context,eEquipPart)
    elseif State == ECursorState.eCS_RenewEquip then --修理装备
        if EquipInfo == nil then return end
        self:RenewClickedEquip(EquipInfo, eEquipPart, Context)
    elseif State == ECursorState.eCS_EquipIntenBack then
        if EquipInfo == nil then return end
        local bigID, index, itemid = EquipInfo:GetBigID(), EquipInfo:GetIndex(),EquipInfo:GetItemID()
        self:UseEquipIntenBackItem( bigID, index, itemid , 0, 0,eEquipPart, Context)
	end
end

--护甲片设定
function CPlayerStatus:OnArmorPieceEnactment(EquipInfo, Context)
	--判断是否处于战斗状态下。
	if g_MainPlayer:IsInBattleState() then
		MsgClient(171007)
		return
	end
	--判断是否处于缴械状态下。
	if g_MainPlayer:IsInForbitUseWeaponState() then
		MsgClient(171008)
		return
	end
	local bigID, index, itemid = EquipInfo:GetBigID(), EquipInfo:GetIndex(),EquipInfo:GetItemID()
	if g_ItemInfoMgr:IsStaticEquip(bigID) then --判断是不是静态装备
		local tbl = {bigID,index,itemid,Context,0,0}
		g_WndMouse:SetCursorState(ECursorState.eCS_ArmorPieceEnactment,tbl)
		g_GameMain.m_EquipEnactmentAttrWnd:ArmorPieceEnactment()
	end
	g_WndMouse:ClearCursorSpecialState()
end

--鉴定装备
function CPlayerStatus:OnEquipIdentify(EquipInfo,eEquipPart, Context)
	--判断是否处于战斗状态下。
	if g_MainPlayer:IsInBattleState() then
		MsgClient(170009)
		return
	end
	--判断是否处于缴械状态下。
	if g_MainPlayer:IsInForbitUseWeaponState() then
		MsgClient(170010)
		return
	end
	local bigID, index, itemid = EquipInfo:GetBigID(), EquipInfo:GetIndex(),EquipInfo:GetItemID()
	local IsStatic = g_ItemInfoMgr:GetItemInfo(bigID,index,"IsStatic")
	if IsStatic == 1 then
			MsgClient(170008)
			return
	end
	if g_ItemInfoMgr:IsStaticEquip(bigID) then
		if g_ItemInfoMgr:IsArmor(bigID) or g_ItemInfoMgr:IsStaticJewelry(bigID) then --判断是不是防具或者饰品
			GacEquipIdentify.ArmorOrJewelryIdentify(bigID,index,itemid,Context)
		elseif g_ItemInfoMgr:IsWeapon(bigID) then --判断是不是武器
			GacEquipIdentify.WeaponIdentify(bigID,index,itemid,Context,0,0,eEquipPart)
		else
			MsgClient(170000)
		end
	end
	g_WndMouse:ClearCursorSpecialState()
end

--装备炼化魂
function CPlayerStatus:OnEquipSmeltSoul(EquipInfo, Context,eEquipPart)
	--判断是否处于战斗状态下。
	if g_MainPlayer:IsInBattleState() then
		MsgClient(194004)
		return
	end
	--判断是否处于缴械状态下。
	if g_MainPlayer:IsInForbitUseWeaponState() then
		MsgClient(194005)
		return
	end
	local bigID, index, itemid = EquipInfo:GetBigID(), EquipInfo:GetIndex(),EquipInfo:GetItemID()
	if g_ItemInfoMgr:IsStaticEquip(bigID) then --判断是不是静态装备
		local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(itemid)
		local DisplayName	= g_ItemInfoMgr:GetItemLanInfo(bigID, index,"DisplayName")
		local equipName = g_Tooltips:GetEquipShowName(DynInfo,DisplayName,"*")
		equipName = string.format("[%s]", equipName)
		local function CallBack(g_GameMain,uButton)
			if uButton == MB_BtnOK then
				local tbl = {bigID,index,itemid,Context,0,0}
				g_WndMouse:SetCursorState(ECursorState.eCS_EquipSmeltSoul,tbl)
				CEquipSmeltSoulScroll.EquipSmeltSoul(2,eEquipPart) 
			end
			g_GameMain.m_EquipSmeltSoul = nil
			g_WndMouse:ClearCursorSpecialState()
			return true
		end
		if g_GameMain.m_EquipSmeltSoul then
			g_WndMouse:ClearCursorSpecialState()
			return
		end
		g_GameMain.m_EquipSmeltSoul = MessageBox(g_GameMain, MsgBoxMsg(17010,equipName),  BitOr(MB_BtnOK,MB_BtnCancel), CallBack, g_GameMain)
	end
end

--如果装备耐久值为0，则装备蒙红显示最上层
local function SetColorWndFocus(EquipWnd)
	local partTbl = g_GameMain.m_RoleStatus.Part
    local colorWnd = nil
    for i, v in pairs(partTbl) do
        if EquipWnd == v.iconbtn then
            colorWnd = v.IconColorBtn
            break
        end
    end
    if colorWnd ~= nil and
        colorWnd:IsShow() then
        colorWnd:SetFocus()
    end
end

--装备双侧交换(左右手交换)
function CPlayerStatus:ExChangeEquip(Context, EquipInfo, EquipWnd, eEquipPart)
	local nItem, SelectEquipPart, MouseEquipWnd = Context[1], Context[2], Context[3]
	if (SelectEquipPart == EEquipPart.eRingLeft and eEquipPart == EEquipPart.eRingRight) or
		(SelectEquipPart == EEquipPart.eRingRight and eEquipPart == EEquipPart.eRingLeft) then
		Gac2Gas:SwitchTwoRing(g_Conn)
	elseif (SelectEquipPart == EEquipPart.eWeapon and eEquipPart == EEquipPart.eAssociateWeapon )or
			(SelectEquipPart == EEquipPart.eAssociateWeapon and eEquipPart == EEquipPart.eWeapon ) then
		Gac2Gas:SwitchWeapon(g_Conn)
	end
	
    SetColorWndFocus(EquipWnd)
	g_WndMouse:ClearCursorAll()
end

--把从包裹来的装备放到身上
function CPlayerStatus:ButOnFromBag(Context, EquipWnd, eEquipPart, EquipInfo)
	local FromRoom, Fromrow, Fromcol, Count = Context[1], Context[2],Context[3], Context[4]
	local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc(FromRoom)
	local SrcPos = SrcCt:GetPosByIndex(Fromrow, Fromcol, FromRoom)
	local Grid = g_GameMain.m_BagSpaceMgr:GetGridByRoomPos(FromRoom, SrcPos)
	local nBigID,nIndex = Grid:GetType()
	if g_ItemInfoMgr:IsEquip(nBigID) then
		g_GacUseItem(EquipWnd, nBigID, nIndex, FromRoom, SrcPos, eEquipPart)
	end
	g_WndMouse:ClearCursorAll()
end

--打开面板
function CPlayerStatus:OnOpenWnd()
	--属性同步
	local id = g_MainPlayer:GetEntityID()
	local viewinfoMgr = CFighterViewInfoMgr.GetWnd()
	viewinfoMgr:AddViewInfoByID(id)
	self.m_Target = g_MainPlayer
	self:DrawRole()
	SetAni(self.m_WndBody.RenderObj)
	self:ShowErrorEquipInfo()
	self:SetShowNeedInfo()
end

--关闭面板
function CPlayerStatus:OnCloseWnd()
	if self.m_DrawTick ~= nil then
		UnRegisterTick(self.m_DrawTick)
		self.m_DrawTick = nil
	end
	local id = g_MainPlayer:GetEntityID()
	local viewinfoMgr = CFighterViewInfoMgr.GetWnd()
	viewinfoMgr:DelViewInfoByID(id)
end

function CPlayerStatus:ShowPanel(bFlag)
	self:ShowWnd(bFlag)
	if(bFlag) then
		self:OnOpenWnd()
	end
end

function CPlayerStatus:VirtualExcludeWndClosed()
	self:OnCloseWnd()
end

--清除炼化框中的图标并恢复被锁定的放置装备的格子的可用状态
function CPlayerStatus:ClearUpIconWnd()
	g_WndMouse:ClearIconWnd(self.m_UpSpaceIconWnd)
	if(self.m_fromBagItemWnd) then
		self.m_fromBagItemWnd:EnableWnd(true)
	end
    g_GameMain.m_EquipUpIntensifyWnd:CloseMenuList()
end

function CPlayerStatus:UpDateSoulValue(soulValue)
	self.m_SoulValue:SetWndText(soulValue)
end

function CPlayerStatus:AddImage(Wnd,strTexture)
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP_ENABLE	
	local DefaultImage = WND_IMAGE_LIST:new()
	DefaultImage:AddImage(Wnd:GetGraphic(), -1,  strTexture, nil, CFPos:new(), "0xffffffff", 15 ) 	
	Wnd:SetWndBkImage( Flag, DefaultImage )
end

--更新角色面板信息
function CPlayerStatus:UpdataEquipTips()
	for i = 1, nAllPartNum do
		local Iconbtn = self.Part[i].iconbtn
		local eEquipPart = self.Part[i].PartType
		local PartTypeName = self.Part[i].PartTypeName
		if self.Part[i].m_Info then
			local Info = self.Part[i].m_Info 
			local nBigID,nIndex,nItemID = Info:GetBigID(),Info:GetIndex(),Info:GetItemID()
			g_SetItemRichToolTips(Iconbtn,nBigID,nIndex,nItemID)
		else
			Iconbtn:SetMouseOverDescAfter(PartTypeName)
		end
	end
end

function CPlayerStatus:UpdateLifeEquipByItemID(nItemID)
	local dynItemInfo = g_DynItemInfoMgr:GetDynItemInfo(nItemID)
	if ( not dynItemInfo ) then return end
	for i = 1, nAllPartNum do
		local part = self.Part[i]
		local item = part.m_Info
		if( item and nItemID == item:GetItemID() ) then
			if( 0 == dynItemInfo.m_nLifeTime ) then
				local nBigID, nIndex = item:GetBigID(), item:GetIndex()
				local itemLifeInfo = g_ItemInfoMgr:GetItemLifeInfo(nBigID, nIndex)
				if( itemLifeInfo ) then
					g_LoadIconFromRes(itemLifeInfo("TimeOutSmallIcon"), part.iconbtn, -2, IP_ENABLE, IP_CLICKDOWN)
					g_SetItemRichToolTips(part.iconbtn,nBigID, nIndex, nItemID)
				end
			end
			break
		end
	end
end

function CPlayerStatus:UpdateLifeEquipTipsByItemID(nItemID)
	for i = 1, nAllPartNum do
		local part = self.Part[i]
		local item = part.m_Info
		if( item and nItemID == item:GetItemID() ) then
			local nBigID, nIndex = item:GetBigID(), item:GetIndex()
			g_SetItemRichToolTips(part.iconbtn,nBigID, nIndex, nItemID)
			break
		end
	end
end

function CPlayerStatus:GetEquipPartToolTips(eEquipPart)
	local ItemInfo = nil 
	for i = 1, nAllPartNum do
		if eEquipPart == self.Part[i].PartType
			and self.Part[i].m_Info ~= nil then
			ItemInfo = self.Part[i].m_Info	
			break
		end
	end
	if ItemInfo ~= nil then
		local nBigID, nIndex, nItemID = ItemInfo:GetBigID(), ItemInfo:GetIndex(), ItemInfo:GetItemID()
		return {nBigID, nIndex, nItemID}
	else
		return nil
	end
end

function CPlayerStatus:SetPieceSkillNameToNumberTbl()
	self.m_PieceSkillNameToNumberTbl = {}
	for i = 1, nAllPartNum do
		if self.Part[i].m_Info ~= nil then
			local Info = self.Part[i].m_Info
			local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(Info:GetItemID())
			if DynInfo == nil then
				LogErr("物品动态信息错误" .. "sBigID:" .. Info:GetBigID() .. "sIndex:" .. Info:GetIndex()  .. "ID:" .. Info:GetItemID())
			end
			SetJingLingPieceSkillNumber(self.m_PieceSkillNameToNumberTbl,DynInfo)
		end
	end
end

--得到角色身上的精灵技能
function CPlayerStatus:GetJingLingSkillNameTbl()
	local JingLingSkillTbl = {}
	for PieceSkillName,PieceNumber in pairs (self.m_PieceSkillNameToNumberTbl) do
		if g_AttributeOfJingLingTbl[PieceSkillName] and g_AttributeOfJingLingTbl[PieceSkillName].PieceCount <= PieceNumber then
			table.insert(JingLingSkillTbl, g_AttributeOfJingLingTbl[PieceSkillName].Index)
		end
	end
	return JingLingSkillTbl
end

function CPlayerStatus:ShowErrorEquipInfo()
	for i = 1, nAllPartNum do
		if self.Part[i].m_Info then
			local ItemID = self.Part[i].m_Info:GetItemID()
			local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(ItemID)
			if  DynInfo.BaseLevel > g_MainPlayer:CppGetLevel() then
				self.Part[i].IconColorBtn:ShowWnd(true)
			else
				if DynInfo.CurDuraValue > 0 then
					self.Part[i].IconColorBtn:ShowWnd(false)
				else
					self.Part[i].IconColorBtn:ShowWnd(true)
				end	 
			end
		else
			self.Part[i].IconColorBtn:ShowWnd(false)
		end
	end
end

function CPlayerStatus:OpenSelfWnd( flag )
	if not flag then 								--关面板时调用
		local state = g_WndMouse:GetCursorState()
		if state == ECursorState.eCS_RenewEquip then
			g_WndMouse:ClearCursorSpecialState()
		end
	end
	self:ShowPanel( flag )
	g_GameMain.m_EquipUpIntensifyWnd:SetWndFlashInfo()
end

--设置经验条的长度
function CPlayerStatus:SetExpLong(nowExp, maxExp)
	self.m_Exp_pro:SetRange( maxExp )
	self.m_Exp_pro:SetPos( nowExp )
	self.m_Exp_pro:SetWndText( nowExp .. "/" .. maxExp )
end


function CPlayerStatus:SetShowNeedInfo()
	if g_MainPlayer then
		local tongpos = g_GameMain.m_TongBase.m_TongPos
		local tongName = g_MainPlayer.m_Properties:GetTongName()
		local PosNameTable = g_TongPurviewMgr:GetPosNameTableOrderByLevel()
		tongpos = PosNameTable[tongpos]
		SetShowNeedInfoCommFun(self,g_MainPlayer,tongpos,tongName)
	end
end

--RPC					--5			--Q			--1002
--得到人物的详细信息 -- 装备的类型 ，装备的名称，物品ID，装备的部位
function CPlayerStatus.RetEquipInfo( nBigID, nIndex, nItemID, eEquipPart,nBindingType )
	local Item = CItem:new(nBigID,nIndex,nItemID,nBindingType)
	local SmallIcon = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"SmallIcon")

	assert(IsNumber(eEquipPart))
	g_DynItemInfoMgr:CreateItemInfo(nItemID,nBigID,nIndex,nBindingType)
	local Wnd = g_GameMain.m_RoleStatus
	local Iconbtn
	for i = 1, nAllPartNum do
		if eEquipPart == Wnd.Part[i].PartType then
			Wnd.Part[i].m_Info = Item
			Iconbtn = Wnd.Part[i].iconbtn
			break
		end
	end

	--设置精灵信息
	g_GameMain.m_RoleStatus:SetPieceSkillNameToNumberTbl()
	g_GameMain.m_RoleStatus:ShowErrorEquipInfo()
	g_GameMain.m_WndMainBag.m_ctItemRoom:SetQualityFlashInfo(Iconbtn, nItemID)
	g_LoadIconFromRes(SmallIcon, Iconbtn, -2,  IP_ENABLE, IP_CLICKDOWN)
	g_GameMain.m_RoleStatus:UpdataEquipTips()
	g_GameMain.m_WndMainBag.m_ctItemRoom:UpdateBagTips()
	
	if g_ItemInfoMgr:IsStaticWeapon(nBigID) then
		CUseItemRpcMgr.SetAttackEffect(nBigID,nIndex ,eEquipPart,g_MainPlayer,g_MainPlayer.m_Properties:GetHorseType(),
		g_MainPlayer.m_Properties:GetBehaviorType(),g_MainPlayer:CppGetClass(),g_MainPlayer.m_Properties:GetSex())
	end
end

--更改副手武器普攻类型
function CPlayerStatus.ReAssociateAttr(bool)
	if bool then
		local AssociateWeaponItem = g_GameMain.m_RoleStatus.Part[6].m_Info
		local nBigID,nIndex,nItemID = AssociateWeaponItem:GetBigID(), AssociateWeaponItem:GetIndex(),AssociateWeaponItem:GetItemID()
		local EquipType = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"EquipType")
		local AttackSpeed = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"AttackSpeed")
		local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(nItemID)
		local CommonlySkillName = g_GetPlayerClassNameByID(g_MainPlayer:CppGetClass()).. "副手武器普通攻击"
		if DynInfo.BaseLevel <= g_MainPlayer:CppGetLevel() then
			if g_ItemInfoMgr:IsWeapon(nBigID) and tonumber(DynInfo.CurDuraValue) > 0 then
				g_MainPlayer:SetAHNACfg(CommonlySkillName,AttackSpeed,EquipType)		
			elseif g_ItemInfoMgr:IsStaticShield(nBigID) and tonumber(DynInfo.CurDuraValue) > 0 then
				g_MainPlayer:SetAHNACfg("",0,EquipType)	
			elseif tonumber(DynInfo.CurDuraValue) == 0 then
				g_MainPlayer:SetAHNACfg("",0,"")		
			end
		else
			g_MainPlayer:SetAHNACfg("",0,"")	
		end
	else
		--print("摘副手")
		g_MainPlayer:SetAHNACfg("",0,"")	
	end
end

--更改主手武器普攻类型
function CPlayerStatus.ReWeaponAttr(bool,CommonlySkillName)
	if bool then
		local WeaponItem = g_GameMain.m_RoleStatus.Part[12].m_Info
		if WeaponItem ~= nil then
			local item_type,item_name = WeaponItem:GetBigID(), WeaponItem:GetIndex()
			local AttackSpeed = g_ItemInfoMgr:GetItemInfo(item_type,item_name,"AttackSpeed")
			local EquipType = g_ItemInfoMgr:GetItemInfo(item_type,item_name,"EquipType")
			local DynInfo		= g_DynItemInfoMgr:GetDynItemInfo(WeaponItem:GetItemID())
			if DynInfo.BaseLevel <= g_MainPlayer:CppGetLevel() then
				g_MainPlayer:SetMHNACfg(CommonlySkillName,AttackSpeed,EquipType,WeaponDPSFloor_Common(EquipType,"AttackType"))
			else
				g_MainPlayer:SetMHNACfg(CommonlySkillName,2,"","")	
			end
		end
	else
		g_MainPlayer:SetMHNACfg(CommonlySkillName,2,"","")
	end
end

--在角色属性面板按住SHIFT+左键
function DoShiftDownState(wnd,Child)
	local Wnd = g_GameMain.m_CreateChatWnd.m_ChatSendArea
	if Wnd:IsShow() then
		for i = 1, nAllPartNum do
			if(Child == wnd.Part[i].iconbtn) then
				local EquipInfo = wnd.Part[i].m_Info
				if EquipInfo then
					local bigID, index, itemid = EquipInfo:GetBigID(), EquipInfo:GetIndex(),EquipInfo:GetItemID()
					local DisplayName	= g_ItemInfoMgr:GetItemLanInfo(bigID, index,"DisplayName")
					index = DisplayName and DisplayName or index
					Wnd.m_CEditMessage:InsertStr("[" .. itemid .. "]")
					local ItemTable = g_GameMain.m_CreateChatWnd.m_ChatSendArea.ItemTable
					local Item = {GlobalID = itemid,Index = index, BigId = bigID}
					ItemTable[itemid] = Item
					Wnd.m_CEditMessage:SetFocus()
				end
				break
			end
		end
	end
end

