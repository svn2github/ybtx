
local loadstring = loadstring
local g_ItemInfoMgr = g_ItemInfoMgr
local GasArmorPieceEnactmentSql = class()
cfg_load "item/ArmorPieceEnactment_Common"
local g_MoneyMgr = g_MoneyMgr
local ArmorPieceEnactment_Common = ArmorPieceEnactment_Common
local event_type_tbl = event_type_tbl

local GasArmorPieceEnactmentDB = CreateDbBox(...)
local ArmorPieceEquipPartTbl = {}
------------------------------------------------------------------------------------------------------	
function GasArmorPieceEnactmentDB.InitArmorPieceEquipPartTbl()
	local keys = ArmorPieceEnactment_Common:GetKeys()
	for i=1,#keys do
		local key = keys[i]
		ArmorPieceEquipPartTbl[key] = {}
		local ArmorPieceEquipPartStr = ArmorPieceEnactment_Common(key,"EquipPart") --护甲片的部位
		local EquipPartTbl = loadstring( "return " .. ArmorPieceEquipPartStr)()
		for j = 1,#EquipPartTbl do
			ArmorPieceEquipPartTbl[key][EquipPartTbl[j]] = true
		end
	end
end
--require文件时，将配置表需要loadstring的内容存储到表中，避免多次loadstring
GasArmorPieceEnactmentDB.InitArmorPieceEquipPartTbl()
------------------------------------------------------------------------------------------------------
local single_equip_cost_tbl = {
								[6] = 5000,
								[7] = 10000,
								[8] = 15000,
								[9] = 20000
}

local both_equip_cost_tbl = {
								[12] = 5000,
								[13] = 5000,
								[14] = 10000,
								[15] = 10000,
								[16] = 15000,
								[17] = 15000,
								[18] = 20000,
								[19] = 20000,
}


local StmtDef = {
	"_GetArmorPieceAttr",
	[[ select iea_sAttr,iea_uAddTimes from tbl_item_equip_armor where is_uId = ? order by iea_uIndex]]
}
DefineSql(StmtDef, GasArmorPieceEnactmentSql)

--@brief 查询装备表里面的护甲片属性及属性值
function GasArmorPieceEnactmentDB.GetArmorPieceEnactmentAttr(equipId)
	local res = GasArmorPieceEnactmentSql._GetArmorPieceAttr:ExecStat(equipId)
	local attr1,attr2,attr3,attr4 = "","","",""
	local addTimes1,addTimes2,addTimes3,addTimes4 = 0,0,0,0
	local num = res:GetRowNum()
	if num == 1 then 
		attr1 = res:GetData(0,0)
		addTimes1 = res:GetData(0,1)
	elseif num == 2 then 
		attr1 = res:GetData(0,0)
		addTimes1 = res:GetData(0,1)
		attr2 = res:GetData(1,0)
		addTimes2 = res:GetData(1,1)
	elseif num == 3 then 
		attr1 = res:GetData(0,0)
		addTimes1 = res:GetData(0,1)
		attr2 = res:GetData(1,0)
		addTimes2 = res:GetData(1,1)
		attr3 = res:GetData(2,0)
		addTimes3 = res:GetData(2,1)
	elseif num == 4 then 
		attr1 = res:GetData(0,0)
		addTimes1 = res:GetData(0,1)
		attr2 = res:GetData(1,0)
		addTimes2 = res:GetData(1,1)	
		attr3 = res:GetData(2,0)
		addTimes3 = res:GetData(2,1)
		attr4 = res:GetData(3,0)
		addTimes4 = res:GetData(3,1)
	end
	
	return {{attr1,addTimes1},{attr2,addTimes2},{attr3,addTimes3},{attr4,addTimes4}}
end
------------------------------------------------------------------------------------------------------	
--验证从客户端传过来的装备ID是否在正确
local StmtDef = {
	"_GetEquipPartByEquipId",
	[[ select ce_uEquipPart from tbl_item_equip where is_uId = ? and cs_uId =? ]]
}
DefineSql(StmtDef, GasArmorPieceEnactmentSql)


local StmtDef = {
	"_SaveArmorPieceAttrInfo",
	[[ replace into tbl_item_equip_armor (is_uId,iea_sAttr,iea_uAddTimes,iea_uIndex) values(?,?,?,?)]]
}
DefineSql(StmtDef, GasArmorPieceEnactmentSql)

local StmtDef = {
	"_SelectEquipEnactmentTimes",
	[[ select iea_uAddTimes from tbl_item_equip_armor where is_uId = ? and iea_sAttr =? and iea_uIndex = ?]]
}
DefineSql(StmtDef, GasArmorPieceEnactmentSql)

local StmtDef = {
	"_UpdateEquipEnactmentTimes",
	[[ update tbl_item_equip_armor set iea_uAddTimes = iea_uAddTimes + 1 where is_uId = ? and iea_sAttr =? and iea_uIndex = ?  ]]
}
DefineSql(StmtDef, GasArmorPieceEnactmentSql)


local StmtDef = {
	"_GetEquipEnactmentTotalTimes",
	[[ select sum(iea_uAddTimes) from tbl_item_equip_armor where is_uId = ? ]]
}
DefineSql(StmtDef, GasArmorPieceEnactmentSql)

--@brief 修改装备护甲片设定属性及属性值
local function SaveEquipAttr(fieldValue,equipId,index,charId,isBothEquip,update_index)
	local AddTimes = ArmorPieceEnactment_Common(fieldValue,"AddTimes")
	local add_times_res = GasArmorPieceEnactmentSql._GetEquipEnactmentTotalTimes:ExecStat(equipId)
	local total_add_times = 0 
	if add_times_res:GetRowNum() > 0 then
		total_add_times = add_times_res:GetData(0,0)
	end
	local res = GasArmorPieceEnactmentSql._SelectEquipEnactmentTimes:ExecStat(equipId,fieldValue,index)
	local add_times = 0
	if res:GetRowNum() > 0 then
		add_times = res:GetData(0,0)
	end
	local cost = 0
	if isBothEquip then
		if (index == 0) or (index ~= 0 and ((add_times ~= 0 and AddTimes > add_times) or update_index == 0)   ) then
			cost = both_equip_cost_tbl[total_add_times] or 0
		end
	elseif index == 0 or (index ~= 0 and add_times ~= 0 and AddTimes > add_times) then
		cost = single_equip_cost_tbl[total_add_times] or 0
	end
	
	if cost > 0 then
		cost = - cost
		local MoneyManagerDB =	RequireDbBox("MoneyMgrDB") 
		local fun_info = g_MoneyMgr:GetFuncInfo("ArmorPieceEnactment")
		local bFlag,uMsgID = MoneyManagerDB.AddMoneyByType(fun_info["FunName"],fun_info["ArmorPieceEnactment"],charId, 1, cost,nil,event_type_tbl["装备镶嵌护甲片扣钱"])
		if not bFlag then
			return bFlag,uMsgID
		end
	end
	local uAddTimes = 1
	if AddTimes ~= 1 then
		if res:GetRowNum() > 0 then
			if res:GetData(0,0) < AddTimes then
				GasArmorPieceEnactmentSql._UpdateEquipEnactmentTimes:ExecStat(equipId,fieldValue,index)
				uAddTimes = res:GetData(0,0) + 1
			else
				GasArmorPieceEnactmentSql._SaveArmorPieceAttrInfo:ExecStat(equipId,fieldValue,1,index)
			end
		else
			GasArmorPieceEnactmentSql._SaveArmorPieceAttrInfo:ExecStat(equipId,fieldValue,1,index)
		end
	else
		GasArmorPieceEnactmentSql._SaveArmorPieceAttrInfo:ExecStat(equipId,fieldValue,1,index)
	end
	if g_DbChannelMgr:LastAffectedRowNum() > 0 then
		local LogMgrDB = RequireDbBox("LogMgrDB")
		LogMgrDB.SaveArmorIncrustationInfo( charId,equipId,fieldValue,uAddTimes,index)
		return true,cost
	else
		CancelTran()
		return false
	end
end

--@brief 武器护甲片设定
local function WeaponArmorPieceEnactment(nbigID,nIndex,equipId,update_index,ArmorPieceName,charId)
	local	equipType = g_ItemInfoMgr:GetItemInfo(nbigID,nIndex,"EquipType")
	if equipType and equipType ~= "" then
		local equiptype = string.sub(equipType,1,2)
		if equiptype and equiptype ~= "" then
			local addAttrTbl = GasArmorPieceEnactmentDB.GetArmorPieceEnactmentAttr(equipId)
			local equip_addAttr1,equip_addAttr2,equip_addAttr3,equip_addAttr4 = unpack(addAttrTbl)
			local AddTimes1 = ArmorPieceEnactment_Common(equip_addAttr1[1],"AddTimes")
			local AddTimes2 = ArmorPieceEnactment_Common(equip_addAttr2[1],"AddTimes")
			if equiptype == "双" or equiptype == "长" then
				local AddTimes3 = ArmorPieceEnactment_Common(equip_addAttr3[1],"AddTimes")
				local AddTimes4 = ArmorPieceEnactment_Common(equip_addAttr4[1],"AddTimes")
				if equip_addAttr1[1] == ""  or update_index == 1 or (ArmorPieceName == equip_addAttr1[1] and AddTimes1 > equip_addAttr1[2]) then
					return SaveEquipAttr(ArmorPieceName,equipId,1,charId,true,update_index)
				elseif equip_addAttr2[1] == "" or update_index == 2 or (ArmorPieceName == equip_addAttr2[1] and AddTimes2 > equip_addAttr2[2]) then
					return SaveEquipAttr(ArmorPieceName,equipId,2,charId,true,update_index)
				elseif equip_addAttr3[1] == "" or update_index == 3 or (ArmorPieceName == equip_addAttr3[1] and AddTimes3 > equip_addAttr3[2]) then
					return SaveEquipAttr(ArmorPieceName,equipId,3,charId,true,update_index)
				elseif equip_addAttr4[1] == "" or update_index == 4 or (ArmorPieceName == equip_addAttr4[1] and AddTimes4 > equip_addAttr4[2]) then
					return SaveEquipAttr(ArmorPieceName,equipId,4,charId,true,update_index)
				end
			else
				if equip_addAttr1[1] == "" or update_index == 1 or (ArmorPieceName == equip_addAttr1[1] and AddTimes1 > equip_addAttr1[2]) then
					return SaveEquipAttr(ArmorPieceName,equipId,1,charId)
				elseif equip_addAttr2[1] == "" or update_index == 2 or (ArmorPieceName == equip_addAttr2[1] and AddTimes2 > equip_addAttr2[2]) then
					return SaveEquipAttr(ArmorPieceName,equipId,2,charId)
				end
			end
		end
	end
end

--@brief 护甲片设定
function GasArmorPieceEnactmentDB.ArmorPieceEnactment(data)
	local armorPieceRoomIndex = data["armorPieceRoomIndex"]
	local armorPiecePos = data["armorPiecePos"]
	local equipRoomIndex = data["equipRoomIndex"]
	local equipPos = data["equipPos"]
	local equipitemid = data["equipitemid"]
	local uCharId = data["uCharId"]
	local bigID = data["nBigID"]
	local nIndex = data["nIndex"]
	local update_index = data["update_index"]
	local itemType = data["itemType"]
	local ArmorPieceName = data["ArmorPieceName"]
	local equip_id = {}
	local EquipRet_Old ={}
	local EquipRet_New ={}
	local equipId = 0
	local equip_part = 0
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local EquipMgrDB = RequireDbBox("EquipMgrDB")
	local armorPieceId = 0
	local flag = false
	local bSucc,msgId = nil,0
	--读取护甲片配置表
	local sItemType, sItemName, count = g_RoomMgr.GetTypeCountByPosition(uCharId, armorPieceRoomIndex, armorPiecePos)
	if count == nil or count == 0 or sItemType ~= 34 then
		return false
	end
	
	local armorPiece_equipType = g_ItemInfoMgr:GetItemInfo(itemType,ArmorPieceName,"EquipType")
	if (armorPiece_equipType == nil or armorPiece_equipType == "") then
		return false
	end
	
	local armorPieceSet = g_RoomMgr.GetAllItemFromPosition(uCharId, armorPieceRoomIndex, armorPiecePos)
	if armorPieceSet:GetRowNum() > 0  then
		armorPieceId = armorPieceSet(1,1)
	else
		return false
	end

	if equipRoomIndex ~= 0 and equipPos ~= 0 then
		local sItemType, sItemName, count = g_RoomMgr.GetTypeCountByPosition(uCharId, equipRoomIndex, equipPos)
		local binding = g_RoomMgr.GetBindingTypeByPos(uCharId, equipRoomIndex, equipPos)
		if count == nil or count == 0 or sItemType < 5 or sItemType > 9 then
			return false
		end 
		equip_id = g_RoomMgr.GetAllItemFromPosition(uCharId, equipRoomIndex, equipPos)
		if equip_id:GetRowNum() > 0 then
			equipId = equip_id(1,1)
		end
	else
		if equipitemid ~= 0 then
			local equipSet = GasArmorPieceEnactmentSql._GetEquipPartByEquipId:ExecSql("n",equipitemid,uCharId)
			if nil ~= equipSet and equipSet:GetRowNum() > 0 then
				equipId = equipitemid
				equip_part = equipSet:GetData(0,0)
			    EquipRet_Old =  EquipMgrDB.GetEquipAllInfo( equipitemid )
				flag = true
			else
				return false
			end
		end
	end
   
  if equipId == 0 then
  	return false   --要鉴定的装备不存在
  end
  local equip_addAttr1,equip_addAttr2 = unpack(GasArmorPieceEnactmentDB.GetArmorPieceEnactmentAttr(equipId))
  local AddTimes1 = ArmorPieceEnactment_Common(equip_addAttr1[1],"AddTimes")
  local AddTimes2 = ArmorPieceEnactment_Common(equip_addAttr2[1],"AddTimes")
	--判断是不是防具或者饰品
	if g_ItemInfoMgr:IsArmor(bigID) or g_ItemInfoMgr:IsStaticJewelry(bigID) then
		local equipPart = g_ItemInfoMgr:GetItemInfo(bigID,nIndex,"EquipPart")
		local ifEnactment = ArmorPieceEquipPartTbl[ArmorPieceName][equipPart]
		if ifEnactment then
			if equip_addAttr1[1] == "" or update_index == 1 or (ArmorPieceName == equip_addAttr1[1] and AddTimes1 > equip_addAttr1[2]) then
				bSucc,msgId = SaveEquipAttr(ArmorPieceName,equipId,1,uCharId)
				if not bSucc then
					return false,msgId
				end
			elseif equip_addAttr2[1] == "" or update_index == 2  or (ArmorPieceName == equip_addAttr2[1] and AddTimes2 > equip_addAttr2[2]) then
				bSucc,msgId = SaveEquipAttr(ArmorPieceName,equipId,2,uCharId)
				if not bSucc then
					return false,msgId
				end
			end
		else
			return false,171003
		end
	--判断是不是武器
	elseif g_ItemInfoMgr:IsWeapon(bigID) then
		local ifEnactment = ArmorPieceEquipPartTbl[ArmorPieceName]["武器"]
		if ifEnactment then
			bSucc,msgId = WeaponArmorPieceEnactment(bigID,nIndex,equipId,update_index,ArmorPieceName,uCharId)
			if not bSucc then
				return false,msgId
			end
		else
			return false,171003
		end
	--判断是否为盾牌
	elseif g_ItemInfoMgr:IsStaticShield(bigID) then
		local ifEnactment = ArmorPieceEquipPartTbl[ArmorPieceName]["武器"]
		if ifEnactment then
			if equip_addAttr1[1] == "" or update_index == 1 or (ArmorPieceName == equip_addAttr1[1] and AddTimes1 > equip_addAttr1[2]) then
				bSucc,msgId = SaveEquipAttr(ArmorPieceName,equipId,1,uCharId)
				if not bSucc then
					return false,msgId
				end
			elseif equip_addAttr2[1] == "" or update_index == 2 or (ArmorPieceName == equip_addAttr2[1] and AddTimes2 > equip_addAttr2[2]) then
				bSucc,msgId = SaveEquipAttr(ArmorPieceName,equipId,2,uCharId)
				if not bSucc then
					return false,msgId
				end
			end
		else
			return false,171003
		end
	--判断是不是戒指
	elseif g_ItemInfoMgr:IsRing(bigID) then
		local ifEnactment = ArmorPieceEquipPartTbl[ArmorPieceName]["戒指"]
		if ifEnactment then
			if equip_addAttr1[1] == "" or update_index == 1 or (ArmorPieceName == equip_addAttr1[1] and AddTimes1 > equip_addAttr1[2]) then
				bSucc,msgId = SaveEquipAttr(ArmorPieceName,equipId,1,uCharId)
				if not bSucc then
					return false,msgId
				end
			elseif equip_addAttr2[1] == "" or update_index == 2 or (ArmorPieceName == equip_addAttr2[1] and AddTimes2 > equip_addAttr2[2]) then
				bSucc,msgId = SaveEquipAttr(ArmorPieceName,equipId,2,uCharId)
				if not bSucc then
					return false,msgId
				end
			end
		else
			return false,171003
		end
	end

	local DelRet = g_RoomMgr.DelItemByPos(uCharId,armorPieceRoomIndex,armorPiecePos,1,event_type_tbl["物品使用"])
	if IsNumber(DelRet) then
		CancelTran()
		return false
	end
	EquipRet_New = EquipMgrDB.GetEquipAllInfo( equipitemid )
      
	local tbl =
		{ ["EquipRet_New"] = EquipRet_New,
			["EquipRet_Old"] = EquipRet_Old,
			["armorPieceId"] = armorPieceId,
			["Flag"] = flag,
			["equipId"] = equipId,
			["equip_part"] = equip_part,
			["msgId"] = msgId
			
		}
	return true,tbl
end


SetDbLocalFuncType(GasArmorPieceEnactmentDB.ArmorPieceEnactment)
return GasArmorPieceEnactmentDB
