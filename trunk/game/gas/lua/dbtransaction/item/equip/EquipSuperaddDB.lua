cfg_load  "equip/superadd/EquipSuperaddMaxPhase_Common"
cfg_load  "equip/superadd/EquipSuperaddConsume_Common"
cfg_load  "equip/superadd/EquipSuperaddAttrRate_Common"

local GetMaxEquipSuperaddPhase = GetMaxEquipSuperaddPhase
local GetEquipSuperaddConsumeInfo = GetEquipSuperaddConsumeInfo
local CheckEquipCanBeSuperadded = CheckEquipCanBeSuperadded
local g_ItemInfoMgr = CItemInfoMgr:new()
local g_EquipSuperaddItemAdaptedEquipLevel = g_EquipSuperaddItemAdaptedEquipLevel
local EquipSuperaddAttrRate_Common = EquipSuperaddAttrRate_Common
local g_MoneyMgr = CMoney:new()
local LogErr      =     LogErr
local EquipSuperaddMaxPhase_Common = EquipSuperaddMaxPhase_Common
local EquipSuperaddConsume_Common = EquipSuperaddConsume_Common
local event_type_tbl = event_type_tbl

local EquipSuperaddDB = CreateDbBox(...)
local SqlTbl = {}
local LiuTongMoneyType = 1 	--流通金币支付方式

local StmtDef = {
    "_GetEquipSuperaddInfo",
    [[
        select ies_uCurSuperaddPhase from  tbl_item_equip_superaddation where is_uId = ?
    ]]
}DefineSql(StmtDef, SqlTbl)

local StmtDef = {
    "_InsertEquipSuperaddInfo",
    [[
        insert into tbl_item_equip_superaddation (ies_uCurSuperaddPhase, is_uId) values (?,?)
    ]]
}DefineSql(StmtDef, SqlTbl)

local function CheckEquipSuperaddSucOrNot(curSuperaddPhase)
    local sucProb = EquipSuperaddConsume_Common(curSuperaddPhase, "SucProb")
    local random = math.random(1, 100)
    --print("********")
    --print("random" .. random)
    --print("sucProb" .. sucProb)
    if random <= sucProb then    
        return true 
    end
    
    return false, 1054
end

local function GetConsumeStoneID(charID, needStoneNum, equipBaseLevel)
    local needStoneTblInfo = {}
    local leftNeedStoneNum = needStoneNum
    local stoneType = g_ItemInfoMgr:GetEquipSuperaddItem()
    local g_RoomMgr =RequireDbBox("GasRoomMgrDB")
    local stoneIDTbl = {}
    local haveStoneCount = 0
    for i, v in pairs(g_EquipSuperaddItemAdaptedEquipLevel) do
        if leftNeedStoneNum > 0 then
            local stoneName = i
            local equipLowLevel = v[1]
            local equipHighLevel = v[2]
            if equipLowLevel <= equipBaseLevel and equipBaseLevel <= equipHighLevel then
                local stoneCount =  g_RoomMgr.GetAllItemCount(charID, stoneType, stoneName) 
                if stoneCount > 0 then
                    if stoneCount < leftNeedStoneNum then
                        leftNeedStoneNum = leftNeedStoneNum - stoneCount
                        local stoneInfoRet = g_RoomMgr.GetNCountItemIdByName(charID, stoneType, stoneName, stoneCount)
                        haveStoneCount = haveStoneCount + stoneInfoRet:GetRowNum()
                        table.insert(stoneIDTbl, stoneInfoRet)
                    else
                        local stoneInfoRet = g_RoomMgr.GetNCountItemIdByName(charID, stoneType, stoneName, leftNeedStoneNum)
                        haveStoneCount = haveStoneCount + stoneInfoRet:GetRowNum()
                        table.insert(stoneIDTbl, stoneInfoRet)
                    end
                end
            end
        else
            break
        end
    end
   
    if haveStoneCount >= needStoneNum then
        return true, stoneIDTbl 
    else
        return false, 1057
    end    
end

function EquipSuperaddDB.EquipSuperaddConsume(charID, curSuperaddPhase, baselLevel, nBigId, itemName)
    local fun_info = g_MoneyMgr:GetFuncInfo("EquipSuperadd")
    local needConsumeMoney ,needConsumeSoul, needStoneNum= GetEquipSuperaddConsumeInfo(curSuperaddPhase, baselLevel, nBigId, itemName)

    local MoneyManagerDB = RequireDbBox("MoneyMgrDB")
    local bFlag,uMsgID = MoneyManagerDB.AddMoneyByType(fun_info["FunName"],fun_info["EquipSuperadd"],charID, LiuTongMoneyType, -needConsumeMoney,uEventId, event_type_tbl["装备追加消耗"])
    if not bFlag then
		CancelTran()
		if IsNumber(uMsgID) then
			return false,uMsgID
		else
			return false,1055
		end
	end
	
    local EquipMgrDB = RequireDbBox("EquipMgrDB")
	local haveEnoughSoulFlag = EquipMgrDB.HaveEnoughSoul(charID, needConsumeSoul)
	if haveEnoughSoulFlag == false then
	    CancelTran()
        return false, 1056 
	end
    local param = {["soulCount"] = -needConsumeSoul,["PlayerId"] = charID,["addSoulType"] = event_type_tbl["装备追加消耗"]}
	local bFlag,total_soul = EquipMgrDB.ModifyPlayerSoul(param)
	if not bFlag then
		local errMsgID = 1056
		CancelTran()
		return false,errMsgID
	end
	
	local stoneSucFlag, ret = GetConsumeStoneID(charID, needStoneNum, baselLevel)
	if stoneSucFlag == false then
	    CancelTran()
        return false, ret 
	end
	
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local stoneInfoTbl = {}
	for i, v in pairs (ret) do 
        local stoneInfoRet = v
        local stoneInfoCount = stoneInfoRet:GetRowNum()
        for i=1, stoneInfoCount do
            local stoneID = stoneInfoRet:GetData(i-1, 0) 
            local stoneRoomIndex = stoneInfoRet:GetData(i-1, 1) 
            local stonePos = stoneInfoRet:GetData(i-1, 2) 
            local suc, msgID = g_RoomMgr.DelItemByID(charID,stoneID,event_type_tbl["装备追加消耗"],uEventId)
            if suc ~= true then
                CancelTran()
                return false, msgID or 1057
    		end
    		
            table.insert(stoneInfoTbl, {stoneID, stoneRoomIndex, stonePos})
        end
	end
	
	local result = {}
	result["NeedConsumeMoney"] = -needConsumeMoney
	result["total_soul"] = total_soul
	result["StoneInfoTbl"] = stoneInfoTbl
	return true, result
end

local StmtDef = {
    "_UpdateEquipCurSuperaddPhase",
    [[
        update tbl_item_equip_superaddation set ies_uCurSuperaddPhase = ies_uCurSuperaddPhase + 1 where is_uId = ?
    ]]
}DefineSql(StmtDef, SqlTbl)

function EquipSuperaddDB.EquipSuperadd(data)
    local charID    =   data["CharID"]
    local equipType =   data["EquipType"]
    local equipName =   data["EquipName"]
    local equipID   =   data["EquipID"]
    
    local equipSuperaddationRet = SqlTbl._GetEquipSuperaddInfo:ExecStat(equipID)
    local rowNum = equipSuperaddationRet:GetRowNum()
    local equipCurSuperaddPhase = 0
    local EquipMgrDB = RequireDbBox("EquipMgrDB")
    local equipBaseLevel = EquipMgrDB.GetEquipBaseLevel(equipType, equipID)
    local equipMaxSuperaddPhase = GetMaxEquipSuperaddPhase(equipBaseLevel) or 0
    
    if rowNum >= 1 then
        equipCurSuperaddPhase =  equipSuperaddationRet:GetData(0, 0)
    end
    
    local AdvanceEquipDB = RequireDbBox("EquipAdvanceDB")
    local advanceRet = AdvanceEquipDB.GetEquipAdvaceInfo(equipID)
    if advanceRet:GetRowNum() < 1 then
        return false, 1059 
    end
    local advanceQuality = advanceRet:GetData(0, 1)

    local ret, msgID = CheckEquipCanBeSuperadded(advanceQuality, equipCurSuperaddPhase, equipMaxSuperaddPhase)
    if ret == false then
       return false, msgID 
    end
    
    local sucProbRet, errMsgID = CheckEquipSuperaddSucOrNot(equipCurSuperaddPhase)

    local EquipMgrDB = RequireDbBox("EquipMgrDB")
    local equipBaseLevel = EquipMgrDB.GetEquipBaseLevel(equipType, equipID)
    local consumeSucFlag, result = EquipSuperaddDB.EquipSuperaddConsume(charID, equipCurSuperaddPhase, equipBaseLevel, equipType, equipName)
    if consumeSucFlag == false then
        return false, result
    end
    
    
    local newCurSuperaddPhase = equipCurSuperaddPhase + 1
    if sucProbRet == true then
        if rowNum >= 1 then
            SqlTbl._UpdateEquipCurSuperaddPhase:ExecStat(equipID)
        else
            SqlTbl._InsertEquipSuperaddInfo:ExecStat(newCurSuperaddPhase, equipID)
        end
        if g_DbChannelMgr:LastAffectedRowNum() < 1 then
            CancelTran()
            return false, 1054
        end
        result["MsgID"] = 1058
    else
        newCurSuperaddPhase = equipCurSuperaddPhase
        result["MsgID"] = errMsgID
    end
    
    local superaddRate = EquipSuperaddAttrRate_Common(newCurSuperaddPhase ,"AddAttrRate")
    local oldSuperaddRate = EquipSuperaddAttrRate_Common(equipCurSuperaddPhase, "AddAttrRate")
    result["SuperaddRate"] = superaddRate
    result["OldSuperaddRate"] = oldSuperaddRate
    
    local equipPart = EquipMgrDB.GetEquipPartById(charID, equipID)
    if equipPart > 0 and newCurSuperaddPhase > equipCurSuperaddPhase  then
        local equipInfo =  EquipMgrDB.GetEquipAllInfo(equipID)
        result["EquipInfo"] = equipInfo
    end

    result["EquipPart"] = equipPart
    result["EquipID"] = equipID
    result["CurSuperaddPhase"] = newCurSuperaddPhase
    result["MaxSuperaddPhase"] = equipMaxSuperaddPhase
    local g_LogMgr = RequireDbBox("LogMgrDB")
    g_LogMgr.SaveEquipSuperAddationInfo( charID,equipID,newCurSuperaddPhase)
    return true, result
end


function EquipSuperaddDB.GetEquipSuperaddInfo(equipID)
    local curEquipSuperaddPhaseRet = SqlTbl._GetEquipSuperaddInfo:ExecStat(equipID)
    local curEquipSuperaddPhase = 0
    if curEquipSuperaddPhaseRet:GetRowNum() > 0 then
        curEquipSuperaddPhase = curEquipSuperaddPhaseRet:GetData(0, 0)
    end
    local superaddRate = EquipSuperaddAttrRate_Common(curEquipSuperaddPhase ,"AddAttrRate")
    local result = {}
    result[1] = curEquipSuperaddPhase
    result[2] = superaddRate
    return result
        
end

function EquipSuperaddDB.GetEquipSuperaddRate(curEquipSuperaddPhase)
    local superaddRate = EquipSuperaddAttrRate_Common(curEquipSuperaddPhase ,"AddAttrRate")
    return superaddRate
end


return EquipSuperaddDB