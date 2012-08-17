lan_load "skill/Lan_SkillPart_Common"
lan_load "skill/Lan_SkillName_Client"

CFightInfoWnd = class()
local FightInfoMaxLen = 100

function CFightInfoWnd:Ctor(Parent)
	self:InitFunc()
	self.m_FightInfoTbl = {}
	self.m_FightInfoDataTbl = {}
	self.m_FlushFightInfoTick = nil
	self.m_FightInfoShow = false
end

function CFightInfoWnd.GetWnd()
	local Wnd = g_GameMain.m_FightInfoWnd
	if not Wnd then
		Wnd = CFightInfoWnd:new(g_GameMain)
		g_GameMain.m_FightInfoWnd = Wnd
	end
	return Wnd
end

function CFightInfoWnd:InitFunc()
	self._m_Func = {}
	self._m_Func[EFightInfoIndex.EFII_Hurt] 						= self.Hurt							--"伤害"
	self._m_Func[EFightInfoIndex.EFII_StrikeHurt] 			= self.StrikeHurt				--"伤害暴击"
	self._m_Func[EFightInfoIndex.EFII_Miss]							= self.Miss							--"Miss躲避"
	self._m_Func[EFightInfoIndex.EFII_Resist] 					= self.Resist		 				--"抵抗"
	self._m_Func[EFightInfoIndex.EFII_Parry] 						= self.Parry						--"招架"
	self._m_Func[EFightInfoIndex.EFII_Dodge] 						= self.Dodge						--"闪避"
	self._m_Func[EFightInfoIndex.EFII_SetUpState] 			= self.SetUpState				--"安装状态"
	self._m_Func[EFightInfoIndex.EFII_ClearState] 			= self.ClearState				--"撤销状态"
	self._m_Func[EFightInfoIndex.EFII_Block] 						= self.Block						--"格挡"
	self._m_Func[EFightInfoIndex.EFII_ComResist] 				= self.ComResist		 		--"完全抵抗"
	self._m_Func[EFightInfoIndex.EFII_Immune] 					= self.Immune						--"免疫"
	self._m_Func[EFightInfoIndex.EFII_NonTypeAttackHit] = self.NonTypeAttackHit --"伤害(无类型)"
	self._m_Func[EFightInfoIndex.EFII_StateEffectHit] 	= self.StateEffectHit	 	--"伤害被吸收"
	self._m_Func[EFightInfoIndex.EFII_MultiAbsorb]			= self.MultiAbsorb   		--"被多种吸收(格挡|抵抗+伤害变更)"
	self._m_Func[EFightInfoIndex.EFII_Heal] 						= self.Heal				   		--"治疗"
	self._m_Func[EFightInfoIndex.EFII_StrikeHeal] 			= self.StrikeHeal				--"治疗暴击"
	self._m_Func[EFightInfoIndex.EFII_DoSkill] 					= self.DoSkill		  		--"施放技能:目标"
	self._m_Func[EFightInfoIndex.EFII_DoPosSkill] 			= self.DoPosSkill		 		--"施放技能:地点"
	self._m_Func[EFightInfoIndex.EFII_RenewMP] 					= self.RenewMP					--"获得魔法值"
	self._m_Func[EFightInfoIndex.EFII_ConsumeMP]	 			= self.ConsumeMP	 	 		--"消耗魔法值"
	self._m_Func[EFightInfoIndex.EFII_LostMP] 					= self.LostMP			 	 		--"吸收魔法值"
	self._m_Func[EFightInfoIndex.EFII_ConsumeHP] 				= self.ConsumeHP	   		--"消耗生命值"
	self._m_Func[EFightInfoIndex.EFII_SuccessDoCasting] = self.SuccessDoCasting --"流程结束"
	self._m_Func[EFightInfoIndex.EFII_InterruptCasting] = self.InterruptCasting --"流程中断"
	self._m_Func[EFightInfoIndex.EFII_RenewRP] 					= self.RenewRP					--"获得怒气值"
	self._m_Func[EFightInfoIndex.EFII_Die] 							= self.Die							--"死亡"
	self._m_Func[EFightInfoIndex.EFII_ConsumeRP] 				= self.ConsumeRP	    	--"消耗怒气值"
	self._m_Func[EFightInfoIndex.EFII_AddBurstSoulTimes]= self.AddBurstSoulTimes--"增加爆魂蓄力点"
	self._m_Func[EFightInfoIndex.EFII_LostRP] 					= self.LostRP						--"流失怒气"
	self._m_Func[EFightInfoIndex.EFII_StateEffectStrikeHit]	= self.StateEffectStrikeHit	--"暴击伤害被吸收"
end

function CFightInfoWnd:IsHPChangeRelated(uInfoIndex)
	if uInfoIndex == EFightInfoIndex.EFII_Hurt or 
		uInfoIndex == EFightInfoIndex.EFII_StrikeHurt or 
		uInfoIndex == EFightInfoIndex.EFII_Resist or 
		uInfoIndex == EFightInfoIndex.EFII_Block or 
		uInfoIndex == EFightInfoIndex.EFII_NonTypeAttackHit or 
		uInfoIndex == EFightInfoIndex.EFII_StateEffectHit or 
		uInfoIndex == EFightInfoIndex.EFII_MultiAbsorb or 
		uInfoIndex == EFightInfoIndex.EFII_Heal or 
		uInfoIndex == EFightInfoIndex.EFII_StrikeHeal or
		uInfoIndex == EFightInfoIndex.EFII_StateEffectStrikeHit then
		return true
	end
	return false
end

function CFightInfoWnd:OnFightInfoShow( bShow )
	if bShow then
		self.m_FightInfoShow = true
		--self:FlushFightInfoData()
		if self.m_FlushFightInfoTick == nil then
			self.m_FlushFightInfoTick = RegClassTick( "FlushFightInfoTick", CFightInfoWnd.FlushOnTick , 330, self)
			self:FlushOnTick()
		end
	else
		self.m_FightInfoShow = false
		self:UnRegisterFightInfoTick()
	end
end

function CFightInfoWnd:FlushFightInfoData()
	for i=1,#(self.m_FightInfoDataTbl) do
		self:PrintFightInfo( self.m_FightInfoDataTbl[i] )
	end
	self.m_FightInfoDataTbl = {}
end

-- 战斗信息打印
function CFightInfoWnd:OnPrintFightInfo( pFightInfoData )
	local pFightInfoDataTbl = {}
	pFightInfoDataTbl["uInfoIndex"]	 		= pFightInfoData:GetInfoIndex()
	pFightInfoDataTbl["uObj1ID"] 				= pFightInfoData:GetObj1ID()
	pFightInfoDataTbl["uObj2ID"] 				= pFightInfoData:GetObj2ID()
	pFightInfoDataTbl["szSkillName"]		= pFightInfoData:GetSkillName()
	pFightInfoDataTbl["szStateName"] 		= pFightInfoData:GetStateName()
	pFightInfoDataTbl["szArgName"] 			= pFightInfoData:GetArgName()
	pFightInfoDataTbl["uHPChanged"]			= math.abs(pFightInfoData:GetHPChanged())
	pFightInfoDataTbl["uAttackType"]		= pFightInfoData:GetAttackType()
	pFightInfoDataTbl["uHPExtra"]				= pFightInfoData:GetHPExtra()
	pFightInfoDataTbl["uTimeHour"] 			= pFightInfoData:GetHour()
	pFightInfoDataTbl["uTimeMinute"] 		= pFightInfoData:GetMinute()
	pFightInfoDataTbl["uTimeSecond"] 		= pFightInfoData:GetSecond()
	pFightInfoDataTbl["uMillisecond"]		= pFightInfoData:GetMillisecond()
	self:PrintFightInfo( pFightInfoDataTbl )
end

function CFightInfoWnd:PrintFightInfo( pFightInfoDataTbl )
	--if self.m_FightInfoShow or g_GameMain.m_bSaveFightInfo then
		local FightInfo = self:JointFightInfo( pFightInfoDataTbl ) --测试下table.contact是否有速度提升
		if(FightInfo ~= "") then
			if #(self.m_FightInfoTbl) > 50 then
				table.remove(self.m_FightInfoTbl,1)
			end
			table.insert(self.m_FightInfoTbl,FightInfo)
			self:SaveFightInfo(FightInfo)
		end
--	else
--		if #(self.m_FightInfoDataTbl) > 50 then
--			table.remove(self.m_FightInfoDataTbl,1)
--		end
--		table.insert(self.m_FightInfoDataTbl,pFightInfoDataTbl)
--	end
end

function CFightInfoWnd:JointFightInfo( pFightInfoDataTbl )
	if g_MainPlayer == nil or not IsCppBound(g_MainPlayer) then
		return ""
	end
	local uInfoIndex				 = pFightInfoDataTbl["uInfoIndex"]
	local uObj1ID 					 = pFightInfoDataTbl["uObj1ID"]
	local uObj2ID 					 = pFightInfoDataTbl["uObj2ID"]
	local szSkillName				 = pFightInfoDataTbl["szSkillName"]
	local szStateName 			 = pFightInfoDataTbl["szStateName"]
	local szArgName 				 = pFightInfoDataTbl["szArgName"]
	local uHPChanged				 = pFightInfoDataTbl["uHPChanged"]
	local uAttackType				 = pFightInfoDataTbl["uAttackType"]
	local uHPExtra					 = pFightInfoDataTbl["uHPExtra"]
	local uTimeHour 				 = pFightInfoDataTbl["uTimeHour"]
	local uTimeMinute 			 = pFightInfoDataTbl["uTimeMinute"]
	local uTimeSecond 			 = pFightInfoDataTbl["uTimeSecond"]
	local uMillisecond			 = pFightInfoDataTbl["uMillisecond"]
	local uSelfID 					 = g_MainPlayer:GetEntityID()
	
	if(1 == g_GameMain.m_SceneStateForClient) then
		return ""
	end
	
	local Func = self._m_Func[uInfoIndex]
	if( Func == nil ) then
		return "战斗信息打印错误!"
	else
		uObj1MasterID = 0
		uObj2MasterID = 0
		
		Obj1 = CCharacterFollower_GetCharacterByID(uObj1ID)
		if Obj1 ~= nil then
			if Obj1:GetMaster() ~= nil then
				uObj1MasterID = Obj1:GetMaster():GetEntityID()
			end
		end
		
		Obj2 = CCharacterFollower_GetCharacterByID(uObj2ID)
		if Obj2 ~= nil then
			if Obj2:GetMaster() ~= nil then
				uObj2MasterID = Obj2:GetMaster():GetEntityID()
			end
		end
		local FightInfo = ""
		if uSelfID == uObj1ID or uSelfID == uObj1MasterID then
			FightInfo = Func(self, uObj1ID, uObj2ID, szSkillName, 
										szStateName, szArgName, uHPChanged, uAttackType, uHPExtra, false)
		elseif uSelfID == uObj2ID or uSelfID == uObj2MasterID then
			FightInfo = Func(self, uObj1ID, uObj2ID, szSkillName, 
										szStateName, szArgName, uHPChanged, uAttackType, uHPExtra, true)
		elseif(uInfoIndex == EFightInfoIndex.EFII_Die and g_GameMain.m_TeamBase:IsTeamMember(uObj2ID) ) then
			--Obj2是队友,死亡
			FightInfo = Func(self, uObj1ID, uObj2ID, szSkillName, 
										szStateName, szArgName, uHPChanged, uAttackType, uHPExtra, false)
		end
		if uTimeHour and uTimeMinute and uTimeSecond and FightInfo ~= "" then
			if uTimeHour < 10 then
				uTimeHour = "0"..uTimeHour
			end
			if uTimeMinute < 10 then
				uTimeMinute = "0"..uTimeMinute
			end
			if uTimeSecond < 10 then
				uTimeSecond = "0"..uTimeSecond
			end
			
			if g_GameMain.m_bExactFightInfo and uMillisecond then
				if uMillisecond <10 then
					uMillisecond = "00" .. uMillisecond
				elseif uMillisecond <100 then
					uMillisecond = "0" .. uMillisecond
				end
				FightInfo = GetStaticTextClient(4561).."["..uTimeHour..":"..uTimeMinute..":"..uTimeSecond..":"..uMillisecond.."]"..FightInfo
			else
				FightInfo = GetStaticTextClient(4561).."["..uTimeHour..":"..uTimeMinute..":"..uTimeSecond.."]"..FightInfo
			end
		end
		return FightInfo
	end
end

function CFightInfoWnd:FlushOnTick()
	local Wnd = g_GameMain.m_CreateChatWnd.m_CChatWnd
	local bFightInfoEmpty = true
	for i=1,#(self.m_FightInfoTbl) do
		table.insert(Wnd.ChatInfoTbl[3],self.m_FightInfoTbl[i])
		table.insert(Wnd.ChatInfoTbl[3],"#r")
		table.insert(Wnd.ChatInfoTbl[3],"")
		table.insert(Wnd.ChatInfoTbl[3],"")
		bFightInfoEmpty = false
	end
	self.m_FightInfoTbl = {}
	if not bFightInfoEmpty then
		Wnd:ChatInfoShow()
	end
end

function CFightInfoWnd:UnRegisterFightInfoTick()
	if self.m_FlushFightInfoTick then
		UnRegisterTick( self.m_FlushFightInfoTick )
		self.m_FlushFightInfoTick = nil
	end
end

function CFightInfoWnd:SaveFightInfo(fightInfo)
	if not g_GameMain.m_bSaveFightInfo then
		return
	end
	while true do
		local pos = string.find( fightInfo, "#", 0)
		if pos == nil then
			break
		end
		local sLength = string.len(fightInfo)
		s1 = string.sub(fightInfo, 0, pos-1)
		s2 = string.sub(fightInfo, pos+8, sLength)
		fightInfo = s1..s2
	end
	
	local strpath = g_RootPath .. "var/gac/FightInfo.txt"
	local f = CLuaIO_Open(strpath, "a")
	if f then
		f:WriteString(fightInfo.."\n")
		f:Close()
	end
end

local function GetCharacterName(uObjID)
	local showname = ""
	if g_MainPlayer and g_MainPlayer:GetEntityID() == uObjID then
		return GetStaticTextClient(4546)
	end
	local Character = CCharacterFollower_GetCharacterByID(uObjID)
	if Character==nil then
		return showname
	end
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

local function GetGiftSkillDisplayName(sTalentName)
	local classId = g_MainPlayer:CppGetClass()
	GiftSkill_Common = loadGeniusfile(classId)
	if GiftSkill_Common == nil or GiftSkill_Common(sTalentName) == nil then
		return nil
	end
	Lan_GiftSkill_Common = loadGeniusfile(classId,true)
	local Lan_GiftSkillName = Lan_GiftSkill_Common(MemH64(GiftSkill_Common(sTalentName,"SkillName")),"DisplayName")
	return Lan_GiftSkillName
end

local function GetSkillDisplayName(sSkillName)
	local Lan_skill = Lan_SkillPart_Common(MemH64(sSkillName))
	local Lan_npcskill = Lan_SkillName_Client(MemH64(sSkillName))
	if Lan_skill then 
		return  Lan_skill("DisplayName")
	elseif Lan_npcskill then
		return  Lan_npcskill("DisplayName")
	else
		local Lan_GiftSkillName = GetGiftSkillDisplayName(sSkillName)
		if Lan_GiftSkillName then
			return Lan_GiftSkillName
		else
			return ""
		end
	end
end

local function GetBaseFightInfo(charName,sSkillName,szStateName,szArgName) --GetSkillDisplayNameLinkDe
	local SkillName = ""
	local description,FightInfoType = GetBuffDisplayName(szStateName)
	if description ~= nil and FightInfoType == "" then
		SkillName = description
	else
		SkillName = GetSkillDisplayName(sSkillName)
		if not SkillName or SkillName == "" then
			SkillName = GetSkillDisplayName(szArgName)
		end
	end
	if charName ~= "" and SkillName ~= "" then
		SkillName = GetStaticTextClient(4502) .. charName ..GetStaticTextClient(4504)..SkillName
	else
		SkillName = GetStaticTextClient(4502) .. charName ..GetStaticTextClient(4566)..SkillName
	end
	return SkillName
end

local function IsItemSkill(uFromID,uToID,sSkillName)
	if uFromID == uToID then
		return true
	end
	--因为skillInst上的名字不一定是技能名，有可能是天赋名，所以GetSkillCoolDownType可能找不到技能而报错
	--暂时用上面的办法
--	local Character = CCharacterFollower_GetCharacterByID(upFromID)
--	if Character then
--		local Type = Character:GetViewVariantHolder():GetUInt8("Attribute/Type")
--		if Type == ECharacterType.Player then
--			--确保sSkillName~=""
--			local eCoolDownType = g_MainPlayer:GetSkillCoolDownType(sSkillName)
--			if eCoolDownType == ESkillCoolDownType.eSCDT_DrugItemSkill or eCoolDownType == ESkillCoolDownType.eSCDT_OtherItemSkill 
--				or eCoolDownType == ESkillCoolDownType.eSCDT_SpecialItemSkill then
--				return true
--			end
--		end
--	end
	return false
end

local function GetAttackType(uAttackType)
	local attackType = ""	
	if(uAttackType == EAttackType.eATT_None) then
		attackType = ""--GetStaticTextClient(4548)
	elseif(uAttackType == EAttackType.eATT_Puncture) then
		attackType = GetStaticTextClient(4549)
	elseif(uAttackType == EAttackType.eATT_Chop) then
		attackType = GetStaticTextClient(4549)
	elseif(uAttackType == EAttackType.eATT_BluntTrauma) then
		attackType = GetStaticTextClient(4549)
	elseif(uAttackType == EAttackType.eATT_Nature) then
		attackType = GetStaticTextClient(4552)
	elseif(uAttackType == EAttackType.eATT_Destroy) then
		attackType = GetStaticTextClient(4553)
	elseif(uAttackType == EAttackType.eATT_Evil) then
		attackType = GetStaticTextClient(4554)
	elseif(uAttackType == EAttackType.eATT_TaskNonBattle) then
		attackType = GetStaticTextClient(4555)
	elseif(uAttackType == EAttackType.eATT_TaskBomb) then
		attackType = GetStaticTextClient(4556)
	elseif(uAttackType == EAttackType.eATT_TaskSpecial) then
		attackType = GetStaticTextClient(4557)
	end
	return GetStaticTextClient(4558)..attackType
end


function CFightInfoWnd:Hurt(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	local charName = GetCharacterName(uObj1ID)
	local charNameTarget = GetCharacterName(uObj2ID)
	local sBaseFightInfo = GetBaseFightInfo(charName,szSkillName,szStateName,szArgName)
	
	local FightInfo = sBaseFightInfo ..GetStaticTextClient(4508).. charNameTarget  .. GetStaticTextClient(4505) .. uHPChanged .. GetAttackType(uAttackType) .. GetStaticTextClient(4506)
	return FightInfo
end

function CFightInfoWnd:StrikeHurt(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	local charName = GetCharacterName(uObj1ID)
	local charNameTarget = GetCharacterName(uObj2ID)
	local sBaseFightInfo = GetBaseFightInfo(charName,szSkillName,szStateName,szArgName)
	local FightInfo = ""
	if sBaseFightInfo then
		FightInfo = sBaseFightInfo .. GetStaticTextClient(4507).. GetStaticTextClient(4503).. charNameTarget .. GetStaticTextClient(4505) .. uHPChanged .. GetAttackType(uAttackType) .. GetStaticTextClient(4506)
	end
	return FightInfo
end

function CFightInfoWnd:Miss(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
		
	local charName = GetCharacterName(uObj1ID)
	local charNameTarget = GetCharacterName(uObj2ID)
	local sBaseFightInfo = GetBaseFightInfo(charName,szSkillName,szStateName,szArgName)
	local FightInfo = ""
	if sBaseFightInfo then
		FightInfo = sBaseFightInfo .. GetStaticTextClient(4509).. charNameTarget
	end
	return FightInfo
end

function CFightInfoWnd:Resist(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	local FightInfo = self:Hurt(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	if uHPExtra ~= 0 then
		FightInfo = FightInfo .. GetStaticTextClient(4513) .. uHPExtra .. GetStaticTextClient(4514) .. GetStaticTextClient(4568)
	end
	return FightInfo
end

function CFightInfoWnd:Parry(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	local charName = GetCharacterName(uObj1ID)
	local charNameTarget = GetCharacterName(uObj2ID)
	local sBaseFightInfo = GetBaseFightInfo(charName,szSkillName,szStateName,szArgName)
	local FightInfo = ""
	if sBaseFightInfo then
		FightInfo = sBaseFightInfo ..GetStaticTextClient(4520).. charNameTarget  .. GetStaticTextClient(4517)
	end
	return FightInfo
end

function CFightInfoWnd:Dodge(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	local charName = GetCharacterName(uObj1ID)
	local charNameTarget = GetCharacterName(uObj2ID)
	local sBaseFightInfo = GetBaseFightInfo(charName,szSkillName,szStateName,szArgName)
	local FightInfo = ""
	if sBaseFightInfo then
		FightInfo = sBaseFightInfo ..GetStaticTextClient(4520).. charNameTarget  .. GetStaticTextClient(4518)
	end
	return FightInfo
end

function CFightInfoWnd:SetUpState(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	local description,FightInfoType = GetBuffDisplayName(szArgName)
	local FightInfo = ""
	if description ~= nil then
		if FightInfoType == "姿态" then
			FightInfo = GetStaticTextClient(4502) .. GetStaticTextClient(4510) .. description --.. GetStaticTextClient(4523)
		elseif FightInfoType == "光环" then
			FightInfo = GetStaticTextClient(4502) .. GetStaticTextClient(4510) .. description .. GetStaticTextClient(4524)
		elseif FightInfoType == "领域" then
			FightInfo = GetStaticTextClient(4502) .. GetStaticTextClient(4525) .. description .. GetStaticTextClient(4527)
		else
			FightInfo = GetStaticTextClient(4502) .. GetStaticTextClient(4510) .. description .. GetStaticTextClient(4511)
		end
	end
	return FightInfo
end

function CFightInfoWnd:ClearState(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	local description,FightInfoType = GetBuffDisplayName(szArgName)
	local FightInfo = ""
	if description ~= nil then
		if FightInfoType == "姿态" then
			FightInfo = GetStaticTextClient(4502) .. GetStaticTextClient(4528) .. description --.. GetStaticTextClient(4523)
		elseif FightInfoType == "光环" then
			FightInfo = GetStaticTextClient(4502) .. GetStaticTextClient(4528) .. description .. GetStaticTextClient(4524)
		elseif FightInfoType == "领域" then
			FightInfo = GetStaticTextClient(4502) .. GetStaticTextClient(4526) .. description .. GetStaticTextClient(4527)
		else
			FightInfo = GetStaticTextClient(4502) .. description .. GetStaticTextClient(4512)
		end
	end
	return FightInfo
end

function CFightInfoWnd:Block(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	local FightInfo = self:Hurt(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	if uHPExtra ~= 0 then
		FightInfo = FightInfo .. GetStaticTextClient(4513) .. uHPExtra .. GetStaticTextClient(4516) .. GetStaticTextClient(4568)
	end
	return FightInfo
end

function CFightInfoWnd:ComResist(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	local charName = GetCharacterName(uObj1ID)
	local charNameTarget = GetCharacterName(uObj2ID)
	local sBaseFightInfo = GetBaseFightInfo(charName,szSkillName,szStateName,szArgName)
	local FightInfo = ""
	if sBaseFightInfo then
		FightInfo = sBaseFightInfo ..GetStaticTextClient(4520).. charNameTarget  .. GetStaticTextClient(4515)
	end
	return FightInfo
end

function CFightInfoWnd:Immune(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	local charName = GetCharacterName(uObj1ID)
	local charNameTarget = GetCharacterName(uObj2ID)
	local sBaseFightInfo = GetBaseFightInfo(charName,szSkillName,szStateName,szArgName)
	local FightInfo = ""
	if sBaseFightInfo then
		FightInfo = sBaseFightInfo ..GetStaticTextClient(4520).. charNameTarget  .. GetStaticTextClient(4519)
	end
	return FightInfo
end

function CFightInfoWnd:NonTypeAttackHit(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	local charName = GetCharacterName(uObj1ID)
	local charNameTarget = GetCharacterName(uObj2ID)
	local sBaseFightInfo = GetBaseFightInfo(charName,szSkillName,szStateName,szArgName)
	local FightInfo = ""
	if not sBaseFightInfo then
		FightInfo = ""
	elseif(charName ~= "" and charNameTarget ~= "") then
		FightInfo = sBaseFightInfo ..GetStaticTextClient(4508).. charNameTarget  .. GetStaticTextClient(4505) .. uHPChanged .. GetAttackType(uAttackType) .. GetStaticTextClient(4506)
	elseif(charName == "" and charNameTarget ~= "") then
		FightInfo = sBaseFightInfo ..GetStaticTextClient(4508).. charNameTarget  .. GetStaticTextClient(4505) .. uHPChanged .. GetAttackType(uAttackType) .. GetStaticTextClient(4506)
	end
	return FightInfo
end

function CFightInfoWnd:StateEffectHit(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	local DisplayName,FightInfoType = GetBuffDisplayName(szArgName)
	if DisplayName == nil then
		DisplayName = ""
	end
	local FightInfo = ""
	local FightInfo = self:Hurt(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	if uHPExtra ~= 0 then
		FightInfo = FightInfo .. GetStaticTextClient(4513) .. uHPExtra .. GetStaticTextClient(4558) .. GetStaticTextClient(4520) ..DisplayName .. GetStaticTextClient(4567) .. GetStaticTextClient(4568)
	end
	return FightInfo
end

function CFightInfoWnd:MultiAbsorb(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	local FightInfo = ""
	local FightInfo = self:Hurt(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	if uHPExtra ~= 0 then
		FightInfo = FightInfo .. GetStaticTextClient(4513) .. uHPExtra .. GetStaticTextClient(4558) .. GetStaticTextClient(4520) .. GetStaticTextClient(4569).. GetStaticTextClient(4568)
	end
	return FightInfo
end

function CFightInfoWnd:Heal(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	local charName = GetCharacterName(uObj1ID)
	local charNameTarget = GetCharacterName(uObj2ID)
	local sBaseFightInfo = GetBaseFightInfo(charName,szSkillName,szStateName,szArgName)
	local bIsItemSkill = IsItemSkill(uObj1ID,uObj2ID,szSkillName)
	local FightInfo = ""
	if sBaseFightInfo and charName ~= "" then
		FightInfo = sBaseFightInfo.. GetStaticTextClient(4544) .. charNameTarget  .. GetStaticTextClient(4521) .. uHPChanged .. GetStaticTextClient(4522)
	elseif sBaseFightInfo then
		bIsItemSkill = true
	end
	if bIsItemSkill then
		local SkillName = GetSkillDisplayName(szSkillName)
		if SkillName ~= "" then
			SkillName = GetStaticTextClient(4562) .. SkillName
		end
		FightInfo = GetStaticTextClient(4502) .. charNameTarget .. SkillName .. GetStaticTextClient(4521) .. uHPChanged .. GetStaticTextClient(4522)
	end
	return FightInfo
end

function CFightInfoWnd:StrikeHeal(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	local charName = GetCharacterName(uObj1ID)
	local charNameTarget = GetCharacterName(uObj2ID)
	local sBaseFightInfo = GetBaseFightInfo(charName,szSkillName,szStateName,szArgName)
	local FightInfo = ""
	if sBaseFightInfo then
		FightInfo = sBaseFightInfo .. GetStaticTextClient(4507).. GetStaticTextClient(4544) .. charNameTarget  .. GetStaticTextClient(4521) .. uHPChanged .. GetStaticTextClient(4522)
	end
	return FightInfo
end

function CFightInfoWnd:DoSkill(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	local charName = GetCharacterName(uObj1ID)
	local skillName = GetSkillDisplayName(szSkillName)
	local FightInfo = ""
	if skillName and skillName ~= "" then
		FightInfo = GetStaticTextClient(4502) .. charName .. GetStaticTextClient(4541) .. skillName
	end
	return FightInfo
end	

function CFightInfoWnd:DoPosSkill(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	local charName = GetCharacterName(uObj1ID)
	local skillName = GetSkillDisplayName(szSkillName)
	local FightInfo = ""
	if skillName and skillName ~= "" then
		FightInfo = GetStaticTextClient(4502) .. charName .. GetStaticTextClient(4541) .. skillName
	end
	return FightInfo
end

function CFightInfoWnd:RenewMP(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	local charName = GetCharacterName(uObj1ID)
	local charNameTarget = GetCharacterName(uObj2ID)
	local sBaseFightInfo = GetBaseFightInfo(charName,szSkillName,szStateName,szArgName)
	local bIsItemSkill = IsItemSkill(uObj1ID,uObj2ID,szSkillName)
	local FightInfo = ""
	if sBaseFightInfo and charName ~= "" then
		FightInfo = sBaseFightInfo .. GetStaticTextClient(4544) .. charNameTarget .. GetStaticTextClient(4521) 
	.. uHPChanged .. GetStaticTextClient(4558) .. GetStaticTextClient(4559)
	elseif sBaseFightInfo then
		bIsItemSkill = true
	end
	if bIsItemSkill then
		local SkillName = GetSkillDisplayName(szSkillName)
		if SkillName ~= "" then
			SkillName = GetStaticTextClient(4562) .. SkillName
		else
			SkillName = GetSkillDisplayName(szArgName)
			if SkillName ~= "" then
				SkillName = GetStaticTextClient(4562) .. SkillName
			end
		end
		FightInfo = GetStaticTextClient(4502) .. charNameTarget .. SkillName .. GetStaticTextClient(4521) 
		.. uHPChanged .. GetStaticTextClient(4558) .. GetStaticTextClient(4559)
	end
	return FightInfo
end

function CFightInfoWnd:ConsumeMP(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	local charName = GetCharacterName(uObj2ID)
	local FightInfo = GetStaticTextClient(4502) .. charName .. GetStaticTextClient(4560) 
	.. uHPChanged .. GetStaticTextClient(4558) .. GetStaticTextClient(4559)
	return FightInfo
end

function CFightInfoWnd:LostMP(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	local charNameTarget = GetCharacterName(uObj2ID)
	local sSkillName = GetSkillDisplayName(szSkillName)
	if sSkillName ~= "" then
		sSkillName =  GetStaticTextClient(4562) .. sSkillName 
	end
	local FightInfo = GetStaticTextClient(4502) .. charNameTarget .. sSkillName
		 .. GetStaticTextClient(4563) .. uHPChanged .. GetStaticTextClient(4558) .. GetStaticTextClient(4559)
	return FightInfo
end

function CFightInfoWnd:ConsumeHP(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	local charName = GetCharacterName(uObj1ID)
	local FightInfo = GetStaticTextClient(4502) .. charName .. GetStaticTextClient(4560) .. uHPChanged .. GetStaticTextClient(4522)
	return FightInfo
end

function CFightInfoWnd:SuccessDoCasting(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	local charName = GetCharacterName(uObj1ID)
	local sSkillName = GetSkillDisplayName(szArgName)
	local FightInfo = ""
	if sSkillName and sSkillName ~= "" then
		FightInfo = GetStaticTextClient(4502) .. charName .. GetStaticTextClient(4536) .. sSkillName
	end
	return FightInfo
end

function CFightInfoWnd:InterruptCasting(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	local charName = GetCharacterName(uObj1ID)
	local targetName = GetCharacterName(uObj2ID)
	local sSkillName = GetSkillDisplayName(szSkillName)
	local FightInfo = ""
	if sSkillName and sSkillName ~= "" then
		if charName == "" then
			FightInfo = GetStaticTextClient(4502) .. targetName .. GetStaticTextClient(4539) .. sSkillName
		else
			FightInfo = GetStaticTextClient(4502) .. targetName .. GetStaticTextClient(4520) ..charName.. GetStaticTextClient(4539) .. sSkillName
		end
	end
	return FightInfo
end

function CFightInfoWnd:RenewRP(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	local charName = GetCharacterName(uObj1ID)
	local charNameTarget = GetCharacterName(uObj2ID)
	local bIsItemSkill = IsItemSkill(uObj1ID,uObj2ID,szSkillName)
	local FightInfo = GetStaticTextClient(4502) .. charName .. GetStaticTextClient(4521) 
	.. uHPChanged .. GetStaticTextClient(4558) .. GetStaticTextClient(4564)
	if bIsItemSkill then
		local SkillName = GetSkillDisplayName(szSkillName)
		if SkillName ~= "" then
			SkillName = GetStaticTextClient(4562) .. SkillName
		end
		FightInfo = GetStaticTextClient(4502) .. charNameTarget .. SkillName .. GetStaticTextClient(4521) 
		.. uHPChanged .. GetStaticTextClient(4558) .. GetStaticTextClient(4564)
	end
	return FightInfo
end

function CFightInfoWnd:Die(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	local FightInfo = ""
	local charName = GetCharacterName(uObj1ID)
	local charNameTarget = GetCharacterName(uObj2ID)
	if uObj1ID == uObj2ID then
		if charNameTarget then
			FightInfo = GetStaticTextClient(4502) .. charNameTarget .. GetStaticTextClient(4565)
		else
			charNameTarget = g_GameMain.m_TeamBase:GetMemberNameByEntityId(uObj2ID)
			if charNameTarget then
				FightInfo = GetStaticTextClient(4502) .. charNameTarget .. GetStaticTextClient(4565)
			end
		end
	elseif uObj2ID == g_MainPlayer:GetEntityID() then
		FightInfo = GetStaticTextClient(4502) .. charName .. GetStaticTextClient(4530) .. charNameTarget
	elseif uObj1ID == g_MainPlayer:GetEntityID() then
		FightInfo = GetStaticTextClient(4502) .. charName .. GetStaticTextClient(4530) .. charNameTarget
	else
		if charNameTarget == "" then
			charNameTarget = g_GameMain.m_TeamBase:GetMemberNameByEntityId(uObj2ID)
		end
		if charNameTarget then --获取队友的名字 ,出现nil的情况
			FightInfo = GetStaticTextClient(4502) .. charNameTarget ..GetStaticTextClient(4547)
		end
	end
	return FightInfo
end

function CFightInfoWnd:ConsumeRP(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	local charName = GetCharacterName(uObj1ID)
	local FightInfo = GetStaticTextClient(4502) .. charName .. GetStaticTextClient(4560) 
	.. uHPChanged .. GetStaticTextClient(4558) .. GetStaticTextClient(4564)
	return FightInfo
end

function CFightInfoWnd:AddBurstSoulTimes(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	local charNameTarget = GetCharacterName(uObj2ID)
	local sSkillName = GetSkillDisplayName(szSkillName)
	local FightInfo = GetStaticTextClient(4502) .. charNameTarget .. GetStaticTextClient(4570) .. sSkillName
	 .. GetStaticTextClient(4571).. uHPChanged .. GetStaticTextClient(4572)
	return FightInfo
end

function CFightInfoWnd:LostRP(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	local charNameTarget = GetCharacterName(uObj2ID)
	local sSkillName = GetSkillDisplayName(szSkillName)
	if sSkillName ~= "" then
		sSkillName =  GetStaticTextClient(4562) .. sSkillName 
	end
	local FightInfo = GetStaticTextClient(4502) .. charNameTarget .. sSkillName
		 .. GetStaticTextClient(4563) .. uHPChanged .. GetStaticTextClient(4558) .. GetStaticTextClient(4564)
	return FightInfo
end

function CFightInfoWnd:StateEffectStrikeHit(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	local DisplayName,FightInfoType = GetBuffDisplayName(szArgName)
	if DisplayName == nil then
		DisplayName = ""
	end
	local FightInfo = ""
	local FightInfo = self:StrikeHurt(uObj1ID, uObj2ID, szSkillName, szStateName, szArgName,
								uHPChanged, uAttackType, uHPExtra, bPassive)
	if uHPExtra ~= 0 then
		FightInfo = FightInfo .. GetStaticTextClient(4513) .. uHPExtra .. GetStaticTextClient(4558) .. GetStaticTextClient(4520) ..DisplayName .. GetStaticTextClient(4567) .. GetStaticTextClient(4568)
	end
	return FightInfo
end

function CFightInfoWnd:GetExpInfo(uInfoIndex, uObj1ID, uObj2ID, szSkillName, 
							uHPChanged, uHPExtra, TeamAwardExp)
	-- 修行塔不给经验提示
	if g_GameMain.m_SceneType == 14 then
		return ""
	end
	local charName = GetCharacterName(uObj1ID)
	local FightInfo = GetStaticTextClient(4531, charName, uHPChanged)
	if uHPExtra ~= 0 or TeamAwardExp ~= 0 then
		FightInfo = FightInfo.."("..GetStaticTextClient(4532, uHPChanged-uHPExtra-TeamAwardExp)
		if uHPExtra ~= 0 then
			FightInfo = FightInfo..GetStaticTextClient(4533, uHPExtra)
		end
		if TeamAwardExp ~= 0 then
			FightInfo = FightInfo..GetStaticTextClient(4534, TeamAwardExp)
		end
		FightInfo = FightInfo..")"
	end
	return FightInfo
end
