cfg_load "item/Equip_JewelrySProperty_Common"
cfg_load "item/Equip_ArmorSProperty_Common"
cfg_load "item/Equip_ShieldSProperty_Common"

--原先的Equip_Armor_Common,StaticJewery_Common表中的EquipPart属性列填的是数字，现在改成填字符串
--但是程序中很多地方用到那列填数字的（如和其他数字的比较），所以用下面的这个tbl做一个映射
--字符串到数字的映射 3:头部 4:衣服 5:腰带  7:鞋子  14:肩部 15:护手 16:背部	10:项链 11:饰品
EEquipPart = {}

EEquipPart.eWeapon					= 1		--主手
EEquipPart.eAssociateWeapon			= 2		--副手
EEquipPart.eHead					= 3		--头部
EEquipPart.eWear					= 4		--衣服
EEquipPart.eSash					= 5		--腰带
	--EEquipPart.eCuff					= 6		--护腕
EEquipPart.eShoe					= 7		--鞋子
EEquipPart.eRingLeft				= 8		--戒指左
EEquipPart.eRingRight				= 9		--戒指右
EEquipPart.eNecklace				= 10	--项链
EEquipPart.eAccouterment			= 11	--佩饰
	--EEquipPart.eManteau				= 12    --披风
	--EEquipPart.eCoat					= 13	--外套
EEquipPart.eShoulder				= 14	--肩部
EEquipPart.eHand					= 15	--护手
EEquipPart.eBack					= 16    --背部

----------------------------------------------------------------------------------
--得到防具、饰品、盾牌的静态属性名称和数值
function GetStaticPropertyName(nBigId,sIndex)
	local PropertyNameTbl = {"HealthPoint","Defence","NatureResistanceValue",
							"DestructionResistanceValue","EvilResistanceValue",
							"StrikeValue","StrikeMultiValue","AccuratenessValue",
							"MagicHitValue","ManaPoint"}
	local NameAndValueTbl = {}
	local Info 
	if g_ItemInfoMgr:IsArmor(nBigId) then
		Info = Equip_ArmorSProperty_Common(sIndex)
	elseif g_ItemInfoMgr:IsJewelry(nBigId) then
		Info = Equip_JewelrySProperty_Common(sIndex)
	elseif g_ItemInfoMgr:IsStaticShield(nBigId) then
		Info = Equip_ShieldSProperty_Common(sIndex)
	else
		assert(false,nBigId .. "错误的BigID")
	end
	
	for i = 1, #(PropertyNameTbl) do
		if Info(PropertyNameTbl[i]) and Info(PropertyNameTbl[i]) ~= "" then
			table.insert(NameAndValueTbl,{PropertyNameTbl[i],Info(PropertyNameTbl[i])})
		end
	end
	return NameAndValueTbl
end
