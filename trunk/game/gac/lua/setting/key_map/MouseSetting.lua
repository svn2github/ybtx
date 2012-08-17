gac_gas_require "setting/SettingMgr"

CMouseSetting = class( SQRDialog )

function CMouseSetting:Ctor(parent)
	self:CreateFromRes( "MouseSetWnd", parent )
	self:SetDefault()
end

function CMouseSetting:SetDefault()
	self.m_LockTargetFlag	= false
	self.m_MoveLeftFlag		= true
	self.m_SelectLeftFlag	= false
	self.m_SelectRightFlag	= true
	self.m_AttrackLeftFlag	= true
	self.m_AttrackRightFlag	= false
	self.m_TabDistanceFlag	= 10
	
	self.m_TabDistance:SetRange(20)
	self.m_TabDistance:SetPage(1)
	self:SetInfo()
end

function CMouseSetting:OnChildCreated()
	self.m_LockTarget			= self:GetDlgChild("LockTarget")
	self.m_MoveLeft				= self:GetDlgChild("moveLeft")
	self.m_MoveRight			= self:GetDlgChild("moveRight")
	self.m_SelectLeft			= self:GetDlgChild("selectLeft")
	self.m_SelectRight			= self:GetDlgChild("selectRight")
	self.m_AttrackLeft			= self:GetDlgChild("attrackLeft")
	self.m_AttrackRight			= self:GetDlgChild("attrackRight")
	self.m_TabDistance			= self:GetDlgChild("tabDistance")
	self.m_TabDistanceNumber	= self:GetDlgChild("tabDistanceNumber")
	self.m_CustomCK				= self:GetDlgChild("CustomCK")
	
	self.m_tblDefaultCK = {}
	for i = 1, 3 do
		self.m_tblDefaultCK[i] = self:GetDlgChild("DefaultCK" .. i)
	end
end

function CMouseSetting:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == SCROLL_MOVE) then
		if(Child == self.m_TabDistance) then
			self:UpdateTabDistanceText()
		end
	elseif (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_AttrackLeft) then
			if( self.m_AttrackRight:GetCheck() ) then
				self.m_AttrackLeft:SetCheck( not self.m_AttrackLeft:GetCheck() )
				self.m_AttrackRight:SetCheck( false )
			else
				self.m_AttrackLeft:SetCheck( not self.m_AttrackLeft:GetCheck() )
			end
		elseif(Child == self.m_AttrackRight) then
			if( self.m_AttrackLeft:GetCheck() ) then
				self.m_AttrackRight:SetCheck( not self.m_AttrackRight:GetCheck() )
				self.m_AttrackLeft:SetCheck( false )
			else
				self.m_AttrackRight:SetCheck( not self.m_AttrackRight:GetCheck() )
			end
		elseif(Child == self.m_CustomCK) then
			self:EnableCK(true)
		else
			for i, v in ipairs(self.m_tblDefaultCK) do
				if(Child == v) then
					self:SetMouseDefalt(i)
				end
			end
		end
	end
end

function CMouseSetting:OpenPanel(bFlag)
	if(not bFlag) then
		self:SetInfo()
	end
	self:ShowWnd(bFlag)
end

function CMouseSetting:SetInfo()
	self.m_LockTarget:SetCheck(not self.m_LockTargetFlag)
	self.m_MoveLeft:SetCheck(self.m_MoveLeftFlag)
	self.m_MoveRight:SetCheck(not self.m_MoveLeftFlag)
	self.m_SelectLeft:SetCheck(self.m_SelectLeftFlag)
	self.m_SelectRight:SetCheck(self.m_SelectRightFlag)
	self.m_AttrackLeft:SetCheck(self.m_AttrackLeftFlag)
	self.m_AttrackRight:SetCheck(self.m_AttrackRightFlag)
	self.m_TabDistance:SetPos(self.m_TabDistanceFlag - 1)
	self.m_TabDistanceNumber:SetWndText(self.m_TabDistanceFlag)
	
	local lockTarget	= self.m_LockTargetFlag and 1 or 0
	local movekey		= self.m_MoveLeftFlag and 1 or 0
	local selectLeft	= self.m_SelectLeftFlag and 1 or 0
	local selectRight	= self.m_SelectRightFlag and 1 or 0
	local attrackLeft	= self.m_AttrackLeftFlag and 1 or 0
	local attrackRight	= self.m_AttrackRightFlag and 1 or 0
	
	local nDefaultType = g_SettingMgr:MatchMouseSettingDefault(lockTarget, movekey,
		selectLeft, selectRight, attrackLeft, attrackRight)
	if(nDefaultType) then
		self:EnableCK(false)
		self.m_tblDefaultCK[nDefaultType]:SetCheck(true)
	else
		self:EnableCK(true)
		self.m_CustomCK:SetCheck(true)
	end
end

function CMouseSetting:SetMouseDefalt(nType)
	local tbl = {[0] = false, [1] = true}
	local info = g_SettingMgr:GetMouseOptionByType(nType)
	self.m_LockTarget:SetCheck(not tbl[info.lockTarget])
	self.m_MoveLeft:SetCheck(tbl[info.movekey])
	self.m_MoveRight:SetCheck(not tbl[info.movekey])
	self.m_SelectLeft:SetCheck(tbl[info.selectLeft])
	self.m_SelectRight:SetCheck(tbl[info.selectRight])
	self.m_AttrackLeft:SetCheck(tbl[info.attrackLeft])
	self.m_AttrackRight:SetCheck(tbl[info.attrackRight])
	self:EnableCK(false)
end

function CMouseSetting:UpdateTabDistanceText()
	local distance = self.m_TabDistance:GetPos() + 1
	self.m_TabDistanceNumber:SetWndText(distance)
end

function CMouseSetting:EnableCK(bFlag)
	self.m_LockTarget:EnableWnd(bFlag)
	self.m_MoveLeft:EnableWnd(bFlag)
	self.m_MoveRight:EnableWnd(bFlag)
	self.m_SelectLeft:EnableWnd(bFlag)
	self.m_SelectRight:EnableWnd(bFlag)
	self.m_AttrackLeft:EnableWnd(bFlag)
	self.m_AttrackRight:EnableWnd(bFlag)
end

function CMouseSetting:ChangeMouseCtrl()
	local lockTarget	= self.m_LockTarget:GetCheck() and 0 or 1
	local movekey		= self.m_MoveLeft:GetCheck() and 1 or 0
	local selectLeft	= self.m_SelectLeft:GetCheck() and 1 or 0
	local selectRight	= self.m_SelectRight:GetCheck() and 1 or 0
	local attrackLeft	= self.m_AttrackLeft:GetCheck() and 1 or 0
	local attrackRight	= self.m_AttrackRight:GetCheck() and 1 or 0

	if attrackLeft == 0 and attrackRight == 0 and (self.m_AttrackLeftFlag == true or self.m_AttrackRightFlag == true) then
		MessageBox( g_GameMain, MsgBoxMsg(4000), MB_BtnOK )
	end
	
	local distance = self.m_TabDistance:GetPos() + 1
	Gac2Gas:ChangeMouseCtrl(g_Conn, lockTarget, movekey, selectLeft, selectRight, attrackLeft, attrackRight, distance)
end

function CMouseSetting:ChangeCtrlKey( lockTarget, movekey, selectLeft, selectRight, attrackLeft, attrackRight, distance)
	self.m_LockTargetFlag	= lockTarget == 1
	self.m_MoveLeftFlag		= movekey == 1
	self.m_SelectLeftFlag	= selectLeft == 1
	self.m_SelectRightFlag	= selectRight == 1
	self.m_AttrackLeftFlag	= attrackLeft == 1
	self.m_AttrackRightFlag	= attrackRight == 1
	self.m_TabDistanceFlag	= distance
	self:SetInfo()
	self:ChangeMouseSettingEnd()
end

function CMouseSetting:ChangeMouseSettingEnd()
	local distance = self.m_TabDistance:GetPos() + 1
	g_MainPlayer:SetTabDistance(distance)
end

function CMouseSetting:OpenAttackOnRButtonDown()
	return self.m_AttrackRightFlag
end

function CMouseSetting:OpenSelectOnRButtonDown() --开启选择
	return self.m_SelectRightFlag and not self.m_AttrackRightFlag
end

function CMouseSetting:OpenMoveOnRButtonDown() --开启移动
	return not self.m_MoveLeftFlag and not self.m_SelectRightFlag and not self.m_AttrackRightFlag
end

function CMouseSetting:OpenAttackOnLButtonDown()
	return self.m_AttrackLeftFlag
end

function CMouseSetting:OpenSelectOnLButtonDown() --开启选择
	return self.m_SelectLeftFlag  and not self.m_AttrackLeftFlag
end

function CMouseSetting:OpenMoveOnLButtonDown() --仅开启移动
	return self.m_MoveLeftFlag and not self.m_SelectLeftFlag  and not self.m_AttrackLeftFlag
end

function CMouseSetting:CanUnLockTargetOnRButtonUp()
	local bCanUnLockObj = true
	local Target = CEntityClient_GetSelectedEntity(g_CoreScene)
	if( Target ~= nil ) then
		if self.m_AttrackRightFlag then
			bCanUnLockObj = false
		end
		if self.m_SelectRightFlag and not self.m_AttrackRightFlag then
			bCanUnLockObj = false
		end
	end
		
	return bCanUnLockObj and (not self.m_LockTargetFlag) and self.m_MoveLeftFlag
end
	
function CMouseSetting:CanUnLockTargetOnLButtonUp()
	local bCanUnLockObj = true
	local Target = CEntityClient_GetSelectedEntity(g_CoreScene)
	if( Target ~= nil ) then
		if self.m_AttrackLeftFlag then
			bCanUnLockObj = false
		end
		if self.m_SelectLeftFlag and not self.m_AttrackLeftFlag then
			bCanUnLockObj = false
		end
	end
	return bCanUnLockObj and not self.m_LockTargetFlag and not self.m_MoveLeftFlag
end

function CMouseSetting:CanLButtonClickObj()
	if self.m_SelectLeftFlag or self.m_AttrackLeftFlag or (not self.m_MoveLeftFlag
		 and not self.m_SelectRightFlag and not self.m_AttrackRightFlag) then
		return true
	end
	return false
end

function CMouseSetting:CanRButtonClickObj()
	if (not self.m_SelectLeftFlag) and (not self.m_AttrackLeftFlag) and 
		(self.m_SelectRightFlag or self.m_AttrackRightFlag or self.m_MoveLeftFlag ) then
		return true
	end
	return false
end
