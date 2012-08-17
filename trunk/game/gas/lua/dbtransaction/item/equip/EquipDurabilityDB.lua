cfg_load "equip/EquipDurabilityValue_Server"
cfg_load "equip/MoneyConsumedFixingEquip_Common"
gac_gas_require "item/Equip/EquipChecking"
gac_gas_require "player/PlayerMgr"

local LogErr                =   LogErr
local EEquipPart = EEquipPart
local loadstring = loadstring
local EquipDurabilityValue_Server = EquipDurabilityValue_Server
local EquipMgrDB = RequireDbBox("EquipMgrDB")
local MoneyConsumedFixingEquip_Common = MoneyConsumedFixingEquip_Common
local EEquipPart = EEquipPart
local g_StoreRoomIndex = g_StoreRoomIndex
local g_MoneyMgr = CMoney:new()
local g_ItemInfoMgr = g_ItemInfoMgr
local g_GetPlayerClassNameByID = g_GetPlayerClassNameByID
local event_type_tbl = event_type_tbl
local EquipDurabilityDB = CreateDbBox(...)
local SqlTbl = {}

-----------------------------------------

local StmtDef = {
    "_InsertEquipDuraValue",
    [[
        insert into tbl_item_equip_durability set is_uId= ? , ied_uMaxDuraValue =?, ied_uCurDuraValue =?
    ]]
}
DefineSql(StmtDef, SqlTbl)

local EquipDuraOutRandomScaleTbl = {}
local function ReadEquipDuraOutRandomScaleFromCfg()
	local Keys = EquipDurabilityValue_Server:GetKeys()
    for i, v in pairs(Keys) do
        local RowInfo = EquipDurabilityValue_Server(v)
        local OutRandomScaleTbl = loadstring("return" .. RowInfo("OutRandomScale"))()
        EquipDuraOutRandomScaleTbl[v] = OutRandomScaleTbl
    end
end
ReadEquipDuraOutRandomScaleFromCfg()

--装备产出时，记录装备当前的耐久值
function EquipDurabilityDB.InsertEquipDuraValue(equipID, baseLevel,charId)
	local duraInfo = EquipDurabilityValue_Server(baseLevel)
	local duraValue = duraInfo("MaxDurabilityValue")
	local randomTbl =  EquipDuraOutRandomScaleTbl[baseLevel]
	local tblLen = # randomTbl
	local randomNo = math.random(1, tblLen)
	local duraScale = randomTbl[randomNo]/100
	duraValue = math.ceil( duraValue * (1 + duraScale) )
    SqlTbl._InsertEquipDuraValue:ExecStat(equipID, duraValue, duraValue)
    if g_DbChannelMgr:LastAffectedRowNum() < 1 then
  		CancelTran()
  		return false
 	end
 	local g_LogMgr = RequireDbBox("LogMgrDB")
 	g_LogMgr.SaveEquipDuraInfo( charId, equipID,duraValue,duraValue )
    return true
end


local StmtDef = {
    "_GetEquipCurDuraValue",
    [[
        select ied_uCurDuraValue, ied_uMaxDuraValue  from tbl_item_equip_durability where is_uId = ?
    ]]
}
DefineSql(StmtDef, SqlTbl)
function EquipDurabilityDB.GetEquipCurDuraValue(equipID)
    local duraRet = SqlTbl._GetEquipCurDuraValue:ExecStat(equipID)
    if duraRet ~=nil and duraRet:GetRowNum() > 0 then
       return duraRet:GetDataByName(0, "ied_uCurDuraValue"), duraRet:GetDataByName(0, "ied_uMaxDuraValue")
    end
    return 0, 0
end

-----根据装备当前耐久值和耐久上限的比较，得出装备当前的损耗状态
--返回值：（装备当前的耐久值/耐久上限的比例值）0表示为0；1表示小于30%但大于0; 2表示大于30%
function EquipDurabilityDB.GetEquipUllageState(equipID)
    local curDuraValue, maxDuraValue = EquipDurabilityDB.GetEquipCurDuraValue(equipID)
    local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
    local equipType, equipName = g_RoomMgr.GetItemType(equipID)
    local equipBaseLevel = EquipMgrDB.GetEquipBaseLevel(equipType, equipID)
    if equipBaseLevel == 0 then
       return 0, 0, 0
    end
    --local maxDuraValue = EquipDurabilityValue_Server[equipBaseLevel].MaxDurabilityValue
    local scale = curDuraValue / maxDuraValue
    if scale >= 0.3 then
        return 2, curDuraValue, maxDuraValue
    elseif scale > 0 and scale < 0.3 then
        return 1, curDuraValue, maxDuraValue
    elseif scale == 0 then
        return 0 , curDuraValue, maxDuraValue
    end
    return 0, curDuraValue, maxDuraValue 
end

--披风不损伤耐久
local StmtDef = {
    "_GetEquipRetNotDura0ByCharID",
    [[
        select 
            equip.is_uId, equip.ce_uEquipPart from tbl_item_equip as equip, tbl_item_equip_durability as dura 
        where cs_uId = ? and dura.ied_uCurDuraValue > 0 and equip.is_uId = dura.is_uId 
                        and equip.ce_uEquipPart != ?
    ]]
}
DefineSql(StmtDef, SqlTbl)

local StmtDef = {
    "_UpdateEquipDuraValue",
    [[
        update tbl_item_equip_durability set ied_uCurDuraValue = ? where is_uId = ?
    ]]
}
DefineSql(StmtDef, SqlTbl)

local function GetDecreaseDuraValue(equipID, attackTimes, beAttackedTimes, deadFlag, equipType)
    local equipLevel = EquipMgrDB.GetEquipBaseLevel(equipType, equipID)
    if equipLevel == 0 then
       return false, 187000 
    end

    local decreasingValueOneAttack = EquipDurabilityValue_Server(equipLevel,"AttackedDecreaseValue")
    local totalDecreasingValue = decreasingValueOneAttack * (attackTimes + beAttackedTimes)
    if deadFlag then
        totalDecreasingValue = totalDecreasingValue + EquipDurabilityValue_Server(equipLevel,"DeadDecreaseValue")
    end
    return totalDecreasingValue
end

--攻击和被攻击次数达到一定数目的时候，计算装备耐久相关信息
function EquipDurabilityDB.UpdateEquipDurability(data)
    local charID            =   data["CharID"]
    local attackTimes       =   data["AttackTimes"]   
    local beAttackedTimes   =   data["BeAttackTimes"]
    local bDead             =   data["DeadFlag"]
    local g_EquipDbMgr = RequireDbBox("EquipMgrDB")
	local AllInfo = g_EquipDbMgr.GetPlayerEquipData(charID) 
		   
    local equipRet  = SqlTbl._GetEquipRetNotDura0ByCharID:ExecStat(charID, EEquipPart.eBack)
    local equipRetLen = equipRet:GetRowNum()
    local equipTbl  = {}
    if equipRet ~=nil and equipRetLen > 0 then
        equipTbl = equipRet:GetTableSet()
    end
    if equipRetLen == 0 then
       return  false,{["AllEquipInfo"] =AllInfo}
    end
    local random = math.random(1, 2)
    local damagedEquipCount = random + 1
    local damagedEquipTbl = {}
    local g_RoomMgr = RequireDbBox("GasRoomMgrDB")

    if damagedEquipCount > equipRetLen then
        damagedEquipCount = equipRetLen
    end
    for i=1, damagedEquipCount do
        local randomEquipNo = math.random(1, equipRetLen)
        local equipID = equipTbl(randomEquipNo,1)
        local equipPart = equipTbl(randomEquipNo,2)
        local equipType, equipName = g_RoomMgr.GetItemType(equipID)
        local totalDecreasingValue, msgID = GetDecreaseDuraValue(equipID, attackTimes, beAttackedTimes, bDead, equipType)
        if totalDecreasingValue == false then
            return false, msgID
        end
        local curDuraValue  =  EquipDurabilityDB.GetEquipCurDuraValue(equipID) - totalDecreasingValue
        if curDuraValue  < 0 then
            curDuraValue = 0
        end
        curDuraValue = string.format("%.4f", curDuraValue)
        SqlTbl._UpdateEquipDuraValue:ExecStat(curDuraValue, equipID)
        if g_DbChannelMgr:LastAffectedRowNum() < 1 then
      		CancelTran()
      		return false,{["AllEquipInfo"] =AllInfo}
 	    end
 	    local MaxDuraValueRet = SqlTbl._GetEquipMaxDuraValue:ExecStat(equipID)
    	local MaxDuraValue = MaxDuraValueRet:GetData(0,0)
 	    local g_LogMgr = RequireDbBox("LogMgrDB")
 			g_LogMgr.SaveEquipDuraInfo( charID, equipID,MaxDuraValue,curDuraValue )
 			
 	    local duraState, curDuraValue, maxDuraValue = EquipDurabilityDB.GetEquipUllageState(equipID)
 	    local equipAllInfo = {}
 	    local shortCut, newCommonlySkillName
 	    if duraState == 0 then
 	        equipAllInfo = EquipMgrDB.GetEquipAllInfo(equipID)   
            if equipPart == EEquipPart.eWeapon then
                local shortCutRet, newCommonSkill = EquipDurabilityDB.UpdateWeaponRelatedShortCut(charID, equipType, equipName, false)
                shortCut = shortCutRet
                newCommonlySkillName = newCommonSkill
            end
 	    end
 	    local newCurDura = EquipMgrDB.GetIntegralDuraValue(curDuraValue)
        local EquipTbli = {equipID, duraState, equipAllInfo, equipPart, equipType, equipName, newCurDura, maxDuraValue, shortCut, newCommonlySkillName}
        table.insert(damagedEquipTbl,EquipTbli)
        equipRetLen = equipRetLen - 1
    end
	local result = {["EquipInfoTbl"] =damagedEquipTbl,["AllEquipInfo"] =AllInfo}
	return true,  result
end

local StmtDef = {
    "UpdateEquipDuraMaxValue",
    [[
        update tbl_item_equip_durability set ied_uCurDuraValue = ied_uMaxDuraValue where is_uId= ?
    ]]
}
DefineSql(StmtDef, SqlTbl)

local StmtDef = {
    "_CheckHaveDemagedEquip",
    [[
        select count(*) from tbl_item_equip_durability as dura, tbl_item_equip as equip 
        where equip.cs_uId = ? and dura.is_uId = equip.is_uId  
        and dura.ied_uCurDuraValue / ied_uMaxDuraValue < 0.3 
    ]]
}
DefineSql(StmtDef, SqlTbl)

--检查当前身上有无耐久损伤小于30%的装备
function EquipDurabilityDB.CheckHaveDemagedEquip(charID)
    local bHaveDemagedEquip = 0
    local bHaveDemagedEquipRet = SqlTbl._CheckHaveDemagedEquip:ExecStat(charID)
    if bHaveDemagedEquipRet:GetRowNum() > 0 then
        bHaveDemagedEquip = bHaveDemagedEquipRet:GetData(0, 0)
    end
    return bHaveDemagedEquip
end

local StmtDef = {
    "_GetEquipMaxDuraValue",
    [[
        select ied_uMaxDuraValue from tbl_item_equip_durability where is_uId = ?
    ]]
}
DefineSql(StmtDef, SqlTbl)

function EquipDurabilityDB.UpdateWeaponRelatedShortCut(charID, equipType, equipName, bFlag)
    local FightSkillDB = RequireDbBox("FightSkillDB")
    local playerClass = FightSkillDB.Dbs_SelectPlayerClass(charID)
    local oldCommonlySkill, newCommonlySkill
    if bFlag then --耐久从0恢复到正常
        oldCommonlySkill = g_GetPlayerClassNameByID(playerClass).. "普通攻击"
        newCommonlySkill = g_ItemInfoMgr:GetCommonlySkillByID(playerClass,equipType,equipName)
    else
        oldCommonlySkill = g_ItemInfoMgr:GetCommonlySkillByID(playerClass,equipType,equipName)        
        newCommonlySkill = g_GetPlayerClassNameByID(playerClass).. "普通攻击"
    end
    local ShortCutDB = RequireDbBox("ShortcutDB")
    ShortCutDB.UpDateShortcut(charID,newCommonlySkill,oldCommonlySkill)
    local ShortCutRet = ShortCutDB.GetShortcutInfo(charID)

    return ShortCutRet, newCommonlySkill
end

--修复选中装备当前的耐久值
function EquipDurabilityDB.RenewEquipDuraValue(data)
    local charID            = data["CharID"]
    local equipID           = data["EquipID"]
    local equipPart         = data["EquipPart"]
    local needGetPos        = data["GetPosFlag"]
    local checkDemagedEquip = data["CheckDemagedEquip"]
    local choosedMoneyType  = data["ChoosedMoneyType"]


    local ItemBagLockDB = RequireDbBox("ItemBagLockDB")
    --检测帐户是否锁定
    if ItemBagLockDB.HaveItemBagLock(charID) then
        return false,160045
    end
    
    local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
    --判断是否过期物品
    local ItemLifeMgrDB = RequireDbBox("ItemLifeMgrDB")
		if not ItemLifeMgrDB.IsTimeOutItem(equipID) then
			return false,187004
		end
    local equipType, equipName = g_RoomMgr.GetItemType(equipID)
    local curDuraValue, maxDuraValue = EquipDurabilityDB.GetEquipCurDuraValue(equipID)
    
    local equipRet = nil
    local curDuraState =  curDuraValue
    if curDuraValue == maxDuraValue then
       return false, 187001 
    end
    local shortCut, newCommonlySkillName
    if curDuraState == 0 then
        equipRet = EquipMgrDB.GetEquipAllInfo(equipID)
        if equipPart == EEquipPart.eWeapon then
            local shortCutRet, newCommonSkill = EquipDurabilityDB.UpdateWeaponRelatedShortCut(charID, equipType, equipName, true)
            shortCut = shortCutRet
            newCommonlySkillName = newCommonSkill
        end
    end
    local roomIndex, pos
    
    if needGetPos then
        local tbl= g_RoomMgr.GetRoomIndexAndPosByItemId(charID, equipID)
        roomIndex, pos = tbl(1,1), tbl(1,2)
    end
    
    local equipBaseLevel = EquipMgrDB.GetEquipBaseLevel(equipType, equipID) 
    if equipBaseLevel == 0 then
        LogErr("未查询到装备等级", "ItemID:" .. ( equipID or "-" ) .. "EquipType" .. ( equipType or "-" ))
        return  false, 187002
    end
    local needMoney = MoneyConsumedFixingEquip_Common(equipBaseLevel,"MoneyConsumed")
    local fun_info = g_MoneyMgr:GetFuncInfo("EquipDurability")
    local MoneyManagerDB =	RequireDbBox("MoneyMgrDB")
	--local bFlag,uMsgID = MoneyManagerDB.AddMoneyByType(fun_info["FunName"],fun_info["RenewEquip"],charID, choosedMoneyType, -needMoney,nil,event_type_tbl["修装备扣钱"])
	local bFlag,uMsgID, moneyCount = MoneyManagerDB.SubtractMoneyBySysFee(needMoney, charID, fun_info, fun_info["RenewEquip"],event_type_tbl["修装备扣钱"])
    local add_Money = moneyCount
	local add_bMoney = uMsgID

	if not bFlag then
		CancelTran()
		if IsNumber(uMsgID) then
			return false,uMsgID
		else
			return false, 187002
		end
	end
    SqlTbl.UpdateEquipDuraMaxValue:ExecStat(equipID)
    if g_DbChannelMgr:LastAffectedRowNum() < 1 then
  		CancelTran()
  		return false, 187001
    end
    local bHaveDemagedEquip
    if checkDemagedEquip then
         bHaveDemagedEquip= EquipDurabilityDB.CheckHaveDemagedEquip(charID)
    end    
    local MaxDuraValueRet = SqlTbl._GetEquipMaxDuraValue:ExecStat(equipID)
    local MaxDuraValue = MaxDuraValueRet:GetData(0,0)
    local g_LogMgr = RequireDbBox("LogMgrDB")
 		g_LogMgr.SaveEquipDuraInfo( charID, equipID,MaxDuraValue,MaxDuraValue )
    local result = {["EquipID"]=equipID, ["EquipPart"]= equipPart, ["EquipType"]= equipType, ["EquipName"] = equipName,["OldDuraState"] = curDuraState, ["EquipRet"] =equipRet,
                    ["RoomIndex"] = roomIndex, ["Pos"] =pos, ["bHaveDemagedEquip"] = bHaveDemagedEquip, ["MaxDuraValue"] = MaxDuraValue, ["CostMoney"] =-needMoney,
                    ["Money"] =add_Money , ["BindingMoney"] = add_bMoney, ["ChoosedMoneyType"] = choosedMoneyType, ["ShortCutRet"] = shortCut, ["NewCommonlySkillName"] = newCommonlySkillName}
    return true, result
end


local StmtDef = {
    "_GetEquipRetByCharID",
    [[
        select equip.is_uId, equip.ce_uEquipPart, dura.ied_uCurDuraValue, dura.ied_uMaxDuraValue from tbl_item_equip as equip, tbl_item_equip_durability as dura 
        where equip.cs_uId = ? and equip.is_uId = dura.is_uId and dura.ied_uCurDuraValue < dura.ied_uMaxDuraValue
    ]]
}
DefineSql(StmtDef, SqlTbl)

--修复背包中的装备isr_uRoomIndex >= 50 and gir_uRoomIndex < 54表明是添加的附背包中的
local StmtDef = {
    "_GetEquipRetInBag",
    [[
				select distinct(room.is_uId), igp.gir_uRoomIndex, igp.gir_uPos, dura.ied_uCurDuraValue 
        from tbl_item_equip_durability as dura, tbl_item_in_grid as room ,tbl_grid_in_room as igp
        where igp.gir_uGridID = room.gir_uGridID and  room.is_uId = dura.is_uId and dura.ied_uCurDuraValue < dura.ied_uMaxDuraValue 
        and igp.cs_uId = ? and ( igp.gir_uRoomIndex = ? 
        or (igp.gir_uRoomIndex in (select bag.ibiu_uRoomIndex from tbl_item_bag_in_use bag where igp.cs_uId = bag.cs_uId) ))    
    ]]
}
DefineSql(StmtDef, SqlTbl)


--修复玩家身上所有的装备
function EquipDurabilityDB.RenewAllEquipDuraValue(data)
    local charID = data["CharID"]
    local choosedMoneyType = data["ChoosedMoneyType"]
    local equipRet  = SqlTbl._GetEquipRetByCharID:ExecStat(charID)
    local equipTbl  = {}    --耐久值小于耐久上限值在身上的装备
    local ItemBagLockDB = RequireDbBox("ItemBagLockDB")
    --检测帐户是否锁定
    if ItemBagLockDB.HaveItemBagLock(charID) then
        return false,160045
    end
    
    
    local equipRetLen = equipRet:GetRowNum()
    if equipRet ~=nil and equipRetLen > 0 then
        equipTbl = equipRet:GetTableSet()
    end
    local equipRetInBag = SqlTbl._GetEquipRetInBag:ExecStat(charID, g_StoreRoomIndex.PlayerBag)
    local equipRetInBagTbl =  {}--耐久值小于耐久上限值在包裹里的装备
    local equipRetInBagLen = equipRetInBag:GetRowNum()
    if equipRetInBag ~= nil and equipRetInBagLen > 0 then 
         equipRetInBagTbl = equipRetInBag:GetTableSet()
    end
          
    if equipRetInBagLen == 0 and equipRetLen == 0 then
       return  false, 187000
    end
    
    local result = {}       
    result["EquipOnWearing"] = {}
    result["EquipInBag"] = {}
    for i=1, equipRetLen do
        local euqipID    =  equipTbl(i,1)
        local para = {["EquipID"] =equipTbl(i,1), ["EquipPart"]=equipTbl(i,2), ["ChoosedMoneyType"] = choosedMoneyType, ["CharID"]= charID}
        local suc, ret   =  EquipDurabilityDB.RenewEquipDuraValue(para)
        if suc ~= true then
            CancelTran()
            return false, ret
        end
        table.insert(result["EquipOnWearing"], ret)
    end
    
    for i=1, equipRetInBagLen do
        local equipID = equipRetInBagTbl(i,1)
        local roomIndex = equipRetInBagTbl(i,2)
        local pos       = equipRetInBagTbl(i,3)
        local curDuraValue = equipRetInBagTbl(i,4)
        local para = {["EquipID"] =equipRetInBagTbl(i,1), ["EquipPart"]=0, ["ChoosedMoneyType"] =choosedMoneyType, ["CharID"]= charID}
        local suc, ret   =  EquipDurabilityDB.RenewEquipDuraValue(para)
        if suc ~= true then
            CancelTran()
            return false, ret
        end
        ret["RoomIndex"] = roomIndex
        ret["Pos"] = pos
        table.insert(result["EquipInBag"], ret)
    end
    
    return true, result
end

--仅修复玩家身上的装备
function EquipDurabilityDB.RenewEquipInRoleDuraValue(data)
    local charID = data["CharID"]
    local choosedMoneyType = data["ChoosedMoneyType"]
    local equipRet  = SqlTbl._GetEquipRetByCharID:ExecStat(charID)
    local equipTbl  = {}    --耐久值小于耐久上限值在身上的装备
    local ItemBagLockDB = RequireDbBox("ItemBagLockDB")
    --检测帐户是否锁定
    if ItemBagLockDB.HaveItemBagLock(charID) then
        return false,160045
    end
    
    
    local equipRetLen = equipRet:GetRowNum()
    if equipRet ~=nil and equipRetLen > 0 then
        equipTbl = equipRet:GetTableSet()
    end
    
    local result = {}       
    result["EquipOnWearing"] = {}
    for i=1, equipRetLen do
        local euqipID    =  equipTbl(i,1)
        local para = {["EquipID"] =equipTbl(i,1), ["EquipPart"]=equipTbl(i,2), ["ChoosedMoneyType"] = choosedMoneyType, ["CharID"]= charID}
        local suc, ret   =  EquipDurabilityDB.RenewEquipDuraValue(para)
        if suc ~= true then
            CancelTran()
            return false, ret
        end
        table.insert(result["EquipOnWearing"], ret)
    end  
    return true, result
end

--仅修复背包内的装备
function EquipDurabilityDB.RenewEquipInBagDuraValue(data)
    local charID = data["CharID"]
    local choosedMoneyType = data["ChoosedMoneyType"]
    local ItemBagLockDB = RequireDbBox("ItemBagLockDB")
    --检测帐户是否锁定
    if ItemBagLockDB.HaveItemBagLock(charID) then
        return false,160045
    end
    local equipRetInBag = SqlTbl._GetEquipRetInBag:ExecStat(charID, g_StoreRoomIndex.PlayerBag)
    local equipRetInBagTbl =  {}--耐久值小于耐久上限值在包裹里的装备
    local equipRetInBagLen = equipRetInBag:GetRowNum()
    if equipRetInBag ~= nil and equipRetInBagLen > 0 then 
         equipRetInBagTbl = equipRetInBag:GetTableSet()
    end
          
    if equipRetInBagLen == 0 and equipRetLen == 0 then
       return  false, 187000
    end
    
    local result = {}       
    result["EquipInBag"] = {}
    for i=1, equipRetInBagLen do
        local equipID = equipRetInBagTbl(i,1)
        local roomIndex = equipRetInBagTbl(i,2)
        local pos       = equipRetInBagTbl(i,3)
        local curDuraValue = equipRetInBagTbl(i,4)
        local para = {["EquipID"] =equipRetInBagTbl(i,1), ["EquipPart"]=0, ["ChoosedMoneyType"] =choosedMoneyType, ["CharID"]= charID}
        local suc, ret   =  EquipDurabilityDB.RenewEquipDuraValue(para)
        if suc ~= true then
            CancelTran()
            return false, ret
        end
        ret["RoomIndex"] = roomIndex
        ret["Pos"] = pos
        table.insert(result["EquipInBag"], ret)
    end
    
    return true, result
end

SetDbLocalFuncType(EquipDurabilityDB.UpdateEquipDurability)
SetDbLocalFuncType(EquipDurabilityDB.RenewEquipDuraValue)
SetDbLocalFuncType(EquipDurabilityDB.RenewAllEquipDuraValue)
return EquipDurabilityDB


