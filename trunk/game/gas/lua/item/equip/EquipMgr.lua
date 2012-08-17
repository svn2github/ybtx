
gas_require "item/equip/EquipMgrInc"
gac_gas_require "item/Equip/EquipCommonFunc"
gas_require "framework/main_frame/GetTableFromExecuteSql"
cfg_load "equip/WeaponDPSFloor_Common"
cfg_load	"equip/suit/EquipSuit_Common"
cfg_load "equip/ShieldBaseValue_Common"
cfg_load "item/Equip_RingSProperty_Common"
cfg_load "equip/advance/AttributeOfJingLingSkill_Common"
cfg_load "equip/MantleAddValue_Common"
cfg_load "item/ArmorPieceEnactment_Common"
local EquipMgrDB = "EquipMgrDB"

------------------------------------------------------------------------
function CEquip:Ctor(uItemId, uCharId, uItemType, sItemName)
	self.m_uItemId	 = uItemId
	self.m_uPlayerId = uCharId
	self.m_uItemType = uItemType
	self.m_sItemName = sItemName
end

---------------------------------------------------------------

function CEquip:MantleAddValueFun(player)
    local info = MantleAddValue_Common(self.m_sItemName)
    if info then
    	for i = 1,3 do
    		local talentName =  "SkillName" .. i
    		local talentPoint = "TalentPoint" .. i
    		if info(talentName) and info(talentName) ~= "" and info(talentPoint) and info(talentPoint) ~= "" then
    			player:AddEquipSuiteEff(info(talentName), info(talentPoint))
    		end
    	end
    end
end

function CEquip:MantleRemoveValueFun(player)
    local info = MantleAddValue_Common(self.m_sItemName)
    if info then
    	for i = 1,3 do
    		local talentName =  "SkillName" .. i
    		local talentPoint = "TalentPoint" .. i
    		if info(talentName) and info(talentName) ~= "" and info(talentPoint) and info(talentPoint) ~= "" then
    			if player:RemoveEquipSuiteEff(info(talentName), info(talentPoint)) == false then
                    LogErr("披风删除天赋出错", "CharID:" .. (player.m_uID or "-")) 
    			end
    		end
    	end
    end
end

--------------------------武器-------------------------------
--参数数据库查询的结果集
function Weapon:SetDynInfo(ret)
	self.ViewName 		= ret(1, 1)
	self.BindingType 	= ret(1, 2)
	self.BaseLevel 		= ret(1, 3)
	self.CurLevel       =  ret(1, 4)
	self.DPS 					= ret(1, 5)
	self.AttackSpeed = ret(1, 6)
	self.DPSFloorRate = ret(1, 7)
	self.uIntensifyQuality = ret(1, 8)
	self.sIntenSoul = ret(1, 9)
end

function Weapon:Info()
	local info = {
		self.m_uItemId,   
		self.ViewName, 		
		self.BindingType, 	
		self.BaseLevel, 
		self.CurLevel,		
		self.DPS,
		self.AttackSpeed,
		self.DPSFloorRate,
		self.uIntensifyQuality,
		self.MaxDuraValue,
		self.CurDuraValue,
		self.sIntenSoul
	}
	return info
end

function Weapon:SendEquipInfo2Conn(CharID)
	Gas2GacById:RetCommonWeaponInfo(CharID, unpack(self:Info(),1,12))
	self:RetIntensifyInfo(CharID)
    self:RetIntensifyAddInfo(CharID)
    self:RetAdvanceInfo(CharID)
    self:RetEnactmentInfo(CharID)
    self:RetSuperaddRate(CharID)
end

--------------------设置强化附加属性的值（1~3阶段的属性）------------------------------
function CEquip:DisableIntensifyAttr(player, equipPart, destroySource)
	if not player.m_ErrorEquipTbl[equipPart]  then
    	self:DestroyTranID(player,equipPart, "IntensifyPropty", destroySource)
    end
end

function CEquip:EnableIntensifyAttr(player, eEquipPart, createSource)  
	if not player.m_ErrorEquipTbl[eEquipPart]  then 
	    local transID = self:CreateTranID(player, eEquipPart, "IntensifyPropty", createSource)
	    --设置附加属性1
	    local attr1, attr1_var
	    if self.uAddAttr1 ~= nil then
	       attr1, attr1_var = unpack(self.uAddAttr1)
	    end
		 
		if (attr1_var ~= nil ) and  (tonumber(attr1_var) ~= 0) then
	        --print(attr1, attr1_var,"附加1")
			player:ChangeTransactionPropertyValue(transID, GetPropertyAdderName(attr1),math.ceil(attr1_var * self:GetEquipSuperRate(self.m_SuperaddRate)))
		end	
		local attr2, attr2_var 
	    if self.uAddAttr2 ~= nil then
	       attr2, attr2_var = unpack(self.uAddAttr2)
	    end
		--设置附加属性2
		if (attr2_var ~= nil ) and (tonumber(attr2_var) ~= 0) then
	        --print(attr2, attr2_var,"附加2")
			player:ChangeTransactionPropertyValue(transID, GetPropertyAdderName(attr2),math.ceil(attr2_var * self:GetEquipSuperRate(self.m_SuperaddRate)))
		end
		player:CommitTransaction(transID)
		self:SetAttrStateByDuraState(player, transID)  
	end
end

-----强化额外附加属性（强化4~9阶段的属性设置）------------------
function CEquip:DisableAddAttrPropty(player, equipPart, destroySource)
	if not player.m_ErrorEquipTbl[equipPart]  then
    	self:DestroyTranID(player, equipPart, "IntensifyAddPropty", destroySource)
    end
end

function CEquip:EnableAddAttrPropty(player, equipPart, createSource)
	if not player.m_ErrorEquipTbl[equipPart] then
	    local transID =  self:CreateTranID(player, equipPart, "IntensifyAddPropty", createSource)
		local AddAttrInfoTbl = self:GetIntensifyAddInfo()
		for i = 2 ,#(AddAttrInfoTbl),2 do
			if AddAttrInfoTbl[i] ~= "" then
				player:ChangeTransactionPropertyValue(transID, GetPropertyAdderName(AddAttrInfoTbl[i]),math.ceil(AddAttrInfoTbl[i + 1] * self:GetEquipSuperRate(self.m_SuperaddRate)))
			else
				break
			end
		end
	    player:CommitTransaction(transID)
	    self:SetAttrStateByDuraState(player, transID)  
	end
end

--强化3阶段产出的额外天赋或者技能，设置到player身上或者摘掉该属性
--参数：player、天赋名称、当前等级、bool(true表明设置到player身上，false表明摘掉属性)
function CEquip:SetIntensifyAddTalent(player, talentIndex, equipLevel, bEnableOrDisable)
    local talentName = SuitPartMapTalentOrSkill_Common(talentIndex,"SuitPartAttributeName")
    local talentLevelInfo = GetIntensifyAddTalentInfo(talentIndex)
    local equipLevelType = GetLevelParameter(equipLevel)
	local talentParam = talentLevelInfo[equipLevelType][1] --天赋等级
	if bEnableOrDisable then
		player:AddEquipSuiteEff(talentName, talentParam)
	else
		if player:RemoveEquipSuiteEff(talentName, talentParam) == false then
            LogErr("套装部件删除天赋出错", "CharID" .. (player.m_uID or "-") )
		end
	end
end

--存储套装技能对应的等级的tbl
local EquipSuitParamTbl  ={}

--根据产出的套装属性，和角色身上所含的该套装的部件数目，得到套装属性对应的参数
local function GetSuitAttrNeedParameter(suitAttrName, includingPartsCount, suitName, baseLevel,Conn)
	local suitAttributeParameterInfo = EquipSuit_Common(suitName, tostring(includingPartsCount), "SuitAttributeParameter")
	local suitAttrParameterTbl = "{" .. suitAttributeParameterInfo .. "}"
	if EquipSuitParamTbl[suitAttrParameterTbl] == nil then
        local tbl =  loadstring("return " .. suitAttrParameterTbl)()
        EquipSuitParamTbl[suitAttrParameterTbl] = tbl
	end
	suitAttrParameterTbl = EquipSuitParamTbl[suitAttrParameterTbl]
	local levelParameter= GetLevelParameter(baseLevel)
	assert(IsNumber(levelParameter))
	if suitAttrParameterTbl[levelParameter] == nil then
	   	LogErr("得到套装属性出错", "suitName:" .. suitName .. ";baseLevel:" .. baseLevel .. ";suitAttrName:" .. suitAttrName .. ";includingPartsCount:" .. includingPartsCount, Conn)
	end
	return suitAttrParameterTbl[levelParameter][1]
end

--将套装属性设置在玩家身上或从玩家身上去掉套装属性
function CEquipMgr:GetSuitAttributeBySuitName(player, addOrRemoveSuitAttr, suitAttrTbl, includingPartsCount, suitName, baseLevel)
	local suitAttrTblLen = # suitAttrTbl
	if addOrRemoveSuitAttr then				--添加套装属性到套装属性表
		for i=1, suitAttrTblLen do
		    local needPartsCount = tonumber(suitAttrTbl[i][1])
			if IsNumber(needPartsCount) then
				if needPartsCount > includingPartsCount-1 and  needPartsCount <= includingPartsCount then
					local parameter = GetSuitAttrNeedParameter(suitAttrTbl[i][2], includingPartsCount, suitName, baseLevel, player.m_Conn)
					if parameter == nil then
						LogErr("EquipSuit_Common中parameter为nil", "suitName:" .. suitName .. ";baseLevel:" .. baseLevel .. ";includingPartsCount:" .. includingPartsCount, player.m_Conn)
					end
					player:AddEquipSuiteEff(suitAttrTbl[i][2], parameter)
				end
			elseif suitAttrTbl["FourCountSuit"] and includingPartsCount == 4 then 
				--local playerClass = g_GetPlayerClassNameByID(player.m_uID)
				local playerClass = g_GetPlayerClassNameByID(player.m_Class)
				if playerClass == suitAttrTbl[i][1] then
					local parameter = GetSuitAttrNeedParameter(suitAttrTbl[i][2], suitAttrTbl[i][1], suitName, baseLevel, player.m_Conn)
					if parameter == nil then
    					LogErr("EquipSuit_Common中parameter为nil", "suitName:" .. suitName .. ";baseLevel:" .. baseLevel .. ";includingPartsCount:" .. includingPartsCount, player.m_Conn)
					end
					player:AddEquipSuiteEff(suitAttrTbl[i][2], parameter)
				end
			end
		end
	elseif  addOrRemoveSuitAttr == false then	--从删除套装属性表删除套装属性
		for i=1, suitAttrTblLen do
		    local needPartsCount = tonumber(suitAttrTbl[i][1])
			if IsNumber(needPartsCount) then
				if includingPartsCount >= needPartsCount and includingPartsCount - 1 < needPartsCount then
					local parameter = GetSuitAttrNeedParameter(suitAttrTbl[i][2], includingPartsCount, suitName, baseLevel, player.m_Conn)
					if parameter == nil then
 						LogErr("EquipSuit_Common中parameter为nil", "suitName:" .. suitName .. ";baseLevel:" .. baseLevel .. ";includingPartsCount:" .. includingPartsCount, player.m_Conn)
					end
					if player:RemoveEquipSuiteEff(suitAttrTbl[i][2], parameter) == false then
                        LogErr("套装删除天赋出错", "SuitName:" .. (suitName or "-") .. "CharID:" .. (player.m_uID or "-")) 
					end
				end
			elseif suitAttrTbl["FourCountSuit"] and includingPartsCount == 4 then 
				--local playerClass = g_GetPlayerClassNameByID(player.m_uID)
				local playerClass = g_GetPlayerClassNameByID(player.m_Class)
				if playerClass == suitAttrTbl[i][1] then
					local parameter = GetSuitAttrNeedParameter(suitAttrTbl[i][2], suitAttrTbl[i][1], suitName, baseLevel, player.m_Conn)
					if parameter == nil then
						LogErr("EquipSuit_Common中parameter为nil", "suitName:" .. suitName .. ";baseLevel:" .. baseLevel .. ";includingPartsCount:" .. includingPartsCount, player.m_Conn)
					end
					if player:RemoveEquipSuiteEff(suitAttrTbl[i][2], parameter) == false then
                        LogErr("套装删除天赋出错", "SuitName:" .. (suitName or "-") .. "CharID:" .. (player.m_uID or "-"))  
					end
				end
			end
		end
	end
end

--登陆时，角色的套装属性
function CEquipMgr:GetSuitAttributeByPlayerID(player, suitInfoTbl)
	for i=1, # suitInfoTbl do
		local suitName = suitInfoTbl[i]["SuitName"]
		local suitAttrTbl = suitInfoTbl[i]["SuitAttrTbl"]
		local includingPartsCount = suitInfoTbl[i]["IncludingPartsCount"]
		local baseLevel = suitInfoTbl[i]["BaseLevel"]
		for j=1, # suitAttrTbl do
		    local needPartsCount = tonumber(suitAttrTbl[j][1])
			if IsNumber(needPartsCount) then
				if includingPartsCount >= needPartsCount then
						local parameter = GetSuitAttrNeedParameter(suitAttrTbl[j][2], needPartsCount, suitName, baseLevel, player.m_Conn)
						if parameter == nil then
							LogErr("EquipSuit_Common中parameter为nil", "suitName:" .. suitName .. ";baseLevel:" .. baseLevel .. ";includingPartsCount:" .. includingPartsCount, player.m_Conn)
						end
						player:AddEquipSuiteEff(suitAttrTbl[j][2], parameter)
				end
			elseif suitAttrTbl["FourCountSuit"] and includingPartsCount == 4  then
				local playerClass = g_GetPlayerClassNameByID(player.m_Class)
				if playerClass == suitAttrTbl[j][1] then
					local parameter = GetSuitAttrNeedParameter(suitAttrTbl[j][2], suitAttrTbl[j][1], suitName, baseLevel, player.m_Conn)
					if parameter == nil then
						LogErr("EquipSuit_Common中parameter为nil", "suitName:" .. suitName .. ";baseLevel:" .. baseLevel .. ";includingPartsCount:" .. includingPartsCount, player.m_Conn)
					end
					player:AddEquipSuiteEff(suitAttrTbl[j][2], parameter)
				end
			end
		end
	end
end

function Weapon:EnableBasePropty(player,eEquipPart, createSource)
	if not player.m_ErrorEquipTbl[eEquipPart]  then
	    local transID = self:CreateTranID(player, eEquipPart, "BasePropty", createSource)
	    ----设置最大攻击和最小攻击
		local MaxAttact, MinAttact = GetWeaponDPSFloat(self.AttackSpeed,self.DPS,self.DPSFloorRate)
	    --	print(MaxAttact, MinAttact,"+++++++++++")
		if eEquipPart == EEquipPart.eWeapon then
			player:ChangeTransactionPropertyValue(transID, "MainHandMaxWeaponDamageAdder", MaxAttact)
			player:ChangeTransactionPropertyValue(transID, "MainHandMinWeaponDamageAdder", MinAttact)
		elseif eEquipPart == EEquipPart.eAssociateWeapon then
			player:ChangeTransactionPropertyValue(transID, "AssistantMaxWeaponDamageAdder", MaxAttact)
			player:ChangeTransactionPropertyValue(transID, "AssistantMinWeaponDamageAdder", MinAttact)		
		end
		player:CommitTransaction(transID)
		self:SetAttrStateByDuraState(player, transID)  
	end
end

function Weapon:EnableEquipPieceEnactment(player,eEquipPart, createSource)
	if not player.m_ErrorEquipTbl[eEquipPart] then
		local transID = self:CreateTranID(player, eEquipPart, "EnactmentPropty", createSource)
		--护甲属性1
		if self.ArmorPieceAttr1 ~= "" then
			local sArmorAttr1 = GetPropertyAdderName(ArmorPieceEnactment_Common(self.ArmorPieceAttr1,"EquipType"))
			local uArmorAttr1 = ArmorPieceEnactment_Common(self.ArmorPieceAttr1,"PropertyValue")
			if self.ArmorPiece1AddTimes ~= 0 then
			 uArmorAttr1 = uArmorAttr1*self.ArmorPiece1AddTimes
			end
			if sArmorAttr1 ~= "" and uArmorAttr1 ~= 0 then
				player:ChangeTransactionPropertyValue(transID, sArmorAttr1, math.floor(uArmorAttr1))
			end
		end
		--护甲属性2
		if self.ArmorPieceAttr2 ~= "" then
			local sArmorAttr2 = GetPropertyAdderName(ArmorPieceEnactment_Common(self.ArmorPieceAttr2,"EquipType"))
			local uArmorAttr2 = ArmorPieceEnactment_Common(self.ArmorPieceAttr2,"PropertyValue")
			if self.ArmorPiece2AddTimes ~= 0 then
			 uArmorAttr2 = uArmorAttr2*self.ArmorPiece2AddTimes
			end
			if sArmorAttr2 ~= "" and uArmorAttr2 ~= 0 then
				player:ChangeTransactionPropertyValue(transID, sArmorAttr2, math.floor(uArmorAttr2))
			end
		end

		--护甲属性3
		if self.ArmorPieceAttr3 ~= "" then
			local sArmorAttr3 = GetPropertyAdderName(ArmorPieceEnactment_Common(self.ArmorPieceAttr3,"EquipType"))
			local uArmorAttr3 = ArmorPieceEnactment_Common(self.ArmorPieceAttr3,"PropertyValue")
			if self.ArmorPiece3AddTimes ~= 0 then
			 uArmorAttr3 = uArmorAttr3*self.ArmorPiece3AddTimes
			end
			if sArmorAttr3 ~= "" and uArmorAttr3 ~= 0 then
				player:ChangeTransactionPropertyValue(transID, sArmorAttr3, math.floor(uArmorAttr3))
			end
		end
		--护甲属性4
		if self.ArmorPieceAttr4 ~= "" then
			local sArmorAttr4 = GetPropertyAdderName(ArmorPieceEnactment_Common(self.ArmorPieceAttr4,"EquipType"))
			local uArmorAttr4 = ArmorPieceEnactment_Common(self.ArmorPieceAttr4,"PropertyValue")
			if self.ArmorPiece4AddTimes ~= 0 then
			 uArmorAttr4 = uArmorAttr4*self.ArmorPiece4AddTimes
			end
			if sArmorAttr4 ~= "" and uArmorAttr4 ~= 0 then
				player:ChangeTransactionPropertyValue(transID, sArmorAttr4, math.floor(uArmorAttr4))
			end
		end
		player:CommitTransaction(transID)
		self:SetAttrStateByDuraState(player, transID) 
	end 
end

--激活武器属性
function Weapon:EnablePropty(player,eEquipPart, createSource)
    self:EnableBasePropty(player,eEquipPart, createSource)
	
    --设置强化1~3阶段产出的属性
    self:EnableIntensifyAttr(player,eEquipPart, createSource)
	
	--设置强化石附加属性
	self:EnableAddAttrPropty(player,eEquipPart, createSource)
	
	--进阶属性
	self:EnableAdvanceAttr(player,eEquipPart, createSource)	
	
	--护甲片属性设定
    self:EnableEquipPieceEnactment(player,eEquipPart, createSource)
       
    --如果耐久为0，将属性无效化
    --self:SetAttrStateByDuraState(player, eEquipPart)
end

--取消武器属性
function Weapon:DisablePropty(player, eEquipPart, destroySource)
	--基础属性设置
	self:DisableBasePropty(player, eEquipPart, destroySource)
	
	--设置强化1~3阶段产出属性
	self:DisableIntensifyAttr(player, eEquipPart, destroySource)
		
	--设置强化石附加属性
	self:DisableAddAttrPropty(player, eEquipPart, destroySource)
		
	self:DisableAdvanceAttr(player, eEquipPart, destroySource)	
		
	self:DisableEquipPieceEnactment(player, eEquipPart, destroySource)
end

--设置主手普通攻击
function Weapon:SetWeaponSkill(player,nBigID,nIndex,bInitPlayer)
	local Class = player:CppGetClass()
	local AssociateWeaponItem= player:GetEquipInfo(EEquipPart.eAssociateWeapon)
	local AttackType_Associate
	if AssociateWeaponItem ~= nil and g_ItemInfoMgr:IsWeapon(AssociateWeaponItem[1]) then
		local EquipType = g_ItemInfoMgr:GetItemInfo(AssociateWeaponItem[1],AssociateWeaponItem[2],"EquipType")
		AttackType_Associate = WeaponDPSFloor_Common(EquipType,"AttackType")
	else
		AssociateWeaponItem = nil 
	end
	
	local EquipType = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"EquipType")
	local RAttackType = WeaponDPSFloor_Common(EquipType,"AttackType")
	if Class == EClass.eCL_Warrior then
		if	AssociateWeaponItem ~= nil then
			player:CppInitMHNA("大剑士单手武器普通攻击",RAttackType,EquipType,self.AttackSpeed,bInitPlayer) --双持
			return "大剑士单手武器普通攻击"
		elseif EquipType=="单手剑" or EquipType=="单手斧" or EquipType=="主手剑" or EquipType=="主手斧" then--单手武器	
			player:CppInitMHNA("大剑士单手武器普通攻击",RAttackType,EquipType,self.AttackSpeed,bInitPlayer)
			return "大剑士单手武器普通攻击"
		elseif EquipType=="双手剑" or EquipType=="双手斧" then-- 双手武器
			player:CppInitMHNA("大剑士双手武器普通攻击",RAttackType,EquipType,self.AttackSpeed,bInitPlayer)
			return "大剑士双手武器普通攻击"
		end
	elseif Class == EClass.eCL_MagicWarrior then
		if	AssociateWeaponItem ~= nil then
			player:CppInitMHNA("魔剑士单手武器普通攻击",RAttackType,EquipType,self.AttackSpeed,bInitPlayer)
			return "魔剑士单手武器普通攻击"
		elseif EquipType=="单手剑" or EquipType=="单手锤" or EquipType== "主手剑" or EquipType== "主手锤" then
			player:CppInitMHNA("魔剑士单手武器普通攻击",RAttackType,EquipType,self.AttackSpeed,bInitPlayer)
			return "魔剑士单手武器普通攻击"
		elseif EquipType=="双手剑" or EquipType=="双手锤" then
			player:CppInitMHNA("魔剑士双手武器普通攻击",RAttackType,EquipType,self.AttackSpeed,bInitPlayer)
			return "魔剑士双手武器普通攻击"
		end
	elseif Class == EClass.eCL_Paladin then
		if EquipType =="骑士枪" then
			player:CppInitMHNA("人类骑士骑士枪普通攻击",RAttackType,EquipType,self.AttackSpeed,bInitPlayer)
			return "人类骑士骑士枪普通攻击"			
		elseif EquipType == "双手弓" then		
			player:CppInitMHNA("人类骑士弓武器普通攻击",RAttackType,EquipType,self.AttackSpeed,bInitPlayer)
			return "人类骑士弓武器普通攻击"		
		end
	elseif Class == EClass.eCL_NatureElf then
		if EquipType=="主手自然杖"  then
			player:CppInitMHNA("法师自然系单手杖武器普通攻击",RAttackType,EquipType,self.AttackSpeed,bInitPlayer)	
			return "法师自然系单手杖武器普通攻击"
		elseif EquipType=="双手自然杖" then
			player:CppInitMHNA("法师自然系双手杖武器普通攻击",RAttackType,EquipType,self.AttackSpeed,bInitPlayer)
			return "法师自然系双手杖武器普通攻击"
		elseif EquipType=="主手暗黑杖"  then
			player:CppInitMHNA("法师暗黑系单手杖武器普通攻击",RAttackType,EquipType,self.AttackSpeed,bInitPlayer)	
			return "法师暗黑系单手杖武器普通攻击"
		elseif EquipType=="双手暗黑杖" then
			player:CppInitMHNA("法师暗黑系双手杖武器普通攻击",RAttackType,EquipType,self.AttackSpeed,bInitPlayer)
			return "法师暗黑系双手杖武器普通攻击"
		elseif EquipType=="主手破坏杖"  then
			player:CppInitMHNA("法师破坏系单手杖武器普通攻击",RAttackType,EquipType,self.AttackSpeed,bInitPlayer)	
			return "法师破坏系单手杖武器普通攻击"
		elseif EquipType=="双手破坏杖" then
			player:CppInitMHNA("法师破坏系双手杖武器普通攻击",RAttackType,EquipType,self.AttackSpeed,bInitPlayer)
			return "法师破坏系双手杖武器普通攻击"
		end
	elseif Class == EClass.eCL_EvilElf then
			if EquipType=="主手自然杖"  then
			player:CppInitMHNA("邪魔自然系单手杖武器普通攻击",RAttackType,EquipType,self.AttackSpeed,bInitPlayer)	
			return "邪魔自然系单手杖武器普通攻击"
		elseif EquipType=="双手自然杖" then
			player:CppInitMHNA("邪魔自然系双手杖武器普通攻击",RAttackType,EquipType,self.AttackSpeed,bInitPlayer)
			return "邪魔自然系双手杖武器普通攻击"
		elseif EquipType=="主手暗黑杖"  then
			player:CppInitMHNA("邪魔暗黑系单手杖武器普通攻击",RAttackType,EquipType,self.AttackSpeed,bInitPlayer)	
			return "邪魔暗黑系单手杖武器普通攻击"
		elseif EquipType=="双手暗黑杖" then
			player:CppInitMHNA("邪魔暗黑系双手杖武器普通攻击",RAttackType,EquipType,self.AttackSpeed,bInitPlayer)
			return "邪魔暗黑系双手杖武器普通攻击"
		elseif EquipType=="主手破坏杖"  then
			player:CppInitMHNA("邪魔破坏系单手杖武器普通攻击",RAttackType,EquipType,self.AttackSpeed,bInitPlayer)	
			return "邪魔破坏系单手杖武器普通攻击"
		elseif EquipType=="双手破坏杖" then
			player:CppInitMHNA("邪魔破坏系双手杖武器普通攻击",RAttackType,EquipType,self.AttackSpeed,bInitPlayer)
			return "邪魔破坏系双手杖武器普通攻击"
		end
	elseif Class == EClass.eCL_Priest then
		if EquipType=="主手自然杖"  then
			player:CppInitMHNA("牧师自然系单手杖武器普通攻击",RAttackType,EquipType,self.AttackSpeed,bInitPlayer)	
			return "牧师自然系单手杖武器普通攻击"
		elseif EquipType=="双手自然杖" then
			player:CppInitMHNA("牧师自然系双手杖武器普通攻击",RAttackType,EquipType,self.AttackSpeed,bInitPlayer)
			return "牧师自然系双手杖武器普通攻击"
		elseif EquipType=="主手暗黑杖"  then
			player:CppInitMHNA("牧师暗黑系单手杖武器普通攻击",RAttackType,EquipType, self.AttackSpeed,bInitPlayer)	
			return "牧师暗黑系单手杖武器普通攻击"
		elseif EquipType=="双手暗黑杖" then
			player:CppInitMHNA("牧师暗黑系双手杖武器普通攻击",RAttackType,EquipType,self.AttackSpeed,bInitPlayer)
			return "牧师暗黑系双手杖武器普通攻击"
		elseif EquipType=="主手破坏杖"  then
			player:CppInitMHNA("牧师破坏系单手杖武器普通攻击",RAttackType,EquipType,self.AttackSpeed,bInitPlayer)	
			return "牧师破坏系单手杖武器普通攻击"
		elseif EquipType=="双手破坏杖" then
			player:CppInitMHNA("牧师破坏系双手杖武器普通攻击",RAttackType,EquipType,self.AttackSpeed,bInitPlayer)
			return "牧师破坏系双手杖武器普通攻击"
		end
	elseif Class == EClass.eCL_ElfHunter then
		if EquipType=="双手弓" then
			player:CppInitMHNA("精灵弓箭手弓武器普通攻击",RAttackType,EquipType,self.AttackSpeed,bInitPlayer)
			return "精灵弓箭手弓武器普通攻击"
		elseif EquipType=="双手弩" then
			player:CppInitMHNA("精灵弓箭手弩武器普通攻击",RAttackType,EquipType,self.AttackSpeed,bInitPlayer)
			return "精灵弓箭手弩武器普通攻击"
		end
	elseif Class == EClass.eCL_DwarfPaladin then
		if EquipType=="骑士枪" then
			player:CppInitMHNA("矮人骑士单手武器普通攻击",RAttackType,EquipType,self.AttackSpeed,bInitPlayer)
			return "矮人骑士单手武器普通攻击"
		elseif EquipType=="长柄棒" or EquipType=="长柄矛" or EquipType=="长柄刀"  then
			player:CppInitMHNA("矮人骑士长柄武器普通攻击",RAttackType,EquipType,self.AttackSpeed,bInitPlayer)
			return "矮人骑士长柄武器普通攻击"
		end
	elseif Class == EClass.eCL_OrcWarrior then
		if	AssociateWeaponItem ~= nil then 
			player:CppInitMHNA("兽人战士单手武器普通攻击",RAttackType,EquipType,self.AttackSpeed,bInitPlayer)
			return "兽人战士单手武器普通攻击"
		elseif EquipType== "单手斧" or EquipType== "单手锤" or EquipType=="单手刀"
				or EquipType=="主手斧" or EquipType=="主手锤"  then
			player:CppInitMHNA("兽人战士单手武器普通攻击",RAttackType,EquipType,self.AttackSpeed,bInitPlayer)
			return "兽人战士单手武器普通攻击"
		elseif EquipType=="双手锤" or EquipType=="双手斧" then
			player:CppInitMHNA("兽人战士双手武器普通攻击",RAttackType,EquipType,self.AttackSpeed,bInitPlayer)
			return "兽人战士双手武器普通攻击"
		end
	end	
	
end

--设置副手武器的普攻技能
function Weapon:SetAssociateSkill(player,ItemType,ItemName,bInitPlayer)
	local ItemEquipType = g_ItemInfoMgr:GetItemInfo(ItemType,ItemName,"EquipType")
	local Class = player:CppGetClass()
	local WeaponItem = player:GetEquipInfo(EEquipPart.eWeapon)
	if WeaponItem ~= nil then
		local EquipType = g_ItemInfoMgr:GetItemInfo(WeaponItem[1],WeaponItem[2],"EquipType")
		local AttackType_Weapon = WeaponDPSFloor_Common(EquipType,"AttackType")
		if Class == EClass.eCL_Warrior then
			player:CppInitAHNA("大剑士单手武器普通攻击",AttackType_Weapon,EquipType,self.AttackSpeed,bInitPlayer) --双持
		elseif Class == EClass.eCL_MagicWarrior then
			player:CppInitAHNA("魔剑士单手武器普通攻击",AttackType_Weapon,EquipType,self.AttackSpeed,bInitPlayer)
		elseif Class == EClass.eCL_OrcWarrior then
			player:CppInitAHNA("兽人战士单手武器普通攻击",AttackType_Weapon,EquipType,self.AttackSpeed,bInitPlayer)
		end
	end
	
	local AttackType = WeaponDPSFloor_Common(ItemEquipType,"AttackType")
	if Class == EClass.eCL_Warrior then
		player:CppInitAHNA("大剑士副手武器普通攻击",AttackType,ItemEquipType,self.AttackSpeed,bInitPlayer)
	elseif Class == EClass.eCL_MagicWarrior then
		player:CppInitAHNA("魔剑士副手武器普通攻击",AttackType,ItemEquipType,self.AttackSpeed,bInitPlayer)
	elseif Class == EClass.eCL_OrcWarrior then	
		player:CppInitAHNA("兽人战士副手武器普通攻击",AttackType,ItemEquipType,self.AttackSpeed,bInitPlayer)
	end
end
----------------------------------防具----------------------
function Armor:SetDynInfo(ret)
	self.ViewName 		= ret(1, 1)
	self.BindingType 	= ret(1, 2)
	self.BaseLevel 		= ret(1, 3)
	self.CurLevel       = ret(1, 4)
	self.OutputAttr1	= ret(1, 5)
	self.OutputAttr2	= ret(1, 6)
	self.OutputAttr3	= ret(1, 7)
	self.StaticProValue = ret(1, 8)
	self.uIntensifyQuality = ret(1, 9) 	
	self.sIntenSoul     = ret(1, 10)
end

function Armor:Info()
	local info = {
		self.m_uItemId,
		self.ViewName, 		
		self.BindingType, 	
		self.BaseLevel, 
		self.CurLevel,			
		self.OutputAttr1,
		self.OutputAttr2,
		self.OutputAttr3,
		self.StaticProValue,
		self.uIntensifyQuality,
		self.MaxDuraValue,
		self.CurDuraValue,
		self.sIntenSoul
	}
	return info
end

function Armor:SendEquipInfo2Conn(CharID)
	Gas2GacById:RetCommonArmorInfo(CharID, unpack(self:Info(),1,13))
	self:RetIntensifyInfo(CharID)
    self:RetIntensifyAddInfo(CharID)
    self:RetAdvanceInfo(CharID)
    self:RetEnactmentInfo(CharID)
    self:RetSuperaddRate(CharID)
end

function CEquip:CreateTranID(player, equipPart, proptyType, createSource)
    if not IsCppBound(player)  then
        return 
    end
    local transID = player:CreateTransactionID() 
    if player.m_EquipAttrTransIDTbl[equipPart] == nil then
        player.m_EquipAttrTransIDTbl[equipPart] ={}
        player.m_EquipAttrTransIDTbl[equipPart][proptyType] = {}
        player.m_EquipAttrTransIDTbl[equipPart][proptyType]["TranID"] = transID
        player.m_EquipAttrTransIDTbl[equipPart][proptyType]["CreateSource"] = createSource
        player.m_EquipAttrTransIDTbl[equipPart][proptyType]["DestroySource"]= ""
    else
        if player.m_EquipAttrTransIDTbl[equipPart][proptyType] == nil  then
            player.m_EquipAttrTransIDTbl[equipPart][proptyType] = {}
            player.m_EquipAttrTransIDTbl[equipPart][proptyType]["DestroySource"]= ""
        end
        if player.m_EquipAttrTransIDTbl[equipPart][proptyType]["TranID"] == nil then
            player.m_EquipAttrTransIDTbl[equipPart][proptyType]["TranID"] = transID 
            player.m_EquipAttrTransIDTbl[equipPart][proptyType]["CreateSource"] = createSource
        else
            local oldTransID = player.m_EquipAttrTransIDTbl[equipPart][proptyType]["TranID"]
            if oldTransID == nil then
               LogErr("属性产出，事务不存在，" .. "EquipPart:" .. eEquipPart .. ",proptyType:" .. proptyType .. ", destroySource:" .. destroySource, player.m_Conn) 
            end
            player:DestroyTransaction(oldTransID)
            local oldCreateSource = player.m_EquipAttrTransIDTbl[equipPart][proptyType]["CreateSource"]
            local oldDestroySource = player.m_EquipAttrTransIDTbl[equipPart][proptyType]["DestroySource"]
            LogErr("装备属性产出PropertyTranID错误,", proptyType .. "之前的未撤销:" .. "装备部位" .. equipPart .. ",CreateSource:" .. oldCreateSource .. ",DestroySource:" .. oldDestroySource, player.m_Conn) 
            player.m_EquipAttrTransIDTbl[equipPart][proptyType]["TranID"] = transID
            player.m_EquipAttrTransIDTbl[equipPart][proptyType]["CreateSource"] = createSource
            player.m_EquipAttrTransIDTbl[equipPart][proptyType]["DestroySource"]= ""
        end
    end
    return transID   
end

function Armor:EnableBasePropty(player,eEquipPart, createSource)
	if not player.m_ErrorEquipTbl[eEquipPart]  then 
	    local transID = self:CreateTranID(player, eEquipPart, "BasePropty", createSource)
	
		local IsStatic = g_ItemInfoMgr:GetItemInfo(self.m_uItemType,self.m_sItemName,"IsStatic")	
		if IsStatic == 0 or IsStatic == "" then
			local outputAttr = GetEquipOutputAttrName(self.m_uItemType,self.m_sItemName)
			local outputAttrCount = {self.OutputAttr1, self.OutputAttr2, self.OutputAttr3}
			for i = 1,#(outputAttr) do 
	--			print(GetPropertyAdderName(outputAttr[i]),math.floor(outputAttrCount[i],"防具++++"))
				player:ChangeTransactionPropertyValue(transID, GetPropertyAdderName(outputAttr[i]),math.floor(outputAttrCount[i]))
			end
		elseif IsStatic == 1 then
			local NameAndValueTbl = GetStaticPropertyName(self.m_uItemType,self.m_sItemName)
			local outputAttrCount = {self.OutputAttr1, self.OutputAttr2, self.OutputAttr3,self.StaticProValue}
			for i = 1,#(NameAndValueTbl) do 
				local AttrName = GetAttrName(NameAndValueTbl[i][1])	
	--			print(GetPropertyAdderName(AttrName),math.floor(outputAttrCount[i]),"防具静态属性+++++")
				player:ChangeTransactionPropertyValue(transID, GetPropertyAdderName(AttrName),math.floor(outputAttrCount[i]))
			end
		end
		player:CommitTransaction(transID)
		self:SetAttrStateByDuraState(player, transID)  
	end
end

function CEquip:EnableEquipPieceEnactment(player,eEquipPart, createSource)
	if not player.m_ErrorEquipTbl[eEquipPart]  then 
		local transID = self:CreateTranID(player, eEquipPart, "EnactmentPropty", createSource)
		--护甲属性1
		if self.ArmorPieceAttr1 ~= "" then
			local AddAttr3 = GetPropertyAdderName(ArmorPieceEnactment_Common(self.ArmorPieceAttr1,"EquipType"))
			local AddAttrValue3 = ArmorPieceEnactment_Common(self.ArmorPieceAttr1,"PropertyValue")
			if self.ArmorPiece1AddTimes ~= 0 then
			 AddAttrValue3 = AddAttrValue3*self.ArmorPiece1AddTimes
			end
			if AddAttr3 ~= "" and AddAttrValue3 ~= 0 then
				player:ChangeTransactionPropertyValue(transID, AddAttr3, math.floor(AddAttrValue3))
			end
		end
		--护甲属性2
		if self.ArmorPieceAttr2 ~= "" then
			local AddAttr4 = GetPropertyAdderName(ArmorPieceEnactment_Common(self.ArmorPieceAttr2,"EquipType"))
			local AddAttrValue4 = ArmorPieceEnactment_Common(self.ArmorPieceAttr2,"PropertyValue")
			if self.ArmorPiece2AddTimes ~= 0 then
			 AddAttrValue4 = AddAttrValue4*self.ArmorPiece2AddTimes
			end
			if AddAttr4 ~= "" and AddAttrValue4 ~= 0 then
				player:ChangeTransactionPropertyValue(transID, AddAttr4, math.floor(AddAttrValue4))
			end
		end
		player:CommitTransaction(transID)
		self:SetAttrStateByDuraState(player, transID)  
	end
end

function CEquip:SetAttrStateByDuraState(player, transID)
    if self.CurDuraValue / self.MaxDuraValue == 0 then
          player:DisableTransaction(transID)
    end
end

function Armor:EnablePropty(player,eEquipPart, createSource)
	if self.nLeftTime == 0 then return end
	if eEquipPart == EEquipPart.eBack then
		self:MantleAddValueFun(player)
	else
	    self:EnableBasePropty(player,eEquipPart, createSource)
	
	    --设置强化1~3阶段产出的属性
	    self:EnableIntensifyAttr(player,eEquipPart, createSource)
		
		--设置强化石附加属性
		self:EnableAddAttrPropty(player,eEquipPart, createSource)	
		
		self:EnableAdvanceAttr(player,eEquipPart, createSource)	
		
	    self:EnableEquipPieceEnactment(player,eEquipPart, createSource)	
	end
end


function CEquip:DisableEquipPieceEnactment(player,eEquipPart, destroySource)
	if not player.m_ErrorEquipTbl[eEquipPart]  then
    	self:DestroyTranID(player,eEquipPart, "EnactmentPropty", destroySource)
    end
end

function CEquip:DestroyTranID(player,eEquipPart, proptyType, destroySource)
    if player.m_EquipAttrTransIDTbl[eEquipPart] == nil then
        LogErr("装备属性撤消PropertyTranID错误,", proptyType .. ",之前的已撤销, eEquipPart部分为nil:" .. "装备部位" .. eEquipPart, player.m_Conn) 
    else
        if player.m_EquipAttrTransIDTbl[eEquipPart][proptyType]["TranID"] == nil then
            local oldCreateSource = player.m_EquipAttrTransIDTbl[eEquipPart][proptyType]["CreateSource"]
            local oldDestroySource = player.m_EquipAttrTransIDTbl[eEquipPart][proptyType]["DestroySource"]
            LogErr("装备属性撤消PropertyTranID错误,", proptyType .. "之前的已撤销:" .. "装备部位" .. eEquipPart .. ",CreateSource:" .. oldCreateSource .. ",DestroySource:" .. oldDestroySource, player.m_Conn)
            player.m_EquipAttrTransIDTbl[eEquipPart][proptyType]["CreateSource"] = ""
            player.m_EquipAttrTransIDTbl[eEquipPart][proptyType]["DestroySource"] = ""
        else
            local transID = player.m_EquipAttrTransIDTbl[eEquipPart][proptyType]["TranID"]
            if transID == nil then
               LogErr("属性撤销，事务不存在，" .. "EquipPart:" .. eEquipPart .. ",proptyType:" .. proptyType .. ", destroySource:" .. destroySource, player.m_Conn) 
            end
            player:DestroyTransaction(transID)
            player.m_EquipAttrTransIDTbl[eEquipPart][proptyType]["TranID"] = nil
            player.m_EquipAttrTransIDTbl[eEquipPart][proptyType]["DestroySource"]= destroySource
        end
    end
end

--取消武器附加属性
function Armor:DisablePropty(player,eEquipPart, destroySource)
	if self.nLeftTime == 0 then return end
	if eEquipPart == EEquipPart.eBack then
		self:MantleRemoveValueFun(player)
	else
	    self:DisableBasePropty(player,eEquipPart, destroySource)
	    
	    --设置强化1~3阶段产出属性
	    self:DisableIntensifyAttr(player,eEquipPart, destroySource)
		
		--设置强化石附加属性
		self:DisableAddAttrPropty(player,eEquipPart, destroySource)
		
		self:DisableAdvanceAttr(player,eEquipPart, destroySource)
			
	    self:DisableEquipPieceEnactment(player,eEquipPart, destroySource)
	end
end
---------------------------------戒指-----------------------
function Ring:SetDynInfo(ret)
	self.ViewName 		 	= ret(1, 1)
	self.BindingType 	 	= ret(1, 2)
	self.BaseLevel 		 	= ret(1, 3)
	self.CurLevel           = ret(1, 4)
	self.DPS 			 			= ret(1, 5)
	self.StaticProValue = ret(1, 6)
	self.uIntensifyQuality = ret(1, 7) 
	self.sIntenSoul         = ret(1, 8)
end

function Ring:Info()
	local info = {
		self.m_uItemId,
		self.ViewName, 		
		self.BindingType, 	
		self.BaseLevel, 	
		self.CurLevel,		
		self.DPS, 	
		self.StaticProValue,		
		self.uIntensifyQuality,
	    self.MaxDuraValue,
		self.CurDuraValue,
		self.sIntenSoul
	}
	return info
end

function Ring:SendEquipInfo2Conn(CharID)
	Gas2GacById:RetCommonRingInfo(CharID, unpack(self:Info(),1,11))
	self:RetIntensifyInfo(CharID)
    self:RetIntensifyAddInfo(CharID)
    self:RetAdvanceInfo(CharID)
    self:RetEnactmentInfo(CharID)
    self:RetSuperaddRate(CharID)
end

function Ring:EnableBasePropty(player, eEquipPart, createSource)
	if not player.m_ErrorEquipTbl[eEquipPart]  then 
	    local transID = self:CreateTranID(player, eEquipPart, "BasePropty", createSource)
		--设置戒指的伤害
		local DamageType = g_ItemInfoMgr:GetItemInfo(self.m_uItemType,self.m_sItemName,"DamageType")
		if DamageType == 1 then --法术伤害
	--		print(self.DPS,"戒指法术++++")
			player:ChangeTransactionPropertyValue(transID, "MagicDamageValueAdder", math.floor(self.DPS))
		elseif DamageType == 2 then
	--		print(self.DPS,"戒指物理++++")
			player:ChangeTransactionPropertyValue(transID, "PhysicalDPSAdder", math.floor(self.DPS))
		end
		--设置静态属性
		if self.StaticProValue ~= 0 then
			local RingStaticPropertyInfo = Equip_RingSProperty_Common(self.m_sItemName)		
			local PropertyName = GetRingStaticPropertyName(RingStaticPropertyInfo)
			local AttrName = GetAttrName(PropertyName)
	--		print(GetPropertyAdderName(AttrName),math.floor(self.StaticProValue),"戒指静态属性+++")
			player:ChangeTransactionPropertyValue(transID, GetPropertyAdderName(AttrName), math.floor(self.StaticProValue))
		end
		player:CommitTransaction(transID)  
		self:SetAttrStateByDuraState(player, transID)  
	end
end


--激活戒指附加属性
function Ring:EnablePropty(player, eEquipPart, createSource)
    self:EnableBasePropty(player, eEquipPart, createSource)
	
    --设置强化1~3阶段产出的属性
    self:EnableIntensifyAttr(player, eEquipPart, createSource)
	
	--设置强化石附加属性
	self:EnableAddAttrPropty(player, eEquipPart, createSource)
	
	self:EnableAdvanceAttr(player, eEquipPart, createSource)			
	
	self:EnableEquipPieceEnactment(player, eEquipPart, createSource)
end 

--取消戒指附加属性
function Ring:DisablePropty(player,eEquipPart, destroySource)
    self:DisableBasePropty(player,eEquipPart, destroySource)

    --设置强化1~3阶段产出属性
    self:DisableIntensifyAttr(player,eEquipPart, destroySource)
	
	--设置强化石附加属性
	self:DisableAddAttrPropty(player,eEquipPart, destroySource)
	
	self:DisableAdvanceAttr(player,eEquipPart, destroySource)
		
    self:DisableEquipPieceEnactment(player,eEquipPart, destroySource)
end

------------------------------盾牌-----------------------------------
function Shield:SetDynInfo(ret)
	self.ViewName 		= ret(1, 1)
	self.BindingType 	= ret(1, 2)
	self.BaseLevel 		= ret(1, 3)
	self.CurLevel       =   ret(1, 4)
	self.uIntensifyQuality = ret(1, 5)
	self.uOutputAttrValue1 = ret(1,6)
	self.uOutputAttrValue2 = ret(1, 7)
	self.uOutputAttrValue3 = ret(1, 8)
	self.uOutputAttrValue4 = ret(1, 9)
	self.sIntenSoul     =   ret(1, 10)
end

function Shield:Info()
	local info = {
		self.m_uItemId,   
		self.ViewName, 		
		self.BindingType, 	
		self.BaseLevel, 	
		self.CurLevel,	
		self.uIntensifyQuality,
	    self.uOutputAttrValue1,
		self.uOutputAttrValue2,
		self.uOutputAttrValue3,
		self.uOutputAttrValue4,
	    self.MaxDuraValue ,
		self.CurDuraValue,
		self.sIntenSoul
	} 
	return info
end 

function Shield:SendEquipInfo2Conn(CharID)
	Gas2GacById:RetCommonShieldInfo(CharID, unpack(self:Info(), 1, 13))
	self:RetIntensifyInfo(CharID)
    self:RetIntensifyAddInfo(CharID)
    self:RetAdvanceInfo(CharID)
    self:RetEnactmentInfo(CharID)
    self:RetSuperaddRate(CharID)
end

function Shield:EnableBasePropty(player,eEquipPart, createSource)
	if not player.m_ErrorEquipTbl[eEquipPart]  then
	--设置盾牌的伤害
	    local transID = self:CreateTranID(player, eEquipPart, "BasePropty", createSource)
		local IsShield = g_ItemInfoMgr:GetItemInfo(self.m_uItemType,self.m_sItemName,"IsShield")
		if IsShield == 1 then		--是盾牌
			local blockRate ,blockValue = GetShieldBlock(self.CurLevel,self.m_uItemType,self.m_sItemName)
			player:ChangeTransactionPropertyValue(transID, "BlockRateMultiplier", blockRate)
			player:ChangeTransactionPropertyValue(transID, "BlockDamageAdder", math.floor(blockValue))
			local outputAttrValue1,outputAttrValue2,outputAttrValue3,outputAttrValue4
			if IsStatic == 1 then
				outputAttrValue1 = self.uOutputAttrValue1
				outputAttrValue2 = self.uOutputAttrValue2
				outputAttrValue3 = self.uOutputAttrValue3
				outputAttrValue4 = self.uOutputAttrValue4
			else
				local CurLevel = self.CurLevel
				outputAttrValue1 = ShieldBaseValue_Common(CurLevel,"Defence")
				outputAttrValue2 = ShieldBaseValue_Common(CurLevel,"NatureResistanceValue")
				outputAttrValue3 = ShieldBaseValue_Common(CurLevel,"DestructionResistanceValue")
				outputAttrValue4 = ShieldBaseValue_Common(CurLevel,"EvilResistanceValue")
			end
			player:ChangeTransactionPropertyValue(transID, "DefenceAdder", outputAttrValue1)
			player:ChangeTransactionPropertyValue(transID, "NatureResistanceValueAdder", outputAttrValue2)
			player:ChangeTransactionPropertyValue(transID, "DestructionResistanceValueAdder", outputAttrValue3)
			player:ChangeTransactionPropertyValue(transID, "EvilResistanceValueAdder", outputAttrValue4)
		elseif IsShield == 2 then --是法器
			local DamageValue =  GetShieldMagicDamage(self.CurLevel,self.m_uItemType,self.m_sItemName)
	--		print(DamageValue,"+++法器")
			player:ChangeTransactionPropertyValue(transID, "MagicDamageValueAdder", math.floor(DamageValue))		
		end	
		player:CommitTransaction(transID)
		self:SetAttrStateByDuraState(player, transID)  
	end
end

--激活盾牌附加属性
function Shield:EnablePropty(player,eEquipPart, createSource)
	self:EnableBasePropty(player,eEquipPart, createSource)

    --设置强化1~3阶段产出的属性
    self:EnableIntensifyAttr(player,eEquipPart, createSource)
	
	--设置强化石附加属性
	self:EnableAddAttrPropty(player,eEquipPart, createSource)
	
	self:EnableAdvanceAttr(player,eEquipPart, createSource)	
		
	self:EnableEquipPieceEnactment(player, eEquipPart, createSource)
end 

function CEquip:DisableBasePropty(player, eEquipPart, destroySource)
	if not player.m_ErrorEquipTbl[eEquipPart]  then
		self:DestroyTranID(player, eEquipPart, "BasePropty", destroySource)
	end
end

--取消盾牌附加属性
function Shield:DisablePropty(player,eEquipPart, destroySource)
	self:DisableBasePropty(player,eEquipPart, destroySource)
	
    --设置强化1~3阶段产出属性
    self:DisableIntensifyAttr(player,eEquipPart, destroySource)
	
	--设置强化石附加属性
	self:DisableAddAttrPropty(player,eEquipPart, destroySource)
	
	self:DisableAdvanceAttr(player,eEquipPart, destroySource)
		
    self:DisableEquipPieceEnactment(player,eEquipPart, destroySource)
end

----------------------------------------------------------
--三个参数(角色id，物品类型，物品名称,数据库查询结果集)
function CEquipMgr:CreateEquipObj(uItemId, uCharId, nBigID, nIndex)
  if uItemId == nil or uCharId == nil or nBigID == nil or nIndex == nil then
		return nil
	end
	assert(IsNumber(uItemId))
	assert(IsNumber(uCharId))
	assert(IsNumber(nBigID))
	local equip = nil
	if  nBigID == 5 then		--武器
		equip = Weapon:new(uItemId, uCharId, nBigID, nIndex)
	elseif nBigID == 6 or nBigID == 9 then
		equip = Armor:new(uItemId, uCharId, nBigID, nIndex)
	elseif nBigID == 8 then
		equip = Ring:new(uItemId, uCharId, nBigID, nIndex)
	elseif nBigID == 7 then
		equip = Shield:new(uItemId, uCharId, nBigID, nIndex)
	else
		return nil
	end	
    return equip
end

function CEquipMgr:GetEquipFromRet(uItemId, uCharId, nBigID, nIndex, rettbl)
    local equip = self:CreateEquipObj(uItemId, uCharId, nBigID, nIndex)
    if equip == nil then
        return nil    
    end
    
    if rettbl["BaseInfo"]:GetRowNum() > 0 then
        equip:SetDynInfo(rettbl["BaseInfo"])
    end
    equip:SetIntensifyInfo(rettbl["IntensifyInfo"])
	equip:SetIntensifyAddInfo(rettbl["IntensifyAddInfo"])
	equip:SetAdvanceInfo(rettbl["AdvanceInfo"])
	equip:SetEquipEnactmentInfo(rettbl["EnactmentInfo"])
	--设置装备的耐久信息
	equip.MaxDuraValue = rettbl["MaxDuraValue"]
	equip.CurDuraValue = rettbl["CurDuraValue"]
	equip.nLeftTime = rettbl.m_tblTimeOut[1]
	
	equip:SetSuperaddInfo(uItemId, rettbl["EquipSuperaddInfo"])
	return equip
end

--获取CEquipMgr对象
function g_GetEquipMgr()
	g_EquipMgr = g_EquipMgr or CEquipMgr:new()
	return g_EquipMgr
end

-----------------------------------RPC Function------------------------------------
--灌输强化魂
function Gac2Gas:PourIntensifySoul(Conn, uItemId, uSoulNum)
    if not IsCppBound(Conn.m_Player) then
        return 
    end
    local charID = Conn.m_Player.m_uID
	local data = { ["ItemID"] = uItemId, ["CharID"] = charID, ["SoulNum"] = uSoulNum }
	Conn.m_Player:OnSavePlayerSoulTickFunc(nil,Conn.m_Player)
	local function CallBack(suc, errorMsg,total_soul)
	 	if suc == false then
	 		Gas2GacById:NoticePourIntensifyError(charID, uItemId, uSoulNum)
	 		MsgToConn(Conn, 8003, errorMsg)
	 	elseif suc then
	 		Gas2GacById:NoticePourIntensifySuccess(charID,uItemId)
	 		if IsCppBound(Conn.m_Player) then
	 		    Conn.m_Player.m_uSoulValue = total_soul + Conn.m_Player.m_uKillDropSoulCount
	 		    Gas2GacById:ReturnModifyPlayerSoulNum(charID,Conn.m_Player.m_uSoulValue)
	 		end
	 	end
	end
	--local equipMgrEntry = (g_DBTransDef["EquipMgrDB"])
	local ret = CallAccountManualTrans(Conn.m_Account, EquipMgrDB, "PourIntensifySoul", CallBack, data)
	if ret == 1 then 
	    Gas2GacById:NoticePourIntensifyError(charID, uItemId, uSoulNum)
	end
end

--将装备的数据库信息写入内存
local function SetEquipUpgradeInfo( Conn, player, nEquipPart, uItemId, nEquipType, sEquipName, oldequipRet,newequipRet )
	local equip_mgr = g_GetEquipMgr()
	local oldequip  = equip_mgr:GetEquipFromRet(uItemId, player.m_uID, nEquipType, sEquipName,oldequipRet)
	local newequip  = equip_mgr:GetEquipFromRet(uItemId, player.m_uID, nEquipType, sEquipName,newequipRet)
	if(0 ~= nEquipPart) then --不是升级包裹中的装备
		oldequip:DisablePropty(player, nEquipPart)
		newequip:EnablePropty(player, nEquipPart)
	end
	newequip:SendEquipInfo2Conn(player.m_uID)
	Gas2Gac:NoticeUpgradeSuccess(Conn, uItemId)
end

--装备升级
function Gac2Gas:UpgradeEquip(Conn, uItemId, nEquipPart)
	if(not Conn.m_Player) then return end
	if Conn.m_Player:IsInBattleState() and nEquipPart ~= 0 then
		MsgToConn(Conn, 1033)
		return 
	end
	if Conn.m_Player:IsInForbitUseWeaponState() and nEquipPart ~= 0 then
		MsgToConn(Conn, 1034)
		return 
	end

	local data = {["ItemID"] = uItemId
					,["CharID"] = Conn.m_Player.m_uID
					,["sceneName"] = Conn.m_Player.m_Scene.m_SceneName				
					 } 
    
	local function CallBack(suc, nEquipType, sEquipName, oldequipRet,newequipRet)
		if suc == false then
			local errorMsgNo = nEquipType
			MsgToConn(Conn, errorMsgNo)
	 		Gas2Gac:NoticeUpgradeError(Conn, uItemId)
		elseif suc then
			SetEquipUpgradeInfo( Conn, Conn.m_Player, nEquipPart, uItemId, nEquipType, sEquipName, oldequipRet, newequipRet)
		end
	end
	--local entry = (g_DBTransDef["EquipMgrDB"])
	CallAccountManualTrans(Conn.m_Account, EquipMgrDB, "EquipUpgrade", CallBack, data)	
end

--将装备的数据库信息写入内存
local function SetEquipIntensifyInfo( Conn, player, oldEquipRet, upInfo, playerAddAttrInfo, charID )
	local nEquipType, sEquipName, newEquipRet = upInfo["EquipType"], upInfo["EquipName"], upInfo["EquipRet"]    
	local equipPart, equipID = upInfo["EquipPart"], upInfo["EquipID"]
	local oldequip = CEquip:new(equipID, charID, nEquipType, sEquipName)
	oldequip:SetIntensifyInfo(oldEquipRet)
	
	local newequip = CEquip:new(equipID, charID, nEquipType, sEquipName)
	newequip:SetIntensifyInfo(newEquipRet)
	if(0 ~= equipPart) then --强化身上的装备
	    local baseLevel = playerAddAttrInfo["BaseLevel"]
		if playerAddAttrInfo["SuitAttrTbl"] ~= nil then
			local suitAttrTbl, includingPartsCount = playerAddAttrInfo["SuitAttrTbl"], playerAddAttrInfo["IncludingPartsCount"]
			local suitName = playerAddAttrInfo["SuitName"]
			local equip_mgr = g_GetEquipMgr()
			equip_mgr:GetSuitAttributeBySuitName(player, true, suitAttrTbl, includingPartsCount, suitName, baseLevel)	
		end
		local equipSuperaddInfo = upInfo["EquipSuperaddInfo"]
		oldequip:DisableIntensifyAttr(player, equipPart, "SetEquipIntensifyInfo")
		player:SetPlayerEquipPhase(equipPart,newequip.uIntensifyPhase, 7)
	    newequip.CurDuraValue = upInfo["CurDuraValue"]
	    newequip.MaxDuraValue = upInfo["MaxDuraValue"]
	    newequip:SetSuperaddInfo(equipID, equipSuperaddInfo)
		newequip:EnableIntensifyAttr(player, equipPart, "SetEquipIntensifyInfo")
		Gas2Gac:UpdateModel(player.m_Conn,player:GetEntityID())
		Gas2Gac:UpdateModel(player:GetIS(0),player:GetEntityID())
		if newequip.uIntenTalentIndex and newequip.uIntenTalentIndex > 0  then
	        newequip:SetIntensifyAddTalent(player, newequip.uIntenTalentIndex, baseLevel, true)
	    end
	   CGasFightingEvaluation.SaveFightingEvaluationInfo(player)
	end
	newequip:RetIntensifyInfo(charID)
	Gas2GacById:NoticeIntensifySuccess(charID, equipID,equipPart)
end

function Gac2Gas:IntensifyEquip(Conn, uItemId, EquipPart)
	if(not Conn.m_Player) then return end
	if Conn.m_Player:IsInBattleState() and EquipPart ~= 0 then
	    MsgToConn(Conn, 1031)
	    Gas2Gac:UpdateEquipIntensifyInfo(Conn,uItemId)
		return 
	end
	if Conn.m_Player:IsInForbitUseWeaponState() and EquipPart ~= 0  then
		MsgToConn(Conn, 1032)
		Gas2Gac:UpdateEquipIntensifyInfo(Conn,uItemId)
		return 
	end
	local charID = Conn.m_Player.m_uID
	local data = {["ItemID"] = uItemId
					,["CharID"] = charID
					,["sceneName"] = Conn.m_Player.m_Scene.m_SceneName
					,["EquipPart"] = EquipPart
				}
	local function CallBack(suc, oldEquipRet, upInfo, playerAddAttrInfo)
		if suc then
			SetEquipIntensifyInfo(Conn, Conn.m_Player, oldEquipRet, upInfo, playerAddAttrInfo, charID)
		else
			local errorMsgNO = oldEquipRet
			if errorMsgNO ~= nil then 
				Gas2GacById:UpdateEquipIntensifyInfo(charID,uItemId)
				MsgToConn(Conn, errorMsgNO)
			end
		end
	end
	--local entry = (g_DBTransDef["EquipIntensifyDB"])
	CallAccountManualTrans(Conn.m_Account, "EquipIntensifyDB", "EquipIntensify", CallBack, data)	
end

--回复物品信息
function Gac2Gas:GetInfoByID(Conn,uItemId)
    if not IsCppBound(Conn.m_Player) then
        return 
    end
    local charID = Conn.m_Player.m_uID
	local data = {["ItemId"] = uItemId, ["CharID"] = charID}
	local function CallBack(suc, result)
		if suc then
		    local itemType = result["ItemType"]
		    local itemName = result["ItemName"]
		    local itemID = result["ItemID"]
			Gas2Gac:RetInfoByID(Conn,itemID,itemType,itemName)
		end
		
	end
	--local entry = (g_DBTransDef["EquipMgrDB"])
	CallAccountManualTrans(Conn.m_Account, EquipMgrDB, "GetItemInfoByID", CallBack, data)
end

-------------------------------------右键点击包裹中的魂珠，将其魂值分配在角色身上------------------------------------
--右键点击包裹中的魂珠，将其含有的魂值分配在角色身上
function Gac2Gas:AvargeSoulCountToEquip(Conn, nRoomIndex, nPos)
    if not IsCppBound(Conn.m_Player) then
        return 
    end
	local data = {["RoomIndex"]= nRoomIndex, ["Pos"]= nPos, ["CharID"]= Conn.m_Player.m_uID
					,["sceneName"] = Conn.m_Player.m_Scene.m_SceneName}
	Conn.m_Player:OnSavePlayerSoulTickFunc(nil,Conn.m_Player)
	OnSavePlayerExpFunc({Conn.m_Player})
	local function CallBack(suc, result, sStrType)
		if suc then
			if sStrType == "AddExp" then
				if IsCppBound(Conn.m_Player) then
					local CurLevel,LevelExp = result.CurLevel,result.LevelExp
					local AddExp = result.AddExp
					if CurLevel then
						local AddExpTbl = {}
						AddExpTbl["Level"] = CurLevel
						AddExpTbl["Exp"] = LevelExp
						AddExpTbl["AddExp"] = AddExp
						AddExpTbl["uInspirationExp"] = 0
						CRoleQuest.AddPlayerExp_DB_Ret(Conn.m_Player, AddExpTbl)
					end
				end
			elseif sStrType == "AreaFbPoint" then
				Gas2Gac:UpdatePlayerPoint(Conn, result.AreaFbPointType, result.soulCount)
			else
				Conn.m_Player.m_uSoulValue = result.total_soul + Conn.m_Player.m_uKillDropSoulCount
				Gas2Gac:ReturnModifyPlayerSoulNum(Conn,Conn.m_Player.m_uSoulValue)
				Gas2Gac:ReturnCharAddSoulMsg(Conn,sStrType)
			end
		else
			local errorMsgNo = result
			if errorMsgNo then
				MsgToConn(Conn, errorMsgNo)
			end
		end
	end
	CallAccountManualTrans(Conn.m_Account, EquipMgrDB, "AvargeSoulCountToPlayer", CallBack, data)
end

-----------装备强化洗白---
local function EquipIntensifyBack(Conn, result, total_soul, charID)
    local intensifyBackType = result["IntensifyBackType"]
	local newEquipIntenRet = result["EquipRetTbl"]["IntensifyInfo"]
	local newEquipIntenAddRet = result["EquipRetTbl"]["IntensifyAddInfo"]
	local nRoomIndex = result["RoomIndex"]
	local nPos = result["Pos"]
	local consumeItemID = result["ItemID"]
	local suitName = result["SuitName"]
	local suitAttrTbl = result["SuitAttrTbl"]
	local includingPartsCount = result["IncludingPartsCount"]
	local baseLevel = result["EquipBaseLevel"]
	local oldEquipIntenRet = result["OldEquipRetTbl"]["IntensifyInfo"]
	local oldEquipIntenAddRet = result["OldEquipRetTbl"]["IntensifyAddInfo"]
	local intensifyPhaseNum = result["IntensifyPhaseNum"]
	local nBigID = result["EquipType"]
	local nIndex = result["EquipName"]
	local equipID = result["EquipID"]
	local nEquipPart = result["EquipPart"]
	local equip_mgr = g_GetEquipMgr()
	local newequip = CEquip:new(equipID, charID, nBigID, nIndex)
	newequip:SetIntensifyInfo(newEquipIntenRet)
	newequip:SetIntensifyAddInfo(newEquipIntenAddRet)
	newequip:RetIntensifyInfo(charID)
	newequip:RetIntensifyAddInfo(charID)
	
	local oldequip = CEquip:new(equipID, Conn.m_Player.m_uID, nBigID, nIndex)
	oldequip:SetIntensifyInfo(oldEquipIntenRet)
	oldequip:SetIntensifyAddInfo(oldEquipIntenAddRet)		
	Conn.m_Player.m_uSoulValue = total_soul + Conn.m_Player.m_uKillDropSoulCount
	Gas2GacById:ReturnModifyPlayerSoulNum(charID,Conn.m_Player.m_uSoulValue)
	if nEquipPart ~= 0 then     --如果强化洗白的装备是穿在玩家身上的
	    if suitName ~= nil and suitName ~= "" and (result["IntenBackUseStone"] == nil and intensifyPhaseNum == 3 or 
	       result["IntenBackUseStone"]== true and  intensifyPhaseNum >= 3)then --只有在产生套装的3阶段重铸时，才影响装备的套装属性		
			equip_mgr:GetSuitAttributeBySuitName(Conn.m_Player, false, suitAttrTbl, includingPartsCount, suitName, baseLevel)	
        end
        local equipSuperaddInfo = result["EquipSuperaddInfo"]
		oldequip:DisableIntensifyAttr(Conn.m_Player, nEquipPart, "EquipIntensifyBack")
		oldequip:DisableAddAttrPropty(Conn.m_Player, nEquipPart, "EquipIntensifyBack")
        Conn.m_Player:SetPlayerEquipPhase(nEquipPart,newequip.uIntensifyPhase, 7)
	    newequip.CurDuraValue = result["CurDuraValue"]
	    newequip.MaxDuraValue = result["MaxDuraValue"]
        newequip:SetSuperaddInfo(equipID, equipSuperaddInfo)
	    newequip:EnableIntensifyAttr(Conn.m_Player, nEquipPart, "EquipIntensifyBack")
		newequip:EnableAddAttrPropty(Conn.m_Player, nEquipPart, "EquipIntensifyBack")
		Gas2Gac:UpdateModel(Conn,Conn.m_Player:GetEntityID())
		Gas2Gac:UpdateModel(Conn.m_Player:GetIS(0),Conn.m_Player:GetEntityID())
	    if oldequip.uIntenTalentIndex and oldequip.uIntenTalentIndex > 0 and  (result["IntenBackUseStone"] == nil and intensifyPhaseNum == 3 
	        or result["IntenBackUseStone"]== true and  intensifyPhaseNum >= 3) then
            oldequip:SetIntensifyAddTalent(Conn.m_Player, oldequip.uIntenTalentIndex, baseLevel, false)
        end
     CGasFightingEvaluation.SaveFightingEvaluationInfo(Conn.m_Player)	
	end
end

function Gac2Gas:EquipIntensifyBack(Conn, nBigID, nIndex, nItemID, nEquipPart)
	-- 判断是否在战斗状态	
	if Conn.m_Player == nil then
	   return  
	end
    if Conn.m_Player:IsInBattleState() and nEquipPart ~= 0 then
        MsgToConn(Conn, 180012)
        return 
    end
	--判断是否在缴械状态
	if Conn.m_Player:IsInForbitUseWeaponState() and nEquipPart ~= 0  then
	    MsgToConn(Conn, 180013)
	   return  
	end
	Conn.m_Player:OnSavePlayerSoulTickFunc(nil,Conn.m_Player)
	local charID = Conn.m_Player.m_uID 
	local data = {["EquipType"] = nBigID, ["EquipName"]= nIndex, ["EquipID"]=nItemID, ["CharID"]=charID
								,["sceneName"] = Conn.m_Player.m_Scene.m_SceneName, ["EquipPart"] = nEquipPart}
	local function CallBack(suc, errorMsgNum, result,total_soul)
		MsgToConn(Conn, errorMsgNum)
		if suc then
            EquipIntensifyBack(Conn, result, total_soul, charID)
            Gas2GacById:NoticeIntensifySuccess(charID, nItemID, nEquipPart)
		else
		    Gas2GacById:UpdateEquipIntensifyInfo(charID, nItemID)
		end
	end
	CallAccountManualTrans(Conn.m_Account, "EquipIntensifyDB", "EquipIntensifyBack", CallBack, data)			
end

--@brief 查看目标玩家身上的魂值
--@param playerId：要查看的玩家Id
function Gac2Gas:GetAimPlayerSoulNum(Conn,playerId)
	local data = {
							["uCharID"] = playerId
							}
							
	local function CallBack(soulcount)
		Gas2Gac:ReturnAimPlayerSoulNum(Conn, soulcount)
	end
	
	CallAccountAutoTrans(Conn.m_Account, "EquipMgrDB", "GetPlayerSoulNum", CallBack, data)
end


local function SetEquipIntensifyAddAttrInfo(Conn, stoneID, result, charID)
    local player = Conn.m_Player
    local nEquipType = result["EquipType"]
    local sEquipName = result["EquipName"]
    local oldEquipRet = result["OldEquipInfoTbl"]
    local newEquipRet = result["NewEquipInfoTbl"]
    local uEquipId = result["EquipID"]
    local nEquipPart = result["EquipPart"]
    local oldIntensifyInfo = result["OldIntensifyInfo"]
    local newIntensifyInfo = result["NewIntensifyInfo"]
    local nRoomIndex2 = result["RoomIndex2"]
    local nPos2= result["Pos2"]
    local stoneID2 = result["StoneID2"]
	local oldequip = CEquip:new(uEquipId, player.m_uID, nEquipType, sEquipName)
	oldequip:SetIntensifyAddInfo(oldEquipRet)

	local newequip = CEquip:new(uEquipId, player.m_uID, nEquipType, sEquipName)
	newequip:SetIntensifyAddInfo(newEquipRet)
	newequip:SetIntensifyInfo(newIntensifyInfo)
	if(0 ~= nEquipPart) then --强化身上的装备
        local equipSuperaddInfo = result["EquipSuperaddInfo"]
		oldequip:DisableAddAttrPropty(player, nEquipPart, "SetEquipIntensifyAddAttrInfo")
		player:SetPlayerEquipPhase(nEquipPart, newequip.uIntensifyPhase, 7)
	    newequip.CurDuraValue = result["CurDuraValue"]
	    newequip.MaxDuraValue = result["MaxDuraValue"]
	    newequip:SetSuperaddInfo(uEquipId, equipSuperaddInfo)
		newequip:EnableAddAttrPropty(player, nEquipPart, "SetEquipIntensifyAddAttrInfo")
		Gas2Gac:UpdateModel(player.m_Conn,player:GetEntityID())
		Gas2Gac:UpdateModel(player:GetIS(0),player:GetEntityID())
		CGasFightingEvaluation.SaveFightingEvaluationInfo(player)
	end
	newequip:RetIntensifyAddInfo(charID)
	newequip:RetIntensifyInfo(charID)
	Gas2GacById:NoticeIntensifySuccess(charID, uEquipId, nEquipPart)
end

----通过道具强化的装备
function Gac2Gas:EquipIntensifyUseStone(Conn, uItemId, EquipPart, nRoomIndex, nPos )
    local charID  = Conn.m_Player.m_uID
    local data = {["ItemID"] = uItemId, ["RoomIndex"] = nRoomIndex, ["Pos"] = nPos,
                    ["CharID"] = charID,
					["sceneName"] = Conn.m_Player.m_Scene.m_SceneName,
					["EquipPart"] = EquipPart}

    local function CallBack(suc, stoneID, result )
    	if suc then
    		SetEquipIntensifyAddAttrInfo(Conn, stoneID, result, charID)	
    	else
    		local errorMsgNO = stoneID
    		local equipID = result
    		if errorMsgNO ~= nil then 
    			Gas2GacById:UpdateEquipIntensifyInfo(charID,equipID)
    			MsgToConn(Conn, errorMsgNO)
    		end
    	end
    end
	CallAccountManualTrans(Conn.m_Account, "EquipIntensifyDB", "EquipIntensifyUseStone", CallBack, data)
end

------------------------发送装备强化信息--------------------
function CEquip:SetIntensifyInfo(ret)
    if ret:GetRowNum() < 1 then
        return
    end
    self.IntensifySoulNum = ret(1,1)
	self.uAddAttr1 		= { ret(1,2), ret(1,3) }
	self.uAddAttr2 		= { ret(1,4) , ret(1,5)}
	self.uIntensifyPhase= ret(1,6)
	self.sSuitName= ret(1,7)
	self.uIntensifyTimes = ret(1,8)
	self.uIntensifyBackTimes = ret(1,9)
	self.uIntenTalentIndex  = ret(1,10) 
end

function CEquip:GetIntensifyInfo()
    if self.uAddAttr1 == nil then
        self.uAddAttr1 = {}
    end
    if self.uAddAttr2 == nil then
        self.uAddAttr2 = {}
    end
    local info = {
        self.m_uItemId,
        self.IntensifySoulNum or 0,
        self.uAddAttr1[1] or  "",self.uAddAttr1[2] or 0,
        self.uAddAttr2[1] or "",self.uAddAttr2[2] or 0,
	    self.uIntensifyPhase or 0,
	    self.sSuitName or "",
	    self.uIntensifyTimes or 0,
	    self.uIntensifyBackTimes or 0,
    	self.uIntenTalentIndex or 0
    }
    return info
end

function CEquip:RetIntensifyInfo(charID)
    Gas2GacById:RetEquipIntensifyInfo(charID, unpack(self:GetIntensifyInfo()))
end

-------------------------装备强化4~9阶段附加属性信息----------------------
function CEquip:SetIntensifyAddInfo(ret)
    if ret:GetRowNum() < 1 then
        return
    end
    self.AddAttr4       =   ret(1, 1)
    self.AddAttr4Value  =   ret(1, 2)
    self.AddAttr5       =   ret(1, 3)
    self.AddAttr5Value  =   ret(1, 4)
    self.AddAttr6       =   ret(1, 5)
    self.AddAttr6Value  =   ret(1, 6)
    self.AddAttr7       =   ret(1, 7)
    self.AddAttr7Value  =   ret(1, 8)
    self.AddAttr8       =   ret(1, 9)
    self.AddAttr8Value  =   ret(1, 10)
    self.AddAttr9       =   ret(1, 11)
    self.AddAttr9Value  =   ret(1, 12)
end

function CEquip:GetIntensifyAddInfo()
    local info = {
        self.m_uItemId,
        self.AddAttr4 or "",
        self.AddAttr4Value or 0,
        self.AddAttr5 or "",
        self.AddAttr5Value or 0,
        self.AddAttr6 or "" ,
        self.AddAttr6Value or 0,
        self.AddAttr7 or "",
        self.AddAttr7Value or 0, 
        self.AddAttr8 or "",
        self.AddAttr8Value or 0,
        self.AddAttr9 or "",
        self.AddAttr9Value or 0
    }   
    return info
end

function CEquip:RetIntensifyAddInfo(charID)
    Gas2GacById:RetEquipAddAttrInfo(charID, unpack(self:GetIntensifyAddInfo()))
end

--------------------------------------------------------------------------------
--装备护甲属性
function CEquip:SetEquipEnactmentInfo(ret)
	local atrr1,atrr2,atrr3,atrr4 = unpack(ret)
	self.ArmorPieceAttr1 = atrr1[1]
	self.ArmorPiece1AddTimes = atrr1[2]
	self.ArmorPieceAttr2 = atrr2[1]
	self.ArmorPiece2AddTimes = atrr2[2]
	self.ArmorPieceAttr3 = atrr3[1]
	self.ArmorPiece3AddTimes = atrr3[2]
	self.ArmorPieceAttr4 = atrr4[1]
	self.ArmorPiece4AddTimes = atrr4[2]
end


function CEquip:GetEquipEnactmentInfo()
	local info = {
	self.m_uItemId,
	self.ArmorPieceAttr1,
	self.ArmorPiece1AddTimes,
	self.ArmorPieceAttr2,
	self.ArmorPiece2AddTimes,
	self.ArmorPieceAttr3,
	self.ArmorPiece3AddTimes,
	self.ArmorPieceAttr4,
	self.ArmorPiece4AddTimes
	}
	return info
end


function CEquip:RetEnactmentInfo(charID)
    Gas2GacById:RetEquipEnactmentInfo(charID, unpack(self:GetEquipEnactmentInfo()))
end
-------------------------装备进阶产出的额外属性----------------------------------
function CEquip:SetAdvanceInfo(ret)
    if ret:GetRowNum() < 1 then
        return
    end
    self.CurAdvancePhase = ret(1,1)
    self.MaxAdvancePhase = ret(1,2)
    self.AdvanceSkillPiece1 = ret(1,3)
    self.AdvanceSkillPiece2 = ret(1,4)
    self.AdvanceSkillPiece3 = ret(1,5)
    self.AdvanceSkillPiece4 = ret(1,6)
    self.ChoosedAdvanceSkillPiece = ret(1,7)
    self.JingLingType       =   ret(1,8)
    self.AdvanceSoulNum     =   ret(1,9)
    self.AdvancedTimes  = ret(1,10)
    self.AdvancedAttrValueTbl= {ret(1,11), ret(1,12), ret(1,13), ret(1,14)}
    self.AdvanceSoul     =  ret(1, 15)
    self.AdvancedAttrTbl = {ret(1, 16), ret(1,17), ret(1,18), ret(1,19)}
end

function CEquip:GetAdvanceInfo()
    if self.AdvancedAttrValueTbl == nil then
       self.AdvancedAttrValueTbl = {} 
    end
    local info = {
        self.m_uItemId,
        self.CurAdvancePhase,
        self.MaxAdvancePhase,
        self.AdvanceSkillPiece1 or 0,
        self.AdvanceSkillPiece2 or 0,
        self.AdvanceSkillPiece3 or 0,
        self.AdvanceSkillPiece4 or 0,
        self.ChoosedAdvanceSkillPiece or 0,
        self.JingLingType or "",
        self.AdvanceSoulNum or 0,
        self.AdvancedTimes,
        self.AdvancedAttrValueTbl[1] or 0,
        self.AdvancedAttrValueTbl[2] or 0,
        self.AdvancedAttrValueTbl[3] or 0,
        self.AdvancedAttrValueTbl[4] or 0,
        self.AdvanceSoul,
        self.AdvancedAttrTbl[1] or "",
        self.AdvancedAttrTbl[2] or "",
        self.AdvancedAttrTbl[3] or "",
        self.AdvancedAttrTbl[4] or ""
    }
    return info
end

function CEquip:RetAdvanceInfo(charID)
    Gas2GacById:RetEquipAdvanceInfo(charID, unpack(self:GetAdvanceInfo()))
end

function CEquip:SetSuperaddInfo(equipID, superaddInfo)
    self.m_uItemId  = equipID
    self.m_CurSuperaddPhase = superaddInfo[1]
    self.m_SuperaddRate = superaddInfo[2]
end


function CEquip:GetEquipSuperaddInfo()
    local info = {
        self.m_uItemId,
        self.m_CurSuperaddPhase,
        self.m_SuperaddRate
    }
    return info
end

function CEquip:RetSuperaddRate(charID)
    Gas2GacById:RetEquipSuperaddRate(charID, self.m_uItemId, self.m_CurSuperaddPhase,  self.m_SuperaddRate)
end

function CEquip:GetEquipSuperRate(rate)
	return 1 + rate/100
end

function CEquip:EnableAdvanceAttr(player, eEquipPart, createSource, equipLevel)    
    if not player.m_ErrorEquipTbl[eEquipPart] then
	    local transID = self:CreateTranID(player, eEquipPart, "AdvancedPropty", createSource)
			local AdvancedRoot = g_ItemInfoMgr:GetItemInfo(self.m_uItemType, self.m_sItemName,"AdvancedRoot")
	    if equipLevel == nil then
	        equipLevel = self.BaseLevel
	    end
	    --local attrlTbl = GetAdvancedAttribute(AdvancedRoot, self.CurAdvancePhase, equipLevel) --进阶前装备的进阶属性表
	    local attrlTbl = self.AdvancedAttrTbl
	    local attrValueNum = # self.AdvancedAttrValueTbl
	    local attrNum = # attrlTbl
	    local attrInfoNum = attrNum
	    if attrValueNum < attrNum then
	        attrInfoNum = attrValueNum
	    end
	    for i=1, attrInfoNum do
	    	local attr1 = attrlTbl[i]
	    	local attr1_var = math.ceil(self.AdvancedAttrValueTbl[i] * self:GetEquipSuperRate(self.m_SuperaddRate))
	    	if attr1 ~= "" and attr1_var ~= 0 then
	    		player:ChangeTransactionPropertyValue(transID, GetPropertyAdderName(attr1),attr1_var)
	    	end
	    end
	    player:CommitTransaction(transID)
	    self:SetAttrStateByDuraState(player, transID)  
	end
end

function CEquip:DisableAdvanceAttr(player, equipPart, destroySource)
	if not player.m_ErrorEquipTbl[equipPart]  then
    	self:DestroyTranID(player, equipPart, "AdvancedPropty", destroySource)
    end
end

local function EquipAdvanceCallBack(Conn, result, charID)
	if not IsCppBound(Conn.m_Player) then
		return
	end
    local equipID = result["EquipID"]
    local equipPart = result["EquipPart"] 
    local advancedStoneRoom = result["RoomIndex"] 
    local advancedStonePos = result["Pos"] 
    local stoneID = result["StoneID"] 
    local equipLevel = result["EquipLevel"]
    local oldEquipAdvanceSkillPieceTbl = result["OldEquipAdvanceSkillPieceTbl"]
    local newEquipAdvanceSkillPieceTbl = result["NewEquipAdvanceSkillPieceTbl"]
    local advancedStoneRoom2 = result["RoomIndex2"] 
    local advancedStonePos2 = result["Pos2"] 
    local stoneID2  =   result["StoneID2"]
    local equipType = result["EquipType"]
    local equipName = result["EquipName"]
    local oldequip = CEquip:new(equipID, charID,equipType, equipName)
    oldequip:SetAdvanceInfo(oldEquipAdvanceSkillPieceTbl)
    local newequip = CEquip:new(equipID, charID,equipType, equipName)
    newequip:SetAdvanceInfo(newEquipAdvanceSkillPieceTbl)
    newequip:RetAdvanceInfo(charID)
    if equipPart ~= 0 then
        local equipSuperaddInfo = result["EquipSuperaddInfo"]
        oldequip:DisableAdvanceAttr(Conn.m_Player, equipPart, "EquipAdvanceCallBack")
        newequip.CurDuraValue = result["CurDuraValue"]
        newequip.MaxDuraValue = result["MaxDuraValue"]
        newequip:SetSuperaddInfo(equipID, equipSuperaddInfo)
        newequip:EnableAdvanceAttr(Conn.m_Player, equipPart, "EquipAdvanceCallBack", equipLevel)
        Conn.m_Player:SetPlayerEquipPhase(equipPart, 0, newequip.CurAdvancePhase)
        CGasFightingEvaluation.UpdateFightingEvaluationInfo(Conn.m_Player)
    end
    Gas2GacById:NoticeAdvanceSuccess(charID, equipID)
    local total_soul = result["TotalSoulNum"]
    Conn.m_Player.m_uSoulValue = total_soul + Conn.m_Player.m_uKillDropSoulCount
    Gas2GacById:ReturnModifyPlayerSoulNum(charID,Conn.m_Player.m_uSoulValue)
end
local function CheckEquipCanBeModified(Conn, equipPart, msgID1, msgID2)
    if(not Conn.m_Player) then 
        return false
    end
	if Conn.m_Player:IsInBattleState() and equipPart ~= 0 then
		MsgToConn(Conn, msgID1)
		return false
	end
	if Conn.m_Player:IsInForbitUseWeaponState() and equipPart ~= 0 then
		MsgToConn(Conn, msgID2)
		return false
	end   
	return true
end

function Gac2Gas:EquipAdvance(Conn, equipID, equipPart, stoneRoomIndex1, stonePos1)
    local ret = CheckEquipCanBeModified(Conn, equipPart, 1033, 1034)
    if ret ~= true then
        if not IsCppBound(Conn.m_Player) then
            return 
        end
        Gas2Gac:NoticeAdvanceError(Conn, equipID)
        Gas2Gac:UpdateModel(Conn,Conn.m_Player:GetEntityID())
		Gas2Gac:UpdateModel(Conn.m_Player:GetIS(0),Conn.m_Player:GetEntityID())
        return
    end
    Conn.m_Player:OnSavePlayerSoulTickFunc(nil,Conn.m_Player)
    local charID = Conn.m_Player.m_uID
    local data = {["EquipID"] = equipID, ["EquipPart"]=equipPart, ["RoomIndex1"]=stoneRoomIndex1,
                   ["Pos1"] = stonePos1, ["CharID"] =charID,["sceneName"] = Conn.m_Player.m_Scene.m_SceneName }
    local function CallBack(suc, result)
        if suc then
            EquipAdvanceCallBack(Conn, result, charID)
            local msgID = result["MessageID"]
            local backPhase = result["BackPhase"]
            if backPhase ~= 0 then
               MsgToConn(Conn, msgID, backPhase)
            else
               MsgToConn(Conn, msgID)
            end
        else
            local errMsgID = result["MsgID"]
            local equipID  = result["EquipID"]
            Gas2GacById:NoticeAdvanceError(charID, equipID)
            MsgToConn(Conn, errMsgID)
        end
        if IsCppBound(Conn.m_Player) then
            Gas2Gac:UpdateModel(Conn,Conn.m_Player:GetEntityID())
		    Gas2Gac:UpdateModel(Conn.m_Player:GetIS(0),Conn.m_Player:GetEntityID())
		end
    end

    CallAccountManualTrans(Conn.m_Account, "EquipAdvanceDB", "EquipAdvance", CallBack, data)
end

function Gac2Gas:ActiveJingLingSkillPiece(Conn, equipID, choosedActiveIndex,equipPart)
    local ret = CheckEquipCanBeModified(Conn, equipPart, 1039, 1040)
    if ret ~= true then
        return
    end
    if not IsCppBound(Conn.m_Player) then
        return 
    end
    local charID = Conn.m_Player.m_uID
    local data = {["CharID"] = charID, ["EquipID"]= equipID, ["ChoosedActiveIndex"]=choosedActiveIndex, ["EquipPart"]=equipPart,
    							["sceneName"] = Conn.m_Player.m_Scene.m_SceneName}
    Conn.m_Player:OnSavePlayerSoulTickFunc(nil,Conn.m_Player)
    local function CallBack(suc, result)
        if suc then
            if equipPart ~= 0 then
                local equip_mgr =  g_GetEquipMgr()
                local skillIndex = result["DelJingLingSkillIndex"]
                equip_mgr:SetCharJingLingSkill(Conn.m_Player, false, skillIndex, charID)     
            end
            local curActvieIndex = result["ChoosedActiveIndex"]
            Gas2GacById:RetActiveJingLingSkillPiece(charID,equipID,curActvieIndex,equipPart)
            local total_soul = result["TotalSoulNum"]
             Conn.m_Player.m_uSoulValue = total_soul + Conn.m_Player.m_uKillDropSoulCount
            Gas2GacById:ReturnModifyPlayerSoulNum(charID,Conn.m_Player.m_uSoulValue)
        else
            local errMsgID = result
            MsgToConn(Conn, errMsgID) 
        end
    end
    
    CallAccountManualTrans(Conn.m_Account, "EquipAdvanceDB", "ActiveJingLingSkillPiece", CallBack, data)
end

---进阶的装备重生
function Gac2Gas:AdvancedEquipReborn(Conn, equipID, equipPart)
    local ret = CheckEquipCanBeModified(Conn, equipPart, 1043, 1044)
    if ret ~= true then
        return
    end

    local charID = Conn.m_Player.m_uID
    local data = {["EquipID"] = equipID, ["EquipPart"]=equipPart, ["CharID"] =charID ,["sceneName"] = Conn.m_Player.m_Scene.m_SceneName }
    Conn.m_Player:OnSavePlayerSoulTickFunc(nil, Conn.m_Player)
    local function CallBack(suc, result)
        if suc then
            local equipID = result["EquipID"]
            local equipPart = result["EquipPart"]
            local newAdvanceInfo = result["NewAdvanceInfo"]
            local newEquip = CEquip:new(equipID)
            newEquip:SetAdvanceInfo(newAdvanceInfo)
            newEquip:RetAdvanceInfo(charID)
            if equipPart ~= 0 then
                local oldActiveJingLingSkill = result["OldActiveJingLingSkill"]
                local upActiveJingLingSkillFlag = result["UpActiveJingLingSkillFlag"]
                if upActiveJingLingSkillFlag == true and oldActiveJingLingSkill ~= nil and oldActiveJingLingSkill ~= 0 then
                   --取消技能效果
                   local equip_mgr =  g_GetEquipMgr()
                   equip_mgr:SetCharJingLingSkill(Conn.m_Player, false, oldActiveJingLingSkill, charID)
                end
                Gas2GacById:RetActiveJingLingSkillPiece(charID,equipID,0,equipPart)
            end
            Gas2GacById:AdvancedEquipReborn(charID,equipID) 
            local total_soul = result["TotalSoulNum"]
            if result["CurAdvancePhase"] >= 2 and IsCppBound(Conn.m_Player) then
                Conn.m_Player.m_uSoulValue = total_soul + Conn.m_Player.m_uKillDropSoulCount
                Gas2GacById:ReturnModifyPlayerSoulNum(charID,Conn.m_Player.m_uSoulValue)
            end
        else
            local msgID = result
            MsgToConn(Conn, msgID)
        end
    end

    CallAccountManualTrans(Conn.m_Account, "EquipAdvanceDB", "AdvancedEquipReborn", CallBack, data)
end

function Gac2Gas:ActiveJingLingSkill(Conn, newActiveSkillIndex)
    local ret = CheckEquipCanBeModified(Conn, 1, 1041, 1042)    --1的作用是起到不能激活精灵技能的标志位
    if ret ~= true then
        return
    end
    local charID = Conn.m_Player.m_uID
    local data = {["NewSkillIndex"] =newActiveSkillIndex, ["CharID"]=charID,["sceneName"] = Conn.m_Player.m_Scene.m_SceneName  }
    local function CallBack(suc, result)
        if suc then
            local NewActiveSkillIndex = result["NewActiveSkillIndex"]
            local oldActiveSkillIndex = result["OldActiveSkillIndex"]
            local equip_mgr =  g_GetEquipMgr()
            equip_mgr:SetCharJingLingSkill(Conn.m_Player, false, oldActiveSkillIndex, charID)
            equip_mgr:SetCharJingLingSkill(Conn.m_Player, true, NewActiveSkillIndex, charID)
        else
            local msgID = result
            MsgToConn(Conn, msgID)
        end
    end
    CallAccountManualTrans(Conn.m_Account, "EquipAdvanceDB", "ActiveJingLingSkill", CallBack, data)
end

--根据bFlag的值决定添加还是删除精灵技能，true为添加，false为取消
function CEquipMgr:SetCharJingLingSkill(player, bFlag, skillIndex, charID)
    if not IsCppBound(player) then
       return 
    end
    local skillParam = AttributeOfJingLingSkill_Common(skillIndex)
    if skillParam ~= nil then
        local skillName = skillParam("SkillName")
        local skillLevel = player:CppGetLevel()
        if  bFlag  then
            player:AddSkill(skillName, skillLevel)  
            Gas2GacById:RetActiveJingLingSkill(charID,skillIndex)
        else
            player:RemoveSkill(skillName)
            Gas2GacById:RetActiveJingLingSkill(charID,0)
        end
    end
end 

------------------装备耐久-----------------------

function CEquipMgr:RenewEquipDuraValue(Conn, result, charID)
    local equipID   =   result["EquipID"]
    local equipPart =   result["EquipPart"]
    if equipPart == 0 then
        self:RenewEquipDuraValueInBag(Conn, result)
        return 
    end
    local nBigID    =   result["EquipType"]   
    local nIndex    =   result["EquipName"]
    local oldDuraState =   result["OldDuraState"]
    local equipRet  =   result["EquipRet"]
    local bHaveDemagedEquip = result["bHaveDemagedEquip"]
    local maxDuraValue = result["MaxDuraValue"]
    local shortCutRet = result["ShortCutRet"]
    local newCommonlySkillName = result["NewCommonlySkillName"]
    Gas2Gac:RetEquipDuraValue(Conn, equipID, equipPart, 2, maxDuraValue, maxDuraValue)
    if oldDuraState == 0 then
        local equip = self:GetEquipFromRet(equipID, Conn.m_Player.m_uID, nBigID, nIndex, equipRet)
        --equip:EnablePropty(Conn.m_Player, equipPart)
        equip:EnableTran(Conn.m_Player, equipPart)
        if nBigID ==  5 then
            if equipPart == EEquipPart.eWeapon then
                local CommonlySkillName = equip:SetWeaponSkill(Conn.m_Player,nBigID,nIndex,false)
        		Gas2Gac:ReWeaponAttr(Conn,true,CommonlySkillName)
                Gas2Gac:ReCommonlySkill(Conn,newCommonlySkillName)
		        Conn.m_Player:GetShortInfoByRet(shortCutRet)
            elseif equipPart == EEquipPart.eAssociateWeapon then
                equip:SetAssociateSkill(Conn.m_Player,nBigID, nIndex,false)	
            end
        end
    end
    if bHaveDemagedEquip == 0 then
        Gas2Gac:RetShowEquipDuraWnd(Conn, false)
    end
    local liuTongJin = result["Money"]
    local bindingJin = result["BindingMoney"]
    Gas2GacById:RetRenewEquipSuc(charID, equipPart, nBigID, nIndex, equipID)
end

function CEquipMgr:RenewEquipDuraValueInBag(Conn, result, charID)
    local equipID = result["EquipID"]
    local equipPart = result["EquipPart"]
    local nBigID    = result["EquipType"]
    local nIndex    = result["EquipName"]
    local roomIndex = result["RoomIndex"]
    local pos       = result["Pos"]
    local curDuraValue = result["CurDuraValue"]
    local MaxDuraValue = result["MaxDuraValue"]
    local choosedMoneyType = result["ChoosedMoneyType"]
    Gas2Gac:RetEquipDuraValueInBag(Conn, equipID, roomIndex, pos, MaxDuraValue, MaxDuraValue)
end

--恢复选中装备当前耐久值
function Gac2Gas:RenewEquipDuraValue(Conn, equipPart, equipID, choosedMoneyType)
	if not DistanceJudgeBetweenNpcAndPlayer(Conn,"修理匠师")	then
		return
	end
    local ret = CheckEquipCanBeModified(Conn, equipPart, 1045, 1046)
    if ret ~= true then
        return
    end
    if Conn.m_Player.m_ItemBagLock then
        MsgClient(160045)
        return
    end
    local charID = Conn.m_Player.m_uID
    local data = {["EquipPart"] =equipPart , ["EquipID"]=equipID,["CharID"]=charID,
                   ["ChoosedMoneyType"] = choosedMoneyType, ["CheckDemagedEquip"] = true}  
    local function CallBack(suc, result)
        if suc then
            local equip_mgr =  g_GetEquipMgr()
            equip_mgr:RenewEquipDuraValue(Conn, result, charID)
            Gas2Gac:RefreshListCtrlInRole(Conn)
           	Gas2Gac:RefreshListCtrlInBag(Conn)
			CGasFightingEvaluation.UpdateFightingEvaluationInfo(Conn.m_Player)
            MsgToConn(Conn, 187003)
        else
            local errMsgID = result	
            MsgToConn(Conn, errMsgID) 
        end
    end
    if equipPart == 0 then
       data["GetPosFlag"] = true 
    end
    CallAccountManualTrans(Conn.m_Account, "EquipDurabilityDB", "RenewEquipDuraValue", CallBack, data)
end

--仅恢复背包里装备的耐久值
function Gac2Gas:RenewEquipInBagDuraValue(Conn, choosedMoneyType)
	if not DistanceJudgeBetweenNpcAndPlayer(Conn,"修理匠师")	then
		return
	end
    local ret = CheckEquipCanBeModified(Conn, 1, 1045, 1046)
    if ret ~= true then
        return
    end
    if Conn.m_Player.m_ItemBagLock then
        MsgClient(160045)
        return
    end
    local charID = Conn.m_Player.m_uID
    local data = {["CharID"] =charID , ["ChoosedMoneyType"]=choosedMoneyType }
    local function CallBack(suc, result)
        if suc then
            local equip_mgr =  g_GetEquipMgr()
            local equipCountInBag = # result["EquipInBag"]
            for i=1, equipCountInBag do
                equip_mgr:RenewEquipDuraValueInBag(Conn, result["EquipInBag"][i], charID)
            end
            Gas2Gac:RefreshListCtrlInBag(Conn)
            MsgToConn(Conn, 187003) 
        else
            local errMsgID = result
            MsgToConn(Conn, errMsgID) 
        end
        Gas2GacById:RenewAllEquipEnd(charID)
    end

    local ret = CallAccountManualTrans(Conn.m_Account, "EquipDurabilityDB", "RenewEquipInBagDuraValue", CallBack, data)
    if ret == 1 then
        Gas2Gac:RenewAllEquipEnd(Conn)
    end
end

--仅恢复角色身上的装备当前耐久值
function Gac2Gas:RenewEquipOnWearingDuraValue(Conn, choosedMoneyType)
	if not DistanceJudgeBetweenNpcAndPlayer(Conn,"修理匠师")	then
		return
	end
    local ret = CheckEquipCanBeModified(Conn, 1, 1045, 1046)
    if ret ~= true then
        return
    end
    if Conn.m_Player.m_ItemBagLock then
        MsgClient(160045)
        return
    end
    local charID = Conn.m_Player.m_uID
    local data = {["CharID"] =charID , ["ChoosedMoneyType"]=choosedMoneyType }
    local function CallBack(suc, result)
        if suc then
            local equip_mgr =  g_GetEquipMgr()
            local equipCount = # result["EquipOnWearing"]
            for i=1, equipCount do
                equip_mgr:RenewEquipDuraValue(Conn, result["EquipOnWearing"][i], charID)
            end
            Gas2Gac:RetShowEquipDuraWnd(Conn, false)
            Gas2Gac:RefreshListCtrlInRole(Conn)
            CGasFightingEvaluation.UpdateFightingEvaluationInfo(Conn.m_Player)
            MsgToConn(Conn, 187003) 
        else
            local errMsgID = result
            MsgToConn(Conn, errMsgID) 
        end
        Gas2GacById:RenewAllEquipEnd(charID)
    end

    local ret = CallAccountManualTrans(Conn.m_Account, "EquipDurabilityDB", "RenewEquipInRoleDuraValue", CallBack, data)
    if ret == 1 then
        Gas2Gac:RenewAllEquipEnd(Conn)
    end
end

--恢复身上所有装备当前耐久值
function Gac2Gas:RenewAllEquipDuraValue(Conn, choosedMoneyType)
	if not DistanceJudgeBetweenNpcAndPlayer(Conn,"修理匠师")	then
		return
	end
    local ret = CheckEquipCanBeModified(Conn, 1, 1045, 1046)
    if ret ~= true then
        return
    end
    if Conn.m_Player.m_ItemBagLock then
        MsgClient(160045)
        return
    end
    local charID = Conn.m_Player.m_uID
    local data = {["CharID"] =charID , ["ChoosedMoneyType"]=choosedMoneyType }
    local function CallBack(suc, result)
        if suc then
            local equip_mgr =  g_GetEquipMgr()
            local equipCount = # result["EquipOnWearing"]
            local equipCountInBag = # result["EquipInBag"]
            for i=1, equipCount do
                equip_mgr:RenewEquipDuraValue(Conn, result["EquipOnWearing"][i], charID)
            end
            Gas2Gac:RetShowEquipDuraWnd(Conn, false)
            for i=1, equipCountInBag do
                equip_mgr:RenewEquipDuraValueInBag(Conn, result["EquipInBag"][i], charID)
            end
            CGasFightingEvaluation.UpdateFightingEvaluationInfo(Conn.m_Player)
            MsgToConn(Conn, 187003) 
        else
            local errMsgID = result
            MsgToConn(Conn, errMsgID) 
        end
        Gas2GacById:RenewAllEquipEnd(charID)
    end

    local ret = CallAccountManualTrans(Conn.m_Account, "EquipDurabilityDB", "RenewAllEquipDuraValue", CallBack, data)
    if ret == 1 then
        Gas2Gac:RenewAllEquipEnd(Conn)
    end
end

function CEquip:DisableTran(player, equipPart)
    local attrTranIDTbl = player.m_EquipAttrTransIDTbl[equipPart]
    if attrTranIDTbl ~= nil then
        for i, v in pairs(attrTranIDTbl) do
            local transID = v["TranID"]
            player:DisableTransaction(transID)
        end
    end
end

function CEquip:EnableTran(player, equipPart)
    local attrTranIDTbl = player.m_EquipAttrTransIDTbl[equipPart]
    if attrTranIDTbl ~= nil then
        for i, v in pairs(attrTranIDTbl) do
            local transID = v["TranID"]
            player:EnableTransaction(transID)
        end
    end
end

---得到装备当前的耐久状态
--（装备当前的耐久值/耐久上限的比例值）0表示为0；1表示小于30%但大于0; 2表示大于30%
function CEquipMgr:GetEquipDuraState(Conn, equipID, duraState, equipPart, equipRet, nBigID, nIndex, curDuraValue, maxDuraValue, shortCutRet, newCommonlySkillName)
    Gas2Gac:RetEquipDuraValue(Conn, equipID, equipPart, duraState, curDuraValue, maxDuraValue)
    if duraState ==  0 then  
        local equip = self:GetEquipFromRet(equipID, Conn.m_Player.m_uID, nBigID, nIndex, equipRet)
        --equip:DisablePropty(Conn.m_Player, equipPart)
        equip:DisableTran(Conn.m_Player, equipPart)
        if nBigID ==  5 then
            if equipPart == EEquipPart.eWeapon then
                local CommonlySkillName  = g_GetPlayerClassNameByID(Conn.m_Player.m_Class).. "普通攻击"
                Conn.m_Player:CppInitMHNA(CommonlySkillName,"无类型","",2.0,false)
                Gas2Gac:ReWeaponAttr(Conn,false,CommonlySkillName)
                Gas2Gac:ReCommonlySkill(Conn,newCommonlySkillName)
		        Conn.m_Player:GetShortInfoByRet(shortCutRet)
            elseif equipPart == EEquipPart.eAssociateWeapon then
                Conn.m_Player:CppInitAHNA(nil,"无类型","",0,false)
            end
        end
    end
end


--发送装备信息，不管切服后消息是否能收到（角色首次登陆游戏时，也需要调用该接口）
function Weapon:SendEquipInfoUsingConn(Conn)
	Gas2Gac:RetCommonWeaponInfo(Conn, unpack(self:Info()))
	self:RetAdvanceInfoUsingConn(Conn)
    self:RetIntensifyInfoUsingConn(Conn)
    self:RetIntensifyAddInfoUsingConn(Conn)
    self:RetEnactmentInfoUsingConn(Conn)
    self:RetSuperaddRateUsingConn(Conn)
end

function Armor:SendEquipInfoUsingConn(Conn)
	Gas2Gac:RetCommonArmorInfo(Conn, unpack(self:Info()))
	self:RetAdvanceInfoUsingConn(Conn)
    self:RetIntensifyInfoUsingConn(Conn)
    self:RetIntensifyAddInfoUsingConn(Conn)
    self:RetEnactmentInfoUsingConn(Conn)
    self:RetSuperaddRateUsingConn(Conn)
end

function Shield:SendEquipInfoUsingConn(Conn)
	Gas2Gac:RetCommonShieldInfo(Conn, unpack(self:Info()))
	self:RetAdvanceInfoUsingConn(Conn)
    self:RetIntensifyInfoUsingConn(Conn)
    self:RetIntensifyAddInfoUsingConn(Conn)
    self:RetEnactmentInfoUsingConn(Conn)
    self:RetSuperaddRateUsingConn(Conn)
end

function Ring:SendEquipInfoUsingConn(Conn)
	Gas2Gac:RetCommonRingInfo(Conn, unpack(self:Info()))
	self:RetAdvanceInfoUsingConn(Conn)
    self:RetIntensifyInfoUsingConn(Conn)
    self:RetIntensifyAddInfoUsingConn(Conn)
    self:RetEnactmentInfoUsingConn(Conn)
    self:RetSuperaddRateUsingConn(Conn)
end

function CEquip:RetAdvanceInfoUsingConn(Conn)
    Gas2Gac:RetEquipAdvanceInfo(Conn, unpack(self:GetAdvanceInfo()))
end

function CEquip:RetIntensifyAddInfoUsingConn(Conn)
    Gas2Gac:RetEquipAddAttrInfo(Conn, unpack(self:GetIntensifyAddInfo()))
end

function CEquip:RetIntensifyInfoUsingConn(Conn)
    Gas2Gac:RetEquipIntensifyInfo(Conn, unpack(self:GetIntensifyInfo()))
end

--装备护甲信息
function CEquip:RetEnactmentInfoUsingConn(Conn)
    Gas2Gac:RetEquipEnactmentInfo(Conn, unpack(self:GetEquipEnactmentInfo()))
end

function CEquip:RetSuperaddRateUsingConn(Conn)
    Gas2Gac:RetEquipSuperaddRate(Conn, unpack(self:GetEquipSuperaddInfo()))
end

function Gac2Gas:UseEquipIntenBackItem(Conn, equipID, equipPart, equipRoom, equipPos, roomIndex, nPos, intenBackItemType, intenBackItemName)
  	-- 判断是否在战斗状态	
	if Conn.m_Player == nil then
	   return  
	end
    if Conn.m_Player:IsInBattleState() and nEquipPart ~= 0 then
        MsgToConn(Conn, 180012)
        return 
    end
	--判断是否在缴械状态
	if Conn.m_Player:IsInForbitUseWeaponState() and nEquipPart ~= 0  then
	    MsgToConn(Conn, 180013)
	   return  
	end
	local charID = Conn.m_Player.m_uID
	local data = {["CharID"]= charID, ["EquipID"]= equipID, ["EquipPart"] = equipPart,
	["RoomIndex"] = roomIndex, ["Pos"] = nPos, ["IntenBackItemType"] =intenBackItemType ,  
	["IntenBackItemName"] =intenBackItemName, ["SceneName"] = Conn.m_Player.m_Scene.m_SceneName	}  
    
    
    local function CallBack(sucFlag, result, total_soul)
        if sucFlag  then
            EquipIntensifyBack(Conn, result, total_soul, charID)
            MsgToConn(Conn, 180000)
        else
            local errMsgID = result
            MsgToConn(Conn, errMsgID) 
            Gas2GacById:UpdateEquipIntensifyInfo(charID, equipID)
        end
        
        Gas2GacById:RetEquipIntenBack(charID, equipRoom, equipPos, equipPart, equipID)
    end    
    CallAccountManualTrans(Conn.m_Account, "EquipIntensifyDB", "UseEquipIntenBackItem", CallBack, data)	
end

