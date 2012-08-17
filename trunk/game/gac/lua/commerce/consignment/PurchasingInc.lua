cfg_load "commerce/PurchasingAgent_Common"

CPurchasing = class ()

--属性列索引:
local AttrNameMapTbl = {}
AttrNameMapTbl["单手剑"] = "单手剑"
AttrNameMapTbl["单手锤"] = "单手锤"
AttrNameMapTbl["单手斧"] = "单手斧"
AttrNameMapTbl["单手刀"] = "单手刀"
AttrNameMapTbl["主手自然杖"] = "主手杖"
AttrNameMapTbl["主手暗黑杖"] = "主手杖"
AttrNameMapTbl["主手破坏杖"] = "主手杖"
AttrNameMapTbl["主手杖"] = "主手杖"
AttrNameMapTbl["双手剑"] = "双手剑"
AttrNameMapTbl["双手斧"] = "双手斧"
AttrNameMapTbl["双手锤"] = "双手锤"
AttrNameMapTbl["双手自然杖"] = "法杖"
AttrNameMapTbl["双手暗黑杖"] = "法杖"
AttrNameMapTbl["双手破坏杖"] = "法杖"
AttrNameMapTbl["双手杖"] = "法杖"

AttrNameMapTbl["盾牌"] = "盾牌"
AttrNameMapTbl["法器"] = "法器"
AttrNameMapTbl["头部"] = "头部"
AttrNameMapTbl["衣服"] = "衣服"
AttrNameMapTbl["腰带"] = "腰带"
AttrNameMapTbl["鞋子"] = "鞋子"
AttrNameMapTbl["肩部"] = "肩部"
AttrNameMapTbl["护手"] = "护手"
AttrNameMapTbl["背部"] = "背部"

AttrNameMapTbl["项链"] = "项链"
AttrNameMapTbl["佩饰"] = "佩饰"

AttrNameMapTbl["攻击符石"] = "攻击符石"
AttrNameMapTbl["命中符石"] = "命中符石"
AttrNameMapTbl["防御符石"] = "防御符石"
AttrNameMapTbl["韧性符石"] = "韧性符石"
AttrNameMapTbl["上限符石"] = "上限符石"
AttrNameMapTbl["法防符石"] = "法防符石"

AttrNameMapTbl["强化石"] = "强化石"

AttrNameMapTbl["药水"] = "药剂"
AttrNameMapTbl["国战药水"] = "药剂"
AttrNameMapTbl["药剂"] = "药剂"
AttrNameMapTbl["食物"] = "食品"
AttrNameMapTbl["传送卷轴"] = "魔法物品"
AttrNameMapTbl["队友召唤符"] = "魔法物品"
AttrNameMapTbl["复活石"] = "魔法物品"
AttrNameMapTbl["多倍卷轴"] = "魔法物品"
AttrNameMapTbl["魔法物品"] = "魔法物品"

AttrNameMapTbl["锻造铸甲材料"] = "锻造、铸甲材料"
AttrNameMapTbl["烹饪制药材料"] = "烹饪、制药材料"
AttrNameMapTbl["杂物"] = "普通物品"
AttrNameMapTbl["普通物品"] = "普通物品"
AttrNameMapTbl["坐骑"] = "坐骑"


local tbl_ItemParentNodeTbl ={}
tbl_ItemParentNodeTbl[1] = {5} --武器
tbl_ItemParentNodeTbl[2] = {6,7}	--防具
tbl_ItemParentNodeTbl[3] = {8,9}	--饰品
tbl_ItemParentNodeTbl[4] = {14, 15}	--符石系统
tbl_ItemParentNodeTbl[5] = {34, 38, 43, 49, 50}	--装备强化
tbl_ItemParentNodeTbl[6] = {3,46}	--消耗品
tbl_ItemParentNodeTbl[7] = {2,24,25,27,28,30,32,37,40}	--杂物
tbl_ItemParentNodeTbl[8] = {10,17,18,19,23,26,29,31,36,39}	--其他物品

tbl_TypeToChildNode = {}
tbl_TypeToChildNode[2] = "背包"
tbl_TypeToChildNode[8] = "戒指"
tbl_TypeToChildNode[15] = "符文"
tbl_TypeToChildNode[24] = "宝箱"
tbl_TypeToChildNode[25] = "书籍"
tbl_TypeToChildNode[27] = "锻造、铸甲材料"
tbl_TypeToChildNode[28] = "锻造、铸甲材料"
tbl_TypeToChildNode[30] = "矿图"
tbl_TypeToChildNode[32] = "种子"
tbl_TypeToChildNode[34] = "加持秘典"
tbl_TypeToChildNode[37] = "修行塔扩展包"
tbl_TypeToChildNode[38] = "进阶石"
tbl_TypeToChildNode[40] = "小宠物"
tbl_TypeToChildNode[43] = "重铸石"
tbl_TypeToChildNode[46] = "玩法物品"
tbl_TypeToChildNode[49] = "强化石"
tbl_TypeToChildNode[50] = "追加石"
	
tbl_BaseItemNode = {}
tbl_BaseItemNode[1] = {}
tbl_BaseItemNode[1]["烹饪、制药材料"] = 7
tbl_BaseItemNode[1]["锻造、铸甲材料"] = 7
tbl_BaseItemNode[1]["强化石"] = 5
tbl_BaseItemNode[1]["坐骑"] = 7
tbl_BaseItemNode[1]["普通物品"] = 7
	
	
local CfgInfo = { [5] = {Equip_Weapon_Common, "EquipType"}, 
							[6] = {Equip_Armor_Common,  "EquipPart"}, 
							[7] = {Equip_Shield_Common, "EquipType"},
							[8] = {Equip_Ring_Common, "EquipType"}, 
							[9] = {Equip_Jewelry_Common,"EquipPart"},
							[g_ItemInfoMgr:GetStoneBigID()] = {Stone_Common, "StoneType"},
							[g_ItemInfoMgr:GetSkillItemBigID()] = {SkillItem_Common, "ItemType"},
							[g_ItemInfoMgr:GetBasicItemBigID()] = {BasicItem_Common, "CSMSortType"},
}







function GetChildNodeByItemType(ItemBigID,ItemName)
	for i=1, # tbl_ItemParentNodeTbl do
		local tbl_Temp = tbl_ItemParentNodeTbl[i]
		for j=1, # tbl_Temp do
			if tbl_Temp[j] == ItemBigID then
				return i
			end
		end
	end
	local TreeList_ChildNode = ""
	local Info = CfgInfo[ItemBigID]
	if Info then
		TreeList_ChildNode = Info[1](ItemName,Info[2])
		TreeList_ChildNode = AttrNameMapTbl[TreeList_ChildNode]
		if tbl_BaseItemNode[ItemBigID] then
			return tbl_BaseItemNode[ItemBigID][TreeList_ChildNode]
		end
	end
end

function GetChildNode(ItemBigID,ItemName)
	local TreeList_ChildNode = ""
	local Info = CfgInfo[ItemBigID]
	if Info then
		TreeList_ChildNode = Info[1](ItemName,Info[2])
		TreeList_ChildNode = AttrNameMapTbl[TreeList_ChildNode]
	elseif tbl_TypeToChildNode[ItemBigID] then
		TreeList_ChildNode = tbl_TypeToChildNode[ItemBigID]
	end
	
--	local keys = Lan_CSMTreeList_Client:GetKeys()
--	for i,v in pairs( keys ) do
--		for k=1, 11 do
--			local ChildNodeStr = string.format("%s%d", "ChildNode", k-1)
--			local ChildNodeName = CSMTreeList_Client(uIndex,ChildNodeStr)
--			if ChildNodeName == TreeList_ChildNode then
--				return TreeList_ChildNode
--			end
--		end
--	end
	return TreeList_ChildNode
end