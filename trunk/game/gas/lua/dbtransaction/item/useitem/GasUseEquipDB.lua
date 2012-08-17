gac_gas_require "item/Equip/EquipDefInc"
gac_gas_require "item/item_info_mgr/ItemInfoMgr"
gac_gas_require "player/PlayerMgr"
gac_gas_require "item/Equip/EquipCommonFunc"

local g_ItemInfoMgr = CItemInfoMgr:new()
local EEquipPart = EEquipPart
local EquipPartNameToNum = EquipPartNameToNum
local AdvanceEquipDB = RequireDbBox("EquipAdvanceDB")
local EquipDurabilityDB = RequireDbBox("EquipDurabilityDB")
local g_GetPlayerClassNameByID = g_GetPlayerClassNameByID
local CheckSuitCount = CheckSuitCount
local CheckWeaponEquipType = CheckWeaponEquipType
local event_type_tbl = event_type_tbl

local UseItemSql = CreateDbBox(...)

local SqlTbl = {}

--检测使用物品
function UseItemSql.UseItemVerify(PlayerId,uPlayerLevel,nRoomIndex,nPos)
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	if not g_RoomMgr.CheckRoomAndPos(PlayerId,nRoomIndex,nPos) then
		return false, 1011
	end
	
	--需要判断物品是否正确,这里的nItemID只是第一个物品，所以任何使用的物品都会是第一个
	local nBigID,nIndex,nItemID = g_RoomMgr.GetOneItemByPosition(PlayerId,nRoomIndex,nPos)
	if nBigID == nil then
		return false, 1011
	end
	--判断是否过期物品
	local ItemLifeMgrDB = RequireDbBox("ItemLifeMgrDB")
	if not ItemLifeMgrDB.IsTimeOutItem(nItemID) then
		return false,13
	end
	
	--非装备要检查，装备不需要检查
	if not g_ItemInfoMgr:IsEquip(nBigID) then
		--等级判断
		local use_level = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"BaseLevel") or 1
		if uPlayerLevel < use_level then
			return false,801
		end		
		
		--需要根据PlayerId从数据库查询结果
		if not g_RoomMgr.IsInPlayerBag(PlayerId,nRoomIndex) then
			return false, 1011
		end
	end
	return tonumber(nBigID),nIndex,nItemID
end

local StmtDef = {
    "_GetEquipSuitName",
    [[
        select iei_sSuitName from tbl_item_equip_intensify where is_uId = ?
    ]]
}
DefineSql(StmtDef, SqlTbl)

function UseItemSql.GetEquipSuitName(equipId)
    local suitName = ""
    local suitNameRet = SqlTbl._GetEquipSuitName:ExecStat(equipId)
    if suitNameRet:GetRowNum() > 0 then
        suitName = suitNameRet:GetDataByName(0, "iei_sSuitName")
    end
    return suitName
end

--得到套装传给高速的数据
function UseItemSql.GetSuitAttributeBySuitName(PlayerId, SuitName)
	local EquipIntensifyDB = RequireDbBox("EquipIntensifyDB")
	if SuitName ~= nil and SuitName ~= "" then
		local suc, suitAttrTbl, includingPartsCount = EquipIntensifyDB.GetSuitAttributeBySuitName(PlayerId, SuitName)	
		if suc then
			local EquipSuitTbl = {suitAttrTbl, includingPartsCount, SuitName}
			return EquipSuitTbl
		end
		return
	end
end

--使用装备sql函数
function UseItemSql.UseEquip(nBigID,nIndex,nItemID,data)
	local EquipInfo_New = {nBigID,nIndex,nItemID}
	local PlayerInfo  = data["PlayerInfo"]
	local PlayerId = data["PlayerInfo"].Id
	local nEquipPart = data["EquipPart"]
	local nPos = data["Pos"]
	local nRoomIndex = data["RoomIndex"]
	local sceneName = data["sceneName"]
	local EquipMgrDB = RequireDbBox("EquipMgrDB")
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local EquipIntensifyDB				=	RequireDbBox("EquipIntensifyDB")
	local WeaponInfo = EquipMgrDB.GetEquipInfo(PlayerId,EEquipPart.eWeapon)
	local AssociateWeaponInfo = EquipMgrDB.GetEquipInfo(PlayerId,EEquipPart.eAssociateWeapon)
	local AssociateWeaponRet
    local SuitName = UseItemSql.GetEquipSuitName(nItemID)
    local equipSuit_NewPre = UseItemSql.GetSuitAttributeBySuitName(PlayerId, SuitName)
	if AssociateWeaponInfo ~= nil then
		local TempRet = EquipMgrDB.GetEquipAllInfo( AssociateWeaponInfo[3] )
		AssociateWeaponRet = TempRet	
	end
	--检查是不是饰品或戒指
	if not g_ItemInfoMgr:IsEquipNotJewelry(nBigID)then
		return false,1012
	end
	--需要查询数据库得到装备的等级
	local nLevel,HandType,eEquipPart,EquipRet_New, equipRet_NewRet
	if g_ItemInfoMgr:IsStaticEquip(nBigID) then
		local TempRet = EquipMgrDB.GetEquipAllInfo( nItemID )
		EquipRet_New = TempRet
		equipRet_NewRet = TempRet["BaseInfo"]:GetRowSet(1)
		nLevel = equipRet_NewRet(3)
	else
		return false,1011
	end

	local equipCanUseLevel = nLevel
	local EquipPart = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"EquipPart")
	if g_ItemInfoMgr:IsWeapon(nBigID) then
		local result , handtype ,TempPart = g_ItemInfoMgr:WeaponCanUse(PlayerInfo,nBigID,nIndex,equipCanUseLevel,WeaponInfo,AssociateWeaponInfo,nEquipPart)
		HandType = handtype
		eEquipPart = TempPart
		if not result then
			return false,handtype
		end
	elseif g_ItemInfoMgr:IsArmor(nBigID) then
		local result,errnum = 	g_ItemInfoMgr:ArmorCanUse(PlayerInfo,nBigID,nIndex,equipCanUseLevel)
		if not result then
			return false,errnum
		end
		eEquipPart = EquipPartNameToNum[EquipPart]
	elseif g_ItemInfoMgr:IsAssociateWeapon(nBigID) then
		local result,errnum = g_ItemInfoMgr:AssociateWeaponCanUse(PlayerInfo,nBigID,nIndex,equipCanUseLevel,WeaponInfo)
		if not result then
			return false,errnum
		end
		eEquipPart = EEquipPart.eAssociateWeapon
	end
	if eEquipPart == EEquipPart.eAssociateWeapon and AssociateWeaponInfo ~= nil then
		AssociateWeaponInfo = nil 
	end 
	
	local EquipInfo = EquipMgrDB.GetEquipInfo(PlayerId,eEquipPart)
	local EquipRet_Old,EquipSuitTbl_Old
	local RDoubleHandInfo = nil
	local suitNameOfEquipReplaced = nil  --主手或副手上被替换的武器所含的套装名称；如果是主副手都有武器，且被双手武器替换，则这个记录的是主手上被替换的武器套装名称
    local suitNameOfEquip
	if EquipInfo ~= nil then
		local TempRet = EquipMgrDB.GetEquipAllInfo( EquipInfo[3] )		
		EquipRet_Old = 	TempRet	
		local SuitName = UseItemSql.GetEquipSuitName(EquipInfo[3])
		suitNameOfEquipReplaced = SuitName
		EquipSuitTbl_Old = UseItemSql.GetSuitAttributeBySuitName(PlayerId, SuitName)
	elseif  eEquipPart == EEquipPart.eAssociateWeapon then
		--如果装备的部位是副手，同时主手武器为双手武器
		RDoubleHandInfo = EquipMgrDB.GetEquipInfo(PlayerId,EEquipPart.eWeapon)
		if RDoubleHandInfo ~= nil then
			local _, RDoubleHandType = g_ItemInfoMgr:EquipTypeCheck(g_ItemInfoMgr:GetItemInfo(RDoubleHandInfo[1],RDoubleHandInfo[2],"EquipType"),PlayerInfo.Class)
			if RDoubleHandType == "双" then
				local TempRet = EquipMgrDB.GetEquipAllInfo( RDoubleHandInfo[3] )		
				EquipRet_Old = 	TempRet	
				local SuitName = UseItemSql.GetEquipSuitName(RDoubleHandInfo[3])
				suitNameOfEquipReplaced = SuitName
				EquipSuitTbl_Old = UseItemSql.GetSuitAttributeBySuitName(PlayerId, SuitName)		
			else
				RDoubleHandInfo = nil
			end
		end
	end
	
	local old_CommonlySkillName,new_CommonlySkillName
	local CommonlySkillName = g_GetPlayerClassNameByID(PlayerInfo.Class).. "普通攻击"
	if EquipInfo ~= nil then
		if eEquipPart == EEquipPart.eWeapon then
			if EquipRet_Old["DuraState"] == 0 then
				old_CommonlySkillName = CommonlySkillName
			else
				old_CommonlySkillName = g_ItemInfoMgr:GetCommonlySkillByID(PlayerInfo.Class,EquipInfo[1],EquipInfo[2])
			end
			if EquipRet_New["DuraState"] == 0 then
				new_CommonlySkillName = CommonlySkillName
			else				
				new_CommonlySkillName = g_ItemInfoMgr:GetCommonlySkillByID(PlayerInfo.Class,nBigID,nIndex)
			end
		end
	else
		if eEquipPart == EEquipPart.eWeapon then
			old_CommonlySkillName = CommonlySkillName
			if EquipRet_New["DuraState"] == 0 then
				new_CommonlySkillName = CommonlySkillName
			else
				new_CommonlySkillName = g_ItemInfoMgr:GetCommonlySkillByID(PlayerInfo.Class,nBigID,nIndex)
			end
		elseif eEquipPart == EEquipPart.eAssociateWeapon and RDoubleHandInfo then
			local TempRet = EquipMgrDB.GetEquipAllInfo( RDoubleHandInfo[3] )
			if TempRet["DuraState"] == 0 then
				old_CommonlySkillName = CommonlySkillName
			else
				old_CommonlySkillName = g_ItemInfoMgr:GetCommonlySkillByID(PlayerInfo.Class,RDoubleHandInfo[1],RDoubleHandInfo[2])
			end
			new_CommonlySkillName = CommonlySkillName
		end
	end
	local EquipSuitTbl_Associate
	if HandType == "双" then
		if AssociateWeaponInfo ~= nil then
			local SuitName = UseItemSql.GetEquipSuitName(AssociateWeaponInfo[3])
			suitNameOfEquip = SuitName
			EquipSuitTbl_Associate = UseItemSql.GetSuitAttributeBySuitName(PlayerId, SuitName)
			--如果主副手上的装备成套装，这两个装备被同时替换掉，这个属于第二个被替换的，那么查询出来的含有的套装件数应该减去第一件的
			if suitNameOfEquipReplaced ~= "" and 
			    suitNameOfEquipReplaced ~= nil and  
			    suitNameOfEquipReplaced == SuitName then 
			    EquipSuitTbl_Associate[2] = EquipSuitTbl_Associate[2] - 1
		    end
		end
    end
	
	----------------------数据库操作-------------------
	
	--替换该部位上的装备
	local ReplaceResult,uEventId = EquipMgrDB.ReplaceEquip(PlayerId,nItemID,eEquipPart)
	if not ReplaceResult then
		CancelTran()
		return false,802
	end
	if RDoubleHandInfo then
		local DeleteResult = EquipMgrDB.DeleteEquip(PlayerId,EEquipPart.eWeapon)
		if not DeleteResult then
			CancelTran()
			return false,802
		end
	end

	if HandType == "双" then
		if AssociateWeaponInfo ~= nil then
		    local DeleteResult = EquipMgrDB.DeleteEquip(PlayerId,EEquipPart.eAssociateWeapon)
			if not DeleteResult then
				CancelTran()
				return false,802
			end
		end
	end
	
	
	local BindingChange
	if tonumber(equipRet_NewRet(2)) == 1 then
		BindingChange = true
		g_RoomMgr.UpdateItemBindingType(2, {nItemID},PlayerId)
		if g_DbChannelMgr:LastAffectedRowNum() > 0 then
		    --EquipRet_New["BaseInfo"](2) = 2    --将装备是否绑定信息设置为绑定
		    EquipRet_New = EquipMgrDB.GetEquipAllInfo( nItemID )
		else
		    CancelTran()
		    return false,802
		end
	end
	
	--装备套装相关

	local EquipSuitTbl_New = UseItemSql.GetSuitAttributeBySuitName(PlayerId, SuitName)	
    if equipSuit_NewPre  and EquipSuitTbl_New and EquipSuitTbl_New[2] == equipSuit_NewPre[2] then
        EquipSuitTbl_New = nil
    end
    
    if suitNameOfEquipReplaced then
         local EquipSuitTbl_OldPre = UseItemSql.GetSuitAttributeBySuitName(PlayerId, suitNameOfEquipReplaced)
         if EquipSuitTbl_OldPre and EquipSuitTbl_Old and EquipSuitTbl_OldPre[2] == EquipSuitTbl_Old[2] then
            EquipSuitTbl_Old = nil
         end
    end
    
    if suitNameOfEquip then
        local EquipSuitTbl_AssociatePre = UseItemSql.GetSuitAttributeBySuitName(PlayerId, suitNameOfEquip)
        if EquipSuitTbl_Associate and EquipSuitTbl_AssociatePre and EquipSuitTbl_AssociatePre[2] == EquipSuitTbl_Associate[2] then
            EquipSuitTbl_Associate = nil
        end          
    end
	--在包裹中删除装备的物品
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local DelItemResult = g_RoomMgr.MoveItemFromPackageByPos(PlayerId,nRoomIndex,nPos,1)
	if IsNumber(DelItemResult) then
		CancelTran()
		return false,802		
	end
	--存储该部位原有的装备
	if EquipInfo ~= nil then
		local RenewItemResult = g_RoomMgr.RenewItemRoomPos(PlayerId,EquipInfo[3],nRoomIndex,nPos)
		if IsNumber(RenewItemResult) then
			CancelTran()
			return false,802		
		end		
	elseif RDoubleHandInfo ~= nil then
		local RenewItemResult = g_RoomMgr.RenewItemRoomPos(PlayerId,RDoubleHandInfo[3],nRoomIndex,nPos)
		if IsNumber(RenewItemResult) then
			CancelTran()
			return false,802		
		end			
	end
	
	--双手装备时在包裹中存储副手上的装备
	local RetRoomInfo
	if HandType == "双" then
		if AssociateWeaponInfo ~= nil and EquipInfo ~= nil then
			local item_bag_box = RequireDbBox("CPutItemToBagDB")
			local tbl_item_info = {}
			table.insert(tbl_item_info, {AssociateWeaponInfo[3]})
			
			local params = {}
			params.m_nCharID = PlayerId
			params.m_nItemType = AssociateWeaponInfo[1]
			params.m_sItemName = AssociateWeaponInfo[2]
			params.m_tblItemID = tbl_item_info
			local bFlag, ret_info = item_bag_box.AddCountItem(params)
			if not bFlag then
				CancelTran()
				return false,160000
			end
			RetRoomInfo = ret_info[1]
		elseif AssociateWeaponInfo ~= nil then
			local RenewItemResult = g_RoomMgr.RenewItemRoomPos(PlayerId,AssociateWeaponInfo[3],nRoomIndex,nPos)
			if IsNumber(RenewItemResult) then
				CancelTran()
				return false,802		
			end			
		end
	end	
	local ShortCutRet
	if (eEquipPart == EEquipPart.eWeapon) 
		or (eEquipPart == EEquipPart.eAssociateWeapon and RDoubleHandInfo ) then 
		local ShortCutDB = RequireDbBox("ShortcutDB")
		ShortCutDB.UpDateShortcut(PlayerId,new_CommonlySkillName,old_CommonlySkillName)
		ShortCutRet = ShortCutDB.GetShortcutInfo(PlayerId)
	end	
	
	local suc, skillIndex = AdvanceEquipDB.GetRemoveJingLingSkillIndex(PlayerId)
    if suc == false and skillIndex == -1 then
        CancelTran()
		return false,802
    end
	
	local haveDemagedEquip = EquipDurabilityDB.CheckHaveDemagedEquip(PlayerId)
	
	--低速加佣兵等级指引计数
 	--local MercenaryLevelDB = RequireDbBox("MercenaryLevelDB")
 	--local MlRes = nil
 	--if SuitName ~= nil and SuitName ~= "" then
	-- 	local res = CheckSuitCount(RItemInfo.SoulRoot, SuitName)
	--	if res == 2 or res == 6 then
	--		MlRes = MercenaryLevelDB.SlowAddMercenaryLevelCount(PlayerId, "紫装")
	--	end
	--end
	local tbl = 
	{
		["OldEquipInfo"] = EquipInfo,
		["RDoubleHandInfo"] = RDoubleHandInfo, -- 存储主手上双手武器的信息（副手武器换双手武器时）
		["OldEquipRet"] = EquipRet_Old, 
		["NewEquipRet"] = EquipRet_New,
		["NewEquipInfo"] = EquipInfo_New,
		["HandType"] = HandType,
		["AssociateWeaponInfo"] = AssociateWeaponInfo,
		["AssociateWeaponRet"]  = AssociateWeaponRet,
		["EquipPart"] = eEquipPart,
		["new_CommonlySkillName"] = new_CommonlySkillName,
		["RetRoomInfo"] = RetRoomInfo,
		["BindingChange"] = BindingChange,
		["ShortCutRet"] = ShortCutRet,
		["EquipSuitTbl_New"] = EquipSuitTbl_New,
		["EquipSuitTbl_Old"] = EquipSuitTbl_Old,
		["EquipSuitTbl_Associate"] = EquipSuitTbl_Associate,
		["RemoveJingLingSkillIndex"] = skillIndex,
		["HaveDemagedEquip"] = haveDemagedEquip,
		["uEventId"] = uEventId,
		--["MlRes"] = MlRes,
 	}

	return true,tbl
end

--使用饰品sql函数
function UseItemSql.UseJewelry(nBigID,nIndex,nItemID,data)
	local EquipInfo_New = {nBigID,nIndex,nItemID}
	local PlayerInfo  = data["PlayerInfo"]
	local PlayerId = data["PlayerInfo"].Id
	local nEquipPart = data["EquipPart"]
	local nPos = data["Pos"]
	local nRoomIndex = data["RoomIndex"]
	local sceneName = data["sceneName"]
	
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local EquipIntensifyDB				=	RequireDbBox("EquipIntensifyDB")
	if not g_RoomMgr.CheckRoomAndPos(PlayerId,nRoomIndex,nPos) then
		return false,-1
	end
	local SuitName = UseItemSql.GetEquipSuitName(nItemID)
    local EquipSuitTbl = UseItemSql.GetSuitAttributeBySuitName(PlayerId, SuitName)
    
	local EquipRet_New
	local EquipMgrDB = RequireDbBox("EquipMgrDB")
	local TempRet = EquipMgrDB.GetEquipAllInfo( nItemID )
	EquipRet_New = TempRet
	local equipRet_NewRet = TempRet["BaseInfo"]:GetRowSet(1)
	local nLevel = equipRet_NewRet(3)
		
	local jewelryUseLevel = nLevel
	local EquipPart = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"EquipPart")	
	--判断是否为饰品,饰品是否可用
	if not g_ItemInfoMgr:JewelryCanUse(PlayerInfo,nBigID,nIndex,jewelryUseLevel) then
		return false,1013
	end
	local eEquipPart
	if 	g_ItemInfoMgr:IsRing(nBigID) then
		if nEquipPart == EEquipPart.eRingLeft or nEquipPart == EEquipPart.eRingRight then
			eEquipPart = nEquipPart
		else
			local RingLeftInfo = EquipMgrDB.GetEquipInfo(PlayerId,EEquipPart.eRingLeft)
			if RingLeftInfo ~=nil then
				eEquipPart = EEquipPart.eRingRight
			else
				eEquipPart = EEquipPart.eRingLeft
			end		
		end
	else
		local equipPartNum = EquipPartNameToNum[EquipPart]
		eEquipPart = equipPartNum
	end
	-----------------------------------低速逻辑----------------------------
	local EquipInfo = EquipMgrDB.GetEquipInfo(PlayerId,eEquipPart)
	--删除旧的属性
	local EquipRet_Old,EquipSuitTbl_Old,EquipSuitTbl_OldAfter, suitNamePre
	if EquipInfo ~= nil then
	    suitNamePre = UseItemSql.GetEquipSuitName(EquipInfo[3])	
		local TempRet = EquipMgrDB.GetEquipAllInfo( EquipInfo[3] )
		EquipRet_Old = TempRet
		EquipSuitTbl_Old = UseItemSql.GetSuitAttributeBySuitName(PlayerId, suitNamePre)	
	end
	--替换新的饰品
	local bool,uEventId = EquipMgrDB.ReplaceEquip(PlayerId,nItemID,eEquipPart)
	if not bool then
		CancelTran()
		return false,802
	end
	
	local BindingChange
	if tonumber(equipRet_NewRet(2)) == 1 then
		BindingChange = true
		g_RoomMgr.UpdateItemBindingType(2, {nItemID},PlayerId)
		if g_DbChannelMgr:LastAffectedRowNum() > 0 then
		    --EquipRet_New["BaseInfo"](2) = 2    --将装备是否绑定信息设置为绑定
		    EquipRet_New = EquipMgrDB.GetEquipAllInfo( nItemID )
		else
		    CancelTran()
		    return false,802
		end
	end	
	
	local EquipSuitTbl_New = UseItemSql.GetSuitAttributeBySuitName(PlayerId, SuitName)
	if EquipSuitTbl and EquipSuitTbl_New and EquipSuitTbl[2] == EquipSuitTbl_New[2] then
	    EquipSuitTbl_New = nil
	end
	if suitNamePre ~= nil then
	   EquipSuitTbl_OldAfter =  UseItemSql.GetSuitAttributeBySuitName(PlayerId, suitNamePre)
	end
	if EquipSuitTbl_OldAfter and EquipSuitTbl_Old  and EquipSuitTbl_OldAfter[2] == EquipSuitTbl_Old[2] then
	    EquipSuitTbl_Old = nil
	end
		
	--删除包裹里的物品
	local DelItemResult = g_RoomMgr.MoveItemFromPackageByPos(PlayerId,nRoomIndex,nPos,1)
	if IsNumber(DelItemResult) then
		CancelTran()
		return false,802		
	end
	if EquipInfo ~= nil then
		local RenewItemResult = g_RoomMgr.RenewItemRoomPos(PlayerId,EquipInfo[3],nRoomIndex,nPos)
		if IsNumber(RenewItemResult) then
			CancelTran()
			return false,802		
		end		
	end
	local suc, skillIndex = AdvanceEquipDB.GetRemoveJingLingSkillIndex(PlayerId)
    if suc == false and skillIndex == -1 then
        CancelTran()
		return false,802
    end	
	
	local haveDemagedEquip = EquipDurabilityDB.CheckHaveDemagedEquip(PlayerId)
	
	--低速加佣兵等级指引计数
	--local MercenaryLevelDB = RequireDbBox("MercenaryLevelDB")
 	--local MlRes = nil
 	--if SuitName ~= nil and SuitName ~= "" then
	-- 	local res = CheckSuitCount(RItemInfo.SoulRoot, SuitName)
	--	if res == 2 or res == 6 then
	--		MlRes = MercenaryLevelDB.SlowAddMercenaryLevelCount(PlayerId, "紫装")
	--	end
	--end
	local tbl = 
	{
		["OldEquipInfo"] = EquipInfo,
		["OldEquipRet"] = EquipRet_Old,
		["NewEquipRet"] = EquipRet_New,
		["NewEquipInfo"] = EquipInfo_New,
		["EquipPart"] = eEquipPart,
		["BindingChange"] = BindingChange,
		["EquipSuitTbl_New"] = EquipSuitTbl_New,
		["EquipSuitTbl_Old"] = EquipSuitTbl_Old,
		["RemoveJingLingSkillIndex"] = skillIndex,
		["HaveDemagedEquip"] = haveDemagedEquip,
		["uEventId"] = uEventId
		--["MlRes"] = MlRes,
 	}	

	return true,tbl
end

--
function UseItemSql.UseDrug(nItemBigID,nItemIndex,nItemID,data)
	local Info = {nItemBigID,nItemIndex,nItemID}
	local tbl = 
	{
		["NewEquipInfo"] = Info,
		["EquipPart"] = 0,
	}
	return true,tbl
end

function UseItemSql.UseWhiteStone(nItemBigID,nItemIndex,nItemID,data)
	local lStoneSqr = RequireDbBox("StoneMgrDB")
	local PlayerId  = data["PlayerInfo"].Id
  	local nRoomIndex  = data["RoomIndex"]
  	local nPos  = data["Pos"]
  	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")	
	local nHaveCount = g_RoomMgr.GetCountByPosition(PlayerId, nRoomIndex, nPos)
	
	local parameter = {}
	parameter["player_id"] = PlayerId
	parameter["item_id"] = nItemID
	parameter["have_count"] = nHaveCount
	parameter["sceneName"] = data["sceneName"]
	local ret = lStoneSqr.WhiteStoneAppraiseFirstTime(parameter)
	local Info = {nItemBigID,nItemIndex,nItemID}
	local tbl = 
	{
		["NewEquipInfo"] = Info,
		["EquipPart"] = 0,
		["HighStoneResult"] = ret,
	}
	return true,tbl
end

local ExecuteFunc =
{
	[1] = nil,
	[2] = nil,
	[3] = UseItemSql.UseDrug,				-- 使用药品
	[5] = UseItemSql.UseEquip,				-- 装备，静态
	[6] = UseItemSql.UseEquip,
	[7] = UseItemSql.UseEquip,
	[8] = UseItemSql.UseJewelry,			-- 戒指
	[9] = UseItemSql.UseJewelry,			-- 饰品
	[10] = UseItemSql.UseEquip,
	[11] = UseItemSql.UseEquip,
	[12] = UseItemSql.UseJewelry,
	[18] = UseItemSql.UseWhiteStone,       
}

--使用物品接口
function UseItemSql.EntryFunc(data)
	local PlayerId  = data["PlayerInfo"].Id
	local nRoomIndex  = data["RoomIndex"]
	local nPos  = data["Pos"]
	local uPlayerLevel = data["PlayerInfo"].Level
	local nBigIDClient,nIndexClient = data["nBigID"], data["Index"]
  	
  local ItemBagLockDB = RequireDbBox("ItemBagLockDB")
  if ItemBagLockDB.HaveItemBagLock(PlayerId) then
  	return false,160009
  end
 	local nItemBigID,nItemIndex,nItemID = UseItemSql.UseItemVerify(PlayerId,uPlayerLevel,nRoomIndex,nPos)
	
	if not nItemBigID then
		return false,nItemIndex
	end
	
	if nBigIDClient ~= nItemBigID or nIndexClient ~= nItemIndex then
		return false,802
	end
	
	if nItemBigID~= -1 and nItemBigID ~= 0 then
		if ExecuteFunc[nItemBigID] ~= nil then
			local bool,tbl = ExecuteFunc[nItemBigID](nItemBigID,nItemIndex,nItemID,data)	--,MLRes
			return bool,tbl	--,MLRes
		end
	end
end

--删除装备sql函数
function UseItemSql.DelEquip(data)
	local eEquipPart = data["EquipPart"]
	local PlayerId	 = data["PlayerId"]	
	local Class = data["Class"]
	local sceneName = data["sceneName"]

	local EquipMgrDB = RequireDbBox("EquipMgrDB")
	local EquipIntensifyDB				=	RequireDbBox("EquipIntensifyDB")
	local EquipInfo = EquipMgrDB.GetEquipInfo(PlayerId,eEquipPart)
	if EquipInfo == nil then
		return false,1011
	end

	local EquipRet_Old,EquipSuitTbl_Old,EquipSuitTbl_OldPre, SuitName
	if EquipInfo ~= nil then
		local TempRet = EquipMgrDB.GetEquipAllInfo( EquipInfo[3] )
		EquipRet_Old = TempRet
		SuitName = UseItemSql.GetEquipSuitName(EquipInfo[3])
		EquipSuitTbl_Old = UseItemSql.GetSuitAttributeBySuitName(PlayerId, SuitName)
	end
	
	local lRoomMgr = RequireDbBox("GasRoomMgrDB")
	lRoomMgr.DelItemFromStaticTable(EquipInfo[3],PlayerId,event_type_tbl["左键点击删除"])
	local new_CommonlySkillName = 0
	local ShortCutRet
	if eEquipPart == EEquipPart.eWeapon  then 
		new_CommonlySkillName = g_GetPlayerClassNameByID(Class).. "普通攻击"
		local old_CommonlySkillName	= g_ItemInfoMgr:GetCommonlySkillByID(Class,EquipInfo[1],EquipInfo[2])
		local ShortCutDB = RequireDbBox("ShortcutDB")
		ShortCutDB.UpDateShortcut(PlayerId,new_CommonlySkillName,old_CommonlySkillName)
		ShortCutRet = ShortCutDB.GetShortcutInfo(PlayerId)
	end	
	if SuitName ~=nil then
	    EquipSuitTbl_OldPre = UseItemSql.GetSuitAttributeBySuitName(PlayerId, SuitName)
	end
	if EquipSuitTbl_OldPre and EquipSuitTbl_Old and EquipSuitTbl_OldPre[2] == EquipSuitTbl_Old[2] then
	    EquipSuitTbl_Old = nil
	end
    local suc, skillIndex = AdvanceEquipDB.GetRemoveJingLingSkillIndex(PlayerId)
    if suc == false and skillIndex == -1 then
        CancelTran()
		return false,802
    end
    local haveDemagedEquip = EquipDurabilityDB.CheckHaveDemagedEquip(PlayerId)
	local tbl =
	{
		["EquipInfo"] = EquipInfo,
		["EquipRet_Old"] = EquipRet_Old,
		["new_CommonlySkillName"] = new_CommonlySkillName,
		["ShortCutRet"] = ShortCutRet,
		["EquipSuitTbl_Old"] = EquipSuitTbl_Old,
		["RemoveJingLingSkillIndex"] = skillIndex,
		["HaveDemagedEquip"] = haveDemagedEquip,
	}
	return true,tbl
end

local function CommonFetchEquipCode(EquipInfo,PlayerId,eEquipPart,Class,sceneName)
	local EquipMgrDB = RequireDbBox("EquipMgrDB")
	local EquipRet_Old = EquipMgrDB.GetEquipAllInfo( EquipInfo[3] )
	local SuitName = UseItemSql.GetEquipSuitName(EquipInfo[3])
	local EquipSuitTbl_Old = UseItemSql.GetSuitAttributeBySuitName(PlayerId, SuitName)	
	local DeleteResult = EquipMgrDB.DeleteEquip(PlayerId,eEquipPart)
	if not DeleteResult then
		CancelTran()
		return false,802
	end
	local new_CommonlySkillName,old_CommonlySkillName
	local ShortCutRet
	if eEquipPart == EEquipPart.eWeapon  then
		new_CommonlySkillName = g_GetPlayerClassNameByID(Class).. "普通攻击"
		if EquipRet_Old["CurDuraValue"]	 == 0 then
			old_CommonlySkillName = new_CommonlySkillName
		else
			old_CommonlySkillName = g_ItemInfoMgr:GetCommonlySkillByID(Class,EquipInfo[1],EquipInfo[2])			
		end
		local ShortCutDB = RequireDbBox("ShortcutDB")
		if new_CommonlySkillName ~= old_CommonlySkillName then
			ShortCutDB.UpDateShortcut(PlayerId,new_CommonlySkillName,old_CommonlySkillName)	
			ShortCutRet = ShortCutDB.GetShortcutInfo(PlayerId)
		end
	end
	local suc, skillIndex = AdvanceEquipDB.GetRemoveJingLingSkillIndex(PlayerId)
    if suc == false and skillIndex == -1 then
        CancelTran()
		return false,802
    end
    local haveDemagedEquip = EquipDurabilityDB.CheckHaveDemagedEquip(PlayerId)
    --摘除装备写log
		local LogMgrDB = RequireDbBox("LogMgrDB")
    local uEventId = LogMgrDB.SaveEquipPutOffInfo( PlayerId, EquipInfo[3],sceneName )
	local tbl =
	{
		["EquipInfo"] = EquipInfo,
		["EquipRet_Old"] = EquipRet_Old,
		["new_CommonlySkillName"] = new_CommonlySkillName,
		["ShortCutRet"] = ShortCutRet,
		["EquipSuitTbl_Old"] = EquipSuitTbl_Old,
		["RemoveJingLingSkillIndex"] = skillIndex,
		["HaveDemagedEquip"] =  haveDemagedEquip,
		["uEventId"] = uEventId
	}

	
	return true,tbl
end

--取下装备
function UseItemSql.FetchEquip(data)
	local PlayerId  = data["PlayerId"]
	local eEquipPart = data["EquipPart"]
	local nPos = data["Pos"]
	local Class = data["Class"]
	local nRoomIndex = data["RoomIndex"]
	local sceneName = data["sceneName"]
	
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	if not g_RoomMgr.CheckRoomAndPos(PlayerId,nRoomIndex,nPos) then
		return false,1022
	end
	--空间必须没有物品
	local nBigID,nIndex,nItemID = g_RoomMgr.GetOneItemByPosition(PlayerId,nRoomIndex,nPos)
	if nBigID ~= nil then
		return false,1022
	end

	local EquipMgrDB = RequireDbBox("EquipMgrDB")
	local EquipInfo = EquipMgrDB.GetEquipInfo(PlayerId,eEquipPart)
	if EquipInfo == nil then
		return false,1011
	end
	
	local RenewItemResult = g_RoomMgr.RenewItemRoomPos(PlayerId,EquipInfo[3],nRoomIndex,nPos)
	if IsNumber(RenewItemResult) then
		CancelTran()
		return false,802		
	end		
	local succ,tbl = CommonFetchEquipCode(EquipInfo,PlayerId,eEquipPart,Class,sceneName)
    local SuitName = UseItemSql.GetEquipSuitName(EquipInfo[3])
	local EquipSuitTblPre = UseItemSql.GetSuitAttributeBySuitName(PlayerId, SuitName)	
	if EquipSuitTblPre and tbl["EquipSuitTbl_Old"] and EquipSuitTblPre[2] == tbl["EquipSuitTbl_Old"][2] then
	    tbl["EquipSuitTbl_Old"] = nil
	end
	
	if succ then
		tbl["Pos"] = nPos
		tbl["RoomIndex"] = nRoomIndex
		tbl["EquipID"] = EquipInfo[3]
	end
	return succ,tbl
end

--交换戒指
function UseItemSql.SwitchTwoRing(data)
	local PlayerId = data["PlayerId"]
	local EquipMgrDB = RequireDbBox("EquipMgrDB")
	local RingLeftInfo = EquipMgrDB.GetEquipInfo(PlayerId,EEquipPart.eRingLeft)
	local RingRightInfo = EquipMgrDB.GetEquipInfo(PlayerId,EEquipPart.eRingRight)

	if RingLeftInfo == nil and RingRightInfo == nil then
		return false,1038
	end

	local RingLeftRet,RingRightRet
	if RingLeftInfo ~= nil then
		local TempRet = EquipMgrDB.GetEquipAllInfo( RingLeftInfo[3] )
		RingLeftRet = TempRet
	end
	if RingRightInfo ~= nil then
		local TempRet = EquipMgrDB.GetEquipAllInfo( RingRightInfo[3] )
		RingRightRet = TempRet	
	end
		
	local ret = EquipMgrDB.SwitchTwoRing(PlayerId,RingLeftInfo,RingRightInfo)
	if ret == false then
	   	return false, 1038
	end
	
	local tbl = 
	{
		["RingLeftInfo"] = RingLeftInfo,
		["RingRightInfo"] = RingRightInfo,
		["RingLeftRet"] = RingLeftRet,
		["RingRightRet"] = RingRightRet
	}
	return true,tbl
end

--交换主手和副手武器
function UseItemSql.SwitchWeapon(data)
	local PlayerId = data["PlayerId"]
	local Class = data["Class"]
	local EquipMgrDB = RequireDbBox("EquipMgrDB")
	local EquipIntensifyDB				=	RequireDbBox("EquipIntensifyDB")
	local WeaponInfo = EquipMgrDB.GetEquipInfo(PlayerId,EEquipPart.eWeapon)
	local AssociateWeaponInfo = EquipMgrDB.GetEquipInfo(PlayerId,EEquipPart.eAssociateWeapon)
	
	if WeaponInfo == nil and AssociateWeaponInfo == nil then
		return false,0
	end
	
	local Info1,Info2
	local Info1_EquipType = ""
	local Info2_EquipType = ""
	if WeaponInfo then
		Info1 = g_ItemInfoMgr:HaveItem(WeaponInfo[1],WeaponInfo[2])
	end
	if AssociateWeaponInfo then
		Info2 = g_ItemInfoMgr:HaveItem(AssociateWeaponInfo[1],AssociateWeaponInfo[2])
	end

	if Info1 then
		Info1_EquipType = g_ItemInfoMgr:GetItemInfo(WeaponInfo[1],WeaponInfo[2],"EquipType")
	end
	if Info2 then
		Info2_EquipType = g_ItemInfoMgr:GetItemInfo(AssociateWeaponInfo[1],AssociateWeaponInfo[2],"EquipType")
	end
	if Info1_EquipType ~= "" and (not CheckWeaponEquipType(Info1_EquipType)) then
		return false,0
	end
	
	if Info2_EquipType ~= "" and (not CheckWeaponEquipType(Info2_EquipType)) then
		return false,0
	end
	
	local WeaponRet,AssociateWeaponRet
	if WeaponInfo then
		local TempRet = EquipMgrDB.GetEquipAllInfo( WeaponInfo[3] )
		WeaponRet = TempRet
	end
	
	if AssociateWeaponInfo then
		local TempRet = EquipMgrDB.GetEquipAllInfo( AssociateWeaponInfo[3] )
		AssociateWeaponRet = TempRet
	end

	local result = EquipMgrDB.SwitchWeapon(PlayerId,WeaponInfo,AssociateWeaponInfo	)
	if not result then
		CancelTran()
		return false,0		
	end

	local ShortCutRet,old_CommonlySkillName,new_CommonlySkillName
	if WeaponInfo then
		if WeaponRet["CurDuraValue"] > 0 then
			old_CommonlySkillName	= g_ItemInfoMgr:GetCommonlySkillByID(Class,WeaponInfo[1],WeaponInfo[2])
		else
			old_CommonlySkillName = g_GetPlayerClassNameByID(Class).. "普通攻击"
		end
	else
		old_CommonlySkillName = g_GetPlayerClassNameByID(Class).. "普通攻击"
	end
	
	if AssociateWeaponInfo then 
		if AssociateWeaponRet["CurDuraValue"] > 0 then
			new_CommonlySkillName	= g_ItemInfoMgr:GetCommonlySkillByID(Class,AssociateWeaponInfo[1],AssociateWeaponInfo[2])
		else
			new_CommonlySkillName	= g_GetPlayerClassNameByID(Class).. "普通攻击"	
		end
	else
		new_CommonlySkillName	= g_GetPlayerClassNameByID(Class).. "普通攻击"		
	end
	
	local ShortCutDB = RequireDbBox("ShortcutDB")
	ShortCutDB.UpDateShortcut(PlayerId,new_CommonlySkillName,old_CommonlySkillName)	
	ShortCutRet = ShortCutDB.GetShortcutInfo(PlayerId)

	local tbl = 
	{
		["WeaponInfo"] = WeaponInfo,
		["AssociateWeaponInfo"] = AssociateWeaponInfo,
		["WeaponInfoTbl"] = WeaponRet,
		["AssociateWeaponInfoTbl"] = AssociateWeaponRet,
		["ShortCutRet"] = ShortCutRet,
		["new_CommonlySkillName"] = new_CommonlySkillName
	}
	return true,tbl
end

----缴械
function UseItemSql.ForbidUseWeapon(data)
	local PlayerId = data["PlayerId"]
	local forbidFlag = data["ForbidFlag"]
	local EquipMgrDB = RequireDbBox("EquipMgrDB")

	local WeaponInfo = EquipMgrDB.GetEquipInfo(PlayerId,EEquipPart.eWeapon)
	local WeaponID = WeaponInfo and WeaponInfo[3] or 0

	local WeaponRet,AssociateWeaponRet = nil,nil
	if WeaponID ~= 0 then
		local TempRet = EquipMgrDB.GetEquipAllInfo( WeaponID )
		WeaponRet = TempRet
	end

	local tbl =
	{
		["WeaponInfo"] = WeaponInfo,
		["WeaponRet"]  = WeaponRet,
		["ForbidFlag"] = forbidFlag
	}
	return tbl
end

--右键摘除装备
function UseItemSql.RClickFetchEquip(data)
	local PlayerId  = data["PlayerId"]
	local eEquipPart = data["EquipPart"]
	local Class = data["Class"]
	local sceneName = data["sceneName"]
	
	local EquipMgrDB = RequireDbBox("EquipMgrDB")
	local EquipInfo = EquipMgrDB.GetEquipInfo(PlayerId,eEquipPart)
	if EquipInfo == nil then
		return false,1011
	end
	
	local nType, sName,itemId = EquipInfo[1],EquipInfo[2],EquipInfo[3]
	local item_bag_box = RequireDbBox("CPutItemToBagDB")
	local succ,tbl = CommonFetchEquipCode(EquipInfo,PlayerId,eEquipPart,Class,sceneName)
    local SuitName = UseItemSql.GetEquipSuitName(EquipInfo[3])
	local EquipSuitTblPre = UseItemSql.GetSuitAttributeBySuitName(PlayerId, SuitName)	
	if EquipSuitTblPre and tbl["EquipSuitTbl_Old"] and EquipSuitTblPre[2] == tbl["EquipSuitTbl_Old"][2] then
	    tbl["EquipSuitTbl_Old"] = nil
	end
	if succ then
		local params = {}
		params.m_nCharID = PlayerId
		params.m_nItemType = nType
		params.m_sItemName = sName
		params.m_tblItemID = {{itemId}}
		local bFlag, ret_info = item_bag_box.AddCountItem(params)
		if bFlag then
			tbl["RoomIndex"] = ret_info[1].m_nRoomIndex
			tbl["Pos"]  = ret_info[1].m_nPos
			return true,tbl
		else
			CancelTran()
			return false,ret_info
		end
	end
end


SetDbLocalFuncType(UseItemSql.ForbidUseWeapon) 
SetDbLocalFuncType(UseItemSql.SwitchTwoRing)
SetDbLocalFuncType(UseItemSql.SwitchWeapon)
SetDbLocalFuncType(UseItemSql.FetchEquip)
SetDbLocalFuncType(UseItemSql.DelEquip)
SetDbLocalFuncType(UseItemSql.RClickFetchEquip)
SetDbLocalFuncType(UseItemSql.EntryFunc)
return UseItemSql


