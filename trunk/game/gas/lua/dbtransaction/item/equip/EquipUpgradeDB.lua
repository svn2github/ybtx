----武器升级
--local StmtDef = {
--	"_UpgradeWeapon",
--	[[
--		update 
--			tbl_item_weapon
--		set 
--			iw_uCurLevel = ?, iw_uDPS = ?, 
--			iw_uUpgradeSoulNum = 0, iw_uAddAttrValue1=?, iw_uAddAttrValue2 =?
--		where 
--			is_uId = ?
--	]]
--}
--DefineSql(StmtDef, Weapon)

--武器升级
--function EquipMgrDB.WeaponUpgrade(uItemId,  player, equipRet,sceneName)
--	local upgradeSoulNum = equipRet:GetDataByName(0, "iw_uUpgradeSoulNum")
--	local curLevel		 = equipRet:GetDataByName(0, "iw_uCurLevel")
--	local baseLevel 	 = equipRet:GetDataByName(0, "iw_uBaseLevel")
--	local g_LogMgr = RequireDbBox("LogMgrDB")
--	
--	local checkResult, errorMsgNo = SatisfyUpgradeCondition(player, upgradeSoulNum, curLevel, baseLevel)
--	if checkResult == false then
--		return false, errorMsgNo
--	end
--	
--	local uAddAttr1Value = equipRet:GetDataByName(0, "iw_uAddAttrValue1")
--	local uAddAttr2Value = equipRet:GetDataByName(0, "iw_uAddAttrValue2")
--	local dps			 = equipRet:GetDataByName(0, "iw_uDPS")
--	local nextLevel = curLevel + 1
--	
--	local curDPSUpgradeFun = EquipUpgrade_Common[curLevel].UpgradeParameter
--	local nextDPSUpgradeFun = EquipUpgrade_Common[nextLevel].UpgradeParameter
--	local DPS = string.format("%.1f", dps/curDPSUpgradeFun*nextDPSUpgradeFun)
--	local itensifyAttr1 = string.format("%0.f", uAddAttr1Value/curDPSUpgradeFun*nextDPSUpgradeFun)
--	local itensifyAttr2 = string.format("%0.f", uAddAttr2Value/curDPSUpgradeFun*nextDPSUpgradeFun)
--	
--	Weapon._UpgradeWeapon:ExecSql("", nextLevel, DPS, itensifyAttr1, itensifyAttr2, uItemId)
--	g_LogMgr.SaveEquipUpgradeInfo( player, uItemId,sceneName, nextLevel)
--	if (g_DbChannelMgr:LastAffectedRowNum() > 0) then
--		return true, curLevel
--	else
--		CancelTran()
--		return false, 181003
--	end
--end

--升级
--local StmtDef = {
--	"_UpgradeArmor",
--	[[
--		update 
--			tbl_item_armor
--		set 
--			ia_uCurLevel = ?, 
--			ia_uUpgradeAtrrValue1=?, ia_uUpgradeAtrrValue2=?, ia_uUpgradeAtrrValue3=?, ia_uStaticProValue = ?,
--			ia_uUpgradeSoulNum = 0, ia_uAddAttrValue1= ?, ia_uAddAttrValue2 = ?
--		where 
--			is_uId = ?
--	]]
--}
--DefineSql(StmtDef, Armor)
--防具升级
--function EquipMgrDB.ArmorUpgrade(uItemId, playerID, equipRet,sceneName)
--	local upgradeSoulNum = equipRet:GetDataByName(0, "ia_uUpgradeSoulNum")
--	local curLevel		 = equipRet:GetDataByName(0, "ia_uCurLevel")
--	local baseLevel 	 = equipRet:GetDataByName(0, "ia_uBaseLevel")
--	local g_LogMgr = RequireDbBox("LogMgrDB")
--	local checkResult, errorMsgNo = SatisfyUpgradeCondition(playerID, upgradeSoulNum, curLevel, baseLevel)
--	if checkResult == false then
--		return false, errorMsgNo
--	end
--	
--	local outputAttr1 	 = equipRet:GetDataByName(0, "ia_uUpgradeAtrrValue1")
--	local outputAttr2 	 = equipRet:GetDataByName(0, "ia_uUpgradeAtrrValue2")
--	local outputAttr3 	 = equipRet:GetDataByName(0, "ia_uUpgradeAtrrValue3")
--	local outputStaticProperty = equipRet:GetDataByName(0, "ia_uStaticProValue")
--	local intensifyAttr1 = equipRet:GetDataByName(0, "ia_uAddAttrValue1")
--	local intensifyAttr2 = equipRet:GetDataByName(0, "ia_uAddAttrValue2")
--	local nextLevel		 = curLevel + 1
--	local curDPSUpgradeFun = EquipUpgrade_Common[curLevel].UpgradeParameter
--	local nextDPSUpgradeFun = EquipUpgrade_Common[nextLevel].UpgradeParameter
--	local upgradeScale = nextDPSUpgradeFun/curDPSUpgradeFun
--	outputAttr1 = tonumber(string.format("%.0f", outputAttr1*upgradeScale))
--	outputAttr2 = tonumber(string.format("%.0f", outputAttr2*upgradeScale))
--	outputAttr3 = tonumber(string.format("%.0f", outputAttr3*upgradeScale))
--	outputStaticProperty = tonumber(string.format("%.0f", outputStaticProperty*upgradeScale))
--	intensifyAttr1 =  tonumber(string.format("%.0f", intensifyAttr1*upgradeScale))
--	intensifyAttr2 =  tonumber(string.format("%.0f", intensifyAttr2*upgradeScale))
--	
--	Armor._UpgradeArmor:ExecSql("", nextLevel, outputAttr1, outputAttr2, outputAttr3, outputStaticProperty, intensifyAttr1, intensifyAttr2, uItemId )
--	g_LogMgr.SaveEquipUpgradeInfo( playerID, uItemId,sceneName, nextLevel)
--	if (g_DbChannelMgr:LastAffectedRowNum() > 0) then
--		return true, curLevel
--	else
--		CancelTran()
--		return false, 181003
--	end
--end

----盾牌升级
--local StmtDef = {
--	"_UpgradeShield",
--	[[
--		update 
--			tbl_item_shield
--		set 
--			is_uCurLevel = ?,
--			is_uUpgradeSoulNum = 0, is_uAddAttrValue1 = ?, is_uAddAttrValue2 =?
--		where 
--			is_uId = ?
--	]]
--}
--DefineSql(StmtDef, Shield)

--盾牌升级修改产出属性值

--盾牌升级
--function EquipMgrDB.ShieldUpgrade(uItemId, playerID, equipRet,sceneName)
--	local upgradeSoulNum = equipRet:GetDataByName(0, "is_uUpgradeSoulNum")
--	local curLevel		 = equipRet:GetDataByName(0, "is_uCurLevel")
--	local baseLevel 	 = equipRet:GetDataByName(0, "is_uBaseLevel")
--	local g_LogMgr = RequireDbBox("LogMgrDB")
--	local checkResult, errorMsgNo = SatisfyUpgradeCondition(playerID, upgradeSoulNum, curLevel, baseLevel)
--	if checkResult == false then
--		return false, errorMsgNo
--	end
--	
--	local intensifyAttr1Value = equipRet:GetDataByName(0, "is_uAddAttrValue1")
--	local intensifyAttr2Value = equipRet:GetDataByName(0, "is_uAddAttrValue2")
--	local nextLevel		 = curLevel + 1
--	local curDPSUpgradeFun = EquipUpgrade_Common[curLevel].UpgradeParameter
--	local nextDPSUpgradeFun = EquipUpgrade_Common[nextLevel].UpgradeParameter
--	local upgradeScale = nextDPSUpgradeFun/curDPSUpgradeFun
--	local intensifyAttr1 = tonumber(string.format("%.0f", intensifyAttr1Value * upgradeScale))
--	local intensifyAttr2 = tonumber(string.format("%.0f", intensifyAttr2Value * upgradeScale))
--	Shield._UpgradeShield:ExecSql("", nextLevel, intensifyAttr1, intensifyAttr2, uItemId )
--	
--	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
--	local nBigId, sName = g_RoomMgr.GetItemType(uItemId)
--	local info = g_ItemInfoMgr:GetItemInfo(nBigId, sName)
--	if info.IsShield == 1 and info.IsStatic == 1 then
--		local outputAttr1,outputAttr2,outputAttr3,outputAttr4
--		outputAttr1 	 = equipRet:GetDataByName(0, "isa_uAttrValue1")
--		outputAttr2 	 = equipRet:GetDataByName(0, "isa_uAttrValue2")
--		outputAttr3 	 = equipRet:GetDataByName(0, "isa_uAttrValue3")
--		outputAttr4 	 = equipRet:GetDataByName(0, "isa_uAttrValue4")
--		outputAttr1 = tonumber(string.format("%.0f", outputAttr1*upgradeScale))
--		outputAttr2 = tonumber(string.format("%.0f", outputAttr2*upgradeScale))
--		outputAttr3 = tonumber(string.format("%.0f", outputAttr3*upgradeScale))
--		outputAttr4 = tonumber(string.format("%.0f", outputAttr4*upgradeScale))
--		Shield._UpgradeShieldOutputAttrValue:ExecSql("",outputAttr1,outputAttr2,outputAttr3,outputAttr4,uItemId)
--	end
--	
--	g_LogMgr.SaveEquipUpgradeInfo( playerID, uItemId,sceneName, nextLevel)
--	if (g_DbChannelMgr:LastAffectedRowNum() > 0) then
--		return true, curLevel
--	else
--		CancelTran()
--		return false, 181003
--	end
--end

--戒指升级
--local StmtDef = {
--	"_UpgradeRing",
--	[[
--		update 
--			tbl_item_ring
--		set 
--			ir_uCurLevel = ?, ir_uDPS = ?, 
--			ir_uUpgradeSoulNum = 0, ir_uAddAttrValue1 =?, ir_uAddAttrValue2= ?
--		where 
--			is_uId = ?
--	]]
--}
--DefineSql(StmtDef, Ring)

--戒指升级
--function EquipMgrDB.RingUpgrade(uItemId, playerID, equipRet,sceneName)
--	local upgradeSoulNum = equipRet:GetDataByName(0, "ir_uUpgradeSoulNum")
--	local curLevel		 = equipRet:GetDataByName(0, "ir_uCurLevel")
--	local baseLevel 	 = equipRet:GetDataByName(0, "ir_uBaseLevel")
--	local g_LogMgr = RequireDbBox("LogMgrDB")
--	local checkResult, errorMsgNo = SatisfyUpgradeCondition(playerID, upgradeSoulNum, curLevel, baseLevel)
--	if checkResult == false then
--		return false, errorMsgNo
--	end
--	
--	local dps 				  = equipRet:GetDataByName(0, "ir_uDPS")
--	local intensifyAttr1Value = equipRet:GetDataByName(0, "ir_uAddAttrValue1")
--	local intensifyAttr2Value = equipRet:GetDataByName(0, "ir_uAddAttrValue2")
--	local nextLevel = curLevel + 1
--	
--	local curDPSUpgradeFun = EquipUpgrade_Common[curLevel].UpgradeParameter
--	local nextDPSUpgradeFun = EquipUpgrade_Common[nextLevel].UpgradeParameter
--	local upgradeScale = nextDPSUpgradeFun/curDPSUpgradeFun 
--	local DPS = tonumber(string.format("%.1f", dps * upgradeScale))
--	local intensifyAttr1 = tonumber(string.format("%.0f", intensifyAttr1Value * upgradeScale))
--	local intensifyAttr2 = tonumber(string.format("%.0f", intensifyAttr2Value * upgradeScale))
--
--	Ring._UpgradeRing:ExecSql("", nextLevel, DPS, intensifyAttr1, intensifyAttr2, uItemId)
--	g_LogMgr.SaveEquipUpgradeInfo( playerID, uItemId,sceneName, nextLevel)
--	if (g_DbChannelMgr:LastAffectedRowNum() > 0) then
--		return true, curLevel
--	else
--		CancelTran()
--		return false, 181003
--	end
--end


----武器灌输升级魂
--local StmtDef = {
--	"_PourWeaponUpgradeSoulNum",
--	[[
--		update 
--			tbl_item_weapon
--		set 
--			iw_uUpgradeSoulNum = iw_uUpgradeSoulNum + ?
--		where 
--			is_uId = ?
--	]]
--}
--DefineSql(StmtDef, Weapon)

--防具灌输升级魂
--local StmtDef = {
--	"_PourArmorUpgradeSoulNum",
--	[[
--		update 
--			tbl_item_armor
--		set 
--			ia_uUpgradeSoulNum = ia_uUpgradeSoulNum + ?
--		where 
--			is_uId = ?
--	]]
--}
--DefineSql(StmtDef, Armor)

----盾牌灌输升级魂
--local StmtDef = {
--	"_PourShieldUpgradeSoulNum",
--	[[
--		update 
--			tbl_item_shield
--		set 
--			is_uUpgradeSoulNum = is_uUpgradeSoulNum + ?
--		where 
--			is_uId = ?
--	]]
--}
--DefineSql(StmtDef, Shield)

--戒指灌输升级魂
--local StmtDef = {
--	"_PourRingUpgradeSoulNum",
--	[[
--		update 
--			tbl_item_ring
--		set 
--			ir_uUpgradeSoulNum = ir_uUpgradeSoulNum + ?
--		where 
--			is_uId = ?
--	]]
--}
--DefineSql(StmtDef, Ring)

----根据装备type，装备数据库查询结果，得到 upgradeSoulNum, curLevel, unusedSoulNum, sqlStatement 
--function EquipMgrDB.GetPourUpgradeSoulInfo(nEquipType, equipRet)
--	local upgradeSoulNum, curLevel, unusedSoulNum, sqlStatement
--	if nEquipType == EquipType["Weapon"] then			--是武器
--	 	upgradeSoulNum 	= equipRet:GetDataByName(0, "iw_uUpgradeSoulNum")
--		curLevel		= equipRet:GetDataByName(0, "iw_uCurLevel")
--		sqlStatement  	= Weapon._PourWeaponUpgradeSoulNum
--	
--	elseif nEquipType == EquipType["Armor"] or
--			nEquipType == EquipType["Jewelry"] then		--是防具
--	 	upgradeSoulNum 	= equipRet:GetDataByName(0, "ia_uUpgradeSoulNum")
--		curLevel		= equipRet:GetDataByName(0, "ia_uCurLevel")
--		sqlStatement  	= Armor._PourArmorUpgradeSoulNum
--	
--	elseif nEquipType == EquipType["Shield"] then		--盾牌
--		upgradeSoulNum 	= equipRet:GetDataByName(0, "is_uUpgradeSoulNum")
--		curLevel		= equipRet:GetDataByName(0, "is_uCurLevel")
--		sqlStatement  	= Shield._PourShieldUpgradeSoulNum
--	
--	elseif nEquipType == EquipType["Ring"] then			--戒指
--		upgradeSoulNum 	= equipRet:GetDataByName(0, "ir_uUpgradeSoulNum")
--		curLevel		= equipRet:GetDataByName(0, "ir_uCurLevel")
--		sqlStatement  	= Ring._PourRingUpgradeSoulNum
--	end
--	return upgradeSoulNum, curLevel, sqlStatement
--end

--装备升级入口
--function EquipMgrDB.EquipUpgrade(data)
--	local uItemId = data["ItemID"]
--	local uCharId = data["CharID"]
--	local sceneName = data["sceneName"]
--	assert(IsNumber(uItemId))
--	assert(IsNumber(uCharId))
--	local EquipIntensifyDB				=	RequireDbBox("EquipIntensifyDB")
--	local nEquipType, sEquipName, oldequipRet = EquipMgrDB.GetEquipBaseInfo( uItemId )
--	local oldEquipAddAttrRetTbl = EquipIntensifyDB.GetEquipAddAttrInfoTbl(uItemId) 
--	local suc, upInfo
--	if nEquipType == EquipType["Weapon"] then
--		suc, upInfo = EquipMgrDB.WeaponUpgrade(uItemId, uCharId, oldequipRet,sceneName)
--		
--	elseif nEquipType == EquipType["Armor"]  or 
--		nEquipType == EquipType["Jewelry"] then
--		suc, upInfo  = EquipMgrDB.ArmorUpgrade(uItemId, uCharId, oldequipRet,sceneName)
--		
--	elseif nEquipType == EquipType["Shield"] then
--		suc, upInfo  = EquipMgrDB.ShieldUpgrade(uItemId, uCharId, oldequipRet,sceneName)
--		
--	elseif nEquipType == EquipType["Ring"] then
--		suc, upInfo  = EquipMgrDB.RingUpgrade(uItemId, uCharId, oldequipRet,sceneName)
--	end
--	local _, _, newequipRet = EquipMgrDB.GetEquipBaseInfo( uItemId )
--	
--	if suc then
--		local oldEquipInfoTbl = {}
--		local newEquipInfoTbl = {}
--		local oldEquipRetLen = oldequipRet:GetRowNum()
--		local newEquipRetLen = newequipRet:GetRowNum()
--		local oldEquipBaseInfoTbl = {}
--		local newEquipBaseInfoTbl = {}
--		for i=1, oldEquipRetLen do
--			table.insert(oldEquipBaseInfoTbl, oldequipRet:GetRow(i-1))
--		end
--		for i=1, newEquipRetLen do
--			table.insert(newEquipBaseInfoTbl, newequipRet:GetRow(i-1))
--		end
--		local curLevel		 = upInfo
--		local ret, errorMsgId = EquipMgrDB.UpgradeEquipAddInfo(curLevel, uItemId, nEquipType)
--		if ret == false then
--		    return false, errorMsgId
--		end
--		local newEquipAddAttrRetTbl = EquipIntensifyDB.GetEquipAddAttrInfoTbl(uItemId) 
--		oldEquipInfoTbl = {oldEquipBaseInfoTbl, oldEquipAddAttrRetTbl}
--		newEquipInfoTbl = {newEquipBaseInfoTbl, newEquipAddAttrRetTbl}
--		return true, nEquipType, sEquipName, oldEquipInfoTbl, newEquipInfoTbl
--	else
--		local errorMsgNo = upInfo
--		return false, errorMsgNo
--	end
--end


------------------------------装备注入升级魂-------------------------------------------

--[[

--装备灌输升级魂
function Gac2Gas:PourUpgradeSoul(Conn, uItemId, uSoulNum)
	local data = { ["ItemID"] = uItemId, ["CharID"] = Conn.m_Player.m_uID, ["SoulNum"] = uSoulNum }
	local function CallBack(suc, errorMsg, total_soul)
	 	if suc == false then
	 		MsgToConn(Conn, 8003, errorMsg)
	 		Gas2Gac:NoticePourUpgradeError(Conn, uItemId, uSoulNum)
	 	elseif suc then
	 		Gas2Gac:NoticePourUpgradeSuccess(Conn, uItemId)
	 		Gas2Gac:ReturnModifyPlayerSoulNum(Conn,total_soul)
	 	end
	end
	CallAccountManualTrans(Conn.m_Account, EquipMgrDB, "PourUpgradeSoul", CallBack, data)
end

--装备注入升级魂
function EquipMgrDB.PourUpgradeSoul(data)
	local uItemId  		=	data["ItemID"]
	local uCharId  		= 	data["CharID"]
	local uSoulNum		=	data["SoulNum"]
	assert(IsNumber(uItemId))
	assert(IsNumber(uCharId))
	assert(IsNumber(uSoulNum))
	local total_soul = 0
	local nEquipType, sEquipName, equipRet = EquipMgrDB.GetEquipBaseInfo( uItemId )
	
	local	isUpIntensify = g_ItemInfoMgr:GetItemInfo(nEquipType,sEquipName,"isUpIntensify")
	if tonumber(isUpIntensify) ~= 1 then
		local errorMsg = "该装备不能升级!"
		return false,errorMsg
	end
	
	if uSoulNum == 0 then
		local errorMsg = "要注魂的数目为0，不合法!"
		return false, errorMsg
	end
	if nEquipType == nil or sEquipName == nil or equipRet == nil then
		local errorMsg = "未查找到该装备的相关信息!"
		return false, errorMsg
	end
	
	local soul_set = EquipMgr._SelectPlayerSoulNum:ExecSql("n", uCharId)
	if soul_set and soul_set:GetRowNum() > 0 then
		local current_soul = soul_set:GetData(0,0)
		if uSoulNum > current_soul then
			local errorMsg = "魂值不足,升级失败!"
			return false, errorMsg
		end
	else
		local errorMsg = "魂值查询出现异常,升级失败!"
		return false, errorMsg
	end
	
	local upgradeSoulNum, curLevel, sqlStatement = EquipMgrDB.GetPourUpgradeSoulInfo(nEquipType, equipRet)
	if curLevel >= 150 then
		return false, "已经150级不能升级了!"
	end
	if upgradeSoulNum + uSoulNum > GetUpgradeNeededSoul(curLevel) then
		local errorMsg = "溢出啦!"
		return false, errorMsg
	end
	
	sqlStatement:ExecSql("", uSoulNum, uItemId)
	if (g_DbChannelMgr:LastAffectedRowNum() > 0) then 
		EquipMgr._UpdatePlayerSoulNum:ExecSql("",-uSoulNum,uCharId)
		if g_DbChannelMgr:LastAffectedRowNum() < 1 then
			local errorMsg = "数据库更新失败!"
			CancelTran()
			return false,errorMsg
		end

		local soul_set = EquipMgr._SelectPlayerSoulNum:ExecSql("n", uCharId)
		if soul_set and soul_set:GetRowNum() > 0 then
			total_soul = soul_set:GetData(0,0)
		end
		return true, nil, total_soul
	else
		CancelTran()
		local errorMsg = "注魂，更新数据库未成功!"
		return false, errorMsg
	end
end
--]]