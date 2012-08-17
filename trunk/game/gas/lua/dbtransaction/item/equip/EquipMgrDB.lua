gac_gas_require "item/Equip/EquipCommonFunc"
gac_gas_require "item/item_info_mgr/ItemInfoMgr"
gac_gas_require "item/Equip/EquipDefInc"
cfg_load "equip/upgrade/EquipUpgrade_Common"
cfg_load "equip/RingBaseValue_Common"
cfg_load "item/Equip_WeaponSProperty_Common"
cfg_load "item/Equip_ShieldSProperty_Common"
cfg_load "item/Equip_RingSProperty_Common"

local GetEquipFxPhase       =   GetEquipFxPhase
local LogErr                =   LogErr
local GetIntensifyNeededSoul =GetIntensifyNeededSoul
local EEquipPart 			= 	EEquipPart				--只读，相当于静态配置表
local EquipUpgrade_Common	=	EquipUpgrade_Common
local WeaponDPSFloor_Common = WeaponDPSFloor_Common
local RingBaseValue_Common	= 	RingBaseValue_Common
local Equip_WeaponSProperty_Common = Equip_WeaponSProperty_Common
local Equip_ShieldSProperty_Common = Equip_ShieldSProperty_Common
local Equip_RingSProperty_Common = Equip_RingSProperty_Common
local g_ItemInfoMgr 		= 	CItemInfoMgr:new()
local g_GetDefaultResID 	= 	g_GetDefaultResID  --只读，相当于配置表
local GetRandomScale		=	GetRandomScale	--只读，相当于配置表
local GetRingStaticPropertyName = GetRingStaticPropertyName --只读，相当于配置表
local GetStaticPropertyName = GetStaticPropertyName --只读，相当于配置表
local g_sParseWeaponTblServer = g_sParseWeaponTblServer
local GetBaseWeaponDPS = GetBaseWeaponDPS
local GetEquipOutputAttr = GetEquipOutputAttr
local event_type_tbl = event_type_tbl

local EquipMgrDB 		=	CreateDbBox(...)
local Weapon 			= 	{}
local Armor				=	{}
local Shield			=	{}
local Ring				=	{}
local EquipMgr			=	{}
local EquipType			=	{
["Weapon"]	=	5,
["Armor"]	=	6,
["Shield"]	=	7,
["Ring"]	=	8,
["Jewelry"]	=	9
}

local FromPlace ={}
FromPlace.Bag =1			--从包裹中取得的
FromPlace.StatusWnd =2		--从主角属性面板取得的

--获取升级时候需要的魂的数目
local function GetUpgradeNeededSoul(level)
	return math.floor(EquipUpgrade_Common(level,"VanishSoulCount"))
end

------------------------------------------------------------------------
local StmtDef = {
	"_GetSoulCount",
	[[ select ip_uSoulNum from tbl_item_pearl where is_uId = ? ]]
}
DefineSql(StmtDef, EquipMgr)

local StmtDef ={
    "_InsertIntensifySoulNum",
    [[
        insert into tbl_item_equip_intensify set is_uId = ?, iei_uIntensifySoulNum = ?
    ]]
}
DefineSql(StmtDef, EquipMgr)

local StmtDef ={
    "_GetSoulUseTime",
    [[
       select ipl_uLimitTime from tbl_item_pearl_limit 
       where cs_uId = ? and ipl_uItemType = ? and ipl_uLimitType = ?
    ]]
}
DefineSql(StmtDef, EquipMgr)

local StmtDef ={
    "_CountSoulUseTime",
    [[
       select ifnull(sum(ipl_uLimitTime),0) from tbl_item_pearl_limit 
       where cs_uId = ? and ipl_uLimitType = ?
    ]]
}
DefineSql(StmtDef, EquipMgr)

local StmtDef ={
    "_AddSoulUseTime",
    [[
       replace into tbl_item_pearl_limit(cs_uId,ipl_uItemType,ipl_uLimitType,ipl_uLimitTime) values(?,?,?,?)
    ]]
}
DefineSql(StmtDef, EquipMgr)
--@brief 将魂珠的魂值加到玩家身上
function EquipMgrDB.AvargeSoulCountToPlayer(data)
	local nRoomIndex		=	data["RoomIndex"]
	local nPos				=	data["Pos"]
	local uCharId			=	data["CharID"]
	local sSceneName		=	data["sceneName"]
	local result = {}
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local sPearlType, sPearlName, uPearlId = g_RoomMgr.GetOneItemByPosition(uCharId, nRoomIndex, nPos)
	if sPearlType  == nil or sPearlName == nil or uPearlId == nil then
		return false, 160001
	end
	local BaseLevel = g_ItemInfoMgr:GetItemInfo(sPearlType, sPearlName,"BaseLevel")
	local ex = RequireDbBox("Exchanger")
	local _,_,char_level = ex.getPlayerNameAndLevelById(uCharId)
	if char_level < BaseLevel then
		return false,801
	end
	local soulRet = EquipMgr._GetSoulCount:ExecSql("n", uPearlId)
	if soulRet:GetRowNum() == 0 then
		return false,182002
	end	
	local soulCount = soulRet:GetDataByName(0, "ip_uSoulNum")
	local ret1,eventId = g_RoomMgr.DelItemByID(uCharId, uPearlId,event_type_tbl["物品使用"])
	if (not ret1) then
		CancelTran()
		return false, 182003
	end
	local SoulType = g_ItemInfoMgr:GetItemInfo(sPearlType, sPearlName,"SoulType")
	local nLimitType = g_ItemInfoMgr:GetItemInfo(sPearlType, sPearlName,"LimitType")
	if nLimitType and nLimitType > 0 then
		local LimitInfo = g_ItemInfoMgr:GetSoulUseLimitInfo(nLimitType)
		local tblCount = EquipMgr._CountSoulUseTime:ExecStat(uCharId,nLimitType)
		local now_time = tblCount(1,1)
		if now_time >= LimitInfo("LimitTime") then
				CancelTran()
				return false,32
		end
	end
	local nLimitTime = 0
	local SoulUseTime = EquipMgr._GetSoulUseTime:ExecStat(uCharId,SoulType,nLimitType)
	if SoulUseTime:GetRowNum() > 0 then
		nLimitTime = SoulUseTime(1,1)
	end
	EquipMgr._AddSoulUseTime:ExecStat(uCharId,SoulType,nLimitType,nLimitTime+1)
	if g_DbChannelMgr:LastAffectedRowNum() < 1 then
		CancelTran()
		return
	end
	if SoulType == 1 then
		--魂珠
		local param = {["soulCount"] = soulCount,["PlayerId"] = uCharId,["eventId"] = eventId}
		local bFlag,total_soul = EquipMgrDB.ModifyPlayerSoul(param)
		if bFlag then
			result = {
								["PearlID"]=uPearlId,
								["total_soul"] = total_soul
							}
			return true, result,soulCount
		else
			CancelTran()
			return false, 182003
		end
	elseif SoulType == 2 then
		--经验球
		local params = {}
		params.char_id = uCharId
		params.addExp = soulCount
		params.uEventId = eventId
		local RoleQuestDB = RequireDbBox("RoleQuestDB")
		local CurLevel,LevelExp = RoleQuestDB.AddExp(params)
		local tblRetRes = {}
		tblRetRes.PearlID = uPearlId
		tblRetRes.CurLevel = CurLevel
		tblRetRes.LevelExp = LevelExp
		tblRetRes.AddExp = soulCount
		return true,tblRetRes,"AddExp"
	elseif SoulType == 3 then --积分球
		local AreaFbPointType	= g_ItemInfoMgr:GetItemInfo(sPearlType, sPearlName,"AreaFbPointType")
		local AreaFbPointDB		= RequireDbBox("AreaFbPointDB")
		local bFlag,uMsgID		= AreaFbPointDB.AddAreaFbPointByType(uCharId,soulCount,AreaFbPointType,sSceneName,eventId)
		local tblRetRes = {}
		tblRetRes.soulCount = soulCount
		tblRetRes.AreaFbPointType = AreaFbPointType
		return bFlag, tblRetRes, "AreaFbPoint"
	end
end
------------------------------------------------------------------------------------
--查看玩家身上还剩下多少魂值
local StmtDef = {
	"_SelectPlayerSoulNum",
	[[ select cs_uSoulNum from tbl_char_soul where cs_uId = ?]] 
}
DefineSql(StmtDef, EquipMgr)

local StmtDef = {
	"_HaveEnoughSoulSql",
	[[ select count(*)  from tbl_char_soul where cs_uId = ? and cs_uSoulNum + ? >= 0]] 
}
DefineSql(StmtDef, EquipMgr)

function EquipMgrDB.HaveEnoughSoul(uCharId,soulCount)
	local res = EquipMgr._HaveEnoughSoulSql:ExecStat(uCharId,soulCount)
	return res:GetData(0,0) == 1
end

--修改玩家身上的魂值
function EquipMgrDB.ModifyPlayerSoul(data)
	local soulCount = data["soulCount"]
	local uCharId  = data["PlayerId"]
	local addSoulType = data["addSoulType"]
	local eventId = data["eventId"]
	local errorLogFlag = data["errorLogFlag"]
	local total_soul = 0
	if soulCount ~= 0 then
		if soulCount < 0 then 
			if not EquipMgrDB.HaveEnoughSoul(uCharId,soulCount) then
				return false
			end
		end
		local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
		if not CharacterMediatorDB.UpdateCharSoul(uCharId,soulCount) then	
			local soul_set = EquipMgr._SelectPlayerSoulNum:ExecSql("n", uCharId)
			if soul_set:GetRowNum() > 0 then
				total_soul = soul_set:GetData(0,0)
			end
			LogErr("修改魂值失败","玩家id:" .. uCharId .. ",要修改的魂值：" .. soulCount .. ",原来的魂值：" .. total_soul)
			error("修改魂值失败")
		end
	end
	local soul_set = EquipMgr._SelectPlayerSoulNum:ExecSql("n", uCharId)
	if soul_set:GetRowNum() > 0 then
		total_soul = soul_set:GetData(0,0)
	end
	local g_LogMgr = RequireDbBox("LogMgrDB")
	local succ = g_LogMgr.AddSoulByEventId(eventId,soulCount,addSoulType,uCharId,total_soul)
	if not succ then
		error("记录修改魂值log失败")
	end
	return true,total_soul
end

------------------------------------将魂珠从装备中炼化出来---------------------------------------------------
--如果装备是从角色属性面板取得的fromPlace为1，uItemId即为eEquipPart
--从包裹取得fromPlace为2, uItemId即为EquipId
function EquipMgrDB.GetEquipInfoByEquipFromPlace(equipInfoTbl, fromPlace, uItemId)
	local equipId = nil
	if fromPlace == FromPlace.Bag then
		equipId = uItemId
		
	elseif fromPlace == FromPlace.StatusWnd then
		if equipInfoTbl == nil then
			return false
		end
		equipId = equipInfoTbl[3]
	end
	local equipType, equipName, equipRet = EquipMgrDB.GetEquipBaseInfo(equipId)
	
	return equipType, equipName, equipRet
end

----根据装备的类型得到装备的未使用魂值和绑定类型
function EquipMgrDB.GetUnusedSoulNumAndBindingType(equipType, equipRet)
	local unusedSoulNum, bingType 
	if equipType == EquipType["Weapon"] then
		unusedSoulNum, bingType  = equipRet:GetDataByName(0, "iw_uUnusedSoulNum"), equipRet:GetDataByName(0, "isb_bIsbind")

	elseif equipType == EquipType["Armor"] or 
		 equipType == EquipType["Jewelry"] then 
		unusedSoulNum, bingType  = equipRet:GetDataByName(0, "ia_uUnusedSoulNum"), equipRet:GetDataByName(0, "isb_bIsbind")
	
	elseif equipType == EquipType["Shield"] then
		unusedSoulNum, bingType  = equipRet:GetDataByName(0, "is_uUnusedSoulNum"), equipRet:GetDataByName(0, "isb_bIsbind")
	
	elseif equipType == EquipType["Ring"] then
		unusedSoulNum, bingType  = equipRet:GetDataByName(0, "ir_uUnusedSoulNum"), equipRet:GetDataByName(0, "isb_bIsbind")
	end
	return unusedSoulNum, bingType 
end
-------------------------------查询装备数据库信息----------------------------------------
--获取武器的动态信息
local StmtDef = {
	"_GetWeaponDynInfo",
	[[
		select 
			weapon.iw_sName,ifnull(binding.isb_bIsbind,0), weapon.iw_uBaseLevel, weapon.iw_uCurLevel, weapon.iw_uDPS,  
			weapon.iw_uAttackSpeed,weapon.iw_uDPSFloorRate,
			weapon.iw_uIntensifyQuality, weapon.iw_sIntenSoul
		from 
			tbl_item_weapon as weapon
		left join tbl_item_is_binding as binding
		on(weapon.is_uId = binding.is_uId)
		where 
			weapon.is_uId = ?
	]]
}
DefineSql(StmtDef, Weapon)

--查询武器动态信息
function EquipMgrDB.GetWeaponDynInfo(uItemId)
	--查询
	local ret = Weapon._GetWeaponDynInfo:ExecStat(uItemId)
	if ret:GetRowNum() == 0 then
		ret:Release()
		return {}
	end
	return ret
end
	
--从防具数据库，读动态信息
local StmtDef = {
	"_GetArmorDynInfo",
	[[
		select 
			armor.ia_sName, ifnull(binding.isb_bIsbind,0), armor.ia_uBaseLevel, armor.ia_uCurLevel, 
			armor.ia_uUpgradeAtrrValue1,armor.ia_uUpgradeAtrrValue2,armor.ia_uUpgradeAtrrValue3,ia_uStaticProValue,
			armor.ia_uIntensifyQuality, armor.ia_sIntenSoul
		from 
			tbl_item_armor as armor
		left join tbl_item_is_binding as binding
			on(armor.is_uId = binding.is_uId)
		where 
			armor.is_uId = ?
	]]
}
DefineSql(StmtDef, Armor)

function EquipMgrDB.GetArmorDynInfo(uItemID)
	local ret = Armor._GetArmorDynInfo:ExecStat(uItemID)
	if ret:GetRowNum() == 0 then
		ret:Release()
		return {}
	end
	return ret
end

--获取盾牌的动态信息
local StmtDef = {
	"_GetShieldDynInfo",
	[[
		select 
			shield.is_sName, ifnull(binding.isb_bIsbind,0), shield.is_uBaseLevel, shield.is_uCurLevel, 
			shield.is_uIntensifyQuality, 0, 0, 0, 0, shield.is_sIntenSoul 
		from 
			tbl_item_shield as shield
		left join tbl_item_is_binding as binding
		on(shield.is_uId = binding.is_uId)
		where 
			shield.is_uId = ?
	]]
}
DefineSql(StmtDef, Shield)

--获取静态盾牌的动态信息
local StmtDef = {
	"_GetStaticShieldDynInfo",
	[[
		select 
			shield.is_sName, ifnull(binding.isb_bIsbind,0), shield.is_uBaseLevel, shield.is_uCurLevel,  
			shield.is_uIntensifyQuality,
			shieldAttr.isa_uAttrValue1,shieldAttr.isa_uAttrValue2,shieldAttr.isa_uAttrValue3,shieldAttr.isa_uAttrValue4, shield.is_sIntenSoul 
		from 
			tbl_item_is_binding as binding
		right join tbl_item_shield as shield
			on(shield.is_uId = binding.is_uId)
		left outer join
			tbl_item_shield_Attr as shieldAttr
			on (shield.is_uId = shieldAttr.is_uId)
		where 
			shield.is_uId = ?
	]]
}
DefineSql(StmtDef, Shield)

function EquipMgrDB.GetShieldDynInfo(uItemID)
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local sType, sName = g_RoomMgr.GetItemType(uItemID)
	local nBigId = tonumber(sType)
	local IsShield = g_ItemInfoMgr:GetItemInfo(nBigId, sName,"IsShield")
	local IsStatic = g_ItemInfoMgr:GetItemInfo(nBigId, sName,"IsStatic")
	local ret
	--查询
	if IsShield == 1 and IsStatic == 1 then
		ret = Shield._GetStaticShieldDynInfo:ExecStat(uItemID)
	else
		ret = Shield._GetShieldDynInfo:ExecStat(uItemID)
	end
	return ret
end
---------------------------------------------------------------------------------------------------------
--查询戒指的动态信息
local StmtDef = {
	"_GetRingDynInfo",
	[[
		select 
			ring.ir_sName, binding.isb_bIsbind, ring.ir_uBaseLevel, ring.ir_uCurLevel,ring.ir_uDPS,ring.ir_uStaticProValue,
			ring.ir_uIntensifyQuality, ring.ir_sIntenSoul
		from 
			tbl_item_ring as ring
		left join tbl_item_is_binding as binding
			on(ring.is_uId = binding.is_uId)
		where 
			ring.is_uId = ?
	]]
}
DefineSql(StmtDef, Ring)

function EquipMgrDB.GetRingDynInfo(uItemID)
	--查询
	local ret = Ring._GetRingDynInfo:ExecStat(uItemID)
	if ret:GetRowNum() == 0 then
		ret:Release()
		return {}
	end
	return ret
end

local function GetEquipBaseInfoRet(uItemId)
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local sType, sName 	= 	g_RoomMgr.GetItemType(uItemId)
	local nEquipType	= 	tonumber(sType)
	local dynInfoRet	=	nil
	if nEquipType == EquipType["Weapon"] then			--是武器
	 	dynInfoRet = EquipMgrDB.GetWeaponDynInfo(uItemId)
	 
	elseif nEquipType == EquipType["Armor"] or
			nEquipType == EquipType["Jewelry"] then		--是防具
		dynInfoRet = EquipMgrDB.GetArmorDynInfo(uItemId)
	
	elseif nEquipType == EquipType["Shield"] then		--盾牌
		dynInfoRet = EquipMgrDB.GetShieldDynInfo(uItemId)
		
	elseif nEquipType == EquipType["Ring"] then			--戒指
		dynInfoRet = EquipMgrDB.GetRingDynInfo(uItemId)
	end
	return nEquipType, sName, dynInfoRet
end

--得到装备的基本信息，通过装备id得到Type和Name， 查询装备的数据库信息
function EquipMgrDB.GetEquipBaseInfo( uItemId )
	local nEquipType, sName, dynInfoRet = GetEquipBaseInfoRet(uItemId)
	return nEquipType, sName, dynInfoRet
end

--得到装备的基本信息，通过装备id得到Type和Name， 查询装备的数据库信息
function EquipMgrDB.GetEquipBaseInfoTbl( uItemId )
    local nEquipType, sName, equipBaseInfoRet = GetEquipBaseInfoRet(uItemId)
    if equipBaseInfoRet == nil then
        LogErr("装备类型错误！", "ItemId:" .. ( uItemId or "-" ) .. "EquipType:" .. ( nEquipType or "-" ) )
    end
    return equipBaseInfoRet 
end


-------------------------------------装备注入强化魂---------------------------------------
--武器灌输升级魂
local StmtDef = {
	"_PourEquipIntensifySoulNum",
	[[
		update 
			tbl_item_equip_intensify
		set 
			iei_uIntensifySoulNum = iei_uIntensifySoulNum + ?
		where 
			is_uId = ?
	]]
}
DefineSql(StmtDef, EquipMgr)


--根据装备type，装备数据库查询结果，得到 curLevel, intensifyPhase, intensifySoulNum, unusedSoulNum, sqlStatement 
function EquipMgrDB.GetPourIntensifySoulInfo( nEquipType, equipRet, uItemId )
	local curLevel, intensifyQuality

    local sqlStatement  		= EquipMgr._PourEquipIntensifySoulNum
	if nEquipType == EquipType["Weapon"] then			--是武器
		curLevel			= equipRet:GetDataByName(0, "iw_uBaseLevel")
		intensifyQuality    = equipRet:GetDataByName(0, "iw_uIntensifyQuality")
		
	elseif nEquipType == EquipType["Armor"] or
			nEquipType == EquipType["Jewelry"] then		--是防具
	    curLevel			= equipRet:GetDataByName(0, "ia_uBaseLevel")
	    intensifyQuality    = equipRet:GetDataByName(0, "ia_uIntensifyQuality")
	
	elseif nEquipType == EquipType["Shield"] then		--盾牌
		curLevel			= equipRet:GetDataByName(0, "is_uBaseLevel")
		intensifyQuality    = equipRet:GetDataByName(0, "is_uIntensifyQuality")
	
	elseif nEquipType == EquipType["Ring"] then			--戒指
		curLevel			= equipRet:GetDataByName(0, "ir_uBaseLevel")
		intensifyQuality    = equipRet:GetDataByName(0, "ir_uIntensifyQuality")
	end
	return curLevel, intensifyQuality, sqlStatement
end

--装备灌输强化魂
function EquipMgrDB.PourIntensifySoul(data)
	local uItemId  		=	data["ItemID"]
	local uCharId  		= 	data["CharID"]
	local uSoulNum		=	data["SoulNum"]

	assert(IsNumber(uItemId))
	assert(IsNumber(uCharId))
	assert(IsNumber(uSoulNum))
	local nEquipType, sEquipName, equipRet = EquipMgrDB.GetEquipBaseInfo( uItemId )

	if nEquipType == nil or sEquipName == nil or equipRet == nil then
		return false, "信息不合法!"
	end
	
	local soul_set = EquipMgr._SelectPlayerSoulNum:ExecSql("n", uCharId)
	if soul_set and soul_set:GetRowNum() > 0 then
		local current_soul = soul_set:GetData(0,0)
		if uSoulNum > current_soul then
			local errorMsg = "魂值不足,强化失败!"
			return false, errorMsg
		end
	else
		local errorMsg = "魂值查询出现异常,强化失败!"
		return false, errorMsg
	end
	local EquipIntensifyDB				=	RequireDbBox("EquipIntensifyDB")
	local intensifyInfoRet = EquipIntensifyDB.GetEquipIntensifyInfoTbl(uItemId)
	local intensifyPhase		=  0
	local intensifySoulNum = 0
	if intensifyInfoRet:GetRowNum() > 0 then
        intensifyPhase =  intensifyInfoRet:GetDataByName(0, "iei_uIntensifyPhase")  or 0
        intensifySoulNum= intensifyInfoRet:GetDataByName(0, "iei_uIntensifySoulNum") or 0
    end
	local curLevel, intensifyQuality, sqlStatement = EquipMgrDB.GetPourIntensifySoulInfo( nEquipType, equipRet, uItemId )
	
    if intensifyQuality == nil or intensifyQuality == 0 then
		local errorMsg = "该装备不能被强化!"
		return false,errorMsg
	end
	
	if 	intensifyPhase >= intensifyQuality then
		local errorMsg = "已经是最高阶段，不能再强化了!"
		return false, errorMsg
	end
	
	if uSoulNum == 0 then
		local errorMsg = "要注入的强化魂数目为0!"
		return false, errorMsg
	end
	if curLevel > 150 then
		return false, "最高等级150！"
	end
	if intensifySoulNum + uSoulNum > GetIntensifyNeededSoul(curLevel, g_ItemInfoMgr:GetItemInfo(nEquipType,sEquipName,"EquipType")) then
		local errorMsg = "溢出啦!"
		return false, errorMsg
	end

	if intensifyInfoRet:GetRowNum() == 0 then
	   EquipMgr._InsertIntensifySoulNum:ExecSql("", uItemId, uSoulNum )
	else
	    sqlStatement:ExecSql("", uSoulNum, uItemId)
	end
	
	if (g_DbChannelMgr:LastAffectedRowNum() > 0) then
		local param = {["soulCount"] = -uSoulNum,["PlayerId"] = uCharId,["addSoulType"] = event_type_tbl["装备强化灌输魂"]}
		local bFlag,total_soul = EquipMgrDB.ModifyPlayerSoul(param)
		if not bFlag then
			local errorMsg = "数据库更新失败！"
			return false,errorMsg
		end
		
		local res = EquipMgrDB.SelectEquipIntensifyInfo(uItemId)
		if res:GetRowNum() > 0 then
			local g_LogMgr = RequireDbBox("LogMgrDB")
			g_LogMgr.SaveEquipIntensifyInfo( uCharId, res)
		end
		return true, nil, total_soul
	else
		CancelTran()
		local errorMsg = "注入强化混操作未成功!"
		return false, errorMsg
	end
end
---------------------------------装备升级------------------------------------------------
local StmtDef = {
	"_GetPlayerCurLevel",
	[[
		select cb_uLevel from tbl_char_basic where cs_uId = ?
	]]
}
DefineSql(StmtDef,EquipMgr)

--检查装备升级的前提条件是否满足
local function SatisfyUpgradeCondition(playerID, upgradeSoulNum, curLevel, baseLevel)
	assert(IsNumber(playerID))
	local playerCurLevelRet = EquipMgr._GetPlayerCurLevel:ExecSql("n", playerID)
	local playerCurLevel = playerCurLevelRet:GetDataByName(0, "cb_uLevel")
	if upgradeSoulNum < GetUpgradeNeededSoul(curLevel) then
		return false, 181000
	end
	
	if (playerCurLevel <= curLevel) then
		return false, 181001
	end
	
	if (curLevel - baseLevel >= 10) then
		return false, 181002
	end
	return true
end

--升级装备的附加属性信息
function EquipMgrDB.UpgradeEquipAddInfo(curLevel, equipID, equipType)
	local nextLevel  = curLevel + 1
	local curDPSUpgradeFun = EquipUpgrade_Common(curLevel,"UpgradeParameter")
	local nextDPSUpgradeFun = EquipUpgrade_Common(nextLevel,"UpgradeParameter")
	local EquipIntensifyDB				=	RequireDbBox("EquipIntensifyDB")
	local equipAddInfoTbl = EquipIntensifyDB.GetEquipAddAttrInfoTbl(equipID)
	if # equipAddInfoTbl  > 0 then --表明有4~9阶段的附加属性信息
        local upgradeScale = nextDPSUpgradeFun / curDPSUpgradeFun
        local newAddAttr4Value = string.format("%0.f", equipAddInfoTbl[2] * upgradeScale)
        local newAddAttr5Value = string.format("%0.f", equipAddInfoTbl[4] * upgradeScale)
        local newAddAttr6Value = string.format("%0.f", equipAddInfoTbl[6] * upgradeScale)
        local newAddAttr7Value = string.format("%0.f", equipAddInfoTbl[8] * upgradeScale)
        local newAddAttr8Value = string.format("%0.f", equipAddInfoTbl[10] * upgradeScale)
        local newAddAttr9Value = string.format("%0.f", equipAddInfoTbl[12] * upgradeScale)
        
        local EquipIntensifyDB				=	RequireDbBox("EquipIntensifyDB")
        local sqlTblName, sqlKeyStr = EquipIntensifyDB.GetSqlKeyInfoForEquipAddAttr(equipType)
        local sqlStatement = "update " .. sqlTblName .. " set " .. sqlKeyStr .."uAddAttr4Value = " .. newAddAttr4Value .. ", "
        sqlStatement = sqlStatement .. sqlKeyStr .. "uAddAttr5Value = " .. newAddAttr5Value .. ", " .. sqlKeyStr .. "uAddAttr6Value = " .. newAddAttr6Value .. ", "
        sqlStatement = sqlStatement .. sqlKeyStr .. "uAddAttr7Value = " .. newAddAttr7Value .. ", " .. sqlKeyStr .. "uAddAttr8Value = " .. newAddAttr8Value .. ", "
        sqlStatement = sqlStatement .. sqlKeyStr .. "uAddAttr9Value = " .. newAddAttr9Value .. " where is_uId = " .. equipID

        g_DbChannelMgr:TextExecute(sqlStatement)
--   	    if g_DbChannelMgr:LastAffectedRowNum() < 1 then
--   	    	CancelTran()
--		    return false, 181003
--		else
--		    return true
--	    end
		return true
    end
    return true
end

------------------------------Equip Mgr----------------------------------
--here to manage all equip include weapon, shield, armor and ring
--so I think we only need 4 tables to store equip, 
--end
--静态装备(盾牌)产出时的四种属性

local StmtDef = {
	"_CreateEquipWeapon",
	[[
		insert into tbl_item_weapon(is_uId, iw_uBaseLevel, iw_uCurLevel,iw_uDPS,iw_uAttackSpeed,iw_uDPSFloorRate,iw_uIntensifyQuality,iw_sIntenSoul)
		values(?,?,?,?,?,?,?,?)
	]]
}
DefineSql(StmtDef, Weapon)

--@brief 产生武器
local function CreateEquipWeapon(charId,uItemId,nBigId,sName,IsStatic,BaseLevel,CurLevel,intensifyQuality)
	local DPS ,DPSFloorRate 
	local EquipType = g_ItemInfoMgr:GetItemInfo(nBigId, sName,"EquipType")
	if IsStatic == 0 or IsStatic == "" then
		DPS = GetBaseWeaponDPS(CurLevel,EquipType)
		DPSFloorRate = WeaponDPSFloor_Common(EquipType,"DPSFloorRate")
	elseif IsStatic == 1 then
		DPS = Equip_WeaponSProperty_Common(sName,"DPS")
		DPSFloorRate = Equip_WeaponSProperty_Common(sName,"DPSFloorRate")
	end
	local AttackSpeed = g_ItemInfoMgr:GetItemInfo(nBigId, sName,"AttackSpeed")
	local intenSoul = g_ItemInfoMgr:GetItemInfo(nBigId, sName,"SoulRoot")
	Weapon._CreateEquipWeapon:ExecStat(uItemId, BaseLevel, CurLevel,DPS,AttackSpeed,DPSFloorRate,intensifyQuality, intenSoul)
	if g_DbChannelMgr:LastAffectedRowNum() == 0 then
		error("武器产生失败")
	end
	local LogMgrDB = RequireDbBox("LogMgrDB")
	LogMgrDB.SaveEquipWeaponInfo( charId,uItemId,BaseLevel,CurLevel,DPS,AttackSpeed,DPSFloorRate,intensifyQuality,intenSoul)
end
-----------------------------------------------------------------------------------
local StmtDef = {
	"_CreateEquipArmorOrJewelry",
	[[
		insert into tbl_item_armor(is_uId,ia_uBaseLevel, ia_uCurLevel,ia_uUpgradeAtrrValue1,ia_uUpgradeAtrrValue2,ia_uUpgradeAtrrValue3,ia_uStaticProValue,ia_uIntensifyQuality, ia_sIntenSoul)
		values(?,?,?,?,?,?,?,?,?)
	]]
}
DefineSql(StmtDef, Armor)

--@brief 产生防具或者项链
local function CreateEquipArmorOrJewelry(charId,uItemId,nBigId, sName,IsStatic,BaseLevel, CurLevel,intensifyQuality)
	local outputAttr1, outputAttr2, outputAttr3,StaticPropertyValue = 0,0,0,0
	if IsStatic == 0 or IsStatic == "" then	
		outputAttr1, outputAttr2, outputAttr3 = GetEquipOutputAttr( nBigId, sName, CurLevel )
		if outputAttr1 == nil or outputAttr2 == nil or outputAttr3 == nil then
			error("产出的装备没有匹配的基础属性")
		end
	elseif IsStatic == 1 then
		local Temptbl = GetStaticPropertyName(nBigId, sName)
		outputAttr1 = Temptbl[1][2]
		outputAttr2 = Temptbl[2][2]
		outputAttr3 = Temptbl[3][2]
		StaticPropertyValue = Temptbl[4] and Temptbl[4][2] or 0 
	end
	local intenSoul = g_ItemInfoMgr:GetItemInfo(nBigId, sName,"SoulRoot")
	Armor._CreateEquipArmorOrJewelry:ExecStat(uItemId,  BaseLevel, CurLevel,outputAttr1,outputAttr2,outputAttr3,StaticPropertyValue,intensifyQuality, intenSoul)
	if g_DbChannelMgr:LastAffectedRowNum() == 0 then
		error("防具或者项链产生失败")
	end
	local LogMgrDB = RequireDbBox("LogMgrDB")
	LogMgrDB.SaveEquipArmorInfo( charId,uItemId,BaseLevel,CurLevel,outputAttr1,outputAttr2,outputAttr3,StaticPropertyValue,intensifyQuality,intenSoul)
end
-----------------------------------------------------------------------------------
local StmtDef = {
	"_CreateEquipRing",
	[[
		insert into tbl_item_ring(is_uId, ir_uBaseLevel,ir_uCurLevel, ir_uDPS,ir_uStaticProValue,ir_uIntensifyQuality, ir_sIntenSoul)
		values(?,?,?,?,?,?,?)
	]]
}
DefineSql(StmtDef, Ring)

--@brief 产生戒指
local function CreateEquipRing(charId,uItemId,sName,IsStatic,BaseLevel,CurLevel,intensifyQuality)
	local DPS,StaticPropertyValue = 0,0
	if IsStatic == 0 or IsStatic == "" then
		DPS = RingBaseValue_Common(CurLevel,"AttackCount")
	elseif IsStatic == 1 then
		DPS = Equip_RingSProperty_Common(sName,"DPS")
		local _,PropertyValue = GetRingStaticPropertyName(Equip_RingSProperty_Common(sName))
		StaticPropertyValue = PropertyValue
	end
	local intenSoul = g_ItemInfoMgr:GetItemInfo(EquipType["Ring"], sName,"SoulRoot")
	Ring._CreateEquipRing:ExecStat(uItemId, BaseLevel,CurLevel, DPS,StaticPropertyValue,intensifyQuality, intenSoul)	
	if g_DbChannelMgr:LastAffectedRowNum() == 0 then
		error("戒指产生失败")
	end				
	local LogMgrDB = RequireDbBox("LogMgrDB")
	LogMgrDB.SaveEquipRingInfo( charId,uItemId,BaseLevel,CurLevel,DPS,StaticPropertyValue,intensifyQuality,intenSoul)
end
-----------------------------------------------------------------------------------
local StmtDef = {
	"_SaveCreateShieldAttr",
	[[
		insert into 
			tbl_item_shield_Attr 
		set 
			is_uId = ?,isa_uAttrValue1 = ?,isa_uAttrValue2 = ?,isa_uAttrValue3 = ?,isa_uAttrValue4 = ?
	]]
}
DefineSql(StmtDef, Shield)

local StmtDef = {
	"_CreateEquipShield",
	[[
		insert into tbl_item_shield(is_uId, is_uBaseLevel,is_uCurLevel,is_uIntensifyQuality, is_sIntenSoul)
		values(?,?,?,?,?)
	]]
}
DefineSql(StmtDef, Shield)

--@brief 产生盾牌或者法器
local function CreateEquipShield(charId,uItemId,nBigId, sName,BaseLevel,CurLevel,intensifyQuality,IsStatic)
	local intenSoul = g_ItemInfoMgr:GetItemInfo(nBigId, sName,"SoulRoot")
	Shield._CreateEquipShield:ExecStat(uItemId,BaseLevel,CurLevel,intensifyQuality, intenSoul)						
	if g_DbChannelMgr:LastAffectedRowNum() == 0 then
		error("盾牌产生失败")
	end
	local LogMgrDB = RequireDbBox("LogMgrDB")
	if tonumber(g_ItemInfoMgr:GetItemInfo(nBigId, sName,"IsShield")) == 1 and IsStatic == 1 then
		local attrValue1,attrValue2,attrValue3,attrValue4
		attrValue1 = Equip_ShieldSProperty_Common(sName,"Defence")
		attrValue2 = Equip_ShieldSProperty_Common(sName,"NatureResistanceValue")
		attrValue3 = Equip_ShieldSProperty_Common(sName,"DestructionResistanceValue")
		attrValue4 = Equip_ShieldSProperty_Common(sName,"EvilResistanceValue")
		Shield._SaveCreateShieldAttr:ExecSql("",uItemId,attrValue1,attrValue2,attrValue3,attrValue4)
		if g_DbChannelMgr:LastAffectedRowNum() == 0 then
			error("盾牌静态信息产生失败")
		end
		LogMgrDB.SaveShieldInfo( charId,uItemId,attrValue1,attrValue2,attrValue3,attrValue4)
	end						
	LogMgrDB.SaveEquipShieldInfo( charId,uItemId,BaseLevel,CurLevel,intensifyQuality,intenSoul)
end
-----------------------------------------------------------------------------------
--@brief 装备产生
function EquipMgrDB.CreateEquip(uItemId,uLevel,charId)
	assert(IsNumber(uItemId))
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local sType, sName = g_RoomMgr.GetItemType(uItemId)
	local nBigId = tonumber(sType)
	local sql = ""
	local BaseLevel = uLevel or (g_ItemInfoMgr:GetItemInfo(nBigId, sName,"BaseLevel") or 1)  --基础等级
	local CurLevel = uLevel or (g_ItemInfoMgr:GetItemInfo(nBigId, sName,"AttrLevel") or 1)  --数值等级
	local intensifyQuality = g_ItemInfoMgr:GetItemInfo(nBigId, sName,"IntensifyQuality")
	local IsStatic = g_ItemInfoMgr:GetItemInfo(nBigId, sName,"IsStatic")
	if g_ItemInfoMgr:IsWeapon(nBigId) then
		CreateEquipWeapon(charId,uItemId,nBigId,sName,IsStatic,BaseLevel,CurLevel,intensifyQuality)					
	elseif g_ItemInfoMgr:IsArmor(nBigId) or g_ItemInfoMgr:IsJewelry(nBigId) then
		CreateEquipArmorOrJewelry(charId,uItemId,nBigId, sName,IsStatic,BaseLevel, CurLevel,intensifyQuality)
	elseif g_ItemInfoMgr:IsRing(nBigId) then
		CreateEquipRing(charId,uItemId,sName,IsStatic,BaseLevel,CurLevel,intensifyQuality)
	elseif g_ItemInfoMgr:IsAssociateWeapon(nBigId) then
		CreateEquipShield(charId,uItemId,nBigId, sName,BaseLevel,CurLevel,intensifyQuality,IsStatic)
	else
		return nil
	end
    
	local data = {["ItemID"] =uItemId, ["AdvancedCurLevel"] = g_ItemInfoMgr:GetItemInfo(nBigId, sName,"AdvancedCurLevel"), ["AdvancedQuality"] =g_ItemInfoMgr:GetItemInfo(nBigId, sName,"AdvancedQuality"),
								["charId"] = charId, ["AdvanceSoul"] = g_ItemInfoMgr:GetItemInfo(nBigId, sName,"AdvancedRoot")}
	--装备产出时，记录装备的进阶相关属性
	local EquipAdvanceDB = RequireDbBox("EquipAdvanceDB")
	EquipAdvanceDB.InsertEquipOutAdvanceInfo(data)
	--装备产出是记录装备耐久信息
	local EquipDurabilityDB  = RequireDbBox("EquipDurabilityDB")
	EquipDurabilityDB.InsertEquipDuraValue(uItemId, BaseLevel,charId)
end

---------------------------------------------------------------------
---替换角色身上的装备
local StmtDef=
{
    "_ReplaceEquip",
    "replace into	tbl_item_equip (cs_uId,is_uId,ce_uEquipPart) values(?,?,?)"
}
DefineSql(StmtDef,EquipMgr)

function EquipMgrDB.ReplaceEquip(PlayerId,nItemID,eEquipPart)
	--Player:SetEquipInfo(nBigID,nIndex,nItemID,eEquipPart)
	EquipMgr._ReplaceEquip:ExecSql("",PlayerId,nItemID,eEquipPart)
	if g_DbChannelMgr:LastAffectedRowNum() > 0 then
		 --穿装备写log
		local LogMgrDB = RequireDbBox("LogMgrDB")
		local uEventId = LogMgrDB.SaveEquipPutOnInfo( PlayerId, nItemID ,eEquipPart)
		return true,uEventId
	else
		return false
	end
end

local StmtDef = {
	"_GetEquipPartById",
	[[ select ce_uEquipPart from tbl_item_equip where is_uId = ? and cs_uId =? ]]
}
DefineSql(StmtDef, EquipMgr)
function EquipMgrDB.GetEquipPartById(PlayerId,nItemID)
	local res = EquipMgr._GetEquipPartById:ExecStat(nItemID,PlayerId)
	if res:GetRowNum() == 0 then return 0 end
	return res:GetData(0,0)
end
--删除身上的装备
local StmtDef=
{
    "_DeleteEquip",
    "delete from tbl_item_equip where cs_uId = ? and ce_uEquipPart = ?;" 
}
DefineSql(StmtDef,EquipMgr)

function EquipMgrDB.DeleteEquip(PlayerId,eEquipPart)
	--Player:ClearEquipInfo(eEquipPart)
	EquipMgr._DeleteEquip:ExecSql("",PlayerId,eEquipPart)
	if g_DbChannelMgr:LastAffectedRowNum() > 0 then
		return true
	else
		return false
	end	
end


--根据部位得到装备的信息
local StmtDef=
{
	"_GetEquipInfo",
	[[
	select
		itype.is_uType, itype.is_sName, itype.is_uId
	from 
		tbl_item_static as itype join tbl_item_equip as ce
			on itype.is_uId = ce.is_uId
	where ce.cs_uId = ? and ce.ce_uEquipPart = ?;
	]]
}
DefineSql(StmtDef,EquipMgr)


function EquipMgrDB.GetEquipInfo(PlayerId,eEquipPart)
	local EquipRet = EquipMgr._GetEquipInfo:ExecSql("ns[32]n",PlayerId,eEquipPart)
	local EquipInfo 
	if EquipRet:GetRowNum() > 0 then
		EquipInfo= {tonumber(EquipRet:GetDataByName(0,"is_uType")),EquipRet:GetDataByName(0,"is_sName"),
					tonumber(EquipRet:GetDataByName(0,"is_uId"))}
		return EquipInfo
	end
end

local StmtDef=
{
    "_Delete2Equip",
    "delete from tbl_item_equip where cs_uId = ? and ce_uEquipPart in(?, ?);" 
}
DefineSql(StmtDef,EquipMgr)
--交换左右戒指
function EquipMgrDB.SwitchTwoRing(PlayerId,RingLeftInfo,RingRightInfo)
	EquipMgr._Delete2Equip:ExecSql("",PlayerId,EEquipPart.eRingLeft,EEquipPart.eRingRight)
	if RingLeftInfo ~= nil then
		EquipMgr._ReplaceEquip:ExecSql("",PlayerId,RingLeftInfo[3],EEquipPart.eRingRight)
		if g_DbChannelMgr:LastAffectedRowNum() < 1 then
            CancelTran()
            return false 
		end
	end
	if RingRightInfo ~= nil then
		EquipMgr._ReplaceEquip:ExecSql("",PlayerId,RingRightInfo[3],EEquipPart.eRingLeft)
		if g_DbChannelMgr:LastAffectedRowNum() < 1 then
            CancelTran()
            return false 
		end
	end
	return true
end

--交换主手和副手武器
function EquipMgrDB.SwitchWeapon(PlayerId,WeaponInfo,AssociateWeaponInfo)
	EquipMgr._Delete2Equip:ExecSql("",PlayerId,EEquipPart.eWeapon,EEquipPart.eAssociateWeapon)
	if WeaponInfo ~= nil then
		EquipMgr._ReplaceEquip:ExecSql("",PlayerId,WeaponInfo[3],EEquipPart.eAssociateWeapon)
	end
	if AssociateWeaponInfo ~= nil then
		EquipMgr._ReplaceEquip:ExecSql("",PlayerId,AssociateWeaponInfo[3],EEquipPart.eWeapon)
	end
	return g_DbChannelMgr:LastAffectedRowNum() >= 1
end

local StmtDef=
{
	"_GetEquipType",
	[[
	select
		itype.is_uType, itype.is_sName, itype.is_uId, ce.ce_uEquipPart,ifnull(binding.isb_bIsbind,0)
	from 
		tbl_item_static as itype join tbl_item_equip as ce 
			on itype.is_uId = ce.is_uId
		left join tbl_item_is_binding as binding
			on itype.is_uId = binding.is_uId
	where ce.cs_uId = ?;
	]]
}
DefineSql(StmtDef,EquipMgr)

--得到身上装备信息
function EquipMgrDB.GetPlayerEquipData(nCharID)
	local res = EquipMgr._GetEquipType:ExecStat(nCharID)
	local tblAll = {}
	local row = res:GetRowNum()
	local ItemMakerMgrDB = RequireDbBox("ItemMakerMgrDB")
	for y=1, row do
		local tmp = {}
		if g_ItemInfoMgr:IsStaticEquip(res:GetData(y-1,0)) then
			tmp["nBigID"],tmp["nIndex"],tmp["nItemID"],tmp["eEquipPart"],tmp["nItemBindingType"] = tonumber(res:GetData(y-1,0)),res:GetData(y-1,1),tonumber(res:GetData(y-1,2)),res:GetData(y-1,3),res:GetData(y-1,4)
			tmp["EquipRet"] = EquipMgrDB.GetEquipAllInfo( tmp["nItemID"])
			ItemMakerMgrDB.SendItemMakerByTblIDs(nCharID,{tmp["nItemID"]})	
			table.insert(tblAll,tmp)
		end
	end
	return tblAll
end


function EquipMgrDB.GetAimPlayerInfo(data)
	local EquipData = EquipMgrDB.GetPlayerEquipData(data["nCharID"])
	data["uCharID"] = data["nCharID"] 
	local FightingEvaluationDB = RequireDbBox("FightingEvaluationDB")
	local suitCountTbl,uDPS,uIntensifyPoint = FightingEvaluationDB.GetFightingEvaluationDB(data)
	return {EquipData,{suitCountTbl,uDPS,uIntensifyPoint}}
end

--根据装备ID，得到相关的强化阶段
function EquipMgrDB.GetEquipIntenPhaseByID(equipID)
    local sql = "select ifnull(iei_uIntensifyPhase, 0) from tbl_item_equip_intensify where is_uId = "
    sql = string.format("%s%s", sql, equipID)
    local _, ret = g_DbChannelMgr:TextExecute(sql)
    if ret:GetRowNum() > 0 then
        return ret:GetData(0, 0)
    else
        return 0
    end
end

--根据装备ID，得到相关的进阶阶段
function EquipMgrDB.GetEquipAdvancePhaseByID(equipID)
    local sql = "select iea_uCurAdvancePhase from tbl_item_equip_advance where is_uId = "
    sql = string.format("%s%s", sql, equipID)
    local _, ret = g_DbChannelMgr:TextExecute(sql)
    if ret:GetRowNum() > 0 then
        return ret:GetData(0, 0)
    else
        return 0
    end
end

function EquipMgrDB.GetEquipFxInfo(equipID)
	local equipIntenPhase = EquipMgrDB.GetEquipIntenPhaseByID(equipID)
	local equipAdvancePhase = EquipMgrDB.GetEquipAdvancePhaseByID(equipID)
    local fxPhase = GetEquipFxPhase(equipAdvancePhase, equipIntenPhase)

    return fxPhase
end

--得到身上装备模型信息
function EquipMgrDB.GetEquipResID(nCharID,nClass)
	local res = EquipMgr._GetEquipType:ExecSql("ns[32]nnn",nCharID)
	local uBodyResID,uArmResID,uShoeResID = g_GetDefaultResID(nClass)
	local uWeaponID , uOffWeaponID ,uArmetResID, uShoulderResID, uBackResID=0,0,"","",""
	local uWeaponType=""
	local row = res:GetRowNum()
	local fxPhaseTbl = {}
	fxPhaseTbl["WeaponFxPhase"] = 0
	fxPhaseTbl["OffWeaponFxPhase"] = 0
	fxPhaseTbl["WearFxPhase"] = 0
	fxPhaseTbl["HeadFxPhase"] = 0
	fxPhaseTbl["HandFxPhase"] = 0
    fxPhaseTbl["ShoulderFxPhase"] = 0
    fxPhaseTbl["ShoeFxPhase"] = 0
	for y=1, row do
		local item_type,item_name = res:GetData(y-1,0), res:GetData(y-1,1)
		local equipID = res:GetData(y-1, 2)
		local EquipType = g_ItemInfoMgr:GetItemInfo(item_type,item_name,"EquipType")
		local ResID = g_ItemInfoMgr:GetItemInfo(item_type,item_name,"ResID")
		local eEquipPart = res:GetData(y-1,3)
		if eEquipPart == EEquipPart.eWeapon then 
			uWeaponID = g_sParseWeaponTblServer[ResID]
			uWeaponType= EquipType
			local fxPhase = EquipMgrDB.GetEquipFxInfo(equipID)
		    fxPhaseTbl["WeaponFxPhase"] = fxPhase
		    
		elseif eEquipPart == EEquipPart.eAssociateWeapon then
			local ruslut , HandType = g_ItemInfoMgr:EquipTypeCheck(EquipType,nClass)
			if HandType == "单" then
				uOffWeaponID = g_sParseWeaponTblServer[g_ItemInfoMgr:GetItemInfo(item_type,item_name,"AssociateResID")] or 0
			else
				uOffWeaponID = g_sParseWeaponTblServer[ResID] or 0
			end
            local fxPhase = EquipMgrDB.GetEquipFxInfo(equipID)
		    fxPhaseTbl["OffWeaponFxPhase"] = fxPhase
			
		elseif eEquipPart == EEquipPart.eWear then
			uBodyResID = ResID
            local fxPhase = EquipMgrDB.GetEquipFxInfo(equipID)
		    fxPhaseTbl["WearFxPhase"] = fxPhase
			
		elseif eEquipPart == EEquipPart.eHead then
			uArmetResID = ResID
            local fxPhase = EquipMgrDB.GetEquipFxInfo(equipID)
		    fxPhaseTbl["HeadFxPhase"] = fxPhase
			
		elseif eEquipPart == EEquipPart.eHand then
			uArmResID = ResID
            local fxPhase = EquipMgrDB.GetEquipFxInfo(equipID)
		    fxPhaseTbl["HandFxPhase"] = fxPhase
			
		elseif eEquipPart == EEquipPart.eShoulder then
			uShoulderResID = ResID
            local fxPhase = EquipMgrDB.GetEquipFxInfo(equipID)
		    fxPhaseTbl["ShoulderFxPhase"] = fxPhase
			
		elseif eEquipPart == EEquipPart.eBack then
			uBackResID = ResID
			
		elseif eEquipPart == EEquipPart.eShoe then
			uShoeResID = ResID
            local fxPhase = EquipMgrDB.GetEquipFxInfo(equipID)
		    fxPhaseTbl["ShoeFxPhase"] = fxPhase
		end
	end

	return {uArmetResID, uBodyResID, uBackResID,uShoulderResID, uArmResID,uShoeResID, uWeaponID,uOffWeaponID}, fxPhaseTbl
end

function EquipMgrDB.GetItemInfoByID(data)
	local uItemId = data["ItemId"]
	local char_id = data["CharID"]
	
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local nBigId, nIndex = g_RoomMgr.GetItemType(uItemId)
	if nBigId == nil then
		return false
	end
	local nBindingType = g_RoomMgr.GetItemBindingTypeByID(uItemId)
	local GetItemDynInfoBox = RequireDbBox("GetItemDynInfoDB")
	local infoTbl = GetItemDynInfoBox.GetItemDynInfo(uItemId, nBigId, char_id, char_id, {uItemId})
	
	local result = {}
	result["ItemType"] = nBigId
	result["ItemName"] = nIndex
	result["BindingType"] = nBindingType
	result["ItemID"] = uItemId
	result["ItemInfoTbl"] = infoTbl
	return true,result
end
----------------------------------------------------------------------------------------------------------
--根据玩家Id查看玩家身上的魂值
function EquipMgrDB.GetPlayerSoulNum(data)
	local uCharID = data["uCharID"]
	local playersoul = 0
	local playersoulset = EquipMgr._SelectPlayerSoulNum:ExecSql("n",uCharID)
	if playersoulset and playersoulset:GetRowNum() > 0 then
		playersoul = playersoulset:GetData(0,0)
	end
	return playersoul
end
------------------------------查询装备基础属性---------------------------------------

function EquipMgrDB.GetIntegralDuraValue(curDuraValue)
    local inteDuraValue = curDuraValue
    if curDuraValue >0 and curDuraValue < 1 then
        inteDuraValue = 1
    else
        inteDuraValue = tonumber(string.format("%.0f", curDuraValue))
    end
    return inteDuraValue
end

--根据装备id，查询装备相关的所有信息：基础信息、强化信息、进阶信息
function EquipMgrDB.GetEquipAllInfo(equipID)   
    local equipInfoTbl = {}
    equipInfoTbl["BaseInfo"] = EquipMgrDB.GetEquipBaseInfoTbl(equipID)
    
    local EquipIntensifyDB				=	RequireDbBox("EquipIntensifyDB")
    equipInfoTbl["IntensifyInfo"]  = EquipIntensifyDB.GetEquipIntensifyInfoTbl(equipID)
    equipInfoTbl["IntensifyAddInfo"] = EquipIntensifyDB.GetEquipAddAttrInfoTbl(equipID)
    
    local equipAdvanceDB = RequireDbBox("EquipAdvanceDB")
    equipInfoTbl["AdvanceInfo"]    = equipAdvanceDB.GetEquipAdvaceInfo(equipID)
    
    local EquipEnactmentDB = RequireDbBox("GasArmorPieceEnactmentDB")
    equipInfoTbl["EnactmentInfo"] = EquipEnactmentDB.GetArmorPieceEnactmentAttr(equipID)
    
    local EquipDurabilityDB = RequireDbBox("EquipDurabilityDB")
    local equipDuraState, curDuraValue, maxDuraValue = EquipDurabilityDB.GetEquipUllageState(equipID)

    equipInfoTbl["DuraState"] = equipDuraState
    
    equipInfoTbl["CurDuraValue"] = EquipMgrDB.GetIntegralDuraValue(curDuraValue)
    equipInfoTbl["MaxDuraValue"] = maxDuraValue
    
    local EquipSuperaddDB =  RequireDbBox("EquipSuperaddDB")
    local superaddInfo = EquipSuperaddDB.GetEquipSuperaddInfo(equipID)
    equipInfoTbl["EquipSuperaddInfo"] = superaddInfo
    
    local ItemLifeMgrDB = RequireDbBox("ItemLifeMgrDB")
    local old_time = ItemLifeMgrDB.GetOnlyItemLeftTime(equipID)
		local bTimeOut = ItemLifeMgrDB.IsTimeOutItem(equipID)
		equipInfoTbl.m_tblTimeOut = {old_time,bTimeOut}
    return  equipInfoTbl
end


function EquipMgrDB.GetEquipBaseLevel(equipType, equipID)
    if equipType == nil or equipID == nil then
        return 0
    end

    local EquipIntensifyDB = RequireDbBox("EquipIntensifyDB")
    local tblName, sqlKey = EquipIntensifyDB.GetSqlKeyInfoByEquipType(equipType)
    local sql = "select " .. sqlKey .. "uBaseLevel  from " .. tblName .. " where is_uId = " .. equipID
    local _, equipLevelRet = g_DbChannelMgr:TextExecute(sql)
    if equipLevelRet ~= nil and equipLevelRet:GetRowNum() >0 then
       return equipLevelRet:GetData(0, 0) 
    end
    return 0
end

-------------------------------------------------------------------------------------------
local StmtDef = {
	"_QueryWeaponInfo",
	[[
		select iw_uBaseLevel,iw_uCurLevel,iw_uDPS,iw_uAttackSpeed,iw_uDPSFloorRate,iw_uIntensifyQuality,iw_sIntenSoul from tbl_item_weapon where is_uId = ?
	]]
}
DefineSql(StmtDef, Weapon)

local StmtDef = {
	"_QueryArmorInfo",
	[[
		select ia_uBaseLevel,ia_uCurLevel,ia_uUpgradeAtrrValue1,ia_uUpgradeAtrrValue2,ia_uUpgradeAtrrValue3,ia_uStaticProValue,ia_uIntensifyQuality,ia_sIntenSoul from tbl_item_armor where is_uId = ?
	]]
}
DefineSql(StmtDef, Armor)

local StmtDef = {
	"_QueryRingInfo",
	[[
		select ir_uBaseLevel,ir_uCurLevel,ir_uDPS,ir_uStaticProValue,ir_uIntensifyQuality,ir_sIntenSoul from tbl_item_ring where is_uId = ?
	]]
}
DefineSql(StmtDef, Ring)

local StmtDef = {
	"_QueryShieldInfo",
	[[
		select is_uBaseLevel,is_uCurLevel,is_uIntensifyQuality,is_sIntenSoul from tbl_item_shield where is_uId = ?
	]]
}
DefineSql(StmtDef, Shield)

local StmtDef = {
	"_QueryShieldAttrInfo",
	[[
		select isa_uAttrValue1,isa_uAttrValue2,isa_uAttrValue3,isa_uAttrValue4 from tbl_item_shield_Attr where is_uId = ?
	]]
}
DefineSql(StmtDef, Shield)	

local StmtDef = {
	"_QueryEquipAdvanceInfo",
	[[
		select iea_uCurAdvancePhase,iea_uTotalAdvancedTimes,iea_uSkillPieceIndex1,iea_uSkillPieceIndex2,iea_uSkillPieceIndex3,iea_uSkillPieceIndex4,
		 iea_uChoosedSkillPieceIndex,iea_sJingLingType,iea_uAdvanceSoulNum,iea_uAdvancedTimes,iea_uAdvancedAttrValue1,iea_uAdvancedAttrValue2,
		 iea_uAdvancedAttrValue3,iea_uAdvancedAttrValue4,iea_sAdvancedSoulRoot,iea_sAdvancedAttr1,iea_sAdvancedAttr2,iea_sAdvancedAttr3,
		 iea_sAdvancedAttr4 from tbl_item_equip_advance where is_uId = ?
	]]
}
DefineSql(StmtDef, EquipMgr)	

local StmtDef = {
	"_QueryEquipIntensifyInfo",
	[[
		select is_uId,iei_uIntensifySoulNum,iei_uPreAddAttr1,iei_uPreAddAttrValue1,iei_uPreAddAttr2,iei_uPreAddAttrValue2,iei_uAddAttr1,iei_uAddAttrValue1,
		iei_uAddAttr2,iei_uAddAttrValue2,iei_uIntensifyPhase,iei_uEuqipSuitType,iei_sSuitName,iei_uIntensifyBackState,iei_uIntensifyTimes,iei_uIntensifyBackTimes,iei_uIntenTalentIndex from tbl_item_equip_intensify where is_uId = ?
	]]
}
DefineSql(StmtDef, EquipMgr)		

local StmtDef = {
	"_QueryIntensifyAddAttrInfo",
	[[
		select is_uId,iei_sAddAttr4,iei_uAddAttr4Value,iei_sAddAttr5,iei_uAddAttr5Value,iei_sAddAttr6,iei_uAddAttr6Value,iei_sAddAttr7,
		iei_uAddAttr7Value,iei_sAddAttr8,iei_uAddAttr8Value,iei_sAddAttr9,iei_uAddAttr9Value from tbl_item_equip_intensifyAddAttr where is_uId = ?
	]]
}
DefineSql(StmtDef, EquipMgr)

local StmtDef = {
	"_QueryEquipArmorInfo",
	[[
		select iea_sAttr,iea_uAddTimes,iea_uIndex from tbl_item_equip_armor where is_uId = ?
	]]
}
DefineSql(StmtDef, EquipMgr)	

local StmtDef = {
	"_QueryEquipDurabilityInfo",
	[[
		select ied_uMaxDuraValue,ied_uCurDuraValue from tbl_item_equip_durability where is_uId = ?
	]]
}
DefineSql(StmtDef, EquipMgr)	


local StmtDef = {
	"_QueryEquipSuperaddationInfo",
	[[
		select ies_uCurSuperaddPhase
	     from tbl_item_equip_superaddation where is_uId = ?
	]]
}
DefineSql(StmtDef, EquipMgr)	

local StmtDef = {
	"_QueryEquipMakerInfo",
	[[
		select is_uId,cs_uId from tbl_item_maker where is_uId = ?
	]]
}
DefineSql(StmtDef, EquipMgr)	
    
--@brief 删除装备之前存下装备的动态属性信息
function EquipMgrDB.DelEquipDynamicInfoSaveLog(nCharID,equipId,nBigId)
	local LogMgrDB = RequireDbBox("LogMgrDB")
	if nBigId == 5 then
		local res = Weapon._QueryWeaponInfo:ExecStat(equipId)
		LogMgrDB.SaveEquipWeaponInfo( nCharID,equipId,res(1,1),res(1,2),res(1,3),res(1,4),res(1,5),res(1,6),res(1,7))
	elseif nBigId == 6 or nBigId == 9 then
		local res = Armor._QueryArmorInfo:ExecStat(equipId)
		LogMgrDB.SaveEquipArmorInfo( nCharID,equipId,res(1,1),res(1,2),res(1,3),res(1,4),res(1,5),res(1,6),res(1,7),res(1,8))
	elseif nBigID == 7 then
		local res1 = Shield._QueryShieldInfo:ExecStat(equipId)
		LogMgrDB.SaveEquipShieldInfo( nCharID,equipId,res(1,1),res(1,2),res(1,3),res(1,4))
		local res2 = Shield._QueryShieldAttrInfo:ExecStat(equipId)
		if res2:GetRowNum() > 0 then
			LogMgrDB.SaveShieldInfo( nCharID,equipId,res(1,1),res(1,2),res(1,3),res(1,4))
		end
	elseif nBigId == 8 then
		local res = Ring._QueryRingInfo:ExecStat(equipId)
		LogMgrDB.SaveEquipRingInfo( nCharID,equipId,res(1,1),res(1,2),res(1,3),res(1,4),res(1,5),res(1,6))
	end
	local advanceInfoRet = EquipMgr._QueryEquipAdvanceInfo:ExecStat(equipId)
	if advanceInfoRet:GetRowNum() > 0 then
		LogMgrDB.SaveEquipAdvanceInfo( nCharID,equipId,advanceInfoRet)
	end
	local equipIntensifyRet = EquipMgr._QueryEquipIntensifyInfo:ExecStat(equipId)
	if equipIntensifyRet:GetRowNum() > 0 then
		LogMgrDB.SaveEquipIntensifyInfo( nCharID, equipIntensifyRet)
	end
	local intensifyAddAttr = EquipMgr._QueryIntensifyAddAttrInfo:ExecStat(equipId)
	if intensifyAddAttr:GetRowNum() > 0 then
		LogMgrDB.SaveEquipIntensifyInfoAddAttr( nCharID, intensifyAddAttr)
	end
	local equipArmorfo = EquipMgr._QueryEquipArmorInfo:ExecStat(equipId)
	if equipArmorfo:GetRowNum() > 0 then
		for i = 1,equipArmorfo:GetRowNum() do
			LogMgrDB.SaveArmorIncrustationInfo( nCharID,equipId,equipArmorfo(i,1),equipArmorfo(i,2),equipArmorfo(i,3))
		end
	end
	local durabilityfo = EquipMgr._QueryEquipDurabilityInfo:ExecStat(equipId)
	if durabilityfo:GetRowNum() > 0 then 
		LogMgrDB.SaveEquipDuraInfo( nCharID, equipId,durabilityfo(1,1),durabilityfo(1,2) )
	end
	local superaddationInfo =  EquipMgr._QueryEquipSuperaddationInfo:ExecStat(equipId)
	if superaddationInfo:GetRowNum() > 0 then
		LogMgrDB.SaveEquipSuperAddationInfo( nCharID,equipId,superaddationInfo(1,1))
	end
	local makerInfo = EquipMgr._QueryEquipMakerInfo:ExecStat(equipId)
	if makerInfo:GetRowNum() > 0 then
		LogMgrDB.SaveEquipMakerInfo(makerInfo(1,1),makerInfo(1,2))
	end
end

SetDbLocalFuncType(EquipMgrDB.GetPlayerEquipData)
SetDbLocalFuncType(EquipMgrDB.PourIntensifySoul)
SetDbLocalFuncType(EquipMgrDB.GetItemInfoByID)
SetDbLocalFuncType(EquipMgrDB.AvargeSoulCountToPlayer)
SetDbLocalFuncType(EquipMgrDB.GetPlayerSoulNum)
SetDbLocalFuncType(EquipMgrDB.GetEquipBaseLevel)
SetDbLocalFuncType(EquipMgrDB.ModifyPlayerSoul)
SetDbLocalFuncType(EquipMgrDB.GetAimPlayerInfo)
return EquipMgrDB
