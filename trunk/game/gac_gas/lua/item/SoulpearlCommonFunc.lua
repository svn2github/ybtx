cfg_load "item/SoulPearl_Common"
--根据魂珠所含魂值和当前选择的支付方式（金币or积分），得到魂珠具体的价格
--参数：魂珠所含魂值，支付方式
function GetSoulpearlPriceByTypeAndCount(item_name,soulCount, payType)
    local pay_Type = payType
    if pay_Type == nil then
        pay_Type = 0
    end

    local soulpearlType = g_ItemInfoMgr:GetSoulPearlBigID()
    local soulpearlPriceMethod
    if pay_Type == 0 then
        soulpearlPriceMethod = g_ItemInfoMgr:GetItemInfo(soulpearlType, item_name, "Price")
        if IsNumber(soulpearlPriceMethod) then
            return  soulpearlPriceMethod
        end
    else
        soulpearlPriceMethod = GetItemJiFenPrice(soulpearlType, item_name ..":" .. soulCount, 1, payType)
        if IsNumber(soulpearlPriceMethod) then
           return soulpearlPriceMethod 
        end
    end
    local beginStr = "return function (x) \n local soulPrice = "
    local endStr = "\n  return soulPrice end"
    local funcStr = beginStr .. soulpearlPriceMethod .. endStr
    local func = loadstring(funcStr)()
    return func(soulCount)
end

--得到魂珠价格信息：价格+字符串描述；eg：**金**银**铜或者**自然积分
function GetSoulpearlPriceInfo(item_name,soulCount, payType, priceRate, eGoldType)
    if (g_GameMain.m_NPCShopSell:IsShow() or g_GameMain.m_NPCShopPlayerSold:IsShow()) 
        and g_GameMain.m_NPCShopSell.m_PayType ~= 0  then
        if payType == nil then
           payType = g_GameMain.m_NPCShopSell.m_PayType
        end 
        local soulpearlPrice = GetSoulpearlPriceByTypeAndCount(item_name,soulCount, payType)
        local jiFenPriceDesc = math.ceil(soulpearlPrice * priceRate) .. GetJiFenTypeStr(payType)
        return jiFenPriceDesc 
    else
        local soulpearlPrice = GetSoulpearlPriceByTypeAndCount(item_name,soulCount, payType or 0)
        local c_money = CMoney:new()
        local moneyDesc =  c_money:ChangeMoneyToString(math.ceil(soulpearlPrice * priceRate), eGoldType)
        return moneyDesc
    end
end