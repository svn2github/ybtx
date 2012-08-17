
CStatisticFightInfoWnd = class(SQRDialog)
CStatisticFITotalWnd = class(SQRDialog)
CStatisticFIPersonalWnd = class(SQRDialog)
CFIPersonalItemWnd = class(SQRDialog)
CFITotalItemWnd = class(SQRDialog)

local EShowStatisticType = 
{
	None					= 0,
	Totall				= 1,
	Personal			= 2,
	LastTotall		= 3,
	LastPersonal	= 4,
}

local EShowHurtType = 
{
	Hurt = 1,
	Heal = 2,
}

function CStatisticFightInfoWnd:Ctor(Parent)
	self:CreateFromRes("WndStatisticFightInfo",Parent)
	self.m_Total_Btn		= self:GetDlgChild("totall")
	self.m_Personal_Btn = self:GetDlgChild("personal")
	self.m_LastTotall_Btn 	= self:GetDlgChild("lasttotall")
	self.m_LastPersonal_Btn = self:GetDlgChild("lastpersonal")
	self.m_Begin_Btn		= self:GetDlgChild("begin")
	self.m_End_Btn 			= self:GetDlgChild("end")
	self.m_Clear_Btn 		= self:GetDlgChild("clear")
	self.m_Close_Btn 		= self:GetDlgChild("close")
	self.m_Left_Btn 		= self:GetDlgChild("left")
	self.m_Right_Btn 		= self:GetDlgChild("right")
	
	self.m_Totall_Wnd		= CStatisticFITotalWnd:new()
	self.m_Totall_Wnd:CreateFromRes("WndSFITotall",self)
	self.m_Totall_Wnd.m_HurtList = self.m_Totall_Wnd:GetDlgChild("HurtList")
	self.m_Totall_Wnd.m_TotallHurtTextWnd  = self.m_Totall_Wnd:GetDlgChild("TotallHurt")
	self.m_Totall_Wnd.m_DpsTextWnd			   = self.m_Totall_Wnd:GetDlgChild("Dps")
	self.m_Totall_Wnd.m_HurtPercentTextWnd = self.m_Totall_Wnd:GetDlgChild("HurtPercent")
	self.m_Totall_Wnd.m_HurtList:InsertColumn(0, self.m_Totall_Wnd.m_HurtList:GetWndOrgWidth())
	self.m_Personal_Wnd	= CStatisticFIPersonalWnd:new()
	self.m_Personal_Wnd:CreateFromRes("WndSFIPersonal",self)
	self.m_Personal_Wnd.m_HurtList = self.m_Personal_Wnd:GetDlgChild("HurtList")
	self.m_Personal_Wnd.m_TotallHurtTextWnd = self.m_Personal_Wnd:GetDlgChild("TotallHurt")
	self.m_Personal_Wnd.m_HurtList:InsertColumn(0, self.m_Personal_Wnd.m_HurtList:GetWndOrgWidth())
	self:InitStatisticFunc()
	self:Init()
end

function CStatisticFightInfoWnd.GetWnd()
	local Wnd = g_GameMain.m_StatisticFightInfoWnd
	if not Wnd then
		Wnd = CStatisticFightInfoWnd:new(g_GameMain)
		g_GameMain.m_StatisticFightInfoWnd = Wnd
	end
	return Wnd
end

function CStatisticFightInfoWnd:Init()
	self.m_SelfHurtTargetTbl = {}					--伤害目标列表
	self.m_LastFightSelfHurtTargetTbl = {}--上次战斗伤害目标列表
	self.m_TotallHurtTbl = {}							--所有伤害列表,队伍,团队
	self.m_LastFightTotallHurtTbl = {}		--上次战斗所有伤害列表,队伍,团队
	self.m_SelfHealTargetTbl = {}
	self.m_LastFightSelfHealTargetTbl = {}
	self.m_TotallHealTbl = {}
	self.m_LastFightTotallHealTbl = {}
	self.m_TotallHurt = 0									--所有伤害总和
	self.m_LastFightTotallHurt = 0				--上次战斗所有伤害总和
	self.m_TotallHeal = 0
	self.m_LastFightTotallHeal = 0
	self.m_SelfHurtTargetItemWnd = {}
	self.m_LastFightSelfHurtTargetItemWnd = {}
	self.m_TotallHurtItemWnd = {}
	self.m_LastFightTotallHurtItemWnd = {}
	self.m_uFightTime = {}								--统计战斗时间{总时间,开始时间}
	self.m_uLastFightTime = {}						--上一次战斗时间{总时间,开始时间}
	
	self.m_bBeginStatistic = false				--统计开始
	self.m_eShowStatisticType = EShowStatisticType.Totall
	self.m_eShowHurtType = EShowHurtType.Hurt
end

function CStatisticFightInfoWnd:OnOpen(bShow)
	self:ShowWnd(bShow)
	if bShow then
		if self.m_eShowHurtType == EShowHurtType.Hurt then
			self.m_Personal_Wnd.m_TotallHurtTextWnd:SetWndText(GetStaticTextClient(4701))
			self.m_Totall_Wnd.m_TotallHurtTextWnd:SetWndText(GetStaticTextClient(4701))
			self.m_Totall_Wnd.m_DpsTextWnd:SetWndText(GetStaticTextClient(4702))
			self.m_Totall_Wnd.m_HurtPercentTextWnd:SetWndText(GetStaticTextClient(4703))
		elseif self.m_eShowHurtType == EShowHurtType.Heal then
			self.m_Personal_Wnd.m_TotallHurtTextWnd:SetWndText(GetStaticTextClient(4704))
			self.m_Totall_Wnd.m_TotallHurtTextWnd:SetWndText(GetStaticTextClient(4704))
			self.m_Totall_Wnd.m_DpsTextWnd:SetWndText(GetStaticTextClient(4705))
			self.m_Totall_Wnd.m_HurtPercentTextWnd:SetWndText(GetStaticTextClient(4706))
		end
		
		if( self.m_eShowStatisticType == EShowStatisticType.Totall ) then
			self:ShowTotalStatistic()
			self.m_Totall_Wnd:ShowWnd(true)
		elseif( self.m_eShowStatisticType == EShowStatisticType.LastTotall ) then
			self:ShowLastTotalStatistic()
			self.m_Totall_Wnd:ShowWnd(true)
		elseif( self.m_eShowStatisticType == EShowStatisticType.Personal ) then
			self:ShowPersonalStatistic()
			self.m_Personal_Wnd:ShowWnd(true)
		elseif( self.m_eShowStatisticType == EShowStatisticType.LastPersonal ) then
			self:ShowLastPersonalStatistic()
			self.m_Personal_Wnd:ShowWnd(true)
		end
	end
end

local function GetShowName(uObjID)
	local Character = CCharacterFollower_GetCharacterByID(uObjID)
	if Character==nil then
		return nil
	end
	local showname = ""
	local Type = Character:GetViewVariantHolder(ECodeTableIndex.eCTI_Static):GetUInt8("Attribute/Type")
	if Type == ECharacterType.Player then
		showname = Character.m_Properties:GetCharName()
	elseif Type == ECharacterType.Npc then
		if Character:GetRealName() == "" then
			local npcName = Character.m_Properties:GetCharName()
			showname = GetNpcDisplayName(npcName)
	  else
	   	showname = Character:GetRealName()
	  end
	end
	return showname
end

local function ShowStatisticInfo(StatisticInfoTbl, type, itemWnd)
	if itemWnd ~= nil then --已经存在
		itemWnd.m_Name:SetWndText(StatisticInfoTbl[1])
		itemWnd.m_TotallHurt:SetWndText(StatisticInfoTbl[2])
		if type == 1 then
			itemWnd.m_Num:SetWndText(StatisticInfoTbl[3])
		elseif type ==2 then
			itemWnd.m_Dps:SetWndText(StatisticInfoTbl[3])
			itemWnd.m_HurtPercent:SetWndText(StatisticInfoTbl[4])
		end
		return
	end
end

local function InsertStatisticInfo(ListWnd, StatisticInfoTbl, type, itemWndTbl)
	local ItemCount = ListWnd:GetItemCount()
	ListWnd:InsertItem(ItemCount, 20)
	local Item = ListWnd:GetSubItem(ItemCount, 0)
	if type == 1 then
		local itemWnd = CFIPersonalItemWnd:new()
		itemWnd:CreateFromRes("WndSFIPersonalItem", Item)
		itemWnd:ShowWnd( true )
		itemWnd.m_Name 			= itemWnd:GetDlgChild("Name")
		itemWnd.m_TotallHurt = itemWnd:GetDlgChild("TotalHurt")
		itemWnd.m_Num	 			= itemWnd:GetDlgChild("Num")
		itemWnd.m_Name:SetWndText(StatisticInfoTbl[1])
		itemWnd.m_TotallHurt:SetWndText(StatisticInfoTbl[2])
		itemWnd.m_Num:SetWndText(StatisticInfoTbl[3])
		table.insert(itemWndTbl,itemWnd)
	elseif type == 2 then
		local itemWnd = CFITotalItemWnd:new()
		itemWnd:CreateFromRes("WndSFITotallItem", Item)
		itemWnd:ShowWnd( true )
		itemWnd.m_Name 				= itemWnd:GetDlgChild("Name")
		itemWnd.m_TotallHurt 	= itemWnd:GetDlgChild("TotalHurt")
		itemWnd.m_Dps	 				= itemWnd:GetDlgChild("Dps")
		itemWnd.m_HurtPercent	= itemWnd:GetDlgChild("HurtPercent")
		itemWnd.m_Name:SetWndText(StatisticInfoTbl[1])
		itemWnd.m_TotallHurt:SetWndText(StatisticInfoTbl[2])
		itemWnd.m_Dps:SetWndText(StatisticInfoTbl[3])
		itemWnd.m_HurtPercent:SetWndText(StatisticInfoTbl[4])
		table.insert(itemWndTbl,itemWnd)
	end
	Item:SetMouseWheel(true)
	if not ListWnd:IsShow() then
		ListWnd:ShowWnd(true)
	end
end

function CStatisticFightInfoWnd:ClearStatisticFightInfoByType(Type)
	if Type == EShowStatisticType.Totall then
		self.m_Totall_Wnd.m_HurtList:DeleteAllItem()
		self.m_Totall_Wnd.m_HurtList:InsertColumn(0, self.m_Totall_Wnd.m_HurtList:GetWndOrgWidth())
		self.m_TotallHurtItemWnd = {}
	elseif Type == EShowStatisticType.LastTotall then
		self.m_Totall_Wnd.m_HurtList:DeleteAllItem()
		self.m_Totall_Wnd.m_HurtList:InsertColumn(0, self.m_Totall_Wnd.m_HurtList:GetWndOrgWidth())
		self.m_LastFightTotallHurtItemWnd = {}
	elseif Type == EShowStatisticType.Personal then
		self.m_Personal_Wnd.m_HurtList:DeleteAllItem()
		self.m_Personal_Wnd.m_HurtList:InsertColumn(0, self.m_Personal_Wnd.m_HurtList:GetWndOrgWidth())
		self.m_SelfHurtTargetItemWnd = {}
	elseif Type == EShowStatisticType.LastPersonal then
		self.m_Personal_Wnd.m_HurtList:DeleteAllItem()
		self.m_Personal_Wnd.m_HurtList:InsertColumn(0, self.m_Personal_Wnd.m_HurtList:GetWndOrgWidth())
		self.m_LastFightSelfHurtTargetItemWnd = {}
	end
end

function CStatisticFightInfoWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if( Child == self.m_Total_Btn and self.m_eShowStatisticType ~= EShowStatisticType.Totall ) then
			self.m_Personal_Wnd:ShowWnd(false)
			self.m_eShowStatisticType = EShowStatisticType.Totall
			self:ClearStatisticFightInfoByType(EShowStatisticType.Totall)
			self:ShowTotalStatistic()
			self.m_Totall_Wnd:ShowWnd(true)
		elseif( Child == self.m_LastTotall_Btn and self.m_eShowStatisticType ~= EShowStatisticType.LastTotall ) then
			self.m_Personal_Wnd:ShowWnd(false)
			self.m_eShowStatisticType = EShowStatisticType.LastTotall
			self:ClearStatisticFightInfoByType(EShowStatisticType.LastTotall)
			self:ShowLastTotalStatistic()
			self.m_Totall_Wnd:ShowWnd(true)
		elseif( Child == self.m_Personal_Btn and self.m_eShowStatisticType ~= EShowStatisticType.Personal ) then
			self.m_Totall_Wnd:ShowWnd(false)
			self.m_eShowStatisticType = EShowStatisticType.Personal
			self:ClearStatisticFightInfoByType(EShowStatisticType.Personal)
			self:ShowPersonalStatistic()
			self.m_Personal_Wnd:ShowWnd(true)
		elseif( Child == self.m_LastPersonal_Btn and self.m_eShowStatisticType ~= EShowStatisticType.LastPersonal ) then
			self.m_Totall_Wnd:ShowWnd(false)
			self.m_eShowStatisticType = EShowStatisticType.LastPersonal
			self:ClearStatisticFightInfoByType(EShowStatisticType.LastPersonal)
			self:ShowLastPersonalStatistic()
			self.m_Personal_Wnd:ShowWnd(true)
		elseif( Child == self.m_Begin_Btn ) then
			self:BeginStatisticFightInfo()
			if g_MainPlayer:CppGetCtrlState(EFighterCtrlState.eFCS_InBattle) then
				self:BeginStatisticFightTime(GetShowName(g_MainPlayer:GetEntityID()))
			end
		elseif( Child == self.m_End_Btn ) then
			self:EndStatisticFightInfo()
		elseif( Child == self.m_Clear_Btn ) then
			local bStatistic = self:IsStatisticFightInfo()
			self:EndStatisticFightInfo()
			self:ClearAllStatisticFightInfo()
			if bStatistic then
				self:BeginStatisticFightInfo()
			end
			if g_MainPlayer:CppGetCtrlState(EFighterCtrlState.eFCS_InBattle) then
				self:BeginStatisticFightTime(GetShowName(g_MainPlayer:GetEntityID()))
			end
		elseif( Child == self.m_Left_Btn ) then
			if self.m_eShowHurtType == EShowHurtType.Heal then
				self.m_eShowHurtType = EShowHurtType.Hurt
				self:ClearStatisticFightInfoByType(self.m_eShowStatisticType)
				self:OnOpen(self:IsShow())
			end
		elseif( Child == self.m_Right_Btn ) then
			if self.m_eShowHurtType == EShowHurtType.Hurt then
				self.m_eShowHurtType = EShowHurtType.Heal
				self:ClearStatisticFightInfoByType(self.m_eShowStatisticType)
				self:OnOpen(self:IsShow())
			end
		elseif( Child == self.m_Close_Btn ) then
			self:ShowWnd(false)
		end
	end
end

function CStatisticFightInfoWnd:IsStatisticFightInfo()
	return self.m_bBeginStatistic
end

local function StatisticTick(Tick, Wnd)
	Wnd:OnStatisticTick()
end

function CStatisticFightInfoWnd:BeginStatisticFightTime(Name)
	if Name == nil or Name == "" then
		return
	end
	if self.m_uFightTime[Name] then
		if self.m_uFightTime[Name][2] ~= 0 then --已经开始统计
			return
		end
		self.m_uFightTime[Name][2] = GetProcessTime()
	else
		self.m_uFightTime[Name] = {0,0}
		self.m_uFightTime[Name][2] = GetProcessTime()
	end
end

function CStatisticFightInfoWnd:BeginStatisticFightInfo(Name)
	if self.m_pStatisticTick then
		return
	end
	self.m_bBeginStatistic = true
	self.m_pStatisticTick = RegisterTick( "FightStatisticTick", StatisticTick, 1000,self )
end

function CStatisticFightInfoWnd:PauseStatisticFightInfo(Name) --离开战斗,暂停计时
	if Name and self.m_uFightTime[Name] then
		self.m_uFightTime[Name][1] = self.m_uFightTime[Name][1] + (GetProcessTime() - self.m_uFightTime[Name][2])/1000
		self.m_uFightTime[Name][2] = 0
	else
		for k, v in pairs(self.m_uFightTime) do
			if v[2] ~= 0 then
				v[1] = v[1] + (GetProcessTime() - v[2])/1000
			end
			v[2] = 0
		end
	end
--	self.m_uFightTime = self.m_uFightTime + (GetProcessTime() - self.m_uBeginStatisticTime)/1000
--	self.m_uBeginStatisticTime = 0
end

function CStatisticFightInfoWnd:ClearAllStatisticFightInfo()
	self.m_Totall_Wnd.m_HurtList:DeleteAllItem()
	self.m_Totall_Wnd.m_HurtList:InsertColumn(0, self.m_Totall_Wnd.m_HurtList:GetWndOrgWidth())
	self.m_TotallHurtItemWnd = {}
	self.m_LastFightTotallHurtItemWnd = {}

	self.m_Personal_Wnd.m_HurtList:DeleteAllItem()
	self.m_Personal_Wnd.m_HurtList:InsertColumn(0, self.m_Personal_Wnd.m_HurtList:GetWndOrgWidth())
	self.m_SelfHurtTargetItemWnd = {}
	self.m_LastFightSelfHurtTargetItemWnd = {}
	
	self.m_SelfHurtTargetTbl = {}
	self.m_TotallHurtTbl = {}
	self.m_SelfHealTargetTbl = {}
	self.m_TotallHealTbl = {}
	self.m_TotallHurt = 0
	for k, v in pairs(self.m_uFightTime) do--清空时,统计战斗时间清零
		v[1] = 0
		v[2] = 0
	end
	self.m_LastFightSelfHurtTargetTbl = {}
	self.m_LastFightTotallHurtTbl = {}
	self.m_LastFightSelfHealTargetTbl = {}
	self.m_LastFightTotallHealTbl = {}
	self.m_LastFightTotallHurt = 0
	for k, v in pairs(self.m_uLastFightTime) do
		v[1] = 0
		v[2] = 0
	end
	if g_MainPlayer:CppGetCtrlState(EFighterCtrlState.eFCS_InBattle) then
		self:BeginStatisticFightTime(GetShowName(g_MainPlayer:GetEntityID()))
		self:ResetLastFightBeginData(GetShowName(g_MainPlayer:GetEntityID()))
	end
end

function CStatisticFightInfoWnd:EndStatisticFightInfo()
	self:PauseStatisticFightInfo()
	self.m_bBeginStatistic = false
	UnRegisterTick(self.m_pStatisticTick)
	self.m_pStatisticTick = nil
end

function CStatisticFightInfoWnd:ResetLastFightBeginData(Name)
	self.m_uLastFightTime[Name][2] = GetProcessTime()
	if self.m_eShowStatisticType == EShowStatisticType.LastPersonal then
		self:ClearStatisticFightInfoByType(EShowStatisticType.LastPersonal)
	elseif self.m_eShowStatisticType == EShowStatisticType.LastTotall then
		self:ClearStatisticFightInfoByType(EShowStatisticType.LastTotall)
	end
	self.m_LastFightSelfHurtTargetTbl = {}
	self.m_LastFightTotallHurtTbl = {}
	self.m_LastFightSelfHealTargetTbl = {}
	self.m_LastFightTotallHealTbl = {}
	self.m_LastFightTotallHurt = 0
	self.m_uLastFightTime[Name][1] = 0	--重新进入战斗时间清零
end

function CStatisticFightInfoWnd:OnBattleStateChanged(uEntityId, bEnterBattle)
	if uEntityId == 0 or uEntityId == nil or not IsCppBound(g_MainPlayer) then
		return
	end
	local Name = nil
	if uEntityId == g_MainPlayer:GetEntityID() then
		Name = GetShowName(uEntityId)
	elseif(g_GameMain.m_TeamBase:IsTeamMember(uEntityId)) then
		Name = g_GameMain.m_TeamBase:GetMemberNameByEntityId(uEntityId)
	end
	if not Name then
		return
	end
	
	if self:IsStatisticFightInfo() then
		if self.m_uFightTime[Name] then
			if bEnterBattle and self.m_uFightTime[Name][2] == 0 then
				self:BeginStatisticFightTime(Name)
			elseif not bEnterBattle and self.m_uFightTime[Name][2] ~= 0 then
				self:PauseStatisticFightInfo(Name)
			end
		else
			if bEnterBattle then
				self.m_uFightTime[Name] = {0,0}
				self:BeginStatisticFightTime(Name)
			end
		end
	end
	if self.m_uLastFightTime[Name] then
		if bEnterBattle and self.m_uLastFightTime[Name][2] == 0 then
			self:ResetLastFightBeginData(Name)
		elseif not bEnterBattle and self.m_uLastFightTime[Name][2] ~= 0 then
			self:UpLastFightTime()
			self.m_uLastFightTime[Name][2] = 0
		end
	else
		if bEnterBattle then
			self.m_uLastFightTime[Name] = {0,0}
			self:ResetLastFightBeginData(Name)
		end
	end
end

function CStatisticFightInfoWnd:OnEnterNewTeam()
	--自己进入另一个队伍时提示是否清空统计
	if self.m_ShowMsgBox then
		return
	end
	local function ClearStatisticInfo(g_GameMain,uButton)
		if uButton == MB_BtnOK then
			self:EndStatisticFightInfo()
			self:ClearAllStatisticFightInfo()
			self:BeginStatisticFightInfo()
			if g_MainPlayer:CppGetCtrlState(EFighterCtrlState.eFCS_InBattle) then
				self:BeginStatisticFightTime(GetShowName(g_MainPlayer:GetEntityID()))
			end
		end
		self.m_ShowMsgBox = false
		g_GameMain.m_MsgBox = nil
		return true
	end
	g_GameMain.m_MsgBox = MessageBox(g_GameMain, GetStaticTextClient(4700), BitOr( MB_BtnOK, MB_BtnCancel), ClearStatisticInfo, g_GameMain)
	self.m_ShowMsgBox = true
end

function CStatisticFightInfoWnd:InitStatisticFunc()
	self._m_Func = {}
	self._m_Func[EFightInfoIndex.EFII_Hurt] 						= self.StatisticHurt		--"伤害"
	self._m_Func[EFightInfoIndex.EFII_StrikeHurt] 			= self.StatisticHurt		--"伤害暴击"
	self._m_Func[EFightInfoIndex.EFII_Miss] 						= self.StatisticHurt		--"Miss躲避"
	self._m_Func[EFightInfoIndex.EFII_Resist] 					= self.StatisticHurt 		--"抵抗"
	self._m_Func[EFightInfoIndex.EFII_Parry] 						= self.StatisticHurt		--"招架"
	self._m_Func[EFightInfoIndex.EFII_Dodge]						= self.StatisticHurt		--"闪避"
	self._m_Func[EFightInfoIndex.EFII_SetUpState] 			= nil										--"安装状态"
	self._m_Func[EFightInfoIndex.EFII_ClearState] 			= nil										--"撤销状态"
	self._m_Func[EFightInfoIndex.EFII_Block] 						= self.StatisticHurt		--"格挡"
	self._m_Func[EFightInfoIndex.EFII_ComResist] 				= self.StatisticHurt		--"完全抵抗"
	self._m_Func[EFightInfoIndex.EFII_Immune] 					= self.StatisticHurt		--"免疫"
	self._m_Func[EFightInfoIndex.EFII_NonTypeAttackHit] = self.StatisticHurt		--"伤害(无类型)"
	self._m_Func[EFightInfoIndex.EFII_StateEffectHit]		= self.StatisticHurt  	--"伤害被吸收"
	self._m_Func[EFightInfoIndex.EFII_MultiAbsorb]			= self.StatisticHurt    --"被多种原因吸收(格挡|抵抗+伤害变更)"
	self._m_Func[EFightInfoIndex.EFII_Heal] 						= self.StatisticHeal		--"治疗"
	self._m_Func[EFightInfoIndex.EFII_StrikeHeal] 			= self.StatisticHeal  	--"治疗暴击"
	self._m_Func[EFightInfoIndex.EFII_DoSkill] 					= nil						 				--"施放技能:目标"
	self._m_Func[EFightInfoIndex.EFII_DoPosSkill]				= nil										--"施放技能:地点"
	self._m_Func[EFightInfoIndex.EFII_RenewMP] 					= nil						     	 	--"获得魔法值"
	self._m_Func[EFightInfoIndex.EFII_ConsumeMP]				= nil						     	 	--"消耗魔法值"
	self._m_Func[EFightInfoIndex.EFII_LostMP] 					= nil						       	--"吸收魔法值"
	self._m_Func[EFightInfoIndex.EFII_ConsumeHP]				= nil						       	--"消耗生命值"
	self._m_Func[EFightInfoIndex.EFII_SuccessDoCasting] = nil						    		--"流程结束"
	self._m_Func[EFightInfoIndex.EFII_InterruptCasting] = nil						    		--"流程中断"
	self._m_Func[EFightInfoIndex.EFII_RenewRP] 					= nil						       	--"获得怒气值"
	self._m_Func[EFightInfoIndex.EFII_Die] 							= self.StatisticDie     --"死亡"
	self._m_Func[EFightInfoIndex.EFII_ConsumeRP] 				= nil						       	--"消耗怒气值"
	self._m_Func[EFightInfoIndex.EFII_AddBurstSoulTimes]= nil						  			--"获得经验"
	self._m_Func[EFightInfoIndex.EFII_LostRP] 					= nil										--"流失怒气"
	self._m_Func[EFightInfoIndex.EFII_StateEffectStrikeHit] = self.StatisticHurt--"暴击伤害被吸收"
end

function CStatisticFightInfoWnd:FlushStatisticFightInfo()
	--伤害目标列表
	if not self:IsShow() then
		return
	end
	if self.m_eShowStatisticType == EShowStatisticType.Totall then
		self:ShowTotalStatistic()
	elseif self.m_eShowStatisticType == EShowStatisticType.Personal then
		self:ShowPersonalStatistic()
	end
	--技能频率列表
--	for k, v in pairs(self.m_SkillHurtTargetTextTbl) do
--		if type(v) == "table" then
--			Update(k, v[1],v[2],v[3],v[4],v[5],v[6],string.format("%d", v[6]/uAllTime))
--		end
--	end
	--伤害我的目标列表
--	for k, v in pairs(self.m_AllHurtSelfTextTbl) do
--		if type(v) == "table" then
--			Update(k, v[1], string.format("%d", v[1]/uAllTime))
--		end
--	end
	--伤害我的技能频率列表
--	for k, v in pairs(self.m_SkillHurtSelfTextTbl) do
--		if type(v) == "table" then
--			Update(k, v[1],v[2],v[3],v[4],v[5],v[6],string.format("%d", v[6]/uAllTime))
--		end
--	end
end

function CStatisticFightInfoWnd:FlushLastFightStatistic()
	if not self:IsShow() then
		return
	end
	if self.m_eShowStatisticType == EShowStatisticType.LastTotall then
		self:ShowLastTotalStatistic()
	elseif self.m_eShowStatisticType == EShowStatisticType.LastPersonal then
		self:ShowLastPersonalStatistic()
	end
end

function CStatisticFightInfoWnd:OnStatisticTick()
	if not self:IsShow() then
		return
	end
	if self.m_eShowStatisticType == EShowStatisticType.Totall then
		self:ShowTotalStatistic()
	elseif self.m_eShowStatisticType == EShowStatisticType.LastTotall then
		self:ShowLastTotalStatistic()
	end
end

function CStatisticFightInfoWnd:ShowTotalStatistic()
	if self.m_eShowHurtType == EShowHurtType.Hurt then
		self:ShowTotalStatisticHPChange(self.m_TotallHurtTbl,self.m_TotallHurt)
	elseif self.m_eShowHurtType == EShowHurtType.Heal then
		self:ShowTotalStatisticHPChange(self.m_TotallHealTbl,self.m_TotallHeal)
	end
end

function CStatisticFightInfoWnd:ShowLastTotalStatistic()
	if self.m_eShowHurtType == EShowHurtType.Hurt then
		self:ShowLastTotalStatisticHPChange(self.m_LastFightTotallHurtTbl,self.m_LastFightTotallHurt)
	elseif self.m_eShowHurtType == EShowHurtType.Heal then
		self:ShowLastTotalStatisticHPChange(self.m_LastFightTotallHealTbl,self.m_LastFightTotallHeal)
	end
end

function CStatisticFightInfoWnd:ShowPersonalStatistic()
	if self.m_eShowHurtType == EShowHurtType.Hurt then
		self:ShowPersonalStatisticHPChange(self.m_SelfHurtTargetTbl)
	elseif self.m_eShowHurtType == EShowHurtType.Heal then
		self:ShowPersonalStatisticHPChange(self.m_SelfHealTargetTbl)
	end
end

function CStatisticFightInfoWnd:ShowLastPersonalStatistic()
	if self.m_eShowHurtType == EShowHurtType.Hurt then
		self:ShowLastPersonalStatisticHPChange(self.m_LastFightSelfHurtTargetTbl)
	elseif self.m_eShowHurtType == EShowHurtType.Heal then
		self:ShowLastPersonalStatisticHPChange(self.m_LastFightSelfHealTargetTbl)
	end
end

function CStatisticFightInfoWnd:ShowTotalStatisticHPChange(HPChangeTbl,TotallHPChange)
	local ItemCount = self.m_Totall_Wnd.m_HurtList:GetItemCount()
	for i=1, #(HPChangeTbl) do
		local name = HPChangeTbl[i][1]
		if not self.m_uFightTime[name] then
			return
		end
		local uFightTime = 0
		if self:IsStatisticFightInfo() and self.m_uFightTime[name][2] ~= 0 then
			uFightTime = (GetProcessTime() - self.m_uFightTime[name][2])/1000
		end
		local uAllTime = uFightTime + self.m_uFightTime[name][1]
		if uAllTime < 1 then
			uAllTime = 1
		end
		local persent = string.format("%.0f", HPChangeTbl[i][2]*100/TotallHPChange).."%"
		local Dps = string.format("%.1f", HPChangeTbl[i][2]/uAllTime)
		if ItemCount < i then
			InsertStatisticInfo(self.m_Totall_Wnd.m_HurtList,{HPChangeTbl[i][1],HPChangeTbl[i][2],Dps,persent},2,self.m_TotallHurtItemWnd)
		else
			ShowStatisticInfo({HPChangeTbl[i][1],HPChangeTbl[i][2],Dps,persent},2,self.m_TotallHurtItemWnd[i])
		end
	end
end

function CStatisticFightInfoWnd:UpLastFightTime()
	for k, v in pairs(self.m_uLastFightTime) do
		if v[2] ~= 0 then
			v[1] = (GetProcessTime() - v[2])/1000
			--print(k,v[1],GetProcessTime(),v[2])
		end
		if v[1] < 1 then
			v[1] = 1
		end
	end
end

function CStatisticFightInfoWnd:ShowLastTotalStatisticHPChange(LastFightHPChangeTbl,LastFightTotallHPChange)
	self:UpLastFightTime()
	local ItemCount = self.m_Totall_Wnd.m_HurtList:GetItemCount()
	for i=1, #(LastFightHPChangeTbl) do
		local name = LastFightHPChangeTbl[i][1]
		if not self.m_uLastFightTime[name] or self.m_uLastFightTime[name][1] == 0 then
			return
		end
		local hurt = LastFightHPChangeTbl[i][2]
		local persent = string.format("%.0f", hurt*100/LastFightTotallHPChange).."%"
		local Dps = string.format("%.1f", hurt/self.m_uLastFightTime[name][1])
		if ItemCount < i then
			InsertStatisticInfo(self.m_Totall_Wnd.m_HurtList,{LastFightHPChangeTbl[i][1],hurt,Dps,persent},2,self.m_LastFightTotallHurtItemWnd)
		else
			ShowStatisticInfo({LastFightHPChangeTbl[i][1],hurt,Dps,persent},2,self.m_LastFightTotallHurtItemWnd[i])
		end
	end
end

function CStatisticFightInfoWnd:ShowPersonalStatisticHPChange(SelfHPChangeTbl)
	local ItemCount = self.m_Personal_Wnd.m_HurtList:GetItemCount()
	for i=1, #(SelfHPChangeTbl) do
		if ItemCount < i then
			InsertStatisticInfo(self.m_Personal_Wnd.m_HurtList,SelfHPChangeTbl[i],1,self.m_SelfHurtTargetItemWnd)
		else
			ShowStatisticInfo(SelfHPChangeTbl[i],1,self.m_SelfHurtTargetItemWnd[i])
		end
	end
end

function CStatisticFightInfoWnd:ShowLastPersonalStatisticHPChange(LastFightSelfHPChangeTbl)
	--上次战斗中伤害目标列表
	self:UpLastFightTime()
	local ItemCount = self.m_Personal_Wnd.m_HurtList:GetItemCount()
	for i=1, #(LastFightSelfHPChangeTbl) do
		if ItemCount < i then
			InsertStatisticInfo(self.m_Personal_Wnd.m_HurtList,LastFightSelfHPChangeTbl[i],1,self.m_LastFightSelfHurtTargetItemWnd)
		else
			ShowStatisticInfo(LastFightSelfHPChangeTbl[i],1,self.m_LastFightSelfHurtTargetItemWnd[i])
		end
	end
end

local function FindHurtTbl(HurtTbl, Name)
	for i=1, #(HurtTbl) do
		if HurtTbl[i][1] == Name then
			return HurtTbl[i]
		end
	end
	return nil
end

function CStatisticFightInfoWnd:UpDateTotallHPChange(HPChangeTbl, AttackterShowName,uHPChanged)
--	if self.m_uFightTime[AttackterShowName] == nil or self.m_uFightTime[AttackterShowName][2] == 0 then
--		return
--	end
	local Tbl = FindHurtTbl(HPChangeTbl, AttackterShowName)
	if Tbl == nil then
		table.insert(HPChangeTbl,{AttackterShowName,uHPChanged})
	else
		Tbl[2] = Tbl[2] + uHPChanged
	end
	table.sort(HPChangeTbl,function(a,b) return(a[2]>b[2])end)
end

function CStatisticFightInfoWnd:UpDateLastFightTotallHPChange(HPChangeTbl, AttackterShowName,uHPChanged)
	if self.m_uLastFightTime[AttackterShowName] == nil or self.m_uLastFightTime[AttackterShowName][2] == 0 then
		return
	end
	local Tbl = FindHurtTbl(HPChangeTbl, AttackterShowName)
	if Tbl == nil then
		table.insert(HPChangeTbl,{AttackterShowName,uHPChanged})
	else
		Tbl[2] = Tbl[2] + uHPChanged
	end
	table.sort(HPChangeTbl,function(a,b) return(a[2]>b[2])end)
end

function CStatisticFightInfoWnd:UpDateLastFightSelfHPChange(HPChangeTbl, TargetShowName,uHPChanged)
	local selfName = g_MainPlayer.m_Properties:GetCharName()
	if self.m_uLastFightTime[selfName] == nil or self.m_uLastFightTime[selfName][2] == 0 then
		return
	end
	local Tbl = FindHurtTbl(HPChangeTbl, TargetShowName)
	if Tbl == nil then
		table.insert(HPChangeTbl,{TargetShowName,uHPChanged,0})
	else
		Tbl[2] = Tbl[2] + uHPChanged
	end
	table.sort(HPChangeTbl,function(a,b) return(a[2]>b[2])end)
end

function CStatisticFightInfoWnd:UpDateFightSelfHPChange(HPChangeTbl, TargetShowName,uHPChanged)
	local selfName = g_MainPlayer.m_Properties:GetCharName()
	if self.m_uFightTime[selfName] == nil or self.m_uFightTime[selfName][2] == 0 then
		return
	end
	local Tbl = FindHurtTbl(HPChangeTbl, TargetShowName)
	if Tbl == nil then
		table.insert(HPChangeTbl,{TargetShowName,uHPChanged,0})
	else
		Tbl[2] = Tbl[2] + uHPChanged
	end
	table.sort(HPChangeTbl,function(a,b) return(a[2]>b[2])end)
end

function CStatisticFightInfoWnd:StatisticHurt(uObj1ID, uObj2ID, szSkillName, 
									uHPChanged, uSelfID, uObj1MasterID)
	if uHPChanged == 0 then
		return
	end
	if uSelfID == uObj1ID or uSelfID == uObj1MasterID then
		--伤害目标列表
		local AttackterShowName = GetShowName(uSelfID)
		local TargetShowName = GetShowName(uObj2ID)
		if self:IsStatisticFightInfo() then
			if TargetShowName then
				self:UpDateFightSelfHPChange(self.m_SelfHurtTargetTbl, TargetShowName,uHPChanged)
			end
			self:UpDateTotallHPChange(self.m_TotallHurtTbl,AttackterShowName,uHPChanged)
			self.m_TotallHurt = self.m_TotallHurt + uHPChanged
			self:FlushStatisticFightInfo()
		end
		if TargetShowName then
			self:UpDateLastFightSelfHPChange(self.m_LastFightSelfHurtTargetTbl,TargetShowName,uHPChanged)
		end
		self:UpDateLastFightTotallHPChange(self.m_LastFightTotallHurtTbl,AttackterShowName,uHPChanged)
		self.m_LastFightTotallHurt = self.m_LastFightTotallHurt+uHPChanged
		self:FlushLastFightStatistic()
	elseif g_GameMain.m_TeamBase:IsTeamMember(uObj1ID) then
		self:StatisticTeamMemberHurt(uObj1ID,uHPChanged)
	elseif g_GameMain.m_TeamBase:IsTeamMember(uObj1MasterID) then
		self:StatisticTeamMemberHurt(uObj1MasterID,uHPChanged)
	end
end

function CStatisticFightInfoWnd:StatisticTeamMemberHurt(TeamMemberID, uHPChanged)
	local TeamMemberName = g_GameMain.m_TeamBase:GetMemberNameByEntityId(TeamMemberID)
	if not TeamMemberName then
		return
	end
	if self:IsStatisticFightInfo() then
		self:UpDateTotallHPChange(self.m_TotallHurtTbl,TeamMemberName,uHPChanged)
		self.m_TotallHurt = self.m_TotallHurt + uHPChanged
		self:FlushStatisticFightInfo()
	end
	self:UpDateLastFightTotallHPChange(self.m_LastFightTotallHurtTbl,TeamMemberName,uHPChanged)
	self.m_LastFightTotallHurt = self.m_LastFightTotallHurt+uHPChanged
	self:FlushLastFightStatistic()
end

function CStatisticFightInfoWnd:StatisticDie(uObj1ID, uObj2ID, szSkillName, 
									uHPChanged, uSelfID, uObj1MasterID)
	if uSelfID == uObj1ID or uSelfID == uObj1MasterID then
		--伤害目标列表
		local AttackterShowName = GetShowName(uSelfID)
		local TargetShowName = GetShowName(uObj2ID)
		if not TargetShowName then
			return
		end
		if self:IsStatisticFightInfo() then
			local Tbl = FindHurtTbl(self.m_SelfHurtTargetTbl, TargetShowName)
			if Tbl == nil then
				table.insert(self.m_SelfHurtTargetTbl,{TargetShowName,0,1})
			else
				Tbl[3] = Tbl[3] + 1
			end
			self:FlushStatisticFightInfo()
		end
		local Tbl = FindHurtTbl(self.m_LastFightSelfHurtTargetTbl, TargetShowName)
		if Tbl == nil then
			table.insert(self.m_LastFightSelfHurtTargetTbl,{TargetShowName,0,1})
		else
			Tbl[3] = Tbl[3] + 1
		end
		self:FlushLastFightStatistic()
	end
end

function CStatisticFightInfoWnd:StatisticHeal(uObj1ID, uObj2ID, szSkillName, 
									uHPChanged, uSelfID, uObj1MasterID)
	if uSelfID == uObj1ID or uSelfID == uObj1MasterID then
		--伤害目标列表
		local AttackterShowName = GetShowName(uSelfID)
		local TargetShowName = GetShowName(uObj2ID)
		if self:IsStatisticFightInfo() then
			if TargetShowName then
				self:UpDateFightSelfHPChange(self.m_SelfHealTargetTbl,TargetShowName,uHPChanged)
			end
			self:UpDateTotallHPChange(self.m_TotallHealTbl,AttackterShowName,uHPChanged)
			self.m_TotallHeal = self.m_TotallHeal + uHPChanged
			self:FlushStatisticFightInfo()
		end
		if TargetShowName then
			self:UpDateLastFightSelfHPChange(self.m_LastFightSelfHealTargetTbl,TargetShowName,uHPChanged)
		end
		self:UpDateLastFightTotallHPChange(self.m_LastFightTotallHealTbl,AttackterShowName,uHPChanged)
		self.m_LastFightTotallHeal = self.m_LastFightTotallHeal+uHPChanged
		self:FlushLastFightStatistic()
	elseif(g_GameMain.m_TeamBase:IsTeamMember(uObj1ID) or g_GameMain.m_TeamBase:IsTeamMember(uObj1MasterID)) then
		local TeamMemberName = g_GameMain.m_TeamBase:GetMemberNameByEntityId(uObj1ID)
		if not TeamMemberName then
			return
		end
		if self:IsStatisticFightInfo() then
			self:UpDateTotallHPChange(self.m_TotallHealTbl,TeamMemberName,uHPChanged)
			self.m_TotallHeal = self.m_TotallHeal + uHPChanged
			self:FlushStatisticFightInfo()
		end
		self:UpDateLastFightTotallHPChange(self.m_LastFightTotallHealTbl,TeamMemberName,uHPChanged)
		self.m_LastFightTotallHeal = self.m_LastFightTotallHeal+uHPChanged
		self:FlushLastFightStatistic()
	end
end

-- 战斗信息统计
function CStatisticFightInfoWnd:JointStatisticFightInfo( pFightInfoData )
	local eInfoIndex			= pFightInfoData:GetInfoIndex()
	local uObj1ID 				= pFightInfoData:GetObj1ID()
	local uObj2ID 			  = pFightInfoData:GetObj2ID()
	local szSkillName		  = pFightInfoData:GetSkillName()
	local uRealHPChanged 	= pFightInfoData:GetRealHPChanged()
	local uSelfID					= g_MainPlayer:GetEntityID()
	
	local Func = self._m_Func[eInfoIndex]
	if Func then
		uObj1MasterID = 0
		Obj1 = CCharacterFollower_GetCharacterByID(uObj1ID)
		if Obj1 ~= nil then
			uObj1MasterID = Obj1:GetMasterID()
		end
		Func(self, uObj1ID, uObj2ID, szSkillName, uRealHPChanged, uSelfID, uObj1MasterID)
	end
end
