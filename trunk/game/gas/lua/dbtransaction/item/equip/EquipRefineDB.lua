
local g_ItemInfoMgr =   g_ItemInfoMgr
local GetEquipRefineNeedStoneCount = GetEquipRefineNeedStoneCount
local g_EquipRefineItemAdaptedPhaseInfo= g_EquipRefineItemAdaptedPhaseInfo
local EquipRefineDB   =   CreateDbBox(...)
    
local SqlTbl = {}

local function CheckEquipRefineInfo(charID, equipType, equipName, equipID, stoneType, stoneName)
    local g_RoomMgr = RequireDbBox("GasRoomMgrDB")    
    local equipBigID, equipIndex = g_RoomMgr.GetItemType(equipID)
    if equipType ~= equipBigID or equipIndex ~= equipName then
        return false, 360001 
    end
    local equipBindingType = g_RoomMgr.GetItemBindingTypeByID(equipID)
    if g_ItemInfoMgr:IsStaticEquip(equipType) == false then
        return false, 360002
    end
    
    if g_ItemInfoMgr:IsEquipRefineStone(stoneType) == false then
        return false, 360003 
    end
    
    local equipAdvanceDB = RequireDbBox("EquipAdvanceDB")
    local equipInfoTbl   = equipAdvanceDB.GetEquipAdvaceInfo(equipID)
    local count = equipInfoTbl:GetRowNum() 
    if equipInfoTbl:GetRowNum() < 1 then
        return  false, 360002
    end
    local curAdvancePhase  = equipInfoTbl:GetDataByName(0, "iea_uCurAdvancePhase")
    local advanceQuality   = equipInfoTbl:GetDataByName(0, "iea_uTotalAdvancedTimes")
    
    local EquipIntensifyDB				=	RequireDbBox("EquipIntensifyDB")
    local tbl_name, keyStr  = EquipIntensifyDB.GetSqlKeyInfoByEquipType(equipType)
    
    local sqlStat = string.format("%s%s%s%s%s%s%d","select ", keyStr, "uIntensifyQuality", " from ", tbl_name , " where is_uId =" , equipID)
	local _, intensifyQualityRet = g_DbChannelMgr:TextExecute(sqlStat)
	if intensifyQualityRet:GetRowNum() < 1 then
        return  false, 360002
    end
	local intensifyQuality = intensifyQualityRet:GetData(0, 0 )
    local adaptedPhase =  g_EquipRefineItemAdaptedPhaseInfo[stoneName]
    if intensifyQuality + 1 < adaptedPhase[1] or intensifyQuality + 1 > adaptedPhase[2] then
        return false,  188006
    end

    local suc, needStoneCount = GetEquipRefineNeedStoneCount(curAdvancePhase, advanceQuality, intensifyQuality, equipType, equipName)  
    if suc == false then
        return false,  needStoneCount
    end 
   
    local stoneCount = g_RoomMgr.GetAllItemCount(charID, stoneType, stoneName)
    if needStoneCount > stoneCount then
        return false,  360004
    end
    
    local stoneInfoRet = g_RoomMgr.GetNCountItemIdByName(charID, stoneType, stoneName, needStoneCount)
    if stoneInfoRet:GetRowNum() < needStoneCount then
        return false,  360004
    end
    return true, stoneInfoRet
end

function EquipRefineDB.EquipRefine(data)
    local charID    =   data["CharID"]
    local equipID   =   tonumber(data["EquipID"])
    local equipType =   data["EquipType"]
    local equipName =   data["EquipName"]
    local stoneType =   data["StoneType"]
    local stoneName =   data["StoneName"]

    local g_RoomMgr = RequireDbBox("GasRoomMgrDB")    
    local ret, msgID = CheckEquipRefineInfo(charID, equipType, equipName, equipID, stoneType, stoneName)
    if ret == false then
        return false, msgID 
    end
    
    local stoneInfoRet = msgID
    local stoneInfoCount = stoneInfoRet:GetRowNum()
    local g_LogMgr = RequireDbBox("LogMgrDB")
    
    local delType
    local uEventId = g_LogMgr.SetupEquipIntensifyLevelLog()
    
    local stoneInfoTbl = {}
    for i=1, stoneInfoCount do
        local stoneID = stoneInfoRet:GetData(i-1, 0) 
        local stoneRoomIndex = stoneInfoRet:GetData(i-1, 1) 
        local stonePos = stoneInfoRet:GetData(i-1, 2) 
        local suc, msgID = g_RoomMgr.DelItemByID(charID,stoneID,nil,uEventId)
        if suc ~= true then
            CancelTran()
            return false, msgID or 188003
		end
		
        table.insert(stoneInfoTbl, {stoneID, stoneRoomIndex, stonePos})
    end
    
    local EquipIntensifyDB      =	RequireDbBox("EquipIntensifyDB")
    local sqlTblName, sqlKeyStr = EquipIntensifyDB.GetSqlKeyInfoByEquipType(equipType)
    
    local sqlStat = string.format("%s%s%s%s%s%s%s%d", "update ", sqlTblName, " set ", sqlKeyStr, "uIntensifyQuality =", sqlKeyStr, "uIntensifyQuality + 1 where is_uId =", equipID)
    g_DbChannelMgr:TextExecute(sqlStat)
    if g_DbChannelMgr:LastAffectedRowNum() < 1 then
		CancelTran()
		return false, 188003
	end
	
	local EquipMgrDB = RequireDbBox("EquipMgrDB")
	local newEquipInfo = EquipMgrDB.GetEquipBaseInfoTbl(equipID)
	local result = {}
	result["NewEquipInfo"] = newEquipInfo
	result["StoneInfoTbl"] = stoneInfoTbl
	return true, result
end

SetDbLocalFuncType(EquipRefineDB.EquipRefine)

return EquipRefineDB