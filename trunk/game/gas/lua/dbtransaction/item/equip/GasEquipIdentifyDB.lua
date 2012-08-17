cfg_load "equip/ArmorProValue_Common"
cfg_load "equip/WeaponDPSValue_Common"

local EquipIdentifySql = class()
local ArmorProValue_Common	= 	ArmorProValue_Common
local WeaponDPSFloor_Common = WeaponDPSFloor_Common
local WeaponDPSValue_Common = WeaponDPSValue_Common
local event_type_tbl = event_type_tbl

local EquipIdentifyDB = CreateDbBox(...)

------------------------------------[以下是装备鉴定代码]--------------------------------
--根据鉴定后的属性值修改数据库里面的防具属性
local StmtDef = {
	"_UpdateAomorAtrrValue",
	[[
		update 
			tbl_item_armor
		set 
			ia_uUpgradeAtrrValue1 = ?,ia_uUpgradeAtrrValue2 = ?,ia_uUpgradeAtrrValue3 = ?
		where 
			is_uId = ?
	]]
}
DefineSql(StmtDef, EquipIdentifySql)
--验证从客户端传过来的装备ID是否在正确
local StmtDef = {
	"_GetEquipIdByEquipId",
	[[ select ce_uEquipPart from tbl_item_equip where is_uId = ? and cs_uId =? ]]
}
DefineSql(StmtDef, EquipIdentifySql)


--获取三个属性的随机比例值，如果trendflag(倾向值不为0的话就得按照倾向值具体取值)的
local function GetRandom(trendflag,identify_equipTrendValue)
	local a1, a2, a3, tbl = 0, 0, 0, {}
	if trendflag == 0 then
		a1 = math.random(1, 10)
		table.insert(tbl, a1)
	else
		a1 = math.random(identify_equipTrendValue, 10)
		tbl[trendflag] = a1
	end
	local a2 = math.random(11 - a1) - 1
	local a3 = 10 - a1 - a2
	table.insert(tbl, a2)
	table.insert(tbl, a3)
	return unpack(tbl)
end

local function GetIdentifyAttrValue(armorBaseValue,attrNameTbl,identify_equipTrend,identify_equipTrendValue,equipType,equipPart)
	local attrValue = {}
	local trend_flag = 0
	local flag = 1
	if next(attrNameTbl) and identify_equipTrend ~= "" then
		for i = 1,#attrNameTbl do
			if attrNameTbl[i] == identify_equipTrend then
				trend_flag = i
				break
			end
		end
		if trend_flag == 0 then
			return attrValue
		end
	end
	
	--取得属性比例值
	local attrscale1,attrscale2,attrscale3 = GetRandom(trend_flag,identify_equipTrendValue)
	local healthPoint = ArmorProValue_Common(equipType,equipPart,"HealthPoint")
	local defence = ArmorProValue_Common(equipType,equipPart,"Defence")
	local natureResistanceValue = ArmorProValue_Common(equipType,equipPart,"NatureResistanceValue")
	local destructionResistanceValue = ArmorProValue_Common(equipType,equipPart,"DestructionResistanceValue")
	local evilResistanceValue = ArmorProValue_Common(equipType,equipPart,"EvilResistanceValue")
	local randomScale = tonumber(ArmorProValue_Common(equipType,equipPart,"RandomScale"))

	if healthPoint ~= "" then
		table.insert(attrValue,armorBaseValue* tonumber(healthPoint)/100 + armorBaseValue*randomScale/100*attrscale1/10*2)
		flag = flag + 1
	end
	
	if defence ~= "" then
		if flag == 1 then
			table.insert(attrValue,armorBaseValue* tonumber(defence)/100 + armorBaseValue*randomScale/100*attrscale1/10)
		else
			table.insert(attrValue,armorBaseValue* tonumber(defence)/100 + armorBaseValue*randomScale/100*attrscale2/10)
		end
		flag = flag + 1
	end
	
	if natureResistanceValue ~= "" then
		if flag == 1 then
			table.insert(attrValue,armorBaseValue* tonumber(natureResistanceValue)/100 + armorBaseValue*randomScale/100*attrscale1/10)
		elseif flag == 2 then
			table.insert(attrValue,armorBaseValue* tonumber(natureResistanceValue)/100 + armorBaseValue*randomScale/100*attrscale2/10)
		else
			table.insert(attrValue,armorBaseValue* tonumber(natureResistanceValue)/100 + armorBaseValue*randomScale/100*attrscale3/10)
		end
		flag = flag + 1
	end
	
	if destructionResistanceValue ~= "" then
		if flag == 2 then
			table.insert(attrValue,armorBaseValue* tonumber(destructionResistanceValue)/100 + armorBaseValue*randomScale/100*attrscale2/10)
		else
			table.insert(attrValue,armorBaseValue* tonumber(destructionResistanceValue)/100 + armorBaseValue*randomScale/100*attrscale3/10)
		end
		flag = flag + 1
	end
	
	if evilResistanceValue ~= "" then
		if flag == 3 then
			table.insert(attrValue,armorBaseValue* tonumber(evilResistanceValue)/100 + armorBaseValue*randomScale/100*attrscale3/10)
		end
	end
	return attrValue
end

--防具鉴定
function EquipIdentifyDB.ArmorOrJewelryIdentify(data)
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local EquipMgrDB = RequireDbBox("EquipMgrDB")
	local attrNameTbl = data["attrNameTbl"]
	local identify_equipTrend = data["identify_equipTrend"]
	local armorBaseValue = data["armorBaseValue"]
	local equipType = data["equipType"]
	local equipPart = data["equipPart"]
	local equipRoomIndex = data["equipRoomIndex"]
	local equipPos = data["equipPos"]
	local equipId = 0
	local scrollRoomIndex = data["scrollRoomIndex"]
	local scrollPos = data["scrollPos"]
	local equipitemid = data["equipitemid"]
	local identify_equipTrendValue = data["identify_equipTrendValue"]
	local scrollId = 0
	local equip_id = {}
	local flag = false
	local uCharId = data["uCharId"]
	local part = 0 --设置装备属性取决的装备部位（在身上穿时的数字）

	local scrollTbl = g_RoomMgr.GetAllItemFromPosition(uCharId, scrollRoomIndex, scrollPos)
	if scrollTbl:GetRowNum() > 0 then
		scrollId = scrollTbl(1,1)
	end
	local oldEquipBaseInfoTbl ={}
	local newEquipBaseInfoTbl ={}
	if equipRoomIndex ~= 0 and equipPos ~= 0 then
		equip_id = g_RoomMgr.GetAllItemFromPosition(uCharId, equipRoomIndex, equipPos)
		if equip_id:GetRowNum() > 0 then
			equipId = equip_id(1,1)
		end
	else
		if equipitemid ~= 0 then
			local equipSet = EquipIdentifySql._GetEquipIdByEquipId:ExecSql("n",equipitemid,uCharId)
			if nil ~= equipSet and equipSet:GetRowNum() > 0 then
				equipId = equipitemid
				oldEquipBaseInfoTbl = EquipMgrDB.GetEquipAllInfo( equipId )
				flag = true
				part = EquipMgrDB.GetEquipPartById(uCharId,equipId)
			else
				return false
			end
		end
	end
	
	local attrValueTbl = GetIdentifyAttrValue(armorBaseValue,attrNameTbl,identify_equipTrend,identify_equipTrendValue,equipType,equipPart)
	if #attrValueTbl > 0  then
		local attrvalue1 = tonumber(string.format("%.0f",attrValueTbl[1]))
		local attrvalue2 = tonumber(string.format("%.0f",attrValueTbl[2]))
		local attrvalue3 = tonumber(string.format("%.0f",attrValueTbl[3]))
		
		EquipIdentifySql._UpdateAomorAtrrValue:ExecSql("",attrvalue1,attrvalue2,attrvalue3,equipId)
		local DelRet = g_RoomMgr.DelItemByPos(uCharId,scrollRoomIndex,scrollPos,1,event_type_tbl["物品使用"])
		if IsNumber(DelRet) then
			CancelTran()
			return false
		end
		newEquipBaseInfoTbl = EquipMgrDB.GetEquipAllInfo( equipId )
		local tbl =
						{ ["EquipRet_New"] = newEquipBaseInfoTbl,
							["EquipRet_Old"] = oldEquipBaseInfoTbl,
							["ScrollId"] = scrollId,
							["Flag"] = flag,
							["equipId"] = equipId,
							["Part"] = part
						}
		
		return true,tbl
	end
	return false
end

-----------------------------------------------------------------------------------------------------------
--更新武器攻击速度
local StmtDef = {
	"_UpdateWeaponAttackSpeed",
	[[
		update 
			tbl_item_weapon
		set 
			iw_uAttackSpeed = ?
		where 
			is_uId = ?
	]]
}
DefineSql(StmtDef, EquipIdentifySql)

--更新武器攻击浮动
local StmtDef = {
	"_UpdateWeaponDPSFloorRate",
	[[
		update 
			tbl_item_weapon
		set 
			iw_uDPSFloorRate = ?
		where 
			is_uId = ?
	]]
}
DefineSql(StmtDef, EquipIdentifySql)

local function GetRandomAttackSpeedOrFloorRate(lowerValue,upValue)
	local randomValue = math.random(lowerValue,upValue)
	return randomValue
end

--获取基础武器的DPS
local function GetBaseWeaponDPS(level,EquipType)
	local DPS = WeaponDPSValue_Common(level,"DPSBaseValue") * WeaponDPSFloor_Common(EquipType,"DPSRate")/100
	DPS = string.format("%.1f",DPS)
	return DPS
end

--武器鉴定
function EquipIdentifyDB.WeaponIdentify(data)
	local curLevel = data["curLevel"]
	local identify_equipTrend = data["identify_equipTrend"]
	local equipType = data["equipType"]
	local equipPart = data["equipPart"]
	local scrollRoomIndex = data["scrollRoomIndex"]
	local scrollPos = data["scrollPos"]
	local equipRoomIndex = data["equipRoomIndex"]
	local equipPos = data["equipPos"]
	local equipitemid = data["equipitemid"]
	local identify_equipTrendValue = data["identify_equipTrendValue"]
	local uCharId = data["uCharId"]
	local equipId = 0 --武器的Id
	local scrollId = 0 --卷轴Id
	local DPS = GetBaseWeaponDPS(curLevel,equipType)
	local newAttackSpeed,newDPSFloorRate
	local equipInfo = WeaponDPSFloor_Common(equipType)
	local oldEquipBaseInfoTbl ={}
	local newEquipBaseInfoTbl ={}
	local flag = false
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local EquipMgrDB = RequireDbBox("EquipMgrDB")
	if nil == equipInfo then
		return false
	end
	local part = 0 --设置装备属性取决的装备部位（在身上穿时的数字）
	local scrollTbl = g_RoomMgr.GetAllItemFromPosition(uCharId, scrollRoomIndex, scrollPos)
	if scrollTbl:GetRowNum() > 0  then
		scrollId = scrollTbl(1,1)
	end
	
	if equipRoomIndex ~= 0 and equipPos ~= 0 then
		local equip_id = g_RoomMgr.GetAllItemFromPosition(uCharId, equipRoomIndex, equipPos)
		if equip_id:GetRowNum() > 0 then
			equipId = equip_id(1,1)
		end
	else
		if equipitemid and equipitemid ~= 0 then
			local equipSet = EquipIdentifySql._GetEquipIdByEquipId:ExecSql("n",equipitemid,uCharId)
			if nil ~= equipSet and equipSet:GetRowNum() > 0 then
				equipId = equipitemid
				oldEquipBaseInfoTbl =  EquipMgrDB.GetEquipAllInfo( equipitemid )
				flag = true
				part = EquipMgrDB.GetEquipPartById(uCharId,equipId)
			end
		end
	end

	if "" ~= identify_equipTrend then
		if identify_equipTrend == "攻击速度" then
			if 0 ~= identify_equipTrendValue then
				newAttackSpeed = identify_equipTrendValue
			else
				local attackSpeedLower = equipInfo("AttackSpeedLower")
				local attackSpeedUp = equipInfo("AttackSpeedUp")
				newAttackSpeed = GetRandomAttackSpeedOrFloorRate(attackSpeedLower,attackSpeedUp)
			end
			EquipIdentifySql._UpdateWeaponAttackSpeed:ExecSql("",newAttackSpeed,equipId)
			local DelRet = g_RoomMgr.DelItemByPos(uCharId,scrollRoomIndex,scrollPos,1,event_type_tbl["物品使用"])
			if IsNumber(DelRet) then
				CancelTran()
				return false
			end
			newEquipBaseInfoTbl =  EquipMgrDB.GetEquipAllInfo( equipId )			
			local tbl =
							{ ["EquipRet_New"] = newEquipBaseInfoTbl,
								["EquipRet_Old"] = oldEquipBaseInfoTbl,
								["ScrollId"] = scrollId,
								["Flag"] = flag,
								["equipId"] = equipId,
								["Part"] = part
							}
			
			return true,tbl
		elseif identify_equipTrend == "攻击浮动" then
			if 0 ~= identify_equipTrendValue then
				newDPSFloorRate = identify_equipTrendValue
			else
				local DPSFloorRateLower = equipInfo("DPSFloorRateLower")
				local DPSFloorRateUp = equipInfo("DPSFloorRateUp")
				newDPSFloorRate = GetRandomAttackSpeedOrFloorRate(DPSFloorRateLower,DPSFloorRateUp)
			end
			EquipIdentifySql._UpdateWeaponDPSFloorRate:ExecSql("",newDPSFloorRate,equipId)
			local DelRet = g_RoomMgr.DelItemByPos(uCharId,scrollRoomIndex,scrollPos,1,event_type_tbl["物品使用"])
			if IsNumber(DelRet) then
				CancelTran()
				return false
			end
			newEquipBaseInfoTbl = EquipMgrDB.GetEquipAllInfo( equipId )		
			local tbl =
							{ ["EquipRet_New"] = newEquipBaseInfoTbl,
								["EquipRet_Old"] = oldEquipBaseInfoTbl,
								["ScrollId"] = scrollId,
								["Flag"] = flag,
								["equipId"] = equipId,
								["Part"] = part
							}
			
			return true,tbl
		end
	end
end

SetDbLocalFuncType(EquipIdentifyDB.ArmorOrJewelryIdentify)
SetDbLocalFuncType(EquipIdentifyDB.WeaponIdentify)
return EquipIdentifyDB
