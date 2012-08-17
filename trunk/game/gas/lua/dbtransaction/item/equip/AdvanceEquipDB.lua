gac_gas_require "item/item_info_mgr/ItemInfoMgr"
gac_gas_require "item/Equip/EquipCommonFunc"
cfg_load "equip/advance/AttributeOfJingLingSkill_Common"
cfg_load "equip/advance/AdvancedAttrValueRate_Server"
cfg_load "equip/advance/AdvanceBackPhaseScale_Server"
cfg_load "equip/advance/JingLingTypeToPieceSkill_Common"

local LogErr = LogErr
local loadstring = loadstring
local g_ItemInfoMgr = CItemInfoMgr:new()
local AttributeOfJingLingSkill_Common = AttributeOfJingLingSkill_Common
local IsEquipAdvanceStone           = IsEquipAdvanceStone
local EquipMgrDB				=	RequireDbBox("EquipMgrDB")
local GetEquipHandType = GetEquipHandType
local g_JingLingTypeTbl = g_JingLingTypeTbl
local GetAdvancedAttribute   = GetAdvancedAttribute
local AdvancedAttrValueRate_Server = AdvancedAttrValueRate_Server
local AdvanceBackPhaseScale_Server = AdvanceBackPhaseScale_Server
local GetAdvanceStoneAdpatedLevel = GetAdvanceStoneAdpatedLevel

local JingLingTypeToPieceSkill_Common = JingLingTypeToPieceSkill_Common
local GetIntensifyNeededSoul = GetIntensifyNeededSoul
local CheckAdvancedStoneAdaptedEquip = CheckAdvancedStoneAdaptedEquip
local GetEquipPartByEquipType = GetEquipPartByEquipType
local GetAdvanceSucPorb = GetAdvanceSucPorb
local event_type_tbl = event_type_tbl

local AdvanceEquipDB = CreateDbBox(...)
local SqlTbl  = {}
local JingLingTypeTbl = {"火","风","地","水","冰"}

local AdvanceTimesAddedByUsingGangYu = 10   --使用刚玉时，增加10次进阶次数
local AdvanceTimesAddedByUsingBengYu = 100  --使用崩玉时，增加100次进阶次数
local AdvanceSucLowAdvanceLimitTimes       = 150  --当已进阶次数达到150次以后，不管之后用什么进阶石均进阶成功

--存储装备进阶失败回退到的阶段信息
local AdvanceBackPhaseScaleTbl = {}

function AdvanceEquipDB.ReadAdvanceBackPhaseScaleFromCfg(curAdvancePhase)
    local backPhaseTbl = loadstring("return " .. AdvanceBackPhaseScale_Server(curAdvancePhase,"BackPhase") )()
    local probability = loadstring("return " .. AdvanceBackPhaseScale_Server(curAdvancePhase,"Probability") )()
    AdvanceBackPhaseScaleTbl[curAdvancePhase]  = {}
    AdvanceBackPhaseScaleTbl[curAdvancePhase]["BackPhaseTbl"] = backPhaseTbl
    AdvanceBackPhaseScaleTbl[curAdvancePhase]["Probability"] = probability
end

--进阶时属性值概率表
local AdvancedAttrVauleRateTbl = {}
function AdvanceEquipDB.ReadAdvanceAttrValueRateFromCfg(equipPart)
    local rate = AdvancedAttrValueRate_Server(equipPart, "Rate")
    local randomRateRange = AdvancedAttrValueRate_Server(equipPart, "RandomRateRange")
    local rateTbl = loadstring("return " .. rate )()
    local randomRateTbl = loadstring("return " .. randomRateRange)()
    AdvancedAttrVauleRateTbl[equipPart] = {}
    table.insert(AdvancedAttrVauleRateTbl[equipPart], rateTbl)
    table.insert(AdvancedAttrVauleRateTbl[equipPart], randomRateTbl)
end

function AdvanceEquipDB.GetUpdateSkillPieceSql(haveSkillPieceNum, skillPieceTbl)
    local newSkillPieceInfo = {}
    local skillPieceSql = ""
    for i =1, haveSkillPieceNum do
        local tblLen = # skillPieceTbl
        local random = math.random(1, tblLen)
        table.insert(newSkillPieceInfo, skillPieceTbl[random])
        table.remove(skillPieceTbl, random)
        skillPieceSql = skillPieceSql .. "iea_uSkillPieceIndex" .. i .. "=" ..  newSkillPieceInfo[i].. "," 
    end
    return skillPieceSql
end

local function GetAdvancedAttrValue(advancedPhase, equipType, attrBaseValue,sName)
    local equipPart = GetEquipPartByEquipType(equipType)
    equipPart = g_ItemInfoMgr:GetItemInfo( equipType, sName,equipPart)
    if AdvancedAttrVauleRateTbl[equipPart] == nil then
        AdvanceEquipDB.ReadAdvanceAttrValueRateFromCfg(equipPart)
    end
    local attrValueInfo = AdvancedAttrVauleRateTbl[equipPart]
    local rate = attrValueInfo[1][advancedPhase]
    local randomRateTbl = attrValueInfo[2]
    local randomNo = math.random(1, # randomRateTbl[advancedPhase])
    local randomRate = randomRateTbl[advancedPhase][randomNo]
    local attrValue = string.format("%.0f", attrBaseValue * rate * randomRate)
    return attrValue 
end


local function GetAdvancedAttrValueSql(AdvancedRoot,AdvancedPhase,EquipLevel, equipType,equipName)
    local attrTbl = GetAdvancedAttribute(AdvancedRoot,AdvancedPhase,EquipLevel)   
    local index = # attrTbl
           
    local sql = ""

    for i=1, index do
        local attrName = attrTbl[i][1]
        local attrIndex = attrTbl[i][3]
        local outNewAttrValue =  GetAdvancedAttrValue(attrIndex, equipType, attrTbl[i][2],equipName)       
        --sql = sql .. "iea_uAdvancedAttrValue" .. attrIndex .." =" ..  outNewAttrValue ..","
        sql = string.format("%s%s%d%s%d%s", sql, "iea_uAdvancedAttrValue", attrIndex, " =" , outNewAttrValue, ",")
        sql = string.format("%s%s%d%s%s%s", sql, "iea_sAdvancedAttr", attrIndex, " ='" , attrName, "',")
    end
    
    return sql, index
end


--装备产出时，记录装备进阶相关属性
function AdvanceEquipDB.InsertEquipOutAdvanceInfo(data)
    local uItemId           =   data["ItemID"]
    local advancedCurLevel  =   data["AdvancedCurLevel"]  
    local advancedQuality   =   data["AdvancedQuality"]
    local charId	= data["charId"]
    local advanceSoul  = data["AdvanceSoul"]
    local random = math.random(1,5)
    local jingLingType = JingLingTypeTbl[random]
    local haveSkillPieceNum = 0

    if advancedCurLevel >= 2 and advancedCurLevel < 4 then 
        haveSkillPieceNum = 1
    elseif advancedCurLevel >= 4 and advancedCurLevel < 7 then
        haveSkillPieceNum = 2
    elseif advancedCurLevel == 7 then
        haveSkillPieceNum = 4
    end
   
    --local sqlStatement = " insert into tbl_item_equip_advance set is_uId =" .. uItemId  .. ", iea_uCurAdvancePhase = " .. advancedCurLevel .. ", iea_uTotalAdvancedTimes = " .. advancedQuality .. ","
    local sqlStatement = " insert into tbl_item_equip_advance set is_uId ="
    sqlStatement = string.format("%s%d%s%d%s%d%s", sqlStatement, uItemId, ", iea_uCurAdvancePhase = ", advancedCurLevel, ", iea_uTotalAdvancedTimes = ", advancedQuality, ",")
    
    local g_RoomMgr =RequireDbBox("GasRoomMgrDB")
    local equipType, equipName = g_RoomMgr.GetItemType(uItemId)
    local equipBindingType = g_RoomMgr.GetItemBindingTypeByID(uItemId)
    local equipLevel = EquipMgrDB.GetEquipBaseLevel(equipType, uItemId) 
    local sql  = GetAdvancedAttrValueSql(g_ItemInfoMgr:GetItemInfo(equipType, equipName,"AdvancedRoot"), advancedCurLevel, equipLevel,equipType, equipName)
    local skillPieceTbl = {1, 2, 3, 4, 5}
    local skillPieceSql = AdvanceEquipDB.GetUpdateSkillPieceSql(haveSkillPieceNum, skillPieceTbl)
    --sqlStatement = sqlStatement .. skillPieceSql  .. sql ..  " iea_sJingLingType = '" ..  jingLingType .."' , iea_sAdvanceSoul = '" .. advanceSoul .. "'"
    sqlStatement = string.format("%s%s%s%s%s%s%s%s", sqlStatement, skillPieceSql, sql, " iea_sJingLingType = '", jingLingType, "' , iea_sAdvancedSoulRoot = '", advanceSoul, "'")
 
    g_DbChannelMgr:TextExecute(sqlStatement)
    if g_DbChannelMgr:LastAffectedRowNum() < 1 then
		CancelTran()
		return false
	else
		local newEquipAdvanceTbl = AdvanceEquipDB.GetEquipAdvaceInfo(uItemId)
		local g_LogMgr = RequireDbBox("LogMgrDB")
		g_LogMgr.SaveEquipAdvanceInfo( charId,uItemId,newEquipAdvanceTbl)
	    return true
	end
end

local StmtDef ={
    "_SelectAdvancedSkillPiece",
    [[
        select 
            ifnull(iea_uSkillPieceIndex1, 0) , ifnull(iea_uSkillPieceIndex2, 0), ifnull(iea_uSkillPieceIndex3, 0), ifnull(iea_uSkillPieceIndex4, 0), ifnull(iea_uChoosedSkillPieceIndex, 0) 
        from 
            tbl_item_equip_advance 
        where 
            is_uId = ?
    ]]
}
DefineSql(StmtDef, SqlTbl)

--得到装备的精灵技能碎片信息
function AdvanceEquipDB.GetEquipAdvancedSkillPieceInfo(equipID)
    local ret = SqlTbl._SelectAdvancedSkillPiece:ExecStat(equipID)
    return ret
end

local StmtDef = {
    "_SelectEquipAdvanceLevel",
    [[
        select iea_uCurAdvancePhase , iea_uTotalAdvancedTimes, iea_sAdvancedSoulRoot from tbl_item_equip_advance where is_uId = ?
    ]]
}
DefineSql(StmtDef, SqlTbl)

function AdvanceEquipDB.GetEquipCurAdvancedPhase(equipID)
    local ret = SqlTbl._SelectEquipAdvanceLevel:ExecStat(equipID)
    local equipCurAdvancedPhase = 0
    if ret ~= nil and ret:GetRowNum() > 0 then
       equipCurAdvancedPhase = ret:GetData(0,0) 
    end
    return equipCurAdvancedPhase
end

--更新进阶技能碎片信息
function AdvanceEquipDB.UpdateAdvancedSkillPiece(advancedCurLevel, oldEquipAdvanceSkillPieceTbl, equipID, AdvancedRoot,EquipLevel, equipType, equipName)
    local skillPieceIndexTbl = {}
    for j=1, 5 do
        local bFlag = true
        local colNum = oldEquipAdvanceSkillPieceTbl:GetColNum()
        local ret = oldEquipAdvanceSkillPieceTbl:GetRowSet(1)
        for i=1, colNum -1 do
            if ret(i) == j then
                  bFlag = false
            end
        end
        if bFlag then
            table.insert(skillPieceIndexTbl, j)   
        end
    end
    local maxNumOfUnsedSkillPiece = # skillPieceIndexTbl
    local randomNum = math.random(1, maxNumOfUnsedSkillPiece)
    local choosedSkillPieceNo =skillPieceIndexTbl[randomNum]
    local index = 5 - maxNumOfUnsedSkillPiece + 1 --当前应该更新的skillPiece no
    local sql 
    local attrSql = GetAdvancedAttrValueSql(AdvancedRoot,advancedCurLevel + 1,EquipLevel, equipType, equipName)
    if advancedCurLevel < 6 then
        sql= "update tbl_item_equip_advance set " .. "iea_uSkillPieceIndex" .. index .. " =" .. choosedSkillPieceNo 
        sql = sql .. "," .. attrSql .. "iea_uCurAdvancePhase = iea_uCurAdvancePhase + 1" .. " where is_uId =" .. equipID
    else
        table.remove(skillPieceIndexTbl, randomNum)
        local secondRandom = math.random(1, maxNumOfUnsedSkillPiece-1)
        local secondSkillPieceNo= skillPieceIndexTbl[secondRandom]
        sql = "update tbl_item_equip_advance set " .. "iea_uSkillPieceIndex" .. index .. "= " .. choosedSkillPieceNo
        sql = sql .. ", iea_uSkillPieceIndex" .. index + 1 .. " = " .. secondSkillPieceNo .. "," .. attrSql .. "iea_uCurAdvancePhase = iea_uCurAdvancePhase + 1" ..   " where is_uId =" .. equipID
    end
    g_DbChannelMgr:TextExecute(sql)
    if g_DbChannelMgr:LastAffectedRowNum() < 1 then
		CancelTran()
		return false, 185003
	end 
    return true
end

local StmtDef = {
    "_UpdateCurAdvancePhase",
    [[
        update tbl_item_equip_advance set iea_uCurAdvancePhase  = iea_uCurAdvancePhase  + 1 where is_Uid = ?
    ]]
}
DefineSql(StmtDef, SqlTbl)

local StmtDef = {
    "_UpdateCurBackAdvanceSkill1",
    [[
        update 
            tbl_item_equip_advance 
        set 
            iea_uSkillPieceIndex1 =0, iea_uSkillPieceIndex2 =0, iea_uSkillPieceIndex3=0,
            iea_uSkillPieceIndex4=0, iea_uChoosedSkillPieceIndex =0,  iea_uAdvancedAttrValue2 =0,
            iea_uAdvancedAttrValue3 =0, iea_uAdvancedAttrValue4 = 0, iea_uCurAdvancePhase=?, iea_uAdvancedAttrValue1= ?
        where
            is_uId = ?
    ]]
}
DefineSql(StmtDef, SqlTbl)

local StmtDef ={
    "_UpdateCurBackAdvanceSkill2",
    [[
        update tbl_item_equip_advance  set iea_uSkillPieceIndex2 =0, iea_uSkillPieceIndex3=0, iea_uSkillPieceIndex4=0,
             iea_uCurAdvancePhase=?
        where
            is_uId = ?
    ]]
}
DefineSql(StmtDef, SqlTbl)

local function GetAdvanceBackUpAttrValueSql(AdvancedRoot,curAdvancedPhase,EquipLevel, equipType, equipName)
    local sql, index = GetAdvancedAttrValueSql(AdvancedRoot,curAdvancedPhase,EquipLevel, equipType, equipName)
    if index > 4 then
       LogErr("进阶属性超过4个", "AdvancedRoot:" .. AdvancedRoot .. ",curAdvancedPhase" .. curAdvancedPhase)
       index = 4 
    end
    if index == 1 then
        sql = sql .. "iea_uAdvancedAttrValue2= 0, iea_uAdvancedAttrValue3 = 0 , iea_uAdvancedAttrValue4= 0,"
    elseif index == 2 then
        sql = sql .. "iea_uAdvancedAttrValue3 = 0 , iea_uAdvancedAttrValue4= 0,"
    elseif index == 3 then
        sql = sql .. "iea_uAdvancedAttrValue4= 0,"
    end
    
    return sql
end

function AdvanceEquipDB.GetEquipBackPhase(curAdvancedPhase)
    local backPhase = 1
    local startProb = 0
    local random = math.random(1, 100)
    if AdvanceBackPhaseScaleTbl[curAdvancedPhase] == nil then
        AdvanceEquipDB.ReadAdvanceBackPhaseScaleFromCfg(curAdvancedPhase)
    end
    for i, v in pairs(AdvanceBackPhaseScaleTbl[curAdvancedPhase]["Probability"]) do
        local upProb = startProb + v
        if startProb <= random and random <= upProb then
            backPhase = AdvanceBackPhaseScaleTbl[curAdvancedPhase]["BackPhaseTbl"][i]
            break
        end
        startProb = startProb + v
    end
    
    return backPhase
end
                                                  
function AdvanceEquipDB.EquipAdvanceBack(equipID, advancedCurLevel, oldEquipAdvanceSkillPieceRet,AdvancedRoot,equipBaseLevel, equipType, equipName)
    local oldEquipAdvanceSkillPieceTbl = oldEquipAdvanceSkillPieceRet:GetRowSet(1)
    local choosedSkillPiece =  oldEquipAdvanceSkillPieceTbl(4)
    local choosedSkillPieceNum =0
    if choosedSkillPiece > 0 then
        for i=1, 4 do
            if choosedSkillPiece == oldEquipAdvanceSkillPieceTbl(i) then
                choosedSkillPieceNum = i
            end
        end
    end
    local phaseBackFlag = true
    local backedAdvancePhase = AdvanceEquipDB.GetEquipBackPhase(advancedCurLevel)
    if backedAdvancePhase == advancedCurLevel then
        return true, 185024, 0 --进阶不回退的时候的提示信息
    end
    
    local sql 
    local needUpdate = false 
    if backedAdvancePhase < 2 then
        if oldEquipAdvanceSkillPieceTbl(1) > 0 or  oldEquipAdvanceSkillPieceTbl(2) > 0
            or oldEquipAdvanceSkillPieceTbl(3) > 0 or oldEquipAdvanceSkillPieceTbl(4) > 0 then
            local attrTbl = GetAdvancedAttribute(AdvancedRoot,backedAdvancePhase,equipBaseLevel)
            local attr1Value =  GetAdvancedAttrValue(backedAdvancePhase, equipType, attrTbl[1][2], equipName)
            SqlTbl._UpdateCurBackAdvanceSkill1:ExecStat(backedAdvancePhase, attr1Value, equipID)
            needUpdate =true         
        end
    elseif backedAdvancePhase >= 2  then
        if backedAdvancePhase < 4 then
            sql = "update tbl_item_equip_advance  set iea_uSkillPieceIndex2 =0, iea_uSkillPieceIndex3=0, iea_uSkillPieceIndex4=0,"
        else
            sql = "update tbl_item_equip_advance  set iea_uSkillPieceIndex3=0, iea_uSkillPieceIndex4=0,"
        end
        if choosedSkillPieceNum > 1 then
           sql = sql .. "iea_uChoosedSkillPieceIndex =0,"
        end
        sql = sql .. GetAdvanceBackUpAttrValueSql(AdvancedRoot,backedAdvancePhase,equipBaseLevel, equipType, equipName)
        sql = sql .. "iea_uCurAdvancePhase=" .. backedAdvancePhase .. " where is_uId = " .. equipID
        g_DbChannelMgr:TextExecute(sql)
        needUpdate =true
    end
    if needUpdate then
        if g_DbChannelMgr:LastAffectedRowNum() < 1 then
            CancelTran()
            return false, 185006
        end
    end
    
    return true,185025, backedAdvancePhase
end


local StmtDef = {
    "_UpdateAdvancedTimes",
    [[
        update tbl_item_equip_advance set iea_uAdvancedTimes = iea_uAdvancedTimes + ?, iea_uAdvanceSoulNum = 0 where is_uId =?
    ]]
}
DefineSql(StmtDef, SqlTbl)

local function GetEquipAdvanceSucProbility(addSucProb, advancedCurLevel, equipBaseLevel, equipAdvancedTimes)
    if equipAdvancedTimes >= AdvanceSucLowAdvanceLimitTimes then
	    return true
    end
	
	local stoneAddSucProbability = addSucProb + GetAdvanceSucPorb(advancedCurLevel, equipBaseLevel)
	
	local bAdvanceSucFlag = false
    local randomSucProb = stoneAddSucProbability
    local randomSucNum = math.random(1, 100) /100
    if randomSucNum <= randomSucProb then
        bAdvanceSucFlag = true
    end    
    
    if stoneAddSucProbability >= 1 or bAdvanceSucFlag or advancedCurLevel <= 1 then
        return true
    else
        return false
    end
end

--装备进阶函数
function AdvanceEquipDB.EquipAdvance(data)
    local nCharID           =   data["CharID"]
    local equipID           =   data["EquipID"]
    local advancedStoneRoom =   data["RoomIndex1"]
    local advancedStonePos  =   data["Pos1"]
    local equipPart         =   data["EquipPart"]

    local part =  EquipMgrDB.GetEquipPartById(nCharID,equipID)
    if part ~= equipPart then
        local result = {["MsgID"] =185022, ["EquipID"] =equipID} 
        return false, result 
    end
    
    local g_RoomMgr =   RequireDbBox("GasRoomMgrDB")
    local stoneType, stoneName, stoneID, stoneBindingType =   g_RoomMgr.GetOneItemByPosition(nCharID,advancedStoneRoom,advancedStonePos)
    if stoneType == nil or stoneName == nil then
        local result = {["MsgID"] =185000, ["EquipID"] =equipID}  
        return false, result 
    end

    if g_ItemInfoMgr:IsAdvanceStone(stoneType) == false  then
        local result = {["MsgID"] =185000, ["EquipID"] =equipID}  
        return false, result   
    end
    
    local stoneInfo = g_ItemInfoMgr:GetItemFunInfo(stoneType, stoneName)
    if stoneInfo == nil then
        local result = {["MsgID"] =185000, ["EquipID"] =equipID}  
        return false, result 
    end
    
    local advanceLevelInfo = SqlTbl._SelectEquipAdvanceLevel:ExecStat(equipID)
    if  advanceLevelInfo:GetRowNum() < 1 then
        local result = {["MsgID"] =185001, ["EquipID"] =equipID}  
        return false, result    --进阶所需等级信息不存在
    end
    local advancedCurLevel = advanceLevelInfo:GetDataByName(0, "iea_uCurAdvancePhase")
    local advancedMaxLevel = advanceLevelInfo:GetDataByName(0, "iea_uTotalAdvancedTimes")
    local advanceSoul   =   advanceLevelInfo:GetDataByName(0, "iea_sAdvancedSoulRoot")
    if advancedCurLevel >= advancedMaxLevel or  advancedCurLevel > 6 then
        local result = {["MsgID"] =185002, ["EquipID"] =equipID}  
        return false, result 
    end
    
    local stoneAdaptedLevel = GetAdvanceStoneAdpatedLevel(stoneName)
    if advancedCurLevel < stoneAdaptedLevel[1]  or advancedCurLevel > stoneAdaptedLevel[2] then
       local result = {["MsgID"] =185004, ["EquipID"] =equipID} 
       return false,  result
    end
    
    
    local equipType, equipName = g_RoomMgr.GetItemType(equipID)
    local equipBindingType = g_RoomMgr.GetItemBindingTypeByID(equipID)
    local equipBaseLevel = EquipMgrDB.GetEquipBaseLevel(equipType, equipID)
    local stoneAdaptedEquipPart = CheckAdvancedStoneAdaptedEquip(equipType, equipName, stoneName, equipBaseLevel, advancedCurLevel)
    if stoneAdaptedEquipPart == false then
        local result = {["MsgID"] =185020, ["EquipID"] =equipID} 
        return false,  result   
    end
        
    local ret1 = g_RoomMgr.DelItemByID(nCharID, stoneID,event_type_tbl["物品使用"])
	if (not ret1) then
		CancelTran()
		local result = {["MsgID"] =185000, ["EquipID"] =equipID}  
		return false, result
	end
    
    local ret, totalSoulNum = AdvanceEquipDB.DelEquipAdvancedSoul(nCharID, equipBaseLevel, g_ItemInfoMgr:GetItemInfo(equipType, equipName,"EquipType"), "AdvanceVanishSoulCount")

    if ret == false then
       CancelTran()
       local errMsgID = totalSoulNum
       local result= {["MsgID"] =errMsgID, ["EquipID"] =equipID}  
       return ret,  result
    end
    
    local stoneID2, advancedStoneRoom2, advancedStonePos2
    local stoneAddSucProb2 = 0
    if equipType == 5 and GetEquipHandType(g_ItemInfoMgr:GetItemInfo(equipType, equipName,"EquipType")) == "双" then   --如果是双手武器，需要消耗两个进阶石
        
        local stone2InfoTbl = g_RoomMgr.GetNCountItemIdByName(nCharID, stoneType, stoneName, 1)
        if stone2InfoTbl:GetRowNum() == 0 then
            CancelTran()
            local result = {["MsgID"] =185000, ["EquipID"] =equipID}  
            return false, result
        end
        stoneID2, advancedStoneRoom2, advancedStonePos2 = stone2InfoTbl(1,1),stone2InfoTbl(1,2), stone2InfoTbl(1,3)
        stoneAddSucProb2 = stoneInfo("AddSucProbability")
    end
   
    local oldEquipAdvanceTbl = AdvanceEquipDB.GetEquipAdvaceInfo(equipID)
    local equipAdvancedTimes = oldEquipAdvanceTbl:GetDataByName(0, "iea_uAdvancedTimes")

    if stoneID2 ~= nil then
        local ret1 = g_RoomMgr.DelItemByID(nCharID, stoneID2,event_type_tbl["物品使用"])
    	if (not ret1) then
    		CancelTran()
    		local result = {["MsgID"] =185000, ["EquipID"] =equipID}  
    		return false, result
	    end 
    end
    local oldEquipAdvanceSkillPieceTbl = AdvanceEquipDB.GetEquipAdvancedSkillPieceInfo(equipID)

    local advanceSucMsgId = 0
    local notBackStone = nil
    local messageID = 185023 --进阶成功提示信息
    local backPhase = 0
    
    local addSucProb = stoneInfo("AddSucProbability")
    local sucProbability = GetEquipAdvanceSucProbility(addSucProb, advancedCurLevel, equipBaseLevel, equipAdvancedTimes)
    
    local equipAdvanceTimesAdded = 1
    if addSucProb == 1 then
        equipAdvanceTimesAdded = 100
    end
    
    if stoneInfo("NotBack") == 1 then
        equipAdvanceTimesAdded = 10
    end
    
	if sucProbability == true then  --一定能成功进阶
    	if advancedCurLevel == 1 or advancedCurLevel == 3 or advancedCurLevel == 6 then
            local ret, msgID = AdvanceEquipDB.UpdateAdvancedSkillPiece(advancedCurLevel, oldEquipAdvanceSkillPieceTbl, equipID, advanceSoul,equipBaseLevel, equipType, equipName)
            if ret ~= true then
                local result = {["MsgID"] =msgID, ["EquipID"] =equipID}  
                return false,  result
            end
        else
            local sqlStat = "update tbl_item_equip_advance set  "
            local sql = GetAdvancedAttrValueSql(advanceSoul, advancedCurLevel+1,equipBaseLevel, equipType, equipName)
            sqlStat = sqlStat .. sql .." iea_uCurAdvancePhase = iea_uCurAdvancePhase +1 where is_uId =" .. equipID
            g_DbChannelMgr:TextExecute(sqlStat)
            if g_DbChannelMgr:LastAffectedRowNum() < 1 then
        		CancelTran()
        		local result = {["MsgID"] =185003, ["EquipID"] =equipID}  
        		return false, result
    	    end 
        end
        if advancedCurLevel == 3 then
            messageID = 185026
        end
        if advancedCurLevel == 6  then
            messageID = 185027
        end

    elseif stoneInfo("NotBack") ~= 1 then            --进阶失败，回退
        --local backPhaseNum = math.random(1, advancedCurLevel-1)
        local ret, msgID, backPhaseNum =  AdvanceEquipDB.EquipAdvanceBack(equipID, advancedCurLevel, oldEquipAdvanceSkillPieceTbl, advanceSoul,equipBaseLevel, equipType, equipName)
        if ret ~= true then
            local result = {["MsgID"] =msgID, ["EquipID"] =equipID}  
            return false, result
        end     
        messageID = msgID
        backPhase = backPhaseNum
    elseif stoneInfo("NotBack") == 1 then    
        messageID = 185024
    end
    
    SqlTbl._UpdateAdvancedTimes:ExecStat(equipAdvanceTimesAdded, equipID)
    if g_DbChannelMgr:LastAffectedRowNum() < 1 then
		CancelTran()
		local result = {["MsgID"] =185003, ["EquipID"] =equipID}  
		return false, result
    end
    local equipDuraState, curDuraValue, maxDuraValue 
    if equipPart ~= 0 then
        local EquipDurabilityDB = RequireDbBox("EquipDurabilityDB")
        equipDuraState, curDuraValue, maxDuraValue = EquipDurabilityDB.GetEquipUllageState(equipID)
    end
    local newEquipAdvanceTbl = AdvanceEquipDB.GetEquipAdvaceInfo(equipID)
    local EquipSuperaddDB = RequireDbBox("EquipSuperaddDB")
    local equipSuperaddInfo =  EquipSuperaddDB.GetEquipSuperaddInfo(equipID)
    local result = {["EquipID"] = equipID, ["EquipPart"] = equipPart, ["RoomIndex"] =advancedStoneRoom , ["Pos"] =advancedStonePos,
                    ["StoneID"] = stoneID ,  ["OldEquipAdvanceSkillPieceTbl"]=oldEquipAdvanceTbl, 
                    ["NewEquipAdvanceSkillPieceTbl"] = newEquipAdvanceTbl,
                    ["EquipLevel"] =equipBaseLevel,
                    ["RoomIndex2"] = advancedStoneRoom2, ["Pos2"]= advancedStonePos2, ["StoneID2"] =stoneID2,
                    ["EquipType"] = equipType, ["EquipName"] = equipName, ["TotalSoulNum"]= totalSoulNum,
                    ["CurDuraValue"] = curDuraValue, ["MaxDuraValue"] = maxDuraValue,
                    ["EquipSuperaddInfo"] = equipSuperaddInfo}
    result["MessageID"] = messageID
    result["BackPhase"] = backPhase
	local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.SaveEquipAdvanceInfo( nCharID,equipID,newEquipAdvanceTbl)
	return true, result
end

local StmtDef = {
    "_UpdateAdvancedSkillPiece",
    [[
        update tbl_item_equip_advance set iea_uChoosedSkillPieceIndex = ? where is_uId = ?
    ]]
}   
DefineSql(StmtDef, SqlTbl)

function AdvanceEquipDB.SetChoosedAdvancedSkillPiece(data)
    local equipID       =   data["EquipID"]
    local skillPieceNo  =   data["SkillPieceNo"]
    local uCharId				=		data["uCharId"]
    SqlTbl._UpdateAdvancedSkillPiece:ExecStat(skillPieceNo, equipID)
    if g_DbChannelMgr:LastAffectedRowNum() < 1 then
		CancelTran()
		return false, 185005
	end 
	result = {["EquipID"] = equipID, ["ChoosedSkillPiece"] = skillPieceNo}
	local newEquipAdvanceTbl = AdvanceEquipDB.GetEquipAdvaceInfo(equipID)
	local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.SaveEquipAdvanceInfo( uCharId,equipID,newEquipAdvanceTbl)
  return true, result
end

local StmtDef = {
    "_GetEquipAdvanceInfo",
    [[  
        select 
            iea_uCurAdvancePhase,iea_uTotalAdvancedTimes,ifnull(iea_uSkillPieceIndex1, 0),ifnull(iea_uSkillPieceIndex2,0),
            ifnull(iea_uSkillPieceIndex3,0) ,ifnull(iea_uSkillPieceIndex4,0), ifnull(iea_uChoosedSkillPieceIndex,0),
            iea_sJingLingType, iea_uAdvanceSoulNum, iea_uAdvancedTimes, 
            iea_uAdvancedAttrValue1, iea_uAdvancedAttrValue2, iea_uAdvancedAttrValue3, iea_uAdvancedAttrValue4,
            iea_sAdvancedSoulRoot, iea_sAdvancedAttr1, iea_sAdvancedAttr2, iea_sAdvancedAttr3, iea_sAdvancedAttr4
        from 
            tbl_item_equip_advance
        where 
            is_uId = ?
    ]]
}
DefineSql(StmtDef, SqlTbl)

--根据装备id，查询装备的进阶属性信息
function AdvanceEquipDB.GetEquipAdvaceInfo(equipID)
    local ret = SqlTbl._GetEquipAdvanceInfo:ExecStat(equipID)
    return ret
end

function AdvanceEquipDB.DelEquipAdvancedSoul(charID, curLevel, EquipType, vanishSoulType)
  local uSoulNum = GetIntensifyNeededSoul(curLevel, EquipType, vanishSoulType)
	
	local param = {["soulCount"] = -uSoulNum,["PlayerId"] = charID,["addSoulType"] = event_type_tbl["装备进阶魂"]}
	local bFlag,total_soul = EquipMgrDB.ModifyPlayerSoul(param)
	if not bFlag then
		local errMsgID = 185018
		CancelTran()
		return false,errMsgID
	end
	
	return true, total_soul
end

local StmtDef = {
    "_SelectCurSkillPiece",
    [[
        select ifnull(iea_uChoosedSkillPieceIndex, 0) from tbl_item_equip_advance where is_uId =?
    ]]
}
DefineSql(StmtDef, SqlTbl)

local StmtDef = {
    "_UpdateActiveSkillPiece",
    [[
        update tbl_item_equip_advance set iea_uChoosedSkillPieceIndex =? where is_uId =?
    ]]
}
DefineSql(StmtDef, SqlTbl)

--激活技能精灵碎片;如果已有技能激活项，则覆盖
function AdvanceEquipDB.ActiveJingLingSkillPiece(data)
    local charID                =   data["CharID"]
    local equipID               =   data["EquipID"]
    local choosedActiveIndex    =   data["ChoosedActiveIndex"]
    local equipPart             =   data["EquipPart"]
    if choosedActiveIndex > 5 or choosedActiveIndex < 1 then    --技能激活项不合法
        return false, 185008 
    end
    
    local curSkillPieceIndexRet    =   SqlTbl._SelectCurSkillPiece:ExecStat(equipID)
    local curSkillPieceIndex = curSkillPieceIndexRet:GetData(0, 0)
    if choosedActiveIndex == curSkillPieceIndex then
        return false, 185007
    end
    
    local suc, totalSoulNum =AdvanceEquipDB.VanishedSoul("ActiveJingLingPieceSoul", charID, equipID)
    if suc ~= true then
       local errMsgID = totalSoulNum
       return false,  errMsgID
    end

    SqlTbl._UpdateActiveSkillPiece:ExecStat(choosedActiveIndex, equipID)
    if g_DbChannelMgr:LastAffectedRowNum() < 1 then
		CancelTran()
		return false, 185009
	end
	local activeJingLingSkill = 0
    if equipPart ~= 0 then
        local suc, jingLingSkillIndex = AdvanceEquipDB.GetRemoveJingLingSkillIndex(charID)
        if jingLingSkillIndex == -1 then
            CancelTran()
            return false, 185007
        end
        activeJingLingSkill = jingLingSkillIndex
    end

	local result = {["EquipID"] = equipID, ["ChoosedActiveIndex"]=choosedActiveIndex, ["DelJingLingSkillIndex"]= activeJingLingSkill,
	                ["TotalSoulNum"]= totalSoulNum}
	local newEquipAdvanceTbl = AdvanceEquipDB.GetEquipAdvaceInfo(equipID)
	local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.SaveEquipAdvanceInfo( charID,equipID,newEquipAdvanceTbl)
	return true, result
end

local StmtDef = {
    "_GetJingLingType",
    [[
        select iea_sJingLingType from tbl_item_equip_advance where is_uId = ?
    ]]
}
DefineSql(StmtDef, SqlTbl)

function AdvanceEquipDB.VanishedSoul(vanishType, charID, equipID)
    local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
    local equipType, equipName = g_RoomMgr.GetItemType(equipID)
		local equipBindingType = g_RoomMgr.GetItemBindingTypeByID(equipID)
	local equipBaseLevel = EquipMgrDB.GetEquipBaseLevel(equipType, equipID) 
	local suc, totalSoulNum = AdvanceEquipDB.DelEquipAdvancedSoul(charID, equipBaseLevel, g_ItemInfoMgr:GetItemInfo(equipType, equipName,"EquipType"), vanishType)
    return suc, totalSoulNum
end

--装备技能碎片重生
function AdvanceEquipDB.AdvancedEquipReborn(data)
    local equipID   =   data["EquipID"]
    local equipPart =   data["EquipPart"]
    local charID                =   data["CharID"]

    local skillPieceRet = SqlTbl._SelectAdvancedSkillPiece:ExecStat(equipID) 
    if skillPieceRet:GetRowNum() < 1 then
       return false, 185010 
    end
    local suc, totalSoulNum
    local curAdvancePhase = AdvanceEquipDB.GetEquipCurAdvancedPhase(equipID)   
    if curAdvancePhase >= 2 then --进阶到2阶以后，装备重生才消耗魂值
        suc, totalSoulNum =AdvanceEquipDB.VanishedSoul("EquipRebornVanishSoul", charID, equipID)
        if suc ~= true then
           local errMsgID = totalSoulNum
           return false,  errMsgID
        end
    end
    
    local skillPieceTbl = skillPieceRet:GetRowSet(1)
    local haveSkillPieceNum = 0
    local baseSkillPieceTbl = {1, 2, 3, 4, 5}
    local newSkillPieceTbl = {}
    local oldSkillPieceTbl = {}
    for i=1, 4 do
        if skillPieceTbl(i) > 0 then
            haveSkillPieceNum = haveSkillPieceNum + 1
            baseSkillPieceTbl[skillPieceTbl(i)] = nil
            table.insert(oldSkillPieceTbl, skillPieceTbl(i))
        end
    end
    for i, v in pairs(baseSkillPieceTbl) do
       table.insert(newSkillPieceTbl, v) 
    end
    local needSkillPiece = haveSkillPieceNum - # newSkillPieceTbl
    for i= 1, needSkillPiece do
        local skillPiece = oldSkillPieceTbl[i]
        table.insert(newSkillPieceTbl, skillPiece)
    end
        
    local jingLingTypeRet = SqlTbl._GetJingLingType:ExecStat(equipID)
    local jingLingTypeOld = jingLingTypeRet:GetData(0, 0)
    local newCouldChoosedJingLingType = {"火","风","地","水","冰"}
    for i, v in pairs(newCouldChoosedJingLingType) do
        if jingLingTypeOld == v then
            table.remove(newCouldChoosedJingLingType, i)
            break
        end
    end
    local random = math.random(1, # newCouldChoosedJingLingType)
    local jingLingType = newCouldChoosedJingLingType[random]
    
    local skillPieceSql = AdvanceEquipDB.GetUpdateSkillPieceSql(haveSkillPieceNum, newSkillPieceTbl)

    local sql = "update tbl_item_equip_advance set iea_sJingLingType = '"  .. jingLingType  .. "'," .. skillPieceSql .. "iea_uChoosedSkillPieceIndex = 0 where is_uId= " .. equipID
    g_DbChannelMgr:TextExecute(sql)
    if g_DbChannelMgr:LastAffectedRowNum() < 1 then
		CancelTran()
		return false, 185011
	end
    local upActiveJingLingSkillFlag = false
    local ret = SqlTbl._SelectActiveJingLingSkill:ExecStat(charID)
    local oldActiveJingLingSkill = 0
    if ret ~= nil and ret:GetRowNum() > 0 then
        oldActiveJingLingSkill = ret:GetDataByName(0, "iea_uActiveSkillIndex")
    end

    if ret:GetRowNum() > 0 and oldActiveJingLingSkill ~= 0 then
        local suc, skillPieceTotalCount = AdvanceEquipDB.GetCharSkillPieceCountBySkill(oldActiveJingLingSkill, charID)
        if suc == false then
            LogErr("精灵技能信息错误,精灵类型：", oldActiveJingLingSkill .. "玩家ID:" .. charID)
            --CancelTran()
            --return false, 185011
        end
        if suc == false or AttributeOfJingLingSkill_Common(oldActiveJingLingSkill,"NeedPieceCount") > skillPieceTotalCount then           
            SqlTbl._UpdateActiveJingLingSkill:ExecStat(0, charID)
            if g_DbChannelMgr:LastAffectedRowNum() < 1 then
    		    CancelTran()
    		    return false, 185011
    		end
    		upActiveJingLingSkillFlag = true
        end
	end 
    

	local newAdvanceInfoTbl = AdvanceEquipDB.GetEquipAdvaceInfo(equipID)
	local result = {["NewAdvanceInfo"] =newAdvanceInfoTbl, ["EquipID"] = equipID, ["EquipPart"] =equipPart,
	                ["OldActiveJingLingSkill"] = oldActiveJingLingSkill,
	                ["TotalSoulNum"]= totalSoulNum, ["CurAdvancePhase"] =curAdvancePhase,
	                ["UpActiveJingLingSkillFlag"] = upActiveJingLingSkillFlag}
	local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.SaveEquipAdvanceInfo( charID,equipID,newAdvanceInfoTbl)
	return true, result
end


local StmtDef = {
    "_SelectActiveJingLingSkill",
    [[
        select iea_uActiveSkillIndex from tbl_char_equip_advancedActiveSkill where cs_uId =?
    ]]
}
DefineSql(StmtDef, SqlTbl)

local StmtDef ={
    "_InsertActiveJingLingSkill",
    [[
        insert into tbl_char_equip_advancedActiveSkill set cs_uId= ?, iea_uActiveSkillIndex = ?
    ]]
}
DefineSql(StmtDef, SqlTbl)

local StmtDef = {
    "_UpdateActiveJingLingSkill",
    [[
        update tbl_char_equip_advancedActiveSkill set iea_uActiveSkillIndex = ? where cs_uId= ?
    ]]
}
DefineSql(StmtDef, SqlTbl)
--更新当前激活的技能
function AdvanceEquipDB.ActiveJingLingSkill(data)
    local charID                =   data["CharID"]
    local newActiveSkillIndex    =   data["NewSkillIndex"]

    local skillIndexRet = SqlTbl._SelectActiveJingLingSkill:ExecStat(charID)
    local skillIndex = 0
    if skillIndexRet ~=nil and skillIndexRet:GetRowNum() > 0 then
        skillIndex = skillIndexRet:GetDataByName(0, "iea_uActiveSkillIndex")
    end
    if skillIndex == newActiveSkillIndex then
        return false, 185012
    end
    local suc, skillPieceTotalCount = AdvanceEquipDB.GetCharSkillPieceCountBySkill(newActiveSkillIndex, charID)
    if suc == false then
        return false, 185012
    end
    if skillPieceTotalCount < AttributeOfJingLingSkill_Common(newActiveSkillIndex,"NeedPieceCount") then
        return false, 185012
    end
    if skillIndexRet ~= nil and skillIndexRet:GetRowNum() > 0 then
        SqlTbl._UpdateActiveJingLingSkill:ExecStat(newActiveSkillIndex, charID)
    else
        SqlTbl._InsertActiveJingLingSkill:ExecStat(charID, newActiveSkillIndex)
    end
    if g_DbChannelMgr:LastAffectedRowNum() < 1 then
		CancelTran()
		return false, 185012
	end
	local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.SaveEquipAdvancedActiveSkill( charID,newActiveSkillIndex)
	local result = {["NewActiveSkillIndex"] =newActiveSkillIndex, ["OldActiveSkillIndex"]= skillIndex }
	return true, result
end

local StmtDef ={
    "_GetSkillPieceCountByTypes",
    [[
        select count(advance.iea_uChoosedSkillPieceIndex ), advance.iea_uChoosedSkillPieceIndex, advance.iea_sJingLingType
        from tbl_item_equip_advance as advance, tbl_item_equip as equip
        where advance.is_uId = equip.is_uId  and 
        advance.iea_uChoosedSkillPieceIndex > 0 and equip.cs_uId = ?
        group by advance.iea_uChoosedSkillPieceIndex, advance.iea_sJingLingType
    ]]
}
DefineSql(StmtDef, SqlTbl)

--得到玩家身上所有激活的技能碎片项
function AdvanceEquipDB.GetAdvancedSkillPieceInfo(charID)
    local ret  = SqlTbl._GetSkillPieceCountByTypes:ExecStat(charID)
    return ret
end

--根据当前激活的精灵技能所需的精灵类型和技能碎片类型，查找装备相关信息
local StmtDef = {
    "_GetDelEquipByActiveSkill",
    [[  
        select equip.iea_uChoosedSkillPieceIndex from tbl_item_equip_advance as equip 
        where equip.is_uId = ? and equip.iea_sJingLingType = ? and equip.iea_uChoosedSkillPieceIndex =?
    ]]
}
DefineSql(StmtDef, SqlTbl)

--根据精灵技能索引值，查找玩家身上当前所含组成该技能的碎片总值
--返回值-1表示激活技能对应的技能碎片信息错误
--返回值0表示当前所含的符合技能的技能碎片数目为0
function AdvanceEquipDB.GetCharSkillPieceCountBySkill(jingLingSkillIndex, charID)
    --查找符合精灵技能的技能碎片项名称
    local skillParam = AttributeOfJingLingSkill_Common(jingLingSkillIndex)
    
    if skillParam == nil then          
        return false, -1 
    end
    local jingLingPieceName = skillParam("SkillPieceName")
    local jingLingType = g_JingLingTypeTbl[jingLingPieceName]

    local JingLingSkillPieceIndex = nil
    local skillPieceCfgInfo = JingLingTypeToPieceSkill_Common(jingLingType)
    --根据精灵技能项的名称得到数据库中对应存储的index
    for i=1, 5 do 
        local skillPieceNameInCfg = "SkillPieceName" .. i
        if  skillPieceCfgInfo(skillPieceNameInCfg) == jingLingPieceName then
            JingLingSkillPieceIndex = i
            break
        end            
    end
    if JingLingSkillPieceIndex == nil then
        return false, -1 
    end
    
    local skillPieceTotalCount = 0
    local ret = AdvanceEquipDB.GetAdvancedSkillPieceInfo(charID)
    local rowNum = ret:GetRowNum()
    local skillPieceCountByTypeTbl = ret:GetTableSet()
    --根据精灵类型、技能碎片项索引值得到玩家身上所有该技能碎片的总值
    for i=1, rowNum do
        local jingLingType1 = skillPieceCountByTypeTbl(i, 3)--[i][3]
        local skillPieceIndex = skillPieceCountByTypeTbl(i, 2)--[i][2]
        local skillPieceCount =  skillPieceCountByTypeTbl(i, 1)--[i][1]
        if jingLingType == jingLingType1 and skillPieceIndex == JingLingSkillPieceIndex then
            skillPieceTotalCount = skillPieceCount
            return true, skillPieceTotalCount
        end
    end
    return true, 0
end

--根据装备id，查询如果删除从玩家身上摘除该装备，会不会对当前激活的精灵技能产生影响
--返回值为0表明，不用删除当前激活技能
--返回值-1表明，删除当前激活技能时出错
function  AdvanceEquipDB.GetRemoveJingLingSkillIndex(charID)
    local jingLingSkillIndex = 0
   
    local ret = SqlTbl._SelectActiveJingLingSkill:ExecStat(charID)

    if ret ~= nil and ret:GetRowNum() > 0 then
		jingLingSkillIndex = ret:GetData(0, 0)
        if jingLingSkillIndex == 0 then     --当前没有激活的精灵技能
            return false, 0
        end 
        local suc, totalPieceCount = AdvanceEquipDB.GetCharSkillPieceCountBySkill(jingLingSkillIndex, charID)
        if suc ~= true then
            LogErr("精灵技能信息错误,精灵类型：", jingLingSkillIndex .. "玩家ID:" .. charID)
            --return false, totalPieceCount
        end
        local needPieceCount = AttributeOfJingLingSkill_Common(jingLingSkillIndex, "NeedPieceCount")
        
        if suc == false or needPieceCount > totalPieceCount  then
            --删除当前激活的精灵技能
            SqlTbl._UpdateActiveJingLingSkill:ExecStat(0, charID)
            if g_DbChannelMgr:LastAffectedRowNum() < 1 then
    		    CancelTran()
    		    return false, -1 --删除激活技能失败
    		end
            return true, jingLingSkillIndex
        else 
            return false, 0
        end
    end

    return false, jingLingSkillIndex
end

--查询当前激活的精灵技能索引值
function AdvanceEquipDB.GetJingLingSkillInfo(charID)
    local skillIndexRet = SqlTbl._SelectActiveJingLingSkill:ExecStat(charID)
    if skillIndexRet ~= nil and skillIndexRet:GetRowNum() > 0 then
        local skillIndex = skillIndexRet:GetDataByName(0, "iea_uActiveSkillIndex")
        return skillIndex
    end
    return 0
end

SetDbLocalFuncType(AdvanceEquipDB.EquipAdvance)
SetDbLocalFuncType(AdvanceEquipDB.ActiveJingLingSkillPiece)
SetDbLocalFuncType(AdvanceEquipDB.AdvancedEquipReborn)
SetDbLocalFuncType(AdvanceEquipDB.ActiveJingLingSkill)
return AdvanceEquipDB
