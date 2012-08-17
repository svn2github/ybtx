
CBattleHorseInfo		= class ( CBaseInfo )

--ÕÙ»½ÊÞ×øÆï
function CBattleHorseInfo:Ctor( Parent )
	self:CreateFromRes( "BattleHorseInfo", Parent )
	g_GameMain.m_BattleHorseBuffWnd	= CBaseServantBuffWnd:new(self)
end

function CBattleHorseInfo.GetWnd()
	local Wnd = g_GameMain.m_BattleHorseWnd
	if not Wnd then
		Wnd = CBattleHorseInfo:new(g_GameMain)
		g_GameMain.m_BattleHorseWnd = Wnd
	end
	return Wnd
end

function CBattleHorseInfo:OnChildCreated()
	self.m_NameWnd		= self:GetDlgChild("Name")
	self.SkillButton1 = self:GetDlgChild("Skill1")
	self.SkillButton2 = self:GetDlgChild("Skill2")  
	self.m_WndFace    = self:GetDlgChild("WndPetFace")
	self.m_HPWnd 			= self:GetDlgChild("PetHP")
	self.m_MPWnd 			= self:GetDlgChild("PetMP")
	self:Init(self.m_NameWnd,self.m_WndFace,self.m_HPWnd,self.m_MPWnd,ENpcType.ENpcType_BattleHorse,true)
end

function CBattleHorseInfo:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		if Child == self.SkillButton1 then
			Gac2Gas:MasterAttackCommand(g_Conn, g_MainPlayer:GetEntityID(), ENpcType.ENpcType_BattleHorse) --ÕÙ»½ÊÞµÄNpcÀàÐÍÎª3
		elseif Child == self.SkillButton2 then
			Gac2Gas:MasterRetreatCommand(g_Conn, g_MainPlayer:GetEntityID(), ENpcType.ENpcType_BattleHorse)
		end
	end
end

function CBattleHorseInfo:OnChangeName()
	local Wnd = CChangeNameWnd.GetWnd()
	Wnd:ShowAndSetText(GetStaticTextClient(1061))
end

function CBattleHorseInfo:OnServantDestoryed()
	self:OnServantDestoryedCommon()
	g_GameMain.m_BattleHorseBuffWnd:ClearAllBuffState()
end
