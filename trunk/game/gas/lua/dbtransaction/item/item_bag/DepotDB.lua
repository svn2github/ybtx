gac_gas_require "item/store_room_cfg/StoreRoomCfg"
gac_gas_require "framework/common/CMoney"
local g_SlotIndex = g_SlotIndex
local g_MoneyMgr = CMoney:new()
local StmtOperater = {}	
local event_type_tbl = event_type_tbl
local CDepotMgr = CreateDbBox(...)

local StmtDef=
{
    "_GetDepotMoney",
    [[
    	select dm_uMoney from tbl_depot_money where cs_uId = ?
    ]]
}
DefineSql(StmtDef,StmtOperater)
local StmtDef=
{
    "_AddDepotMoneyInfo",
    [[
    	replace into tbl_depot_money(cs_uId,dm_uMoney) values(?,?)
    ]]
}
DefineSql(StmtDef,StmtOperater)
function CDepotMgr.GetDepotMoney(nCharID)
	local res = StmtOperater._GetDepotMoney:ExecSql( "n", nCharID)
	if res:GetRowNum() == 0 then
		return 0
	end
	return res:GetNumber(0,0)
end

local StmtDef=
{
    "_UpdateDepotMoney",
    [[
    	update tbl_depot_money set dm_uMoney = dm_uMoney + ? where dm_uMoney + ? >= 0
    	and  cs_uId = ?
    ]]
}
DefineSql(StmtDef,StmtOperater)
function CDepotMgr.UpdateDepotMoney(nCharID,nMoney)
	StmtOperater._UpdateDepotMoney:ExecSql( "",nMoney,nMoney, nCharID)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end

function CDepotMgr.GetDepotInfo(parameters)
	local nCharID = parameters["nCharID"]
	return CDepotMgr.GetDepotMoney(nCharID)
end


--´æÇ®
function CDepotMgr.SaveMoneyToDepot(parameters)
	local nCharID = parameters["nCharID"]
	local nMoney = parameters["nMoney"]
	
	local ItemBagLockDB = RequireDbBox("ItemBagLockDB")
  if ItemBagLockDB.HaveItemBagLock(nCharID) then
  	return 160023
  end
	
	local MoneyMgrDB = RequireDbBox("MoneyMgrDB")
	local fun_info = g_MoneyMgr:GetFuncInfo("Depot")
	local bFlag,uMsgID = MoneyMgrDB.AddMoney(fun_info["FunName"],fun_info["SaveMoney"],nCharID, -nMoney,nil,event_type_tbl["Íù²Ö¿â´æÇ®"],nCharID)
	if not bFlag then
		CancelTran()
		if IsNumber(uMsgID) then
			return uMsgID
		else
			return 160002
		end
	end
	
	local bSucc = false
	if CDepotMgr.GetDepotMoney(nCharID) == 0 then
		StmtOperater._AddDepotMoneyInfo:ExecSql( "",nCharID, nMoney)
		if g_DbChannelMgr:LastAffectedRowNum() == 0 then
			error("ÐÞ¸Ä²Ö¿â½ðÇ®Ê§°Ü")
		end
		bSucc = true
	else
		bSucc = CDepotMgr.UpdateDepotMoney(nCharID,nMoney)
		if not bSucc then
			error("ÐÞ¸Ä²Ö¿â½ðÇ®Ê§°Ü")
		end
	end
	local depot_money = CDepotMgr.GetDepotMoney(nCharID)
	local LogMgr = RequireDbBox("LogMgrDB")
	local eventId = uMsgID
	local succ = LogMgr.SaveMoneyChangedFromDepot(eventId,nMoney,nCharID,depot_money)
	if not succ then
		error("¼ÇÂ¼ÐÞ¸Ä²Ö¿âÇ®logÊ§°Ü")
	end
	return bSucc,depot_money
end

--È¡Ç®
function CDepotMgr.GetMoneyFromDepot(parameters)
	local nCharID = parameters["nCharID"]
	local nMoney = parameters["nMoney"]
	local MoneyMgrDB = RequireDbBox("MoneyMgrDB")
	
	local ItemBagLockDB = RequireDbBox("ItemBagLockDB")
  if ItemBagLockDB.HaveItemBagLock(nCharID) then
  	return 160024
  end
  
	if not CDepotMgr.UpdateDepotMoney(nCharID,-nMoney) then
		return 160003
	end
	local depot_money = CDepotMgr.GetDepotMoney(nCharID)
	local fun_info = g_MoneyMgr:GetFuncInfo("Depot")
	local bFlag,uMsgID = MoneyMgrDB.AddMoney(fun_info["FunName"],fun_info["GetMoney"],nCharID, nMoney,nil,event_type_tbl["´Ó²Ö¿âÈ¡Ç®"])
	if not bFlag then
		CancelTran()
		if IsNumber(uMsgID) then
			return uMsgID
		end
	end
	local LogMgr = RequireDbBox("LogMgrDB")
	local eventId = uMsgID
	local succ = LogMgr.SaveMoneyChangedFromDepot(eventId,-nMoney,nCharID,depot_money)
	if not succ then
		error("¼ÇÂ¼ÐÞ¸Ä²Ö¿âÇ®logÊ§°Ü")
	end
	return bFlag,depot_money
end

SetDbLocalFuncType(CDepotMgr.GetDepotInfo)
SetDbLocalFuncType(CDepotMgr.GetMoneyFromDepot)
SetDbLocalFuncType(CDepotMgr.SaveMoneyToDepot)
return CDepotMgr