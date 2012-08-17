cfg_load "equip/HandType_Common"
cfg_load "item/SoulPearlLimitType_Common"
gac_gas_require "item/item_info_mgr/ItemInfoMgrInc"
gac_gas_require "item/Equip/EquipDef"

engine_require "common/Misc/TypeCheck"
gac_gas_require	"framework/common/GlobalRequires"
cfg_load "equip/StaticWeaponType_Common"
lan_load "item/Lan_ItemType_Common"


--代码结构做了变更啦，现在没有ID啦，只有
local Type2BigID = {
	["Material"]       = 1,
	["PlayerBay"]      = 2,
	["SkillItem"]      = 3,
	["StaticWeapon"]   = 5,
	["StaticArmor"]    = 6,
	["StaticShield"]   = 7,
	["StaticJewelry"]  = 8,
	["FortifiedStone"] = 14,
	["AssistantStone"] = 15,
}

--校验物品类型是否合法
function CItemInfoMgr:IsValid(itemType)
	local nBigID = str_or_num_to_num(itemType)
	if(nBigID >= 1 and nBigID <= self.m_tblId2Table[nBigID] ~= nil ) then
		return true
	end
	return false
end

function CItemInfoMgr:CheckType(itemType,nIndex)
	local nBigID = str_or_num_to_num(itemType)
	if self:IsSoulPearl(itemType) then
		nIndex = self:GetSoulpearlInfo(nIndex)
	end
	if self.m_tblId2Table[nBigID] == nil or self.m_tblId2Table[nBigID](nIndex) == nil then
		return false
	end

	return true
end

function CItemInfoMgr:GetBasicItemBigID()
	return 1
end

--获得背包类型的id
function CItemInfoMgr:GetPlayerBagBigID()
	return 2
end

--获得技能物品类型id
function CItemInfoMgr:GetSkillItemBigID()
	return 3
end

--获得武器类型的id
function CItemInfoMgr:GetStaticWeaponBigID()
	return 5
end

--获得防具类型的id
function CItemInfoMgr:GetStaticArmorBigID()
	return 6
end

--获得暗器类型的id
function CItemInfoMgr:GetStaticShieldBigID()
	return 7
end

--获得戒指类型的id
function CItemInfoMgr:GetStaticRingBigID()
	return 8
end

--获得饰品类型的id
function CItemInfoMgr:GetStaticJewelryBigID()
	return 9
end

function CItemInfoMgr:GetTongItemBigID()
	return 10
end

--获得宝石类型id
function CItemInfoMgr:GetStoneBigID()
	return 14
end

--获得打孔材料类型id
function CItemInfoMgr:GetHoleMaterialBigID()
	return 15
end

--获得任务物品类型id
function CItemInfoMgr:GetQuestItemBigID()
	return 16
end

--获得邮件附件类型id
function CItemInfoMgr:GetMailBigID()
	return 17
end

--获得白宝石类型id
function CItemInfoMgr:GetWhiteStoneBigID()
	return 18
end

--获得魂珠类型id
function CItemInfoMgr:GetSoulPearlBigID()
	return 19
end

--获得帮会炼金物品类型id
function CItemInfoMgr:GetTongSmeltItemBigID()
	return 23
end

--获得盒子物品类型id
function CItemInfoMgr:GetBoxItemBigID()
	return 24
end


function CItemInfoMgr:GetEmbryoItemBigID()
	return 27
end

function CItemInfoMgr:GetQualityMaterialItemBigID()
	return 28
end

--获得文本物品类型id
function CItemInfoMgr:GetTextItemBigID()
	return 25
end

--获得阵法书类型id
function CItemInfoMgr:GetBattleArrayBooksBigID()
	return 26
end

--获得锻造类胚胎类型id
function CItemInfoMgr:GetEmbryoBigID()
	return 27
end

--获得锻造类品质材料类型id
function CItemInfoMgr:GetQualityMaterialBigID()
	return 28
end

function CItemInfoMgr:GetOreAreaBigID()
    return 30
end

--获得装备鉴定卷轴id
function CItemInfoMgr:GetEquipIdentifyScrollBigID()
	return 31
end

function CItemInfoMgr:GetFlowersBigID()
    return 32
end


--获得护甲片id
function CItemInfoMgr:GetArmorPieceBigID()
	return 34
end

function CItemInfoMgr:GetMercCardItemBigID()
    return 37
end

--获得进阶石ID
function CItemInfoMgr:GetAdvanceStoneBigID()
	return 38
end

--获得小宠物ID
function CItemInfoMgr:GetCreateNpcItemBigID()
    return 40
end

--获得幻兽蛋id
function CItemInfoMgr:GetPetEggBigID()
	return 41
end
--是否为幻兽蛋
function CItemInfoMgr:IsPetSkillStone(nBigID)
	return 42 == str_or_num_to_num(nBigID)
end

--获得幻兽灵石id
function CItemInfoMgr:GetPetSkillStoneBigID()
	return 42
end
--是否为幻兽灵石
function CItemInfoMgr:IsPetEgg(nBigID)
	return 41 == str_or_num_to_num(nBigID)
end


function CItemInfoMgr:GetMathGameItemBigID()
    return 46
end
--是否为装备
function CItemInfoMgr:IsEquip(nBigID)
	return self:IsStaticEquip(nBigID)
end

--是否为戒指
function CItemInfoMgr:IsEquipNotJewelry(nBigID)
	return self:IsStaticEquipNotJewelry(nBigID)
end

--是否静态装备
function CItemInfoMgr:IsWeapon(nBigID)
	return self:IsStaticWeapon(nBigID)
end

--判断是否为防具
function CItemInfoMgr:IsArmor(nBigID)
	return self:IsStaticArmor(nBigID)
end

function CItemInfoMgr:IsAssociateWeapon(nBigID)
	return self:IsStaticShield(nBigID)
end

--是否是饰品
function CItemInfoMgr:IsJewelry(nBigID)
	return self:IsStaticJewelry(nBigID)
end

--是否是装备精炼石
function CItemInfoMgr:IsEquipRefineItem(nBigID)
    return 49 == str_or_num_to_num(nBigID)
end

function CItemInfoMgr:IsEquipSuperaddItem(nBigID)
    return 50 == str_or_num_to_num(nBigID)
end


--************************************************************************
--|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
--静态装备，不包含饰品
function CItemInfoMgr:IsStaticEquipNotJewelry(itemType)
	local nBigID = str_or_num_to_num(itemType)
	return nBigID >= 5 and nBigID <= 7
end

--是否是玩家背包
function CItemInfoMgr:IsPlayerBag(nBigID)
	return 2 == str_or_num_to_num(nBigID)
end

--是否为物品触发技能,参数：物品id
function CItemInfoMgr:IsItemBurstSkill(nBigID)
	return 3 == str_or_num_to_num(nBigID)
end

--判断是否为武器
function CItemInfoMgr:IsStaticWeapon(nBigID)
	return 5 == str_or_num_to_num(nBigID)
end

--判断是否为防具
function CItemInfoMgr:IsStaticArmor(nBigID)
	return 6 == str_or_num_to_num(nBigID)
end

--判断是否为盾牌
function CItemInfoMgr:IsStaticShield(nBigID)
	return 7 == str_or_num_to_num(nBigID)
end

--是否是戒指
function CItemInfoMgr:IsRing(nBigID)
	return 8 == str_or_num_to_num(nBigID) 
end

--判断是否为饰品
function CItemInfoMgr:IsStaticJewelry(nBigID)
	return 9 == str_or_num_to_num(nBigID)
end

--是否是宝石
function CItemInfoMgr:IsStone(nBigID)
	return 14 == str_or_num_to_num(nBigID)
end

--是否为强化宝石
function CItemInfoMgr:IsFortifiedStone(nBigID)
	return 14 == str_or_num_to_num(nBigID)
end

--是否为祝福宝石
function CItemInfoMgr:IsAssistantStone(nBigID)
	return 15 == str_or_num_to_num(nBigID)
end

--判断是否是任务物品，任务物品16
function CItemInfoMgr:IsQuestItem(nBigID)
	return 16 == str_or_num_to_num(nBigID)
end

--判断是否为邮件文本附件
function CItemInfoMgr:IsMailTextAttachment(nBigID)
	return 17 == str_or_num_to_num(nBigID)
end

--是否为物品触发技能,参数：物品id
function CItemInfoMgr:IsWhiteStone(nBigID)
	return 18 == str_or_num_to_num(nBigID)
end

--是否能通过additem 的gm命令直接添加出来的物品
function CItemInfoMgr:CanAddByAddItemGM(nBigID)
	return 19 ~= str_or_num_to_num(nBigID)
end

--判断是否帮会炼金物品
function CItemInfoMgr:IsTongSmeltItem(nBigID)
	return 23 == str_or_num_to_num(nBigID)
end

--判断是否是盒子物品
function CItemInfoMgr:IsBoxitem(nBigID)
	return 24 == str_or_num_to_num(nBigID)
end

--是否是文本物品
function CItemInfoMgr:IsTextBook(nBigID)
	return 25 == str_or_num_to_num(nBigID)
end

--是否是阵法书
function CItemInfoMgr:IsBattleArrayBooks(nBigID)
	return 26 == str_or_num_to_num(nBigID)
end

--是否是锻造胚胎
function CItemInfoMgr:IsEmbryoItem(nBigID)
	return 27 == str_or_num_to_num(nBigID)
end

--是否是品质材料
function CItemInfoMgr:IsQualityMaterialItem(nBigID)
	return 28 == str_or_num_to_num(nBigID)
end

--判断是否是矿藏图
function CItemInfoMgr:IsOreAreaMap(nBigId)
	return 30 == str_or_num_to_num(nBigId)
end

--判断是否是装备鉴定卷轴
function CItemInfoMgr:IsEquipIdentifyScroll(nBigId)
	return 31 == str_or_num_to_num(nBigId)
end

--判断是否是花卉技能物品
function CItemInfoMgr:IsCultivateFlowerItem(nBigId)
	return 32 == str_or_num_to_num(nBigId)
end

--判断是否是帮会建筑卷轴
function CItemInfoMgr:IsTongBuildingItem(nBigId)
	return 33 == str_or_num_to_num(nBigId)
end

--判断是否是护甲片
function CItemInfoMgr:IsArmorPiece(nBigId)
	return 34 == str_or_num_to_num(nBigId)
end

--判断是否是进阶石
function CItemInfoMgr:IsAdvanceStone(nBigId)
	return 38 == str_or_num_to_num(nBigId)
end

--判断是否是大重铸石
function CItemInfoMgr:IsEquipIntenBackItem(nBigID)
    return 43 == str_or_num_to_num(nBigID)
end

--判断是否是采矿工具
function CItemInfoMgr:IsPickOreItem(nBigID)
    return 45 == str_or_num_to_num(nBigID)
end

--判断是否是商城宝箱
function CItemInfoMgr:IsTurntableItem(nBigID)
    return 47 == str_or_num_to_num(nBigID)
end

function CItemInfoMgr:IsEquipRefineStone(nBigID)
    return 49 == str_or_num_to_num(nBigID)
end

--|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
--************************************************************************

function CItemInfoMgr:GetAppendAttrID()
	return 1
end
function CItemInfoMgr:GetSpecialMagicAttrID()
	return 2
end
function CItemInfoMgr:GetAddCountID()
	return 3
end
function CItemInfoMgr:GetChaseCountID()
	return 4
end
function CItemInfoMgr:GetInitEquipID()
	return 5
end

function CItemInfoMgr:GetInitSkillID()
	return 6
end

function CItemInfoMgr:GetEquipRefineStoneType()
    return 49
end

function CItemInfoMgr:GetEquipIntensifyBackItem()
    return 43
end

function CItemInfoMgr:GetEquipSuperaddItem()
    return 50
end

--返回的第二的结果是过期后的显示名字
function CItemInfoMgr:GetItemLifeInfo(nBigID, nIndex)
	if self:IsSoulPearl(nBigID) then
		nIndex = self:GetSoulpearlInfo(nIndex)
	end
	assert(self:HaveItem(nBigID,nIndex), "该计时物品不存在.大类:" .. nBigID .. "名称:" .. nIndex)
	local keys = ItemLife_Common:GetKeys()
	for i, key in pairs(keys) do
		local p = ItemLife_Common(key)
		if (tonumber(nBigID) == tonumber(p("ItemType")) and nIndex == p("ItemName")) then
			return p, Lan_ItemLife_Common(MemH64(nIndex),"TimeOutName")
		end
	end
end

function CItemInfoMgr:GetTableByBigID(itemType)
	local nBigID = str_or_num_to_num(itemType)
	assert(IsNumber(nBigID) and nBigID > 0 and self.m_tblId2Table[nBigID] ~= nil)
	return self.m_tblId2Table[nBigID]
end

function CItemInfoMgr:GetNoBigIdTable(nID,nIndex)
	return self.m_tblNoBigId[nID](nIndex)
end

function CItemInfoMgr:GetItemLanInfo(itemType, sName, sCoulmnName)
	if self:IsSoulPearl(itemType) then
		sName = self:GetSoulpearlInfo(sName)
	end
	local tbl = self.m_tblId2LanTable[itemType]
	assert(tbl, "无法找到该类物品的对应国际化语言表.物品大类:" .. itemType)
	local info = tbl(MemH64(sName))
	assert(info, "物品信息在对应国际化语言表中无法找到.物品大类:" .. itemType .. "物品名称:" .. sName)
	local ColumnInfo = info(sCoulmnName)
	if sCoulmnName == "Description" then
		local sDescType = self:GetItemInfo(itemType, sName, "DescType")
		if sDescType and "" ~= sDescType then
			local SpecialColumnInfo = Lan_ItemType_Common(MemH64(sDescType), "DisplayName")
			ColumnInfo = GetStrAccordWildcard(ColumnInfo,SpecialColumnInfo)
		end
	end
	return ColumnInfo or ""
end

function CItemInfoMgr:GetItemLanFunInfo(itemType, sName)
	if self:IsSoulPearl(itemType) then
		sName = self:GetSoulpearlInfo(sName)
	end
	local tbl = self.m_tblId2LanTable[itemType]
	assert(tbl, "无法找到该类物品的对应国际化语言表.物品大类:" .. itemType)
	local info = tbl(MemH64(sName))
	assert(info, "物品信息在对应国际化语言表中无法找到.物品大类:" .. itemType .. "物品名称:" .. sName)
	return info
end

function CItemInfoMgr:GetItemLanInfoJustByName(sName,ColumnName)
	local SoulPearlName = self:GetSoulpearlInfo(sName)
	local index = MemH64(SoulPearlName)
	local info = self.m_tblId2LanTable[19][index]
	if (info) then
		return info
	end
	
	local index = MemH64(sName)
	for k, v in pairs(self.m_tblId2LanTable) do
		local info = v(index,ColumnName)
		if( info ) then
			return info
		end
	end
	assert(false, "物品信息在所有物品国际化语言表中无法找到,物品名称:" .. sName)
end

function CItemInfoMgr:GetItemMH64ByDisplayName(itemType, sDisplayName)
	local tblResult = {}
	local tbl = self.m_tblId2LanTable[itemType]
	assert(tbl, "无法找到该类物品的对应国际化语言表.物品大类:" .. itemType)
	local tblKeys  = tbl:GetKeys()
	for k, v in ipairs(tblKeys) do
		local DisplayName = tbl(v, "DisplayName")
		if ( sDisplayName == DisplayName) then
			table.insert(tblResult, v)
		end
	end
	return tblResult
end
---------------------------------某个物品是否可用的检查----------------------------
--职业是否正确
function CItemInfoMgr:BeEquipClass(nBigID,nIndex,nClass)
	local ClassTbl = loadstring("return {" .. self:GetItemInfo(nBigID,nIndex,"Class") .. "}")()
	if #ClassTbl ~= 0 then
		local Flag = true
		local tempstr = ""
		for i = 1 , #ClassTbl do
			tempstr = tempstr..","..ClassTbl[i]
			if 	tonumber(ClassTbl[i]) == tonumber(nClass) then
				Flag = false
			end
		end
		if Flag then
			return false,1015,tempstr.."读配置表信息:"..(self:GetItemInfo(nBigID,nIndex,"Class") or "is nil")
		else
			return true
		end
	else
		return true
	end
end

--种族是否正确
function CItemInfoMgr:BeEquipRace(nBigID,nIndex,nClass)
	local race = ClassToRace(nClass)
	local RaceTbl = loadstring("return {" .. (self:GetItemInfo(nBigID,nIndex,"Race") or "") .. "}")()
	if #RaceTbl ~= 0 then
		local Flag = true
		for i = 1, #RaceTbl do
			if RaceTbl[i] == race then
				Flag = false
			end
		end
		if Flag then
			return false, 1016
		else
			return true
		end
	else
		return true
	end
end

--性别的判断
function CItemInfoMgr:BeEquipSex(nBigID,nIndex,Sex)
	local SexTbl = loadstring("return {" .. self:GetItemInfo(nBigID,nIndex,"Sex") .. "}")()
	if #SexTbl ~= 0 then
		local Flag = true
		for i = 1, #SexTbl do
			if tonumber(SexTbl[i]) == Sex then
				Flag = false
			end
		end
		if Flag then
			return false, 1017
		else
			return true
		end
	else
		return true
	end
end

--阵营的判断
function CItemInfoMgr:BeEquipCamp(nBigID,nIndex,Camp)
	local CampTbl = loadstring("return {" .. self:GetItemInfo(nBigID,nIndex,"Camp") .. "}")()
	if #CampTbl ~= 0 then
		local Flag = true
		for i = 1, #CampTbl do
			if tonumber(CampTbl[i]) == Camp then
				Flag = false
			end
		end
		if Flag then
			return false, 1018
		else
			return true
		end
	else
		return true
	end
end

--静态武器使用
function CItemInfoMgr:WeaponCanUse(PlayerInfo,nBigID,nIndex,nLevelRequire,WeaponInfo,AssociateWeaponInfo,nEquipPart)
	if PlayerInfo.IsInBattleState then
		return false,1024
	end
	if PlayerInfo.IsInForbitUseWeaponState then
		return false,1028
	end
	if nBigID ~= self:GetStaticWeaponBigID() and nBigID ~= self:GetStaticShieldBigID() then
		return false ,1013
	end
	assert(IsNumber(nLevelRequire))
	--人物等级判断,client 直接用数据，而服务器要查询数据库
	if PlayerInfo.Level < nLevelRequire then
		return false, 1014
	end
	
	--职业是否正确
	local result,errnum = self:BeEquipClass(nBigID,nIndex,PlayerInfo.Class)
	if not result then
		return result,errnum
	end
		
	--种族是否正确
	result,errnum = self:BeEquipRace(nBigID,nIndex,PlayerInfo.Class)
	if not result then
		return result,errnum
	end

	--性别的判断
	result,errnum =  self:BeEquipSex(nBigID,nIndex,PlayerInfo.Sex)
	if not result then
		return result,errnum
	end
	
	--阵营的判断
	result,errnum = self:BeEquipCamp(nBigID,nIndex,PlayerInfo.Camp)
	if not result then
		return result,errnum
	end	

	local ruslut , HandType = self:EquipTypeCheck(self:GetItemInfo(nBigID,nIndex,"EquipType"),PlayerInfo.Class)
	if not ruslut then
		return false, 1035
	end

	local eEquipPart = self:CheckEquipPart(PlayerInfo.Class,nEquipPart,HandType,WeaponInfo,AssociateWeaponInfo)
	if eEquipPart == EEquipPart.eAssociateWeapon then
		if HandType == "主"  then
			return false, 1019
		elseif HandType == "单" then
			local Class = PlayerInfo.Class
			if Class ~= EClass.eCL_Warrior and Class ~= EClass.eCL_MagicWarrior and Class ~= EClass.eCL_OrcWarrior then
				return false,1020
			end
		end
	elseif eEquipPart == EEquipPart.eWeapon then
		if HandType == "副" then
			return false,1021
		end
	end
	return true , HandType ,eEquipPart
end

--判断武器是不是适合该职业装备 和该部位
function CItemInfoMgr:EquipTypeCheck(EquipType,Class)
	local Keys = StaticWeaponType_Common:GetKeys()
	for i,p in ipairs (Keys) do
		if tonumber(p) == Class then
			local RowInfo = StaticWeaponType_Common(p)
			tbl = {RowInfo("EquipType1"),RowInfo("EquipType2"),RowInfo("EquipType3"),RowInfo("EquipType4"),
			RowInfo("EquipType5"),RowInfo("EquipType6"),RowInfo("EquipType7"),RowInfo("EquipType8"),RowInfo("EquipType9"),RowInfo("EquipType10")}
			for i = 1, 10 do
				if tbl[i] == EquipType then
					local hand = string.sub(EquipType,1,2)
					if HandType_Common(hand) then
						return true,HandType_Common(hand,"HandType")
					end
				end

			end
		end
	end
	return false, nil
end

--静态防具使用
function CItemInfoMgr:ArmorCanUse(PlayerInfo,nBigID,nIndex,nLevelRequire)
	if PlayerInfo.IsInBattleState then
		return false,1024
	end
	if PlayerInfo.IsInForbitUseWeaponState then
		return false,1028
	end
	if not self:IsArmor(nBigID) then
		return false,1013
	end
	assert(IsNumber(nLevelRequire))
	--人物等级判断
	if PlayerInfo.Level < nLevelRequire then
		return false,1014
	end

	--职业是否正确
	local result,errnum = self:BeEquipClass(nBigID,nIndex,PlayerInfo.Class)
	if not result then
		return result,errnum
	end

	--种族是否正确
	result,errnum = self:BeEquipRace(nBigID,nIndex,PlayerInfo.Class)
	if not result then
		return result,errnum
	end 
	
	--性别的判断
	result,errnum =  self:BeEquipSex(nBigID,nIndex,PlayerInfo.Sex)
	if not result then
		return result,errnum
	end

	--阵营的判断
	result,errnum = self:BeEquipCamp(nBigID,nIndex,PlayerInfo.Camp)
	if not result then
		return result,errnum
	end	
	
	return true
end

--静态盾牌使用
function CItemInfoMgr:AssociateWeaponCanUse(PlayerInfo,nBigID,nIndex,nLevelRequire,WeaponInfo)
	if PlayerInfo.IsInBattleState then
		return false,1024
	end
	if PlayerInfo.IsInForbitUseWeaponState then
		return false,1028
	end
	if not self:IsAssociateWeapon(nBigID) then
		return false,1014
	end
	assert(IsNumber(nLevelRequire))
	--人物等级判断
	if PlayerInfo.Level < nLevelRequire then
		return false,1014
	end

	--职业是否正确
	local result,errnum = self:BeEquipClass(nBigID,nIndex,PlayerInfo.Class)
	if not result then
		return result,errnum
	end

	--种族是否正确
	result,errnum = self:BeEquipRace(nBigID,nIndex,PlayerInfo.Class)
	if not result then
		return result,errnum
	end

	--性别的判断
	result,errnum =  self:BeEquipSex(nBigID,nIndex,PlayerInfo.Sex)
	if not result then
		return result,errnum
	end

	--阵营的判断
	result,errnum = self:BeEquipCamp(nBigID,nIndex,PlayerInfo.Camp)
	if not result then
		return result,errnum
	end	

	local ruslut , HandType = self:EquipTypeCheck(self:GetItemInfo(nBigID,nIndex,"EquipType"),PlayerInfo.Class)
	if not ruslut then
		return false,1035
	end
	return true
end

--静态饰品
function CItemInfoMgr:JewelryCanUse(PlayerInfo,nBigID,nIndex,nLevelRequire)
	if PlayerInfo.IsInBattleState then
		return false,1024
	end
	if PlayerInfo.IsInForbitUseWeaponState then
		return false,1028
	end
	
	if not (self:IsJewelry(nBigID) or self:IsRing(nBigID)) then
		return false,1023
	end
	assert(IsNumber(nLevelRequire),"出错饰品名称:" .. nIndex )
	--人物等级判断
	if PlayerInfo.Level < nLevelRequire then
		return false,1014
	end

	--职业是否正确
	local result,errnum = self:BeEquipClass(nBigID,nIndex,PlayerInfo.Class)
	if not result then
		return result,errnum
	end

	--种族是否正确
	result,errnum = self:BeEquipRace(nBigID,nIndex,PlayerInfo.Class)
	if not result then
		return result,errnum
	end

	--性别的判断
	result,errnum =  self:BeEquipSex(nBigID,nIndex,PlayerInfo.Sex)
	if not result then
		return result,errnum
	end

	--阵营的判断
	result,errnum = self:BeEquipCamp(nBigID,nIndex,PlayerInfo.Camp)
	if not result then
		return result,errnum
	end

	return true
end

--判断是否为法系的装备
function CItemInfoMgr:IsFaEquip(nBigID,nIndex)
	if not self:IsEquip(nBigID) then
		return false
	end
	local Class = self:GetItemInfo(nBigID,nIndex,"Class")
	if Class == nil then
		return false
	end
	if  Class == EClass.eCL_Priest or Class == EClass.eCL_DwarfPaladin or
		Class == EClass.eCL_ElfHunter then
		return true
	end
	return false
end

--武器判断应该返回的装备的部位
function CItemInfoMgr:CheckEquipPart(Class,nEquipPart,HandType,WeaponInfo,AssociateWeaponInfo)
	if nEquipPart == EEquipPart.eWeapon then
		eEquipPart = EEquipPart.eWeapon
	elseif nEquipPart == EEquipPart.eAssociateWeapon then
		if HandType == "双" then
			eEquipPart = EEquipPart.eWeapon
		else
			eEquipPart = EEquipPart.eAssociateWeapon
		end
	else
		--点击别的地方
		if HandType == "副" then
			eEquipPart = EEquipPart.eAssociateWeapon
		elseif HandType == "主" then
			eEquipPart = EEquipPart.eWeapon
		elseif WeaponInfo ~= nil and AssociateWeaponInfo == nil then
			local EquipType = g_ItemInfoMgr:GetItemInfo(WeaponInfo[1],WeaponInfo[2],"EquipType")
			local hand = string.sub(EquipType,1,2)
			if hand == "双" then
				eEquipPart = EEquipPart.eWeapon
			else
				if HandType == "双" then
					eEquipPart = EEquipPart.eWeapon
				elseif Class ~= EClass.eCL_Warrior and Class ~= EClass.eCL_MagicWarrior and Class ~= EClass.eCL_OrcWarrior then
					eEquipPart = EEquipPart.eWeapon
				else
					eEquipPart = EEquipPart.eAssociateWeapon
				end
			end
		else
			eEquipPart = EEquipPart.eWeapon
		end
	end
	return eEquipPart
end

function CItemInfoMgr:GetMailItemName()
	return "Mail_Appendix"
end

function CItemInfoMgr:GetCommonlySkillByID(Class,nBigID,nIndex)
	return self:GetCommonlySkill(Class,nBigID,nIndex)
end

--
function CItemInfoMgr:GetCommonlySkill(Class,nBigID,nIndex)
	local EquipType = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"EquipType")
	if Class == EClass.eCL_Warrior then
		if EquipType=="单手剑" or EquipType=="单手斧" or EquipType=="主手剑" or EquipType=="主手斧" then--单手武器
			return "大剑士单手武器普通攻击"
		elseif EquipType=="双手剑" or EquipType=="双手斧" then-- 双手武器
			return "大剑士双手武器普通攻击"
		end
	elseif Class == EClass.eCL_MagicWarrior then
		if EquipType=="单手剑" or EquipType=="单手锤" or EquipType=="主手剑" or EquipType=="主手锤"then
			return "魔剑士单手武器普通攻击"
		elseif EquipType=="双手剑" or EquipType=="双手锤" then
			return "魔剑士双手武器普通攻击"
		end
	elseif Class == EClass.eCL_Paladin then
		if EquipType=="骑士枪" or EquipType=="主手剑" then
			return "人类骑士单手武器普通攻击"
		elseif EquipType=="长柄棒" or EquipType=="长柄矛" or EquipType=="长柄刀" then
			return "人类骑士长柄武器普通攻击"
		elseif EquipType =="弩" then
			return "人类骑士弩武器普通攻击"
		elseif EquipType == "双手弓" then
			return "人类骑士弓武器普通攻击"
		end
	elseif Class == EClass.eCL_NatureElf then
		if EquipType=="主手自然杖" then
			return "法师自然系单手杖武器普通攻击"
		elseif EquipType=="双手自然杖" then
			return "法师自然系双手杖武器普通攻击"
		elseif EquipType=="主手暗黑杖" then
			return "法师暗黑系单手杖武器普通攻击"
		elseif EquipType=="双手暗黑杖" then
			return "法师暗黑系双手杖武器普通攻击"
		elseif EquipType=="主手破坏杖" then
			return "法师破坏系单手杖武器普通攻击"
		elseif EquipType=="双手破坏杖" then
			return "法师破坏系双手杖武器普通攻击"
		end
	elseif Class == EClass.eCL_EvilElf then
		if EquipType=="主手自然杖" then
			return "邪魔自然系单手杖武器普通攻击"
		elseif EquipType=="双手自然杖" then
			return "邪魔自然系双手杖武器普通攻击"
		elseif EquipType=="主手暗黑杖" then
			return "邪魔暗黑系单手杖武器普通攻击"
		elseif EquipType=="双手暗黑杖" then
			return "邪魔暗黑系双手杖武器普通攻击"
		elseif EquipType=="主手破坏杖" then
			return "邪魔破坏系单手杖武器普通攻击"
		elseif EquipType=="双手破坏杖" then
			return "邪魔破坏系双手杖武器普通攻击"
		end
	elseif Class == EClass.eCL_Priest then
		if EquipType=="主手自然杖" then
			return "牧师自然系单手杖武器普通攻击"
		elseif EquipType=="双手自然杖" then
			return "牧师自然系单手杖武器普通攻击"
		elseif EquipType=="主手暗黑杖" then
			return "牧师暗黑系单手杖武器普通攻击"
		elseif EquipType=="双手暗黑杖" then
			return "牧师暗黑系双手杖武器普通攻击"
		elseif EquipType=="主手破坏杖" then
			return "牧师破坏系单手杖武器普通攻击"
		elseif EquipType=="双手破坏杖" then
			return "牧师破坏系双手杖武器普通攻击"
		end
	elseif Class == EClass.eCL_DwarfPaladin then
		if EquipType=="骑士枪" or EquipType=="主手剑" then
			return "矮人骑士单手武器普通攻击"
		elseif EquipType=="长柄棒" or EquipType=="长柄矛" or EquipType=="长柄刀"  then
			return "矮人骑士长柄武器普通攻击"
		end
	elseif Class == EClass.eCL_ElfHunter then
		if EquipType=="双手弓" then
			return "精灵弓箭手弓武器普通攻击"
		elseif EquipType=="双手弩" then
			return "精灵弓箭手弩武器普通攻击"
		end
	elseif Class == EClass.eCL_OrcWarrior then
		if EquipType== "单手斧" or EquipType== "单手锤" or EquipType=="单手刀"
				or EquipType=="主手斧" or EquipType=="主手锤"  then
			return "兽人战士单手武器普通攻击"
		elseif EquipType=="双手锤" or EquipType=="双手斧" then
			return "兽人战士双手武器普通攻击"
		end
	else
		print("该武器类型" .. EquipType .. "StaticWeaponType_Common配置表中不存在")
	end
end

--参数是名称加经验值或者魂值，例如：通用魂珠:10000
function CItemInfoMgr:GetSoulpearlInfo(sIndex)
	local soulpearlName = sIndex
	local soulpearlCount = 0
		
	local temp = string.find(sIndex,":") or string.find(sIndex,"：")
	if temp then
		soulpearlCount = tonumber( string.sub(sIndex, temp + 1) )
		if soulpearlCount and soulpearlCount > 0 then
			soulpearlName = string.sub(sIndex, 1, temp - 1)
		end
	end
	return soulpearlName, soulpearlCount
end

function CItemInfoMgr:GetSoulUseLimitInfo(nType)
	return SoulPearlLimitType_Common(nType)
end

--判断任务物品是否是同一阵营的
function CItemInfoMgr:PlayerQuestItem(PlayerInfo,nBigID,nIndex)	
	--职业是否正确
	local result,errnum = self:BeEquipClass(nBigID,nIndex,PlayerInfo.Class)
	if not result then
		return result,errnum
	end	
	
	--种族是否正确
	result,errnum = self:BeEquipRace(nBigID,nIndex,PlayerInfo.Class)
	if not result then
		return result,errnum
	end

	--性别的判断
	result,errnum =  self:BeEquipSex(nBigID,nIndex,PlayerInfo.Sex)
	if not result then
		return result,errnum
	end

	--阵营的判断
	result,errnum = self:BeEquipCamp(nBigID,nIndex,PlayerInfo.Camp)
	if not result then
		return result,errnum
	end
	
	return true
end

