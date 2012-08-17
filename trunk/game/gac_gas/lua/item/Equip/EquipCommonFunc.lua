cfg_load "equip/EquipDefence_Common"
cfg_load "equip/WeaponDPSFloor_Common"
cfg_load "equip/ShieldBaseValue_Common"
cfg_load "equip/PropertyName_Common"
cfg_load "equip/upgrade/EquipUpgrade_Common"
cfg_load "item/Equip_ShieldSProperty_Common"
cfg_load "equip/intensify/SuitPartMapTalentOrSkill_Common"
cfg_load "equip/advance/JingLingTypeToPieceSkill_Common"
cfg_load "item/AdvanceStone_Common"
cfg_load "equip/advance/OutDataOfAdvancedEquip_Common"
cfg_load "equip/advance/AttributeOfJingLingSkill_Common"
cfg_load	"equip/suit/ArmyActSuit_Common"			--军法
cfg_load	"equip/suit/ArmyWarSuit_Common"			--军战
cfg_load    "equip/suit/MagicActSuit_Common"		--法魔
cfg_load    "equip/suit/GodWartSuit_Common"			--战神
cfg_load "equip/advance/SucProbOfEquipAdvance_Server"
lan_load "equip/advance/Lan_AttributeOfJingLingSkill_Common"
cfg_load "equip/HandType_Common"
cfg_load "equip/MagicBallBaseValue_Common"
cfg_load "equip/suit/EquipSuit_Common"
cfg_load "equip/WeaponDPSValue_Common"
cfg_load "equip/ArmorProValue_Common"
cfg_load "equip/ArmorBaseValue_Common"
gac_gas_require "item/Equip/EquipChecking"
gac_gas_require "item/Equip/ArmorOutputRandomScale"
cfg_load "equip/EquipRefineNeedStoneCount_Common"
cfg_load "item/EquipSuperaddItem_Common"
cfg_load "equip/superadd/EquipSuperaddConsume_Common"
cfg_load "equip/superadd/EquipSuperaddMaxPhase_Common"

--现在装备产出时可能的三种属性是从五种总共的里边选出三种
local OutputAttrProbability = 5 

function GetEquipHandType(EquipType)
	local hand = string.sub(EquipType,1,2)
	if HandType_Common(hand) then
		return HandType_Common(hand,"HandType")
	else
		return ""
	end
end

--获取强化、进阶等时所需要的魂数
function GetIntensifyNeededSoul(level, euqipType, vanishType)
	local rate = 1
	if euqipType ~=nil and  GetEquipHandType(euqipType) == "双" then
		rate = 2
	end
	if vanishType == nil then
	    return math.floor(EquipUpgrade_Common(level,"VanishSoulCount") * rate)
	else
	    return math.floor(EquipUpgrade_Common(level,vanishType) * rate)
	end
end

--获取升级时候产生的防御增量
function GetDefenceAddon(level)
	return EquipDefence_Common(level,"BaseDefenceAddon")
end

--获取基础防御值
function GetBaseDefence(level)
	return EquipDefence_Common(level,"BaseDefence")
end

--得到武器的最大和最小攻击力
function GetWeaponDPSFloat(AttackSpeed,DPS,DPSFloorRate)
	return math.floor(DPS*AttackSpeed*(1 + DPSFloorRate/100)), math.floor(DPS*AttackSpeed*(1 - DPSFloorRate/100))
end

--获取基础武器的DPS
function GetBaseWeaponDPS(level,EquipType)
	local DPS = WeaponDPSValue_Common(level,"DPSBaseValue") * WeaponDPSFloor_Common(EquipType,"DPSRate")/100
	DPS = string.format("%.1f",DPS)
	return DPS
end


--根据装备产出时对应的属性比例值，按照武器装备魂系统需求文档的计算公式
--计算出对应的：生命值、护甲值、自然抗值、破坏抗值和暗黑抗值的数目
--参数：装备的相关属性, 装备等级
--返回值：装备产出的三个属性
function GetEquipOutputAttr( nBigId, sName, BaseLevel )
	local EquipType = g_ItemInfoMgr:GetItemInfo(nBigId, sName,"EquipType")
	local EquipPart = g_ItemInfoMgr:GetItemInfo(nBigId, sName,"EquipPart")
	local outputAttr = {}	
	local defenceScale = ArmorBaseValue_Common(BaseLevel,"ArmorBaseValue")	--防御量
	local index = 1
	local havelifeAttr = 0		--判断是否有生命值这个属性，便于给剩下的几个属性加随机附加值	
	local randomAddAttr = ArmorProValue_Common(EquipType,EquipPart,"RandomScale")	--随机值比例
	local armorAttrTempTbl = {"HealthPoint", "Defence", "NatureResistanceValue", "DestructionResistanceValue", "EvilResistanceValue"}
	randomAddAttr =  defenceScale * randomAddAttr/100 
	for i =1, OutputAttrProbability do
		if ArmorProValue_Common(EquipType,EquipPart,armorAttrTempTbl[i]) ~= "" then
			outputAttr[index] = ArmorProValue_Common(EquipType,EquipPart,armorAttrTempTbl[i])
			outputAttr[index] = defenceScale * outputAttr[index]/100
			index = index + 1
			if i == 1 then
				havelifeAttr = 1
			end
		end
	end
	local value1RandomScale, value2RandomScale, value3RandomScale = GetRandomScale()
	--如果产出值是生命值，则产出值+随机值*2，其他的是产出值+随机值*比例值
	if havelifeAttr ==  1 then
		outputAttr[1] = tonumber(string.format("%.0f", outputAttr[1] + 2 * value1RandomScale * randomAddAttr))
	else
		outputAttr[1] = tonumber(string.format("%.0f", outputAttr[1] + value1RandomScale*randomAddAttr))
	end
	outputAttr[2] = tonumber(string.format("%.0f", outputAttr[2] + value2RandomScale*randomAddAttr))
	outputAttr[3] = tonumber(string.format("%.0f", outputAttr[3] + value3RandomScale*randomAddAttr))
	return outputAttr[1], outputAttr[2], outputAttr[3]
end

function GetPropertyName(AttrName)
	return PropertyName_Common(AttrName,"EPropertyName")
end

--得到属性名（中文）
function GetAttrName(PropertyName)
	local Keys = PropertyName_Common:GetKeys()
	for i ,p in pairs(Keys) do
		local RowInfo = PropertyName_Common(p)
		if RowInfo("EPropertyName") == PropertyName then
			return p 
		end
	end
	if PropertyName ~= nil then
		LogErr("报错属性名称1","报错属性名称" .. PropertyName)
	else
		LogErr("报错属性名称为nil")
	end
	return ""
end

function GetPropertyAdderName(AttrName)
	--assert(PropertyName_Common[AttrName],"报错属性名称" .. AttrName)
	if AttrName ~= nil and PropertyName_Common(AttrName) == nil then
	   LogErr("报错属性名称2","报错属性名称" .. AttrName)
	   return ""
	elseif PropertyName_Common(AttrName) == nil  then
		LogErr("报错属性名称nil")
		return ""
	end
	return PropertyName_Common(AttrName,"PropertyAdder")
end

--得到盾牌的格挡率和格挡值
function GetShieldBlock(CurLevel,nBigID, nIndex)
	local BlockRate,BlockValue
	local IsStatic	= g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"IsStatic")
	if IsStatic == 0 or IsStatic == "" then
		BlockRate = ShieldBaseValue_Common(CurLevel,"BlockRate")/100
		BlockValue = ShieldBaseValue_Common(CurLevel,"BlockValue")
	elseif IsStatic == 1 then
		local ShieldStaticPropertyInfo = Equip_ShieldSProperty_Common(nIndex)
		BlockRate = ShieldStaticPropertyInfo("BlockRate")/100
		local curUpgradeFun = EquipUpgrade_Common(CurLevel,"UpgradeParameter")
		local nextUpgradeFun = EquipUpgrade_Common(CurLevel,"UpgradeParameter")
		BlockValue = string.format("%.1f",ShieldStaticPropertyInfo("BlockValue")/curUpgradeFun * nextUpgradeFun)
	end
	return BlockRate,BlockValue
end

function GetShieldMagicDamage(CurLevel,nBigID,nIndex)
	local DamageValue
	local IsStatic	= g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"IsStatic")
	if IsStatic == 0 or IsStatic == "" then
		DamageValue = MagicBallBaseValue_Common(CurLevel,"MagicDamageValue")
	elseif IsStatic == 1 then
		local ShieldStaticPropertyInfo = Equip_ShieldSProperty_Common(nIndex)
		local curUpgradeFun = EquipUpgrade_Common(CurLevel,"UpgradeParameter")
		local nextUpgradeFun = EquipUpgrade_Common(CurLevel,"UpgradeParameter")
		DamageValue = string.format("%.1f",ShieldStaticPropertyInfo("MagicDamageValue")/curUpgradeFun * nextUpgradeFun)
	end	
	return DamageValue
end

--得到戒指静态属性的名称和数值
function GetRingStaticPropertyName(Info)
	if Info == nil then
		return 0,0
	end
	local PropertyNameTbl = {"StrikeValue","StrikeMultiValue","AccuratenessValue","MagicHitValue"}
	for i = 1, 4 do 
		if Info(PropertyNameTbl[i]) ~= nil and Info(PropertyNameTbl[i]) ~= "" then
			return PropertyNameTbl[i],Info(PropertyNameTbl[i])
		end
	end
	return 0,0
end

--得到装备等级区分函数
function GetLevelParameter(curLevel)
	if curLevel <= 40 then
	    return 1
	elseif curLevel > 40 and curLevel <= 80 then
		return 2
	elseif curLevel > 80 and curLevel <= 90 then
		return 3
	elseif curLevel > 90 and curLevel <= 120 then
		return 4
	elseif curLevel >120 and curLevel <= 150 then
		return 5
	end
	return nil
end

--用于存储装备进阶时，装备等级分割表
local EquipAdvanceOutDataLevelScaleTbl = {}
--装备等级对应的属性值表
local EquipAdvanceOutDataAttrValueTbl = {}

AddCheckLeakFilterObj(EquipAdvanceOutDataAttrValueTbl)
AddCheckLeakFilterObj(EquipAdvanceOutDataLevelScaleTbl)

--得到进阶属性函数
function GetAdvancedAttribute(AdvancedRoot,AdvancedPhase,EquipLevel)
	local AttributeTbl = {}
	if AdvancedPhase ==0 then
	   return  AttributeTbl
	end
	local advancePhaseStr = tostring(AdvancedPhase)
	local attributeNameKeys = OutDataOfAdvancedEquip_Common:GetKeys(AdvancedRoot, advancePhaseStr)
	for i, v in pairs(attributeNameKeys) do
	    local attrName = v
	    local DisplayPosition = OutDataOfAdvancedEquip_Common(AdvancedRoot, advancePhaseStr,attrName,"DisplayPosition")
	    local equipLevelScaleStr =OutDataOfAdvancedEquip_Common(AdvancedRoot, advancePhaseStr, attrName, "EquipLevelScale")
      
        if EquipAdvanceOutDataLevelScaleTbl[equipLevelScaleStr] == nil then
            local levelScaleTbl = loadstring("return {" .. equipLevelScaleStr .. "}")()
            EquipAdvanceOutDataLevelScaleTbl[equipLevelScaleStr] = levelScaleTbl
        end
	    local LevelTbl =  EquipAdvanceOutDataLevelScaleTbl[equipLevelScaleStr]
    	    
        local attrValueStr = OutDataOfAdvancedEquip_Common(AdvancedRoot, advancePhaseStr, attrName, "AttributeValue")
		if EquipAdvanceOutDataAttrValueTbl[attrValueStr] == nil then
            local attrValueTbl =  loadstring("return " .. attrValueStr)()
            EquipAdvanceOutDataAttrValueTbl[attrValueStr] = attrValueTbl
		end
        local ValueTbl = EquipAdvanceOutDataAttrValueTbl[attrValueStr] 
        
		for j = 1 ,#(LevelTbl) do
			if LevelTbl[j][1] <= EquipLevel and LevelTbl[j][2] >= EquipLevel then
				table.insert(AttributeTbl ,{attrName,ValueTbl[j],DisplayPosition})
			end
		end
	end
--	if AttributeTbl == nil then
--		LogErr("进阶属性错误",AdvancedRoot,AdvancedPhase,EquipLevel)
--	end
	return AttributeTbl
end

--得到精灵激活项与精灵类型对应表
local function GetPieceSkillToJingLingTypeTbl()
	local JingLingTypeTbl = {}
	local keys = JingLingTypeToPieceSkill_Common:GetKeys()
	for i , v in pairs (keys) do
	    local jingLingType = v
		for j = 1,5 do 
		    local pieceNameStr = "SkillPieceName" .. j
		    local skillPieceName = JingLingTypeToPieceSkill_Common(jingLingType, pieceNameStr)
			JingLingTypeTbl[skillPieceName] = jingLingType
		end
	end
	return JingLingTypeTbl
end

--精灵激活项与精灵类型对应表
g_JingLingTypeTbl = GetPieceSkillToJingLingTypeTbl()
AddCheckLeakFilterObj(g_JingLingTypeTbl)

--得到精灵属性
local function GetAttributeOfJingLing()
	local AttributeTbl = {}
	local keys = AttributeOfJingLingSkill_Common:GetKeys()
	for i ,v in pairs (keys) do
	    local SkillPieceIndex = v
	    local skillPieceName = AttributeOfJingLingSkill_Common(SkillPieceIndex, "SkillPieceName")
		if AttributeTbl[skillPieceName] == nil then
			AttributeTbl[skillPieceName] = {}
		end
		local displayContent = Lan_AttributeOfJingLingSkill_Common(i)
		
		AttributeTbl[skillPieceName]["DisplaySkillPieceName"] = displayContent("DisplaySkillPieceName")
		AttributeTbl[skillPieceName]["DescTooltips"] = displayContent("DisplayDescTooltips")
		AttributeTbl[skillPieceName]["EffectTooltips"] = displayContent("DisplayEffectTooltips")		 
		AttributeTbl[skillPieceName]["SkillName"] = displayContent("DisplaySkillName ")			
		AttributeTbl[skillPieceName]["SkillLevel"] = tonumber(AttributeOfJingLingSkill_Common(SkillPieceIndex, "SkillLevel"))			
		AttributeTbl[skillPieceName]["Index"] = SkillPieceIndex		
		AttributeTbl[skillPieceName]["PieceCount"] = AttributeOfJingLingSkill_Common(SkillPieceIndex, "NeedPieceCount")
	end
	return AttributeTbl
end

--得到精灵激活项对应的精灵技能信息
g_AttributeOfJingLingTbl = GetAttributeOfJingLing()
AddCheckLeakFilterObj(g_AttributeOfJingLingTbl)

--修改精灵激活项统计表数据
function SetJingLingPieceSkillNumber(PieceSkillNameTbl,DynInfo)
	if DynInfo ~= nil and DynInfo.ChoosedAdvanceSkillPiece <= 0 then
		return 
	end
    local pieceTitleStr = string.format("%s%d", "SkillPieceName", DynInfo.ChoosedAdvanceSkillPiece)
    local PieceSkillName = JingLingTypeToPieceSkill_Common(DynInfo.JingLingType, pieceTitleStr)

	local PieceSkillNumber = PieceSkillNameTbl[PieceSkillName]
	if PieceSkillNumber == nil then
		PieceSkillNameTbl[PieceSkillName] = 1
	else
		PieceSkillNameTbl[PieceSkillName] = PieceSkillNumber + 1
	end
end

--存储装备强化到3阶段时，对应的天赋或技能等级tbl
local EquipSuitPartTalentParamTbl = {}
AddCheckLeakFilterObj(EquipSuitPartTalentParamTbl)

--根据天赋index，得到天赋相关信息
function GetIntensifyAddTalentInfo(talentIndex)
	local suitAttributeParameterInfo = SuitPartMapTalentOrSkill_Common(talentIndex,"SuitPartAttributeParameter")
	if EquipSuitPartTalentParamTbl[suitAttributeParameterInfo] == nil then
	   	local paramContent = "{" .. suitAttributeParameterInfo .. "}"
	    local tbl = loadstring("return " .. paramContent)()
	    EquipSuitPartTalentParamTbl[suitAttributeParameterInfo] = tbl
	end
	local suitAttrParameterTbl = EquipSuitPartTalentParamTbl[suitAttributeParameterInfo]

    return suitAttrParameterTbl
end

function GetEquipPartByEquipType(equipType)
    if equipType == 5 or equipType == 7 or equipType == 8 then
        return "EquipType"
    elseif equipType == 6 or equipType == 9 then
        return "EquipPart"
    end
end

--读取装备进阶石配置表信息到内存中
local AdvanceStoneInfoTbl ={}
AddCheckLeakFilterObj(AdvanceStoneInfoTbl)

local function ReadAdvanceStoneInfoFromCfg()
		local tbl = AdvanceStone_Common:GetKeys()
    for i, v in ipairs(tbl) do
        local stoneName = v
        local adaptedEquipPartTbl = loadstring(" return " .. AdvanceStone_Common(v,"AdaptedEquipPart"))()
        local stoneAdaptedLevel = loadstring("return " .. AdvanceStone_Common(v,"AdaptedAdvanceLevelScale"))()
        AdvanceStoneInfoTbl[stoneName]  ={}
        AdvanceStoneInfoTbl[stoneName]["EquipPartTbl"] = adaptedEquipPartTbl
        AdvanceStoneInfoTbl[stoneName]["AdaptedLevel"] = stoneAdaptedLevel
    end
end
ReadAdvanceStoneInfoFromCfg()

--判断进阶石的部位是否和装备匹配
function CheckAdvancedStoneAdaptedEquip(equipType,equipName, stoneName, BaseLevel, CurAdvancePhase)
    local equipPart = GetEquipPartByEquipType(equipType)
    equipPart = g_ItemInfoMgr:GetItemInfo(equipType,equipName,equipPart)
    local adaptedEquipPartTbl = AdvanceStoneInfoTbl[stoneName]["EquipPartTbl"]
    local bAdaptedFlag = false
    for i,v in pairs(adaptedEquipPartTbl) do
        if v == equipPart then
            bAdaptedFlag = true
            break
        end       
    end
    local stoneType = g_ItemInfoMgr:GetAdvanceStoneBigID()
    local AdaptedEquipLevelScale = g_ItemInfoMgr:GetItemInfo(stoneType, stoneName,"AdaptedEquipLevelScale")
    local stoneAdaptedLevel = AdvanceStoneInfoTbl[stoneName]["AdaptedLevel"]
    if BaseLevel > AdaptedEquipLevelScale or
        CurAdvancePhase  < stoneAdaptedLevel[1] or
        CurAdvancePhase  > stoneAdaptedLevel[2] or 
        bAdaptedFlag ~= true then
        return false
    end
    return true
end

function GetAdvanceStoneAdpatedLevel(stoneName)
    if AdvanceStoneInfoTbl[stoneName] == nil then
       LogErr("进阶石信息错误", "StoneName:" .. stoneName) 
    end
    return AdvanceStoneInfoTbl[stoneName]["AdaptedLevel"]
end

--根据装备curLevel,得到等级类型（1或2）（1~30级返回1，31~150级返回2）
function GetEquipLevelType( curLevel)
	local curLevelNo = nil
	if curLevel >0 and curLevel < 41 then		
		curLevelNo  = 1
	elseif curLevel >40 and curLevel < 151 then
		curLevelNo  = 2
	end
	if curLevelNo == nil then
--		print("装备等级不在允许范围1~150级内")
		return  nil
	end
	return curLevelNo
end


local tblSuitCommon = {	["军战"] = ArmyWarSuit_Common,
						["军法"] = ArmyActSuit_Common,
						["法魔"] = MagicActSuit_Common,
						["战神"] = GodWartSuit_Common,	
						}

AddCheckLeakFilterObj(tblSuitCommon)

function GetSuitCfgBySoulType(SoulRoot)
    return tblSuitCommon[SoulRoot]
end

function GetIntenEquipPart(equipType, itemInfo)
    local equipPart = GetEquipPartByEquipType(equipType)
    if equipType == 5 then
        return string.sub(itemInfo(equipPart), 1, 4)
    else
        return itemInfo(equipPart)
    end
end

--得到套装件数
function CheckSuitCount(SoulRoot,SuitName, equipPart)
	local PartsCount = 0
	if SuitName and SuitName ~= "" then
		local tbl = EquipSuit_Common(SuitName)
		if tbl  then
			local keysTbl = tbl:GetKeys()
			local needPartsCount = keysTbl[1] 
			PartsCount = EquipSuit_Common(SuitName, needPartsCount, "IncludingPartsCount")
			return PartsCount
		else
			assert(false,SuitName .. "套装在配置表EquipSuit_Common中不存在")
		end
	end
	return PartsCount
end

--存储装备进阶成功概率表
local AdvanceSucPorbInfoTbl = {}
AddCheckLeakFilterObj(AdvanceSucPorbInfoTbl)

local function ReadAdvanceSucProbFromCfg(advancedPhase)
    local equipLevelScale = SucProbOfEquipAdvance_Server(advancedPhase, "EquipLevelScale")
    local equipLevelScaleTbl = loadstring("return " .. "{" .. equipLevelScale .. "}")() 
    local sucProbability = SucProbOfEquipAdvance_Server(advancedPhase, "SucProbability")
    local sucProbTbl = loadstring("return " .. sucProbability)()
    AdvanceSucPorbInfoTbl[advancedPhase] = {}
    table.insert(AdvanceSucPorbInfoTbl[advancedPhase], equipLevelScaleTbl)
    table.insert(AdvanceSucPorbInfoTbl[advancedPhase], sucProbTbl)
end

function GetAdvanceSucPorb(advancedPhase, equipLevel)
    local sucProbInfo = AdvanceSucPorbInfoTbl[advancedPhase+1]
    if sucProbInfo == nil then
        ReadAdvanceSucProbFromCfg(advancedPhase+1)
        sucProbInfo = AdvanceSucPorbInfoTbl[advancedPhase+1]
    end
    local equipLevelScaleTbl = sucProbInfo[1]
    local index = 1
    for i=1, # equipLevelScaleTbl do
        if equipLevelScaleTbl[i][1] <= equipLevel and equipLevel <= equipLevelScaleTbl[i][2] then
           index = i
           break
        end
    end
    local sucProbTbl = sucProbInfo[2]

    local sucProb = sucProbTbl[index] /100
    return sucProb
end


function GetEquipFxPhase(advancePhase, intensifyPhase)
    if advancePhase == nil then
       advancePhase = 0 
    end
    if intensifyPhase == nil then
       intensifyPhase = 0 
    end
    
    if advancePhase == 0 then
        return 0
    elseif advancePhase >0 and advancePhase <= 3 then
        return 1
    elseif advancePhase > 3 and advancePhase < 7 then
        return 2
    elseif advancePhase == 7 then
        if intensifyPhase > 0 then
            return 3
        else
            return 2
        end
    end
end

function GetShowIntensifyRoot(intenSoul, nBigID)
	local SoulMapInfoTbl = g_AdvancedSoul2DisplayRootTbl
    local showIntenSoul = nil
    if nBigID == 5 or nBigID == 7 or nBigID == 8 then
        if intenSoul == "战神" then
            showIntenSoul = SoulMapInfoTbl["物理攻击"]
        end
        if intenSoul == "法魔" then
            showIntenSoul = SoulMapInfoTbl["法术攻击"]
        end
    else
        if intenSoul == "战神" then
            showIntenSoul = SoulMapInfoTbl["物理防御"]
        end
        if intenSoul == "法魔" then
            showIntenSoul = SoulMapInfoTbl["法术防御"]
        end 
    end
    return showIntenSoul
end

function GetEquipRefineNeedStoneCount(curAdvancePhase, advanceQuality, intensifyQuality, equipType, equipName)
    if advanceQuality < 5 then  --  不可精炼
        return false, 188001
    end
    
    if curAdvancePhase < advanceQuality then --未进阶满
        return false, 188002 
    end
    
    if intensifyQuality >= 9 then
        return false, 188005
    end

    local needCount = EquipRefineNeedStoneCount_Common(intensifyQuality, "NeedStoneCount")
    if equipType == 5 and GetEquipHandType(g_ItemInfoMgr:GetItemInfo(equipType, equipName,"EquipType")) == "双" then   --如果是双手武器，需要消耗两个进阶石
        needCount =  needCount * 2
    end
    return true, needCount
end


function CheckEquipCanIntensify(MaxAdvancePhase, CurAdvancePhase, IntensifyQuality)
    if MaxAdvancePhase >=5 and
    	MaxAdvancePhase == CurAdvancePhase and 
    	IntensifyQuality >= 1 then
    	return true
    else
        return false 
    end
end

function GetEquipTypeInInten(itemType, itemName)
    local EquipType = g_ItemInfoMgr:GetItemInfo(itemType, itemName,"EquipType")	
    local equipPart = ""
    if g_ItemInfoMgr:IsStaticWeapon(itemType) then
        equipPart = string.sub(EquipType, 1, 4)
        
    elseif g_ItemInfoMgr:IsStaticArmor(itemType) or
        g_ItemInfoMgr:IsStaticJewelry(itemType) then
        equipPart = g_ItemInfoMgr:GetItemInfo(itemType, itemName,"EquipPart")
        
    elseif g_ItemInfoMgr:IsStaticShield(itemType) then
        equipPart = g_ItemInfoMgr:GetItemInfo(itemType, itemName,"EquipType")	
        
    elseif g_ItemInfoMgr:IsRing(itemType) then
        equipPart = "戒指"
    end
    return equipPart
end

function CheckEquipCanBeSuperadded(advanceQuality, curSuperaddPhase, superaddQulity)
    if advanceQuality < 1 then
        return  false, 1050
	end
	
	if superaddQulity == 0 then
	    return false, 1052    
	end
	
	if curSuperaddPhase >= superaddQulity then
        return false,  1051
	end
	
	return true
end

g_EquipSuperaddItemAdaptedEquipLevel = {}
local function ReadEquipSuperaddItemAdaptedEquipLevel()
    local keys = EquipSuperaddItem_Common:GetKeys()
    for i, v in pairs (keys) do
        local levelScaleStr =  EquipSuperaddItem_Common(v, "EquipLevel")
        local levelScale = loadstring("return" .. levelScaleStr)()
        g_EquipSuperaddItemAdaptedEquipLevel[v] = levelScale
    end 
end
ReadEquipSuperaddItemAdaptedEquipLevel()

--得到装备追加消耗的信息
--返回值：钱、魂值、石头个数
function GetEquipSuperaddConsumeInfo(curSuperaddPhase, baselLevel, nBigId, itemName)
    local needConsumeMoney  = EquipSuperaddConsume_Common(curSuperaddPhase, "ConsumeMoney") 
    local needConsumeSoul   = EquipSuperaddConsume_Common(curSuperaddPhase, "ConsumeSoul") 
    local needStoneNum = EquipSuperaddConsume_Common(curSuperaddPhase, "ConsumeStoneNum")
    needConsumeSoul  = math.floor((1500-(150-baselLevel)*9.176)/1500 * needConsumeSoul)
    

    if nBigId == 5 and GetEquipHandType(g_ItemInfoMgr:GetItemInfo(nBigId, itemName,"EquipType")) == "双" then   --如果是双手武器，需要消耗两个进阶石
        needConsumeMoney = needConsumeMoney * 2
        needConsumeSoul = needConsumeSoul * 2
        needStoneNum =  needStoneNum * 2 
    end
    
    return needConsumeMoney, needConsumeSoul, needStoneNum
end

function GetMaxEquipSuperaddPhase(equipBaseLevel)
    local keys = EquipSuperaddMaxPhase_Common:GetKeys()
    for i, v in pairs (keys) do
        local equipLowLevel = v
        local equipHighLevel = EquipSuperaddMaxPhase_Common(equipLowLevel, "EquipHighLevel") 
        if equipLowLevel <= equipBaseLevel and equipBaseLevel <= equipHighLevel then
            local maxSuperaddPhase =  EquipSuperaddMaxPhase_Common(equipLowLevel, "SuperaddMaxPhase")
            return maxSuperaddPhase
        end
    end
    return nil
end

g_EquipRefineItemAdaptedPhaseInfo = {}
local function ReadEquipRefineItemAdaptedPhase()
    local keys = EquipRefineItem_Common:GetKeys()
    for i, v in pairs (keys) do
        local itemName = v
        local adpatedPhaseStr = EquipRefineItem_Common(itemName, "UpMaxIntenPhaseScale")
        local adpatedPhase = loadstring("return"  .. adpatedPhaseStr )()
        g_EquipRefineItemAdaptedPhaseInfo[itemName] = adpatedPhase
    end
end
ReadEquipRefineItemAdaptedPhase()


function IsEquipSuitedToPlayerByClass(nBigID, nIndex)
    local Class = g_ItemInfoMgr:GetItemInfo(nBigID, nIndex,"Class")
    local bCanUse = false
	if Class ~= nil and  Class ~= "" then
		local tbl = loadstring( "return{" .. Class .. "}")()
		if table.maxn(tbl) ~= 9 then
			local classID = g_GameMain.m_PlayerInfoTbl.m_PlayerClassID
			for i = 1, #tbl do
				if ( tonumber(tbl[i]) == classID ) then
					bCanUse = true
					return bCanUse
				end
			end
		else
		    bCanUse = true 
		    return bCanUse
		end
	else
        bCanUse = true 
        return bCanUse
	end
	return bCanUse
end

local EquipTypeAdaptedClassTbl = {}

--通过武器类型，获取武器适用的职业
--返回值代表职业类型的字符串，如:兽人
function GetWeaponAdaptedClass(equipType)
    if EquipTypeAdaptedClassTbl[equipType] == nil then
        EquipTypeAdaptedClassTbl[equipType] = {}  
    	local Keys = StaticWeaponType_Common:GetKeys()
        for i, v in pairs ( Keys ) do
            local classIndex = v
            local RowInfo = StaticWeaponType_Common(classIndex)
            for j=1, 10 do
                local equipTypeStr = "EquipType" .. j
                if RowInfo(equipTypeStr) == equipType then
                    local class = g_GetPlayerClassNameByID(classIndex)
                    EquipTypeAdaptedClassTbl[equipType][class] = class
                end
            end
        end
    end
    return EquipTypeAdaptedClassTbl[equipType] 
end

g_EquipSuitCommonAdaptedClass = {}
function IsEquipSuitAdaptedClass(suitName, classNameTbl)
    if g_EquipSuitCommonAdaptedClass[suitName] == nil then
        local keys = EquipSuit_Common:GetKeys(suitName)
        local adaptedClassTbl = loadstring("return" .. EquipSuit_Common(suitName, keys[1], "AdaptedClass"))()
        g_EquipSuitCommonAdaptedClass[suitName] = adaptedClassTbl
    end
    if not IsTable(g_EquipSuitCommonAdaptedClass[suitName]) then
        return true
    else
        for i, v in pairs (g_EquipSuitCommonAdaptedClass[suitName]) do
            local class = v
            for j, w in pairs (classNameTbl) do
                if class == w then
                    return true 
                end
            end
        end
        return false
    end
end