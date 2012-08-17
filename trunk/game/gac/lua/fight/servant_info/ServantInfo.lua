
CServantInfo			= class ( CBaseInfo )

--召唤兽
function CServantInfo:Ctor( Parent )
	self:CreateFromRes( "PetInfo", Parent )
end

function CServantInfo.GetWnd()
	local Wnd = g_GameMain.m_Servant
	if not Wnd then
		Wnd = CServantInfo:new(g_GameMain)
		g_GameMain.m_Servant = Wnd
		g_GameMain.m_ServantBuffWnd = CBaseServantBuffWnd:new(g_GameMain.m_Servant)
	end
	return Wnd
end

function CServantInfo:OnChildCreated()
	self.m_WndFace		= self:GetDlgChild("WndPetFace")
	self.m_NameWnd		= self:GetDlgChild("Name")
	self.m_HPWnd		= self:GetDlgChild("PetHP")
	self.m_MPWnd		= self:GetDlgChild("PetMP")
	self.SkillButton1	= self:GetDlgChild("Skill1")
	self.SkillButton2	= self:GetDlgChild("Skill2")
	self.SkillButton3	= self:GetDlgChild("Skill3")
	--self.SkillButton3:ShowWnd(false)
	self:Init(self.m_NameWnd,self.m_WndFace,self.m_HPWnd,self.m_MPWnd,ENpcType.ENpcType_Summon,true)
end

function CServantInfo:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		if Child == self.SkillButton1 then
			Gac2Gas:MasterAttackCommand(g_Conn, g_MainPlayer:GetEntityID(), ENpcType.ENpcType_Summon)	--召唤兽的Npc类型为3
		elseif Child == self.SkillButton2 then
			Gac2Gas:MasterRetreatCommand(g_Conn, g_MainPlayer:GetEntityID(), ENpcType.ENpcType_Summon)
		elseif Child == self.SkillButton3 then
			self:OrderServantMove()
--		Gac2Gas:MasterDisbandCommand(g_Conn, g_MainPlayer:GetEntityID(), ENpcType.ENpcType_Summon)
		end
	end
end

function CServantInfo:OnChangeName()
	local Wnd = CChangeNameWnd.GetWnd()
	Wnd:ShowAndSetText(GetStaticTextClient(1061))
end

function CServantInfo:OnServantDestoryed()
	self:OnServantDestoryedCommon()
	g_GameMain.m_ServantBuffWnd:ClearAllBuffState()
	CChangeNameWnd.GetWnd():ShowWnd(false)
end

function CServantInfo:OrderServantMove()
	CGroundSelector:BeginOrderServantMove()
end
