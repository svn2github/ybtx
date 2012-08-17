lan_load "commerce/Lan_CSMEquipSoul_Common"
cfg_load "commerce/CSMEquipSoul_Common"

CConsignment = class ()

--属性列索引:
CConsignment.AttrNameMapIndexTbl = {}
CConsignment.AttrNameMapIndexTbl["单手剑"] = 1
CConsignment.AttrNameMapIndexTbl["单手锤"] = 2
CConsignment.AttrNameMapIndexTbl["单手斧"] = 3
CConsignment.AttrNameMapIndexTbl["单手刀"] = 4
CConsignment.AttrNameMapIndexTbl["主手自然杖"] = 5
CConsignment.AttrNameMapIndexTbl["主手暗黑杖"] = 5
CConsignment.AttrNameMapIndexTbl["主手破坏杖"] = 5
CConsignment.AttrNameMapIndexTbl["主手杖"] = 5
CConsignment.AttrNameMapIndexTbl["双手剑"] = 6
CConsignment.AttrNameMapIndexTbl["双手斧"] = 7
CConsignment.AttrNameMapIndexTbl["双手锤"] = 8
CConsignment.AttrNameMapIndexTbl["双手自然杖"] = 9
CConsignment.AttrNameMapIndexTbl["双手暗黑杖"] = 9
CConsignment.AttrNameMapIndexTbl["双手破坏杖"] = 9
CConsignment.AttrNameMapIndexTbl["双手杖"] = 9

CConsignment.AttrNameMapIndexTbl["盾牌"] = 10
CConsignment.AttrNameMapIndexTbl["法器"] = 11
CConsignment.AttrNameMapIndexTbl["头部"] = 12
CConsignment.AttrNameMapIndexTbl["衣服"] = 13
CConsignment.AttrNameMapIndexTbl["腰带"] = 14
CConsignment.AttrNameMapIndexTbl["鞋子"] = 15
CConsignment.AttrNameMapIndexTbl["肩部"] = 16
CConsignment.AttrNameMapIndexTbl["护手"] = 17
CConsignment.AttrNameMapIndexTbl["背部"] = 18

CConsignment.AttrNameMapIndexTbl["项链"] = 19
CConsignment.AttrNameMapIndexTbl["佩饰"] = 20

CConsignment.AttrNameMapIndexTbl["攻击符石"] = 21
CConsignment.AttrNameMapIndexTbl["命中符石"] = 22
CConsignment.AttrNameMapIndexTbl["防御符石"] = 23
CConsignment.AttrNameMapIndexTbl["韧性符石"] = 24
CConsignment.AttrNameMapIndexTbl["上限符石"] = 25
CConsignment.AttrNameMapIndexTbl["法防符石"] = 26

CConsignment.AttrNameMapIndexTbl["强化石"] = 27

CConsignment.AttrNameMapIndexTbl["药水"] = 29
CConsignment.AttrNameMapIndexTbl["国战药水"] = 29
CConsignment.AttrNameMapIndexTbl["药剂"] = 29
CConsignment.AttrNameMapIndexTbl["食物"] = 30
CConsignment.AttrNameMapIndexTbl["传送卷轴"] = 31
CConsignment.AttrNameMapIndexTbl["队友召唤符"] = 31
CConsignment.AttrNameMapIndexTbl["复活石"] = 31
CConsignment.AttrNameMapIndexTbl["多倍卷轴"] = 31
CConsignment.AttrNameMapIndexTbl["魔法物品"] = 31

CConsignment.AttrNameMapIndexTbl["烹饪制药材料"] = 32
CConsignment.AttrNameMapIndexTbl["杂物"] = 33 
CConsignment.AttrNameMapIndexTbl["坐骑"] = 34    

AddCheckLeakFilterObj(CConsignment.AttrNameMapIndexTbl)

local EquipQualityTbl = {}
EquipQualityTbl["蓝装"] = 3
EquipQualityTbl["紫装"] = 4
EquipQualityTbl["橙装"] = 5

function CConsignment.GetEquipQuality(IntensifyPhase, DynInfo, ItemInfo)
	local quality = 0
	local suitName = DynInfo.SuitName
	if IntensifyPhase ~= nil  then
	    if IntensifyPhase >= 3 then
			if suitName == "" then
				quality = EquipQualityTbl["蓝装"]
			else
			    local equipPart = GetIntenEquipPart(DynInfo.m_nBigID, ItemInfo)
				local number = CheckSuitCount(ItemInfo("SoulRoot"),DynInfo.SuitName, equipPart)
				if number == 6 or number == 2  then
					quality = EquipQualityTbl["紫装"]
				elseif number == 4 or number == 3 or number == 8 then
					quality = EquipQualityTbl["橙装"]
				end
			end
		elseif IntensifyPhase >= 1 then
			quality = EquipQualityTbl["蓝装"]
		else
			quality =  ItemInfo("Quality")
		end
	else
		quality =  ItemInfo("Quality")
	end 
	return quality
end


local EquipSoulRootType = {}
local EquipAdvanceSoulRootType = {}

g_AdvancedSoul2DisplayRootTbl = {}
g_Index2DisplayRootTbl = {}


local AdvanceSoulCount = 0

local function ReadCSMEquipSoulInfo()
    local advanceSoulIndex = 1
    local intenSoulIndex = 1
    local keys = Lan_CSMEquipSoul_Common:GetKeys()
    for i, v in pairs (keys) do
        local key = v
        local soulName = Lan_CSMEquipSoul_Common(key, "SoulRoot")
        local soulRoot = CSMEquipSoul_Common(i,"SoulRoot")
        if CSMEquipSoul_Common(i,"SoulType") == 1 then
            EquipAdvanceSoulRootType[soulName] = advanceSoulIndex
            advanceSoulIndex = advanceSoulIndex + 1
            AdvanceSoulCount = AdvanceSoulCount + 1
        elseif CSMEquipSoul_Common(i,"SoulType") == 2 then
            EquipSoulRootType[soulName] = intenSoulIndex
            intenSoulIndex = intenSoulIndex + 1      
        end
        g_AdvancedSoul2DisplayRootTbl[soulRoot] =  soulName
        g_Index2DisplayRootTbl[v] =  soulName

    end
end
ReadCSMEquipSoulInfo()

function CConsignment.GetEquipAdvanceSoulTbl()
    return EquipAdvanceSoulRootType
end

function CConsignment.GetEquipIntenSoulTbl()
    return EquipSoulRootType
end

function CConsignment.GetEquipAdvanceSoul(advanceSoulName) 
    return EquipAdvanceSoulRootType[advanceSoulName] or 0
end

function CConsignment.GetEquipIntenSoul(intenSoulName)
    return EquipSoulRootType[intenSoulName] or 0
end

function CConsignment.GetAdvanceSoulCount()
    return AdvanceSoulCount
end