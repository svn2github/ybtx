gac_gas_require "commerce/npc_shop/NPCShopMgrInc"
cfg_load "npc/NpcShop_Common"
cfg_load "commerce/ItemInShopTongLevel_Common"
cfg_load "npc/WithOneNpcShopDisplay_Common"

local TongNpcShop = 10--团贡商店，不允许玩家卖东西到商店
local WithOneCommonShop = 1
local WithOneJiFenShop  = -1
local NotWithOneShop = 0


local function InitNpcShopSellList(npcName, index, itemType, itemName, shopPayType, shopType)
    if NotWithOneShop ~= shopType and shopType ~= WithOneJiFenShop then --非随身商店
        if shopType >= WithOneCommonShop then
            local node = {itemType, itemName}
            table.insert(g_NPCShopMgr[npcName], node)
        end
    else
        if g_NPCShopMgr[npcName][index] == nil then
            g_NPCShopMgr[npcName][index] = {}
            g_NPCShopMgr[npcName][index].ItemType = itemType
            table.insert(g_NPCShopMgr[npcName][index], itemName) 
        else
            if g_NPCShopMgr[npcName][index].ItemType == itemType then
                table.insert(g_NPCShopMgr[npcName][index], itemName)
            else
                index = index + 1
                g_NPCShopMgr[npcName][index] = {}
                g_NPCShopMgr[npcName][index].ItemType = itemType
                table.insert(g_NPCShopMgr[npcName][index], itemName) 
            end
        end 
    end
    return index
end

function CheckNPCShop_Common()
    --每一个原组都是一种类型的商品，一种类型的商品里包括很多件，每一件商品的信息为：物品类型、物品名称
   
    local keys = NpcShop_Common:GetKeys()
    g_WithOneJiFenNpcShopMgr = {}
    for i, p in pairs(keys) do
        local npcName = p
        g_NPCShopMgr[npcName] = {}
        local shopName = NpcShop_Common(npcName, "ShopName")
        g_NPCShopMgr[npcName].NPCShopName = shopName
        local index = 1 --商店所售物品的种类信息，默认为1
        local shopType = NpcShop_Common(npcName, "ShopType") --0为npc商店，其余的为随身商

        for j =1, 120 do
            local itemTypeStr = "ItemType" .. j
        	local itemNameStr = "ItemName" .. j
        	local itemType = tonumber(NpcShop_Common(npcName, itemTypeStr))
        	local itemName = NpcShop_Common(npcName, itemNameStr)
        	if itemType == "" or itemType == nil or  itemType ==0  then
        		break
        	end     
	        index = InitNpcShopSellList(npcName, index, itemType, itemName, shopPayType, shopType)
        end
                
        local shopPayType = WithOneNpcShopDisplay_Common(npcName, "JiFenType")
        
        if shopType == WithOneJiFenShop then --积分商店
            if g_WithOneJiFenNpcShopMgr[shopPayType] == nil then
                g_WithOneJiFenNpcShopMgr[shopPayType] = {}
            end
            table.insert(g_WithOneJiFenNpcShopMgr[shopPayType], npcName)
        end  	
    end
end

CheckNPCShop_Common()

cfg_load "commerce/ItemJiFenPrice_Common"
cfg_load "commerce/ItemInShopTongLevel_Common"

local function GetCommonJiFenPrice(itemType, payType, itemName)
    if ItemJiFenPrice_Common(tostring(itemType)) == nil then
       return 0 
    end
    local itemInfo = ItemJiFenPrice_Common(tostring(itemType), tostring(payType))
    if itemInfo == nil then
       return 0 
    end
    local jiFenPrice = itemInfo(itemName) 
    if jiFenPrice == nil then
        jiFenPrice = 0 
    end
    return jiFenPrice
end

local function GetTongJiFenPrice(itemType, payType, itemName)
    local itemInfo = ItemInShopTongLevel_Common(tostring(itemType))
    if itemInfo == nil then
       return 0 
    end
    local info = itemInfo(itemName)
    if info == nil then
       return 0 
    end
    local jiFenPrice = info("JiFenPrice") or 0
    return jiFenPrice
end

--根据物品type、物品名称、价格比率（1或者1/4）、商店类型（积分1、2、3等）
function GetItemJiFenPrice(itemType, itemName, priceRate, payType)
    local jiFenPrice = 0
    if payType == TongNpcShop then--团贡商店，不允许玩家卖东西到商店
        jiFenPrice =  GetTongJiFenPrice(itemType, payType, itemName)
    else
        jiFenPrice = GetCommonJiFenPrice(itemType, payType, itemName)
    end
    jiFenPrice = math.ceil(jiFenPrice * priceRate)
    if jiFenPrice < 0 then
       return  0
    end
    return jiFenPrice
end

--根据商店当前类型，得到物品价格，payType（0金币，之外的是积分）
function GetItemPriceByPayType(itemType, itemName, payType)
    if payType == 0 or payType == nil then
        local Price = g_ItemInfoMgr:GetItemInfo(itemType, itemName, "Price") or 0
        if Price < 0 then
            return 0
        else
            return Price
        end
    else
        local jiFenPrice = GetItemJiFenPrice(itemType, itemName, 1, payType)
        return jiFenPrice
    end
end

function CheckNpcShopItemTongLevel(itemType, itemName, tongLevel, tongPlaceLevel)
    local needTongLevelInfo = ItemInShopTongLevel_Common(tostring(itemType))
    local needTongLevel, needTongPlaceLevel = 0, 0
    local tongLevelInfo = 0
    if needTongLevelInfo then
        tongLevelInfo = needTongLevelInfo(itemName)
    else
        return false, 0, 0
    end
    
    if tongLevelInfo then
        needTongLevel = tongLevelInfo("TongLevel") or 0
        needTongPlaceLevel = tongLevelInfo("TongPlaceLevel") or 0
    else
        return false, 0, 0
    end
    if tongPlaceLevel == nil then
        tongPlaceLevel = 0
    end
    if tongLevel == nil then
        tongLevel = 0
    end
    if needTongLevel > tongLevel or needTongPlaceLevel > tongPlaceLevel then
        return false, needTongLevel, needTongPlaceLevel
    end
    return true, needTongLevel, needTongPlaceLevel
end

function GetItemPriceInTongShop(itemType, itemName, nCount, soulCount)
    local itemInfo = ItemInShopTongLevel_Common(tostring(itemType))
    if itemInfo == nil then
        return 0 
    end
    local info = itemInfo(itemName)
    if info == nil then
        return 0 
    end
    local price = info("Price") 
    if g_ItemInfoMgr:IsSoulPearl(itemType) then
        local soulpearlPriceMethod = price
        local beginStr = "return function (x) \n local soulPrice = "
        local endStr = "\n  return soulPrice end"
        local funcStr = string.format("%s%s%s", beginStr, soulpearlPriceMethod, endStr)
        local func = loadstring(funcStr)()
        price = func(soulCount)
    end
    price = price or 0
    return price
end