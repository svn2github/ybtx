cfg_load "npc/NpcShop_Common"
gac_gas_require "item/SoulpearlCommonFunc"
gac_gas_require "framework/common/CMoney"
gac_gas_require "commerce/npc_shop/NPCShopMgr"


local GetItemPriceInTongShop = GetItemPriceInTongShop
local CheckNpcShopItemTongLevel = CheckNpcShopItemTongLevel
local g_MoneyMgr = CMoney:new()
local GetSoulpearlPriceByTypeAndCount = GetSoulpearlPriceByTypeAndCount
local g_ItemInfoMgr 		= 	CItemInfoMgr:new()
local g_StoreRoomIndex		=	g_StoreRoomIndex
local IsNil					=	IsNil
local GetItemJiFenPrice     =   GetItemJiFenPrice
local GetItemPriceByPayType =   GetItemPriceByPayType
local NpcShop_Common = NpcShop_Common
local LogErr = LogErr
local event_type_tbl = event_type_tbl
local NpcShopDbBox = CreateDbBox(...)

local LiuTongMoneyType 	= 1 	--流通金币支付方式
local BindMoneyType 	= 2		--绑定金币支付方式
local TongNpcShop = 10--团贡商店，不允许玩家卖东西到商店
--玩家身上的几种积分
--[[
			自然积分  ----------------1
			暗黑积分  ----------------2
			破坏积分  ----------------3
			大逃杀积分 ---------------4
			积分赛积分 ---------------5
			护甲积分  ----------------6
--]]
local NpcShop = {}
--------------------------------------------------------------------------------------------------------------------

--出售物品，支付方式是积分
local function SellItemByPoint(ItemName,pay_type,nCount,uCharId, itemType, itemID,sSceneName,uEventId)
	local price = 0
	if g_ItemInfoMgr:IsSoulPearl(itemType) then
	    local g_RoomMgr = RequireDbBox("GasRoomMgrDB")	
	    local soulCount = g_RoomMgr.GetSoulPearlInfoByID(itemID)
        price = GetSoulpearlPriceByTypeAndCount(ItemName,soulCount, pay_type)
	else
        price = GetItemJiFenPrice(itemType, ItemName, 1, pay_type)
	end
	local all_point = math.ceil(price* 1/4) * nCount 
	
	--更新玩家数据库的积分
	local AreaFbPointDB = RequireDbBox("AreaFbPointDB")
	local bFlag,uMsgID = AreaFbPointDB.AddAreaFbPointByType(uCharId,all_point,pay_type,sSceneName,uEventId)
	local DirectDB = RequireDbBox("DirectDB")
	local DirectTbl = DirectDB.AddPlayerJifenDirect(uCharId, "商店获得积分", pay_type, all_point)
	
	return bFlag,all_point,uMsgID,DirectTbl
end

-------------------------------------------------------------------------------------
local StmtDef = {
	"_SaveSellItemInfo",
	[[
		insert into tbl_npcshop_player_sell_item set cs_uId = ?, npsi_nShopType= ?,
		 npsi_sItemType= ?, npsi_sItemName= ?, npsi_nItemCount = ? , npsi_nBindingType=?
	]]
}
DefineSql(StmtDef, NpcShop)

local StmtDef = {
	"_SelectSoldItemCount",
	[[
		select count(*) from tbl_npcshop_player_sell_item where cs_uId = ?
	]]
}
DefineSql(StmtDef, NpcShop)

local StmtDef = {
	"_SelectMinSoldItemIndex",
	[[
		select min(npsi_nId) from tbl_npcshop_player_sell_item where cs_uId = ?
	]]
}
DefineSql(StmtDef, NpcShop)

local StmtDef = {
	"_SaveSellItemId",
	[[
		insert into tbl_player_sell_item_id values(?,?)
	]]
}
DefineSql(StmtDef, NpcShop)

local StmtDef = {
	"_SelectSellItemId",
	[[
		select psii_nItemId from tbl_player_sell_item_id where psii_nId = ?
	]]
}
DefineSql(StmtDef, NpcShop)

--玩家出售物品
function NpcShopDbBox.PlayerSellItemByPos(data)
	local uCharId		=	data["CharID"]
	local nRoomIndex	=	data["RoomIndex"]
	local nPos			=	data["Pos"]
	local nCount		=	data["ItemCount"]
	local npcName		=	data["NpcName"]
	local sSceneName = data["sSceneName"]
	local pay_type = 0
	local succ = false
	local sellItemGetPoint
	local all_cost = 0
	local npcshop_info = NpcShop_Common(npcName)
	
	local ItemBagLockDB = RequireDbBox("ItemBagLockDB")
  if ItemBagLockDB.HaveItemBagLock(uCharId) then
  	local result = {}
		result["MsgID"]= 160019
  	return false,result
  end
	
	if npcshop_info then
		pay_type = NpcShop_Common(npcName, "PayType")
	end
	
	if pay_type == TongNpcShop then --团贡商店
        local result = {}
		result["MsgID"] = 6119
		result["ShopType"] = pay_type
        return false,  result
	end
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")	
	local sType, sName, count = g_RoomMgr.GetTypeCountByPosition(uCharId, nRoomIndex, nPos)
	if count == nil or count == 0 then
		--, "非法操作！"
		local result = {}
		result["MsgID"] = 8012
		result["ShopType"] = pay_type
		return false, result
	end
	if g_ItemInfoMgr:IsSoulPearl(sType) then
		--叠加物品的魂值不一样，不能出售
		if g_RoomMgr.CountSoulNumByPos(uCharId, nRoomIndex, nPos) > 1 then
			return false,{["MsgID"] = 6123,["ShopType"] = pay_type}
		end
	end
	
	if g_RoomMgr.CountItemLimitByName(sType, sName) > 0 then
		CancelTran()
		return false,{["MsgID"] = 21,["ShopType"] = pay_type}
	end
	
	if g_ItemInfoMgr:IsMailTextAttachment(sType) then
		local result = {}
		result["MsgID"]	= 6100					--"不能出售邮件文本附件！"
		result["ShopType"] = pay_type
		return false, result
	end
	
	if g_ItemInfoMgr:IsQuestItem(sType) then		--不能出售任务物品
		local result = {}
		result["MsgID"] = 6100
		result["ShopType"] = pay_type
		return false, result
	end
	if g_ItemInfoMgr:IsPickOreItem(sType) then
		local PickOreItemDB = RequireDbBox("PickOreItemDB")
		local param = data
		param["PlayerID"] = data["CharID"]
		param["ItemName"] = sName
		local DuraTbl = PickOreItemDB.QueryPickOreItemInfo(param)
		if DuraTbl and DuraTbl[2] < DuraTbl[3] then
			local result = {}
			result["MsgID"] = 6100
			result["ShopType"] = pay_type
			return false, result
		end
	end
	if count == nil then
	    local result = {}
		result["MsgID"] 	= 6101				--那个格子没有物品
		result["ShopType"] = pay_type
		return false, result		
	end
	if count < nCount or nCount < 1 then
		local result = {}
		result["MsgID"] 	= 6101				--外挂
		result["ShopType"] = pay_type
		return false, result	
	end
	local tbl_item_id = g_RoomMgr.GetAllItemFromPosition(uCharId, nRoomIndex, nPos)

	if g_ItemInfoMgr:IsSoulPearl(sType) then
    	local soulCount = g_RoomMgr.GetSoulPearlInfoByID(tbl_item_id(1,1))
    	if GetSoulpearlPriceByTypeAndCount(sName,soulCount, pay_type) <= 0 then
    	    local result = {}
    	    result["MsgID"] = 6101
    	    result["ShopType"] = pay_type
            return false, result 
    	end
    else
        if GetItemPriceByPayType(sType, sName, pay_type) == 0 then
    		local result = {}
    	    result["MsgID"]  	= 6101					--该物品没有售价
    	    result["ShopType"] = pay_type
    		return false, result	
    	end
    end
    
    local tbl_sold_item = {}
	for i = 1, nCount do
		table.insert(tbl_sold_item, tbl_item_id(i,1))
	end
  local moneyType = BindMoneyType
	local bindingType = g_RoomMgr.GetItemBindingTypeByID(tbl_item_id(1,1)) or 0
	if 3 == bindingType then
		return false,{["MsgID"] = 6117,["ShopType"] = pay_type}				--不能出售佣兵团绑定物品
	end
  local g_LogMgr = RequireDbBox("LogMgrDB")
	local uEventId = g_LogMgr.LogPlayerGiverToNpc(uCharId, npcName, tbl_sold_item ,event_type_tbl["npc商店出售"] )
	--该物品的支付方式是积分
	local DirectTbl
	if pay_type ~= 0 then
		local bFlag,GetPoint,uMsgID,Direct = SellItemByPoint(sName,pay_type,nCount,uCharId, sType, tbl_item_id(1,1),sSceneName,uEventId)
		succ,sellItemGetPoint = bFlag,GetPoint
		if (not bFlag) then
			if IsNumber(uMsgID) then
				return false,{["MsgID"]= uMsgID,["ShopType"]= pay_type}
			end
		end
		DirectTbl = Direct
	else
	    if g_ItemInfoMgr:IsSoulPearl(sType) then
    	    local soulCount = g_RoomMgr.GetSoulPearlInfoByID(tbl_item_id(1,1))
            local pearlPrice = GetSoulpearlPriceByTypeAndCount(sName,soulCount, pay_type)
            all_cost = math.ceil( pearlPrice / 4) * nCount
        else
            all_cost = math.ceil( g_ItemInfoMgr:GetItemInfo(sType, sName,"Price") / 4) * nCount
        end

		local MoneyManagerDB =	RequireDbBox("MoneyMgrDB")
		local fun_info = g_MoneyMgr:GetFuncInfo("NpcShop")
			--因为我们的游戏中有四种钱，要确定玩家到底是用的哪种钱才可以做相应的log
		local bFlag,uMsgID = MoneyManagerDB.AddMoneyByType(fun_info["FunName"],fun_info["Sell"],uCharId,moneyType, all_cost,uEventId)
		if not bFlag then
			CancelTran()
			if IsNumber(uMsgID) then
				return false,{["MsgID"]= uMsgID,["ShopType"]= pay_type}
			else
				return false,{["MsgID"]= 6103,["ShopType"]= pay_type}
			end
		end
	end
	
	for i = 1, #(tbl_sold_item) do
		if not g_RoomMgr.MoveItemFromPackage(uCharId, tbl_sold_item[i]) then
			CancelTran()
			local result = {}
    	    result["MsgID"]   = 6103
    	    result["ShopType"] = 6103
			return false, result
		end
	end
	
	local itemDynInfoDB = RequireDbBox("GetItemDynInfoDB")	
	local itemInfoTbl = itemDynInfoDB.GetItemDynInfo(tbl_sold_item[1], sType, uCharId, uCharId, tbl_sold_item)
	local result = {["ItemType"] = tonumber(sType),
									["ItemName"]= sName, 
									["ItemIDTbl"] =tbl_sold_item,
									["MoneyType"] =moneyType,
									["MoneyDelta"] =all_cost,
									["ItemInfoTbl"] = itemInfoTbl,
									["ItemBindingType"] = bindingType,
									["pay_type"] = pay_type,
									["SellItemGetPoint"] = sellItemGetPoint,
									["DirectTbl"] = DirectTbl
									}
	local sold_item_count = NpcShop._SelectSoldItemCount:ExecStat(uCharId)
	if sold_item_count:GetData(0,0) < 12 then
		NpcShop._SaveSellItemInfo:ExecStat(uCharId, pay_type, sType, sName, nCount, bindingType)
	else
		local min_id = NpcShop._SelectMinSoldItemIndex:ExecStat(uCharId)
		local item_res = NpcShop._SelectSellItemId:ExecStat(min_id:GetData(0,0))
		local lRoomMgr = RequireDbBox("GasRoomMgrDB")
		for i = 0,item_res:GetRowNum() - 1 do
			local succ = lRoomMgr.DelItemFromStaticTable(item_res:GetData(i,0),uCharId,event_type_tbl["npc商店出售"])
			if not succ then
				CancelTran()
				return false
			end
		end
		NpcShop._DelSellItemInfo:ExecStat(uCharId,min_id:GetData(0,0))
		NpcShop._SaveSellItemInfo:ExecStat(uCharId, pay_type, sType, sName, nCount, bindingType)
	end
	if g_DbChannelMgr:LastAffectedRowNum() == 0 then
	    CancelTran()
	    return false 
	 end
	local soldlistItemInfoID = g_DbChannelMgr:LastInsertId()
  for i = 1, #(tbl_sold_item) do
  	NpcShop._SaveSellItemId:ExecStat(soldlistItemInfoID,tbl_sold_item[i])
  	if g_DbChannelMgr:LastAffectedRowNum() == 0 then
	    CancelTran()
	    return false 
	  end
  end
  local tmp_data = {["uCharId"] = uCharId,["NpcName"] = npcName}
	result["item_list_tbl"] = NpcShopDbBox.GetSoldGoodsList(tmp_data)
	return true, result
end
------------------------------------------------------------------------------------------------
--检查身上的钱是否够
local function CheckMoneyEnough(uCharId, all_cost, curMoneyType)
	local MoneyManagerDB =	RequireDbBox("MoneyMgrDB")
	local moneyEnough  = true
	if curMoneyType == BindMoneyType then		--玩家希望用绑定金币支付
		if all_cost > MoneyManagerDB.GetBindingMoney(uCharId) then
			moneyEnough = false
		end
	elseif curMoneyType == LiuTongMoneyType then
		if all_cost > MoneyManagerDB.GetMoney(uCharId) then
			moneyEnough = false
		end
	else
		moneyEnough = false
	end
	return moneyEnough
end
------------------------------------------------------------------------------------------------
local StmtDef = {
	"_UpdateCurMoneyType",
	[[
		update tbl_char_basic set cb_uMoneyType = ?  where cs_uId = ?
	]]
}
DefineSql(StmtDef, NpcShop)

--用积分买东西
local function BuyItemByPoint(itemName,pay_type,nCount,uCharId,index, itemType, pearlPrice,sSceneName,uEventId)
	local price = 0
	if itemType ~=nil and g_ItemInfoMgr:IsSoulPearl(itemType) then
        price = pearlPrice
	else
	    price = GetItemJiFenPrice(itemType, itemName, 1, pay_type)
	    --price= g_ItemInfoMgr:GetItemInfo(itemType, itemName,"JiFen" .. pay_type)
	end
	local all_cost = math.ceil(price * index) * nCount 
	--根据支付方式得到相应的总积分
	local AreaFbPointDB = RequireDbBox("AreaFbPointDB")
	local all_point = AreaFbPointDB.GetPointStatement(uCharId,pay_type)
	--剩下的积分不足以购买该物品
	if all_cost > all_point or all_cost <=0 then
		return false
	end
	--更新玩家数据库的积分
	local bFlag,uMsgID = AreaFbPointDB.AddAreaFbPointByType(uCharId,-all_cost,pay_type,sSceneName,uEventId)
	return bFlag, - all_cost,uMsgID
end

function NpcShopDbBox.SubtractMoneyByBuyingItem(curMoneyType, uCharId, all_cost, liuTongShop,uEventId,ItemType,ItemName,ItemCount)
   local BuyCount = nil
    if liuTongShop == 1 then --流通商店只能用流通金币
        if curMoneyType == BindMoneyType then
            return false, 6120 
        end
        BuyCount = ItemCount
    end
	local curMoneyTypeRet =NpcShop._UpdateCurMoneyType:ExecSql("", curMoneyType, uCharId)
	local moneyEnough = CheckMoneyEnough(uCharId, all_cost, curMoneyType)
	if moneyEnough  == false or all_cost <= 0 then
	    CancelTran()
		return false, 6109
	end
	
	local MoneyManagerDB =	RequireDbBox("MoneyMgrDB")
	local fun_info = g_MoneyMgr:GetFuncInfo("NpcShop")	
	local bFlag,uMsgID = MoneyManagerDB.AddMoneyByType(fun_info["FunName"],fun_info["Buy"],uCharId, curMoneyType, - all_cost,uEventId,nil,nil,nil,ItemType,ItemName,BuyCount)
	if not bFlag then
		CancelTran()
		if IsNumber(uMsgID) then
			return false,uMsgID
		else
			return false,6109
		end
	end    
	return true
end

function NpcShopDbBox.SubtractJiFenByBuyingItem(curMoneyType, sName,pay_type,nCount,uCharId,nType, pearlPrice,sceneName,uEventId, liuTongShop)
	local needMoney, allCost = 0, 0
	local bSuc, modifiedJiFenCount,uMsgID= BuyItemByPoint(sName,pay_type,nCount,uCharId,1, nType, pearlPrice,sceneName,uEventId)
	if not bSuc  then
	    CancelTran()
		if IsNumber(uMsgID) then
			return false,uMsgID
		else
			return  false, 6108
		end
	end
	if pay_type == TongNpcShop then
        needMoney = GetItemPriceInTongShop(nType, sName, soulCount)
        allCost = needMoney * nCount
        if allCost > 0 then
            local suc, msgID = NpcShopDbBox.SubtractMoneyByBuyingItem(curMoneyType, uCharId, allCost, liuTongShop,uEventId,nType,sName,nCount)
            if suc ~= true then
                return false, msgID 
            end
            local CTongBasicBox  = RequireDbBox("GasTongBasicDB")
            local nTongID = CTongBasicBox.GetTongID(uCharId)
            local para = {["uTongID"]= nTongID, ["uMoney"] = allCost,["nEventType"] = event_type_tbl["团商店购买物品"]}
            local ret = CTongBasicBox.UpdateTongMoney(para)
            if suc ~= true then
                return false, 6109 
            end
        end
	end   
	local result = {}
	result["JiFenCount"] = modifiedJiFenCount
	result["Money"] = allCost
	return true,  result
end

--左键拖拉购买物品
function NpcShopDbBox.BuyItemByPos(data)
	local uCharId		=	data["CharID"]
	local nType			=	data["ItemType"]
	local sName			=	data["ItemName"]
	local nCount		=	data["ItemCount"]
	local nRoomIndex	=	data["RoomIndex"]
	local nPos			=	data["Pos"]
	local npcName		=	data["NpcName"]
	local payMoneyType  =   data["PayMoneyType"]
	local sceneName		=	data["sceneName"]
	local pay_type = 0
	local uMsgID
	local npcshop_info = NpcShop_Common(npcName)
	
	local ItemBagLockDB = RequireDbBox("ItemBagLockDB")
  if ItemBagLockDB.HaveItemBagLock(uCharId) then
  	return false,160018
  end
  
	if npcshop_info then
		pay_type = NpcShop_Common(npcName,"PayType")
	end
	
	if pay_type == TongNpcShop then
	    --团等级  
	    local CTongBasicBox  = RequireDbBox("GasTongBasicDB")
	    local para = {["uPlayerID"] = uCharId}
	    local tongLevel, tongPlaceLevel  = CTongBasicBox.GetTongLevelByPlayerID(para)
	    local suc, needTongLevel = CheckNpcShopItemTongLevel(nType, sName, tongLevel, tongPlaceLevel)
	    if suc == false then
	        return false, 6118, needTongLevel
	    end
	end
	local all_cost = 0 
	local itemName
	local soulCount = 0
	local pearlPrice = nil
	if g_ItemInfoMgr:IsSoulPearl(nType) then
    	itemName, soulCount= g_ItemInfoMgr:GetSoulpearlInfo(sName)
        pearlPrice = GetSoulpearlPriceByTypeAndCount(itemName,soulCount, pay_type)
		all_cost = math.ceil(nCount * pearlPrice)
	else
	    all_cost = math.ceil(nCount * g_ItemInfoMgr:GetItemInfo(nType, sName,"Price"))
	end
	
	local liuTongShop = tonumber(NpcShop_Common(npcName,"LiuTongShop"))
	
	local g_LogMgr = RequireDbBox("LogMgrDB")
	local uEventId = g_LogMgr.LogPlayerTakerFromNpc(uCharId, npcName,nil,event_type_tbl["npc商店购买"] )
	local curMoneyType = payMoneyType 
	local bSuc, modifiedJiFenCount
	local itemBindingType = g_ItemInfoMgr:GetItemInfo(nType, sName,"BindingStyle") or 0
	if pay_type ~= 0 then
		--bSuc, modifiedJiFenCount,uMsgID= BuyItemByPoint(sName,pay_type,nCount,uCharId,1, nType, pearlPrice,sceneName,uEventId)
        local suc, result = NpcShopDbBox.SubtractJiFenByBuyingItem(curMoneyType, sName,pay_type,nCount,uCharId,nType, pearlPrice,sceneName,uEventId, liuTongShop)
        if suc ~= true then
            return false, result 
        end
		itemBindingType = 2
		modifiedJiFenCount = result["JiFenCount"]
		all_cost = result["Money"]
	else
        local suc, msgID = NpcShopDbBox.SubtractMoneyByBuyingItem(curMoneyType, uCharId, all_cost, liuTongShop,uEventId,nType,sName,nCount)
        if suc ~= true then
            return false, msgID 
        end
	end

	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")	
	local nHaveCount = g_RoomMgr.GetCountByPosition(uCharId, nRoomIndex, nPos)
	local FoldLimit = g_ItemInfoMgr:GetItemInfo(nType, sName,"FoldLimit") or 1
	if pay_type == 0 and curMoneyType == 2 then
		itemBindingType = 2
	end
	local res = {}
	local res_add = {}
	if nHaveCount + nCount > FoldLimit then
		local params= {}
		params.nCharID = uCharId
		params.nStoreMain = g_StoreRoomIndex.PlayerBag
		params.nBigID = nType
		params.nIndex = sName
		params.nCount = nCount + nHaveCount - FoldLimit
		params.BindingType = itemBindingType
		params.uEventId = uEventId
		if FoldLimit <= nHaveCount  then
		    CancelTran()
		    return false,6115
		end
		res = g_RoomMgr.AddItemByPos(uCharId, nType, sName, nRoomIndex, nPos, FoldLimit - nHaveCount, itemBindingType,nil,uEventId)
		res_add = g_RoomMgr.AddItem(params)
	else
		res = g_RoomMgr.AddItemByPos(uCharId, nType, sName, nRoomIndex, nPos, nCount, itemBindingType,nil,uEventId)
	end
	if IsNumber(res) then
		CancelTran()
		local errorMsgId = res
		return false, errorMsgId
	end
	
	if IsNumber(res_add) then
		CancelTran()
		local errorMsgId = res_add
		return false, errorMsgId
	end
	local DirectDB = RequireDbBox("DirectDB")
	local DirectTbl = DirectDB.AddPlayerItemDirect(uCharId, "商店购买物品", nType, sName)
	local result = {["MoneyType"] = curMoneyType, ["MoneyDelta"]=-all_cost,
					["JiFenType"] = pay_type, ["ModifiedJiFenCount"] = modifiedJiFenCount, ["DirectTbl"] = DirectTbl}
	return true, result
end

--右键点击购买物品
function NpcShopDbBox.BuyItem(data)
	local uCharId 		=	data["CharID"]
	local nType			=	data["ItemType"]
	local sName			=	data["ItemName"]
	local nCount		=	data["ItemCount"]
	local npcName		=	data["NpcName"]
	local payMoneyType  =   data["PayMoneyType"]
	local sceneName		= 	data["sceneName"]
	local all_cost = 0
	local MoneyManagerDB=	RequireDbBox("MoneyMgrDB")
	local pay_type = 0
	
	local ItemBagLockDB = RequireDbBox("ItemBagLockDB")
  if ItemBagLockDB.HaveItemBagLock(uCharId) then
  	local result = {}
		result["MsgID"]= 160018
  	return false,result
  end
  
	local npcshop_info = NpcShop_Common(npcName)
	if npcshop_info then
		pay_type = NpcShop_Common(npcName, "PayType")
	end
    local liuTongShop = tonumber(NpcShop_Common(npcName,"LiuTongShop"))
	if pay_type == TongNpcShop then
	    --团等级  
	    local CTongBasicBox  = RequireDbBox("GasTongBasicDB")
	    local para = {["uPlayerID"] = uCharId}
	    local tongLevel, tongPlaceLevel  = CTongBasicBox.GetTongLevelByPlayerID(para)
	    local suc, needTongLevel = CheckNpcShopItemTongLevel(nType, sName, tongLevel, tongPlaceLevel)
	    if suc == false then
	        local result = {}
	        result["MsgID"] = 6118
	        result["ErrorMsg"] = needTongLevel
	        return false, result 
	    end
	end

	local pearlPrice = nil
	if g_ItemInfoMgr:IsSoulPearl(nType) then
		local itemName, soulCount = g_ItemInfoMgr:GetSoulpearlInfo(sName)
        pearlPrice = GetSoulpearlPriceByTypeAndCount(itemName,soulCount, pay_type)
		all_cost = math.ceil(nCount * pearlPrice)
    else
        all_cost = math.ceil(nCount * g_ItemInfoMgr:GetItemInfo(nType, sName,"Price"))
	end
	local g_LogMgr = RequireDbBox("LogMgrDB")
	local uEventId = g_LogMgr.LogPlayerTakerFromNpc(uCharId, npcName,nil,event_type_tbl["npc商店购买"] )
	local curMoneyType = payMoneyType
	local bSuc, modifiedJiFenCount
    local nBindingType = g_ItemInfoMgr:GetItemInfo(nType, sName,"BindingStyle") or 0
	if pay_type ~= 0 then
		--bSuc, modifiedJiFenCount = BuyItemByPoint(sName,pay_type,nCount,uCharId,1,nType, pearlPrice,sceneName,uEventId)
        local suc, result = NpcShopDbBox.SubtractJiFenByBuyingItem(curMoneyType, sName,pay_type,nCount,uCharId, nType, pearlPrice,sceneName,uEventId, liuTongShop)
        if suc ~= true then
            local ret = {}
		    ret["MsgID"] = result
			return false, ret
        end
		nBindingType = 2
        modifiedJiFenCount = result["JiFenCount"]
		all_cost = result["Money"]
	else
        local suc, msgID = NpcShopDbBox.SubtractMoneyByBuyingItem(curMoneyType, uCharId, all_cost, liuTongShop,uEventId,nType,sName,nCount)
        if suc ~= true then
            local result = {}
		    result["MsgID"] = msgID
			return false, result
        end
    end

	if pay_type == 0 and curMoneyType == 2 then --用非积分方式购买物品
	    nBindingType = 2
	end
	
	local params= {}
	params.nCharID = uCharId
	params.nStoreMain = g_StoreRoomIndex.PlayerBag
	params.nBigID = nType
	params.nIndex = sName
	params.nCount = nCount
	params.BindingType = nBindingType
	params.sceneName = sceneName
	params.uEventId = uEventId
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")	
	local res = g_RoomMgr.AddItem(params)
	
	if IsNumber(res) then
		CancelTran()
		local result = {}
		result["MsgID"]= res
		return false, result
	end
	local DirectDB = RequireDbBox("DirectDB")
	local DirectTbl = DirectDB.AddPlayerItemDirect(uCharId, "商店购买物品", nType, sName)
	local result = {
					 ["MoneyType"] = curMoneyType, ["MoneyDelta"] =-all_cost,
					 ["payType"] = pay_type, ["ModifiedJiFenCount"] = modifiedJiFenCount, 
					 ["ItemType"] = nType, ["ItemName"] = sName, ["PayMoneyType"] = payMoneyType, ["DirectTbl"] = DirectTbl}
	return true, result
end
-------------------------------------------------------------------------------------
local StmtDef = {
	"_DelSellItemInfo",
	[[
		delete from tbl_npcshop_player_sell_item where  cs_uId = ? and  npsi_nId = ?
	]]
}
DefineSql(StmtDef, NpcShop)

local StmtDef = {
	"_SelectSoldItemInfo",
	[[
		select npsi_nShopType,npsi_sItemType,npsi_sItemName,npsi_nItemCount,npsi_nBindingType from tbl_npcshop_player_sell_item where cs_uId = ? and npsi_nId = ?
	]]
}
DefineSql(StmtDef, NpcShop)

local StmtDef = {
	"_QuerySoldItemIdByInfo",
	[[ 
		select 
		psii.psii_nItemId
		from 
		tbl_npcshop_player_sell_item npsi join tbl_player_sell_item_id psii on npsi.npsi_nId = psii.psii_nId  where cs_uId = ? and npsi.npsi_nId = ?
	]]
}
DefineSql( StmtDef , NpcShop )

--购回选中物品
function NpcShopDbBox.BuyBackItem(data)
	local uCharId			=	data["CharID"]
	local npcName			=	data["NpcName"]
	local nIndex			= data["nIndex"]
	local sceneName = data["sceneName"]
	local isBuyBackItem = data["isBuyBackItem"]
	local all_cost = 0
	local result = {}
	local ItemData = {}
	local tbl_item_info = {}
	local pay_type = 0
	
	local ItemBagLockDB = RequireDbBox("ItemBagLockDB")
  if ItemBagLockDB.HaveItemBagLock(uCharId) then
  	result["MsgID"] = 160018
  	return false,result
  end
	
	local npcshop_info = NpcShop_Common(npcName)
	if npcshop_info then
		pay_type =  NpcShop_Common(npcName, "PayType")
	end
	
	local sold_item_info = NpcShop._SelectSoldItemInfo:ExecStat(uCharId,nIndex)
	if sold_item_info:GetRowNum() == 0 then
		result["MsgID"] = 6106
        result["ShopType"]  = pay_type
        return  false, result
	end
	
	local itemBindType = sold_item_info:GetData(0,4)
	local db_paytype = sold_item_info:GetData(0,0)
	if db_paytype ~= pay_type then
        result["MsgID"] = 6106
        result["ShopType"] = pay_type
        return  false, result
	end
	
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local moneyType = 2
	
	local item_res = NpcShop._QuerySoldItemIdByInfo:ExecStat(uCharId,nIndex)
	if item_res:GetRowNum() == 0 then
        result["MsgID"] = 6106
        result["ShopType"]  = pay_type
        return  false, result
	end
	
	for i = 0, item_res:GetRowNum()-1 do 
		table.insert(tbl_item_info, {item_res:GetData(i,0)})
		table.insert(ItemData,item_res:GetData(i,0))
	end
	
	local pearlPrice = nil
	local nType,sName,nCount = sold_item_info:GetData(0,1),sold_item_info:GetData(0,2),sold_item_info:GetData(0,3)
	if g_ItemInfoMgr:IsSoulPearl(nType) then
		local soulCount = g_RoomMgr.GetSoulPearlInfoByID(item_res:GetData(0,0))
        pearlPrice = GetSoulpearlPriceByTypeAndCount(sName,soulCount, pay_type)
		all_cost = math.ceil(pearlPrice* 1/4) * nCount
	else
	  all_cost = math.ceil(g_ItemInfoMgr:GetItemInfo(nType, sName,"Price") * 1/4) * nCount
	end
	
	local g_LogMgr = RequireDbBox("LogMgrDB") 
	local uEventId = g_LogMgr.LogPlayerTakerFromNpc(uCharId, npcName,ItemData,event_type_tbl["npc商店购回"] )
	local bSuc, modifiedJiFenCount
	if pay_type ~= 0 then
		bSuc, modifiedJiFenCount = BuyItemByPoint(sName,pay_type,nCount,uCharId,1/4, nType, pearlPrice,sceneName,uEventId)
		if not bSuc  then
		    result["MsgID"] = 6112
		    result["ShopType"]  = pay_type
			return  false, result
		end
	else
		local MoneyManagerDB=	RequireDbBox("MoneyMgrDB")
		if CheckMoneyEnough(uCharId, all_cost, moneyType)  == false then
		    result["MsgID"] = 6113
		    result["ShopType"]  = pay_type
			return false, result
		end
		local fun_info = g_MoneyMgr:GetFuncInfo("NpcShop")
		local bFlag,uMsgID = MoneyManagerDB.AddMoneyByType(fun_info["FunName"],fun_info["BuyBack"],uCharId, moneyType, - all_cost,uEventId )
		if not bFlag then
			CancelTran()
			if IsNumber(uMsgID) then
				return false,{["MsgID"] = uMsgID,["ShopType"]  = pay_type}
			else
				return false,{["MsgID"] = 6113,["ShopType"]  = pay_type}
			end
		end
	end
	local params = {}
	params.m_nCharID = uCharId
	params.m_nItemType = nType
	params.m_sItemName = sName
	params.m_tblItemID = tbl_item_info
	params.m_nBindType = itemBindType
	local item_bag_box = RequireDbBox("CPutItemToBagDB")
	local bFlag, ret_info = item_bag_box.AddCountItem(params)
	if bFlag == false then
		CancelTran()
		result["ShopType"]  = pay_type
		if IsNumber(ret_info) then
			result["MsgID"] =  ret_info
		else
			result["MsgID"] =  6122
		end
		return false, result
	end
	NpcShop._DelSellItemInfo:ExecStat(uCharId,nIndex)
	if g_DbChannelMgr:LastAffectedRowNum() == 0 then
		CancelTran()
		result["MsgID"] = 6122
		LogErr("购回失败","玩家id：" .. uCharId .. ",索引：" .. nIndex)
		return false,result
	end
	
	local result = {["ret_info"]=ret_info, ["MoneyType"]=moneyType, ["MoneyDelta"]= -all_cost,
									["payType"] = pay_type, ["ModifiedJiFenCount"] = modifiedJiFenCount}
	if not isBuyBackItem then
		local tmp_data = {["uCharId"] = uCharId,["NpcName"] = npcName}
		result["item_list_tbl"] = NpcShopDbBox.GetSoldGoodsList(tmp_data)
	end								
	return true, result
end
-----------------------------------------------------------------
local StmtDef = {
	"_SelectSoldItemList",
	[[
		select npsi_nId,npsi_sItemType,npsi_sItemName,npsi_nItemCount,npsi_nBindingType from tbl_npcshop_player_sell_item where cs_uId = ? and npsi_nShopType  = ? order by npsi_nId desc 
	]]
}
DefineSql(StmtDef, NpcShop)

local StmtDef = {
	"_SelectSoldItemId",
	[[
		select psii_nItemId from tbl_player_sell_item_id where psii_nId  = ?
	]]
}
DefineSql(StmtDef, NpcShop)

--@brief 获取所有出售的物品列表
function NpcShopDbBox.GetSoldGoodsList(data)
	local uCharId = data["uCharId"] 
	local sNpcName = data["NpcName"]
	local sPayType = NpcShop_Common(sNpcName, "PayType")
	local itemDynInfoDB = RequireDbBox("GetItemDynInfoDB")	
	if not sPayType then
		return 
	end
	local item_list_tbl = {}
	local item_list = NpcShop._SelectSoldItemList:ExecStat(uCharId,sPayType)
	if item_list:GetRowNum() > 0 then
		local idTbl = {}
		for i = 1,item_list:GetRowNum() do
			local uIndex = item_list(i,1)
			local item_set = NpcShop._SelectSoldItemId:ExecStat(uIndex)
			if item_set:GetRowNum() > 0 then
				if idTbl[uIndex] == nil then
					idTbl[uIndex] = {}
				end
				for i = 1,item_set:GetRowNum() do
					table.insert(idTbl[uIndex],item_set(i,1))
				end
			end
			local itemType = item_list(i, 2)
			local count = item_list(i, 4)    
			local itemID = idTbl[uIndex][1]
			local itemName = item_list(i, 3)
		  local itemBindingType = item_list(i, 5)
		  local itemIDInfo = idTbl[uIndex]
		  
	    local itemInfoTbl = itemDynInfoDB.GetItemDynInfo(itemID, itemType, uCharId, uCharId, idTbl[uIndex])
		  local itemInfo = {itemType, itemName, count, itemIDInfo,itemInfoTbl, itemBindingType,uIndex}
			table.insert(item_list_tbl,itemInfo)
		end
	end
	return item_list_tbl
end
--------------------------------------------------------------
local StmtDef = {
	"_SelectSoldItemIndex",
	[[
		select npsi_nId from tbl_npcshop_player_sell_item where cs_uId = ? and npsi_nShopType = ?
	]]
}
DefineSql(StmtDef, NpcShop)

--购回所有已出售的物品
function NpcShopDbBox.BuyBackAllItem(data)
	local uCharId			=	data["CharID"]
	local npcName			=	data["NpcName"]
	local sceneName = data["sceneName"]
	local result			=	{}

	local ItemBagLockDB = RequireDbBox("ItemBagLockDB")
  if ItemBagLockDB.HaveItemBagLock(uCharId) then
  	result["MsgID"] = 160018
  	return false,result
  end
  
	result["ShopType"] = NpcShop_Common(npcName, "PayType")
	local item_index_res = NpcShop._SelectSoldItemIndex:ExecStat(uCharId,result["ShopType"])
	if item_index_res:GetRowNum() == 0 then 
		result["MsgID"] = 6106
		return false,result
	end
	for i = 1,item_index_res:GetRowNum() do
		local nIndex			= item_index_res(i,1)
		data["nIndex"] = nIndex
		local ret, res = NpcShopDbBox.BuyBackItem(data)	
		if ret then
			table.insert(result, res)
		else
			CancelTran()
			result["MsgID"] = res["MsgID"]
			if res["ErrorMsg"] ~= nil then
			    result["ErrorMsg"] = res["ErrorMsg"]
			end
			return false, result
		end
	end
	return true, result
end
------------------------------------------------------------------
local StmtDef = {
	"_DelSoldListItem",
	[[ 
		delete from tbl_npcshop_player_sell_item where cs_uId = ?
	]]
}
DefineSql( StmtDef , NpcShop )

local StmtDef = {
	"_QuerySoldItemId",
	[[ 
		select 
		psii.psii_nItemId
		from 
		tbl_npcshop_player_sell_item npsi join tbl_player_sell_item_id psii on npsi.npsi_nId = psii.psii_nId  where cs_uId = ?
	]]
}
DefineSql( StmtDef , NpcShop )

--@brief 下线删除商店购回列表的物品
function NpcShopDbBox.LogoutDelSoldItemList(data)
	local char_id	= data["char_id"]
	local SceneName = data["scene_name"]
	local lRoomMgr = RequireDbBox("GasRoomMgrDB")
	local item_res = NpcShop._QuerySoldItemId:ExecStat(char_id)
	if item_res:GetRowNum() > 0 then
		for i = 0,item_res:GetRowNum() - 1 do
			local succ = lRoomMgr.DelItemFromStaticTable(item_res:GetData(i,0),char_id,event_type_tbl["npc商店出售"])
			if not succ then
				CancelTran()
				return 
			end
		end
		NpcShop._DelSoldListItem:ExecStat(char_id)
		if g_DbChannelMgr:LastAffectedRowNum() == 0 then
			CancelTran()
			return 
		end
	end
end

function NpcShopDbBox.UpdateChoosedMoneyPayType(data)
    local charId        =   data["CharID"] 
    local moneyPayType  =   data["MoneyPayType"]
    
    NpcShop._UpdateCurMoneyType:ExecStat(moneyPayType, charId)
    if g_DbChannelMgr:LastAffectedRowNum() > 0 then
        return true, moneyPayType
    else
        return false
    end
end

local StmtDef = {
	"_GetShopTipFlagInfo",
	[[ 
		select 
		    ifnull(nbt_LiuTongShopBuyTip, 1), ifnull(nbt_CommonShopBuyTip,1 )
		from 
		    tbl_npcshop_buy_tip 
		where 
		    cs_uId = ?
	]]
}
DefineSql( StmtDef , NpcShop )
function NpcShopDbBox.GetShopTipFlagInfo(charID)
    local ret = NpcShop._GetShopTipFlagInfo:ExecStat(charID)
    local liuTongShopTipFlag = 1
    local commonShopTipFlag  = 1
    if ret:GetRowNum() > 0 then
        liuTongShopTipFlag = ret:GetData(0, 0)
        commonShopTipFlag = ret:GetData(0, 1)
    end
    local tbl = {["LiuTongShopTipFlag"] = liuTongShopTipFlag, 
    ["CommonShopTipFlag"] = commonShopTipFlag}
    return tbl
end

local StmtDef ={
    "_UpdateLiuTongShopFlag",
    [[
        update tbl_npcshop_buy_tip set nbt_LiuTongShopBuyTip = ? where cs_uId = ?
    ]]
}
DefineSql(StmtDef , NpcShop)

local StmtDef ={
    "_UpdateCommonShopFlag",
    [[
        update tbl_npcshop_buy_tip set nbt_CommonShopBuyTip = ? where cs_uId = ?
    ]]
}
DefineSql(StmtDef , NpcShop)

local StmtDef = {
    "_GetShopTipFlag",
    [[
        select count(*) from tbl_npcshop_buy_tip where cs_uId = ?
    ]]
}
DefineSql(StmtDef , NpcShop)

local StmtDef = {
    "_InsertShopTipFlag",
    [[
        insert into tbl_npcshop_buy_tip set cs_uId = ?
    ]]
}
DefineSql(StmtDef , NpcShop)

function NpcShopDbBox.UpdateShopTipFlag(data)
    local charID    =   data["CharID"]
    local shopType = data["ShopType"]
    local tipFlag = data["TipFlag"]
    local ret = NpcShop._GetShopTipFlag:ExecStat(charID)
    if ret:GetData(0, 0) < 1 then
        NpcShop._InsertShopTipFlag:ExecStat(charID)
    end
    
    if shopType == 1 then --流通商店
        NpcShop._UpdateLiuTongShopFlag:ExecStat(tipFlag, charID)
    else
        NpcShop._UpdateCommonShopFlag:ExecStat(tipFlag, charID)      
    end
    if g_DbChannelMgr:LastAffectedRowNum() == 0 then
        return false
    end
    
    local tbl = NpcShopDbBox.GetShopTipFlagInfo(charID)
    return true, tbl
end

local StmtDef = {
    "_SetDefaultShopTipFlag",
    [[
        update tbl_npcshop_buy_tip set nbt_LiuTongShopBuyTip = 1, nbt_CommonShopBuyTip = 1 where cs_uId = ?
    ]]
}
DefineSql(StmtDef , NpcShop)

function NpcShopDbBox.SetDefaultShopTipFlag(data)
    local charID    =   data["CharID"]
    local ret = NpcShop._GetShopTipFlag:ExecStat(charID)
    if ret:GetData(0, 0) > 0 then
        NpcShop._SetDefaultShopTipFlag:ExecStat(charID)
        if g_DbChannelMgr:LastAffectedRowNum() > 0 then
            return true
        end
    end
    return false
end

SetDbLocalFuncType(NpcShopDbBox.BuyItem)
SetDbLocalFuncType(NpcShopDbBox.BuyItemByPos)
SetDbLocalFuncType(NpcShopDbBox.BuyBackItem)
SetDbLocalFuncType(NpcShopDbBox.BuyBackAllItem)
SetDbLocalFuncType(NpcShopDbBox.PlayerSellItemByPos)
SetDbLocalFuncType(NpcShopDbBox.GetSoldGoodsList)
SetDbLocalFuncType(NpcShopDbBox.UpdateChoosedMoneyPayType)
SetDbLocalFuncType(NpcShopDbBox.GetShopTipFlagInfo)
SetDbLocalFuncType(NpcShopDbBox.UpdateShopTipFlag)
SetDbLocalFuncType(NpcShopDbBox.SetDefaultShopTipFlag)
return NpcShopDbBox 
