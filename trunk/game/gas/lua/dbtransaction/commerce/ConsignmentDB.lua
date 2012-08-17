gac_gas_require "framework/common/CMoney"
cfg_load "item/Equip_Armor_Common"
cfg_load "item/Equip_Weapon_Common"
cfg_load "item/Equip_Jewelry_Common"
cfg_load "item/Equip_Shield_Common"
cfg_load "item/Stone_Common"
cfg_load "item/SkillItem_Common"
cfg_load "item/BasicItem_Common"
gac_gas_require "commerce/consignment/Consignment"
cfg_load "item/Equip_Ring_Common"
cfg_load "equip/suit/EquipSuit_Common"
lan_load "equip/suit/Lan_EquipSuit_Common"

local GetAdvanceSoulCount = CConsignment.GetAdvanceSoulCount()
local GetShowIntensifyRoot = GetShowIntensifyRoot
local SoulMapInfoTbl = g_AdvancedSoul2DisplayRootTbl
local IndexMapShowSoul = g_Index2DisplayRootTbl
local Lan_EquipSuit_Common= Lan_EquipSuit_Common
local EquipSuit_Common = EquipSuit_Common
local g_ItemInfoMgr = g_ItemInfoMgr
local Equip_Armor_Common = Equip_Armor_Common
local Equip_Weapon_Common = Equip_Weapon_Common
local Equip_Jewelry_Common = Equip_Jewelry_Common
local Equip_Shield_Common = Equip_Shield_Common
local Stone_Common = Stone_Common
local SkillItem_Common = SkillItem_Common
local BasicItem_Common = BasicItem_Common
local g_MoneyMgr = CMoney:new()
local g_ItemInfoMgr			=	CItemInfoMgr:new()
local LogErr = LogErr
local AttrNameMapIndexTbl = CConsignment.AttrNameMapIndexTbl
local GetEquipQuality = CConsignment.GetEquipQuality
local loadstring   = loadstring
local Equip_Ring_Common= Equip_Ring_Common
local GetEquipIntenSoul = CConsignment.GetEquipIntenSoul
local GetEquipAdvanceSoul  = CConsignment.GetEquipAdvanceSoul
local MemH64 = MemH64
local event_type_tbl = event_type_tbl
local ConsignmentDB			=	CreateDbBox(...)
local CMarket				=	{}

local Panel = {}
Panel.SellOrder = 1
Panel.BuyOrder = 2
Panel.CharSellOrder = 3
Panel.CharBuyOrder = 4

local RememberPriceState = true

------------------------------------添加出售订单------------------------------------
local StmtDef = 
{
	"GetCSMSellPriceByTypeName",
	[[
		select msp_uPrice from tbl_market_sell_price where cs_uId =? and msp_sItemType =? and msp_sItemName =?
	]]
}
DefineSql(StmtDef, CMarket)
--记录出售物品价格
local StmtDef = 
{
	"SetRememberSellPrice",
	[[
		insert into tbl_market_sell_price (cs_uId, msp_sItemType, msp_sItemName, msp_uPrice ) values(?,?,?,?)  	
	]]
}
DefineSql(StmtDef, CMarket)
local StmtDef =
{
	"UpdateRememberSellPrice",
	[[
		update tbl_market_sell_price set msp_uPrice = ? where cs_uId =? and msp_sItemType=? and msp_sItemName=?
	]]
}
DefineSql(StmtDef, CMarket)

--添加收购订单
local StmtDef = 
{
	"_AddOrderStatic",
	"insert into tbl_market_order_static(mos_tCreateTime) values ( now() )"
}
DefineSql(StmtDef, CMarket)

--添加出售订单
local StmtDef = 
{
	"_AddSellOrder",
	[[
	insert into tbl_market_sell_order (mos_uId, cs_uId, mso_sItemType, mso_sItemName, mso_uCount, mso_uPrice, mso_tEndTime, mso_uCanUseLevel,mso_sSuitName, mso_uQuality, mso_sItemDisplayName) values (?, ?, ?, ?, ?, ?, DATE_ADD(now(), INTERVAL ? hour),?,?,?,?)
	]]
}
DefineSql(StmtDef, CMarket)

local StmtDef ={
    "_AddSellOrderEquipInfo",
    [[
    insert into tbl_market_equip_info (mos_uId, mei_uIntenQuality, mei_uAdvanceQuality, mei_sIntenSoul, mei_sAdvanceSoul) values(?,?,?,?,?)
    ]]
}DefineSql(StmtDef, CMarket)

local StmtDef = 
{	
	"_AddItemToOrder",
	[[
	insert into tbl_item_market (is_uId, mos_uId) values (?, ?)
	]]
}
DefineSql(StmtDef, CMarket)

--@brief 根据装备类别返回套装名称
local function GetSuitNameByEquipType(equipType,is_UId)
	local suitName = ""
	local query_sql = "select iei_sSuitName from tbl_item_equip_intensify where is_uId = " .. is_UId
	local _, resObj = g_DbChannelMgr:TextExecute(query_sql)
	if resObj and resObj:GetRowNum () > 0 then
		suitName = resObj:GetData(0,0)
	end
	return suitName
end


local StmtDef = {
    "_GetSellOrderInfo",
    [[
        select mos_uId, mso_sItemType, mso_sItemName from tbl_market_sell_order
    ]]
}
DefineSql(StmtDef, CMarket)

local StmtDef = {
   "_UpdateSellOrderQuality",
   [[
        update tbl_market_sell_order set mso_uQuality =? where mos_uId = ?
   ]]
}
DefineSql(StmtDef, CMarket)

local StmtDef = {
	"_GetPlayerCurLevel",
	[[
		select cb_uLevel from tbl_char_basic where cs_uId = ?
	]]
}
DefineSql(StmtDef,CMarket)

local function GetEquipDisplaySuitName(suitName)
    local index = 0
    local keys = EquipSuit_Common:GetKeys(suitName)
    local needPartsCount = keys[1]
    if needPartsCount == nil then
       return "" 
    end
    local lanIndex = string.format("%s%s", suitName, needPartsCount)
    local displaySuitName = Lan_EquipSuit_Common(MemH64(lanIndex), "DisplayName")
    return displaySuitName
end


local StmtDef = {
    "_GetAvgPriceByItemName",
    [[
        select avg(mso_uPrice/ mso_uCount) 
        from tbl_market_sell_order where mso_sItemName = ? and (unix_timestamp(mso_tEndTime) - unix_timestamp(now())) > 0
    ]]
}
DefineSql(StmtDef, CMarket)
--添加出售订单
function ConsignmentDB.CSMAddSellOrder(data)
	local uCharId				=	data["CharID"]
	local nRoomIndex			=	data["RoomIndex"]
	local nPos					=	data["Pos"]
	local uCount				=	data["ItemCount"]
	local bRememberPriceState	=	data["RememberPriceState"]
	local uPrice				=	data["Price"]
	local uTime					=	data["Time"]
	local suit_name             =   ""
	local needFee               =   data["NeedFee"]
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")	
	local sItemType, sItemName, count = g_RoomMgr.GetTypeCountByPosition(uCharId, nRoomIndex, nPos)
	if count == 0 or count == nil then
		--, "非法操作！"
		return false, 8012
	end
	if g_RoomMgr.CountItemLimitByName(sItemType,sItemName) > 0 then
		return false,21
	end
	local nBigID = tonumber(sItemType)
	
	local tbl_item_id = g_RoomMgr.GetAllItemFromPosition(uCharId, nRoomIndex, nPos)

	if count < uCount then
		--, "外挂！"
		return 	false, 8013
	end
	
	if g_ItemInfoMgr:IsQuestItem(sItemType) then		--不能出售任务物品
		--, "不能拍卖任务物品！"
		return false, 8014
	end
	if g_ItemInfoMgr:IsSoulPearl(sItemType) then
		--叠加物品的魂值不一样，不能出售
		if g_RoomMgr.CountSoulNumByPos(uCharId,nRoomIndex, nPos) > 1 then
			return false,8035
		end
	end
	local itemBindingType = g_RoomMgr.GetItemBindingTypeByID(tbl_item_id(1,1))	
	if itemBindingType == 2  or 3 == itemBindingType then --物品绑定类型为：绑定
		--, "不能出售绑定物品！"
		return false, 8015
	end
	--[[
	local avgPriceRet = CMarket._GetAvgPriceByItemName:ExecStat(sItemName)
	local avgPrice = 0
	if  avgPriceRet:GetRowNum() > 0 then
	    avgPrice = math.ceil(avgPriceRet:GetData(0,0) or 0)
	end
	if avgPrice > 0 then
	    local addPrice = math.ceil(uPrice /count)
	    if addPrice > avgPrice * 2 then
            return  false, 8036, sItemType, sItemName, avgPrice
	    end
	end
	--]]
	
	if bRememberPriceState == RememberPriceState then
		local sell_price_ret = CMarket.GetCSMSellPriceByTypeName:ExecSql("n", uCharId, sItemType, sItemName)
		if sell_price_ret:GetRowNum() == 0 then
			CMarket.SetRememberSellPrice:ExecSql("", uCharId, sItemType, sItemName, uPrice)
		else
			if sell_price_ret:GetData(0,0) ~= uPrice then
				CMarket.UpdateRememberSellPrice:ExecSql("", uPrice, uCharId, sItemType, sItemName)
			end
		end
		sell_price_ret:Release()
		if not (g_DbChannelMgr:LastAffectedRowNum()>0) then
			CancelTran()
			--, "更新记住价格出错！"
			return false, 8016
		end
	end
	local itemInfo =  g_ItemInfoMgr:GetItemInfo( nBigID, sItemName)
    local uLevel = itemInfo("BaseLevel")
	CMarket._AddOrderStatic:ExecSql("")
	local order_id = g_DbChannelMgr:LastInsertId()
	if uLevel == nil then
		uLevel = 0
	end
	local quality =  0
	local intenQuality, intenSoul, advanceQuality, advanceSoul
	if g_ItemInfoMgr:IsStaticEquip(nBigID) then--如果是装备的话
	    local itemDynInfoDB = RequireDbBox("GetItemDynInfoDB")
        local DynInfo = itemDynInfoDB.GetItemDynInfo(tbl_item_id(1,1), nBigID, uCharId)
        if DynInfo.IntensifyInfo:GetRowNum() > 0 then
            local intensifyPhase = DynInfo.IntensifyInfo(1,6)
            suit_name = DynInfo.IntensifyInfo(1,7) or ""
            local equipDynInfo = {}
            equipDynInfo.SuitName = suit_name 
            equipDynInfo.BaseLevel = DynInfo.BaseInfo(1,3)
            equipDynInfo.AddAttr1 = DynInfo.IntensifyInfo(1, 2)
            equipDynInfo.AddAttr2 = DynInfo.IntensifyInfo(1, 4)
            equipDynInfo.m_nBigID = nBigID
            quality = GetEquipQuality(intensifyPhase, equipDynInfo, itemInfo)
        else
            quality = itemInfo("Quality" ) or 0
        end
        local EquipMgrDB				=	RequireDbBox("EquipMgrDB")
	    uLevel = EquipMgrDB.GetEquipBaseLevel(nBigID, tbl_item_id(1,1))
	    local EquipIntensifyDB				=	RequireDbBox("EquipIntensifyDB")
	    local tbl, sqlKey = EquipIntensifyDB.GetSqlKeyInfoByEquipType(nBigID)
	    intenQuality = DynInfo.BaseInfo:GetDataByName(0, sqlKey .. "uIntensifyQuality")
	    intenSoul =   DynInfo.BaseInfo:GetDataByName(0, sqlKey .. "sIntenSoul")
	    advanceQuality    =DynInfo.AdvanceInfo(1, 2)
	    advanceSoul = DynInfo.AdvanceInfo(1, 15)
	    intenSoul 	= GetShowIntensifyRoot(intenSoul,nBigID)
	    advanceSoul = SoulMapInfoTbl[advanceSoul]
    else
        quality = itemInfo("Quality" ) or 0
	end
	local displaySuitName = ""
	if suit_name ~= ""  then
	    displaySuitName = GetEquipDisplaySuitName(suit_name)
	end
	
	local itemDisplayName = g_ItemInfoMgr:GetItemLanInfo(sItemType, sItemName,"DisplayName")
	
	CMarket._AddSellOrder:ExecSql("", order_id, uCharId, sItemType, sItemName, uCount, uPrice, uTime, uLevel,displaySuitName, quality, itemDisplayName)
    if not (g_DbChannelMgr:LastAffectedRowNum()>0) then
		CancelTran()
		--, "添加订单失败！"
		return false, 8017
	end

    
    if g_ItemInfoMgr:IsStaticEquip(nBigID) then--如果是装备的话
        CMarket._AddSellOrderEquipInfo:ExecSql("", order_id, intenQuality, advanceQuality, intenSoul, advanceSoul)
    end
    
	if not (g_DbChannelMgr:LastAffectedRowNum()>0) then
		CancelTran()
		--, "添加订单失败！"
		return false, 8017
	end

	local tbl_ids={}
	local ItemData = {}
	for i = 1, uCount do
		local ItemID = tbl_item_id(i,1)
		--向另一张表里面添加
		CMarket._AddItemToOrder:ExecSql("", ItemID, order_id)
		if not (g_DbChannelMgr:LastAffectedRowNum() == 1) then
			CancelTran()
			--, "向订单中添加物品失败！"
			return false, 8018
		end
		if not g_RoomMgr.MoveItemFromPackage(uCharId, ItemID) then
			CancelTran()
			--, "删除物品失败！"
			return false, 8019
		end
		
		table.insert( ItemData, ItemID )
	end 
		
	local level_res = CMarket._GetPlayerCurLevel:ExecStat(uCharId)
	local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.LogConsignmentAddSellOrder(uCharId,ItemData,order_id,uCount,uPrice,level_res:GetData(0,0))
	
	--交管理费
	local fun_info = g_MoneyMgr:GetFuncInfo("CSM")
	local MoneyManagerDB=	RequireDbBox("MoneyMgrDB")
	local bFlag,uMsgID, money = MoneyManagerDB.SubtractMoneyBySysFee(needFee, uCharId, fun_info, fun_info["AddSellOrder"],event_type_tbl["交易所添加订单管理费"])
	if not bFlag then
		CancelTran()
		--, "收手续费出错！" 
		if IsNumber(uMsgID) then
			return false,uMsgID
		else
			return false, 8020
		end
	end
	local bind_money_count, money_count = uMsgID, money
	return true, ItemData,sItemType, bind_money_count, money_count
end

--------------------------------------------取消出售订单----------------------------------
--获取某条卖东西记录的数据
local StmtDef = 
{
	"_GetSellOrder",
	[[
	select mso.mos_uId, mso.cs_uId, mso.mso_sItemType, mso.mso_sItemName, mso.mso_uCount, mso.mso_uPrice, unix_timestamp(mso.mso_tEndTime)
	from tbl_market_sell_order mso
	where mso.mos_uId = ?
	]]
}
DefineSql(StmtDef, CMarket)
function ConsignmentDB.GetSellOrderInfo(uOrderId)
	assert(IsNumber(uOrderId))
	local ret = CMarket._GetSellOrder:ExecSql("nns[12]s[32]nnn", uOrderId)
	local row  = ret:GetRowNum()
	if row == 0 then
		ret:Release()
		return nil
	end

	local sellOrder = {}
	for i = 1, ret:GetColNum() do
		sellOrder[i] = ret:GetData(0, i-1)
	end
	ret:Release()
	return sellOrder
end


--取消出售订单
local StmtDef = 
{
	"_CancelSellOrder",
	[[
	delete from tbl_market_order_static
	where mos_uId = ?
	]]
}
DefineSql(StmtDef, CMarket)

--提取附件
local StmtDef = 
{
	"_SelectBuyOrderItem",
	[[
	select is_uId 
	from tbl_item_market
	where mos_uId = ?
	]]
}
DefineSql(StmtDef, CMarket)
local StmtDef = 
{
	"_DeleteOrderItem",
	[[
	delete
	from tbl_item_market
	where mos_uId = ?
	]]
}
DefineSql(StmtDef, CMarket)
function ConsignmentDB.CSMCancelSellOrder(data)
	local uCharId		=	data["CharID"]
	local uOrderId		=	data["OrderID"]
	assert(IsNumber(uCharId))
	assert(IsNumber(uOrderId))

	local info = ConsignmentDB.GetSellOrderInfo(uOrderId)
	if info == nil then
		--, "订单错误！"
		return false, 1
	end

	if info[2] ~= uCharId then
		--, "玩家信息错误"
		return false, 8021
	end
	local uBigId, sItemName = tonumber(info[3]), info[4]
	local ret = CMarket._SelectBuyOrderItem:ExecSql("n", uOrderId)
	if ret:GetRowNum() == 0  then
		ret:Release()
		return false
	end
	local item_bag_box = RequireDbBox("CPutItemToBagDB")
	local row = ret:GetRowNum()
	local bFlag, ret_info
	local itemTbl = {}
	if 0 ~= row then
		local tbl_item_id = {}

		for i = 1, row do
			table.insert(tbl_item_id, {ret:GetDataByName(i-1, "is_uId")})
			table.insert(itemTbl,ret:GetDataByName(i-1, "is_uId"))
		end
		local params = {}
		params.m_nCharID = uCharId
		params.m_nItemType = uBigId
		params.m_sItemName = sItemName
		params.m_tblItemID = tbl_item_id
		bFlag, ret_info = item_bag_box.AddCountItem(params)
		if not bFlag then
			ret:Release()
			CancelTran()
			return false,ret_info
		end

		CMarket._CancelSellOrder:ExecSql("", uOrderId)
		if not (g_DbChannelMgr:LastAffectedRowNum()>0) then
			ret:Release()
			CancelTran()
			--取消丁单失败
			return false, 8010
		end
	end
	ret:Release()
	local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.LogConsignmentCancelSellOrder( uCharId,itemTbl,uOrderId,19)
	return true, ret_info
end

--------------------------------------------添加收购订单----------------------------------
local StmtDef = 
{
	"GetCSMBuyPriceByTypeName",
	[[
		select mbp_uPrice from tbl_market_buy_price where cs_uId =? and mbp_sItemType =? and mbp_sItemName =?		
	]]
}
DefineSql(StmtDef, CMarket)
--记录收购物品价格
local StmtDef = 
{
	"SetRememberBuyPrice",
	[[
		insert into tbl_market_buy_price (cs_uId, mbp_sItemType, mbp_sItemName, mbp_uPrice ) values(?,?,?,?)  	
	]]
}
DefineSql(StmtDef, CMarket)

local StmtDef =
{
	"UpdateRememberBuyPrice",
	[[
		update tbl_market_buy_price set mbp_uPrice = ? where cs_uId =? and mbp_sItemType=? and mbp_sItemName=?
	]]
}
DefineSql(StmtDef, CMarket)
local StmtDef = 
{
	"_AddBuyOrder",
	[[
	insert into tbl_market_buy_order (mos_uId, cs_uId, mbo_sItemType, mbo_sItemName, mbo_uCount, mbo_uPrice, mbo_tEndTime, mbo_uCanUseLevel, mbo_isUId, mbo_sSuitName, mbo_uSoulRoot) values (?, ?, ?, ?, ?, ?, DATE_ADD(now(), INTERVAL ? hour), ?, ?, ?, ?)
	]]
}
DefineSql(StmtDef, CMarket)


function ConsignmentDB.CSMAddBuyOrder(data)
	local uCharId				=	data["CharID"]
	local nRoomIndex			=	data["RoomIndex"]
	local nPos					=	data["Pos"]
	local uCount				=	data["ItemCount"]
	local uPrice				=	data["ItemPrice"]
	local uTime					=	data["Time"]
	local suit_name
	local bRememberPriceState	=	data["RememberPrice"]
	local needFee               =   data["NeedFee"]
    
    local ItemBagLockDB = RequireDbBox("ItemBagLockDB")
    --检测帐户是否锁定
    if ItemBagLockDB.HaveItemBagLock(uCharId) then
        return false,160046
    end
    
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local sItemType, sItemName, is_UId = g_RoomMgr.GetOneItemByPosition(uCharId,nRoomIndex,nPos)
	if sItemType == nil or sItemName == nil or is_UId == nil then
		--, "非法操作！"
		return false, 8012
	end
	if g_ItemInfoMgr:IsQuestItem(sItemType) then		--不能出售任务物品
		--, "不能拍卖任务物品！"
		return false, 8014
	end
	
	local nBigID	= tonumber(sItemType)
	local itemBindingType = g_RoomMgr.GetItemBindingTypeByID(is_UId)	
	if itemBindingType == 2 or 3 == itemBindingType then --物品绑定类型为：绑定
		--, "不能出售绑定物品！"
		return false, 8015
	end
	
	if bRememberPriceState == RememberPriceState then
		local buy_price_ret = CMarket.GetCSMBuyPriceByTypeName:ExecSql("n", uCharId, sItemType, sItemName)
		if buy_price_ret:GetRowNum() == 0 then
			CMarket.SetRememberBuyPrice:ExecSql("", uCharId, sItemType, sItemName, uPrice)
		else
			if buy_price_ret:GetData(0,0) ~= uPrice then
				CMarket.UpdateRememberBuyPrice:ExecSql("", uPrice, uCharId, sItemType, sItemName)
			end
		end
		if g_DbChannelMgr:LastAffectedRowNum() < 1 then
			CancelTran()
			--, "更新记住价格出错！"
			return false, 8016
		end
		buy_price_ret:Release()
	end
	CMarket._AddOrderStatic:ExecSql("")
	local order_id = g_DbChannelMgr:LastInsertId()

	local uLevel = tonumber(g_ItemInfoMgr:GetItemInfo( nBigID, sItemName,"BaseLevel" ))
	if uLevel == nil then
		uLevel = 0
	end
	local quality = 0 
	if nBigID >= 5 and nBigID <= 9 then
	    quality = GetEquipQuality(DynInfo, nBigID, sItemName)
    else
        quality = g_ItemInfoMgr:GetItemInfo( nBigID, sItemName,"Quality" ) or 0
	end

	CMarket._AddBuyOrder:ExecSql("", order_id, uCharId, sItemType, sItemName, uCount, uPrice, uTime, uLevel, is_UId, suit_name, quality)

	if g_DbChannelMgr:LastAffectedRowNum() < 1 then
		CancelTran()
		--, "添加买单失败！"
		return false, 8017
	end
	--缴费
	local MoneyManagerDB =	RequireDbBox("MoneyMgrDB")
	local account = uCount * uPrice
	local fun_info = g_MoneyMgr:GetFuncInfo("CSM")
	local bFlag,uMsgID = MoneyManagerDB.AddMoney(fun_info["FunName"],fun_info["AddBuyOrder"],uCharId, -(needFee+account),nil,event_type_tbl["购买交易所订单"])
	if not bFlag then
		CancelTran()
		--, "收手续费出错！" 
		if IsNumber(uMsgID) then
			return false,uMsgID
		else
			return false, 8023
		end
	end
	
	local MoneyType = 1 
--	local g_LogMgr = RequireDbBox("LogMgrDB")
--	g_LogMgr.LogConsignmentBuyCharOrder( uCharId,MoneyType, account)
	
	local result = -(needFee+account)
	return true, result
end

---------------------------------------------取消收购订单---------------------------------
--获取某条收购订单
local StmtDef = 
{
	"_GetBuyOrder",
	[[
	select mbo.mos_uId, mbo.cs_uId, mbo.mbo_sItemType, mbo.mbo_sItemName, mbo.mbo_uCount, mbo.mbo_uPrice, unix_timestamp(mbo.mbo_tEndTime)
	from tbl_market_buy_order mbo
	where mbo.mos_uId = ?
	]]
}
DefineSql(StmtDef, CMarket)
function ConsignmentDB.GetBuyOrderInfo(uOrderId)
	assert(IsNumber(uOrderId))
	local ret = CMarket._GetBuyOrder:ExecSql("nns[12]s[32]nnn", uOrderId)
	if ret:GetRowNum() == 0 then
		ret:Release()
		return nil
	end
	local buyOrder  = {}
	for i =1, ret:GetColNum() do
		buyOrder[i] = ret:GetData( 0 ,i-1 )
	end
	ret:Release()
	return buyOrder
end

--取消收购订单
local StmtDef = 
{
	"_CancelBuyOrder",
	[[
	delete from tbl_market_buy_order
	where mos_uId = ?
	]]
}
DefineSql(StmtDef, CMarket)
--取消收购订单
function ConsignmentDB.CSMCancelBuyOrder(data)
	local uCharId			=	data["CharID"]
	local uOrderId			=	data["OrderID"]
	assert(IsNumber(uCharId))
	assert(IsNumber(uOrderId))
	
	local info = ConsignmentDB.GetBuyOrderInfo(uOrderId)
	if info == nil then
		--, "订单错误！"
		return false, 8008
	end

	if info[2] ~= uCharId then
		--, "订单错误！"
		return false, 8008
	end
	local MoneyManagerDB=	RequireDbBox("MoneyMgrDB")
	local fun_info = g_MoneyMgr:GetFuncInfo("CSM")
	local bFlag,uMsgID = MoneyManagerDB.AddMoney(fun_info["FunName"],fun_info["CancelBuyOrder"],uCharId, info[5] * info[6],nil,event_type_tbl["手动删除出售订单"])
	if not bFlag then
		CancelTran()
		--, "收手续费出错！" 
		if IsNumber(uMsgID) then
			return false,uMsgID
		else
			return false, 8009
		end
	end
	
	CMarket._CancelBuyOrder:ExecSql("", uOrderId) 
	if not (g_DbChannelMgr:LastAffectedRowNum()>0) then
		CancelTran()
		--, "取消订单失败！"
		return false, 8010
	end
	local backFee = info[5] * info[6]
	
	return true, backFee
end

---------------------提取附件---------------------------------------
function ConsignmentDB.CSMTakeAttachment(data)
	local uCharId			=	data["CharID"]
	local uOrderId			=	data["OrderID"]
	assert(IsNumber(uCharId))
	assert(IsNumber(uOrderId))
	
	local info = ConsignmentDB.GetBuyOrderInfo(uOrderId)
	local uBigId, sItemName = tonumber(info[3]), info[4]
	local ret = CMarket._SelectBuyOrderItem:ExecSql("n", uOrderId)

	if ret:GetRowNum() == 0 then
		ret:Release()
		return false
	end
	local tbl_item_id = {}
	local num = ret:GetRowNum()
	for i = 1, num do
		table.insert(tbl_item_id, {ret:GetData(i-1,0)})
	end
	ret:Release()
	
	local params = {}
	params.m_nCharID = uCharId
	params.m_nItemType = uBigId
	params.m_sItemName = sItemName
	params.m_tblItemID = tbl_item_id
	local item_bag_box = RequireDbBox("CPutItemToBagDB")
	local bFlag, ret_info = item_bag_box.AddCountItem(params)
	if not bFlag then
		CancelTran()
		return false,ret_info
	end
	CMarket._DeleteOrderItem:ExecSql("", uOrderId)
	if not (g_DbChannelMgr:LastAffectedRowNum() > 0) then
		CancelTran()
		----删除丁单错误
		return false,8024
	end
	local info2 = ConsignmentDB.GetBuyOrderInfo(uOrderId)
	if info2[5]==0 then
		CMarket._CancelBuyOrder:ExecSql('', uOrderId)
		if not (g_DbChannelMgr:LastAffectedRowNum() > 0) then
			CancelTran()
			--删除丁单错误
			return false,8024
		end
	end
	
	return true, ret_info
end

-----------------------------------------查询订单-----------------------------------------
--一个数据映射包括多个类型的表
local MapCfgType ={}
MapCfgType[1] = {2,24,25,27,28,30,32,37,40}			--{}里的是物品type
MapCfgType[2] = {8, 9}			--StaticJewelry_Common\StaticRing_Common
MapCfgType[3] = {14, 15}   --Stone_Common\HoleMaterial_Common
MapCfgType[4] = {46}				--消耗品：SkillItem_Common、BasicItem_Common
MapCfgType[5] = {34, 38, 43, 50, 49}	--装备强化相关：AdvanceStone_Common、ArmorPieceEnactment_Common				
MapCfgType[6] = {10,17,18,19,23,26,29,31,36,39} --除了拍卖行列表中显示的物品类型以外的
MapCfgType[7] = {1} --basicItem
MapCfgType[8] = {6,7}--防具和盾牌
MapCfgType[9] = {27,28}--锻造、铸甲类物品

--属性所在列类型
local AttrIndexMapTbl = {}
AttrIndexMapTbl[1] = {27} -- 强化石、重铸石
AttrIndexMapTbl[2] = {29, 30, 31} --药剂、食品、魔法物品
AttrIndexMapTbl[3] = {32, 33, 34} --烹饪制药材料、普通物品、坐骑

local SearchType = {}
SearchType["ByOneItemType"] = 1     --查询某一类物品
SearchType["BySomeItemType"] = 2    --查询某几类物品
SearchType["ByItemTypeAndAttr"] = 3 --根据某类物品的某个属性列查询
SearchType["ByItemTypeOrAttr"] = 4  --根据某几类物品或者某类物品的某个属性列
 
--记录出售物品价格
local StmtDef = 
{
	"_GetCharCamp",
	[[
		select cs_uCamp from tbl_char_static where cs_uId = ?
	]]
}
DefineSql(StmtDef, CMarket)

local StmtDef = {
    "_GetAllRows",
    [[
        select FOUND_ROWS()
    ]]
}
DefineSql(StmtDef, CMarket)


--查询出售订单
function ConsignmentDB.SearchCommonSellOrder(uCharId, uItemType, sName, uBeginLevel, uEndLevel, OrderBy, Page, bSearchType,flag,advanceSoul,maxAdvancePhase,uAttrIndex, intenSoul, maxIntenPhase, quality, campFlag)
	assert(IsString(sName))
	local uBigId = tonumber(uItemType)
	local query_sql = "select SQL_CALC_FOUND_ROWS mso.mos_uId, c.c_sName, mso.mso_sItemType, mso.mso_sItemName, mso.mso_uPrice, mso.mso_uCount, ((unix_timestamp(mso.mso_tEndTime) - unix_timestamp(now()))/3600), c.cs_uId " 
	local query_string = " from tbl_char c, tbl_market_order_static mos"
    query_string = query_string .. ", tbl_market_sell_order mso left join tbl_market_cfg_info as cfg on mso.mso_sItemType = cfg.mci_uType and mso.mso_sItemName = cfg.mci_sName  " 
    
    if advanceSoul ~="" or  maxAdvancePhase ~= 0 or intenSoul ~= "" or maxIntenPhase ~= 0 then
        query_string = query_string .. "left join tbl_market_equip_info equip on mso.mos_uId = equip.mos_uId "    
    end
    
	if SearchType["ByOneItemType"]  == bSearchType then
	    query_string = query_string .. " where  mso.mso_sItemType = '" .. uBigId .."' and "
	
	elseif SearchType["BySomeItemType"] == bSearchType then
        local cfgTypeMapTbl = MapCfgType[uBigId]
		if #cfgTypeMapTbl > 0 then
			query_string = query_string .. " where mso.mso_sItemType in (" .. table.concat(cfgTypeMapTbl, ',') .. ") and "
		end
	
	elseif SearchType["ByItemTypeAndAttr"] == bSearchType then
	    query_string = query_string .. "  where cfg.mci_uType = " .. uBigId .. " and cfg.mci_uAttrIndex = " ..  uAttrIndex  .. " and "
	    
	elseif SearchType["ByItemTypeOrAttr"] == bSearchType then
        local cfgTypeMapTbl = MapCfgType[uBigId]
        query_string = query_string .. " where "
        local addMark = ""
	    if #cfgTypeMapTbl > 0 then
		    query_string = query_string .. " (mso.mso_sItemType  in (" .. table.concat(cfgTypeMapTbl, ',') .. ")  or "
		    addMark = ")"
		end
		query_string = query_string .. " cfg.mci_uAttrIndex in (" .. table.concat(AttrIndexMapTbl[uAttrIndex], ',') ..")" .. addMark ..  " and " 
    else
        query_string = query_string .. " where "
	end
	query_string = query_string .. " unix_timestamp(mso.mso_tEndTime) >0 and  mso.mos_uId = mos.mos_uId and mso.mso_uCount > 0 and mso.cs_uId = c.cs_uId and (unix_timestamp(mso.mso_tEndTime) - unix_timestamp(now())) > 0"

    if campFlag == true then
        local campRet = CMarket._GetCharCamp:ExecStat(uCharId)
        local camp = campRet:GetData(0, 0)
        query_string  = query_string .. " and ( cfg.mci_uCamp = " .. camp .." or cfg.mci_uCamp = 0 or cfg.mci_uCamp is null ) "  
    end
	--等级下限
	if (uBeginLevel ~= nil and uBeginLevel ~= 0) then
		query_string = query_string .." and mso.mso_uCanUseLevel >= " .. uBeginLevel
	end

	--等级上限
	if (uEndLevel ~= nil and uEndLevel ~= 0) then
		query_string = query_string .." and mso.mso_uCanUseLevel <= " .. uEndLevel
	end
	
	if quality > -1 then
	    query_string= query_string .. " and mso.mso_uQuality = " .. quality
	end
	
	if (sName ~= nil) then
		if flag == 1 then --按照物品名称搜
			query_string = query_sql .. query_string
			query_string = query_string .. " and mso_sItemDisplayName  like '" .. "\%"..sName .."\%".. "' "
		elseif flag == 2 then --按照套装名称搜
			query_sql = query_sql .. ",mso.mso_sSuitName"
			query_string = query_sql .. query_string
			query_string = query_string .. " and mso.mso_sSuitName  like '" .. "\%"..sName .."\%".. "' "
		end
	else
		--query_string = query_sql .. query_string
	end
    
 
    if advanceSoul ~=0 or  maxAdvancePhase ~= -1 or intenSoul ~= 0 or maxIntenPhase ~= -1 then
	    	--进阶魂根的匹配
    	if advanceSoul > 0 then
    	    query_string= query_string .. " and equip.mei_sAdvanceSoul = '" .. IndexMapShowSoul[advanceSoul] .. "'"
    	end
    	
    	if maxAdvancePhase > -1 then
    	    query_string = query_string .. " and equip.mei_uAdvanceQuality = " .. maxAdvancePhase
    	end
    	
    	if intenSoul > 0 then
    	    local advanceSoulCount = GetAdvanceSoulCount
    	    query_string= query_string .. " and equip.mei_sIntenSoul ='" .. IndexMapShowSoul[intenSoul + advanceSoulCount] .. "'"
    	end
    	
    	if maxIntenPhase > -1 then
    	    query_string= query_string .. " and equip.mei_uIntenQuality = " .. maxIntenPhase
    	end
    end

--[[
	local temp,query_result
	if flag == 2 then
		temp, query_result = g_DbChannelMgr:TextExecute(query_string)
	else
		temp, query_result = g_DbChannelMgr:TextExecute(query_string)
	end

	local query_result_tbl = {}
	local query_result_count = 0
	if query_result == nil then
		return query_count_result, query_result_tbl
	end
		
	query_result_count = query_result:GetRowNum()
	if  query_result_count == 0 then
		return query_result_count, query_result
	end
--]]

	if (OrderBy == 1) then
		query_string = query_string .. " order by mso.mso_sItemName"
	elseif (OrderBy == 2) then
		query_string = query_string .. " order by mso.mso_uPrice"
	elseif (OrderBy == 3) then
		query_string = query_string .. " order by mso.mso_uCanUseLevel"
	elseif (OrderBy == 4) then
		query_string = query_string .. " Order by mso.mso_tEndTime "
	elseif (OrderBy == 5) then
		query_string = query_string .. " order by c.c_sName"
	end
	
	if Page ~= nil and  Page ~= 0 then
	    local page = Page or 1
	    local begin_record = page * 50 - 50
	    query_string = query_string .. " limit " .. begin_record .. " , 50"
	end
	
	local temp1, query_result1
	if flag == 1 then --按照物品名称搜
		 temp1, query_result1 = g_DbChannelMgr:TextExecute(query_string)
	elseif flag == 2 then --按照套装名称搜
		 temp1, query_result1 = g_DbChannelMgr:TextExecute(query_string)
	end
	
	if query_result1 == nil then
		return 0, {}
	end
	if query_result1:GetRowNum() == 0 then
		return 0, query_result1
	end

	local query_result_countRet = CMarket._GetAllRows:ExecStat()
	local query_result_count =query_result_countRet:GetData(0, 0)
	
	local itemDynInfoDB = RequireDbBox("GetItemDynInfoDB")	 
	local itemsInfoTbl = {}
    local limitQueryCount =query_result1:GetRowNum()
    if limitQueryCount > 50 then
        limitQueryCount = 50
    end
    
    local tbl_id = {}
    local orderItemID = {}
	for i = 1, limitQueryCount do
		local itemid = 0
		local orderID = query_result1(i,1)
		local ret = CMarket._SelectBuyOrderItem:ExecSql("n", orderID)
		if ret:GetRowNum()> 0  then
			itemid = ret:GetData(0,0)
			table.insert(tbl_id,itemid)
            local itemInfoTbl = itemDynInfoDB.GetItemDynInfo(itemid, query_result1(i,3), query_result1(i,8),uCharId, ret)
	        table.insert(itemsInfoTbl, itemInfoTbl[1])
		else
		    query_result_count = query_result_count - 1
		    table.insert(itemsInfoTbl, {})
		end
		table.insert(orderItemID, itemid)
	end
	local newQueryResultTbl = query_result1
	return query_result_count, newQueryResultTbl, itemsInfoTbl, orderItemID
end

--查询收购订单
function ConsignmentDB.SearchCommonBuyOrder(sType, sName, uBegineLevel, uEndLevel, OrderBy, Page, bCfgTblMap,flag,IsZhanshen,IsFamo)
	local uBigId = tonumber(sType)
	assert(IsString(sName))
	local query_sql = "select mbo.mos_uId, c.c_sName, mbo.mbo_sItemType, mbo.mbo_sItemName, mbo.mbo_uPrice, mbo.mbo_uCount, ((unix_timestamp(mbo.mbo_tEndTime) - unix_timestamp(now()))/3600), mbo.mbo_isUId,c.cs_uId " 
	local query_string = "  from tbl_char c,tbl_market_buy_order mbo where unix_timestamp(mbo.mbo_tEndTime) > 0 and mbo.mbo_uCount  > 0 and mbo.cs_uId = c.cs_uId "
	if (uBigId ~= nil and uBigId ~= 0) then
		if bCfgTblMap  then
			local cfgTypeMapTbl = MapCfgType[uBigId]
			if #cfgTypeMapTbl > 0 then
				query_string = query_string .. " and mbo.mbo_sItemType in (" .. table.concat(cfgTypeMapTbl, ',') .. ") "
			end
		else
			query_string = query_string .. " and  mbo.mbo_sItemType = '" .. uBigId .."'"
		end
	end	
	
	--等级下限
	if (uBeginLevel ~= nil and uBeginLevel ~= 0) then
		query_string = query_string .." and mbo.mbo_uCanUseLevel >= " .. uBeginLevel
	end

	--等级上限
	if (uEndLevel ~= nil and uEndLevel ~= 0) then
		query_string = query_string .." and mbo.mbo_uCanUseLevel <= " .. uEndLevel
	end
	
	if (sName ~= nil) then
		if flag == 1 then --按照物品名称搜
			query_string = query_sql .. query_string
			query_string = query_string .. " and mbo.mbo_sItemName  like '" .. "\%"..sName .."\%".. "' "
		elseif flag == 2 then --按照套装名称搜
			query_sql = query_sql .. ",mbo.mbo_sSuitName"
			query_string = query_sql .. query_string
			query_string = query_string .. " and mbo.mbo_sSuitName  like '" .. "\%"..sName .."\%".. "' "
		end
	else
		query_string = query_sql .. query_string	
	end
	
	local isChoosedFaMoOrZhanShenTbl = {}
	local isSelectSoulRoot = false
	if not IsFamo then     --选中了筛选条件为法魔 
	    table.insert(isChoosedFaMoOrZhanShenTbl, 1)
	    isSelectSoulRoot = true
	end
	
	if not IsZhanshen then --选中筛选条件为战神
       table.insert(isChoosedFaMoOrZhanShenTbl, 2)
       isSelectSoulRoot = true
	end
	
	if isSelectSoulRoot then
	    query_string = query_string .. " and mbo.mbo_uSoulRoot in (" .. table.concat(isChoosedFaMoOrZhanShenTbl, ',') .. ") "
	end
	
	local temp,query_result
	if flag == 2 then
		temp, query_result = g_DbChannelMgr:TextExecute(query_string)
	else
		temp, query_result = g_DbChannelMgr:TextExecute(query_string)
	end
	
	local query_result_tbl = {}
	local query_count_result = 0
	if query_result == nil then
		return query_count_result, query_result_tbl
	end
    query_count_result = query_result:GetRowNum()

	if query_count_result == 0  then
		query_result:Release()
		return query_count_result, query_result_tbl
	end

	if (OrderBy == 1) then
		query_string = query_string .. " order by mbo.mbo_sItemName"
	elseif (OrderBy == 2) then
		query_string = query_string .. " order by mbo.mbo_uCount"
	elseif (OrderBy == 3) then
		query_string = query_string .. " order by c.c_sName"
	elseif (OrderBy == 4) then
		query_string = query_string .. " order by mbo.mbo_uPrice"
	elseif (OrderBy == 5) then
		query_string = query_string .. " Order by mbo.mbo_tEndTime "
	end

	local page = Page or 1
	local begin_record = page * 50 - 50
	query_string = query_string .. " limit " .. begin_record .. " , 50"
	
	local temp1, query_result1
	if flag == 1 then --按照物品名称搜
		 temp1, query_result1 = g_DbChannelMgr:TextExecute(query_string)
	elseif flag == 2 then --按照套装名称搜
		 temp1, query_result1 = g_DbChannelMgr:TextExecute(query_string)
	end

	if query_result1 == nil then
		return  0, {}
	end

	if query_result:GetRowNum() == 0  then
		query_result:Release()
		return 0, {}
	end
	query_result_tbl = query_result1:ToTable(true)
    local itemsInfoTbl = {}
	return query_count_result, query_result_tbl, itemsInfoTbl --最后一个返回值：itemsInfoTbl只是为了和出售订单的做法一致，不过不查询物品动态信息
end

function ConsignmentDB.CSMGetOrderByExactItemAttr(data)
	local uCharId			=	data["CharID"]
	local pannelNo			=	data["PannelNo"]
	local uType				=	data["Type"]
	local uAttrIndex		=	data["AttrIndex"]
	local sName				=	data["Name"]
	local uBeginLevel		=	data["BeginLevel"]
	local uEndLevel			=	data["EndLevel"]
	local OrderBy			=	data["OrderBy"]
	local Page				=	data["Page"]
	local flag              =   data["flag"]
	local advanceSoul       =   data["AdvanceSoul"]
	local maxAdvancePhase   =   data["MaxAdvancePhase"]
	local intenSoul         =   data["IntenSoul"]
	local maxIntenPhase     =   data["MaxIntenPhase"]
	local quality           =   data["Quality"]
	local campFlag          =   data["CampFlag"]
	
	local query_result_count, query_result_tbl, itemsInfoTbl, OrderItemIDTbl
    local needDynInfo = false
    
	if pannelNo == Panel.SellOrder then
		query_result_count, query_result_tbl, itemsInfoTbl, OrderItemIDTbl = ConsignmentDB.SearchCommonSellOrder(uCharId, uType, sName, uBeginLevel, uEndLevel, OrderBy, Page,SearchType["ByItemTypeAndAttr"],flag,advanceSoul,maxAdvancePhase, uAttrIndex, intenSoul, maxIntenPhase, quality, campFlag)	
        needDynInfo = true
	elseif pannelNo == Panel.BuyOrder then
		query_result_count, query_result_tbl, itemsInfoTbl = ConsignmentDB.SearchCommonBuyOrder(uType, sName, uBeginLevel, uEndLevel, OrderBy, Page,SearchType["ByItemTypeAndAttr"],flag,advanceSoul,maxAdvancePhase, uAttrIndex, intenSoul, maxIntenPhase)	
	end
	
	local result = {["Count"]=query_result_count, ["Ret"]=query_result_tbl, ["ItemInfoTbl"] =itemsInfoTbl, ["OrderItemIDTbl"] = OrderItemIDTbl }
	return true, result
end

---根据某一类查询匹配订单
function ConsignmentDB.CSMGetOrderBySomeItemType(data)
	local uCharId			=	data["CharID"]
	local pannelNo			=	data["PannelNo"]
	local uType				=	data["Type"]
	local sName				=	data["Name"]
	local uBeginLevel		=	data["BeginLevel"]
	local uEndLevel			=	data["EndLevel"]
	local OrderBy			=	data["OrderBy"]
	local Page				=	data["Page"]
	local flag              =   data["flag"] 
	local advanceSoul       =   data["AdvanceSoul"] 
	local maxAdvancePhase   =   data["MaxAdvancePhase"]
	local intenSoul         =   data["IntenSoul"]
	local maxIntenPhase     =   data["MaxIntenPhase"]
	local quality           =   data["Quality"]
	local campFlag          =   data["CampFlag"]
	local query_result_count, query_result_tbl, itemsInfoTbl  , OrderItemIDTbl
	if pannelNo == Panel.SellOrder then
		query_result_count, query_result_tbl, itemsInfoTbl, OrderItemIDTbl = ConsignmentDB.SearchCommonSellOrder(uCharId, uType, sName, uBeginLevel, uEndLevel, OrderBy, Page, SearchType["BySomeItemType"],flag,advanceSoul,maxAdvancePhase, nil,intenSoul, maxIntenPhase, quality, campFlag)	
	elseif pannelNo == Panel.BuyOrder then
		query_result_count, query_result_tbl, itemsInfoTbl = ConsignmentDB.SearchCommonBuyOrder(uType, sName, uBeginLevel, uEndLevel, OrderBy, Page, SearchType["BySomeItemType"],flag,advanceSoul,maxAdvancePhase,nil, intenSoul, maxIntenPhase)
	end
	local result ={["Count"]=query_result_count, ["Ret"]=query_result_tbl, ["ItemInfoTbl"]=itemsInfoTbl, ["OrderItemIDTbl"] = OrderItemIDTbl}
	return true, result
end

--查询某个类型符合某两个属性列的物品
function ConsignmentDB.CSMGetOrderByItemAttr(data)
	local uCharId			=	data["CharID"]
	local pannelNo			=	data["PannelNo"]
	local uType				=	data["Type"]
	local sName				=	data["Name"]
	local uBeginLevel		=	data["BeginLevel"]
	local uEndLevel			=	data["EndLevel"]
	local OrderBy			=	data["OrderBy"]
	local Page				=	data["Page"]
	local sParentAttrColName=	data["ParentAttrColName"] 
	local sParentNodeText	=	data["ParentNodeText"]
	local sAttrColName		=	data["AttrColName"]
	local sCurNodeText		=	data["CurNodeText"]
	local flag              =   data["flag"] 
	local advanceSoul       =   data["AdvanceSoul"] 
	local maxAdvancePhase   =   data["MaxAdvancePhase"]
	local intenSoul         =   data["IntenSoul"]
	local maxIntenPhase     =   data["MaxIntenPhase"]
	local quality           =   data["Quality"]
	local campFlag          =   data["CampFlag"]
	local needDynInfo = false
	local query_result_count, query_result_tbl, itemsInfoTbl , OrderItemIDTbl
	if pannelNo == Panel.SellOrder then
		query_result_count, query_result_tbl, itemsInfoTbl, OrderItemIDTbl = ConsignmentDB.SearchCommonSellOrder(uCharId, uType, sName, uBeginLevel, uEndLevel, OrderBy, Page,false,flag,advanceSoul,maxAdvancePhase, nil, intenSoul, maxIntenPhase, quality, campFlag)	
	    needDynInfo = true
	elseif pannelNo == Panel.BuyOrder then
		query_result_count, query_result_tbl, itemsInfoTbl = ConsignmentDB.SearchCommonBuyOrder(uType, sName, uBeginLevel, uEndLevel, OrderBy, Page,false,flag,advanceSoul,maxAdvancePhase, nil, intenSoul, maxIntenPhase)
	end
	
	local result_tbl= {}
	local itemDynInfoTbl = {}
	if needDynInfo then
	    for i=1, # query_result_tbl do
    		local sParentAttrColName = g_ItemInfoMgr:GetItemInfo(tonumber(uType), query_result_tbl[i][4],sParentAttrColName)
    		if sParentAttrColName == sParentNodeText and
    			itemInfo[sAttrColName] == sCurNodeText then
    			table.insert(result_tbl, query_result_tbl[i])
    			table.insert(itemDynInfoTbl, itemsInfoTbl[i])
    		end
	    end
	end
	local tblLen = # result_tbl
	local result ={["Count"]=query_result_count, ["Ret"]=result_tbl, ["ItemInfoTbl"]=itemDynInfoTbl, ["OrderItemIDTbl"] = OrderItemIDTbl}
	
	return true, result
end

--查询某几个类别的所有物品，并且根据某个表中的某个属性列中的某些符合要求的order
function ConsignmentDB.CSMGetOrderBySeveralSortItem(data)
	local uCharId			=	data["CharID"]
	local pannelNo			=	data["PannelNo"]
	local uTypeIndex		=	data["ItemTypeTblIndex"]
	local uAttrTblIndex     =   data["AttrTblIndex"]
	local sName				=	data["Name"]
	local uBeginLevel		=	data["BeginLevel"]
	local uEndLevel			=	data["EndLevel"]	
	local OrderBy			=	data["OrderBy"]
	local Page				=	data["Page"]
	local flag              =   data["flag"]
	local advanceSoul       =   data["AdvanceSoul"]
	local maxAdvancePhase   =   data["MaxAdvancePhase"]
	local intenSoul         =   data["IntenSoul"]
	local maxIntenPhase     =   data["MaxIntenPhase"]
	local quality           =   data["Quality"]
	local campFlag          =   data["CampFlag"]
    local needDynInfo = false
	local query_result_count, query_result_tbl, itemsInfoTbl , OrderItemIDTbl
	
	if pannelNo == Panel.SellOrder then
		query_result_count, query_result_tbl, itemsInfoTbl , OrderItemIDTbl= ConsignmentDB.SearchCommonSellOrder(uCharId, uTypeIndex, sName, uBeginLevel, uEndLevel, OrderBy, Page,SearchType["ByItemTypeOrAttr"],flag,advanceSoul,maxAdvancePhase,uAttrTblIndex, intenSoul, maxIntenPhase, quality, campFlag)	
	    needDynInfo = true
	elseif pannelNo == Panel.BuyOrder then
		query_result_count, query_result_tbl, itemsInfoTbl = ConsignmentDB.SearchCommonBuyOrder(uTypeIndex, sName, uBeginLevel, uEndLevel, OrderBy, Page,SearchType["ByItemTypeOrAttr"],flag,advanceSoul,maxAdvancePhase, uAttrTblIndex, intenSoul, maxIntenPhase)
	end

	local result ={["Count"]=query_result_count, ["Ret"]=query_result_tbl, ["ItemInfoTbl"]=itemsInfoTbl, ["OrderItemIDTbl"] = OrderItemIDTbl}
	
	return true, result
    
end

--根据物品type搜索某一类物品
function ConsignmentDB.CSMGetOrderByItemType(data)
	local uCharId			=	data["CharID"]
	local pannelNo			=	data["PannelNo"]
	local uType				=	data["Type"]
	local sName				=	data["Name"]
	local uBeginLevel		=	data["BeginLevel"]
	local uEndLevel			=	data["EndLevel"]
	local OrderBy			=	data["OrderBy"]
	local Page				=	data["Page"]
	local flag              =   data["flag"]
	local advanceSoul       =   data["AdvanceSoul"]
	local maxAdvancePhase   =   data["MaxAdvancePhase"]
	local intenSoul         =   data["IntenSoul"]
	local maxIntenPhase     =   data["MaxIntenPhase"]
	local quality           =   data["Quality"]
	local campFlag          =   data["CampFlag"]
	local query_result_count, query_result_tbl, itemsInfoTbl , OrderItemIDTbl
	if pannelNo == Panel.SellOrder then
		query_result_count, query_result_tbl, itemsInfoTbl, OrderItemIDTbl = ConsignmentDB.SearchCommonSellOrder(uCharId, uType, sName, uBeginLevel, uEndLevel, OrderBy, Page, SearchType["ByOneItemType"] ,flag,advanceSoul,maxAdvancePhase, nil, intenSoul, maxIntenPhase, quality, campFlag)	
	elseif pannelNo == Panel.BuyOrder then
		query_result_count, query_result_tbl, itemsInfoTbl = ConsignmentDB.SearchCommonBuyOrder(uType, sName, uBeginLevel, uEndLevel, OrderBy, Page, SearchType["ByOneItemType"] ,flag,advanceSoul,maxAdvancePhase, nil, intenSoul, maxIntenPhase)
	end
	
	local result = {["Count"]=query_result_count, ["Ret"]=query_result_tbl, ["ItemInfoTbl"] = itemsInfoTbl, ["OrderItemIDTbl"] = OrderItemIDTbl}
	return true, result
end

--得到之前某个物品出售或收购的价格
function ConsignmentDB.GetCSMRememberPrice(data)
	local uCharId		=	data["CharId"]
	local pannelNo		=	data["PannelNo"]
	local itemType		=	data["ItemType"]
	local itemName		=	data["ItemName"]

	local priceRet 	
	if pannelNo == Panel.CharSellOrder then 
		priceRet = CMarket.GetCSMSellPriceByTypeName:ExecSql("n", uCharId, itemType, itemName)
	elseif pannelNo == Panel.CharBuyOrder then
		priceRet = CMarket.GetCSMBuyPriceByTypeName:ExecSql("n", uCharId, itemType, itemName)
	end
	
	if priceRet:GetRowNum() == 0 then
		priceRet:Release()
		return 
	end
	local price = priceRet:GetData(0, 0)
	priceRet:Release()
	return true, price
end

--判断出售物品剩余时间是否<=0
--ruguo<=0，则把没有出售的物品和剩余money一起发送到收购者邮箱
--服务器开始运行后定期刷新此方
local StmtDef = 
{
	"_SelectAllSells",
	[[
	select mos_uId,cs_uId,mso_sItemType,mso_sItemName,mso_uCount,mso_uPrice
	from tbl_market_sell_order
	where mso_tEndTime + 60 <= now() 
	]]
}
DefineSql(StmtDef, CMarket)

local StmtDef = 
{
	"_SelectTimeOutSellsByCharID",
	[[
	select mos_uId,cs_uId,mso_sItemType,mso_sItemName,mso_uCount,mso_uPrice
	from tbl_market_sell_order
	where mso_tEndTime + 60 <= now() and cs_uId = ?
	]]
}
DefineSql(StmtDef, CMarket)

function  ConsignmentDB.SellsIsOrNotOuttime(playerID)
    local all_sells 
    if playerID == nil then
	    all_sells = CMarket._SelectAllSells:ExecSql('nns[12]s[32]nn')
	else
        all_sells = CMarket._SelectTimeOutSellsByCharID:ExecStat(playerID)
	end
	
	if all_sells:GetRowNum() == 0 then
		all_sells:Release()
		return false
	end
	local new_mail_playerID_tbl = {}
	local num = all_sells:GetRowNum() 
	local lSysMailExecutor = RequireDbBox("SysMailDB")
	for i=1,num do
	    local order_id =  all_sells:GetData(i-1,0)
		local order_item_ids = CMarket._SelectBuyOrderItem:ExecSql("n",order_id)
		if order_item_ids:GetRowNum() == 0  then
			order_item_ids:Release()
			LogErr("拍卖行过期订单回退失败", "订单没有关联的物品信息" .. ",OrderID" .. order_id)
			break
		end
		local char_id = all_sells:GetData(i-1,1)
		local order_item_ids_tbl = order_item_ids:ToTable(true)
		local item_type,item_name = all_sells:GetData(i-1,2),all_sells:GetData(i-1,3)
		local succ = lSysMailExecutor.SendSysMail(1001,char_id,1002,
		"1003_item:" .. item_type .. "|" .. item_name,
		order_item_ids_tbl,0,112, 5004)

		if not succ  then
			CancelTran()
			all_sells:Release()
			LogErr("拍卖行过期订单回退失败", "OrderID" .. order_id)
			return  false
		end
		CMarket._CancelSellOrder:ExecSql("", order_id)
		if not (g_DbChannelMgr:LastAffectedRowNum()>0) then
			CancelTran()
			all_sells:Release()
			LogErr("拍卖行过期订单回退失败", "OrderID" .. order_id)
			return false
		end

		table.insert(new_mail_playerID_tbl, char_id)
		local g_LogMgr = RequireDbBox("LogMgrDB")
		g_LogMgr.LogConsignmentCancelSellOrder( char_id,order_item_ids_tbl[1],order_id,20)		
	end
	all_sells:Release()
	return true, new_mail_playerID_tbl
end

--判断收购剩余时间是否<=0
--<=0，则把收购的物品和剩余money一起发送到收购者邮箱
--服务器开始运行后定期刷新此方
local StmtDef = 
{
	"_SelectAllOrder",
	[[
	select mos_uId,cs_uId,mbo_sItemType,mbo_sItemName,mbo_uCount,mbo_uPrice
	from tbl_market_buy_order
	where mbo_tEndTime+60 <= now()
	]]
}
DefineSql(StmtDef, CMarket)
function  ConsignmentDB.OrderIsOrNotOuttime()
	local all_orders = CMarket._SelectAllOrder:ExecSql('nns[12]s[32]nn')
	if all_orders:GetRowNum() == 0 then
		all_orders:Release()
		return	false
	end
	local num = all_orders:GetRowNum()
	local new_mail_playerID_tbl = {}
	local lSysMailExecutor = RequireDbBox("SysMailDB")
	for i=1, num do
	    local order_id = all_orders:GetData(i-1,0)
		local order_item_ids = CMarket._SelectBuyOrderItem:ExecSql("n", order_id)
		if order_item_ids:GetRowNum() ~= 0 then
			local order_item_ids_tbl = order_item_ids:ToTable(true)
			local char_id = all_orders:GetData(i-1,1)
			local item_type,item_name = all_orders:GetData(i-1,2),all_orders:GetData(i-1,3)
			local succ=lSysMailExecutor.SendSysMail(1001,char_id,1002,
			"1004_item:" .. item_type .. "|" .. item_name,
			order_item_ids_tbl,all_orders:GetData(i-1,4) *all_orders:GetData(i-1,5))
			if not succ then
				all_orders:Release()
				CancelTran()
				return  false
			end

			CMarket._CancelBuyOrder:ExecSql("", order_id) 
			if not (g_DbChannelMgr:LastAffectedRowNum()>0) then
				all_orders:Release()
				CancelTran()
				return false
			end

			CMarket._DeleteOrderItem:ExecSql("",order_id)
			if not (g_DbChannelMgr:LastAffectedRowNum() >0) then
				all_orders:Release()
				CancelTran()
				return false
			end 
			table.insert(new_mail_playerID_tbl, char_id)

			--如果收购者不在线，将小助手提示消息保存数据库。
			local LoginServerSql = RequireDbBox("LoginServerDB")
			local SystemMsgDB = RequireDbBox("SystemMsgDB")
			if not LoginServerSql.IsPlayerOnLine(char_id) then
				local parameter = {
											["uCharId"] = char_id,
											["messageId"]= 5005 
										}
				SystemMsgDB.SaveSystemMsg(parameter)
			end
		else
			order_item_ids:Release()  
		end
	end
	all_orders:Release()
	return true, new_mail_playerID_tbl
end

--检查订单是否到期
function ConsignmentDB.OrderTimeTest()
	local sell_order_out_time, sell_playerID_tbl 	=	ConsignmentDB.SellsIsOrNotOuttime()
	local PurchasingAgentDB=	RequireDbBox("PurchasingAgentDB")
	PurchasingAgentDB.OrderIsOrNotOuttime()
	local result = {["SellOrderOutTime"]=sell_order_out_time,["SellPlayerIDTbl"]= sell_playerID_tbl}
	return result
end

--查询收购订单
function ConsignmentDB.CSMSearchBuyOrder(data)
	local uCharId			=	data["CharID"]
	local uType				=	data["Type"]
	local sName				=	data["Name"]
	local uBeginLevel		=	data["BeginLevel"]
	local uEndLevel			=	data["EndLevel"]
	local OrderBy			=	data["OrderBy"]
	local Page				=	data["Page"]
	local flag = data["flag"] 
	local IsZhanshen = data["IsZhanshen"] 
	local IsFamo = data["IsFamo"]
	
	local count, ret, itemInfoTbl = ConsignmentDB.SearchCommonBuyOrder(nil, sName, uBeginLevel, uEndLevel, OrderBy, Page,false,flag,IsZhanshen,IsFamo)
	local result = {["Count"]=count , ["Ret"]=ret, ["ItemInfoTbl"]= itemInfoTbl}
	
	return true, result
end

--查询出售订单
function ConsignmentDB.CSMSearchSellOrder(data)
	local uCharId			=	data["CharID"]
	local sName				=	data["Name"]
	local uBeginLevel		=	data["BeginLevel"]
	local uEndLevel			=	data["EndLevel"]
	local OrderBy			=	data["OrderBy"]
	local Page				=	data["Page"]
	local flag              =   data["flag"] 
	local advanceSoul       =   data["AdvanceSoul"] 
	local maxAdvancePhase   =   data["MaxAdvancePhase"]
	local intenSoul         =   data["IntenSoul"]
	local maxIntenPhase     =   data["MaxIntenPhase"]
	local quality           =   data["Quality"]
	local campFlag          =   data["CampFlag"]
	local count, ret, itemsInfoTbl, OrderItemIDTbl = ConsignmentDB.SearchCommonSellOrder(uCharId, nil, sName, uBeginLevel, uEndLevel, OrderBy, Page,false,flag,advanceSoul,maxAdvancePhase, nil, intenSoul, maxIntenPhase, quality, campFlag)
	local result = {["Count"]=count , ["Ret"]=ret, ["ItemInfoTbl"] = itemsInfoTbl, ["OrderItemIDTbl"] = OrderItemIDTbl}
	
	return true, result
end


--角色查询个人出售列表
function ConsignmentDB.SearchUserSellOrder(uCharId, OrderBy, Page)
	assert(IsNumber(uCharId))
	local result = {}

	local timeoutFlag,new_mail_playerID_tbl = ConsignmentDB.SellsIsOrNotOuttime(uCharId)
	result["SellOrderOutTime"] = timeoutFlag
	result["SellPlayerIDTbl"] = new_mail_playerID_tbl
	
	local query_sql = "select mso.mos_uId,c.c_sName, mso.mso_sItemType, mso.mso_sItemName, mso.mso_uPrice, mso.mso_uCount, ((unix_timestamp(mso.mso_tEndTime) - unix_timestamp(now()))/3600),c.cs_uId" 
	local query_string = " from tbl_char c,tbl_market_sell_order mso,tbl_market_order_static mos where unix_timestamp(mso.mso_tEndTime) > 0 and mso.mso_uCount > 0 and mso.cs_uId = c.cs_uId and mso.mos_uId = mos.mos_uId and mso.cs_uId = " .. uCharId .. " "

	query_string = query_sql .. query_string
	local _, query_result = g_DbChannelMgr:TextExecute(query_string)
	result["Ret"] = query_result
	result["Count"]  = 0
	if query_result == nil then
		return result
	end
	if query_result:GetRowNum() == 0 then
		return result
	end
	result["Count"] = query_result:GetRowNum()

	if (OrderBy == 1) then
		query_string = query_string .. " order by mso.mso_sItemName"
	
	elseif (OrderBy == 2) then
		query_string = query_string .. " order by mso.mso_uPrice"
--	elseif (OrderBy == 3) then
--		query_string = query_string .. " order by mso.cs_uId"
    elseif (OrderBy == 3) then
		query_string = query_string .. " order by mso.mso_uCanUseLevel"
	elseif (OrderBy == 4) then
		query_string = query_string .. " order by mso.mso_tEndTime"
	end

	local page = Page or 1
	local begin_record = page * 50 - 50
	query_string = query_string .. " limit " .. begin_record .. " , 50"

	local _, query_result1 = g_DbChannelMgr:TextExecute(query_string)
	result["Ret"] = query_result1
	if query_result1 == nil then
		return result
	end
	if query_result1:GetRowNum() == 0 then
		return result
	end
	local query_count_result = query_result1:GetRowNum()
	
	local itemDynInfoDB = RequireDbBox("GetItemDynInfoDB")	 
	local itemsInfoTbl = {}
	local orderItemIdTbl = {}
	local tbl_id = {}
	for i = 1,query_count_result do
		local itemid = 0
		local ret = CMarket._SelectBuyOrderItem:ExecSql("n", query_result1(i,1))
		if ret:GetRowNum()> 0  then
			itemid = ret:GetData(0,0)
			table.insert(tbl_id,itemid)
			
		    local itemInfoTbl = itemDynInfoDB.GetItemDynInfo(itemid, query_result1(i,3),uCharId,uCharId, ret)
	        table.insert(itemsInfoTbl, itemInfoTbl)
	    else
	        result["Count"] = result["Count"] - 1 
	        table.insert(itemsInfoTbl, {})
		end
		table.insert(orderItemIdTbl,itemid)

	end
	
	result["Ret"] = query_result1
	result["ItemInfoTbl"] = itemsInfoTbl
	result["orderItemIDTbl"] = orderItemIdTbl
    --query_count_result_total, query_result1, itemsInfoTbl, orderItemIdTbl, timeoutFlag, new_mail_playerID_tbl

	return result
end

function ConsignmentDB.CSMSearchCharSellOrder(data)
	local uCharId			=	data["CharId"]
	local orderBy			=	data["OrderBy"]
	local page				=	data["Page"]
	
	--[[
	local count, ret, itemInfoTbl , orderItemIDTbl, timeoutFlag, new_mail_playerID_tbl=	ConsignmentDB.SearchUserSellOrder(uCharId, orderBy, page)
	local result = {["Count"]=count, ["Ret"]= ret, ["ItemInfoTbl"] = itemInfoTbl, ["orderItemIDTbl"] = orderItemIDTbl}
	result["SellPlayerIDTbl"] = new_mail_playerID_tbl
	result["SellOrderOutTime"] = timeoutFlag
	--]]
	local result = ConsignmentDB.SearchUserSellOrder(uCharId, orderBy, page)
	return true, result
end

--角色查询个人收购列表
function ConsignmentDB.SearchUserBuyOrder(uCharId, OrderBy, Page)
	assert(IsNumber(uCharId))
	local query_sql = "select mbo.mos_uId, c.c_sName, mbo.mbo_sItemType, mbo.mbo_sItemName, mbo.mbo_uPrice, mbo.mbo_uCount, ((unix_timestamp(mbo.mbo_tEndTime) - unix_timestamp(now()))/3600), mbo.mbo_isUId,c.cs_uId " 
	local query_string = " from tbl_char c,tbl_market_buy_order mbo where unix_timestamp(mbo.mbo_tEndTime) >0 and mbo.cs_uId = c.cs_uId and mbo.cs_uId = " .. uCharId .. " "

	query_string = query_sql .. query_string
	local report, query_result = g_DbChannelMgr:TextExecute(query_string)
	local query_result_tbl = {}
	local query_finish_result_tbl = {}
	local query_count_result = 0
	if query_count_result == nil then
		return query_count_result, query_result_tbl, query_finish_result_tbl
	end
	if query_result:GetRowNum() == 0 then
		query_result:Release()
		return query_count_result, query_result_tbl, query_finish_result_tbl
	end
	query_count_result = query_result:GetRowNum()

	if (OrderBy == 1) then
		query_string = query_string .. " order by mbo.mbo_sItemName"
	elseif (OrderBy == 2) then
		query_string = query_string .. " order by mbo.mbo_uCount"
	elseif (OrderBy == 3) then
		query_string = query_string .. " order by mbo.cs_uId"
	elseif (OrderBy == 4) then
		query_string = query_string .. " order by mbo.mbo_uPrice"
	elseif (OrderBy == 5) then
		query_string = query_string .. " order by mbo.mbo_tEndTime"
	end

	local page = Page or 1
	local begin_record = page * 50 - 50
	query_string = query_string .. " limit " .. begin_record .. " , 50"

	local query_finish_num = "select im.mos_uId, count(im.is_uId) from tbl_item_market im join tbl_market_buy_order mbo on (im.mos_uId = mbo.mos_uId) where mbo.cs_uId = " .. uCharId .. " group by im.mos_uId"

	local report, query_result1 = g_DbChannelMgr:TextExecute(query_string)
	local _, query_finish_result = g_DbChannelMgr:TextExecute(query_finish_num)
	if query_result1 == nil then
		return 0, {}, {}
	end
	if query_result1:GetRowNum() == 0 then
		query_result1:Release()
		return 0, {}, {}
	end
	query_result_tbl = query_result1:ToTable(true)

	if query_finish_result ~= nil and query_finish_result:GetRowNum() ~= 0  then
		query_finish_result_tbl = query_finish_result:ToTable(true)
	else
		query_finish_result:Release()
	end
 
	local itemsInfoTbl = {}
	return query_count_result, query_result_tbl, query_finish_result_tbl, itemsInfoTbl
end

--判断某玩家是否有可提取的物品
local StmtDef=
{
	"_SelectAllOrderItems",
	[[
	select mar.is_uId 
	from tbl_item_market mar,tbl_market_buy_order mos
	where mos.cs_uId=? and mos.mos_uId=mar.mos_uId
	]]
}
DefineSql(StmtDef,CMarket)
function  ConsignmentDB.IsOrNotAttachment(uCharId)
	assert(IsNumber(uCharId))
	local item_ids=CMarket._SelectAllOrderItems:ExecSql('n',uCharId)

	if 0== item_ids:GetRowNum() then
		item_ids:Release()
		return false
	end
	item_ids:Release()
	return true
end

function ConsignmentDB.CSMSearchCharBuyOrder(data)
	local uCharId			=	data["CharId"]
	local orderBy			=	data["OrderBy"]
	local page				=	data["Page"]
	local count, ret, finish_buy_order, itemInfoTbl =	ConsignmentDB.SearchUserBuyOrder(uCharId, orderBy, page)
	local isHaveAttachment		=  ConsignmentDB.IsOrNotAttachment(uCharId)	
	local result = {["Count"]=count, ["Ret"]= ret, 
					["FinishBuyOrder"]=finish_buy_order, ["IsHaveAttachment"]=isHaveAttachment,
					["ItemInfoTbl"] = itemInfoTbl }
	
	return true, result

end

local StmtDef =
{
	"_UpdateBuyOrderCount",
	[[
	update tbl_market_buy_order 
	set mbo_uCount = mbo_uCount - ?
	where mos_uId = ? and mbo_uCount - ? >= 0
	]]
}
DefineSql(StmtDef, CMarket)
local StmtDef = 
{
	"_GetItemPos",
	[[
	select gir_uRoomIndex, gir_uPos
	from tbl_item_in_grid as iip,tbl_grid_in_room as igp
	where iip.gir_uGridID = igp.gir_uGridID and iip.is_uId = ?
	]]
}
DefineSql(StmtDef, CMarket)
local StmtDef =
{
	"_SelectMarketCounts",
	[[
	select mbo_uCount from tbl_market_buy_order where mos_uId=? 
	]]
}
DefineSql(StmtDef, CMarket)
local StmtDef =
{
	"_SelectorderItems",
	[[
	select is_uId from tbl_item_market where mos_uId=? 
	]]
}
DefineSql(StmtDef, CMarket)
function ConsignmentDB.CSMSellGoods2Order(data)
	local uCount	=	data["Count"]
	local uOrderId	=	data["OrderID"]
	local uCharId	=	data["CharID"]
	assert(IsNumber(uCount) and uCount > 0)
	assert(IsNumber(uOrderId))
	
	local info = ConsignmentDB.GetBuyOrderInfo(uOrderId)
	if info == nil then
		--,'该订单已经不存在！'
		return false, 8025
	end

	if info[2] == uCharId then
		--, '不能出售自己的收购品！'
		return  false, 8026
	end
	local uBigId, sItemName = tonumber(info[3]), info[4]
	
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	if g_RoomMgr.CountItemLimitByName(uBigId, sItemName) > 0 then
		return false,21
	end
	local count = g_RoomMgr.GetItemCount(uCharId, uBigId, sItemName)
	if count == 0 then
		--, '您没有该物品可以出售！'
		return false, 8027
	end
	if  uCount > info[5] then
		--, '你输入的数量大于购买者需购买数量！'
		return false, 8028
	end
	CMarket._UpdateBuyOrderCount:ExecSql("", uCount, uOrderId, uCount)
	if g_DbChannelMgr:LastAffectedRowNum() < 1 then
		CancelTran()
		return false
	end
	local tbl_item_id = g_RoomMgr.GetNCountItemIdByName(uCharId, uBigId, sItemName, uCount)
	local tbl_ids={}
	local ItemData = {}

	for i = 1, tbl_item_id:GetRowNum() do
		--向另一张表里面添加
		local nItemID =  tbl_item_id(i,1)
		CMarket._AddItemToOrder:ExecSql("",nItemID, uOrderId)
		if g_DbChannelMgr:LastAffectedRowNum() < 1  then
			CancelTran()
			--, '添加订单不成功！'
			return false, 8029
		end
		local pos = CMarket._GetItemPos:ExecSql('nn',nItemID)
		if pos:GetRowNum() == 0 then
			pos:Release()
			CancelTran()
			--, '物品位置错误！'
			return false, 8030
		end
		
		if not g_RoomMgr.MoveItemFromPackage(uCharId, nItemID) then
			CancelTran()
			--, '删除物品错误！'
			return false, 8019
		end
		table.insert(tbl_ids,{pos:GetData(0,0),pos:GetData(0,1),nItemID})
		table.insert( ItemData, nItemID )
		pos:Release()
	end

	--给出售者发送金钱
	local lSysMailExecutor = RequireDbBox("SysMailDB")
	local succ2=lSysMailExecutor.SendSysMail(1001,uCharId,1002,
	"1005_item:" .. uBigId .. "|" .. sItemName,nil,uCount * info[6])
	if not succ2 then
		CancelTran()
		--, '发送物品不成功！'
		return  false, 8031
	end

	--收购数量已够，发送到收购者邮箱
	local counts = CMarket._SelectMarketCounts:ExecSql('n',uOrderId)

	if counts:GetRowNum() == 0 then
		counts:Release()
		return false
	end

	if 0==counts:GetData(0,0)then 
		local order_item_ids = CMarket._SelectorderItems:ExecSql('n',uOrderId)
		if order_item_ids:GetRowNum() ~= 0 then
			local order_item_ids_tbl = order_item_ids:ToTable(true)
			local succ=lSysMailExecutor.SendSysMail(1001,info[2] ,1002,
			"1006_item:" .. uBigId .. "|" .. sItemName,order_item_ids_tbl,0)
			if not succ then
				counts:Release()
				CancelTran()
				--, '发送物品失败！'
				return  false, 8031
			end
			CMarket._DeleteOrderItem:ExecSql('',uOrderId)
			if g_DbChannelMgr:LastAffectedRowNum() < 1 then
				counts:Release()
				CancelTran()
				--, '发送物品失败！'
				return false, 8031
			end
			CMarket._CancelBuyOrder:ExecSql("", uOrderId)
			if g_DbChannelMgr:LastAffectedRowNum() < 1 then
				counts:Release()
				CancelTran()
				--, '发送物品失败！'
				return false, 8031
			end
		else
			order_item_ids:Release()  
		end
	end

	local MoneyType = 1
	local g_LogMgr = RequireDbBox("LogMgrDB")
	--g_LogMgr.LogConsignmentBuyOrder( uCharId, info[2],ItemData )
	--g_LogMgr.LogConsignmentBuyOrder(  info[2], uCharId,{} , MoneyType, uCount * info[6])
	counts:Release()
	
	--如果收购者不在线，将小助手提示消息保存数据库。
	local LoginServerSql = RequireDbBox("LoginServerDB")
	local SystemMsgDB = RequireDbBox("SystemMsgDB")
	if not LoginServerSql.IsPlayerOnLine(info[2]) then
		local parameter = {
											["uCharId"] = info[2],
											["messageId"]= 5003 
										}
		SystemMsgDB.SaveSystemMsg(parameter)
	end
	
	local result = {["TblIds"] =tbl_ids, ["PlayerID"] =info[2]}
	
	return true, result
end

--判断该角色的金钱是否购买
local StmtDef = 
{
	"_SelectMoney",
	[[
	select cm_uMoney
	from tbl_char_money
	where cs_uId = ?
	]]
}
DefineSql(StmtDef, CMarket)

function ConsignmentDB.IsOrNotEnough(nCharId, money_num)
	assert(IsNumber(nCharId))
	assert(IsNumber(money_num))
	local cs_money = CMarket._SelectMoney:ExecSql('n', nCharId)
	if cs_money:GetRowNum() == 0  then
		cs_money:Release()
		return false
	end
	if cs_money:GetData(0,0) < money_num then
		cs_money:Release()
		return false
	end
	cs_money:Release()
	return true
end
--购买订单
--1表示购买成功；
--2表示moeny不足
--3表示物品已经售出
function ConsignmentDB.CSMBuyOrder(data)
	local uCharId	=	data["CharID"]
	local uOrderId	=	data["OrderID"]
	assert(IsNumber(uOrderId))
	assert(IsNumber(uCharId))
	
	local ItemBagLockDB = RequireDbBox("ItemBagLockDB")
	--帐户锁定，不能购买物品
    if ItemBagLockDB.HaveItemBagLock(uCharId) then
        return false,160047
    end
  
	local errorType 	
	local info = ConsignmentDB.GetSellOrderInfo(uOrderId)
	if info == nil then
		--, '该订单已经不存在！' 
		return false, 8025
	end
	local uBigId, sItemName = tonumber(info[3]), info[4]
	local ret = CMarket._SelectBuyOrderItem:ExecSql("n", uOrderId)
	if ret:GetRowNum() == 0 then
		ret:Release()
		errorType = 3
		--订单错误
		return false, 8008, errorType
	end
	local ret_tbl = ret:ToTable(true)
	if ConsignmentDB.IsOrNotEnough(uCharId, 1 * info[6]) == false then      --判断金钱是否足够
		errorType = 2
		-- '钱不够！'
		return false, 8032, erroType
	end
	
	local lSysMailExecutor = RequireDbBox("SysMailDB")
	local succ=lSysMailExecutor.SendSysMail(1001,uCharId,1002,
	"1007_item:" .. uBigId .. "|" .. sItemName,ret_tbl,0,113, 5003)
	if not succ then
	    LogErr("拍卖行购买物品成功，邮寄出错", "OrderID" .. uOrderId)
		CancelTran()
		return false
	end
	CMarket._CancelSellOrder:ExecSql("", uOrderId)
	if not (g_DbChannelMgr:LastAffectedRowNum() > 0) then
		CancelTran()
		--, '取消订单错误！'
		return false, 8010
	end

	local MoneyManagerDB=	RequireDbBox("MoneyMgrDB")
	local fun_info = g_MoneyMgr:GetFuncInfo("CSM")
	
	local g_LogMgr = RequireDbBox("LogMgrDB")
	local itemTbl = {}
	if #ret_tbl > 0 then
		for i = 1,#ret_tbl do
			table.insert(itemTbl,ret_tbl[i][1])
		end
	end 
	local moneySellerGot = g_MoneyMgr:GetAftertaxMoney(1 * info[6]) 
	local sysFee = 1 * info[6] - moneySellerGot
	local uEventId = g_LogMgr.LogConsignmentBuyItem( uCharId, itemTbl, info[1])
	local bFlag,uMsgID = MoneyManagerDB.AddMoney(fun_info["FunName"],fun_info["BuyOrder"],uCharId, -(1* info[6]),uEventId,nil,info[2],moneySellerGot)
	if not bFlag then
		CancelTran()
		--, "收手续费出错！" 
		if IsNumber(uMsgID) then
			return false,uMsgID
		else
			return false, 8033
		end
	end
	--TODO
	--发送系统邮件到出售者
	local succ2=lSysMailExecutor.SendSysMail(1001,info[2],1002,
		"1008_item:" .. uBigId .. "|" .. sItemName,nil, moneySellerGot, 114,5002)
	if not (succ2) then
		CancelTran()
		--, '发送邮件错误！'
		return false, 8031
	end

	local ItemData = {}
	for i= 1 ,info[5] do 
		table.insert( ItemData, ret_tbl[i][1] )
	end

	local result = {["PlayerID"] =info[2], ["CostMoney"] = 1 * info[6]}

    g_LogMgr.SaveTaxLog(sysFee, uCharId, event_type_tbl["寄售交易所扣税"])
	return true, result
end

local StmtDef = 
{
	"deleteCfgInfo",
	[[
	    delete from tbl_market_cfg_info
	]]
}
DefineSql(StmtDef, CMarket)


function ConsignmentDB.ReadCfgToCSMDBTable()
    CMarket.deleteCfgInfo:ExecStat()
    local sql = "insert into tbl_market_cfg_info (mci_uType, mci_sName, mci_uAttrIndex, mci_uCamp) values "
    local equipCfgInfo = { {5, Equip_Weapon_Common, "EquipType"}, 
                           {6, Equip_Armor_Common,  "EquipPart"}, 
                           {7, Equip_Shield_Common, "EquipType"},
                           {8, Equip_Ring_Common, "EquipType"}, 
                           {9, Equip_Jewelry_Common,"EquipPart"}}  
    for j =1, 5 do
        local itemType = equipCfgInfo[j][1]    --武器类型
        local cfg = equipCfgInfo[j][2]
        local equipTypeStr = equipCfgInfo[j][3]
        local sqlStrTbl = {}
        local ItemNames = cfg:GetKeys()
        for i=1,#ItemNames do
            local itemName = ItemNames[i]
            local advanceSoulRoot = GetEquipAdvanceSoul(g_ItemInfoMgr:GetItemLanInfo(itemType, itemName,"DisplayAdvancedRoot"))
            local maxAdvancePhase = cfg(itemName,"AdvancedQuality")
            local intenSoulRoot = GetEquipIntenSoul(g_ItemInfoMgr:GetItemLanInfo(itemType, itemName,"SoulRootDisplayName"))
            local maxIntenPhase = cfg(itemName,"IntensifyQuality")
            local equipAttrIndex = AttrNameMapIndexTbl[cfg(itemName,equipTypeStr)] or 0 --0表示未分类
            local camp = 0
            if tonumber(cfg(itemName,"Camp") ) ~= nil then
                camp = cfg(itemName,"Camp")
            end    
            
            local sqlStr = string.format("%s%d%s%s%s%s%s%d%s%d%s", "(", itemType, ",", "'", itemName, "'", ",", equipAttrIndex, ",", camp ,")")
            table.insert(sqlStrTbl,sqlStr)
        end
        local sqlStatment =  sql .. table.concat(sqlStrTbl, ",")
        g_DbChannelMgr:TextExecute(sqlStatment)

    end

    local sqlItemCommon = "insert into tbl_market_cfg_info (mci_uType, mci_sName, mci_uAttrIndex,mci_uCamp) values"
    local itemCfgInfo = { {g_ItemInfoMgr:GetStoneBigID(), Stone_Common, "StoneType"},
                          {g_ItemInfoMgr:GetSkillItemBigID(), SkillItem_Common, "ItemType"},
                          {g_ItemInfoMgr:GetBasicItemBigID(), BasicItem_Common, "CSMSortType"}}
    for i=1, 3 do
        local itemType  = itemCfgInfo[i][1]
        local cfg = itemCfgInfo[i][2]
        local cfgItemTypeStr = itemCfgInfo[i][3]
        local sqlStrTbl = {}
        local ItemNames = cfg:GetKeys()
        for j =1,#ItemNames do
            local itemName = ItemNames[j]
            local attrIndex = AttrNameMapIndexTbl[cfg(itemName,cfgItemTypeStr)] or 0
            local camp = 0
            if tonumber(cfg(itemName,"Camp")) ~= nil then
               camp = cfg(itemName,"Camp")
            end
            local sqlStr = string.format("%s%d%s%s%s%d%s%d%s", "(" ,itemType , ", '" , itemName , "', " , attrIndex , "," , camp , ")")
            table.insert(sqlStrTbl, sqlStr)
        end
        
        local sqlStatement = sqlItemCommon .. table.concat(sqlStrTbl, ",")
        g_DbChannelMgr:TextExecute(sqlStatement)
    end
    
end



SetDbLocalFuncType(ConsignmentDB.CSMSearchBuyOrder)
SetDbLocalFuncType(ConsignmentDB.CSMSearchSellOrder)
SetDbLocalFuncType(ConsignmentDB.CSMSearchCharBuyOrder)
SetDbLocalFuncType(ConsignmentDB.CSMSearchCharSellOrder)
SetDbLocalFuncType(ConsignmentDB.GetCSMRememberPrice)
SetDbLocalFuncType(ConsignmentDB.CSMGetOrderByItemType)
SetDbLocalFuncType(ConsignmentDB.CSMGetOrderByItemAttr)
SetDbLocalFuncType(ConsignmentDB.CSMGetOrderBySomeItemType)
SetDbLocalFuncType(ConsignmentDB.CSMGetOrderByExactItemAttr)
SetDbLocalFuncType(ConsignmentDB.ReadCfgToCSMDBTable)
SetDbLocalFuncType(ConsignmentDB.CSMAddSellOrder)

return ConsignmentDB
