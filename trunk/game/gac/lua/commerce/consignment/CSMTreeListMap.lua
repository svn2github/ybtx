CConsignmentTreeListMap = class ()

local CfgTblItemBigID = {
	BasicItem		= 	g_ItemInfoMgr:GetBasicItemBigID(),
	PlayerBag 	 	=	g_ItemInfoMgr:GetPlayerBagBigID(),
	
	StaticWeapon	=	g_ItemInfoMgr:GetStaticWeaponBigID(),
	StaticArmor		=	g_ItemInfoMgr:GetStaticArmorBigID(),
	StaticShield	=	g_ItemInfoMgr:GetStaticShieldBigID(),
	StaticRing		=	g_ItemInfoMgr:GetStaticRingBigID(),
	StaticJewelry	=	g_ItemInfoMgr:GetStaticJewelryBigID(),
	
	Stone			=	g_ItemInfoMgr:GetStoneBigID(),
	HoleMaterial	=	g_ItemInfoMgr:GetHoleMaterialBigID(),
	
	QuestItem		=	g_ItemInfoMgr:GetQuestItemBigID(),
	Mail			=	g_ItemInfoMgr:GetMailBigID(),
	WhiteStone		=	g_ItemInfoMgr:GetWhiteStoneBigID(),
	SoulPearl		=	g_ItemInfoMgr:GetSoulPearlBigID(),
	SkillItem		=	g_ItemInfoMgr:GetSkillItemBigID(),
	TongSmeltItem	=	g_ItemInfoMgr:GetTongSmeltItemBigID(),
	BoxItem			=	g_ItemInfoMgr:GetBoxItemBigID(),
	TextItem		=	g_ItemInfoMgr:GetTextItemBigID(),
	BattleArrayBooks=	g_ItemInfoMgr:GetBattleArrayBooksBigID(),
	EmbryoItem	=g_ItemInfoMgr:GetEmbryoItemBigID(),
	QualityMaterialItem = g_ItemInfoMgr:GetQualityMaterialItemBigID(),
	
	Flowers         =   g_ItemInfoMgr:GetFlowersBigID(),
	MercCardItem    =   g_ItemInfoMgr:GetMercCardItemBigID(),
	AdvanceStone    =   g_ItemInfoMgr:GetAdvanceStoneBigID(),
	ArmorPiece      =   g_ItemInfoMgr:GetArmorPieceBigID(),
	OreArea         =   g_ItemInfoMgr:GetOreAreaBigID(),
	CreateNpcItem   =   g_ItemInfoMgr:GetCreateNpcItemBigID(),
	EquipIntenBackItem= g_ItemInfoMgr:GetEquipIntensifyBackItem(),
	EquipSuperaddItem = g_ItemInfoMgr:GetEquipSuperaddItem(),
	EquipRefineItem = g_ItemInfoMgr:GetEquipRefineStoneType(),
	
	MathGameItem    =   g_ItemInfoMgr:GetMathGameItemBigID(),
}

local FuncTbl ={
	GetSearchInfoByItemType 		= 1,			--[查询某类物品]对应的Gac2Gas:GetSearchInfoByItemType,参数：nBigID（物品大类）
	GetSearchInfoNeedParent 		= 2,			--[查询某个类型符合某两个属性列的物品]对应的Gac2Gas:GetSearchInfoByItemAttr，参数:nBigID， parentNodeText, curNodeText
	GetSearchInfoBySomeItemType 	= 3,			--[查询某几类物品]对应的Gac2Gas:GetSearchInfoBySomeItemType,参数：type(数字代表某几个配置表)
	GetSearchInfoByExactItemAttr	= 4,				--[查询某类物品中的某个属性列表中的一个具体的属性]对应的Gac2Gas:CSMGetOrderByExactItemAttr,参数：nBigID, attrColName(属性列名)，exactAttrName（具体的某个属性：如布甲）
    GetSearchInfoByItemTypeOrAttr     = 5,            --[查询某几类物品中某一列符合要求的好几种物品]对应的Gac2Gas:CSMGetOrderBySeveralSortItem,参数：index(物品type所在的tbl的索引值)，attrIndex(物品在某几个配置表中的某几列符合要求的tbl索引值)
}

local IcludingSeveralItemTypeTbl = { 
	Sundries 	= 1,	--杂物，包含BoxItem_Common、BasicItem_Common、EmbryoItem_Common、QualityMaterialItem_Common、PlayerBag_Common、QuestItem_Common的所有物品
	Jewelry 	= 2,	--StaticJewelry_Common\StaticRing_Common
	Stone			= 3, --Stone_Common\HoleMaterial_Common
	ConsumeItem = 4, --消耗品：SkillItem_Common、BasicItem_Common
	EquipIntensify  = 5,    --装备强化相关：AdvanceStone_Common、ArmorPieceEnactment_Common
	Others      =   6,  --除了拍卖行中已列出类别中的其他物品类10、17 18 19 23 26 28 29 31 36 39
--所有的    1 2 3  5 6 7 8 9 10 14 15 16 17 18 19 23 24 25 26 27 28 29 30 31 32 34 36 37 38 39
--（已列出的1、2、3、5、6、7、8、9、14、15、24、25、27、30、32、34、37、38）
    BasicItem       =   7,--basiItem_Common
    ArmorAndShield  =   8,
    EmbryoItem = 9,
}

--一个属性列中多个符合要求的列值
local IncludingSeveralSortItemType = {
    EquipIntenItem  =   1,  --itemType1, -- 强化石、重铸石
    ConsumeItem     =   2,  ----药剂、食品、魔法物品    
    Sunderies       =   3,  -- --烹饪制药材料、普通物品、坐骑  
}

function CConsignmentTreeListMap.CreateCSMTreeListMap()
 local TreeListMap =
{
	["武器"]	 = {FuncTbl.GetSearchInfoByItemType, 		 CfgTblItemBigID.StaticWeapon},
	["单手剑"]	 = {FuncTbl.GetSearchInfoByExactItemAttr, 	 CfgTblItemBigID.StaticWeapon, CConsignment.AttrNameMapIndexTbl["单手剑"]},
	["单手锤"]	 = {FuncTbl.GetSearchInfoByExactItemAttr, 	 CfgTblItemBigID.StaticWeapon, CConsignment.AttrNameMapIndexTbl["单手锤"]},
	["单手斧"]	 = {FuncTbl.GetSearchInfoByExactItemAttr, 	 CfgTblItemBigID.StaticWeapon, CConsignment.AttrNameMapIndexTbl["单手斧"]},
	["单手刀"]	 = {FuncTbl.GetSearchInfoByExactItemAttr, 	 CfgTblItemBigID.StaticWeapon, CConsignment.AttrNameMapIndexTbl["单手刀"]},
	["主手杖"] = {FuncTbl.GetSearchInfoByExactItemAttr, CfgTblItemBigID.StaticWeapon, CConsignment.AttrNameMapIndexTbl["主手杖"]},
	["双手剑"] = {FuncTbl.GetSearchInfoByExactItemAttr, 	 CfgTblItemBigID.StaticWeapon, CConsignment.AttrNameMapIndexTbl["双手剑"]},
	["双手斧"] = {FuncTbl.GetSearchInfoByExactItemAttr, 	 CfgTblItemBigID.StaticWeapon, CConsignment.AttrNameMapIndexTbl["双手斧"]},
	["双手锤"] = {FuncTbl.GetSearchInfoByExactItemAttr, 	 CfgTblItemBigID.StaticWeapon, CConsignment.AttrNameMapIndexTbl["双手锤"]},
    ["法杖"] = {FuncTbl.GetSearchInfoByExactItemAttr, CfgTblItemBigID.StaticWeapon, CConsignment.AttrNameMapIndexTbl["双手杖"]},
	
	["防具"]	 = {FuncTbl.GetSearchInfoBySomeItemType, 		 IcludingSeveralItemTypeTbl.ArmorAndShield},
	["盾牌"] = {FuncTbl.GetSearchInfoByExactItemAttr, 	 CfgTblItemBigID.StaticShield, CConsignment.AttrNameMapIndexTbl["盾牌"]},
	["法器"] = {FuncTbl.GetSearchInfoByExactItemAttr, 	 CfgTblItemBigID.StaticShield, CConsignment.AttrNameMapIndexTbl["法器"]},
	["头部"]	 = {FuncTbl.GetSearchInfoByExactItemAttr,  		 CfgTblItemBigID.StaticArmor, CConsignment.AttrNameMapIndexTbl["头部"]},
	["胸部"]	 = {FuncTbl.GetSearchInfoByExactItemAttr,  		 CfgTblItemBigID.StaticArmor, CConsignment.AttrNameMapIndexTbl["衣服"]},
	["腰部"]	 = {FuncTbl.GetSearchInfoByExactItemAttr,  		 CfgTblItemBigID.StaticArmor, CConsignment.AttrNameMapIndexTbl["腰带"]},
	["脚部"]	 = {FuncTbl.GetSearchInfoByExactItemAttr,  		 CfgTblItemBigID.StaticArmor, CConsignment.AttrNameMapIndexTbl["鞋子"]},
	["肩部"]	 = {FuncTbl.GetSearchInfoByExactItemAttr,  		 CfgTblItemBigID.StaticArmor, CConsignment.AttrNameMapIndexTbl["肩部"]},
	["手部"]	 = {FuncTbl.GetSearchInfoByExactItemAttr,  		 CfgTblItemBigID.StaticArmor, CConsignment.AttrNameMapIndexTbl["护手"]},
	["背部"]	 = {FuncTbl.GetSearchInfoByExactItemAttr,  		 CfgTblItemBigID.StaticArmor, CConsignment.AttrNameMapIndexTbl["背部"]},
	
	["饰品"]	 = {FuncTbl.GetSearchInfoBySomeItemType, 	 IcludingSeveralItemTypeTbl.Jewelry},
    ["项链"]	 = {FuncTbl.GetSearchInfoByExactItemAttr,	 CfgTblItemBigID.StaticJewelry, CConsignment.AttrNameMapIndexTbl["项链"]},
	["佩饰"]	 = {FuncTbl.GetSearchInfoByExactItemAttr,  CfgTblItemBigID.StaticJewelry, CConsignment.AttrNameMapIndexTbl["佩饰"]},
	["戒指"]	 = {FuncTbl.GetSearchInfoByItemType,  		 CfgTblItemBigID.StaticRing},
 	
 	["符石系统"]	 = {FuncTbl.GetSearchInfoBySomeItemType, 	 IcludingSeveralItemTypeTbl.Stone},
 	["攻击符石"] = {FuncTbl.GetSearchInfoByExactItemAttr, CfgTblItemBigID.Stone, CConsignment.AttrNameMapIndexTbl["攻击符石"]},
 	["命中符石"] = {FuncTbl.GetSearchInfoByExactItemAttr, CfgTblItemBigID.Stone, CConsignment.AttrNameMapIndexTbl["命中符石"]},
 	["防御符石"] = {FuncTbl.GetSearchInfoByExactItemAttr, CfgTblItemBigID.Stone, CConsignment.AttrNameMapIndexTbl["防御符石"]},
 	["韧性符石"] = {FuncTbl.GetSearchInfoByExactItemAttr, CfgTblItemBigID.Stone, CConsignment.AttrNameMapIndexTbl["韧性符石"]},
 	["上限符石"] = {FuncTbl.GetSearchInfoByExactItemAttr, CfgTblItemBigID.Stone, CConsignment.AttrNameMapIndexTbl["上限符石"]},
 	["法防符石"] = {FuncTbl.GetSearchInfoByExactItemAttr, CfgTblItemBigID.Stone, CConsignment.AttrNameMapIndexTbl["法防符石"]},
 	["符文"]	 = {FuncTbl.GetSearchInfoByItemType, CfgTblItemBigID.HoleMaterial},
 	
 	["装备强化"] = {FuncTbl.GetSearchInfoByItemTypeOrAttr, IcludingSeveralItemTypeTbl.EquipIntensify, IncludingSeveralSortItemType.EquipIntenItem},
 	["进阶石"] = {FuncTbl.GetSearchInfoByItemType, CfgTblItemBigID.AdvanceStone},
 	["强化石"] = {FuncTbl.GetSearchInfoByExactItemAttr, CfgTblItemBigID.BasicItem, CConsignment.AttrNameMapIndexTbl["强化石"]},
 	["重铸石"] = {FuncTbl.GetSearchInfoByItemType, CfgTblItemBigID.EquipIntenBackItem},
 	["加持秘典"] = {FuncTbl.GetSearchInfoByItemType, CfgTblItemBigID.ArmorPiece},
 	["追加石"] = {FuncTbl.GetSearchInfoByItemType, CfgTblItemBigID.EquipSuperaddItem},
    ["精炼石"] = {FuncTbl.GetSearchInfoByItemType, CfgTblItemBigID.EquipRefineItem}, 	
 	
 	["消耗品"]		 = {FuncTbl.GetSearchInfoByItemTypeOrAttr, IcludingSeveralItemTypeTbl.ConsumeItem, IncludingSeveralSortItemType.ConsumeItem},
    ["药剂"]			 = {FuncTbl.GetSearchInfoByExactItemAttr, CfgTblItemBigID.SkillItem, CConsignment.AttrNameMapIndexTbl["药剂"]},
	["食品"]			 = {FuncTbl.GetSearchInfoByExactItemAttr, CfgTblItemBigID.SkillItem, CConsignment.AttrNameMapIndexTbl["食物"]},
    ["魔法物品"]			 = {FuncTbl.GetSearchInfoByExactItemAttr, CfgTblItemBigID.BasicItem, CConsignment.AttrNameMapIndexTbl["魔法物品"]}, 	
 	["玩法物品"] = {FuncTbl.GetSearchInfoByItemType, CfgTblItemBigID.MathGameItem},
 	
 	["杂物"]		 	 = {FuncTbl.GetSearchInfoByItemTypeOrAttr,	IcludingSeveralItemTypeTbl.Sundries,IncludingSeveralSortItemType.Sunderies},
    ["烹饪、制药材料"]			 = {FuncTbl.GetSearchInfoByExactItemAttr, CfgTblItemBigID.BasicItem, CConsignment.AttrNameMapIndexTbl["烹饪制药材料"]},
	["种子"]			 = {FuncTbl.GetSearchInfoByItemType, CfgTblItemBigID.Flowers},
	["矿图"] = {FuncTbl.GetSearchInfoByItemType, CfgTblItemBigID.OreArea},
	["锻造、铸甲材料"]	 = {FuncTbl.GetSearchInfoBySomeItemType, 	 IcludingSeveralItemTypeTbl.EmbryoItem},
	["普通物品"]	 = {FuncTbl.GetSearchInfoByExactItemAttr, 			CfgTblItemBigID.BasicItem, CConsignment.AttrNameMapIndexTbl["杂物"]},
    ["坐骑"] = {FuncTbl.GetSearchInfoByExactItemAttr, CfgTblItemBigID.BasicItem, CConsignment.AttrNameMapIndexTbl["坐骑"]},
	["小宠物"] = {FuncTbl.GetSearchInfoByItemType,		 CfgTblItemBigID.CreateNpcItem},
	["背包"]	 		 = {FuncTbl.GetSearchInfoByItemType,		 CfgTblItemBigID.PlayerBag},
	["宝箱"]			 = {FuncTbl.GetSearchInfoByItemType,		 CfgTblItemBigID.BoxItem},
	["书籍"] = {FuncTbl.GetSearchInfoByItemType,		 CfgTblItemBigID.TextItem},
	["修行塔扩展包"] = {FuncTbl.GetSearchInfoByItemType,		 CfgTblItemBigID.MercCardItem},
	["其他物品"]       = {FuncTbl.GetSearchInfoBySomeItemType, IcludingSeveralItemTypeTbl.Others}
}
	return TreeListMap
end
