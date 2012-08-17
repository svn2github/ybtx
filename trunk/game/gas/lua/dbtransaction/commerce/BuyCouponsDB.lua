local event_type_tbl = event_type_tbl
local LogErr = LogErr
gac_gas_require "framework/common/CMoney"
local g_MoneyMgr = CMoney:new()
local BuyCouponsDB  = CreateDbBox(...)
    
local StatementSql = {}

local StmtDef = {
    "_GetCouponsPrice",
    [[
        select ifnull(ci_uPrice,0) from tbl_coupons_info where ci_uId = ? and ci_sName = ?  
    ]]
}DefineSql(StmtDef, StatementSql)


local StmtDef = {
    "_InsertCouponsSequenceID",
    [[
        insert into tbl_char_bought_coupons (cs_uId, ci_uId, cbc_sSequenceId) values (?,?,?)
    ]]
}DefineSql(StmtDef, StatementSql)

local StmtDef = {
    "_GetBoughtCouponsInfo",
    [[
        select cbc_uId, c.ci_uId, cbc_sSequenceId, ci_sName, ci_uPrice, ci_uSmallIcon, ci_sDesc, ci_sUrl  from tbl_char_bought_coupons c, tbl_coupons_info i 
        where c.cs_uId= ? and c.ci_uId = i.ci_uId 
    ]]
}DefineSql(StmtDef, StatementSql)

function BuyCouponsDB.BuyCouponsUsingJinBi(data)
    local charID    =   data["CharID"]
    local ID        =   data["ID"]
    local name      =   data["Name"]
    local sequenceID=   data["SequenceID"]
    
    local priceRet = StatementSql._GetCouponsPrice:ExecStat(ID, name) 
    local price = 0
    if priceRet:GetRowNum() >= 1 then
        price = priceRet:GetData(0, 0 )
    else
        LogErr("点券信息不存在", ID, name)
    end 
    
    if price == 0 then
        return false, 430002 
    end
    
    StatementSql._InsertCouponsSequenceID:ExecStat(charID, ID, sequenceID)
    if g_DbChannelMgr:LastAffectedRowNum() == 0 then
        LogErr("点券信息插入错误", charID, ID, sequenceID) 
        return false, 430002
    end
    
    local MoneyManagerDB =	RequireDbBox("MoneyMgrDB")
	local fun_info = g_MoneyMgr:GetFuncInfo("BuyCoupons")
	
	local bFlag,uMsgID = MoneyManagerDB.AddMoney(fun_info["FunName"],fun_info["BuyCouponsUsingJinBi"],charID, -price,nil,event_type_tbl["购买点券"])
	if not bFlag then
		CancelTran()
		if IsNumber(uMsgID) then
			return false,uMsgID
		else
			return false, 8023
		end
	end
	local uEventId = uMsgID
	local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.SaveCharBoughtCouponsLog(uEventId,charID,ID,sequenceID)
	local boughtCouponsRet = StatementSql._GetBoughtCouponsInfo:ExecStat(charID)
    local result = {}
    result["Money"] = price
    result["BoughtCoupons"] = boughtCouponsRet
    return true, result
end

function  BuyCouponsDB.GetBoughtCouponsInfo(data)
    local charID = data["CharID"]
	local boughtCouponsRet = StatementSql._GetBoughtCouponsInfo:ExecStat(charID)
    local result = {}
    result["BoughtCoupons"] = boughtCouponsRet
    return  result
end

local StmtDef = {
    "_GetCapableOfBuyingCouponsInfo",
    [[
        select ci_uId,ci_sName,ci_uPrice,ci_uSmallIcon, ci_sDesc,ci_sUrl
        from tbl_coupons_info 
    ]]
}DefineSql(StmtDef, StatementSql)

function BuyCouponsDB.UpdateCouponsInfo(data)
    local couponsList = data["CouponsList"]
    local g_LogMgr = RequireDbBox("LogMgrDB")
    if # couponsList > 0 then
     
        local stmt = "replace into tbl_coupons_info (ci_uId, ci_sName, ci_uSmallIcon, ci_uPrice, ci_sDesc, ci_sUrl) values "
        local tbl = {}
        for i, v in pairs (couponsList) do
            local ID    =   v["ID"]
            local name  =   v["Name"]
            local smallIcon = v["SmallIcon"]
            local price     = v["Price"]
            local desc      = v["Desc"]
            local url       = v["Url"]                                              
            
            g_LogMgr.SaveCouponsInfoLog(ID,name,smallIcon,price,desc,url)
            local str = string.format("%s%d%s%s%s%d%s%d%s%s%s%s%s", "(", ID, ",'", name, "',", smallIcon, ",", price, ", '", desc, "','", url, "')")
            table.insert(tbl, str)
        end
        local stmtStr = table.concat(tbl, ",")
        stmt = stmt .. stmtStr
        g_DbChannelMgr:TextExecute(stmt)
    end
end 

local StmtDef = {
    "_GetBuyCouponsWebInfo",
    [[
        select  sc_sVarValue from tbl_conf_server where  sc_sVarName = ?      
    ]]
}DefineSql(StmtDef, StatementSql)

function BuyCouponsDB.GetBuyCouponsWebInfo()
    local ip = StatementSql._GetBuyCouponsWebInfo:ExecStat("BuyCouponsWebIP")
    local port = StatementSql._GetBuyCouponsWebInfo:ExecStat("BuyCouponsWebPort")
    return ip, port
end
    
return BuyCouponsDB