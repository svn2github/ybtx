
CCannonInfo				= class ( CBaseInfo )

--炮台
function CCannonInfo:Ctor( Parent )
	self:CreateFromRes( "BattleHorseInfo", Parent )
	g_GameMain.m_CannonBuffWnd = CBaseServantBuffWnd:new(self)
end

function CCannonInfo.GetWnd()
	local Wnd = g_GameMain.m_CannonInfo
	if not Wnd then
		Wnd = CCannonInfo:new(g_GameMain)
		g_GameMain.m_CannonInfo = Wnd
	end
	return Wnd
end

function CCannonInfo:OnChildCreated()
	self.m_NameWnd		= self:GetDlgChild("Name")
	self.SkillButton1 = self:GetDlgChild("Skill1")
	self.SkillButton2 = self:GetDlgChild("Skill2")  
	self.m_WndFace    = self:GetDlgChild("WndPetFace")
	self.m_HPWnd 	= self:GetDlgChild("PetHP")
	self.m_MPWnd 	= self:GetDlgChild("PetMP")
	self:Init(self.m_NameWnd,self.m_WndFace,self.m_HPWnd,self.m_MPWnd,ENpcType.ENpcType_Cannon,false)
	self.SkillButton1:SetWndText(GetStaticTextClient(1062))
	self.SkillButton2:SetWndText(GetStaticTextClient(1063))
end

function CCannonInfo:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		if Child == self.SkillButton1 then
			--Gac2Gas:MasterAttackCommand(g_Conn, g_MainPlayer:GetEntityID(), ENpcType.ENpcType_Cannon) --召唤兽的Npc类型为3
		elseif Child == self.SkillButton2 then
			--Gac2Gas:MasterRetreatCommand(g_Conn, g_MainPlayer:GetEntityID(), ENpcType.ENpcType_Cannon)
		end
	end
end
