gac_gas_require "item/Equip/EquipDefInc"
gac_gas_require "item/Equip/EquipChecking"
cfg_load "equip/ArmorProValue_Common"

--原先的Equip_Armor_Common,StaticJewery_Common表中的EquipPart属性列填的是数字，现在改成填字符串
--但是程序中很多地方用到那列填数字的（如和其他数字的比较），所以用下面的这个tbl做一个映射
--字符串到数字的映射 3:头部 4:衣服 5:腰带  7:鞋子  14:肩部 15:护手 16:背部	10:项链 11:饰品
EquipPartNameToNum ={}
EquipPartNameToNum["头部"]			= 3
EquipPartNameToNum["衣服"]			= 4
EquipPartNameToNum["腰带"]			= 5
EquipPartNameToNum["鞋子"]			= 7
EquipPartNameToNum["肩部"]			= 14
EquipPartNameToNum["护手"]			= 15
EquipPartNameToNum["背部"]			= 16
EquipPartNameToNum["项链"]			= 10
EquipPartNameToNum["佩饰"]			= 11

--装备材料
EEquipMaterial.eLoricae 			= 1 	--铠甲
EEquipMaterial.eFur 				= 2 	--皮甲
EEquipMaterial.eCloth 				= 3 	--布甲
EEquipMaterial.eShortWeapon 		= 4 	--短兵
EEquipMaterial.eLongWeapon  		= 5 	--长兵
EEquipMaterial.eMagicWeapon 		= 6 	--法兵

--配饰类型
EJewelryType.eNecklace				= 1 	--项链
EJewelryType.eAccouterment			= 2 	--饰品
EJewelryType.eRing					= 3 	--戒指

--装备档次
EEquipQuality.eWhite				= 1		--白装
EEquipQuality.eGreen				= 2		--绿装
EEquipQuality.eBlue					= 3		--蓝装
EEquipQuality.ePurple				= 4 	--紫装

--装备使用多少次掉1点耐久
g_EquipUseNumDelOne[EEquipPart.eWeapon] 			= 600
g_EquipUseNumDelOne[EEquipPart.eAssociateWeapon] 	= 0
g_EquipUseNumDelOne[EEquipPart.eHead] 				= 550
g_EquipUseNumDelOne[EEquipPart.eWear] 				= 500
g_EquipUseNumDelOne[EEquipPart.eSash] 				= 650
g_EquipUseNumDelOne[EEquipPart.eShoe] 				= 700

--武器的标准攻击间隔
g_WeaponSpeed[EClass.eCL_Warrior]    		= 1.9
g_WeaponSpeed[EClass.eCL_MagicWarrior]    	= 1.6
g_WeaponSpeed[EClass.eCL_Paladin]   		= 1.1
g_WeaponSpeed[EClass.eCL_NatureElf]   		= 1.3
g_WeaponSpeed[EClass.eCL_EvilElf]    		= 1.4
g_WeaponSpeed[EClass.eCL_Priest]    		= 1.5
g_WeaponSpeed[EClass.eCL_DwarfPaladin]    	= 1.5
g_WeaponSpeed[EClass.eCL_OrcWarrior]  		= 1.6

-- 0 为世界通用
g_WeaponDrift[0]    						= 10
g_WeaponDrift[EClass.eCL_Warrior]    		= 10
g_WeaponDrift[EClass.eCL_MagicWarrior]    	= 10
g_WeaponDrift[EClass.eCL_Paladin]   		= 10
g_WeaponDrift[EClass.eCL_NatureElf]   		= 10
g_WeaponDrift[EClass.eCL_EvilElf]    		= 10
g_WeaponDrift[EClass.eCL_Priest]    		= 10
g_WeaponDrift[EClass.eCL_DwarfPaladin]    	= 10
g_WeaponDrift[EClass.eCL_OrcWarrior]  		= 10



--确认部位是否正确
function g_CheckPart(nNum)
	if nNum >= EEquipPart.eWeapon and nNum <= EEquipPart.eBack then
		return true
	else 
		return false
	end
end

function g_CheckEquipQuality(nNum)
	return true
end

--确认材料是否正确
function g_CheckMaterial(nPart,nMaterial)
	if nPart == EEquipPart.eWeapon then
		if nMaterial >= EEquipMaterial.eShortWeapon and nMaterial <= EEquipMaterial.eMagicWeapon then
			return true
		else
			return false
		end
	elseif nPart >= EEquipPart.eHead and nPart <= EEquipPart.eShoe then
		if nMaterial >= EEquipMaterial.eLoricae and nMaterial <= EEquipMaterial.eCloth then
			return true
		else
			return false
		end
	end
	return true
end

--获得材料的名字
function g_GetMaterialName(nPart,nMaterial)
	if nPart == EEquipPart.eWeapon or (nPart >= EEquipPart.eHead and nPart <= EEquipPart.eShoe) then
		if nMaterial == EEquipMaterial.eLoricae	then
			return "铠甲"
		elseif nMaterial == EEquipMaterial.eFur then
			return "皮甲"
		elseif nMaterial == EEquipMaterial.eCloth	then
			return "布甲"
		elseif nMaterial == EEquipMaterial.eShortWeapon then
			return "短兵"
		elseif nMaterial == EEquipMaterial.eLongWeapon	then
			return "长兵"
		elseif nMaterial == EEquipMaterial.eMagicWeapon then
			return "法兵"
		end
	end
	return nil
end

--判断门派和材料是否匹配
function g_CheckMaterialMatchClass(nEquipPart,nMaterial,nClass)
	if nEquipPart >= EEquipPart.eHead and nEquipPart <= EEquipPart.eShoe then
		if nMaterial == EEquipMaterial.eLoricae	then
			if nClass == EClass.eCL_Warrior or nClass == EClass.eCL_MagicWarrior then
				return true
			else
				return false
			end
		elseif nMaterial == EEquipMaterial.eFur	then
			if nClass == EClass.eCL_EvilElf or nClass == EClass.eCL_Paladin or nClass == EClass.eCL_NatureElf then
				return true
			else
				return false
			end
		elseif nMaterial == EEquipMaterial.eCloth	then
			if nClass == EClass.eCL_Priest or nClass == EClass.eCL_DwarfPaladin or nClass == EClass.eCL_ElfHunter then
				return true
			else
				return false
			end
		end
	elseif nEquipPart == EEquipPart.eWeapon then
		return true
	elseif nEquipPart == EEquipPart.eAssociateWeapon then
		return true
	end
	return false
end

--根据装备的info得到该装备对应的产出属性名称
--参数：根据g_ItemInfoMgr:GetItemInfo(nBigID,nIndex)得出的结果
--返回值：装备产出时伴随的三个属性名称
function GetEquipOutputAttrName(nBigID,nIndex)
	local tblAttrName = {}
	local armorAttrTempTbl = {"HealthPoint", "Defence", "NatureResistanceValue", "DestructionResistanceValue", "EvilResistanceValue"}
	local EquipType	= g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"EquipType")
	local EquipPart	= g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"EquipPart")
	for i =1, 5 do
		if ArmorProValue_Common(EquipType,EquipPart,armorAttrTempTbl[i]) ~= "" and tonumber(ArmorProValue_Common(EquipType,EquipPart,armorAttrTempTbl[i])) > 0 then
			local attrName = GetAttrName(armorAttrTempTbl[i])
			table.insert( tblAttrName, attrName )
		end
	end
	return tblAttrName
end

