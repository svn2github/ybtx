gac_require "information/tooltips/ToolTipsInc"
gac_gas_require "skill/SkillCommon"
cfg_load "skill/SkillPart_Common"
lan_load "skill/Lan_SkillPart_Common"
cfg_load "skill/NonFightSkill_Common"
lan_load "skill/Lan_NonFightSkill_Common"
lan_load "equip/Lan_WeaponDPSFloor_Common"
cfg_load "equip/WeaponDPSFloor_Common"
cfg_load "skill/SkillConsumeMoney_Common"
lan_load "equip/advance/Lan_AttributeOfJingLingSkill_Common"

local ColorTbl = {}
ColorTbl["价格"] 			= g_ColorMgr:GetColor("价格")

local function GetSkillNextLevel(SkillName,PlayerLevel)
	local LevelTbl  = g_PlayerLevelToSkillLevelTbl[SkillName]
	assert(IsTable(LevelTbl),SkillName .. "在配置表SkillPart_Common中PlayerLevelToSkillLevel列填表错误")
	for i = 1,#(LevelTbl) do
		if PlayerLevel >= tonumber(LevelTbl[i][1]) and PlayerLevel <= tonumber( LevelTbl[i][2]) then
			if tostring(LevelTbl[i][3]) == "l" then
				return PlayerLevel +1 ,false
			elseif tonumber( LevelTbl[i][3]) ~= nil then
				return tonumber( LevelTbl[i][2]) + 1,true
			end
		end 
	end
end

local function GetShowSkillLevel(SkillName,SkillLevel)
	local LevelTbl  = g_PlayerLevelToSkillLevelTbl[SkillName]
	for i = 1 ,#(LevelTbl) do
		if LevelTbl[i][3] == SkillLevel then
			return i
		elseif LevelTbl[i][1] <= SkillLevel and LevelTbl[i][2] >= SkillLevel then
			return i
		end
	end	
end

-- 技能栏显示的 技能ToolTips全部
function CTooltips:GetWndSkillTips(SkillName,SkillLevel)
	-- 普攻技能
	if SkillName == g_GameMain.CommonlySkillName or string.find(SkillName,"普通攻击") then
		local tips = "#cFFFFFF" .. self:GetCommonlySkillTips()
		return tips
	end
	
	-- 被动技能  
	local NonFightSkill = self:CheckNonFightSkill(SkillName)
	if NonFightSkill ~= nil then
		local tips = "#cFFFFFF" .. Lan_NonFightSkill_Common(MemH64(NonFightSkill("NonFightSkillName")),"DisplayName") .. "#r" .. Lan_NonFightSkill_Common(MemH64(NonFightSkill("NonFightSkillName")),"SkillDescription")
		return tips
	end
	
	local IsScaleSkill = false
	if SkillPart_Common(SkillName) 
		and SkillPart_Common(SkillName,"PlayerLevelToSkillLevel") 
		and SkillPart_Common(SkillName,"ClassLimit") > 0 then
		local LevelTbl  = g_PlayerLevelToSkillLevelTbl[SkillName]
		if #(LevelTbl) > 1 then
			IsScaleSkill = true
		end
	end
	-- new add
	local IsNoLearnedSkill = false
	local PlayerLevel = g_MainPlayer:CppGetLevel()
	local ClassLimit = SkillPart_Common(SkillName,"ClassLimit")
	if ClassLimit ~= nil and  ClassLimit ~= "" and ClassLimit ~= 0 then
		if SkillLevel  and SkillLevel > 0 and IsScaleSkill then
			SkillLevel = SkillLevel
		elseif SkillLevel  and SkillLevel > 0 and not IsScaleSkill then
			SkillLevel = PlayerLevel
		elseif SkillLevel == nil or SkillLevel == 0 then
			if IsScaleSkill then
				local LevelTbl  = g_PlayerLevelToSkillLevelTbl[SkillName]
				SkillLevel = LevelTbl[1][3]
			else
				SkillLevel = PlayerLevel
			end
			IsNoLearnedSkill = true
		end
	else
		if SkillLevel == nil then
			SkillLevel = 1
		end
	end

	-- 主动技能
	local talentPoint = g_MainPlayer:CppGetTalentPoint(SkillName)
	local skill =  CSkillClient_GetSkillByNameForLua( SkillName)
	local ConsumeType 	= g_MainPlayer:GetConsumeType( SkillName )
	local ConsumeValue 	= g_MainPlayer:GetConsumeValue( SkillName, SkillLevel )	
	local Distance 		= g_MainPlayer:GetSkillDistance( SkillName, talentPoint )
	local ProcessType	= g_MainPlayer:GetCastingProcessType( SkillName )
	local ProcessTime	= g_MainPlayer:GetCastingProcessTime( SkillName, talentPoint )	
	local CoolTime 		= g_MainPlayer:GetCoolDownTime( SkillName, talentPoint )
	local FireCondition	= g_MainPlayer:GetFireCondition( SkillName ) 
	
	local strParameter, strDescription,Color, Tips = "","","",{}
	
	Color = "#cFFFFFF"
	-- 技能名 + 技能等级
	-- 这个技能名改用 DISPLAY NAME
	
	local DisplayName = Lan_SkillPart_Common(MemH64(SkillName),"DisplayName")
	strParameter = DisplayName
	
	local attackType = ""
	local uAttackType = skill:GetAttackType()
	if(uAttackType == EAttackType.eATT_Puncture) then
		attackType = GetStaticTextClient(17005)
	elseif(uAttackType == EAttackType.eATT_Chop) then
		attackType = GetStaticTextClient(17006)
	elseif(uAttackType == EAttackType.eATT_BluntTrauma) then
		attackType = GetStaticTextClient(17007)
	elseif(uAttackType == EAttackType.eATT_Nature) then
		attackType = GetStaticTextClient(17008)
	elseif(uAttackType == EAttackType.eATT_Destroy) then
		attackType = GetStaticTextClient(17009)
	elseif(uAttackType == EAttackType.eATT_Evil) then
		attackType = GetStaticTextClient(17010)
	end	

	local ShowSkillLevel
	if SkillLevel and IsScaleSkill then
		ShowSkillLevel = GetShowSkillLevel(SkillName,SkillLevel)
	end
	if attackType ~= "" then
		if ShowSkillLevel then
			strParameter = strParameter .. "(" .. ToolTipsText(277) .. ShowSkillLevel .. ")" .. "#R" .. GetStaticTextClient(17011) .. attackType .. "#r"
		else
			strParameter = strParameter .. "#R" .. GetStaticTextClient(17011) .. attackType .. "#r"
		end
	else
		if ShowSkillLevel then
			strParameter = strParameter .. "(" .. ToolTipsText(277) .. ShowSkillLevel .. ")" .. "#r"
		else
			strParameter = strParameter .. "#r"
		end
	end

	--学习等级
--	local NeedLevel = SkillPart_Common(SkillName,"NeedLevel")
--	local NextSkillLevelText = ""
--	if SkillPart_Common(SkillName,"PlayerLevelToSkillLevel") ~= nil and SkillPart_Common(SkillName,"PlayerLevelToSkillLevel") ~= "" then
--		local NextSkillLevel, IsShowNextLevelText = GetSkillNextLevel(SkillName,PlayerLevel)
--		NextSkillLevelText = NextSkillLevel >= NeedLevel and NextSkillLevel or NeedLevel
--		if IsShowNextLevelText then
--			NextSkillLevelText = "#R#cFF3700" .. GetStaticTextClient(17012) .. NextSkillLevelText
--		else
--			NextSkillLevelText = ""
--		end
--	end
	--local TempColor = PlayerLevel >= NeedLevel and Color or "#cFF3700"
	--strParameter = strParameter .. TempColor .. GetStaticTextClient(17013) ..NeedLevel .. NextSkillLevelText .. Color .. "#r"
			
	-- 消耗 ++ 施法距离
	if ConsumeType == EConsumeType.eCT_HP then
		strParameter = strParameter .. GetStaticTextClient(17014) .. ConsumeValue .. GetStaticTextClient(17015)
	elseif ConsumeType == EConsumeType.eCT_MP then
		strParameter = strParameter .. GetStaticTextClient(17014) .. ConsumeValue .. GetStaticTextClient(17016)
	elseif ConsumeType == EConsumeType.eCT_RG then
		strParameter = strParameter .. GetStaticTextClient(17014) .. ConsumeValue .. GetStaticTextClient(17017)
	elseif ConsumeType == EConsumeType.eCT_EG then
		strParameter = strParameter .. GetStaticTextClient(17014) .. ConsumeValue .. GetStaticTextClient(17018)
	elseif ConsumeType == EConsumeType.eCT_CP then
		strParameter = strParameter .. GetStaticTextClient(17014) .. ConsumeValue .. GetStaticTextClient(17019)
	else
		strParameter = strParameter .. GetStaticTextClient(17020)
	end
	
	if Distance ~= 0 and Distance <= 15 then
		strParameter = strParameter .. "  #R" .. GetStaticTextClient(17021) .. Distance .. GetStaticTextClient(17022) .. "#r"
	else
		strParameter = strParameter .. "#r"
	end
	
	-- 技能流程 ++ 冷却时间
	if ProcessType == ECastingProcessType.eCPT_Wink then
		strParameter = strParameter .. GetStaticTextClient(17023)
	elseif ProcessType == ECastingProcessType.eCPT_Sing then
		strParameter = strParameter .. GetStaticTextClient(17024) .. ProcessTime .. GetStaticTextClient(17028)
	elseif ProcessType == ECastingProcessType.eCPT_Channel then
		strParameter = strParameter .. GetStaticTextClient(17025) .. ProcessTime .. GetStaticTextClient(17028)
	elseif ProcessType == ECastingProcessType.eCPT_GradeSing then
		strParameter = strParameter .. GetStaticTextClient(17026)
	elseif ProcessType == ECastingProcessType.eCPT_ImmovableSing then
		strParameter = strParameter .. GetStaticTextClient(17038)
	end
	
	if CoolTime ~= 0 then
		strParameter = strParameter .. "#R" .. GetStaticTextClient(17027) .. CoolTime .. GetStaticTextClient(17028) .. "#r"
	else
		strParameter = strParameter .. "#r"
	end
	
	-- 施放条件	
	if FireCondition == 1 then
		strParameter = strParameter .. GetStaticTextClient(17029) .. "#r"
	elseif FireCondition == 2 then
		strParameter = strParameter .. GetStaticTextClient(17030) .. "#r"
	elseif FireCondition == 3 then
		strParameter = strParameter .. GetStaticTextClient(17031) .. "#r"
	end
		
	local strDescription = self:GetSkillTips( SkillPart_Common, SkillName, SkillLevel )
	
	strParameter = strParameter .. strDescription
	
--	local money = ""
--	if IsScaleSkill  or IsNoLearnedSkill then
--		local c_money = CMoney:new()
--		if IsNoLearnedSkill then
--			local money_count = SkillConsumeMoney_Common(SkillPart_Common(SkillName,"NeedLevel"),"LearnSkillMoney")
--			money =  c_money:ChangeMoneyToString(math.ceil(money_count))
--			money = "#r" .. GetStaticTextClient(1139) .. "#R" ..  money		
--		elseif IsScaleSkill then
--			local LevelTbl  = g_PlayerLevelToSkillLevelTbl[SkillName]
--			for i = 1, #(LevelTbl) do
--				if LevelTbl[i][3] == SkillLevel and LevelTbl[i+1] then
--					local money_count = SkillConsumeMoney_Common(LevelTbl[i+1][1],"UpSkillMoney")
--					money =  c_money:ChangeMoneyToString(math.ceil(money_count))
--					money = "#r" .. GetStaticTextClient(1140) .. "#R" ..  money
--					break
--				end
--			end	
--		end	
--	end	
--	strParameter = strParameter .. ColorTbl["价格"] .. money	

	self:SetFirstTipsInfo( Tips, Color, strParameter, 0 )
	
	return self:Completed( Tips )
end

-- 技能栏显示的 普攻技能ToolTips
function CTooltips:GetCommonlySkillTips()
	local Info = g_GameMain.m_RoleStatus.Part[12].m_Info
	local EquipType,DynInfo
	if Info then
		EquipType = g_ItemInfoMgr:GetItemInfo(Info:GetBigID(),Info:GetIndex(),"EquipType")
		DynInfo = g_DynItemInfoMgr:GetDynItemInfo(Info:GetItemID())
	end
	local Tooltip = ""
	if Info == nil or (Info and DynInfo and DynInfo.CurDuraValue == 0)   then
		Tooltip = GetStaticTextClient(17032).."#r"..GetStaticTextClient(17033)..GetStaticTextClient(17039)	
	else
		local AttackType = WeaponDPSFloor_Common(EquipType,"AttackType")
		if DynInfo.BaseLevel > g_MainPlayer:CppGetLevel() or AttackType == "穿刺"  or AttackType == "砍斫" or AttackType == "钝击"  then
			Tooltip = GetStaticTextClient(17032).."#r"..GetStaticTextClient(17033)..GetStaticTextClient(17039)
		else
			local attackType = Lan_WeaponDPSFloor_Common(MemH64(EquipType),"DisplayAttackType")
			Tooltip =  attackType .. "#r"..GetStaticTextClient(17033) .. attackType .. GetStaticTextClient(17034)		
		end
	end	
	return Tooltip
end


-- 检查该技能是不是非战斗技能
function CTooltips:CheckNonFightSkill(SkillName)
	local Keys = NonFightSkill_Common:GetKeys()
	for i ,v in pairs (Keys) do 
		if NonFightSkill_Common(v,"NonFightSkillName") == SkillName then
			return NonFightSkill_Common(v)
		end
	end
end

-- 技能栏显示的 技能ToolTips描述部分
function CTooltips:GetSkillTips( tbl, SkillName, talentPoint )
	local str_desc
	if SkillPart_Common == tbl then
		str_desc = Lan_SkillPart_Common(MemH64(SkillName),"SkillDescription")
	else
		str_desc = Lan_GiftSkill_Common(MemH64(SkillName),"SkillDescription")
	end
	local Level = 0
	if SkillPart_Common == tbl 
		and SkillPart_Common(SkillName,"PlayerLevelToSkillLevel") ~= nil 
		and SkillPart_Common(SkillName,"PlayerLevelToSkillLevel") ~= "" then
		Level = talentPoint
	else
		Level = g_MainPlayer:CppGetLevel()
	end
	
	local arg1, arg2, arg3
	if ( tbl(SkillName,"SkillDesArg1") ~= "NULL" ) then
		local str = "SkillLevel = " .. talentPoint .. "\n "
		str = str .. "return " .. tbl(SkillName,"SkillDesArg1")
		arg1 = loadstring(str)()
	end
	if ( tbl(SkillName,"SkillDesArg2") ~= "NULL" ) then
		local str = "SkillLevel = " .. talentPoint .. "\n "
		str = str .. "return " .. tbl(SkillName,"SkillDesArg2")
		arg2 = loadstring(str)()
	end
	if ( tbl(SkillName,"SkillDesArg3") ~= "NULL" ) then
		local str1 = CCfgGlobal_GetCalcChars("人物属性.公式")
		str1 = string.gsub(str1, "X", "SkillLevel")
		local str = "SkillLevel = " .. Level .. "\n "
		str = str .. "return {" .. str1 .. "}"
		local arg3Tbl = loadstring(str)()
		if Level <= 20 then
			arg3 = arg3Tbl[Level]
		else
			arg3 = arg3Tbl[21]
		end
		arg3 = arg3 * tbl(SkillName,"SkillDesArg3")
	end
	
	if arg1 == nil and arg2 == nil and arg3 ~= nil then
		str_desc = GetStrAccordWildcard(str_desc, arg3)
	elseif arg1 == nil and arg2 ~= nil and arg3 == nil then
		str_desc = GetStrAccordWildcard(str_desc, arg2)
	elseif arg1 ~= nil and arg2 == nil and arg3 == nil then
		str_desc = GetStrAccordWildcard(str_desc, arg1)
	elseif arg1 == nil and arg2 ~= nil and arg3 ~= nil then
		str_desc = GetStrAccordWildcard(str_desc, arg2, arg3)
	elseif arg1 ~= nil and arg2 ~= nil and arg3 == nil then
		str_desc = GetStrAccordWildcard(str_desc, arg1, arg2)
	elseif arg1 ~= nil and arg2 == nil and arg3 ~= nil then
		str_desc = GetStrAccordWildcard(str_desc, arg1, arg3)
	else
		str_desc = GetStrAccordWildcard(str_desc, arg1, arg2, arg3)
	end
	
	return str_desc
end

-- 天赋栏显示的 天赋ToolTips
function CTooltips:GetWndGiftSkillTips(SkillName, SkillLevel)
	local Str, Color, Tips = "","",{}
	local LearnNeedPoint = GiftSkill_Common(SkillName,"LearnNeedPoint") == "" and 1 or GiftSkill_Common(SkillName,"LearnNeedPoint")
	Color = "#cFFFFFF"
	-- 这个技能名改用 DISPLAY NAME
	local DisplayName = Lan_GiftSkill_Common(MemH64(SkillName),"DisplayName")
	--Str = DisplayName --.. "#r"..GetStaticTextClient(17035) .. SkillLevel/LearnNeedPoint
	self:SetFirstTipsInfo(Tips,Color,DisplayName,0)	
	
--	local str = GetStaticTextClient(17036) .. LearnNeedPoint .. "#r"
--	Tips[2] = {}
--	table.insert(Tips[2],str)
	
	if SkillLevel ~= 0 then
		local str_desc = self:GetSkillTips( GiftSkill_Common,SkillName, SkillLevel )
		str_desc = str_desc .. "#r"
		Tips[3] = {}
		table.insert(Tips[3], str_desc)
	end
	if SkillLevel ~= 10 then
		local NextLevel_desc = self:GetSkillTips( GiftSkill_Common,SkillName, SkillLevel+LearnNeedPoint )
		NextLevel_desc = "#r" .. GetStaticTextClient(17037).."#r" .. NextLevel_desc .. "#r"
		Tips[4] = {}
		table.insert(Tips[4], NextLevel_desc)
	end
	return self:Completed(Tips)
end

function CTooltips:GetJingLingPieceTips(JingLingPieceName)
	local JLTypeAttributeTbl = g_AttributeOfJingLingTbl[JingLingPieceName]
	local Str, Color, Tips = "","",{}
	Color = "#cFFFFFF"
	local displaySkillPieceName = Lan_AttributeOfJingLingSkill_Common(JLTypeAttributeTbl.Index,"DisplaySkillPieceName")
	
	self:SetFirstTipsInfo(Tips,Color,displaySkillPieceName,0)
	Str = Str .. JLTypeAttributeTbl.PieceCount .. GetStaticTextClient(5011) .. JLTypeAttributeTbl.DescTooltips .. "#r"
	
	
	Tips[2] = {}
	table.insert(Tips[2],Str)
	
	local PieceSkillNumber = 0
	
	if g_GameMain.m_RoleStatus.m_PieceSkillNameToNumberTbl ~= nil 
		and g_GameMain.m_RoleStatus.m_PieceSkillNameToNumberTbl[JingLingPieceName] ~= nil then
		PieceSkillNumber = g_GameMain.m_RoleStatus.m_PieceSkillNameToNumberTbl[JingLingPieceName]
	end
	Str = GetStaticTextClient(5009) .. PieceSkillNumber 
	
	Str = Str .. "#r" .. GetStaticTextClient(5010) .. JLTypeAttributeTbl.EffectTooltips

	Tips[3] = {}
	table.insert(Tips[3],Str)	
	return self:Completed(Tips)
end
