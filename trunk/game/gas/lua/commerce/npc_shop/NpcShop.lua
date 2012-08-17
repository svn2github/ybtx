gas_require "commerce/npc_shop/NpcShopInc"
cfg_load "npc/NpcShop_Common"
gac_gas_require "item/SoulpearlCommonFunc"
gac_gas_require "commerce/npc_shop/NPCShopMgr"
local NpcShopDB = "NpcShopDB"

local function GetDistanceFromNpcValidate(Conn, npcName)
    if NpcShop_Common(npcName, "ShopType") == 0 then
	    if (not DistanceJudgeBetweenNpcAndPlayer(Conn,"商店")) then
		    return false
	    else
    	    return true
	    end
	end
	return ture
end

-- 回购列表的大小
--NPC SHOP
--Ctor
function CNpcShop:Ctor(sNpcName)
	self.m_sNpcName = sNpcName
	self:InitBuyList()
end
--Init BuyList
function CNpcShop:InitBuyList()
	--获取配置表信息
	self.m_tblBuyList = {}
	self.m_sShopName =  NpcShop_Common(self.m_sNpcName,"ShopName")
	for i = 1, 120 do
		local itemtype = "ItemType" .. i
		local itemname = "ItemName" .. i
		local type = tonumber(NpcShop_Common(self.m_sNpcName, itemtype))
		local name = NpcShop_Common(self.m_sNpcName, itemname)
		if(type == "" or type == nil) then
			return
		end
		self.m_tblBuyList[type] = self.m_tblBuyList[type] or {}
		self.m_tblBuyList[type][name] = 1		--标识某个该商店出售的物品为1，用于判断某个物品是否是该商店出售的
	end
end

--judge if this Npc sold this item
function CNpcShop:IsItemSold(sType, sName)
	local nBigId = tonumber(sType)
	if self.m_tblBuyList[nBigId] == nil then
	   return false 
	end
	if self.m_tblBuyList[nBigId][sName] == nil then
	   return false 
	end
	return self.m_tblBuyList[nBigId][sName] == 1
end

--buy item no pos
function CNpcShop:BuyItemCheck(Conn, nType, sName, nCount)
	if not (self:IsItemSold(nType, sName)) then
		MsgToConn(Conn, 6107)
		return false
	end
	local pay_type = 0
	local npcshop_info = NpcShop_Common(self.m_sNpcName)
	
	if npcshop_info then
		pay_type = NpcShop_Common(self.m_sNpcName,"PayType")
	end
	local all_cost 
	if g_ItemInfoMgr:IsSoulPearl(nType) then
		sName, soulCount =  g_ItemInfoMgr:GetSoulpearlInfo(sName)
		local price = GetSoulpearlPriceByTypeAndCount(sName,soulCount, pay_type)
		all_cost = math.ceil(nCount * price)
    else        
        all_cost = math.ceil(nCount * GetItemPriceByPayType(nType, sName, pay_type))
        --all_cost = math.ceil(nCount * GetItemPrice(nType, sName, pay_type))
	end
	
		
	if nCount <= 0 or all_cost <= 0 then
		MsgToConn(Conn, 6107)
		return false
	end
end

local function sold_goods_list(Conn, shopType,item_list_tbl)
    if not IsCppBound(Conn.m_Player) then
         return 
    end
	Gas2Gac:ReturnPlayerSoldGoodsListBegin(Conn)	
	if #item_list_tbl > 0 then
		for i = 1,#item_list_tbl do
			local tbl = item_list_tbl[i] 
			local nBigID,nIndex, nItemID, itemInfoTbl, bindingType,uPosIndex = tbl[1], tbl[2], tbl[4][1], tbl[5][1], tbl[6],tbl[7]
			Gas2Gac:ReturnPlayerSoldGoodsList(Conn, nBigID,nIndex, tbl[3], i, nItemID,shopType,uPosIndex)--物品type、name、count、显示位置、id
		end
	end
	Gas2Gac:ReturnPlayerSoldGoodsListEnd(Conn)
end 
----------------------- NPC Shop Manager -------------------
function CNpcShopMgr:g_GetNpcShop(npc_name)
	assert(IsString(npc_name))
	g_NpcShop = g_NpcShop or {}
	g_NpcShop[npc_name] = g_NpcShop[npc_name] or CNpcShop:new(npc_name)
	return g_NpcShop[npc_name]
end
--------------------------RPC Function----------------------
--购买物品，右键点击方式
local function BuyItem(Conn, nBigID, nIndex, result, charID)
	local ItemInfo		= result["ItemIDTbl"]		
	local moneyDelta		= result["MoneyDelta"]
	local payMoneyType		= result["MoneyType"]	
	local bindingType		= result["BindingType"]
	local payType = result["payType"] 
	local modifiedJiFenCount = result["ModifiedJiFenCount"]
	local DirectTbl = result["DirectTbl"]
	if payType ~= 0 then
		Gas2GacById:UpdatePlayerPoint(charID,payType,modifiedJiFenCount)
		if payType == 7 then
			Conn.m_Player:UpdatePlayerPkValue(modifiedJiFenCount)
		end
	end
	
	if not IsCppBound(Conn.m_Player) then
        return 
	end	
	local uCharId = Conn.m_Player.m_uID
	Gas2Gac:RetNpcTradeSucc(Conn)
	if DirectTbl then
		SendPlayerFinishDirect(Conn.m_Player, DirectTbl)
	end
end

function  Gac2Gas:BuyItem(Conn, sNpcName, sType, sName, nCount, payMoneyType)
	if not IsCppBound(Conn.m_Player) then
		return 
	end
	
	if Conn.m_Player.m_ItemBagLock then
		MsgToConn(Conn,160018)
		return 
	end			
	local npc_shop = g_NpcShopMgr:g_GetNpcShop(sNpcName)
	local nType = tonumber(sType)
	local ret = npc_shop:BuyItemCheck(Conn, nType, sName, nCount)
	if ret == false then
		return
	end
	
	local charID = Conn.m_Player.m_uID
	local function CallBack(suc, result, errorMsg)
		if suc then
		    local nType = result["ItemType"]
		    local sName = result["ItemName"]
			if g_ItemInfoMgr:IsSoulPearl(nType) then
				sName = g_ItemInfoMgr:GetSoulpearlInfo(sName)
   		end
			BuyItem(Conn, nType, sName, result, charID)
			local moneyType = result["PayMoneyType"]
		    if moneyType ~= 0 then
    	        Gas2Gac:SetMoneyTypeBtnCheck(Conn, moneyType)
	        end
		else
		    local errorMsg = result["ErrorMsg"]
		    local errMsgID = result["MsgID"]
		    if errorMsg ~= nil then
			    MsgToConn(Conn, errMsgID, errorMsg)
			else
			    MsgToConn(Conn, errMsgID)
			end
		end
	end	

	local data = { ["CharID"] =charID, ["ItemType"]= nType, ["ItemName"]= sName,
					["ItemCount"]=nCount, ["NpcName"]= sNpcName, ["PayMoneyType"]= payMoneyType,["sceneName"] = Conn.m_Player.m_Scene.m_SceneName }	
	--local entry = (g_DBTransDef["NpcShopDB"])
	CallAccountManualTrans(Conn.m_Account, NpcShopDB, "BuyItem", CallBack, data)
end

--左键拖拉购买物品
local function  BuyItemByPos(Conn, uCharId, nRoomIndex, nPos, nBigID, sName, result)
	local moneyDelta		= result["MoneyDelta"]
	local payMoneyType		= result["MoneyType"]
	local jiFenType         = result["JiFenType"]
	local modifiedJiFenCount    = result["ModifiedJiFenCount"]	
	local DirectTbl = result["DirectTbl"]
    if jiFenType ~= 0 then
	    Gas2GacById:UpdatePlayerPoint(uCharId,jiFenType,modifiedJiFenCount)
			if jiFenType == 7 then
				Conn.m_Player:UpdatePlayerPkValue(modifiedJiFenCount)
			end
	end
	Gas2Gac:RetNpcTradeSucc(Conn)
	if DirectTbl then
		SendPlayerFinishDirect(Conn.m_Player, DirectTbl)
	end
end

--购买物品，左键拖拉方式
function Gac2Gas:BuyItemByPos(Conn, sNpcName, sType, sName, nCount, nRoomIndex, nPos, payMoneyType)
	if GetDistanceFromNpcValidate(Conn,sNpcName) == false then
		return
	end
	if Conn.m_Player.m_ItemBagLock then
		MsgToConn(Conn,160018)
		return 
	end
	local nBigID = tonumber(sType)
	local npc_shop = g_NpcShopMgr:g_GetNpcShop(sNpcName)
	local ret = npc_shop:BuyItemCheck(Conn, nBigID, sName, nCount)
	if ret == false then
		return 
	end
	local uCharId = Conn.m_Player.m_uID
	local data = {["CharID"]= Conn.m_Player.m_uID, ["ItemType"]= nBigID, ["ItemName"]=sName,
					["ItemCount"]= nCount, ["RoomIndex"]=nRoomIndex, ["Pos"]=nPos, ["NpcName"]=sNpcName,
					["PayMoneyType"] = payMoneyType,["sceneName"] = Conn.m_Player.m_Scene.m_SceneName}
	
	local function CallBack(suc, result, errorMsg)
		if suc then
			if g_ItemInfoMgr:IsSoulPearl(nBigID) then
				sName = g_ItemInfoMgr:GetSoulpearlInfo(sName)
			end
			BuyItemByPos(Conn, uCharId, nRoomIndex, nPos, nBigID, sName, result)
		    if payMoneyType ~= 0 then
    	        Gas2Gac:SetMoneyTypeBtnCheck(Conn, payMoneyType)
	        end
		else
		    if errorMsg ~= nil then
			    MsgToConn(Conn, result, errorMsg)
			else
			    MsgToConn(Conn, result)
			end
		end
	end	
	--local entry = (g_DBTransDef["NpcShopDB"])
	CallAccountManualTrans(Conn.m_Account, NpcShopDB, "BuyItemByPos", CallBack, data)
end

--购回物品
local function BuyBackItem(Conn, result, len, nIndex, charID,item_list_tbl)
	local ret_info   = result["ret_info"]
	local moneyDelta = result["MoneyDelta"]
	local moneyType  = result["MoneyType"]
	local pay_type = result["payType"] 
	local modifiedJiFenCount = result["ModifiedJiFenCount"]
	local player = Conn.m_Player
	
	if pay_type ~= 0 then
		Gas2GacById:UpdatePlayerPoint(charID,pay_type,modifiedJiFenCount)
		if pay_type == 7 then
			Conn.m_Player:UpdatePlayerPkValue(modifiedJiFenCount)
		end
	end
	Gas2Gac:RetNpcTradeSucc(Conn)
	sold_goods_list(Conn, pay_type,item_list_tbl)
end

--购回选中物品
function Gac2Gas:BuyBackItem(Conn, sNpcName, nIndex)
	if GetDistanceFromNpcValidate(Conn,sNpcName) == false then
		return
	end
	if Conn.m_Player.m_ItemBagLock then
		MsgToConn(Conn,160018)
		return 
	end
  local charID = Conn.m_Player.m_uID
	local function CallBack(suc, result)
		if suc then
			BuyBackItem(Conn, result, len, nIndex, charID,result["item_list_tbl"])
		else
	    local errorMsg = result["ErrorMsg"]
	    local errMsgID = result["MsgID"]
	    if errorMsg ~= nil then
			  MsgToConn(Conn, errMsgID, errorMsg)
			else
				MsgToConn(Conn, errMsgID)
			end
		end
	end
	
	local data = {["CharID"]=charID , ["NpcName"]=sNpcName, ["nIndex"]= nIndex
					 ,["sceneName"] = Conn.m_Player.m_Scene.m_SceneName}
	--local entry = (g_DBTransDef["NpcShopDB"])
	CallAccountManualTrans(Conn.m_Account, NpcShopDB, "BuyBackItem", CallBack, data)
end

--购回所有已售出的物品
local function BuyBackAllItem(Conn, result, charID)
	local moneyDelta = result["MoneyDelta"]
	local ret_info   = result["ret_info"]
	local moneyType  = result["MoneyType"]
	local pay_type = result["payType"] 
	local modifiedJiFenCount = result["ModifiedJiFenCount"]
	
	local player = Conn.m_Player
	
	if pay_type ~= 0 then
		Gas2GacById:UpdatePlayerPoint(charID,pay_type,modifiedJiFenCount)
		if pay_type == 7 then
			player:UpdatePlayerPkValue(modifiedJiFenCount)
		end
	end
end

function Gac2Gas:BuyBackAllItem(Conn, sNpcName)
	if GetDistanceFromNpcValidate(Conn,sNpcName) == false then
		return
	end
	if Conn.m_Player.m_ItemBagLock then
		MsgToConn(Conn,160018)
		return 
	end
	local charID= Conn.m_Player.m_uID
	local function CallBack(suc, result)
		if suc then
			for i=1, # result do	
				if BuyBackAllItem(Conn, result[i], charID) == false then
					return 
				end
			end
			Gas2Gac:RetNpcTradeSucc(Conn)
			sold_goods_list(Conn, result["ShopType"],{})
		else
	    local errorMsg = result["ErrorMsg"]
	    local errMsgID = result["MsgID"]
	    if errorMsg ~= nil then
		    MsgToConn(Conn, errMsgID, errorMsg)
			else
			  MsgToConn(Conn, errMsgID)
			end
		end
	end
	local data = {["CharID"]= charID, ["NpcName"]=sNpcName,
								["sceneName"] = Conn.m_Player.m_Scene.m_SceneName,["isBuyBackItem"] = true }
								
	--local entry = (g_DBTransDef["NpcShopDB"])
	CallAccountManualTrans(Conn.m_Account, NpcShopDB, "BuyBackAllItem", CallBack, data)
end

local function SellItemByPos(Conn, suc, result, nRoomIndex, nPos, nCount, charID)
	local tbl_sold_item 	=	result["ItemIDTbl"]
	local nType				=	result["ItemType"]
	local sName				=	result["ItemName"]
	local moneyDelta		=	result["MoneyDelta"]
	local moneyType			=	result["MoneyType"]
	local itemInfoTbl = result["ItemInfoTbl"]
	local bindingType	=	result["ItemBindingType"]
	local pay_type = result["pay_type"]
	local sellItemGetPoint = result["SellItemGetPoint"]
	local DirectTbl = result["DirectTbl"]
	local player			=	Conn.m_Player
	if not IsCppBound(Conn.m_Player) then
	    return
	end
	if pay_type ~= 0 then
		Gas2GacById:UpdatePlayerPoint(charID,pay_type,sellItemGetPoint)
		if pay_type == 7 then
			player:UpdatePlayerPkValue(sellItemGetPoint)
		end
	end
	Gas2Gac:RetNpcTradeSucc(Conn)
	if DirectTbl then
		SendPlayerFinishDirect(player, DirectTbl)
	end
end

function Gac2Gas:PlayerSellItemByPos(Conn, sNpcName, nRoomIndex, nPos, nCount)
	if GetDistanceFromNpcValidate(Conn,sNpcName) == false then
		return
	end
	if not IsCppBound(Conn.m_Player) then
		return 
	end
	if Conn.m_Player.m_ItemBagLock then
		MsgToConn(Conn,160019)
		return 
	end
	local charID =  Conn.m_Player.m_uID
	local function CallBack(suc, result)
		if suc then
			SellItemByPos(Conn, suc, result, nRoomIndex, nPos, nCount, charID)
			sold_goods_list(Conn, result["pay_type"],result["item_list_tbl"])
		else
			local errorMsgId = result["MsgID"]
			local shopType = result["ShopType"]
			MsgToConn(Conn, errorMsgId)
		end
	end
	local data = {["RoomIndex"]= nRoomIndex, ["Pos"]=nPos, 
					["ItemCount"]= nCount, ["CharID"]=charID, 
					["NpcName"] = sNpcName,["sSceneName"] = Conn.m_Player.m_Scene.m_SceneName}
					
	--local entry = (g_DBTransDef["NpcShopDB"])
	CallAccountManualTrans(Conn.m_Account, NpcShopDB, "PlayerSellItemByPos", CallBack, data)
end

--获取已售物品列表
function Gac2Gas:PlayerSoldGoodsList(Conn, NpcName)
	if GetDistanceFromNpcValidate(Conn,sNpcName) == false  then
		return
	end
	--打断物品使用读条
	if Conn.m_Player.m_UseItemLoadingTick then
		StopItemProgressTick(Conn.m_Player)
	end
 
	if Conn.m_Player.m_CommLoadProTick then
		CommStopLoadProgress(Conn.m_Player, EProgressBreak.ePB_OpenShop)
	end
	
	local function CallBack(item_list_tbl)
		sold_goods_list( Conn, NpcShop_Common(NpcName, "PayType"),item_list_tbl)
	end
	
	local data = {["uCharId"] = Conn.m_Player.m_uID,["NpcName"] = NpcName}
	
	CallAccountManualTrans(Conn.m_Account, NpcShopDB, "GetSoldGoodsList", CallBack, data)
end

--参数：1流通金币，2绑定金币
function Gac2Gas:UpdateChoosedMoneyPayType(Conn, moneyPayType)
	if GetDistanceFromNpcValidate(Conn,sNpcName) == false then
		return
	end
	if not IsCppBound(Conn.m_Player) then
        return  
	end
    local data = {["CharID"] = Conn.m_Player.m_uID, ["MoneyPayType"] = moneyPayType}
    local function CallBack(suc, moneyType)
        if suc then
            Gas2Gac:SetMoneyTypeBtnCheck(Conn, moneyType)
        end
        Gas2Gac:ReturnUpdateMoneyPayType(Conn)
    end
    
    CallAccountManualTrans(Conn.m_Account, NpcShopDB, "UpdateChoosedMoneyPayType", CallBack, data)
end

function Gac2Gas:UpdateShopTipFlag(Conn, shopType, tipFlag) 
    local data = {["CharID"] = Conn.m_Player.m_uID, ["ShopType"]=shopType, ["TipFlag"] = tipFlag}
    local function CallBack(suc, result)
        if suc then
            
            local liuTongShopTipFlag = result["LiuTongShopTipFlag"]   
            local commonShopTipFlag =   result["CommonShopTipFlag"] 
            Gas2Gac:ReturnNpcShopBuyTipInfo(Conn, liuTongShopTipFlag, commonShopTipFlag)
        end
    end
    CallAccountManualTrans(Conn.m_Account, NpcShopDB, "UpdateShopTipFlag", CallBack, data)  
end

function Gac2Gas:SetDefaultShopTipFlag(Conn)
    local data = {["CharID"] = Conn.m_Player.m_uID}
    local function CallBack(suc, result)
        if suc then
            Gas2Gac:ReturnNpcShopBuyTipInfo(Conn, 1, 1)
        end
    end
    CallAccountManualTrans(Conn.m_Account, NpcShopDB, "SetDefaultShopTipFlag", CallBack, data)      
end