gac_gas_require "liveskill/LiveSkillMgr"
cfg_load    "liveskill/LiveSkill_Common"
gac_gas_require "framework/common/CMoney"
   
local MaxOrderCountPerPage = 10
local LiveSkill_Common = LiveSkill_Common
local g_ItemInfoMgr  = g_ItemInfoMgr
local g_LiveSkillMgr    =   g_LiveSkillMgr
local g_RoomMgr = RequireDbBox("GasRoomMgrDB")	
local g_MoneyMgr = CMoney:new()
local event_type_tbl = event_type_tbl


local ContractManufactureDB = CreateDbBox(...)
local CContractManufacture = {}    
local LiuTongMoneyType 	= 1
    
local StmtDef = {
    "_InsertCMOrderStatic",
    [[
        insert into tbl_contract_manufacture_order_static(cmo_tCreateTime) values ( now() )
    ]]
}DefineSql(StmtDef, CContractManufacture)
    
local StmtDef = {
    "_InsertCMOrderInfo",
    [[
        insert into tbl_contract_manufacture_order (cmo_uId, cs_uId, cmo_sSkillName, cmo_sDirection, cmo_sPrescripName, cmo_tEndTime, cmo_uCMMoney, cmo_sDisplayProductName) values (?,?,?,?,?,DATE_ADD(now(), INTERVAL ? hour),?, ?)
    ]]
}DefineSql(StmtDef, CContractManufacture)

local StmtDef = {
    "_InsertCMOrderItemInfo",
    [[
        insert into tbl_contract_manufacture_order_item(cmo_uId, is_uId) values (?,?)
    ]]
}DefineSql(StmtDef, CContractManufacture)

--检查订单所需生活技能材料是否足够
local function CheckMaterialCountEnough(charID, materialTbl, materialCountTbl)
    for i, v in pairs (materialTbl)  do   
        local materialType = v[1]
        local materialName = v[2]
        local materialNum = v[3]
        if materialType ~= nil and materialName ~= "" then
            local materialCount = g_RoomMgr.GetItemCount(charID, materialType, materialName)
            if materialCount < materialNum then
                return false, 340001
            else
                materialCountTbl[i] = materialNum
            end
        end
    end
    return true
end
   
function ContractManufactureDB.AddContractManufactureOrder(data)
    local charID        = data["CharID"]
    local skillType     = data["SkillType"]
    local direction     = data["Direction"]
    local timeLimit     = data["TimeLimit"]
    local moneyCount    = data["Money"]
    local systemFee     = data["SystemFee"]
    local prescripName  = data["PrescripName"]
    
    local ItemBagLockDB = RequireDbBox("ItemBagLockDB")
    --检测帐户是否锁定
    if ItemBagLockDB.HaveItemBagLock(charID) then
        return false, 160043
    end
    local materialTbl = g_LiveSkillMgr:GetMateialInfoByProductInfo(skillType,direction,prescripName)
    local productType = LiveSkill_Common(skillType,direction,prescripName, "ProductType1")
    local productName = LiveSkill_Common(skillType,direction,prescripName, "ProductName1")
    local materialCountTbl = {0, 0, 0}
    
    local ret, msgID = CheckMaterialCountEnough(charID, materialTbl, materialCountTbl)
    if ret == false then
        return false, msgID
    end 

    local all_cost = moneyCount 
    local MoneyManagerDB =	RequireDbBox("MoneyMgrDB")
    if all_cost > MoneyManagerDB.GetMoney(charID) then
        return false, 340002
    end
    local fun_info = g_MoneyMgr:GetFuncInfo("ContractManufacture")
    local bFlag,uMsgID = MoneyManagerDB.AddMoneyByType(fun_info["FunName"],fun_info["AddContractOrder"],charID, LiuTongMoneyType, - all_cost,uEventId, event_type_tbl["代工收费"],charID)
	if not bFlag then
		CancelTran()
		if IsNumber(uMsgID) then
			return false,uMsgID
		else
			return false,340002
		end
	end    
	
    local sysFeeFlag,uMsgID, money = MoneyManagerDB.SubtractMoneyBySysFee(systemFee, charID, fun_info, fun_info["AddContractOrder"],event_type_tbl["添加代工订单管理费"])
    if not sysFeeFlag then
		CancelTran()
		if IsNumber(uMsgID) then
			return false,uMsgID
		else
			return false,340002
		end
	end   
    local bind_money_count, money_count = uMsgID, money
    
    CContractManufacture._InsertCMOrderStatic:ExecStat()
    local cmOrderID = g_DbChannelMgr:LastInsertId()
    
    local displayProductName = g_ItemInfoMgr:GetItemLanInfo(productType, productName, "DisplayName")
    CContractManufacture._InsertCMOrderInfo:ExecStat(cmOrderID, charID, skillType, direction, prescripName, timeLimit, moneyCount, displayProductName)
    if g_DbChannelMgr:LastAffectedRowNum() < 1  then
        CancelTran()
        return false, 340005
    end
    
    local materialIDTbl = {}
    local ItemIDTbl = {}
    for i, v in pairs (materialCountTbl) do
        local materialCount = v
        if materialCount > 0 then
            local materialType = materialTbl[i][1]
            local materialName = materialTbl[i][2]
            local tbl_item_id = g_RoomMgr.GetNCountItemIdByName(charID, materialType, materialName, materialCount) 
            local itemIdNum = tbl_item_id:GetRowNum()
            for j=1, itemIdNum do
                local itemID = tbl_item_id(j, 1)
                local ret = g_RoomMgr.MoveItemFromPackage(charID, itemID)
                if ret == false then
                    CancelTran()
                    return false, 340003
                end
                CContractManufacture._InsertCMOrderItemInfo:ExecStat(cmOrderID, itemID)
                if g_DbChannelMgr:LastAffectedRowNum() < 1  then
                    CancelTran()
                    return false, 340003
                end
                local nRoomIndex = tbl_item_id(j, 2)
                local nPos = tbl_item_id(j, 3)
                table.insert(materialIDTbl, {itemID, nRoomIndex, nPos})
                table.insert(ItemIDTbl, itemID)
            end
        end
    end
    local g_LogMgr = RequireDbBox("LogMgrDB")
    g_LogMgr.LogAddContractManuOrder( charID,ItemIDTbl,cmOrderID,skillType,direction,prescripName,moneyCount)
    local result = {}
    result["ItemInfo"]  = materialIDTbl
    result["Money"]     = all_cost + money_count
    result["BindMoneyCount"] = bind_money_count 
    return true, result
end

local StmtDef = {
    "_SelectCharCMOrder",
    [[
        select 
            cmo_uId, cmo_sSkillName, cmo_sDirection, cmo_sPrescripName,((unix_timestamp(cmo_tEndTime) - unix_timestamp(now()))/3600) , cmo_uCMMoney
        from 
            tbl_contract_manufacture_order
        where 
            cs_uId = ? limit ?, 10
    ]]
}DefineSql(StmtDef, CContractManufacture)

local StmtDef = {
    "_SelectCharCMOrderCount",
    [[
        select count(*) from tbl_contract_manufacture_order where cs_uId = ?
    ]]
}DefineSql(StmtDef, CContractManufacture)

local StmtDef = {
    "_SelectCMOrderItemInfo",
    [[
        select 
            i.is_uType, i.is_sName , count(*)
        from 
            tbl_item_static i, tbl_contract_manufacture_order_item cm 
        where 
            i.is_uId = cm.is_uId and cm.cmo_uId = ? group by i.is_uType, i.is_sName 
        
    ]]
}DefineSql(StmtDef, CContractManufacture)

function ContractManufactureDB.SearchCharContractManufactureOrder(data)
    local charID = data["CharID"]
    local curPage = data["CurPage"]

    local charCMOrderCountRet = CContractManufacture._SelectCharCMOrderCount:ExecStat(charID)
    local charCMOrderCount = charCMOrderCountRet:GetData(0,0)
    local totalPage = math.ceil(charCMOrderCount/MaxOrderCountPerPage)
    
    if curPage > totalPage then
        curPage = curPage - 1 
    end
    
    local orderInfoRet = CContractManufacture._SelectCharCMOrder:ExecStat(charID, (curPage-1)*MaxOrderCountPerPage)
    local orderNum =  orderInfoRet:GetRowNum()
    local itemInfoTbl = {}
    for i=1, orderNum do
        local orderID =  orderInfoRet(i, 1)
        local itemInfo =   CContractManufacture._SelectCMOrderItemInfo:ExecStat(orderID)
        table.insert(itemInfoTbl, itemInfo)
    end
    
    local result = {}
    result["OrderNum"] = orderNum
    result["OrderInfoRet"] = orderInfoRet
    result["ItemInfoRet"] = itemInfoTbl
    result["TotalPage"] = totalPage
    return result
end
    
local StmtDef = {
    "_SelectCMOrderByCharIDAndCMID",
    [[
        select count(*) from tbl_contract_manufacture_order where cmo_uId = ? and cs_uId = ?
    ]]
}DefineSql(StmtDef, CContractManufacture)

local StmtDef = {
    "_DeleteCMOrderByCharIDAndCMID",
    [[
        delete from tbl_contract_manufacture_order where cmo_uId = ? and cs_uId = ?
    ]]
}DefineSql(StmtDef, CContractManufacture)

local StmtDef = {
    "_SelectOrderMaterialInfo",
    [[
        select is_uId from tbl_contract_manufacture_order_item where cmo_uId = ?
    ]]    
}DefineSql(StmtDef, CContractManufacture)


local StmtDef = {
    "_SelectMaterialIDByItemTypeName",
    [[
        select item.is_uId from tbl_contract_manufacture_order_item item, tbl_item_static static 
        where static.is_uId = item.is_uId and static.is_uType = ? and static.is_sName = ? and cmo_uId = ?
    ]]
}DefineSql(StmtDef, CContractManufacture)

local StmtDef = {
    "_GetContractOrderMoney",
    [[
        select cmo_uCMMoney from tbl_contract_manufacture_order where cmo_uId = ?
    ]]
}DefineSql(StmtDef, CContractManufacture)

function ContractManufactureDB.CancelContractManufactureOrder(data)
    local charID    =   data["CharID"]
    local cmOrderID =   data["CMOrderID"]
 
    local orderInfo = CContractManufacture._SelectCMOrderByCharIDAndCMID:ExecStat(cmOrderID, charID)
    local ret_info_tbl = {}
    local item_bag_box = RequireDbBox("CPutItemToBagDB")
    local itemIDTbl = {}
    if orderInfo:GetData(0,0) > 0 then
        local materialTypeNameRet = CContractManufacture._SelectCMOrderItemInfo:ExecStat(cmOrderID)
        local materialKinds = materialTypeNameRet:GetRowNum()
        for i= 1, materialKinds do
            local uBigId = materialTypeNameRet:GetData(i-1, 0)
            local sItemName = materialTypeNameRet:GetData(i-1, 1)
            local tbl_item_id = {}
            local cmMaterilaIDRet = CContractManufacture._SelectMaterialIDByItemTypeName:ExecStat(uBigId, sItemName, cmOrderID)
            local materialIDCount =cmMaterilaIDRet:GetRowNum()
    		for i = 1, materialIDCount do
    		    local itemID = cmMaterilaIDRet:GetDataByName(i-1, "is_uId")
    			table.insert(tbl_item_id, {itemID})
    			table.insert(itemIDTbl, itemID)
    		end
    		local params = {}
    		params.m_nCharID = charID
    		params.m_nItemType = uBigId
    		params.m_sItemName = sItemName
    		params.m_tblItemID = tbl_item_id
    		local bFlag, ret_info = item_bag_box.AddCountItem(params)
    		if not bFlag then
    			CancelTran()
    			return false,ret_info
    		end
    		table.insert(ret_info_tbl, ret_info)
        end
        local fun_info = g_MoneyMgr:GetFuncInfo("ContractManufacture")
        local MoneyManagerDB =	RequireDbBox("MoneyMgrDB")
        local money = CContractManufacture._GetContractOrderMoney:ExecStat(cmOrderID)
        local moneyCount = money:GetData(0,0)
        local bFlag,uMsgID = MoneyManagerDB.AddMoneyByType(fun_info["FunName"],fun_info["CancelContractOrder"],charID, LiuTongMoneyType, moneyCount,uEventId, event_type_tbl["取消代工订单回退金钱"])
    	if not bFlag then
    		CancelTran()
    		if IsNumber(uMsgID) then
    			return false,uMsgID
    		else
    			return false,340007
    		end
    	end   
        ret_info_tbl["Money"] = moneyCount
        CContractManufacture._DeleteCMOrderByCharIDAndCMID:ExecStat(cmOrderID, charID)
        if g_DbChannelMgr:LastAffectedRowNum() < 1  then
            CancelTran()
            return false, 340007
        end
    else
        return false, 340006
    end
    local g_LogMgr = RequireDbBox("LogMgrDB")
    g_LogMgr.LogCancelContractManuOrder( charID,itemIDTbl,cmOrderID,event_type_tbl["手动取消代工所订单"] )
    return true, ret_info_tbl
end

local StmtDef = {
    "_SelectTimeOutCMOrder",
    [[
        select cmo_uId, cs_uId, cmo_uCMMoney from tbl_contract_manufacture_order where  cmo_tEndTime < now() - 60
    ]]
}DefineSql(StmtDef, CContractManufacture)


function ContractManufactureDB.CheckCMOrderTimeOut()
    
    local timeOutOrderRet = CContractManufacture._SelectTimeOutCMOrder:ExecStat()
    local timeOutOrderCount = timeOutOrderRet:GetRowNum()
    local lSysMailExecutor = RequireDbBox("SysMailDB")
    local SystemMsgDB= RequireDbBox("SystemMsgDB")
    local new_mail_playerID_tbl = {}
    
    local g_LogMgr = RequireDbBox("LogMgrDB")
    
    for i=1, timeOutOrderCount do
        local orderID = timeOutOrderRet:GetData(i-1, 0)
        local charID = timeOutOrderRet:GetData(i-1, 1)
        local money = timeOutOrderRet:GetData(i-1, 2)
         
        local orderMaterialRet = CContractManufacture._SelectOrderMaterialInfo:ExecStat(orderID)
        local order_item_ids_tbl = orderMaterialRet:ToTable(true)        
        lSysMailExecutor.SendSysMail(7001,charID,7002,"7003",order_item_ids_tbl,money,112, 5006)
        
        CContractManufacture._DeleteCMOrderByCharIDAndCMID:ExecStat(orderID, charID)
        if g_DbChannelMgr:LastAffectedRowNum() < 1  then
            CancelTran()
            return false, 340007
        end
        if ret == false then
            CancelTran()
            return false
        end
        table.insert(new_mail_playerID_tbl, charID)
        
		local itemIDTbl = {}
		for i, v in pairs(order_item_ids_tbl) do
		    table.insert(itemIDTbl, v[1])
	    end
		g_LogMgr.LogCancelContractManuOrder( charID,itemIDTbl,orderID,event_type_tbl["自动取消代工所订单"] )
    end
    

    local result = {["MakeOrderPlayerIDTbl"] = new_mail_playerID_tbl}
    return true, result
end

local StmtDef = {
    "_DeleteCMCfgInfo",
    [[
        delete from tbl_contract_manufacture_cfg_info
    ]]
}DefineSql(StmtDef, CContractManufacture)

function ContractManufactureDB.ReadLiveSkillCfgInfoToDB()
    CContractManufacture._DeleteCMCfgInfo:ExecStat()
    
    local sql = "insert into tbl_contract_manufacture_cfg_info (cmci_sSkillName, cmci_sDirection, cmci_sPrescripName, cmci_uSkillLevel, cmci_uProductLevel) values "
    local sqlStatTbl = {}
    local skillTypeKeys = LiveSkill_Common:GetKeys()
    for i, v in pairs (skillTypeKeys) do
        local skillType = skillTypeKeys[i]
        local directionKeys = LiveSkill_Common:GetKeys(skillType)
        for j, k in pairs (directionKeys) do
            local direction = k
            local prescripNames = LiveSkill_Common:GetKeys(skillType, direction)
            for w, q in pairs (prescripNames) do
                local prescripName = q
                local productType = LiveSkill_Common(skillType, direction, prescripName, "ProductType1")
                local productName = LiveSkill_Common(skillType, direction, prescripName, "ProductName1")
                local skillLevel = LiveSkill_Common(skillType, direction, prescripName, "SkillLevel")
                local productLevel = g_ItemInfoMgr:GetItemInfo(productType, productName, "BaseLevel")
                local stat = string.format("%s%s%s%s%s%s%s%d%s%d%s", "('", skillType, "', '", direction, "', '", prescripName, "', ", skillLevel, ", ",productLevel, ")")
                table.insert(sqlStatTbl, stat)
            end    
        end            
    end
    
    sql = sql .. table.concat(sqlStatTbl, ",")
    
    g_DbChannelMgr:TextExecute(sql)
end


function ContractManufactureDB.SearchCommonContractManuOrder(data)
    local productName       =   data["ProductName"]
    local skillName         =   data["SkillName"]
    local skillLevel        =   data["SkillLevel"]
    local productLowLevel   =   data["LowLevel"]
    local productHighLevel  =   data["HighLevel"]
    local curPage           =   data["CurPage"]

    local sql = "select cmo_uId, ch.c_sName, cmo_sSkillName, cmo_sDirection, cmo_sPrescripName, ((unix_timestamp(cmo_tEndTime) - unix_timestamp(now()))/3600) , cmo_uCMMoney "
    sql = sql .. " from tbl_contract_manufacture_order o, tbl_contract_manufacture_cfg_info cfg, tbl_char ch"
    sql = sql .. " where ((unix_timestamp(cmo_tEndTime) - unix_timestamp(now()))/3600) > 0 "
    
    if skillName ~= "" then
        sql = sql .. " and cfg.cmci_sSkillName = '" ..  skillName .. "' "
    end
    
    if productName ~= "" then
        sql = sql .. " and o.cmo_sDisplayProductName like '%" ..  productName .. "%' "
    end
    
    if productLowLevel > 0 then
       sql = sql .. " and cfg.cmci_uProductLevel >= "  .. productLowLevel
    end
    
    if productHighLevel ~= 0 and productHighLevel < 80 then
       sql = sql .. " and cfg.cmci_uProductLevel <= "  .. productHighLevel
    end
    
    if skillLevel ~= 0 then
       sql = sql .. " and cfg.cmci_uSkillLevel = " .. skillLevel
    end
    
    sql = sql .. " and o.cmo_sSkillName = cfg.cmci_sSkillName and o.cmo_sDirection = cfg.cmci_sDirection and o.cmo_sPrescripName = cfg.cmci_sPrescripName and ch.cs_uId = o.cs_uId "
    
    local sqlLimit = sql .. " limit " .. (curPage-1)*MaxOrderCountPerPage .. ", 10"
    
    local _, totalOrderCountRet = g_DbChannelMgr:TextExecute(sql)
    local totalOrderCount = totalOrderCountRet:GetRowNum()
    local totalPage = math.ceil(totalOrderCount/MaxOrderCountPerPage)
    
    local _, commonCMOrderRet = g_DbChannelMgr:TextExecute(sqlLimit)
    local commonCMOrderCount = commonCMOrderRet:GetRowNum()
    local materialInfoTbl = {}
    for i=1, totalOrderCount do
        local cmOrderID = totalOrderCountRet:GetData(i-1, 0)
        local materialTypeNameRet = CContractManufacture._SelectCMOrderItemInfo:ExecStat(cmOrderID)
        local materialCount = materialTypeNameRet:GetRowNum()
        if materialCount > 0 then
            local materialInfo = {}
            for j =1, materialCount do
                local materialType =  materialTypeNameRet:GetData(j-1, 0)
                local materialName =  materialTypeNameRet:GetData(j-1, 1)
                local materialCount=  materialTypeNameRet:GetData(j-1, 2)
                local node = {materialType, materialName, materialCount}
                table.insert(materialInfo, node)
            end
            table.insert(materialInfoTbl, materialInfo) 
        else
            table.insert(materialInfoTbl, {}) 
        end
    end
    local result = {["TotalOrderCount"] =totalOrderCount , ["TotalPage"] = totalPage, ["OrderRet"] = totalOrderCountRet, ["MaterialInfoTbl"] = materialInfoTbl}
    return result
end

local StmtDef = {
    "_SelectChoosedCMOrder",
    [[
        select 
            cmo_sSkillName, cmo_sDirection, cmo_sPrescripName,cs_uId, cmo_uCMMoney
        from 
            tbl_contract_manufacture_order
        where 
            cmo_uId = ?  and ((unix_timestamp(cmo_tEndTime) - unix_timestamp(now()))/3600) > 0
    ]]
}DefineSql(StmtDef, CContractManufacture)

--代工
function ContractManufactureDB.TakeContractManufactureOrder(data)
    local charID    =   data["CharID"]
    local cmOrderID =   data["CMOrderID"]
    local SceneName = ""
    
    local ItemBagLockDB = RequireDbBox("ItemBagLockDB")
    --检测帐户是否锁定
    if ItemBagLockDB.HaveItemBagLock(charID) then
        return false, 160044
    end
    
    local cmOrderRet = CContractManufacture._SelectChoosedCMOrder:ExecStat(cmOrderID)
    if cmOrderRet:GetRowNum() == 0 then
        return false, 340006 
    end

    local skillName = cmOrderRet:GetData(0,0)
    local direction = cmOrderRet:GetData(0,1)
    local prescripName = cmOrderRet:GetData(0,2)
    local target_id = cmOrderRet:GetData(0,3)
    local DirectionsMakeDB = RequireDbBox("DirectionsMakeDB")
    local bFlag,tblRetRes = DirectionsMakeDB.MakeDirectionForContractManufacture(charID,target_id,skillName,direction,prescripName,SceneName)
	if not bFlag then
		CancelTran()
		return bFlag,tblRetRes
	end
	
	local money = cmOrderRet:GetData(0,4)
	local makeProductFee = g_MoneyMgr:GetAftertaxMoney(money)
    local sysFee = money - makeProductFee
	--通过系统邮件将代工费用发送到生活技能制作方
    local lSysMailExecutor = RequireDbBox("SysMailDB")
	local succ2=lSysMailExecutor.SendSysMail(7001,charID,7002,
	"7005_money:1|" ..  makeProductFee,nil, makeProductFee,165,5007)
	if succ2 ~= true then
        CancelTran()
		return bFlag,tblRetRes
	end
	
	--下面是给erating发送消息的，这块比较特殊，所有单独调用
	local fun_info = g_MoneyMgr:GetFuncInfo("ContractManufacture")
	local MoneyManagerDB =	RequireDbBox("MoneyMgrDB")
	if not MoneyManagerDB.UpdateRmbMoney(target_id,-money,fun_info["FunName"],164,nil,charID,makeProductFee) then
		return
	end
	
	--删除订单
    CContractManufacture._DeleteCMOrderByCharIDAndCMID:ExecStat(cmOrderID, target_id)
    if g_DbChannelMgr:LastAffectedRowNum() < 1  then
        CancelTran()
        return false, 340007
    end
    local g_LogMgr = RequireDbBox("LogMgrDB")
    g_LogMgr.SaveTaxLog(sysFee, charID, event_type_tbl["代工成功扣税"])
    local result = {}
    result["LiveSkillRes"] = tblRetRes
	
	local itemIDTbl =tblRetRes.ItemIDTbl --产出物品ID
	g_LogMgr.LogMakeContractManuOrder( charID,itemIDTbl,cmOrderID)
	return true, result
end

SetDbLocalFuncType(ContractManufactureDB.SearchCommonContractManuOrder)
SetDbLocalFuncType(ContractManufactureDB.SearchCharContractManufactureOrder)
SetDbLocalFuncType(ContractManufactureDB.AddContractManufactureOrder)


return ContractManufactureDB
