CBaseInfo				= class ( SQRDialog )

local function SetPos(wnd, x, y)
	local Rect = CFRect:new()
	wnd:GetLogicRect(Rect)
	if(x) then
		local uWidth  = Rect.right - Rect.left
		Rect.left	= x
		Rect.right	= x+uWidth
	end
	if(y) then
		local uHeight = Rect.bottom - Rect.top
		Rect.top	= y
		Rect.bottom	= y+uHeight
	end
	wnd:SetLogicRect(Rect)
end

function CBaseInfo:Init(NameWnd, faceWnd, HPWnd, MPWnd, eType, bNeedChangeName)
	self.m_NameWnd		= NameWnd
	self.m_FaceWnd		= faceWnd
	self.m_HPWnd		= HPWnd
	self.m_MPWnd		= MPWnd
	self.eNpcType		= eType
	self.bChangeName	= bNeedChangeName
	self.m_HPWnd:SetProgressMode(0)
	self.m_MPWnd:SetProgressMode(0)
end

function CBaseInfo:ReSetServantWndRect()
	
	local tblWnd = {	CServantInfo.GetWnd(),
						CBattleHorseInfo.GetWnd(),
						CTruckInfo.GetWnd(),
						COrdnanceMonsterInfo.GetWnd(),
						CCannonInfo.GetWnd()	}
	
	local rtRarentRect	= CFRect:new()
	local parentwnd		= g_GameMain.m_PlayerInfo
	for i = 1, #tblWnd do
		local wnd = tblWnd[i]
		if ( wnd:IsShow() ) then
			local offsetX = (parentwnd == g_GameMain.m_PlayerInfo) and 1 or 0
			parentwnd:GetLogicRect(rtRarentRect)
			SetPos(wnd, rtRarentRect.left + offsetX, rtRarentRect.bottom)
			parentwnd = wnd
		end
	end
end

function CBaseInfo:OnServantNameChanged()
	local Servant = g_MainPlayer:GetServant(self.eNpcType)
	if not Servant then return end
	local ServantName	= Servant:GetRealName()
	if(ServantName == "") then
		ServantName = GetNpcDisplayName(Servant.m_Properties:GetCharName())
	end
	self.m_NameWnd:SetWndText(ServantName)
end

function CBaseInfo:OnServantCreated()
	local Servant = g_MainPlayer:GetServant(self.eNpcType)
	if not Servant then return end
	self.m_ServantID = Servant:GetEntityID()
	self.m_HPWnd:EndChangeDiff()
	self:OnServantNameChanged(Servant)
	self:ShowWnd(true)
	self:ReSetServantWndRect()
	self:Update()
	self:SetMPPic(7, 150, 49, 58, 0, 0)
end

function CBaseInfo:SetMPPic(left, right, top, bottom, x, y)
	local rect		= CFRect:new()
	local pos		= CFPos:new()
	rect.left, rect.right, rect.top, rect.bottom, pos.x, pos.y = left, right, top, bottom, x, y
	local strTex = g_ImageMgr:GetImagePath(1352)
	g_DynImageMgr:AddImageNeedCut(self.m_MPWnd, strTex, IP_FILL, rect, pos)
end

function CBaseInfo:OnServantDestoryedCommon()
	self.m_ServantID = 0
	self:ShowWnd(false)
	self:ReSetServantWndRect()
end

function CBaseInfo:Update()
	local Servant = g_MainPlayer:GetServant(self.eNpcType)
	if not Servant then return end
	self.m_HPWnd:SetWndText(Servant:CppGetPropertyValueByName("HealthPointAgile") .. "/" .. Servant:CppGetPropertyValueByName("HealthPoint"))
	self.m_HPWnd:SetRange( Servant:CppGetPropertyValueByName("HealthPoint") )	
	self.m_HPWnd:SetPos( Servant:CppGetPropertyValueByName("HealthPointAgile") )
	self.m_MPWnd:SetRange( Servant:CppGetPropertyValueByName("ManaPoint") )
	self.m_MPWnd:SetPos( Servant:CppGetPropertyValueByName("ManaPointAgile") )
	self.m_MPWnd:SetWndText(Servant:CppGetPropertyValueByName("ManaPointAgile") .. "/" .. Servant:CppGetPropertyValueByName("ManaPoint"))
	g_GameMain.m_CharacterInSyncMgr:ChangeHPByMe(self.m_HPWnd)
end

--点击宠物头像, 选中目标事件
function CBaseInfo:OnLButtonDown( wParam, x, y )
	local Servant = g_MainPlayer:GetServant(self.eNpcType)
	if not Servant then return end
	g_MainPlayer:CancelNormalAttack()
	g_MainPlayer:LockObj( Servant )
	g_GameMain.m_TargetInfo:Update()
end

--点击Servant头像, 弹出右键菜单
function CBaseInfo:OnRButtonDown( wParam, x, y )
	local Servant = g_MainPlayer:GetServant(self.eNpcType)
	if not Servant then return end
	if( self.bChangeName ) then
		CChangeNameWnd.GetWnd():Init(self.eNpcType)
		local Menu = CMenu:new(" ",g_GameMain)
		Menu:InsertItem(GacMenuText(1801), self.OnMenuMsg, nil, false, false, nil, self,self.OnChangeName )
		if( g_MainPlayer:GetServant(ENpcType.ENpcType_Summon) ) then
			Menu:InsertItem(GacMenuText(1705),self.OnMenuMsg, nil, false, false, nil, self,self.RemoveServant)
		end
		local Rect=CFRect:new()
		self:GetLogicRect(Rect)
		Menu:SetPos( Rect.left+x+15 , Rect.top+y )
		self.m_Menu=Menu
	end
end

function CBaseInfo:OnMenuMsg(func)
	if(func) then func(self) end
	self.m_Menu:Destroy()
	self.m_Menu = nil
end

--解散召唤兽
function CBaseInfo:RemoveServant()
	if(g_MainPlayer:CppGetCtrlState(EFighterCtrlState.eFCS_InBattleHorse)) then
		--g_MainPlayer:FightSkill( "下召唤兽坐骑", 1)
	else
		g_MainPlayer:DestroyServant()
	end
end