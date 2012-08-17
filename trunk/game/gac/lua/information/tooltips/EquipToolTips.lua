gac_require "information/tooltips/ToolTipsInc"
cfg_load	"equip/suit/ArmyActSuit_Common"			--军法
cfg_load	"equip/suit/ArmyWarSuit_Common"			--军战
cfg_load	"equip/suit/MagicActSuit_Common"		--法魔
cfg_load	"equip/suit/GodWartSuit_Common"			--战神
cfg_load "equip/ShieldBaseValue_Common"
cfg_load "equip/intensify/EquipIntensifyName_Common"
cfg_load "item/Equip_WeaponSProperty_Common"
cfg_load "item/Equip_RingSProperty_Common"
lan_load "equip/Lan_PropertyName_Common"
cfg_load "item/ArmorPieceEnactment_Common"
lan_load "equip/intensify/Lan_SuitPartMapTalentOrSkill_Common"
lan_load "equip/suit/Lan_EquipSuit_Common"

lan_load "equip/intensify/Lan_EquipIntensifyName_Common"
lan_load "equip/suit/Lan_SuitPart_Client"
cfg_load "equip/suit/MagicActSuitNeedEquipPart_Client"
cfg_load "equip/suit/GodWarSuitNeedEquipPart_Client"
cfg_load "equip/suit/EquipSuit_Common"
cfg_load "equip/RingBaseValue_Common"
cfg_load "equip/superadd/EquipSuperaddAttrRate_Common"
cfg_load "equip/intensify/IntenPhaseMapAttrValue_Server"
cfg_load "equip/intensify/IntensifyStoneMapAddAttr_Common"

gac_gas_require "framework/common/CMoney"
gac_gas_require "item/Equip/EquipCommonFunc"

local function SuitNameToLanTbl()
    local tbl = {}
    local suitNameKeys  = EquipSuit_Common:GetKeys()
    for i, v in pairs (suitNameKeys) do
        local suitName = v
        local needPartsCountKeys =  EquipSuit_Common:GetKeys(suitName)
        local needPartsCount = needPartsCountKeys[1]
        local lanIndex = MemH64(string.format("%s%s", suitName, needPartsCount))
        local displaySuitName = Lan_EquipSuit_Common(lanIndex, "DisplayName")
        if tbl[suitName] == nil then
            tbl[suitName] = displaySuitName
        end
    end
	return tbl
end

g_SuitNameToLanTbl = SuitNameToLanTbl()

local c_money = CMoney:new()

local ColorTbl = {}
ColorTbl["稀0"] 			= g_ColorMgr:GetColor("稀0")
ColorTbl["稀1"] 			= g_ColorMgr:GetColor("稀1")
ColorTbl["稀2"] 			= g_ColorMgr:GetColor("稀2")
ColorTbl["稀3"] 			= g_ColorMgr:GetColor("稀3")
ColorTbl["稀4"] 			= g_ColorMgr:GetColor("稀4")
ColorTbl["稀5"] 			= g_ColorMgr:GetColor("稀5")
ColorTbl["稀6"] 			= g_ColorMgr:GetColor("稀6")
ColorTbl["稀7"] 			= g_ColorMgr:GetColor("稀7")
ColorTbl["橙色装备"] 		= g_ColorMgr:GetColor("橙色装备")
ColorTbl["紫色装备"] 		= g_ColorMgr:GetColor("紫色装备")
ColorTbl["蓝色装备"] 		= g_ColorMgr:GetColor("蓝色装备")
ColorTbl["绑定"] 			= g_ColorMgr:GetColor("绑定")
ColorTbl["装备绑定"]        = g_ColorMgr:GetColor("装备绑定")
ColorTbl["基础属性"]		= g_ColorMgr:GetColor("基础属性")
ColorTbl["基础等级"] 		= g_ColorMgr:GetColor("基础等级")
ColorTbl["套装缺少属性"] 	= g_ColorMgr:GetColor("套装缺少属性")
ColorTbl["说明"] 			= g_ColorMgr:GetColor("说明")
ColorTbl["价格"] 			= g_ColorMgr:GetColor("价格")
ColorTbl["护甲片属性"] 		= g_ColorMgr:GetColor("护甲片属性")
ColorTbl["不可用颜色"] 		= g_ColorMgr:GetColor("不可用颜色")
ColorTbl["进阶"] 			= g_ColorMgr:GetColor("进阶")
ColorTbl["制造者"]  	= g_ColorMgr:GetColor("制造者")
ColorTbl["剩余时间"] = g_ColorMgr:GetColor("剩余时间")
ColorTbl["装备评分"] = g_ColorMgr:GetColor("装备评分")
ColorTbl["装备追加"] = g_ColorMgr:GetColor("装备追加")

ColorTbl["火"] = g_ColorMgr:GetColor("火")
ColorTbl["地"] = g_ColorMgr:GetColor("地")
ColorTbl["风"] = g_ColorMgr:GetColor("风")
ColorTbl["水"] = g_ColorMgr:GetColor("水")
ColorTbl["冰"] = g_ColorMgr:GetColor("冰")

ColorTbl["隐藏属性"] = g_ColorMgr:GetColor("隐藏属性")
ColorTbl["耐久和部位"] = g_ColorMgr:GetColor("耐久和部位")
ColorTbl["可用颜色"] = g_ColorMgr:GetColor("可用颜色")

ColorTbl["金"] = g_ColorMgr:GetColor("金")
ColorTbl["银"] = g_ColorMgr:GetColor("银")
ColorTbl["铜"] = g_ColorMgr:GetColor("铜")


local function ModifyStr(str)
	if string.len(str) < 8 then
		local number = 8 - string.len(str) 
		for i = 1,number do
			str = str .. " "
		end
		return str
	end
	return str
end

--得到某套装的组成部件名称
local function GetSuitPartName(SuitName)
	local equipPartInfo = {}
	local SuitTbl = {}
	equipPartInfo = GodWarSuitNeedEquipPart_Client:GetKeys(SuitName)
	SuitTbl = GodWarSuitNeedEquipPart_Client
	if equipPartInfo and table.maxn(equipPartInfo) == 0 then
		equipPartInfo = MagicActSuitNeedEquipPart_Client:GetKeys(SuitName)
		SuitTbl = MagicActSuitNeedEquipPart_Client		
	end
	
	local tbl = {}
	for i, v in pairs(equipPartInfo) do
		local defaultKey = v
	    local equipPart = SuitTbl(SuitName, defaultKey, "EquipPart")
	    local addAttr1 = SuitTbl(SuitName, defaultKey, "AddAttr1")
	    local addAttr2 = SuitTbl(SuitName, defaultKey, "AddAttr2")
	    table.insert(tbl,{equipPart,addAttr1,addAttr2})
	end
	return tbl
end

function CTooltips:SetFirstTipsInfo(Tips, Color ,Str, nSmallIcon)
	Tips[1] = {}
	local str = Color .. Str .. "#r"
	table.insert(Tips[1], str)

	--str = string.format("#%d#r", nSmallIcon)
	--table.insert(Tips[1], str)
end
-- 0灰  1白   2绿   3蓝   4紫   5橙 （6黄  7红  这俩是特殊装备 不一定是指品质好）
function CTooltips:GetSuitNameColor(DynInfo,nItemType, sItemName)
	local Color = "" 
	local Quality = ""
	self.SuitColor = ColorTbl["蓝色装备"]
	if DynInfo and DynInfo.IntensifyPhase then
		if DynInfo.IntensifyPhase >= 3 then
			if DynInfo.SuitName == "" then
				Color = ColorTbl["蓝色装备"]
				Quality = 3
			else
			    local ItemInfo = g_ItemInfoMgr:GetItemInfo(nItemType, sItemName)
				local SoulRoot = DynInfo.IntensifyRoot
			    local equipPart = GetIntenEquipPart(DynInfo.m_nBigID, ItemInfo)
				local number = self:GetSuitCount(SoulRoot,DynInfo.SuitName,equipPart)
				if number == 6 or number == 2  then
					Color = ColorTbl["紫色装备"]
					Quality = 4
				elseif number == 4 or number == 3 or number == 8 then
					Color = ColorTbl["橙色装备"]
					Quality = 5
				end
			end
			self.SuitColor = Color
		elseif DynInfo.IntensifyPhase >= 1 then
			Color = ColorTbl["蓝色装备"]
			Quality = 3
		else
			Quality = g_ItemInfoMgr:GetItemInfo(nItemType, sItemName,"Quality")
			Color = ColorTbl["稀" .. Quality] and ColorTbl["稀" .. Quality] or ColorTbl["稀0"]
		end
	else
		Quality = g_ItemInfoMgr:GetItemInfo(nItemType, sItemName,"Quality")
		Color = ColorTbl["稀" .. Quality] and ColorTbl["稀" .. Quality] or ColorTbl["稀0"]
	end 
	return Color,Quality
end

function CTooltips:SetOtherTipsInfo(Tips, nNum, Str)
	Tips[nNum] = {}
	local str = Str
	table.insert(Tips[nNum], str)
end

function CTooltips:GetSex(nSex)
	local Str = ""
	if nSex ~= nil and nSex ~= "" then
		local sex = g_GameMain.m_PlayerInfoTbl.m_PlayerSex
		local tbl = loadstring( "return{" .. nSex .. "}")()
		if table.maxn(tbl) == 1 then
			if tonumber(tbl[1]) == 1 then
				Str = ToolTipsText(224)
			elseif tonumber(tbl[1]) == 2 then
				Str = ToolTipsText(225)
			end
			if tonumber(tbl[1]) ~= sex then 
				Str = ColorTbl["不可用颜色"] .. Str 
			else
				Str = ""
			end
			return Str
		end
	end	
end

local tblRace = { ["人类"] = 301, ["兽人"] = 302, ["矮人"] = 303, ["精灵"] = 304 }

function CTooltips:GetRace(Race)
	local StrTbl = {}
	if Race ~= nil and Race ~= "" then
		local tbl = loadstring( "return{" .. Race .. "}")()
		if  table.maxn(tbl) ~= 4 then
			local color = ""
			local bCanNotUse = true
			local classID = g_GameMain.m_PlayerInfoTbl.m_PlayerClassID
			local race = ClassToRace(classID)
			for i = 1, #tbl do
				if ( tbl[i] == race ) then
					bCanNotUse = false
				end
				table.insert(StrTbl,ToolTipsText( tblRace[tbl[i]] ))
			end
			if ( bCanNotUse ) then 
				color = ColorTbl["不可用颜色"]
				for i = 1,#(StrTbl) do
					StrTbl[i] = color .. StrTbl[i]
				end
			else
				StrTbl = {}
			end
		end
	end
	return StrTbl
end

function CTooltips:GetClass(Class)
	local StrTbl = {}
	if Class ~= nil and  Class ~= "" then
		local tbl = loadstring( "return{" .. Class .. "}")()
		if table.maxn(tbl) ~= 9 then
			local color = ""
			local bCanNotUse = true
			local classID = g_GameMain.m_PlayerInfoTbl.m_PlayerClassID
			for i = 1, #tbl do
				if ( tonumber(tbl[i]) == classID ) then
					bCanNotUse = false
				end
				color = ColorTbl["不可用颜色"]
				table.insert(StrTbl,color .. g_GameMain.m_DisplayCommonObj:GetPlayerShortClassForShow(tonumber(tbl[i])))
			end
			if ( not bCanNotUse ) then 
				StrTbl = {}
			end
		end
	end
	return StrTbl
end

local function GetCampName(CampID)
	local tbl = {ToolTipsText(228), ToolTipsText(229), ToolTipsText(230)} --{艾米帝国, 神圣教廷,修斯帝国}
	return tbl[CampID] and tbl[CampID] or ""
end

function CTooltips:GetCamp(Camp)
	local StrTbl = {}
	if Camp ~= nil and Camp ~= "" then
		local tbl = loadstring( "return{" .. Camp .. "}" )()
		if table.maxn(tbl) ~= 3 then
			local color = ""
			local bCanNotUse = true
			local playerCamp = g_GameMain.m_PlayerInfoTbl.m_PlayerBirthCamp
			for i = 1, #tbl do
				if ( tonumber(tbl[i]) == tonumber(playerCamp) ) then
					bCanNotUse = false
				end
				table.insert(StrTbl,GetCampName(tbl[i]))
			end
			if ( bCanNotUse ) then 
				color = ColorTbl["不可用颜色"] 
				for i = 1, #(StrTbl) do 
					StrTbl[i] = color .. StrTbl[i]
				end
			else
				StrTbl = {}
			end
			return StrTbl
		end
	end
end

function CTooltips.GetIsTwinEquip(SuitName,IntensifyAddAttr1,IntensifyAddAttr2,PartName)
    local keys = EquipSuit_Common:GetKeys(SuitName)
   	local SuitPartNameTbl = GetSuitPartName(SuitName)

	local TwinPartNumber = 0
	local TwinAttrNumber = 0
	local NameIdex = 0
	for i ,v in pairs (SuitPartNameTbl) do
		if v[1] == PartName then
			TwinPartNumber = TwinPartNumber + 1
			if v[2] == IntensifyAddAttr1 and v[3] == IntensifyAddAttr2 then
				NameIdex = TwinPartNumber
				TwinAttrNumber = TwinAttrNumber + 1
			end
		end
	end
	
	if TwinPartNumber == 2 then
		if NameIdex == 1 then
			return ToolTipsText(322)
		elseif NameIdex == 2 then
			if TwinAttrNumber == 2 then
				return ""
			elseif TwinAttrNumber == 1 then
				return ToolTipsText(323)
			end
		else
			return ""
		end
	else
		return ""
	end
end

function CTooltips:GetPlayerLevelColor(BaseLevel)
	local color = ""
	local playerLevel = g_GameMain.m_PlayerInfoTbl.m_PlayerLevel
	color = playerLevel >= BaseLevel and ColorTbl["基础等级"] or ColorTbl["不可用颜色"]
	return color
end

--护甲片属性
function CTooltips:GetEnactmentAttr(DynInfo)
	local str = ""
	if (DynInfo) then
		local tbl = {{DynInfo.ArmorPieceAttr1,DynInfo.ArmorPiece1AddTimes},{DynInfo.ArmorPieceAttr2,DynInfo.ArmorPiece2AddTimes},
		{DynInfo.ArmorPieceAttr3,DynInfo.ArmorPiece3AddTimes},{DynInfo.ArmorPieceAttr4,DynInfo.ArmorPiece4AddTimes}}
		for i = 1, #(tbl) do
			if ( tbl[i][1] and "" ~= tbl[i][1] ) then
				local DisplayName = Lan_PropertyName_Common(MemH64(ArmorPieceEnactment_Common(tbl[i][1],"EquipType")),"DisplayName")
				local PropertyValue = ArmorPieceEnactment_Common(tbl[i][1],"PropertyValue")
				self:SetPropertyNameToValueFun(ArmorPieceEnactment_Common(tbl[i][1],"EquipType"),PropertyValue)
				local ToolTipIcon =  ArmorPieceEnactment_Common(tbl[i][1],"ToolTipIcon")
				local AddTimes = ArmorPieceEnactment_Common(tbl[i][1],"AddTimes")
				str = (i == 1) and str or ( str .."#r")
				if AddTimes == 1 then
					str = str .. "#W" .. CTextureFontMgr_Inst():GetEmtStrByIdx("#" ..  ToolTipIcon) .. ColorTbl["护甲片属性"] .. DisplayName .. "    +" .. PropertyValue
				else
					str = str .. "#W" .. CTextureFontMgr_Inst():GetEmtStrByIdx("#" ..  ToolTipIcon) .. ColorTbl["护甲片属性"] .. DisplayName .. "    +" .. PropertyValue * tbl[i][2] .. "        (" .. tbl[i][2] ..  "/" .. AddTimes .. ")"
				end
			end
		end
	end
	return str
end


local function GetAddAttrValuePart(AttrName,DynInfo,IntensifyPhase)
	local ItemName = ""
	local keys = IntensifyStoneMapAddAttr_Common:GetKeys()
	for i,v in pairs (keys) do
		local TempAttrName = IntensifyStoneMapAddAttr_Common(v,"AttrName")
		if TempAttrName == AttrName then
			ItemName = v
			break
		end
	end
	
	if (IntensifyPhase >= 3 and IntensifyPhase <= 8) and (ItemName ~= "" and ItemName ~= nil) then
		local totalAttrValue = IntenPhaseMapAttrValue_Server(IntensifyPhase,"TotalAddAttrValue")
		local addAttrScaleRate = IntensifyStoneMapAddAttr_Common(ItemName,"ScaleRate")
    	local levelRate = (1500 - (150 - DynInfo.CurLevel)*9.176)/1500
    	local addAttrValue = totalAttrValue  * addAttrScaleRate * levelRate
    	return addAttrValue
    end
    return 1
end

function CTooltips:GetIntensifyAddAttr(DynInfo)
	local Str = "" 
	if ( DynInfo ) then
		local tbl = {	{DynInfo.AddAttr1, DynInfo.AddAttrValue1},
						{DynInfo.AddAttr2, DynInfo.AddAttrValue2},
						{DynInfo.AddAttr3, DynInfo.AddAttrValue3},
						{DynInfo.AddAttr4, DynInfo.AddAttrValue4},
						{DynInfo.AddAttr5, DynInfo.AddAttrValue5},
						{DynInfo.AddAttr6, DynInfo.AddAttrValue6},
						{DynInfo.AddAttr7, DynInfo.AddAttrValue7},
						{DynInfo.AddAttr8, DynInfo.AddAttrValue8},
					}
						
		local euqipType = g_ItemInfoMgr:GetItemInfo(DynInfo.m_nBigID,DynInfo.m_nIndex,"EquipType")
		local HandType = 1
		if GetEquipHandType(euqipType) == "双" then
			HandType = 2
		end
				
		for i = 1, #tbl do
			if ( tbl[i][1] ~= "" and tbl[i][2] > 0 ) then
				local SuperaddValue = 0
				local tmp = (Str == "") and "" or "#r"
				if i > 2 then
					local ValuePart = GetAddAttrValuePart(tbl[i][1],DynInfo,i)
					local Scale2 = math.ceil(ValuePart * 0.2) * HandType
					local Scale4 = math.ceil(ValuePart * 0.4) * HandType
					local Scale6 = math.ceil(ValuePart * 0.6) * HandType
					local Scale8 = math.ceil(ValuePart * 0.8) * HandType
					local Scale10 = math.ceil(ValuePart) * HandType		
					
					local IconIndex = ""
					if tbl[i][2] <= Scale2 then
						IconIndex = "1044"
					elseif tbl[i][2] > Scale2 and tbl[i][2] <= Scale4 then
						IconIndex = "1043"
					elseif tbl[i][2] > Scale4 and tbl[i][2] <= Scale6 then
						IconIndex = "1042"
					elseif tbl[i][2] > Scale6 and tbl[i][2] <= Scale8 then	
						IconIndex = "1041"
					elseif tbl[i][2] > Scale8 and tbl[i][2] <= Scale10 then
						IconIndex = "1040"
					end
					if DynInfo["SuperaddRate"] and DynInfo["SuperaddRate"] > 0 then
						SuperaddValue = math.ceil(tbl[i][2] * DynInfo["SuperaddRate"]) 
						local SuperaddValueStr = ColorTbl["装备追加"] .. ToolTipsText(112,SuperaddValue)
						Str = Str .. tmp .. "#W" .. CTextureFontMgr_Inst():GetEmtStrByIdx("#" .. IconIndex ) .. self.SuitColor .. Lan_EquipIntensifyName_Common(MemH64(tbl[i][1]),"DisplayFormat") .. tbl[i][2] .. "#R" .. SuperaddValueStr
					else						
						Str = Str .. tmp .. "#W" .. CTextureFontMgr_Inst():GetEmtStrByIdx("#" .. IconIndex ) .. self.SuitColor .. Lan_EquipIntensifyName_Common(MemH64(tbl[i][1]),"DisplayFormat") .. tbl[i][2]
					end
				else
					if DynInfo["SuperaddRate"] and DynInfo["SuperaddRate"] > 0 then
						SuperaddValue = math.ceil(tbl[i][2] * DynInfo["SuperaddRate"]) 
						local SuperaddValueStr = ColorTbl["装备追加"] .. ToolTipsText(112,SuperaddValue)
						Str = Str .. tmp .. self.SuitColor .. Lan_EquipIntensifyName_Common(MemH64(tbl[i][1]),"DisplayFormat") .. tbl[i][2] .. "#R" .. SuperaddValueStr
					else
						Str = Str .. tmp .. self.SuitColor .. Lan_EquipIntensifyName_Common(MemH64(tbl[i][1]),"DisplayFormat") .. tbl[i][2]
					end
				end
				self:SetPropertyNameToValueFun(tbl[i][1],tbl[i][2] + SuperaddValue)
--				if ( tbl[i][2] < 8 ) then
--					Str = Str .. " (破损)"
--				end
			end
		end
	end
	return Str
end


function CTooltips:GetEquipDynName(DynInfo, sDisplayName, sSuitSeparator)
	local Str = ""
	local sViewName = ("" ~= DynInfo.ViewName) and DynInfo.ViewName or sDisplayName
	if ( 1 == DynInfo.IntensifyPhase ) then
		local lanIndex = MemH64(DynInfo.AddAttr1)
		Str = Lan_EquipIntensifyName_Common(lanIndex,"Stage1") .. "*" .. sViewName
		return Str
	elseif ( 2 == DynInfo.IntensifyPhase ) then
		local lanIndex1 = MemH64(DynInfo.AddAttr1)
		local lanIndex2 = MemH64(DynInfo.AddAttr2)
		local PropertyStage1Name = Lan_EquipIntensifyName_Common(lanIndex1,"Stage1")
		Str = PropertyStage1Name .. Lan_EquipIntensifyName_Common(lanIndex2,"Stage2") .. "*" .. sViewName
		return Str
	elseif ( DynInfo.IntensifyPhase > 2 and "" ~= DynInfo.SuitName ) then
		local SuitPartName = CTooltips.GetEquipPartName(DynInfo.m_nBigID,DynInfo.m_nIndex)
		local Name = CTooltips.GetIsTwinEquip(DynInfo.SuitName,DynInfo.AddAttr1,DynInfo.AddAttr2,SuitPartName)
	    local displaySuitName  = g_SuitNameToLanTbl[DynInfo.SuitName] and g_SuitNameToLanTbl[DynInfo.SuitName] or ""
		Str =  displaySuitName .. Name .. sSuitSeparator .. sViewName
		return Str
	else
		Str =  sViewName
		return Str
	end
end

function CTooltips:GetEquipShowName(DynInfo,sDisplayName, sSuitSeparator)

	local Str = ""
	if( DynInfo ) then
		if ( 0 == DynInfo.m_nLifeTime ) then --过期的计时物品
			local _, sTimeOutName = g_ItemInfoMgr:GetItemLifeInfo(DynInfo.m_nBigID,DynInfo.m_nIndex)
			if( sTimeOutName ) then
				Str = sTimeOutName
			else
				Str = self:GetEquipDynName(DynInfo, sDisplayName, sSuitSeparator)
			end
		else
			Str = self:GetEquipDynName(DynInfo, sDisplayName, sSuitSeparator)
		end
	else
		Str = sDisplayName
	end
	return Str	
end

--得到该物品套装使用部位信息
function CTooltips.GetEquipPartName(nBigID,nIndex)
	local EquipType = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"EquipType")
	local EquipPart = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"EquipPart")
	local PartName = ""
	if g_ItemInfoMgr:IsWeapon(nBigID) or 
		g_ItemInfoMgr:IsAssociateWeapon(nBigID) or 
		g_ItemInfoMgr:IsRing(nBigID) then
		PartName= string.sub(EquipType, 1, 4)
	else
		PartName= string.sub(EquipPart, 1, 4)
	end
	return PartName
end

function CTooltips:GetEquipDynNameAndLevel(nBigID,nIndex, DynInfo, sDisplayName, sSuitSeparator)
	local Str = ""
	local sViewName = ("" ~= DynInfo.ViewName) and DynInfo.ViewName or sDisplayName
	local BaseLevelStr = self:InsertEquipBaseLevel(nBigID,nIndex,DynInfo)
	local CurSuperaddPhase = ""
	local AddAttrRate = ""
	if DynInfo["CurSuperaddPhase"] and DynInfo["CurSuperaddPhase"] > 0 then
		CurSuperaddPhase = ColorTbl["装备追加"] .. " +" .. DynInfo["CurSuperaddPhase"]
		AddAttrRate = EquipSuperaddAttrRate_Common(DynInfo["CurSuperaddPhase"], "AddAttrRate")
		AddAttrRate = ColorTbl["装备追加"] .. ToolTipsText(111,AddAttrRate)
	end
	
	if ( 1 == DynInfo.IntensifyPhase ) then
		local SuitNameColor = self:GetSuitNameColor(DynInfo,nBigID,nIndex)		
		local lanIndex = MemH64(DynInfo.AddAttr1)
		if AddAttrRate and AddAttrRate ~= "" then
			Str = SuitNameColor .. Lan_EquipIntensifyName_Common(lanIndex,"Stage1") .. "*" .. sViewName .. CurSuperaddPhase .. "#R" .. BaseLevelStr .. "#r#R" .. AddAttrRate
		else 
			Str = SuitNameColor .. Lan_EquipIntensifyName_Common(lanIndex,"Stage1") .. "*" .. sViewName .. CurSuperaddPhase .. "#R" .. BaseLevelStr 
		end
		return Str
	elseif ( 2 == DynInfo.IntensifyPhase ) then
		local SuitNameColor = self:GetSuitNameColor(DynInfo,nBigID,nIndex)
		local lanIndex1 = MemH64(DynInfo.AddAttr1)
		local lanIndex2 = MemH64(DynInfo.AddAttr2)
		local PropertyStage1Name = Lan_EquipIntensifyName_Common(lanIndex1,"Stage1")
		if AddAttrRate and AddAttrRate ~= "" then
			Str = SuitNameColor .. PropertyStage1Name .. Lan_EquipIntensifyName_Common(lanIndex2,"Stage2") .. "*" .. sViewName  .. CurSuperaddPhase .. "#R" .. BaseLevelStr .. "#r#R" .. AddAttrRate
		else
			Str = SuitNameColor .. PropertyStage1Name .. Lan_EquipIntensifyName_Common(lanIndex2,"Stage2") .. "*" .. sViewName  .. CurSuperaddPhase .. "#R" .. BaseLevelStr
		end
		return Str
	elseif ( DynInfo.IntensifyPhase and DynInfo.IntensifyPhase > 2 and "" ~= DynInfo.SuitName ) then
		local SuitNameColor = self:GetSuitNameColor(DynInfo,nBigID,nIndex)
		local displaySuitName  = g_SuitNameToLanTbl[DynInfo.SuitName] and g_SuitNameToLanTbl[DynInfo.SuitName] or ""
		
		local SuitPartName = CTooltips.GetEquipPartName(nBigID,nIndex)
		local Name = CTooltips.GetIsTwinEquip(DynInfo.SuitName,DynInfo.AddAttr1,DynInfo.AddAttr2,SuitPartName)
		Str = SuitNameColor .. displaySuitName .. Name .. "#R" .. BaseLevelStr .. sSuitSeparator
		return Str .. SuitNameColor .. sViewName .. CurSuperaddPhase .. "#R" .. AddAttrRate
	else
		local SuitNameColor = self:GetSuitNameColor(DynInfo,nBigID,nIndex)
		if AddAttrRate and AddAttrRate ~= "" then
			Str = SuitNameColor .. sViewName .. CurSuperaddPhase .. "#R" .. BaseLevelStr .. "#r#R" .. AddAttrRate
		else
			Str = SuitNameColor .. sViewName .. "#R" .. BaseLevelStr
		end
		return Str
	end
end

function CTooltips:GetEquipCurrentNameAndLevel(DynInfo,nBigID,nIndex, sDisplayName, sSuitSeparator)
	local Str = ""
	if( DynInfo ) then
		if ( 0 == DynInfo.m_nLifeTime ) then --过期的计时物品
			local _, sTimeOutName = g_ItemInfoMgr:GetItemLifeInfo(DynInfo.m_nBigID,DynInfo.m_nIndex)
			if( sTimeOutName ) then
				Str = sTimeOutName
			else
				Str = self:GetEquipDynNameAndLevel(nBigID,nIndex,DynInfo, sDisplayName, sSuitSeparator)
			end
		else
			Str = self:GetEquipDynNameAndLevel(nBigID,nIndex,DynInfo, sDisplayName, sSuitSeparator)
		end
	else
		local BaseLevelStr = self:InsertEquipBaseLevel(nBigID,nIndex,DynInfo)
		local SuitNameColor = self:GetSuitNameColor(DynInfo,nBigID,nIndex)
		Str = SuitNameColor .. sDisplayName .. "#R" .. BaseLevelStr
	end
	return Str
end

function CTooltips:InsertEquipCurrentName(nBigID,nIndex, DynInfo, sDisplayName)
	local str = self:GetEquipCurrentNameAndLevel(DynInfo,nBigID,nIndex, sDisplayName, "#r")
	--str = str .." +" ..  (DynInfo.CurSuperaddPhase or "") .. "#r"
	return str
end

--进阶阶段
function CTooltips:InsertEquipAdvancedPhase(DynInfo,item_type,item_name)
	local str = ""
	if ( DynInfo ) then
		local sSoulRootDisplayName = g_AdvancedSoul2DisplayRootTbl[DynInfo.AdvancedRoot]
		sSoulRootDisplayName = sSoulRootDisplayName and sSoulRootDisplayName or ""
		str = ColorTbl["进阶"] .. ToolTipsText(272, sSoulRootDisplayName, DynInfo.CurAdvancePhase, DynInfo.MaxAdvancePhase) .. "#R" .. ToolTipsText(332, DynInfo.AdvancedTimes >= 150 and 150 or DynInfo.AdvancedTimes )
	elseif DynInfo == nil then
		local AdvancedCurLevel = g_ItemInfoMgr:GetItemInfo(item_type,item_name,"AdvancedCurLevel")
		local AdvancedQuality = g_ItemInfoMgr:GetItemInfo(item_type,item_name,"AdvancedQuality")
		local sSoulRootDisplayName = g_ItemInfoMgr:GetItemLanInfo(item_type,item_name,"DisplayAdvancedRoot")
		str = ColorTbl["进阶"] .. ToolTipsText(272, sSoulRootDisplayName, AdvancedCurLevel,AdvancedQuality)
	end	
	return str
	
end

function CTooltips:InsertEquipBindingType(nItemID, item_type,item_name, DynInfo,BindingStyle)
	local str = ""
	local BindingStyle = BindingStyle or g_ItemInfoMgr:GetItemInfo(item_type,item_name,"BindingStyle")
	if ( DynInfo and 2 == DynInfo.BindingType ) then
		str = ColorTbl["绑定"] .. ToolTipsText(207)
	elseif ( (DynInfo and 1 == DynInfo.BindingType) or (0 == nItemID and 1 == BindingStyle) )then
		str = ColorTbl["装备绑定"] .. ToolTipsText(243)
	elseif ( DynInfo == nil and 2 ==  BindingStyle ) then
		str = ColorTbl["装备绑定"] .. ToolTipsText(200) 
	elseif DynInfo and 3 ==  DynInfo.BindingType then
		str = ColorTbl["装备绑定"] .. ToolTipsText(296) 
	end
	if str ~= "" then
		return str
	end
end

function CTooltips:InsertEquipNumLimit(nItemInfoOnly)
	if ( 1 == nItemInfoOnly ) then
		local str = ColorTbl["可用颜色"] .. ToolTipsText(208)
		return str
	end
end

function CTooltips:InsertEquipBaseLevel(nBigID,nIndex,DynInfo)
	local Level = DynInfo and DynInfo.BaseLevel or g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"BaseLevel" )
	local str = self:GetPlayerLevelColor(Level) .. ToolTipsText(244, Level)
	return str
end

function CTooltips:InsertEquipEnactmentAttr(DynInfo)
	local str = "" 
	str = self:GetEnactmentAttr(DynInfo)
	return str
end

function CTooltips:InsertEquipSex(nSex)
	local str = self:GetSex(nSex)
	if ( str and "" ~= str ) then
		return str
	end
end

function CTooltips:InsertEquipRace(Race)
	local strTbl = self:GetRace(Race)
	if strTbl and #(strTbl) > 0 then
		return strTbl
	end
end

function CTooltips:InsertEquipClass(Class)
	local StrTbl = self:GetClass(Class)
	return StrTbl
end

function CTooltips:InsertEquipCamp(Camp)
	local strTbl = self:GetCamp(Camp)
	if ( strTbl and #(strTbl) > 0 ) then
		return strTbl
	end
end

function CTooltips:InsertEquipAdvance(DynInfo)
	local Str = ""
	if DynInfo ~= nil and DynInfo.CurAdvancePhase > 0 then
		Str = ColorTbl["进阶"] .. self:GetEquipAdvanceAttribute(DynInfo)
	end
	return Str
end

function CTooltips:InsertEquipJingLingSkill(DynInfo)
    local Str = ""
	if DynInfo ~= nil and DynInfo.AdvanceSkillPiece1 > 0 then 
		Str = self:GetJingLingPieceSkillName(DynInfo)
	end
	return Str
end

function CTooltips:InsertEquipIntensify(DynInfo,item_type,item_name)
	local str = ""
	if ( DynInfo ) then
		local SoulRootDisplayName = GetShowIntensifyRoot(DynInfo.IntensifyRoot,item_type)
		SoulRootDisplayName = SoulRootDisplayName and SoulRootDisplayName or ""
		str = ToolTipsText(246,SoulRootDisplayName, DynInfo.IntensifyPhase, DynInfo.IntensifyQuality)
		if tonumber(DynInfo.IntensifyTimes) > 0 then
			str = str .. "#R" .. ToolTipsText(283,DynInfo.IntensifyTimes)
		end 
	elseif DynInfo == nil then
		local SoulRootDisplayName = g_ItemInfoMgr:GetItemLanInfo(item_type,item_name,"SoulRootDisplayName")
		local IntensifyQuality = g_ItemInfoMgr:GetItemInfo(item_type,item_name,"IntensifyQuality")
		str = ToolTipsText(246,SoulRootDisplayName, 0,IntensifyQuality)
	end
	return str
end

function CTooltips:InsertEquipIntensifyAddAttr(DynInfo)
	local str = ""
	str = self:GetIntensifyAddAttr(DynInfo)
	return str
end

function CTooltips:GetEquipSuitShowDesc(str,ItemInfo, DynInfo, choosedSuitColor, SuitName)
    local suitName = DynInfo.SuitName 
    if SuitName ~= nil then
         suitName = SuitName
    end
    
	local soulRoot = DynInfo.IntensifyRoot
	local equipPart = GetIntenEquipPart(DynInfo.m_nBigID, ItemInfo)
	local AllNumber = self:GetSuitCount(soulRoot,suitName, equipPart)
	local CurNumber = self:GetEquipSuitCount(soulRoot,suitName)
	local displaySuitName = g_SuitNameToLanTbl[suitName]
	if choosedSuitColor ~= nil then
	    self.SuitColor = choosedSuitColor 
    end
    
    str = str .. self.SuitColor .. displaySuitName .. "(" .. CurNumber  .. "/" .. AllNumber .. ")#n"
	local SuitPartName = self:ModifyEquipSuitPartName(soulRoot,suitName,displaySuitName)
	local Desc = self:GetSuitDesc(soulRoot,suitName,DynInfo.BaseLevel, DynInfo, equipPart)
	str = str .. SuitPartName .. Desc
	return str
end

function CTooltips:InsertEquipSuitShow(ItemInfo, DynInfo)
	local str = ""
	if DynInfo ~= nil and DynInfo.IntensifyPhase >= 3  then
	    if DynInfo.SuitName ~= nil and DynInfo.SuitName ~= "" then
	        str = g_Tooltips:GetEquipSuitShowDesc(str,ItemInfo, DynInfo)
	    end
		if DynInfo.IntensifyAddTalent ~= "" then --强化到3阶段产生的天赋
			local intenAddTalentDesc = Lan_SuitPartMapTalentOrSkill_Common(DynInfo.IntensifyAddTalent,"DisplaySuitPartAttributeDescription")
			local equipLevelType = GetLevelParameter(DynInfo.BaseLevel)
			local talentParam = GetIntensifyAddTalentInfo(DynInfo.IntensifyAddTalent)[equipLevelType][2]
			str = (str == "") and str or (str .. "#r")
			str = str .. ColorTbl["隐藏属性"] .. ToolTipsText(247) .. GetStrAccordWildcard(intenAddTalentDesc, talentParam) .. "#r"
		end
	end
	return str 
end

function CTooltips:InsertEquipDescription(sDescription)
	local Str = ""
	if ( "" ~= sDescription) then
		Str = ColorTbl["说明"] .. sDescription
	end
	return Str
end

function CTooltips:InsertEquipPayTypePrice(item_type,item_name, bOriginalPrice,GoldType)
	local str = g_Tooltips:GetItemPriceByPayType(item_type,item_name, bOriginalPrice,GoldType)
	if bOriginalPrice == true then
	    str = ColorTbl["价格"] .. ToolTipsText(280, str)
	else
	    str = ColorTbl["价格"] .. ToolTipsText(205, str)
	end
    return str
end

------各种装备独特的属性显示--------------------------------------------------------
function CTooltips:InsertWeaponAttack(nIndex, nItemID, nBigID, DynInfo)
	
	local AttackSpeed = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"AttackSpeed")
	local MaxAttack, MinAttack = 0,0
	if DynInfo ~= nil then 
		MaxAttack, MinAttack = GetWeaponDPSFloat(DynInfo.AttackSpeed, DynInfo.DPS,DynInfo.DPSFloorRate)
	elseif nItemID == 0 then
		local IsStatic = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"IsStatic") 
		if IsStatic == 1 then
			local WeaponStaticPropertyInfo = Equip_WeaponSProperty_Common(nIndex)
			MaxAttack, MinAttack = GetWeaponDPSFloat(AttackSpeed, WeaponStaticPropertyInfo("DPS"),WeaponStaticPropertyInfo("DPSFloorRate"))
		else
			local AttrLevel = g_ItemInfoMgr:GetItemInfo(nBigID, nIndex,"AttrLevel")
			local EquipType = g_ItemInfoMgr:GetItemInfo(nBigID, nIndex,"EquipType")
			local DPS = GetBaseWeaponDPS(AttrLevel,EquipType)
			local DPSFloorRate = WeaponDPSFloor_Common(EquipType,"DPSFloorRate")
			MaxAttack, MinAttack = GetWeaponDPSFloat(AttackSpeed,DPS,DPSFloorRate)
		end
	end
	local nAttackSpeed = DynInfo and DynInfo.AttackSpeed or g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"AttackSpeed")  
	local str = ColorTbl["基础属性"] .. ToolTipsText(258, MinAttack, MaxAttack, nAttackSpeed)
	return str
end

function CTooltips:InsertWeaponDPS(nIndex, nItemID, nBigID, DynInfo)
	local fDPS = 0
	if ( DynInfo ) then
		fDPS = DynInfo.DPS 
	else
		local IsStatic = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"IsStatic") 
		if IsStatic == 1 then
			fDPS = Equip_WeaponSProperty_Common(nIndex,"DPS")
		else
			local AttrLevel = g_ItemInfoMgr:GetItemInfo(nBigID, nIndex,"AttrLevel")
			local EquipType = g_ItemInfoMgr:GetItemInfo(nBigID, nIndex,"EquipType")
			fDPS = GetBaseWeaponDPS(AttrLevel,EquipType)
		end
	end
	local str = ColorTbl["基础属性"] .. ToolTipsText(259, fDPS)
	return str
end

function CTooltips:InsertOtherEquipProperty(DynInfo, nBigID, nIndex)
	local Str = ""
	local IsStatic	= g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"IsStatic")
	local lanTbl = Lan_EquipIntensifyName_Common
	if (IsStatic == 0 or IsStatic == "") then
		local OutputAttr1,OutputAttr2,OutputAttr3 = 0,0,0
		if DynInfo ~= nil then
			OutputAttr1 = DynInfo.OutputAttr1
			OutputAttr2 = DynInfo.OutputAttr2
			OutputAttr3 = DynInfo.OutputAttr3
		else
			local AttrLevel	= g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"AttrLevel")
			OutputAttr1,OutputAttr2,OutputAttr3 = GetEquipOutputAttr(nBigID,nIndex,AttrLevel)
		end
		
		local outputAttr = GetEquipOutputAttrName(nBigID, nIndex)
		self:SetPropertyNameToValueFun(outputAttr[1],OutputAttr1)
		self:SetPropertyNameToValueFun(outputAttr[2],OutputAttr2)
		self:SetPropertyNameToValueFun(outputAttr[3],OutputAttr3)
		Str = Str .. lanTbl(MemH64(outputAttr[1]),"DisplayFormat") .. OutputAttr1 .. "#r"
		Str = Str .. lanTbl(MemH64(outputAttr[2]),"DisplayFormat") .. OutputAttr2 .. "#r"
		Str = Str .. lanTbl(MemH64(outputAttr[3]),"DisplayFormat") .. OutputAttr3
		Str = ColorTbl["基础属性"] .. Str
		if Str ~= "" then
			return Str
		end
	elseif IsStatic == 1 then
		local NameAndValueTbl = GetStaticPropertyName(nBigID, nIndex)
		local OutputAttr1 = DynInfo and DynInfo.OutputAttr1 or NameAndValueTbl[1][2]
		local OutputAttr2 = DynInfo and DynInfo.OutputAttr2 or NameAndValueTbl[2][2]
		local OutputAttr3 = DynInfo and DynInfo.OutputAttr3 or NameAndValueTbl[3][2]
		
		self:SetPropertyNameToValueFun(GetAttrName(NameAndValueTbl[1][1]),OutputAttr1)
		self:SetPropertyNameToValueFun(GetAttrName(NameAndValueTbl[2][1]),OutputAttr2)
		self:SetPropertyNameToValueFun(GetAttrName(NameAndValueTbl[3][1]),OutputAttr3)
		Str = Str .. lanTbl(MemH64(GetAttrName(NameAndValueTbl[1][1])),"DisplayFormat") .. OutputAttr1 .. "#r"
		Str = Str .. lanTbl(MemH64(GetAttrName(NameAndValueTbl[2][1])),"DisplayFormat") .. OutputAttr2 .. "#r"
		Str = Str .. lanTbl(MemH64(GetAttrName(NameAndValueTbl[3][1])),"DisplayFormat") .. OutputAttr3
		Str = ColorTbl["基础属性"] .. Str
		if (DynInfo ~= nil and DynInfo.StaticProValue ~= 0) or #(NameAndValueTbl) == 4  then
			if NameAndValueTbl[4][1] == nil or NameAndValueTbl[4][2] == nil then
				assert(false,"出错装备的名称:" .. nIndex)
			end
			local OutputAttr4 = DynInfo and DynInfo.StaticProValue or NameAndValueTbl[4][2]
			self:SetPropertyNameToValueFun(GetAttrName(NameAndValueTbl[4][1]),OutputAttr4)
			Str = Str .. "#r" .. lanTbl(MemH64(GetAttrName(NameAndValueTbl[4][1])),"DisplayFormat") .. OutputAttr4
		end
		if Str ~= "" then
			return Str
		end
	end
	return Str
end

function CTooltips:InsertRingProperty(nItemID, nBigID, DynInfo, nIndex)
	local Dps = 0
	if DynInfo ~= nil or ( nItemID == 0 and g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"IsStatic") == 1 ) then 
		Dps = DynInfo and DynInfo.DPS or string.format("%.1f",Equip_RingSProperty_Common(nIndex,"DPS"))
	else
		local AttrLevel = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"AttrLevel")
		Dps = RingBaseValue_Common(AttrLevel,"AttackCount")	
	end

	local Str = ""
	local DamageType = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"DamageType")
	if DamageType == 1 then 
		local MagicDamageValue =  Dps
		Str = ColorTbl["基础属性"] .. ToolTipsText(268) .. MagicDamageValue
		self:SetPropertyNameToValueFun(GetAttrName("MagicDamageValue"),MagicDamageValue)
	elseif DamageType == 2 then
		local PhysicalDPS = Dps
		Str = ColorTbl["基础属性"] .. ToolTipsText(269) .. PhysicalDPS
		self:SetPropertyNameToValueFun(GetAttrName("PhysicalDPS"),PhysicalDPS)
	end
	
	--显示戒指静态属性
	local PropertyName,PropertyValue = GetRingStaticPropertyName(Equip_RingSProperty_Common(nIndex))
	if (DynInfo ~= nil and tonumber(DynInfo.StaticProValue) ~= 0) or (nItemID == 0 and PropertyName ~= 0 ) then
		local lanTbl = Lan_EquipIntensifyName_Common
		local ProValue = DynInfo and DynInfo.StaticProValue or PropertyValue
		self:SetPropertyNameToValueFun(GetAttrName(PropertyName),ProValue)
		local AttrName = lanTbl(MemH64(GetAttrName(PropertyName)),"Display")
		Str = Str .. "#r  " .. AttrName .. ":" .. ProValue
	end
	
	return Str
end

function CTooltips:InsertShieldProperty(nBigID,nIndex, DynInfo)
	local Str = ""
	local IsShield	= g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"IsShield")
	local AttrLevel	= g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"AttrLevel")
	if IsShield == 1 then 
		local blockRate ,blockValue = GetShieldBlock((DynInfo and DynInfo.CurLevel or AttrLevel),nBigID, nIndex)
		Str = ColorTbl["基础属性"] .. ToolTipsText(265, blockRate*100, blockValue) --格挡率:  %.3f#r  格挡值: %.1f#r
		
		local IsStatic	= g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"IsStatic")
		local DefenceValue,NatureResistanceValue,DestructionResistanceValue,EvilResistanceValue = 0,0,0,0
		AttrLevel = DynInfo and DynInfo.CurLevel or AttrLevel
		
		if IsStatic ~= 1  then
			DefenceValue = ShieldBaseValue_Common(AttrLevel,"Defence")
			NatureResistanceValue = ShieldBaseValue_Common(AttrLevel,"NatureResistanceValue")
			DestructionResistanceValue = ShieldBaseValue_Common(AttrLevel,"DestructionResistanceValue")
			EvilResistanceValue = ShieldBaseValue_Common(AttrLevel,"EvilResistanceValue")
		else
			DefenceValue = DynInfo and DynInfo.OutputAttrValue1 or ShieldBaseValue_Common(AttrLevel,"Defence")
			NatureResistanceValue = DynInfo and DynInfo.OutputAttrValue2 or ShieldBaseValue_Common(AttrLevel,"NatureResistanceValue")
			DestructionResistanceValue = DynInfo and DynInfo.OutputAttrValue3 or ShieldBaseValue_Common(AttrLevel,"DestructionResistanceValue")
			EvilResistanceValue = DynInfo and DynInfo.OutputAttrValue4 or ShieldBaseValue_Common(AttrLevel,"EvilResistanceValue")
		end
		
		Str = Str .. ToolTipsText(261) .. DefenceValue .. "#r"				--护甲值
		Str = Str .. ToolTipsText(262) .. NatureResistanceValue .. "#r"		--自然抗值
		Str = Str .. ToolTipsText(263) ..DestructionResistanceValue .. "#r"	--破坏抗值
		Str = Str .. ToolTipsText(264) .. EvilResistanceValue				--暗黑抗值	
		
		self:SetPropertyNameToValueFun(GetAttrName("Defence"),DefenceValue)
		self:SetPropertyNameToValueFun(GetAttrName("NatureResistanceValue"),NatureResistanceValue)
		self:SetPropertyNameToValueFun(GetAttrName("DestructionResistanceValue"),DestructionResistanceValue)
		self:SetPropertyNameToValueFun(GetAttrName("EvilResistanceValue"),EvilResistanceValue)
	elseif IsShield == 2 then 
		local DamageValue = GetShieldMagicDamage((DynInfo and DynInfo.CurLevel or AttrLevel),nBigID,nIndex)
		self:SetPropertyNameToValueFun(GetAttrName("MagicDamageValue"),DamageValue)
		Str = ColorTbl["基础属性"] .. ToolTipsText(266, DamageValue) --法伤: %d
	end
	if Str ~= "" then
		return Str
	end	
end

function CTooltips:InsertEquipDuraInfo(DynInfo,item_type,item_name)
		local EquipPart = g_ItemInfoMgr:GetItemInfo(item_type,item_name,"EquipPart")
    if EquipPart ~= "背部" then
        if DynInfo ~= nil then
            local curDuraValue = tonumber(DynInfo.CurDuraValue)
            local maxDuraValue = tonumber(DynInfo.MaxDuraValue)
            local duraInfo = ToolTipsText(306)
            if curDuraValue == 0  then 
                duraInfo = ColorTbl["不可用颜色"]  .. duraInfo
            else
                duraInfo =  ColorTbl["耐久和部位"]  .. duraInfo
            end
            duraInfo = duraInfo .. curDuraValue .. "/" .. maxDuraValue
            return  duraInfo            
        end
    end
end

function CTooltips:InsertEquipFixMoneyStr(DynInfo)
	local str = ""
	if DynInfo ~= nil then
		local curDuraValue = tonumber(DynInfo.CurDuraValue)
		local maxDuraValue = tonumber(DynInfo.MaxDuraValue)		
    	if curDuraValue < maxDuraValue then
        
        	local needMoney = MoneyConsumedFixingEquip_Common(DynInfo.BaseLevel,"MoneyConsumed")
       		local c_money = CMoney:new()
        	local moneyNeedToFixStr = c_money:ChangeMoneyToString(needMoney)
       		str = ColorTbl["不可用颜色"] .. ToolTipsText(305) .. "#R" .. moneyNeedToFixStr
       	end
	end
	return str
end

--该函数没有使用。以后加上
function CTooltips:InsertItemMaker(DynInfo)
    if DynInfo then
    	local maker_name = DynInfo.ItemMaker
    	if maker_name then
    	return ColorTbl["制造者"] .. ToolTipsText(276, maker_name)
      end
    end
    return ""
end

function CTooltips:InsertEquipLeftTime(DynInfo)
	local Str = ""
	if DynInfo then
		nLeftTime = DynInfo.m_nLifeTime
		if nLeftTime and nLeftTime > 0 then
			Str = ColorTbl["剩余时间"] .. ToolTipsText(203, g_CTimeTransMgr:FormatTime(nLeftTime) )
		end
	end
	return Str
end
--------------------------------------------------------


function CTooltips:GetSuitCount(soulRoot,suitName, equipPart)
	return CheckSuitCount(soulRoot, suitName, equipPart)
end

local function GetSuitDescCommom(BaseLevel, tbl)
	local levelParameter= GetLevelParameter(BaseLevel)
	local suitAttrParameterTbl = "{" .. tbl.SuitAttributeParameter .. "}"
	suitAttrParameterTbl = loadstring("return " .. suitAttrParameterTbl)()
	if suitAttrParameterTbl[levelParameter] == nil then
		assert(false, "出错的套装名称:" .. SuitName)
	end
	local index = tbl.Index
	local descriptionString  = GetStrAccordWildcard(Lan_EquipSuit_Common[index].DisplaySuitAttributeDescription, suitAttrParameterTbl[levelParameter][2])
	return descriptionString
end

local SuitAttrParamTbl = {}

--得到描述套装属性的信息
local function GetSuitDesc(SuitName,AllNumber,PartNumber,BaseLevel)
    local levelParameter= GetLevelParameter(BaseLevel)
    local attrParam = EquipSuit_Common(SuitName, tostring(PartNumber), "SuitAttributeParameter")
    local suitAttrParameterTbl = "{" .. attrParam .. "}"
    if SuitAttrParamTbl[suitAttrParameterTbl] == nil then
        SuitAttrParamTbl[suitAttrParameterTbl] = loadstring("return " .. suitAttrParameterTbl)()
    end
    suitAttrParameterTbl = SuitAttrParamTbl[suitAttrParameterTbl]
    
	if suitAttrParameterTbl[levelParameter] == nil then
		assert(false, "出错的套装名称:" .. SuitName)
	end
	local lanIndex = MemH64(string.format("%s%s", SuitName, PartNumber))
    local descriptionString  = GetStrAccordWildcard(Lan_EquipSuit_Common(lanIndex, "DisplaySuitAttributeDescription"), suitAttrParameterTbl[levelParameter][2])
	return descriptionString
end

--得到4件套套装的职业属性描述
local function GetFourSuitDesc(SuitName,BaseLevel)
    local needPartsCountKeys  = EquipSuit_Common:GetKeys(SuitName)
    for i, v in pairs (needPartsCountKeys) do
        local needPartsCount = v 
        if  tonumber( needPartsCount) == nil  then --要求的是职业
            local IncludingPartsCount =  EquipSuit_Common(SuitName, needPartsCount, "IncludingPartsCount") 
            if IncludingPartsCount == 4 then
                local descriptionString = GetSuitDesc(SuitName,IncludingPartsCount,needPartsCount,BaseLevel)
                return descriptionString, needPartsCount
            end
        end
    end
	return "该四件套装没有填写职业名称"
end

function CTooltips:ConnectSuitDesc(EquipSuitCount, SuitName, AllNumber, PartNumber, BaseLevel)
	local str = GetSuitDesc(SuitName, AllNumber, PartNumber, BaseLevel)
	local colorStr = EquipSuitCount >= PartNumber and self.SuitColor or ColorTbl["套装缺少属性"]
	return colorStr .. ToolTipsText(279, PartNumber) .. str
end

function CTooltips:GetSuitDesc(SoulRoot,SuitName,BaseLevel, DynInfo, equipPart)
    local TotalPartNumber = self:GetSuitCount(SoulRoot,DynInfo.SuitName, equipPart) 
	local EquipSuitCount = self:GetEquipSuitCount(SoulRoot,SuitName)
    local needPartsKeys = EquipSuit_Common:GetKeys(SuitName)
    local suitDescStr = ""
    local classSuitDesc = ""
    for i, v in pairs (needPartsKeys) do
        local needPart = v
        local needPartCount = tonumber(needPart)
        if needPartCount ~= nil then --非职业套装
            suitDescStr = suitDescStr .. self:ConnectSuitDesc(EquipSuitCount, SuitName, TotalPartNumber, needPartCount, BaseLevel)
        else
            local ClassName = g_GetPlayerClassNameByID(g_GameMain.m_PlayerInfoTbl.m_PlayerClassID)
    		local strTemp,SuitClass = GetFourSuitDesc(SuitName,BaseLevel)
    		local DisplayName = Lan_Class_Common(MemH64(SuitClass),"DisplayName")
    		strTemp = ToolTipsText(267, DisplayName, strTemp)
    		local Color = ClassName == SuitClass and EquipSuitCount == 4 and self.SuitColor or ColorTbl["套装缺少属性"]
    		classSuitDesc = Color .. strTemp
        end
    end
    suitDescStr = suitDescStr .. classSuitDesc
    return suitDescStr
end

--得到角色身上某个套装的件数
function CTooltips:GetEquipSuitCount(SoulRoot,SuitName)
	local SuitPartNameTbl = GetSuitPartName(SuitName)
	local EquipTbl
	if self.m_IsAimStatus  then
		EquipTbl = g_GameMain.m_AimStatusWnd.Part
	else
		EquipTbl =  g_GameMain.m_RoleStatus.Part
	end
	local EquipSuitCount = 0 
	local tbl = {}
	for i = 1 ,13 do 
		if EquipTbl[i].m_Info ~= nil then
			local nItemID = EquipTbl[i].m_Info:GetItemID()
			local nBigID = EquipTbl[i].m_Info:GetBigID()
			local nIndex = EquipTbl[i].m_Info:GetIndex()
			local EquipType = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"EquipType")
			local EquipPart = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"EquipPart")
			local PartName = ""
			if g_ItemInfoMgr:IsWeapon(nBigID) or 
				g_ItemInfoMgr:IsAssociateWeapon(nBigID) or 
				g_ItemInfoMgr:IsRing(nBigID) then
				PartName= string.sub(EquipType, 1, 4)
			else
				PartName= string.sub(EquipPart, 1, 4)
			end
			local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(nItemID)
			if SuitName == DynInfo.SuitName then 
				for j = 1,#(SuitPartNameTbl) do 
					if SuitPartNameTbl[j][2] == DynInfo.AddAttr1
						and SuitPartNameTbl[j][3] == DynInfo.AddAttr2
						and SuitPartNameTbl[j][1] == PartName then
						EquipSuitCount = EquipSuitCount + 1
						table.insert(tbl,EquipTbl[i].m_Info)
						table.remove(SuitPartNameTbl,j)
						break
					end
				end
			end
		end
	end
	return EquipSuitCount,tbl
end


local function CheckIsSuitPart(SuitPartName,EquipSuitInfoTbl)
	for i = 1, #(EquipSuitInfoTbl) do
		local nBigID,nIndex = EquipSuitInfoTbl[i]:GetBigID(),EquipSuitInfoTbl[i]:GetIndex()
		local nItemID = EquipSuitInfoTbl[i]:GetItemID()
		local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(nItemID)
		local EquipType = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"EquipType")
		local EquipPart = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"EquipPart")  
		if  g_ItemInfoMgr:IsWeapon(nBigID) then
			EquipPart = string.sub(EquipType,1,4)
		elseif g_ItemInfoMgr:IsRing(nBigID) then
			EquipPart = EquipType
		elseif g_ItemInfoMgr:IsStaticShield(nBigID) then
			 EquipPart = EquipType
		else
			EquipPart = EquipPart
		end
		
		if EquipPart == SuitPartName[1] and DynInfo.AddAttr1 == SuitPartName[2]
			and DynInfo.AddAttr2 == SuitPartName[3]  then
			return true,i
		end
	end
	return false
end

--得到装备对应的强化属性
local function GetEquipAddAttrName(EquipSuitInfoTbl)
	local tbl = {}
	for i = 1, #(EquipSuitInfoTbl) do
		local nItemID = EquipSuitInfoTbl[i]:GetItemID()
		local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(nItemID)
		local nBigID,nIndex = EquipSuitInfoTbl[i]:GetBigID(),EquipSuitInfoTbl[i]:GetIndex()
		local Info = g_ItemInfoMgr:GetItemFunInfo(nBigID,nIndex)
		local EquipPart  
		if  g_ItemInfoMgr:IsWeapon(nBigID) then
			EquipPart = string.sub(Info("EquipType"),1,4)
		elseif g_ItemInfoMgr:IsRing(nBigID) then
			EquipPart = Info("EquipType")
		elseif g_ItemInfoMgr:IsStaticShield(nBigID) then
			 EquipPart = Info("EquipType")
		else
			EquipPart = Info("EquipPart")
		end
		table.insert(tbl,{EquipPart,DynInfo.AddAttr1,DynInfo.AddAttr2})
	end
	return tbl
end

function CTooltips:ModifyEquipSuitPartName(SoulRoot,SuitName, displaySuitName)
	local SuitPartNameTbl 		= GetSuitPartName(SuitName)
	local _,EquipSuitInfoTbl 	= self:GetEquipSuitCount(SoulRoot,SuitName)
	local EquipAddAttrNameTbl 	= GetEquipAddAttrName(EquipSuitInfoTbl)
	local str = "" 
	for i = 1, #(SuitPartNameTbl) do 
		local Color = ColorTbl["套装缺少属性"]
		local Flag,Index = CheckIsSuitPart(SuitPartNameTbl[i],EquipSuitInfoTbl)
		if Flag then
			table.remove(EquipSuitInfoTbl,Index)
			Color = self.SuitColor
		end
		
		local name = CTooltips.GetIsTwinEquip(SuitName,SuitPartNameTbl[i][2],SuitPartNameTbl[i][3],SuitPartNameTbl[i][1])
		local displayPartName = Lan_SuitPart_Client(MemH64(SuitPartNameTbl[i][1]),"DisplaySuitPartName")
		str = str .. Color .. "#r"  .. "  " .. displaySuitName .. displayPartName .. name
	end
	return str
end

--得到装备进阶属性描述
function CTooltips:GetEquipAdvanceAttribute(DynInfo)
	local StrTbl = {}
	for i=1, 4 do
		local AttrName = DynInfo["AdvancedAttrName" .. i]
    	local AttrValue = DynInfo["AdvancedAttrValue" .. i]
    	if AttrName and AttrName ~= "" and AttrValue ~= 0 then
    		local SuperaddValue = 0
    		if DynInfo["SuperaddRate"] and DynInfo["SuperaddRate"] > 0 then
    			SuperaddValue = math.ceil(AttrValue * DynInfo["SuperaddRate"]) 
    			local SuperaddValueStr = ColorTbl["装备追加"] .. ToolTipsText(112,SuperaddValue) .. ColorTbl["进阶"]
    			StrTbl[i] = Lan_EquipIntensifyName_Common(MemH64(AttrName),"DisplayFormat") .. AttrValue .. "#R" .. SuperaddValueStr  .. "#r"
    		else
    			StrTbl[i] = Lan_EquipIntensifyName_Common(MemH64(AttrName),"DisplayFormat") .. AttrValue .. "#r"
    		end
    		self:SetPropertyNameToValueFun(AttrName,AttrValue + SuperaddValue)
    	end    		
	end
	local Str = #(StrTbl) > 0 and table.concat(StrTbl,"") or ""
	return Str 	
end

--得到精灵激活项名称
function CTooltips:GetJingLingPieceSkillName(DynInfo)
	local PieceSkillNameStr = ""
	if DynInfo.ChoosedAdvanceSkillPiece ~= nil and DynInfo.ChoosedAdvanceSkillPiece ~= 0 then
        local pieceTitleStr = string.format("%s%d", "SkillPieceName", DynInfo.ChoosedAdvanceSkillPiece)
        local SkillPieceName = JingLingTypeToPieceSkill_Common(DynInfo.JingLingType, pieceTitleStr)
		PieceSkillNameStr = g_AttributeOfJingLingTbl[SkillPieceName].DisplaySkillPieceName
	end								
	return PieceSkillNameStr
end

local function ChangeWndImage(Wnd,StrTexture)
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP_DISABLE	
	local DefaultImage = WND_IMAGE_LIST:new()
	DefaultImage:AddImage(Wnd:GetGraphic(), -1,  StrTexture, nil, CFPos:new(), "0xffffffff", 15 ) 	
	Wnd:SetWndBkImage( Flag, DefaultImage )
end

local function GetWndStrTexture(Quality)
	local str =  g_ImageMgr:GetImagePath(10007)
	local IconStrTexture =  str .. "namebox_" .. Quality .. ".tex"
	local NameWndStrTexture = str .. "box_" .. Quality .. ".tex"
	return IconStrTexture,NameWndStrTexture
end
-----------------------------------------------------------------------------------------------
function CTooltips:EquipToolTipWndPart1Fun(EquipToolTipWndPart1,TipsInfo,DynInfo,IsEquiped)
	local nBigID 				= TipsInfo.m_BigID
	local nIndex 				= TipsInfo.m_Index
	local nItemID 			= TipsInfo.m_ItemID
	local payType 			= TipsInfo.m_PayType
	local BindingStyle 	= TipsInfo.m_BindingStyle
	--是否身上装备
	local LabelText = EquipToolTipWndPart1:GetDlgChild("LabelText")
	if IsEquiped then
		LabelText:ShowWnd(true)
	else
		LabelText:ShowWnd(false)
	end
	
	--显示Icon
	local SmallIcon = g_ItemInfoMgr:GetItemInfo(nBigID, nIndex,"SmallIcon")
	local IconBtn = EquipToolTipWndPart1:GetDlgChild("IconBtn")
	g_LoadIconFromRes(SmallIcon, IconBtn, -2, IP_ENABLE, IP_CLICKDOWN)
	
	--设置物品名称和等级
	local NameRichText = EquipToolTipWndPart1:GetDlgChild("NameRichText")
	local DiTu2Btn = EquipToolTipWndPart1:GetDlgChild("DiTu2Btn")
	NameRichText:SetWndTextXR(true)
	local NameText = self:InsertEquipCurrentName(nBigID,nIndex, DynInfo,g_ItemInfoMgr:GetItemLanInfo(nBigID, nIndex,"DisplayName"))	--第1部分 名称
	NameRichText:SetWndText(NameText)
	local _, Quality  = self:GetSuitNameColor(DynInfo,nBigID,nIndex)	
	local DiTuStrTexture,NameWndStrTexture = GetWndStrTexture(Quality)
	ChangeWndImage(NameRichText,DiTuStrTexture)
	ChangeWndImage(DiTu2Btn,NameWndStrTexture)
	
	local rt_IconBtn = CFRect:new()
	IconBtn:GetLogicRect(rt_IconBtn)
	
	--设置进阶阶段
	local MaxPhase = 7
	local AdvanceCBtnTbl = {}
	local IsShowAdvancBtn = false
	local DiTuBtn = EquipToolTipWndPart1:GetDlgChild("DiTuBtn")
	for i = 1,MaxPhase do
		local CBtn = EquipToolTipWndPart1:GetDlgChild("AdvanceCBtn" .. i)
		CBtn:InserBefore(DiTuBtn)
		table.insert(AdvanceCBtnTbl,CBtn)
	end
	local AdvancedQuality = g_ItemInfoMgr:GetItemInfo(nBigID, nIndex,"AdvancedQuality")
	local MaxAdvancePhase = DynInfo and DynInfo.MaxAdvancePhase or AdvancedQuality
	for i = 1 ,MaxPhase do
		if i > MaxAdvancePhase then
			AdvanceCBtnTbl[i]:ShowWnd(false)
		else
			AdvanceCBtnTbl[i]:ShowWnd(true)
			IsShowAdvancBtn = true
		end
	end	
	local AdvancedCurLevel = g_ItemInfoMgr:GetItemInfo(nBigID, nIndex,"AdvancedCurLevel")
	local CurAdvancePhase = DynInfo and DynInfo.CurAdvancePhase or AdvancedCurLevel
	for i = 1,MaxPhase do
		if i <= CurAdvancePhase then
			AdvanceCBtnTbl[i]:SetCheck(true)
		else
			AdvanceCBtnTbl[i]:SetCheck(false)
		end
	end
	
	local rt_Advance = CFRect:new()
	if IsShowAdvancBtn then
		AdvanceCBtnTbl[1]:GetLogicRect(rt_Advance)
		local BindingStr = self:InsertEquipBindingType(nItemID, nBigID, nIndex, DynInfo,BindingStyle)
		if BindingStr then
			local BindingTypeText = EquipToolTipWndPart1:GetDlgChild("BindingTypeText")
			BindingTypeText:SetStyle(0x46000000)
			BindingTypeText:SetWndText(BindingStr)
			BindingTypeText:InserBefore(DiTuBtn)
		end
	else
		DiTuBtn:ShowWnd(false)
		local BindingStr = self:InsertEquipBindingType(nItemID, nBigID, nIndex, DynInfo,BindingStyle)
		local BindingTypeText = EquipToolTipWndPart1:GetDlgChild("BindingTypeText")
		BindingTypeText:SetStyle(0x44000000)
		if BindingStr then
			BindingTypeText:SetWndText(BindingStr)
			local rt_BindingType = CFRect:new()
			local Height = BindingTypeText:GetWndOrgHeight()
			local Width = BindingTypeText:GetWndOrgWidth()
			rt_BindingType.top = rt_IconBtn.bottom + 10
			rt_BindingType.bottom = rt_BindingType.top + Height
			rt_BindingType.left = rt_IconBtn.left - 3
			rt_BindingType.right = rt_BindingType.left + Width
			BindingTypeText:SetLogicRect(rt_BindingType)
			rt_Advance = rt_BindingType
			rt_Advance.bottom = rt_Advance.bottom - 5
			
		else
			rt_Advance = rt_IconBtn
			BindingTypeText:ShowWnd(false)
		end
	end
	
	local LimitRichTbl = {}
	local LimitNumber = 8
	for i = 1,LimitNumber do
		local RichText = EquipToolTipWndPart1:GetDlgChild("RichText" .. i)
		table.insert(LimitRichTbl,RichText)
	end
	
	local LimitTextTbl = {}
	
	--是否唯一
	local Only = g_ItemInfoMgr:GetItemInfo(nBigID, nIndex,"Only")
	local NumLimitStr = self:InsertEquipNumLimit(Only)
	if NumLimitStr then
		table.insert(LimitTextTbl,NumLimitStr)
	end

	--性别
	local Sex = g_ItemInfoMgr:GetItemInfo(nBigID, nIndex,"Sex")
	local SexStr = self:InsertEquipSex(Sex)
	if SexStr then
		table.insert(LimitTextTbl,SexStr)
	end
	
	--种族
	
	local Race = g_ItemInfoMgr:GetItemInfo(nBigID, nIndex,"Race")
	local RaceTbl = self:InsertEquipRace(Race)
	if RaceTbl and  #(RaceTbl) > 0 then
		for i = 1,#(RaceTbl) do 
			table.insert(LimitTextTbl,RaceTbl[i])
		end
	end

	--阵营限制
	local Camp = g_ItemInfoMgr:GetItemInfo(nBigID, nIndex,"Camp")
	local CampTbl = self:InsertEquipCamp(Camp)
	if CampTbl and #(CampTbl) > 0 then
		for i = 1, #(CampTbl) do
			table.insert(LimitTextTbl,CampTbl[i])
		end
	end
	
	--职业
	local Class = g_ItemInfoMgr:GetItemInfo(nBigID, nIndex,"Class")
	local ClassTbl = self:InsertEquipClass(Class)
	for i = 1,#(ClassTbl) do
		table.insert(LimitTextTbl,ClassTbl[i])
	end

	local rt_Limit = CFRect:new()
	local IsShowLimitBtn = false
	for i = 1, LimitNumber do
		if LimitTextTbl[i] then
			local tempRt = CFRect:new()
			LimitRichTbl[i]:GetLogicRect(tempRt)
			local heightWnd = LimitRichTbl[i]:GetWndOrgHeight()
			tempRt.top = rt_Advance.bottom + 10
			tempRt.bottom = tempRt.top + heightWnd
			LimitRichTbl[i]:SetWndText(LimitTextTbl[i])
			LimitRichTbl[i]:SetLogicRect(tempRt)
			LimitRichTbl[i]:ShowWnd(true)
			IsShowLimitBtn = true
		else
			LimitRichTbl[i]:ShowWnd(false)
		end
	end
	
	if IsShowLimitBtn then
		LimitRichTbl[1]:GetLogicRect(rt_Limit)
	else
		rt_Limit = rt_Advance
	end
	
	--设置装备部位
	local PartNameStatic = EquipToolTipWndPart1:GetDlgChild("PartNameStatic")
	if g_ItemInfoMgr:IsWeapon(nBigID) 
		or g_ItemInfoMgr:IsAssociateWeapon(nBigID) 
		or g_ItemInfoMgr:IsRing(nBigID) then
		PartNameStatic:SetWndText(ColorTbl["耐久和部位"] .. g_ItemInfoMgr:GetItemLanInfo(nBigID, nIndex,"DisplayEquipType"))
	else
		PartNameStatic:SetWndText(ColorTbl["耐久和部位"] .. g_ItemInfoMgr:GetItemLanInfo(nBigID, nIndex,"DisplayEquipPart"))
	end
	
	local rt_PartName = CFRect:new()
	local height_PartName = PartNameStatic:GetWndOrgHeight()
	PartNameStatic:GetLogicRect(rt_PartName)
	rt_PartName.top = rt_Limit.bottom + 2
	rt_PartName.bottom = rt_PartName.top + height_PartName
	PartNameStatic:SetLogicRect(rt_PartName)
	
	--耐久
	local DuraRichText = EquipToolTipWndPart1:GetDlgChild("DuraRichText")
	local DuraStr 
	local EquipPart = g_ItemInfoMgr:GetItemInfo(nBigID, nIndex,"EquipPart")
	if EquipPart == "背部" then
		DuraStr = self:InsertEquipLeftTime(DynInfo)
	else
		DuraStr = self:InsertEquipDuraInfo(DynInfo,nBigID, nIndex)
	end
	DuraRichText:SetWndText(DuraStr)	
	local rt_Dura = CFRect:new()
	local height_Dura = DuraRichText:GetWndOrgHeight()
	DuraRichText:GetLogicRect(rt_Dura)
	rt_Dura.top = rt_Limit.bottom + 2
	rt_Dura.bottom = rt_Dura.top + height_Dura
	DuraRichText:SetLogicRect(rt_Dura)
	
	local rt_Part1 = CFRect:new()
	EquipToolTipWndPart1:GetLogicRect(rt_Part1)
	rt_Part1.bottom = rt_Dura.bottom + 2
	EquipToolTipWndPart1:SetLogicRect(rt_Part1)
end

function CTooltips:GetBasePropertyStr(TipsInfo,DynInfo)
	local nBigID = TipsInfo.m_BigID
	local nIndex = TipsInfo.m_Index
	local nItemID = TipsInfo.m_ItemID
	local BasePropertyStr = ""
	if  g_ItemInfoMgr:IsWeapon(nBigID) then --第5部分 武器的伤害 和 武器攻击速度
		BasePropertyStr = self:InsertWeaponAttack(nIndex, nItemID, nBigID, DynInfo)			
		local DpsStr = self:InsertWeaponDPS(nIndex, nItemID, nBigID, DynInfo)	
		if DpsStr ~= "" then
			BasePropertyStr = BasePropertyStr .. "#r" .. DpsStr
		end
	elseif g_ItemInfoMgr:IsArmor(nBigID) or g_ItemInfoMgr:IsJewelry(nBigID) then
		local EquipPart = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"EquipPart")
		if  EquipPart and EquipPart == "背部" then
			BasePropertyStr = ""
		else
			BasePropertyStr = self:InsertOtherEquipProperty(DynInfo, nBigID, nIndex)
		end
	elseif g_ItemInfoMgr:IsRing(nBigID) then
		BasePropertyStr = self:InsertRingProperty(nItemID, nBigID, DynInfo, nIndex)
	elseif g_ItemInfoMgr:IsAssociateWeapon(nBigID) then
		BasePropertyStr = self:InsertShieldProperty(nBigID,nIndex, DynInfo)
	end
	return BasePropertyStr
end

function CTooltips:EquipToolTipWndPart2Fun(EquipToolTipWndPart2,BasePropertyStr,EnactmentAttr)
	local RichTextPart2 = EquipToolTipWndPart2:GetDlgChild("RichTextPart2")
	RichTextPart2:SetWndTextXR(true)
	RichTextPart2:SetAutoWidth(262, 262)	
	RichTextPart2:SetRowDist(1)
	BasePropertyStr = BasePropertyStr .. "#r" .. EnactmentAttr	
	RichTextPart2:SetWndText(BasePropertyStr)
end

function CTooltips:EquipToolTipWndPart3Fun(EquipToolTipWndPart3,item_type,item_name,DynInfo)
	local RichTextPart3 = EquipToolTipWndPart3:GetDlgChild("RichTextPart3")
	RichTextPart3:SetWndTextXR(true)
	RichTextPart3:SetAutoWidth(262, 262)
	RichTextPart3:SetRowDist(1)
	local AdvanceStr = self:InsertEquipAdvancedPhase(DynInfo,item_type,item_name)					--进阶阶段
	if DynInfo then
		AdvanceStr = AdvanceStr .. "#r" .. self:InsertEquipAdvance( DynInfo)				--进阶属性
	end	
	RichTextPart3:SetWndText(AdvanceStr)	
end

function CTooltips:EquipToolTipWndPart4Fun(EquipToolTipWndPart4,item_type,item_name,DynInfo)
    local RichTextPart4 = EquipToolTipWndPart4:GetDlgChild("RichTextPart3")
    RichTextPart4:SetWndTextXR(true)
    RichTextPart4:SetAutoWidth(262, 262)
    RichTextPart4:SetRowDist(1)
    
	local IntensifyStr = self.SuitColor .. self:InsertEquipIntensify(DynInfo,item_type,item_name)	--强化阶段
	if DynInfo and DynInfo.AddAttr1 ~= "" then
		IntensifyStr = IntensifyStr .. "#r" .. self:InsertEquipIntensifyAddAttr(DynInfo)			--强化附加属性
	end
	if DynInfo and DynInfo.IntensifyPhase >= 3 then
	    local itemInfo = g_ItemInfoMgr:GetItemInfo(item_type,item_name)
		IntensifyStr = IntensifyStr .. "#r" .. self:InsertEquipSuitShow(itemInfo, DynInfo)			--套装显示
	end
	
	RichTextPart4:SetWndText(IntensifyStr)	
end

function CTooltips:EquipToolTipWndPart5Fun(EquipToolTipWndPart5,DescriptionStr)
	local RichTextPart5 = EquipToolTipWndPart5:GetDlgChild("RichTextPart3")
	RichTextPart5:SetWndTextXR(true)
	RichTextPart5:SetAutoWidth(262, 262)
	RichTextPart5:SetRowDist(1)
	RichTextPart5:SetWndText(DescriptionStr)		
end

function CTooltips:EquipToolTipWndPart6Fun(EquipToolTipWndPart6,DynInfo)
	local tbl = {}
	tbl["火"] = GetStaticTextClient(24101)
	tbl["地"] = GetStaticTextClient(24102)
	tbl["风"] = GetStaticTextClient(24103)
	tbl["水"] = GetStaticTextClient(24104)
	tbl["冰"] = GetStaticTextClient(24105)
	local RichTextPart6 = EquipToolTipWndPart6:GetDlgChild("RichTextPart3")	
	RichTextPart6:SetWndTextXR(true)	
	RichTextPart6:SetAutoWidth(262, 262)
	RichTextPart6:SetRowDist(1)
	local color =  ColorTbl[DynInfo.JingLingType]
	color = color and color or ""
	local str = color .. tbl[DynInfo.JingLingType]
	local JingLingStr = self:InsertEquipJingLingSkill(DynInfo)
	str = str .. "#R" .. JingLingStr
	RichTextPart6:SetWndText(str)
end

function CTooltips:EquipToolTipWndPart7Fun(EquipToolTipWndPart7,PriceStr)
	local RichTextPart7 = EquipToolTipWndPart7:GetDlgChild("RichTextPart3")
	RichTextPart7:SetWndTextXR(true)
	RichTextPart7:SetAutoWidth(262, 262)
	RichTextPart7:SetRowDist(1)
	RichTextPart7:SetWndText(PriceStr )		
end

function CTooltips:EquipToolTipWndPart8Fun(EquipToolTipWndPart8,FixMoneyStr)
	local RichTextPart8 = EquipToolTipWndPart8:GetDlgChild("RichTextPart3")
	RichTextPart8:SetWndTextXR(true)
	RichTextPart8:SetAutoWidth(262, 262)
	RichTextPart8:SetRowDist(1)
	RichTextPart8:SetWndText(FixMoneyStr)	
end

function CTooltips:EquipToolTipWndPart9Fun(EquipToolTipWndPart9,EquipAssess)
	local RichTextPart9 = EquipToolTipWndPart9:GetDlgChild("RichTextPart3")
	RichTextPart9:SetWndTextXR(true)
	RichTextPart9:SetAutoWidth(262, 262)
	RichTextPart9:SetRowDist(1)
	local Str = ColorTbl["装备评分"] .. ToolTipsText(110) .. EquipAssess
	RichTextPart9:SetWndText(Str)
end


function CTooltips:EquipToolTipWndPart10Fun(EquipToolTipWndPart10,needTongInfo)
   	local RichTextPart10 = EquipToolTipWndPart10:GetDlgChild("RichTextPart3")
	RichTextPart10:SetWndTextXR(true)
	RichTextPart10:SetAutoWidth(262, 262)
	RichTextPart10:SetRowDist(1)
	RichTextPart10:SetWndText(needTongInfo) 
end

function CTooltips:EquipToolTipWndPart11Fun(EquipToolTipWndPart11,needTongPrice)
    local RichTextPart11 = EquipToolTipWndPart11:GetDlgChild("RichTextPart3")
	RichTextPart11:SetWndTextXR(true)
	RichTextPart11:SetAutoWidth(262, 262)
	RichTextPart11:SetRowDist(1)
	RichTextPart11:SetWndText(needTongPrice)   
end

function CTooltips:SetPropertyNameToValueFun(PropertyName,Value)
	if self.m_PropertyNameToValueTbl then
		if self.m_PropertyNameToValueTbl[PropertyName] then
			self.m_PropertyNameToValueTbl[PropertyName] = self.m_PropertyNameToValueTbl[PropertyName] + Value
		else
			self.m_PropertyNameToValueTbl[PropertyName] = Value
		end
	end
end

function CTooltips:GetEquipAssess(nBigID,nIndex,nItemID)
	local EquipAssess = 0
	local ValueTbl = self.m_PropertyNameToValueTbl
	ValueTbl["生命上限"] =  ValueTbl["生命上限"] and ValueTbl["生命上限"] or 0
	ValueTbl["护甲值"] = ValueTbl["护甲值"] and ValueTbl["护甲值"]  or 0
	ValueTbl["物理躲避值"] = ValueTbl["物理躲避值"] and ValueTbl["物理躲避值"] or 0
	ValueTbl["招架值"] = ValueTbl["招架值"] and ValueTbl["招架值"] or 0
	ValueTbl["韧性值"] = ValueTbl["韧性值"] and ValueTbl["韧性值"] or 0
	ValueTbl["自然抗值"] = ValueTbl["自然抗值"] and ValueTbl["自然抗值"] or 0
	ValueTbl["破坏抗值"] = ValueTbl["破坏抗值"] and ValueTbl["破坏抗值"] or 0
	ValueTbl["暗黑抗值"] = ValueTbl["暗黑抗值"] and ValueTbl["暗黑抗值"] or 0	
	ValueTbl["法术躲避值"] = ValueTbl["法术躲避值"] and ValueTbl["法术躲避值"] or 0		
	
	local ItemInfo  = g_ItemInfoMgr:GetItemInfo(nBigID, nIndex) 
	if g_ItemInfoMgr:IsWeapon(nBigID) 
		or g_ItemInfoMgr:IsRing(nBigID) 
		or g_ItemInfoMgr:IsAssociateWeapon(nBigID) then
			local WeaponDps = 0
			if g_ItemInfoMgr:IsWeapon(nBigID) then
				if (nItemID and nItemID > 0) then 
					local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(nItemID)
					WeaponDps = DynInfo.DPS 
				elseif (nItemID == 0 and tonumber(ItemInfo("IsStatic")) == 1) then
					local DPS = Equip_WeaponSProperty_Common(nIndex,"DPS")
					WeaponDps =  DPS and DPS or 0
				elseif (nItemID == 0 and tonumber(ItemInfo("IsStatic")) ~= 1) then
					WeaponDps = GetBaseWeaponDPS(ItemInfo("AttrLevel"),ItemInfo("EquipType"))
				end
			end
			
			ValueTbl["攻击力"] = ValueTbl["攻击力"] and ValueTbl["攻击力"] or 0
			ValueTbl["精准值"] = ValueTbl["精准值"] and ValueTbl["精准值"] or 0
			ValueTbl["暴击值"] = ValueTbl["暴击值"] and ValueTbl["暴击值"] or 0
			ValueTbl["暴击上限"] = ValueTbl["暴击上限"] and ValueTbl["暴击上限"] or 0
			ValueTbl["法伤"] = ValueTbl["法伤"] and ValueTbl["法伤"] or 0
			ValueTbl["法术命中"] = ValueTbl["法术命中"] and ValueTbl["法术命中"] or 0
			
			
			local value1 = (ValueTbl["攻击力"] + WeaponDps)*10 
			local value2 = ValueTbl["精准值"] + ValueTbl["暴击值"] + ValueTbl["暴击上限"]*2 + ValueTbl["法伤"]*10 + ValueTbl["法术命中"] 
			local value3 = ValueTbl["生命上限"]/2 + ValueTbl["护甲值"] + ValueTbl["物理躲避值"] + ValueTbl["招架值"] + ValueTbl["韧性值"]*2 
			local value4 = ValueTbl["自然抗值"] + ValueTbl["破坏抗值"] + ValueTbl["暗黑抗值"] + ValueTbl["法术躲避值"]
			
			if g_ItemInfoMgr:IsAssociateWeapon(nBigID) and tonumber(ItemInfo("IsShield")) == 1 then -- 盾牌
				EquipAssess = (value1 + value2 + value3 + value4)/100
			else --单手、主手、双手、副手、法器、戒指
				EquipAssess = (value1 + value2)/100 				
			end
	elseif g_ItemInfoMgr:IsJewelry(nBigID)  --头盔、肩膀、衣服、腰带、手套、鞋子、项链、佩饰：
		or g_ItemInfoMgr:IsArmor(nBigID) then	
		local value1 = ValueTbl["生命上限"]/2 + ValueTbl["护甲值"]+ ValueTbl["物理躲避值"] + ValueTbl["招架值"] + ValueTbl["韧性值"]*2  					
		local value2 = ValueTbl["自然抗值"] + ValueTbl["破坏抗值"] + ValueTbl["暗黑抗值"] + ValueTbl["法术躲避值"]
		EquipAssess = (value1 + value2)/100
	end
	
	if nItemID and nItemID > 0 then
		local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(nItemID)
		if DynInfo.IntensifyPhase and DynInfo.IntensifyPhase > 0 then
			EquipAssess = EquipAssess + 1
		end 
	end
	EquipAssess = string.format("%.2f",EquipAssess)
	return EquipAssess
end

function CTooltips:CreateEquipTipWnd(Tip,TipsInfo,IsEquiped)
	local nBigID = TipsInfo.m_BigID
	local nIndex = TipsInfo.m_Index
	local nItemID = TipsInfo.m_ItemID
	local payType = TipsInfo.m_PayType
	local GoldType = TipsInfo.m_GoldType

	local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(nItemID)
	if DynInfo == nil and nItemID ~= 0 then
		return
	end	
	self.m_PropertyNameToValueTbl = {}
	local EquipToolTipWndPart1 = Tip:PushTipsWnd("EquipToolTipWndPart1")
	self:EquipToolTipWndPart1Fun(EquipToolTipWndPart1,TipsInfo,DynInfo,IsEquiped)

	
	--装备的基础属性
	local BasePropertyStr = self:GetBasePropertyStr(TipsInfo,DynInfo)
	--护甲片属性
	local EnactmentAttr = self:InsertEquipEnactmentAttr(DynInfo)
	if BasePropertyStr ~= "" and BasePropertyStr ~= nil then
		local EquipToolTipWndPart2 = Tip:PushTipsWnd("EquipToolTipWndPart2")
		self:EquipToolTipWndPart2Fun(EquipToolTipWndPart2,BasePropertyStr,EnactmentAttr)
	end
	
	--装备进阶属性
	local AdvancedQuality		= g_ItemInfoMgr:GetItemInfo(nBigID, nIndex,"AdvancedQuality")
	if AdvancedQuality >= 1 then
		local EquipToolTipWndPart3 = Tip:PushTipsWnd("EquipToolTipWndPart3")
		self:EquipToolTipWndPart3Fun(EquipToolTipWndPart3,nBigID, nIndex,DynInfo)
	end
	
	local itemCfgAdvanceQuality = tonumber(g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"AdvancedQuality"))
	local itemCfgIntenQuality = tonumber(g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"IntensifyQuality"))
	--装备强化属性
    if ( DynInfo == nil and itemCfgAdvanceQuality >= 5 and itemCfgIntenQuality >=1 )
    	or (DynInfo and 
    	DynInfo.MaxAdvancePhase >= 5 and DynInfo.IntensifyQuality >=1) then
    	local EquipToolTipWndPart4 = Tip:PushTipsWnd("EquipToolTipWndPart3")
    	self.m_IsAimStatus = TipsInfo.m_IsAimStatus
		self:EquipToolTipWndPart4Fun(EquipToolTipWndPart4,nBigID, nIndex,DynInfo)
	end
	
	--物品说明
	local DescriptionStr = self:InsertEquipDescription(g_ItemInfoMgr:GetItemLanInfo(nBigID, nIndex,"Description"))
	DescriptionStr = DescriptionStr .. self:InsertItemMaker(DynInfo)
	if DescriptionStr ~= "" then
		local EquipToolTipWndPart5 = Tip:PushTipsWnd("EquipToolTipWndPart3")
		self:EquipToolTipWndPart5Fun(EquipToolTipWndPart5,DescriptionStr)
	end
	
	--精灵类型
	if DynInfo and DynInfo.MaxAdvancePhase > 1 then
		local EquipToolTipWndPart6 = Tip:PushTipsWnd("EquipToolTipWndPart3")
		self:EquipToolTipWndPart6Fun(EquipToolTipWndPart6,DynInfo)
	end
	
	--装备评分
	local EquipAssess = self:GetEquipAssess(nBigID,nIndex,nItemID)
	if tonumber(EquipAssess) > 0 then
		local EquipToolTipWndPart9 = Tip:PushTipsWnd("EquipToolTipWndPart3")	
		self:EquipToolTipWndPart9Fun(EquipToolTipWndPart9,EquipAssess)
	end

    --团贡商店要求的可购买等级
    local needTongDesc = g_Tooltips:GetSellItemTongInfo(nBigID, nIndex, payType,GoldType)
    if needTongDesc ~= "" then
        local EquipToolTipWndPart10 = Tip:PushTipsWnd("EquipToolTipWndPart3")		
		self:EquipToolTipWndPart10Fun(EquipToolTipWndPart10,needTongDesc)	
    end

    --团贡商店要求的金币价格
    local priceInTong = g_Tooltips:GetSellItemTongPrice(nBigID, nIndex, payType,GoldType)    
    if priceInTong ~= "" then
        local EquipToolTipWndPart11 = Tip:PushTipsWnd("EquipToolTipWndPart3")		
		self:EquipToolTipWndPart11Fun(EquipToolTipWndPart11,priceInTong)
    end
    
	--价格
	local PriceStr = self:InsertEquipPayTypePrice(nBigID, nIndex, payType,GoldType)
	if PriceStr ~= "" then
		local EquipToolTipWndPart7 = Tip:PushTipsWnd("EquipToolTipWndPart3")		
		self:EquipToolTipWndPart7Fun(EquipToolTipWndPart7,PriceStr)	
	end
	
	--修理价格
	local FixMoneyStr = self:InsertEquipFixMoneyStr(DynInfo)
    if g_GameMain.m_NewFixEquipWnd:IsShow() and FixMoneyStr ~= "" then
 		local EquipToolTipWndPart8 = Tip:PushTipsWnd("EquipToolTipWndPart3")		
		self:EquipToolTipWndPart8Fun(EquipToolTipWndPart8,FixMoneyStr)	   	
    end	
end

function CTooltips:CreateChatEquipWnd(ParentWnd,TipsInfo,IsEquiped)
	local nBigID = TipsInfo.m_BigID
	local nIndex = TipsInfo.m_Index
	local nItemID = TipsInfo.m_ItemID
	local payType = TipsInfo.m_PayType

	local DynInfo		= g_DynItemInfoMgr:GetDynItemInfo(nItemID)
	if DynInfo == nil and nItemID ~= 0 then
		return
	end	
	
	self.m_PropertyNameToValueTbl = {}
	ParentWnd.PartInfo = {}
	local EquipToolTipWndPart1 = CEquipLinkWndPart:new()
	EquipToolTipWndPart1:CreateFromRes("EquipToolTipWndPart1",ParentWnd)
	self:EquipToolTipWndPart1Fun(EquipToolTipWndPart1,TipsInfo,DynInfo,IsEquiped)
	EquipToolTipWndPart1:ShowWnd(true)
	table.insert(ParentWnd.PartInfo,EquipToolTipWndPart1)
	--公共聊天装备对比
	if ParentWnd.m_CloseBtn then
		ParentWnd.m_CloseBtn:InserBefore(EquipToolTipWndPart1)	
	end
	
	--设置位置
	local ParentRt = CFRect:new()
	ParentWnd:GetLogicRect(ParentRt)
	local tempRt = CFRect:new()
	local Part1Rt = CFRect:new()
	ParentRt.right = ParentRt.right + 8
	Part1Rt.left = ParentRt.left + 4
	Part1Rt.top = ParentRt.top + 4
	Part1Rt.right = Part1Rt.left + EquipToolTipWndPart1:GetWndOrgWidth()
	Part1Rt.bottom = Part1Rt.top + EquipToolTipWndPart1:GetWndOrgHeight()
	EquipToolTipWndPart1:SetLogicRect(Part1Rt)
	tempRt = Part1Rt
	
	--装备的基础属性
	local BasePropertyStr = self:GetBasePropertyStr(TipsInfo,DynInfo)
	--护甲片属性
	local EnactmentAttr = self:InsertEquipEnactmentAttr(DynInfo)
	if BasePropertyStr ~= "" then
		local EquipToolTipWndPart2 = CEquipLinkWndPart:new()
		EquipToolTipWndPart2:CreateFromRes("EquipToolTipWndPart2",ParentWnd)
		self:EquipToolTipWndPart2Fun(EquipToolTipWndPart2,BasePropertyStr,EnactmentAttr)
		EquipToolTipWndPart2:ShowWnd(true)
		--设置位置
		local Part2Rt = CFRect:new()
		Part2Rt.left = tempRt.left
		Part2Rt.top = tempRt.bottom
		Part2Rt.right = Part2Rt.left + EquipToolTipWndPart2:GetWndOrgWidth()
		Part2Rt.bottom = Part2Rt.top + EquipToolTipWndPart2:GetWndOrgHeight()
		EquipToolTipWndPart2:SetLogicRect(Part2Rt)
		tempRt = Part2Rt
		table.insert(ParentWnd.PartInfo,EquipToolTipWndPart2)	
	end

	--装备进阶属性
	local AdvancedQuality		= g_ItemInfoMgr:GetItemInfo(nBigID, nIndex,"AdvancedQuality")
	if AdvancedQuality > 0 then
		local EquipToolTipWndPart3 = CEquipLinkWndPart:new()
		EquipToolTipWndPart3:CreateFromRes("EquipToolTipWndPart3",ParentWnd)
		self:EquipToolTipWndPart3Fun(EquipToolTipWndPart3,nBigID, nIndex,DynInfo)
		EquipToolTipWndPart3:ShowWnd(true)
		--设置位置
		local Part3Rt = CFRect:new()
		Part3Rt.left = tempRt.left
		Part3Rt.top = tempRt.bottom		
		Part3Rt.right = Part3Rt.left + EquipToolTipWndPart3:GetWndOrgWidth()
		Part3Rt.bottom = Part3Rt.top + EquipToolTipWndPart3:GetWndOrgHeight()	
		EquipToolTipWndPart3:SetLogicRect(Part3Rt)
		tempRt = Part3Rt
		table.insert(ParentWnd.PartInfo,EquipToolTipWndPart3)
	end
	
	--装备强化属性
    if DynInfo and 
        DynInfo.MaxAdvancePhase >= 5 and DynInfo.IntensifyQuality >=1 then
    	local EquipToolTipWndPart4 = CEquipLinkWndPart:new()
    	EquipToolTipWndPart4:CreateFromRes("EquipToolTipWndPart3",ParentWnd)
		self:EquipToolTipWndPart4Fun(EquipToolTipWndPart4,nBigID, nIndex,DynInfo)
		EquipToolTipWndPart4:ShowWnd(true)
		--设置位置
		local Part4Rt = CFRect:new()
		Part4Rt.left = tempRt.left
		Part4Rt.top = tempRt.bottom		
		Part4Rt.right = Part4Rt.left + EquipToolTipWndPart4:GetWndOrgWidth()
		Part4Rt.bottom = Part4Rt.top + EquipToolTipWndPart4:GetWndOrgHeight()	
		EquipToolTipWndPart4:SetLogicRect(Part4Rt)
		tempRt = Part4Rt
		table.insert(ParentWnd.PartInfo,EquipToolTipWndPart4)		
	end
	
	--物品说明
	local DescriptionStr = self:InsertEquipDescription(g_ItemInfoMgr:GetItemLanInfo(nBigID, nIndex,"Description"))
	DescriptionStr = DescriptionStr .. self:InsertItemMaker(DynInfo)
	if DescriptionStr ~= "" then
		local EquipToolTipWndPart5 = CEquipLinkWndPart:new()
		EquipToolTipWndPart5:CreateFromRes("EquipToolTipWndPart3",ParentWnd)
		self:EquipToolTipWndPart5Fun(EquipToolTipWndPart5,DescriptionStr)
		EquipToolTipWndPart5:ShowWnd(true)
		--设置位置
		local Part5Rt = CFRect:new()
		Part5Rt.left = tempRt.left
		Part5Rt.top = tempRt.bottom		
		Part5Rt.right = Part5Rt.left + EquipToolTipWndPart5:GetWndOrgWidth()
		Part5Rt.bottom = Part5Rt.top + EquipToolTipWndPart5:GetWndOrgHeight()	
		EquipToolTipWndPart5:SetLogicRect(Part5Rt)
		tempRt = Part5Rt		
		table.insert(ParentWnd.PartInfo,EquipToolTipWndPart5)
	end
	
	--精灵类型
	if DynInfo then
		local EquipToolTipWndPart6 = CEquipLinkWndPart:new()
		EquipToolTipWndPart6:CreateFromRes("EquipToolTipWndPart3",ParentWnd)
		self:EquipToolTipWndPart6Fun(EquipToolTipWndPart6,DynInfo)
		EquipToolTipWndPart6:ShowWnd(true)
		--设置位置
		local Part6Rt = CFRect:new()
		Part6Rt.left = tempRt.left
		Part6Rt.top = tempRt.bottom		
		Part6Rt.right = Part6Rt.left + EquipToolTipWndPart6:GetWndOrgWidth()
		Part6Rt.bottom = Part6Rt.top + EquipToolTipWndPart6:GetWndOrgHeight()	
		EquipToolTipWndPart6:SetLogicRect(Part6Rt)
		tempRt = Part6Rt	
		table.insert(ParentWnd.PartInfo,EquipToolTipWndPart6)	
	end

	--装备评分
	local EquipAssess = self:GetEquipAssess(nBigID,nIndex,nItemID)
	if tonumber(EquipAssess) > 0 then
		local EquipToolTipWndPart9 = CEquipLinkWndPart:new()
		EquipToolTipWndPart9:CreateFromRes("EquipToolTipWndPart3",ParentWnd)	
		self:EquipToolTipWndPart9Fun(EquipToolTipWndPart9,EquipAssess)
		EquipToolTipWndPart9:ShowWnd(true)
		--设置位置
		local Part9Rt = CFRect:new()
		Part9Rt.left = tempRt.left
		Part9Rt.top = tempRt.bottom		
		Part9Rt.right = Part9Rt.left + EquipToolTipWndPart9:GetWndOrgWidth()
		Part9Rt.bottom = Part9Rt.top + EquipToolTipWndPart9:GetWndOrgHeight()	
		EquipToolTipWndPart9:SetLogicRect(Part9Rt)
		tempRt = Part9Rt	
		table.insert(ParentWnd.PartInfo,EquipToolTipWndPart9)		
	end

	--价格
	local PriceStr = self:InsertEquipPayTypePrice(nBigID, nIndex, payType)
	if PriceStr ~= "" then
		local EquipToolTipWndPart7 = CEquipLinkWndPart:new()
		EquipToolTipWndPart7:CreateFromRes("EquipToolTipWndPart3",ParentWnd)		
		self:EquipToolTipWndPart7Fun(EquipToolTipWndPart7,PriceStr)	
		EquipToolTipWndPart7:ShowWnd(true)
		--设置位置
		local Part7Rt = CFRect:new()
		Part7Rt.left = tempRt.left
		Part7Rt.top = tempRt.bottom		
		Part7Rt.right = Part7Rt.left + EquipToolTipWndPart7:GetWndOrgWidth()
		Part7Rt.bottom = Part7Rt.top + EquipToolTipWndPart7:GetWndOrgHeight()	
		EquipToolTipWndPart7:SetLogicRect(Part7Rt)
		tempRt = Part7Rt	
		table.insert(ParentWnd.PartInfo,EquipToolTipWndPart7)
	end
	for i = 1 ,#(ParentWnd.PartInfo) do
		ParentWnd.PartInfo[i]:EnableWnd(false)
	end
	ParentRt.bottom = tempRt.bottom
	ParentWnd:SetLogicRect(ParentRt)
end

--得到装备属性和属性值
function CTooltips:GetEquipAssessFun(nBigID,nIndex,nItemID)
	self.m_PropertyNameToValueTbl = {}
	self.SuitColor = ""
	local InfoTbl = {}
	InfoTbl.m_BigID = nBigID
	InfoTbl.m_Index = nIndex
	InfoTbl.m_ItemID = nItemID
	local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(nItemID)
	if DynInfo == nil then
		return
	end	
	
	--装备的基础属性
	local BasePropertyStr = self:GetBasePropertyStr(InfoTbl,DynInfo)
	--护甲片属性
	local EnactmentAttr = self:InsertEquipEnactmentAttr(DynInfo)
	--进阶属性
	self:InsertEquipAdvance( DynInfo)
	--强化属性
	self:InsertEquipIntensifyAddAttr(DynInfo)
	local EquipAssess = self:GetEquipAssess(nBigID,nIndex,nItemID)
	return tonumber(EquipAssess)
end
