gac_gas_require "framework/common/CMoney"
gac_gas_require "relation/tong/TongMgr"
local g_MoneyMgr = CMoney:new()
local g_TongMgr = CTongMgr:new()
local event_type_tbl = event_type_tbl
local StmtOperater = {}	

local CTongContriMoney = CreateDbBox(...)

local StmtDef = {
    	"_AddContributeMoneyInfo",
    	[[ 
    		insert into tbl_tong_contribute_money
    		set cs_uId = ?,tcm_uContriNum = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_UpdateContributeMoneyInfo",
    	[[ 
    		update tbl_tong_contribute_money
    		set tcm_uContriNum = tcm_uContriNum + ? where cs_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_AddContributeMoneyforExp",
    	[[ 
    		insert into tbl_tong_contribute_money
    		set cs_uId = ?,tcm_uContriNumforExp = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )
local StmtDef = {
    	"_AddContributeMoneyforSoul",
    	[[ 
    		insert into tbl_tong_contribute_money
    		set cs_uId = ?,tcm_uContriNumforSoul = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )
local StmtDef = {
    	"_AddContributeMoneyforResource",
    	[[ 
    		insert into tbl_tong_contribute_money
    		set cs_uId = ?,tcm_uContriNumforResource = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_UpdateContributeMoneyforExp",
    	[[ 
    		update tbl_tong_contribute_money
    		set tcm_uContriNumforExp = tcm_uContriNumforExp + ? where cs_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )
local StmtDef = {
    	"_UpdateContributeMoneyforSoul",
    	[[ 
    		update tbl_tong_contribute_money
    		set tcm_uContriNumforSoul = tcm_uContriNumforSoul + ? where cs_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )
local StmtDef = {
    	"_UpdateContributeMoneyforResource",
    	[[ 
    		update tbl_tong_contribute_money
    		set tcm_uContriNumforResource = tcm_uContriNumforResource + ? where cs_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetTotalContributeMoneyInfo",
    	[[ 
    		select sum(tcm_uContriNum) from tbl_tong_contribute_money
    		where cs_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetTotalContributeMoneyforExp",
    	[[ 
    		select sum(tcm_uContriNumforExp) from tbl_tong_contribute_money
    		where cs_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetTotalContributeMoneyforSoul",
    	[[ 
    		select sum(tcm_uContriNumforSoul) from tbl_tong_contribute_money
    		where cs_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetTotalContributeMoneyforResource",
    	[[ 
    		select sum(tcm_uContriNumforResource) from tbl_tong_contribute_money
    		where cs_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_DelContributeMoneyInfo",
    	[[ 
    		delete from tbl_tong_contribute_money
    	]]
}    
DefineSql ( StmtDef, StmtOperater )
-----------------------------------------------------------------------
function CTongContriMoney.ClearContributeMoneyInfo(parameters)
	StmtOperater._DelContributeMoneyInfo:ExecStat()
end

local function GetContributeMoney(player_level)
	return player_level * player_level * 40
end

local function GetContributeMoneyForExp(player_level,tong_level)
	 return player_level * player_level * 40
end

local function GetContributeMoneyForSoul(player_level,tong_level)
	return player_level * player_level * 40
end

local function GetContributeMoneyForResource(player_level,tong_level)
	return player_level * player_level * 200
end

function CTongContriMoney.GetMoneyCanContribute(parameters)
	local uPlayerID = parameters["uPlayerID"]
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	local player_level = CharacterMediatorDB.GetPlayerLevel(uPlayerID)
	local max_money = GetContributeMoney(player_level)
	local sum_money_ret = StmtOperater._GetTotalContributeMoneyInfo:ExecStat(uPlayerID)
	return max_money,sum_money_ret(1,1) or 0
end

--【捐献金钱】
function CTongContriMoney.ContributeMoneyDB(parameters)
	local uPlayerID = parameters["uPlayerID"]
	local nNum = parameters["nNum"]
	
	local tong_box = RequireDbBox("GasTongBasicDB")
	local uTongID = tong_box.GetTongID(uPlayerID)
	
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	local player_level = CharacterMediatorDB.GetPlayerLevel(uPlayerID)
	local max_money = GetContributeMoney(player_level)
	local sum_money_ret = StmtOperater._GetTotalContributeMoneyInfo:ExecStat(uPlayerID)
	local sum_money = sum_money_ret(1,1) or 0
	if sum_money+nNum > max_money then
		return false,9372
	end
	
	local MoneyManagerDB=	RequireDbBox("MoneyMgrDB")
	local fun_info = g_MoneyMgr:GetFuncInfo("Tong")
  local bFlag,uMsgID = MoneyManagerDB.AddMoney(fun_info["FunName"],fun_info["Contribut"],uPlayerID, -nNum,nil,event_type_tbl["佣兵小队成员捐献金钱"])
	if not bFlag then
		if IsNumber(uMsgID) then
			CancelTran()
			return false,uMsgID
		end
		return false,9331
	end
	--给帮会加钱
	if not tong_box.UpdateTongMoney({["uTongID"]=uTongID,["uMoney"]= nNum,["nEventType"] = event_type_tbl["佣兵小队成员捐献金钱"]}) then
		CancelTran()
		return
	end
	if sum_money_ret(1,1) == nil then
		StmtOperater._AddContributeMoneyInfo:ExecStat(uPlayerID,nNum)
	else
		StmtOperater._UpdateContributeMoneyInfo:ExecStat(nNum,uPlayerID)
	end
	if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  		error("添加佣兵团捐献金钱信息失败")
  end
  
	local AreaFbPointDB = RequireDbBox("AreaFbPointDB")
	local event_id = uMsgID
	local  IsSucc,_ = AreaFbPointDB.AddAreaFbPointByType(uPlayerID,nNum*0.1,10,"佣兵团捐献金钱",event_id)
	if not IsSucc then
		return false
	end
	local ex = RequireDbBox("Exchanger")
	local char_name = ex.getPlayerNameById(uPlayerID)
	local sMsg = "2014_" .. char_name .. ",money:1|" .. nNum
  if not tong_box.AddTongLogs(uTongID,sMsg, g_TongMgr:GetLogType("人员")) then
  	return false
  end
	return true
end


function CTongContriMoney.ContributeMoneyChangeExp(data)
	local uPlayerID = data["uPlayerID"]
	local uMoney = data["uMoney"]
	local tong_box = RequireDbBox("GasTongBasicDB")
	local uTongID = tong_box.GetTongID(uPlayerID)
	if uTongID == 0 then
		return false,9401
	end
	if uMoney <= 0 then
		return false,9381
	end
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	local player_level = CharacterMediatorDB.GetPlayerLevel(uPlayerID)
	local TongLevel = tong_box.GetTongLevel(uTongID)
	local max_money = GetContributeMoneyForExp(player_level, TongLevel)
	local sum_money_ret = StmtOperater._GetTotalContributeMoneyforExp:ExecStat(uPlayerID)
	local sum_money = sum_money_ret(1,1) or 0
	if sum_money >= max_money then
		return false,9378
	end
	if sum_money + uMoney > max_money then
		return false,9379
	end
	
	local MoneyManagerDB=	RequireDbBox("MoneyMgrDB")
	local fun_info = g_MoneyMgr:GetFuncInfo("Tong")
  local bFlag,uMsgID = MoneyManagerDB.AddMoney(fun_info["FunName"],fun_info["Contribut"],uPlayerID, -uMoney,nil,event_type_tbl["玩家捐钱换经验"])
	if not bFlag then
		if IsNumber(uMsgID) then
			CancelTran()
			return false,uMsgID
		end
		return false,9331
	end
	
	local data = {}
  data["uTongID"] = uTongID
	data["uMoney"] 	= uMoney
	data["nEventType"] = event_type_tbl["玩家捐钱换经验"]
  local bRet = tong_box.UpdateTongMoney(data)
	if not bRet then
		CancelTran() 
		return false,9377
	end
	
	if sum_money_ret(1,1) == nil then
		StmtOperater._AddContributeMoneyforExp:ExecStat(uPlayerID,uMoney)
	else
		StmtOperater._UpdateContributeMoneyforExp:ExecStat(uMoney,uPlayerID)
	end
	if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
		CancelTran() 
  	return false,9377
  end
  local uAddExpValue = uMoney
	local AddExpTbl	= {}
	AddExpTbl["char_id"]			= uPlayerID
	AddExpTbl["addExp"]				= uAddExpValue
	AddExpTbl["uEventId"]			= event_type_tbl["玩家捐钱换经验"]
	local RoleQuestDB = RequireDbBox("RoleQuestDB")
	local uCurLevel, uCurExp = RoleQuestDB.AddExp(AddExpTbl)
	if not uCurLevel or not uCurExp then
		--这里已经扣钱了,不要CancelTran,这是正常的需求,给帮会捐钱,但无经验而已
		return true,9393
	end
	return true, 9386, uCurLevel, uCurExp, uAddExpValue, max_money, sum_money + uMoney
end

function CTongContriMoney.ContributeMoneyChangeSoul(data)
	local uPlayerID = data["uPlayerID"]
	local uMoney = data["uMoney"]
	local tong_box = RequireDbBox("GasTongBasicDB")
	local uTongID = tong_box.GetTongID(uPlayerID)
	if uTongID == 0 then
		return false,9401
	end
	if uMoney <= 0 then
		return false,9381
	end
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	local player_level = CharacterMediatorDB.GetPlayerLevel(uPlayerID)
	local TongLevel = tong_box.GetTongLevel(uTongID)
	local max_money = GetContributeMoneyForSoul(player_level,TongLevel)
	
	local sum_money_ret = StmtOperater._GetTotalContributeMoneyforSoul:ExecStat(uPlayerID)
	local sum_money = sum_money_ret(1,1) or 0
	if sum_money >= max_money then
		return false,9382
	end
	if sum_money + uMoney > max_money then
		return false,9383
	end
	
	local MoneyManagerDB=	RequireDbBox("MoneyMgrDB")
	local fun_info = g_MoneyMgr:GetFuncInfo("Tong")
  local bFlag,uMsgID = MoneyManagerDB.AddMoney(fun_info["FunName"],fun_info["Contribut"],uPlayerID, -uMoney,nil,event_type_tbl["玩家捐钱换经验"])
	if not bFlag then
		if IsNumber(uMsgID) then
			CancelTran()
			return false,uMsgID
		end
		return false,9331
	end
	
	local data = {}
  data["uTongID"] = uTongID
	data["uMoney"] 	= uMoney
	data["nEventType"] = event_type_tbl["玩家捐钱换魂值"]
  local bRet = tong_box.UpdateTongMoney(data)
	if not bRet then
		CancelTran() 
		return false,9377
	end
	if sum_money_ret(1,1) == nil then
		StmtOperater._AddContributeMoneyforSoul:ExecStat(uPlayerID,uMoney)
	else
		StmtOperater._UpdateContributeMoneyforSoul:ExecStat(uMoney, uPlayerID)
	end
	if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
		CancelTran() 
  	return false,9377
  end
  
  local uAddSoulValue = uMoney*2
	local tblSoul = {}
	tblSoul["PlayerId"]  = uPlayerID
	tblSoul["addSoulType"]  = event_type_tbl["玩家捐钱换魂值"]
	tblSoul["soulCount"] = uAddSoulValue
	local EquipMgr= RequireDbBox("EquipMgrDB")
	local bRet,SoulRet = EquipMgr.ModifyPlayerSoul(tblSoul)
	if not bRet then
		CancelTran() 
		return false,9377
	end
	return true, 9387, SoulRet, uAddSoulValue, max_money, sum_money + uMoney
end

function CTongContriMoney.ContributeMoneyChangeResource(data)
	local uPlayerID = data["uPlayerID"]
	local uMoney = data["uMoney"]
	local tong_box = RequireDbBox("GasTongBasicDB")
	local uTongID = tong_box.GetTongID(uPlayerID)
	if uTongID == 0 then
		return false,9401
	end
	if uMoney <= 0 then
		return false,9381
	end
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	local player_level = CharacterMediatorDB.GetPlayerLevel(uPlayerID)
	local TongLevel = tong_box.GetTongLevel(uTongID)
	local max_money = GetContributeMoneyForResource(player_level,TongLevel)
	
	local sum_money_ret = StmtOperater._GetTotalContributeMoneyforResource:ExecStat(uPlayerID)
	local sum_money = sum_money_ret(1,1) or 0
	if sum_money >= max_money then
		return false,9384
	end
	if sum_money + uMoney > max_money then
		return false,9385
	end
	
	local MoneyManagerDB=	RequireDbBox("MoneyMgrDB")
	local fun_info = g_MoneyMgr:GetFuncInfo("Tong")
  local bFlag,uMsgID = MoneyManagerDB.AddMoney(fun_info["FunName"],fun_info["Contribut"],uPlayerID, -uMoney,nil,event_type_tbl["玩家捐钱换经验"])
	if not bFlag then
		if IsNumber(uMsgID) then
			CancelTran()
			return false,uMsgID
		end
		return false,9331
	end
	
	local data = {}
  data["uTongID"] = uTongID
	data["uMoney"] 	= uMoney
	data["nEventType"] = event_type_tbl["玩家捐钱换经验"]
  local bRet = tong_box.UpdateTongMoney(data)
	if not bRet then
		CancelTran() 
		return false,9377
	end
	if sum_money_ret(1,1) == nil then
		StmtOperater._AddContributeMoneyforResource:ExecStat(uPlayerID,uMoney)
	else
		StmtOperater._UpdateContributeMoneyforResource:ExecStat(uMoney,uPlayerID)
	end
	if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
		CancelTran() 
  	return false,9377
  end
	
	local uAddResValue = uMoney*33/10000
	local tong_box = RequireDbBox("GasTongBasicDB")
	local upper_res = tong_box.GetUpperLimitResource(uTongID)
	local now_res = tong_box.GetTongResource(uTongID)
	if now_res + uAddResValue > upper_res then
		CancelTran() 
		return false,9391
	end

	local build_box = RequireDbBox("GasTongbuildingDB")
	local bRet = build_box.UpdateTongResource(uAddResValue,uTongID,event_type_tbl["玩家捐钱换军资"])
	if not bRet then
		CancelTran()
		return false,9377
	end
	return true, 9388, uAddResValue, max_money, sum_money + uMoney
end

function CTongContriMoney.GetTongContributeMoneyInfo(parameters)
	local eIndex = parameters["eIndex"]
	local uPlayerID = parameters["uPlayerID"]
	local tong_box = RequireDbBox("GasTongBasicDB")
	local uTongID = tong_box.GetTongID(uPlayerID)
	if uTongID == 0 then
		return 0,0
	end
	if eIndex == 1 then
		return CTongContriMoney.GetMoneyCanContributeforExp(parameters,uTongID)
	elseif eIndex == 2 then
		return CTongContriMoney.GetMoneyCanContributeforSoul(parameters,uTongID)
	elseif eIndex == 3 then
		return CTongContriMoney.GetMoneyCanContributeforResource(parameters,uTongID)
	end
end

function CTongContriMoney.GetMoneyCanContributeforExp(parameters,uTongID)
	local uPlayerID = parameters["uPlayerID"]
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	local player_level = CharacterMediatorDB.GetPlayerLevel(uPlayerID)
	local tong_box = RequireDbBox("GasTongBasicDB")
	local TongLevel = tong_box.GetTongLevel(uTongID)
	local max_money = GetContributeMoneyForExp(player_level,TongLevel)
	local sum_money_ret = StmtOperater._GetTotalContributeMoneyforExp:ExecStat(uPlayerID)
	local sum_money = sum_money_ret(1,1) or 0
	return max_money,sum_money
end

function CTongContriMoney.GetMoneyCanContributeforSoul(parameters,uTongID)
	local uPlayerID = parameters["uPlayerID"]
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	local player_level = CharacterMediatorDB.GetPlayerLevel(uPlayerID)
	local tong_box = RequireDbBox("GasTongBasicDB")
	local TongLevel = tong_box.GetTongLevel(uTongID)
	local max_money = GetContributeMoneyForSoul(player_level,TongLevel)
	local sum_money_ret = StmtOperater._GetTotalContributeMoneyforSoul:ExecStat(uPlayerID)
	local sum_money = sum_money_ret(1,1) or 0
	return max_money,sum_money
end

function CTongContriMoney.GetMoneyCanContributeforResource(parameters,uTongID)
	local uPlayerID = parameters["uPlayerID"]
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	local player_level = CharacterMediatorDB.GetPlayerLevel(uPlayerID)
	local tong_box = RequireDbBox("GasTongBasicDB")
	local TongLevel = tong_box.GetTongLevel(uTongID)
	local max_money = GetContributeMoneyForResource(player_level,TongLevel)
	local sum_money_ret = StmtOperater._GetTotalContributeMoneyforResource:ExecStat(uPlayerID)
	local sum_money = sum_money_ret(1,1) or 0
	return max_money,sum_money
end

----------------------------------------------------
return CTongContriMoney