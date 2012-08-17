
COrdnanceMonsterInfo	= class ( CBaseInfo )

--攻城器械
function COrdnanceMonsterInfo:Ctor( Parent )
	self:CreateFromRes( "PetInfo", Parent )
	g_GameMain.m_OrdnanceMonsterBuffWnd	= CBaseServantBuffWnd:new(self)
end

function COrdnanceMonsterInfo.GetWnd()
	local Wnd = g_GameMain.m_OrdnanceMonsterInfo
	if not Wnd then
		Wnd = COrdnanceMonsterInfo:new(g_GameMain)
		g_GameMain.m_OrdnanceMonsterInfo = Wnd
	end
	return Wnd
end

function COrdnanceMonsterInfo:OnChildCreated()
	self.m_WndFace		= self:GetDlgChild("WndPetFace")
	self.m_NameWnd		= self:GetDlgChild("Name")
	self.m_HPWnd		= self:GetDlgChild("PetHP")
	self.m_MPWnd		= self:GetDlgChild("PetMP")
	self.SkillButton1	= self:GetDlgChild("Skill1")
	self.SkillButton2	= self:GetDlgChild("Skill2")
	self:Init(self.m_NameWnd,self.m_WndFace,self.m_HPWnd,self.m_MPWnd,ENpcType.ENpcType_OrdnanceMonster,false)
end

function COrdnanceMonsterInfo:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		if Child == self.SkillButton1 then
			--Gac2Gas:MasterAttackCommand(g_Conn, g_MainPlayer:GetEntityID(), ENpcType.ENpcType_OrdnanceMonster)	--召唤兽的Npc类型为3
		elseif Child == self.SkillButton2 then
			--Gac2Gas:MasterRetreatCommand(g_Conn, g_MainPlayer:GetEntityID(), ENpcType.ENpcType_OrdnanceMonster)
		end
	end
end
