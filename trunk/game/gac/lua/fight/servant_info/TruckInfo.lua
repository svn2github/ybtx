gac_gas_require "relation/tong/TongMgr"

CTruckInfo				= class ( CBaseInfo )

function CTruckInfo:Ctor( Parent )
	self:CreateFromRes( "TruckInfo", Parent )
	self.eNpcType		= ENpcType.ENpcType_Truck
	self.bChangeName	= false
end

function CTruckInfo.GetWnd()
	local Wnd = g_GameMain.m_TruckWnd
	if not Wnd then
		Wnd = CTruckInfo:new(g_GameMain)
		g_GameMain.m_TruckWnd = Wnd
	end
	
	return Wnd
end

function CTruckInfo:OnChildCreated()
	self.m_WndFace		= self:GetDlgChild("WndPetFace")
	self.m_NameWnd		= self:GetDlgChild("Name")
	self.m_HPWnd		= self:GetDlgChild("PetHP")
	self.m_MPWnd		= self:GetDlgChild("PetMP")
	
	self.m_HPWnd:SetProgressMode(0)
end

--function CTruckInfo:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
--	if( uMsgID == BUTTON_LCLICK ) then
--		if Child == self.SkillButton1 then
--			Gac2Gas:TruckFollow(g_Conn, self.m_ServantID)
--		elseif Child == self.SkillButton2 then
--			Gac2Gas:TruckStop(g_Conn, self.m_ServantID)
--			CTongForageSeekWnd:OnDestroyed()
--		end
--	end
--end

function CTruckInfo:OnServantCreated()
	local Servant = g_MainPlayer:GetServant(self.eNpcType)
	if not Servant then return end
	self.m_ServantID = Servant:GetEntityID()
	self:OnServantNameChanged(Servant)
	self:ShowWnd(true)
	self:ReSetServantWndRect()
	self:Update()
	self:SetMPPic(7, 150, 56, 65, 0, 0)
	
	local sTruckName	= Servant.m_Properties:GetCharName()
	self.m_nMaxMP		= g_TongMgr.m_tblTruckMaxLoad[sTruckName]
	self:SetMPValue(Servant:GetCurMaterialNum(), self.m_nMaxMP)
	
	--传入驻地战线,打开自动寻路面板
	Gac2Gas:OpenAutoSeekWnd(g_Conn)
end

function CTruckInfo:OnServantDestoryed()
	self:OnServantDestoryedCommon()
	CTongForageSeekWnd.OnDestroyed()
end

function CTruckInfo:Update()
	local Servant = g_MainPlayer:GetServant(self.eNpcType)
	if not Servant then return end
	self.m_NameWnd:SetWndText(ServantName)
	self.m_HPWnd:SetWndText(Servant:CppGetPropertyValueByName("HealthPointAgile") .. "/" .. Servant:CppGetPropertyValueByName("HealthPoint"))
	self.m_HPWnd:SetRange( Servant:CppGetPropertyValueByName("HealthPoint") )	
	self.m_HPWnd:SetPos( Servant:CppGetPropertyValueByName("HealthPointAgile") )
end

function CTruckInfo:SetMPValue(cur, max)
	if cur > max then
		max = cur
	end
	self.m_MPWnd:SetRange(max)
	self.m_MPWnd:SetPos(cur)
	self.m_MPWnd:SetWndText(cur .. "/" .. max)
end