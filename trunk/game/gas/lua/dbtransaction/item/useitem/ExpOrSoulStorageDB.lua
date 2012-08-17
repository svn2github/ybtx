
cfg_load "item/Exp_Soul_Bottle_Common"
local Exp_Soul_Bottle_Common = Exp_Soul_Bottle_Common
local g_ItemInfoMgr  = g_ItemInfoMgr
local event_type_tbl = event_type_tbl
local ExpOrSoulStorageDB = CreateDbBox(...)
local ExpOrSoulStorageSql = {}
-----------------------------------------------------------
local uActivationType 	= 1 	--激活
local uCloseType 	= 2		--关闭

local StmtDef ={
    "_SelectActivationSoulBottle",
    [[
       select is_uId from tbl_item_soul_bottle where cs_uId = ? and isb_uState = 1
    ]]
}
DefineSql(StmtDef, ExpOrSoulStorageSql)


local StmtDef ={
    "_UpdateActivationSoulBottleState",
    [[
       update tbl_item_soul_bottle set isb_uState = ? where cs_uId = ? and is_uId = ?
    ]]
}
DefineSql(StmtDef, ExpOrSoulStorageSql)

local StmtDef ={
    "_QueryStorageNumById",
    [[
       select isb_uStorageNum from tbl_item_soul_bottle where cs_uId = ? and is_uId = ?
    ]]
}
DefineSql(StmtDef, ExpOrSoulStorageSql)


local StmtDef ={
    "_SelectActivationExpBottle",
    [[
       select is_uId from tbl_item_exp_bottle where cs_uId = ? and ieb_uState = 1
    ]]
}
DefineSql(StmtDef, ExpOrSoulStorageSql)


local StmtDef ={
    "_UpdateActivationExpBottleState",
    [[
       update tbl_item_exp_bottle set ieb_uState = ? where cs_uId = ? and is_uId = ?
    ]]
}
DefineSql(StmtDef, ExpOrSoulStorageSql)



--@brief 激活经验瓶、魂瓶	
function ExpOrSoulStorageDB.OpenExpOrSoulBottle(data)
	local uRoomIndex = data["uRoomIndex"]
	local uPos = data["uPos"]
	local uCharId = data["uCharID"]
	local SoulBottleInfo = data["SoulBottleInfo"]
	local uSoulBottleValue = data["uSoulBottleValue"]
	local uLevel = data["uLevel"]
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")

	local sItemType, sItemName, uItemId = g_RoomMgr.GetOneItemByPosition(uCharId, uRoomIndex, uPos)
	if sItemType  == nil or sItemName == nil or uItemId == nil or sItemType ~= 48  then
		return false, 182001
	end
	
	local base_level = Exp_Soul_Bottle_Common(sItemName,"BaseLevel")
	if uLevel < base_level then
		return false,182011
	end
	
	local bottle_type = tonumber(Exp_Soul_Bottle_Common(sItemName,"Type"))
	local result = {}
	if bottle_type == 1 then --魂瓶
		local res = ExpOrSoulStorageSql._SelectActivationSoulBottle:ExecStat(uCharId)
		if res:GetRowNum() == 0 then --没有激活的魂瓶
			ExpOrSoulStorageSql._UpdateActivationSoulBottleState:ExecStat(uActivationType,uCharId,uItemId)
			local num_res = ExpOrSoulStorageSql._QueryStorageNumById:ExecStat(uCharId,uItemId)
			result["uActivationItemId"] = uItemId
			result["uActivationNum"] = num_res:GetData(0,0)
		elseif res:GetData(0,0) == uItemId then --关闭激活的魂瓶
			if SoulBottleInfo then
				if SoulBottleInfo[1] == uItemId and uSoulBottleValue and uSoulBottleValue > 0 then
					local bottleName = SoulBottleInfo[2]
					local uStorageNum = Exp_Soul_Bottle_Common(bottleName,"StorageNum")
					local param = {}
					param["PlayerId"] = uCharId
					param["uSoulBottleValue"] = uSoulBottleValue
					param["uSoulBottleId"] = uItemId
					param["StorageNum"] = uStorageNum
					param["bottleName"] = bottleName
					ExpOrSoulStorageDB.StorageSoulValue(param)
				end
			end
			ExpOrSoulStorageSql._UpdateActivationSoulBottleState:ExecStat(uCloseType,uCharId,uItemId)
			result["uCloseItemId"] = uItemId
		else
			if SoulBottleInfo then
				if SoulBottleInfo[1] == res:GetData(0,0) and uSoulBottleValue and uSoulBottleValue > 0 then
					local bottleName = SoulBottleInfo[2]
					local uStorageNum = Exp_Soul_Bottle_Common(bottleName,"StorageNum")
					local param = {}
					param["PlayerId"] = uCharId
					param["uSoulBottleValue"] = uSoulBottleValue
					param["uSoulBottleId"] = res:GetData(0,0)
					param["StorageNum"] = uStorageNum
					param["bottleName"] = bottleName
					ExpOrSoulStorageDB.StorageSoulValue(param)
				end
			end
			ExpOrSoulStorageSql._UpdateActivationSoulBottleState:ExecStat(uActivationType,uCharId,uItemId)
			ExpOrSoulStorageSql._UpdateActivationSoulBottleState:ExecStat(uCloseType,uCharId,res:GetData(0,0))
			result["uActivationItemId"] = uItemId
			result["uCloseItemId"] = res:GetData(0,0)
			local num_res = ExpOrSoulStorageSql._QueryStorageNumById:ExecStat(uCharId,uItemId)
			result["uActivationItemId"] = uItemId
			result["uActivationNum"] = num_res:GetData(0,0)
		end
	elseif bottle_type == 2 then
		local res = ExpOrSoulStorageSql._SelectActivationExpBottle:ExecStat(uCharId)
		if res:GetRowNum() == 0 then
			ExpOrSoulStorageSql._UpdateActivationExpBottleState:ExecStat(uActivationType,uCharId,uItemId)
			result["uActivationItemId"] = uItemId
		elseif res:GetData(0,0) == uItemId then
			ExpOrSoulStorageSql._UpdateActivationExpBottleState:ExecStat(uCloseType,uCharId,res:GetData(0,0))
			result["uCloseItemId"] = res:GetData(0,0)
		else
			ExpOrSoulStorageSql._UpdateActivationExpBottleState:ExecStat(uActivationType,uCharId,uItemId)
			ExpOrSoulStorageSql._UpdateActivationExpBottleState:ExecStat(uCloseType,uCharId,res:GetData(0,0))
			result["uActivationItemId"] = uItemId
			result["uCloseItemId"] = res:GetData(0,0)
		end
	end
	return g_DbChannelMgr:LastAffectedRowNum() > 0,result,bottle_type,sItemName
end
--------------------------------------------------------------
local StmtDef ={
    "_SelectEnabledSoulBottleInfo",
    [[
       select 
       	iss.is_uId,iss.is_sName,isb_uStorageNum
       from 
       	tbl_item_soul_bottle isb ,tbl_item_static iss
       where 
       	isb.is_uId = iss.is_uId and cs_uId = ? and isb_uState = 1
    ]]
}
DefineSql(StmtDef, ExpOrSoulStorageSql)

--@brief 查询激活状态下的瓶子
function ExpOrSoulStorageDB.QueryEnabledSoulBottle(uCharId)
	local res = ExpOrSoulStorageSql._SelectEnabledSoulBottleInfo:ExecStat(uCharId)
	return res
end 
-------------------------------------------------------------
local StmtDef ={
    "_SelectActiveSoulInfo",
    [[
       select isb.is_uId ,iss.is_sName
       from tbl_item_soul_bottle isb,tbl_item_static iss
       where isb.is_uId = iss.is_uId and cs_uId = ? and isb_uState = 1 
    ]]
}
DefineSql(StmtDef, ExpOrSoulStorageSql)	

--@brief 下线将高速中的魂值存入数据库
function ExpOrSoulStorageDB.StorageSoulValueByCount(data)
	local uCharId = data["char_id"]
	local res = ExpOrSoulStorageSql._SelectActiveSoulInfo:ExecStat(uCharId)
	if res:GetRowNum() == 0 then
		return
	end
	local itemId = res(1,1)
	local bottleName = res(1,2)
	local uStorageNum = Exp_Soul_Bottle_Common(bottleName,"StorageNum")
	local param = {}
	param["PlayerId"] = uCharId
	param["uSoulBottleValue"] = data["uSoulBottleValue"]
	param["uSoulBottleId"] = itemId
	param["StorageNum"] = uStorageNum
	param["bottleName"] = bottleName
	ExpOrSoulStorageDB.StorageSoulValue(param)
end
-------------------------------------------------------------
local StmtDef ={
    "_StorageSoulValue",
    [[
       update tbl_item_soul_bottle set isb_uStorageNum = isb_uStorageNum + ? where cs_uId = ? and is_uId = ? and isb_uState = 1 
    ]]
}
DefineSql(StmtDef, ExpOrSoulStorageSql)

local StmtDef ={
    "_QueryStorageNum",
    [[
       select isb_uStorageNum from tbl_item_soul_bottle where cs_uId = ? and is_uId = ? and isb_uState = 1 
    ]]
}
DefineSql(StmtDef, ExpOrSoulStorageSql)


local StmtDef ={
    "_DelBottle",
    [[
       delete from tbl_item_soul_bottle where cs_uId = ? and is_uId = ? and isb_uState = 1 
    ]]
}
DefineSql(StmtDef, ExpOrSoulStorageSql)

local StmtDef ={
    "_SelectItemPos",
    [[
       select gir_uRoomIndex,gir_uPos from tbl_item_in_grid iip,tbl_grid_in_room as igp
       where cs_uId = ? and is_uId = ? 
    ]]
}
DefineSql(StmtDef, ExpOrSoulStorageSql)	
	

--@brief 给激活状态下的瓶子加对应比例的值
function ExpOrSoulStorageDB.StorageSoulValue(data)
	local uCharId = data["PlayerId"]
	local uCount = data["uSoulBottleValue"]
	local itemId = data["uSoulBottleId"]
	local uStorageNum = data["StorageNum"]
	local bottleName = data["bottleName"]
	local res = ExpOrSoulStorageSql._QueryStorageNum:ExecStat(uCharId,itemId)

	if res:GetRowNum() == 0 then
		return false
	end
	if res:GetData(0,0) + uCount < uStorageNum then
		ExpOrSoulStorageSql._StorageSoulValue:ExecStat(uCount,uCharId,itemId)
	else
		local pos_info = ExpOrSoulStorageSql._SelectItemPos:ExecStat(uCharId,itemId)
		if pos_info:GetRowNum() == 0 then
			return false
		end
		local g_RoomMgr     =   RequireDbBox("GasRoomMgrDB")
		local ret = g_RoomMgr.DelItemByID(uCharId, itemId,event_type_tbl["物品使用"])
		if not ret then
			return false
		end
		
		local itemName = Exp_Soul_Bottle_Common(bottleName,"ExpSoulItemName")
		local params= {}
		params.nCharID = uCharId
		params.nStoreMain = 1
		params.nBigID = 19
		params.nIndex = itemName
		params.nCount = 1
		params.BindingType = g_ItemInfoMgr:GetItemInfo(19, itemName,"BindingStyle")
		params.createType = event_type_tbl["魂瓶满产生魂珠"]
		
		local item_res = g_RoomMgr.AddItem(params)
		if tonumber(item_res) then
			CancelTran()
			return false
		else
			return true,item_res,itemName,pos_info(1,1), pos_info(1,2)
		end
	end

	if g_DbChannelMgr:LastAffectedRowNum() == 0 then
		LogErr("魂瓶存储魂值失败","玩家id：" .. uCharId  .. ",魂值：" .. data["uSoulBottleValue"] .. ",物品id：" .. data["uSoulBottleId"])
	end
end

-------------------------------------------------------------------------------
local StmtDef ={
    "_SaveSoulBottleInfo",
    [[
       insert into tbl_item_soul_bottle values(?,?,?,?)
    ]]
}
DefineSql(StmtDef, ExpOrSoulStorageSql)

local StmtDef ={
    "_SaveExpBottleInfo",
    [[
       insert into tbl_item_exp_bottle values(?,?,?,?)
    ]]
}
DefineSql(StmtDef, ExpOrSoulStorageSql)

function ExpOrSoulStorageDB.CreateExpOrSoulBottle(uCharId,uItemID,sItemName)
	local bottle_type = tonumber(Exp_Soul_Bottle_Common(sItemName,"Type"))
	if bottle_type == 1 then
		ExpOrSoulStorageSql._SaveSoulBottleInfo:ExecStat(uCharId,uItemID,uCloseType,0)
	elseif bottle_type == 2 then
		ExpOrSoulStorageSql._SaveExpBottleInfo:ExecStat(uCharId,uItemID,uCloseType,0)
	end
	if g_DbChannelMgr:LastAffectedRowNum() == 0 then
		error("创建魂瓶或者经验瓶失败！","玩家id" .. uCharId .. ",物品名称：" .. sItemName)
	end
end

SetDbLocalFuncType(ExpOrSoulStorageDB.OpenExpOrSoulBottle)
SetDbLocalFuncType(ExpOrSoulStorageDB.StorageSoulValue)

return ExpOrSoulStorageDB


