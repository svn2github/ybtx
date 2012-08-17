
cfg_load "equip/ArmorBaseValue_Common"

GacEquipIdentify = class(SQRDialog)

--防具鉴定
function GacEquipIdentify.ArmorOrJewelryIdentify(nBigID,nIndex,itemid,context,equipRoomIndex,equipPos)
	local itemType = context[2]
	local identifyScrollName = context[3]
	local scrollRoomIndex = context[4]
	local scrollPos = context[5]
	
	local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(itemid)
	local curLevel = DynInfo.BaseLevel

	--读取鉴定卷轴配置表
	local identify_equipType = g_ItemInfoMgr:GetItemInfo(itemType,identifyScrollName,"EquipType")
	local identify_equipPart = g_ItemInfoMgr:GetItemInfo(itemType,identifyScrollName,"EquipPart")
	local identify_equipTrend = g_ItemInfoMgr:GetItemInfo(itemType,identifyScrollName,"EquipTrend") --倾向值，可以为空
	local identify_equipTrendValue = g_ItemInfoMgr:GetItemInfo(itemType,identifyScrollName,"EquipTrendValue")--倾向值，可以为空
	local identify_equipLevel = g_ItemInfoMgr:GetItemInfo(itemType,identifyScrollName,"EquipLevel")--等级上限

	if curLevel > identify_equipLevel then
		MsgClient(170001)
		return false
	end

	local armorBaseValue = ArmorBaseValue_Common(curLevel,"ArmorBaseValue")
	if not armorBaseValue then
		return false
	end
	--读取装备或者武器配置表
	local equipType = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"EquipType")
	local equipPart = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"EquipPart")
		
	local flag1 = false
	local flag2 = false
	local flag3 = false
	local flag4 = false

	if "" ~= identify_equipType and identify_equipType == equipType then
		flag1 = true
	end
	if "" ~= identify_equipPart and nil ~= equipPart and equipPart == identify_equipPart then
		flag2 = true
	end
	if "" == identify_equipType  then
		flag3 = true
	end
	if "" == identify_equipPart then
		flag4 = true
	end
	if (flag1 and flag2) or (flag1 and flag4) or (flag2 and flag3) or (flag3 and flag4) then
		Gac2Gas:ArmorOrJewelryIdentify(g_Conn,armorBaseValue,nBigID,nIndex,identify_equipTrend or "",identify_equipTrendValue,scrollRoomIndex,scrollPos,equipRoomIndex or 0,equipPos or 0,itemid or 0)
	else
		MsgClient(170002)
		return false
	end
end


--武器鉴定
function GacEquipIdentify.WeaponIdentify(nBigID,nIndex,itemid,context,equipRoomIndex,equipPos,equipPart)
	local itemType = context[2]
	local identifyScrollName = context[3]
	local scrollRoomIndex = context[4]
	local scrollPos = context[5]
	local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(itemid)
	local curLevel = DynInfo.BaseLevel
	
	--读取鉴定卷轴配置表
	local identify_equipType = g_ItemInfoMgr:GetItemInfo(itemType,identifyScrollName,"EquipType")
	local identify_equipTrend = g_ItemInfoMgr:GetItemInfo(itemType,identifyScrollName,"EquipTrend") --倾向值，可以为空
	local identify_equipTrendValue = g_ItemInfoMgr:GetItemInfo(itemType,identifyScrollName,"EquipTrendValue")--倾向值，可以为空
	local identify_equipLevel = g_ItemInfoMgr:GetItemInfo(itemType,identifyScrollName,"EquipLevel")--等级上限

	if curLevel > identify_equipLevel then
		MsgClient(170003)
		return false
	end
	
	--读取装备或者武器配置表
	local equipType = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"EquipType")

	local flag1 = false
	local flag2 = false
	local flag3 = false
	
	if "" ~= identify_equipType and not(string.find(identify_equipType,equipType)) then
		MsgClient(170004)
	end
	
	if "" ~= identify_equipType and (string.find(identify_equipType,equipType)) then
		flag1 = true
	end
	
	if "" == identify_equipType then
		flag2 = true
	end

	if identify_equipTrend == "攻击速度"  or identify_equipTrend == "攻击浮动" then
		flag3 = true
	end
	if (flag1 and flag3) or (flag2 and flag3) then
		Gac2Gas:WeaponIdentify(g_Conn,curLevel,nBigID,nIndex,identify_equipTrend or "",identify_equipTrendValue or 0,scrollRoomIndex,scrollPos,equipRoomIndex or 0,equipPos or 0,itemid or 0,equipPart)
	else
		MsgClient(170005)
		return false
	end
end

---------------------------------装备鉴定服务端返回信息----------------------------------
function GacEquipIdentify.ReturnIdentifyResult(Conn,result)
	if result then
		MsgClient(170006)
		if g_GameMain.m_RoleStatus and g_GameMain.m_RoleStatus:IsShow() then
			g_GameMain.m_RoleStatus:UpdataEquipTips()
		end
		if g_GameMain.m_WndMainBag.m_ctItemRoom and g_GameMain.m_WndMainBag.m_ctItemRoom:IsShow() then
			g_GameMain.m_WndMainBag.m_ctItemRoom:UpdateBagTips()
		end
	else
		MsgClient(170007)
	end
end





